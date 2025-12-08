## Thư mục `comm` (Communication) — Kernel

Tài liệu này mô tả thiết kế, giao thức, API và ví dụ liên quan đến thành phần giao tiếp (communication) giữa kernel và userland cho dự án `DriverInstropectiveProxy`.

Mục tiêu:
- Định nghĩa giao thức trao đổi thông điệp giữa userland và driver kernel.
- Cung cấp cấu trúc dữ liệu, mã lỗi, và ví dụ minh họa (Windows/Linux) để dễ triển khai.
- Nêu rõ vấn đề bảo mật, đồng bộ hoá và kiểm thử.

**Phạm vi:**
- Tài liệu tập trung vào giao tiếp qua character device / device object (ví dụ: Windows DeviceIoControl hoặc Linux char device + ioctl/read/write).

---

**Nội dung chính:**
- Tổng quan kiến trúc
- Giao thức thông điệp (message format)
- API / IOCTL codes
- C struct mẫu và header `comm.h`
- Ví dụ code: userland + kernel snippets
- Bảo mật, đồng bộ hoá, phiên bản
- Kiểm thử và checklist tích hợp

---

**1. Tổng quan kiến trúc**

Thiết kế đơn giản: driver cung cấp một interface IOCTL (Windows) hoặc ioctl/read/write (Linux) cho userland để gửi/nhận thông điệp. Mỗi thông điệp có header cố định (phiên bản, loại, chiều dài) và payload (nội dung thực tế).

Thiết bị giả định: một character device `/dev/driver_inspect` (Linux) hoặc `\\.\\DriverInspect` (Windows symbolic link) mà userland mở và giao tiếp.

Yêu cầu: xác thực quyền truy cập, tránh race condition, giới hạn kích thước payload và kiểm tra version/compatibility.

---

**2. Giao thức thông điệp**

Mỗi message gồm 2 phần chính:
- Header (cố định) — metadata
- Payload (biến đổi) — dữ liệu cụ thể cho loại message

Header (phiên bản, kiểu, id, chiều dài, flags) được đóng gói theo kiểu nhị phân để xử lý nhanh ở kernel.

Định nghĩa (C):

```c
// comm.h - Message header and types
#ifndef COMM_H
#define COMM_H

#include <stdint.h>

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif

typedef enum {
	COMM_CMD_NOP = 0,
	COMM_CMD_PING = 1,
	COMM_CMD_GET_STATUS = 2,
	COMM_CMD_CAPTURE_PACKET = 3,
	COMM_CMD_CFG_UPDATE = 4,
	COMM_EVT_LOG = 0x80, // events from kernel -> user
} comm_cmd_t;

typedef struct {
	uint16_t version;    // protocol version
	uint16_t cmd;        // comm_cmd_t
	uint32_t seq;        // sequence number
	uint32_t payload_len;// payload length in bytes
	uint32_t flags;      // bit flags (future use)
} comm_hdr_t;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#endif // COMM_H
```

Ghi chú:
- `version`: tăng khi thay đổi không tương thích.
- `seq`: dùng để ghép cặp request/response và phát hiện duplicate.

Payload: tuỳ cmd có thể là JSON nhỏ, binary blob, hoặc struct đã định nghĩa. Nếu payload là JSON, payload_len phải là số byte UTF-8.

---

**3. IOCTL / Command codes**

Windows (ví dụ): sử dụng `DeviceIoControl` với code chung `IOCTL_COMM_EXCHANGE` và truyền buffer chứa `comm_hdr_t` + payload.

Trên Linux: triển khai ioctl commands hoặc read/write. Ví dụ ioctl số: `#define IOCTL_COMM_EXCHANGE _IOWR('c', 1, struct comm_ioctl_buf)`

Ví dụ Windows IOCTL definition (header C):

```c
// ctl_codes.h
#include <winioctl.h>

#define COMM_DEVICE_TYPE 0x8000
#define IOCTL_COMM_EXCHANGE CTL_CODE(COMM_DEVICE_TYPE, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
```

Triển khai kernel:
- Nhận buffer từ userland
- Kiểm tra `comm_hdr_t` (version, payload_len <= MAX)
- Xử lý theo `cmd`
- Trả kết quả (response header + response payload)

---

**4. Mẫu header `comm.h` (đầy đủ hơn)**

```c
// comm.h - common header for userland and kernel
#ifndef COMM_H_FULL
#define COMM_H_FULL

#include <stdint.h>

#define COMM_PROTO_VERSION 1
#define COMM_MAX_PAYLOAD (64*1024) // 64KB limit

typedef enum {
	COMM_OK = 0,
	COMM_ERR_INVALID = -1,
	COMM_ERR_NOSUPPORT = -2,
	COMM_ERR_TOOBIG = -3,
	COMM_ERR_AUTH = -4,
	COMM_ERR_INTERNAL = -5,
} comm_status_t;

typedef struct {
	uint16_t version;
	uint16_t cmd;
	uint32_t seq;
	uint32_t payload_len;
	uint32_t flags;
} comm_hdr_t;

// response wrapper
typedef struct {
	comm_hdr_t hdr;
	int32_t status; // comm_status_t
	// followed by payload data (payload_len bytes)
} comm_resp_t;

#endif // COMM_H_FULL
```

