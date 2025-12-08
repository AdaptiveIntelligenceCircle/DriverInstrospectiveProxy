## `hook` — Triết lý và Thiết kế cho Driver Introspective Proxy

File này mô tả triết lý, nguyên tắc thiết kế và kiến trúc cao cấp cho thành phần "hook" trong dự án Driver Introspective Proxy (DIP). Mục tiêu là truyền đạt lý do tồn tại, các ràng buộc an toàn, và một lộ trình triển khai nhẹ, có thể kiểm chứng.

---

**Tổng quan triết lý (Philosophy)**

- **Introspection trước hết, can thiệp tối thiểu:** DIP được thiết kế để quan sát và thu thập thông tin về hành vi hệ thống/driver, trước khi thực hiện bất kỳ thay đổi nào. Các hook phải ưu tiên thu thập, ghi nhận, và báo cáo hơn là thay đổi luồng thực thi.
- **Proxy có trách nhiệm:** Khi cần can thiệp (proxying), hành vi phải rõ ràng, có thể tắt/mở động, và luôn có cơ chế hồi phục (fail-safe) để tránh làm sập hệ thống.
- **Bảo mật và quyền hạn:** Mọi hook phải chạy với nguyên tắc quyền tối thiểu: chỉ yêu cầu các capability/privilege cần thiết, có kiểm soát truy cập từ userland, và audit đầy đủ cho mọi thao tác cảm nhạy.
- **Quan sát (observability) và ít xâm lấn (non-invasive):** Thiết kế cho phép thu thập metadata (timestamps, call stack hash, context IDs) mà không tăng nhiều độ trễ cho luồng chính.
- **Hiệu suất:** Hook phải nhẹ, tránh cấu trúc dữ liệu nặng trong hot-path; dùng batching, lock-free hoặc lock hiếm gặp, và nếu cần xử lý nặng thì đẩy ra worker thread/DEFERRED procedure.

---

**Nguyên tắc thiết kế**

- **Phân lớp rõ ràng:** tách thành (1) registration/management, (2) fast-path capture, (3) async processing/telemetry, (4) policy/enforcement.
- **Fail-safe default:** nếu hook hoặc submodule lỗi, trả về trạng thái không can thiệp (pass-through) và log lỗi, không làm thay đổi hành vi ban đầu.
- **Traceable IDs:** mọi hook invocation phải gắn `trace_id`/`seq` để ghép cặp giữa userland và kernel (tham chiếu đến `comm` module).
- **Boundary contract:** định nghĩa rõ interface giữa kernel hook và userland controller — message formats, timeouts, and max payloads (xem `comm/comm.md`).
- **Configurability:** hỗ trợ bật/tắt hook theo scope (global/process/thread) và theo loại (io, net, file, driver-specific).

---

**Hook Architecture (cao cấp)**

1. Registration Layer
	- API nội bộ để driver/module khác đăng ký hook point (symbolic name, priority, flags).
	- Ghi nhận metadata: owner, description, allowed contexts.

2. Fast-path Capture
	- Mã chạy in-line hoặc gần luồng chính, chỉ nhân bản/bắt metadata và minimal snapshot (pointer, size, timestamp).
	- Không alloate heap lớn; dùng per-cpu ring buffer hoặc lock-free queue để đẩy event sang worker.

3. Async Processing / Proxy Worker
	- Worker xử lý event: parsing, enrich (resolve pid, stacktrace nếu có), policy check, gửi tới userland thông qua `comm` (shared buffer hoặc IOCTL).
	- Nếu proxy needed, worker thực hiện transformation hoặc quyết định block/pass/modify.

4. Policy & Enforcement
	- Policy engine nhỏ nằm trong kernel chỉ cho quyết định nhanh; luật phức tạp đưa sang userland để nhận phê duyệt (optional).

5. Observability
	- Emit structured events (binary + optional JSON) sang `comm` để userland thu thập, hiển thị, hoặc lưu.

---

**API / Pseudocode**

// Registration API (C-like pseudocode)

```c
// register a hook point
int hook_register(const char *name, hook_callback_t cb, void *ctx, uint32_t flags);

// unregister
int hook_unregister(int hook_id);

// callback signature (fast-path): must be minimal and bounded
typedef enum { HOOK_CONTINUE=0, HOOK_BLOCK=1, HOOK_MODIFIED=2 } hook_action_t;
typedef hook_action_t (*hook_callback_t)(const hook_event_t *evt, void *ctx);

// event minimal struct
typedef struct {
	 uint64_t seq;
	 uint32_t pid;
	 uint32_t tid;
	 uint64_t ts_nsec;
	 void *ptr;
	 uint32_t len;
	 uint32_t flags;
} hook_event_t;
```

Ghi chú:
- `hook_callback_t` trên fast-path chỉ nên đọc `evt` và trả kết quả nhanh; không gọi blocking APIs.
- Nếu cần thao tác lâu, callback có thể queue một job cho worker và trả `HOOK_CONTINUE` để cho luồng chính tiếp tục.

---

**Proxying example (safe pattern)**

- Fast-path captures request metadata và đặt job vào worker queue.
- Worker thực hiện kiểm tra, có thể gửi một request sync/async tới userland (qua `comm`) để quyết định. Nếu userland cần phản hồi đồng bộ, worker có thể wait với timeout ngắn; nếu timeout -> default decision.

Pseudo-flow:
1. Fast-path: capture -> enqueue -> return PASS
2. Worker: dequeue -> send to userland -> wait (timeout configurable)
3a. If userland returns BLOCK -> worker applies enforcement (e.g., drop packet)
3b. Else PASS or MODIFY -> apply or ignore

---

**Bảo mật & Safety**

- Kiểm soát quyền: chỉ các tiến trình có quyền (token/uid/capability) mới có thể đăng ký hook control hoặc đọc events nhạy cảm.
- Rate limiting events để tránh DoS từ noisy hook points.
- Validate size/offsets trước khi dereference pointers từ user/kernel shared buffers.
- Use privilege separation: heavy analysis in userland; kernel limited to minimal decisions.

---

**Testing & Validation**

- Unit tests: parsing, registration life-cycle, error paths.
- Integration: enable hook for controlled scenario (e.g., synthetic syscall) and assert events observed.
- Fuzzing: send malformed events/headers to ensure driver doesn't crash (collaborate with `comm` fuzz harness).
- Performance: measure added latency in hot-path; tune ring buffer sizes and batch flush.

---

**Operational guidance**

- Default state: hooks disabled. Operator enables explicit points by policy.
- Logging: minimal in hot path; full logs via async processing.
- Upgrade path: persist hook metadata and version; ensure backward-compat checks.

---