---

**5. Ví dụ code: Userland (Windows) — gửi PING**

```c
// user_ping.c (Windows)
#include <windows.h>
#include <stdio.h>
#include "comm.h"
#include "ctl_codes.h"

int main(void) {
	HANDLE h = CreateFileA("\\\\.\\DriverInspect",
						   GENERIC_READ | GENERIC_WRITE,
						   0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE) {
		printf("Open device failed: %lu\n", GetLastError());
		return 1;
	}

	uint8_t buf[1024];
	comm_hdr_t *hdr = (comm_hdr_t*)buf;
	hdr->version = COMM_PROTO_VERSION;
	hdr->cmd = COMM_CMD_PING;
	hdr->seq = 1;
	hdr->payload_len = 0;
	hdr->flags = 0;

	DWORD bytesOut = 0;
	if (!DeviceIoControl(h, IOCTL_COMM_EXCHANGE, buf, sizeof(comm_hdr_t), buf, sizeof(buf), &bytesOut, NULL)) {
		printf("DeviceIoControl failed: %lu\n", GetLastError());
		CloseHandle(h);
		return 1;
	}

	comm_resp_t *resp = (comm_resp_t*)buf;
	printf("Resp status=%d seq=%u payload_len=%u\n", resp->status, resp->hdr.seq, resp->hdr.payload_len);
	CloseHandle(h);
	return 0;
}
```

Kernel-side (pseudocode) xử lý PING đơn giản trả COMM_OK.

---

**6. Ví dụ Linux: char device + ioctl**

Userland (POSIX) gửi buffer gồm header + payload:

```c
// user_ping_linux.c
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include "comm.h"

#define DEV_PATH "/dev/driver_inspect"
#define IOCTL_COMM_EXCHANGE _IOWR('c', 1, comm_hdr_t)

int main(){
	int fd = open(DEV_PATH, O_RDWR);
	if (fd < 0){ perror("open"); return 1; }

	comm_hdr_t hdr = { .version=COMM_PROTO_VERSION, .cmd=COMM_CMD_PING, .seq=1, .payload_len=0, .flags=0 };
	comm_hdr_t resp;
	if (ioctl(fd, IOCTL_COMM_EXCHANGE, &hdr) < 0){ perror("ioctl"); close(fd); return 1; }
	// kernel can write response into same struct or use read
	printf("Ping sent\n");
	close(fd);
	return 0;
}
```

---

**7. Đồng bộ hoá & Buffering**

- Sử dụng sequence numbers (`seq`) để ghép request/response.
- Hạn chế `payload_len` tối đa (ví dụ 64KB). Nếu cần dữ liệu lớn hơn, dùng shared memory / mmap + handshake để truyền.
- Tránh blocking dài trong kernel: nếu xử lý dài, trả ngay một mã chấp nhận và xử lý bất đồng bộ với notification (event/overlapped/async).

---

**8. Bảo mật**

- Kiểm tra quyền truy cập trước khi thực hiện thao tác nguy hiểm (Windows: kiểm tra token, ACL; Linux: kiểm tra uid/gid và capability).
- Kiểm tra `payload_len` để tránh buffer overflow.
- Thực hiện hardening input validation: không tin payload là JSON hợp lệ, kiểm soát kích thước chuỗi.

---

**9. Kiểm thử**

- Unit test cho parsing header: những case non-sense, version mismatch, too-large payload.
- Integration: chạy userland sample, driver trả đúng response.
- Fuzzing: feed random header/payload và đảm bảo kernel không crash.

Checklist:
- [ ] Document `comm.h` vào repo
- [ ] Thêm sample userland Windows
- [ ] Thêm sample userland Linux
- [ ] Bảo đảm giới hạn payload được enforce
- [ ] Thực hiện unit tests cho parser

---

**10. Phiên bản & tương thích**

- Khi thay đổi header backward-incompatible: tăng `version` và đảm bảo driver từ chối client cũ bằng `COMM_ERR_NOSUPPORT`.

---

Kết luận

File `comm.md` này cung cấp blueprint để triển khai module giao tiếp kernel↔userland: định nghĩa header, ví dụ mã, các lưu ý về bảo mật và kiểm thử. Nếu bạn muốn, tôi có thể:

- Tạo các file `comm.h`, `ctl_codes.h` vào thư mục tương ứng.
- Thêm ví dụ thực tế dưới dạng file C trong repo và chạy build (Windows hoặc MinGW/Cygwin cho Linux-like testing).

Bạn muốn mình tiếp tục tạo mã nguồn mẫu (các file `.c` / `.h`) trong repo không? Nếu có, chọn mục tiêu: `windows` hoặc `linux` hoặc cả hai.

