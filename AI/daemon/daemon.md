# **Driver Introspective Proxy — Daemon Layer**

**Location:** `daemon/`
**Purpose:** Lớp giám sát nền (background introspective daemon) theo dõi **environment**, **kernel signals**, **hooks**, và cung cấp dữ liệu liên tục cho Behavior Engine, Monitor, và Policy System.

Daemon là “tai – mắt – thần kinh cảm biến” của toàn bộ Driver Introspective Proxy (DIP).

---

# **1. Chức năng tổng quát của Daemon**

Daemon trong DIP đảm nhiệm 4 nhiệm vụ chính:

### **1) Quan sát hệ thống liên tục (Continuous Observation)**

* Theo dõi biến động môi trường
* Theo dõi trạng thái thiết bị
* Theo dõi hoạt động kernel / driver

### **2) Thu thập và chuẩn hóa dữ liệu**

Jitter dữ liệu, spikes, noise từ kernel được convert sang *Introspective Signals*.

### **3) Gửi tín hiệu rủi ro và bất thường cho DIP Core**

* Báo thay đổi môi trường
* Cảnh báo bất thường từ hook
* Gửi event cho Behavior Engine, Policy Guard

### **4) Hoạt động dưới nền, độc lập với Behavior Engine**

Daemon không quyết định hành vi → chỉ **cung cấp dữ liệu nền tảng**.

---

# **2. Cấu trúc thư mục daemon/**

```
daemon/
   ├── observe_env.c
   ├── observe_hook.c
   ├── observe_kernel.c
   ├── observe_manager.c
   ├── observe_manager.h
   └── test_observe.c
```

---

# **3. Giải thích từng file**

---

## **3.1. observe_env.c**

Quan sát các yếu tố môi trường ảnh hưởng đến Driver Proxy, ví dụ:

* nhiệt độ thiết bị
* tải CPU / memory
* trạng thái I/O
* mức độ bất thường sensor system-level
* mapping tới "environment risk score"

**Đầu ra:** `env_signal_t`

**Nhiệm vụ chính:**

* Đọc dữ liệu từ OS APIs / system files
* Chuẩn hóa sang cấu trúc DIP
* Gửi dữ liệu lên `observe_manager`

---

## **3.2. observe_hook.c**

Quan sát các **hooks** mà DIP đặt vào những vị trí quan trọng trong hệ thống:

* file system hooks
* netlink hooks
* syscall hooks
* driver hooks
* IPC hooks

**Đầu ra:** `hook_event_t`

**Nhiệm vụ:**

* Ghi nhận sự kiện từ các điểm hook
* Phát hiện bất thường như:

  * unauthorized access
  * unexpected sequence
  * privilege escalation pattern
* Báo lên manager dưới dạng "hook-introspective event"

---

## **3.3. observe_kernel.c**

Theo dõi các tín hiệu kernel-level:

* kernel warnings
* crash signals
* module load/unload
* driver state change
* kernel ring buffer
* low-level hardware fault patterns

**Đầu ra:** `kernel_signal_t`

**Nhiệm vụ:**

* Kéo dữ liệu từ kernel (kmsg, sysfs, procfs hoặc custom pipe)
* Chuyển về dạng phù hợp với DIP
* Đánh tag mức độ rủi ro kernel-level (0 → 1)

---

## **3.4. observe_manager.c / observe_manager.h**

**Trái tim của daemon layer.**

Manager đóng vai trò:

### **1) Điều phối (Coordinator)**

* Nhận signal từ env, hook, kernel
* Gom, chuẩn hóa, merge thành `introspective_packet`

### **2) Kiểm soát vòng đời daemon**

* init
* run
* stop
* restart

### **3) Buffer và pipeline dữ liệu**

* xử lý queue
* loại bỏ trùng lặp
* debounce và smoothing

### **4) Xuất dữ liệu sang DIP Core**

Dữ liệu được gửi đến:

* behavior/introspective_risk_monitor
* monitor engine
* dip kernel interface
* logging subsystem

### **5) Kiểm tra consistency**

Nếu:

* kernel_signal xung đột với env_signal → đánh cảnh báo
* hook_event sai logic → raise anomaly

---

## **3.5. test_observe.c**

File test đơn giản để:

* kiểm thử env observer
* kiểm thử hook observer
* kiểm thử kernel observer
* kiểm thử khả năng merge và dispatch

Dùng để chạy manual check:

```
./test_observe
```

Nó đảm bảo daemon hoạt động đúng *trước khi* được tích hợp với toàn bộ DIP runtime.

---

# **4. Pipeline hoạt động của Daemon**

```
observe_env      →  
                   \
observe_hook      →   observe_manager → DIP message bus → Behavior/Policy
                   /
observe_kernel   →
```

Manager đóng vai trò hợp nhất (“introspective aggregator”).

---

# **5. Loại tín hiệu daemon gửi lên DIP**

Daemon xuất ra **Introspective Signals**, gồm:

### **1) ENV_SIGNAL**

* environment drift
* overheating
* resource starvation
* thermal/CPU anomalies

### **2) HOOK_EVENT**

* unexpected syscalls
* abnormal file interactions
* unauthorized access
* high-risk sequences

### **3) KERNEL_SIGNAL**

* kernel warning
* module inconsistency
* device unstable state
* low-level hardware issues

Tất cả được hợp nhất thành:

```
introspective_packet_t
```

để Behavior Engine hoặc Policy Guard xử lý.

---

# **6. Mục tiêu thiết kế của daemon**

1. **Tối giản, không phụ thuộc nặng**
2. **An toàn – không can thiệp hành vi**
3. **Luôn chạy nền và ổn định**
4. **Không làm chậm hệ thống**
5. **Đảm bảo DIP luôn có dữ liệu thời gian thực**

---

# **7. Ngắn gọn nhất**

> **Daemon của DIP là lớp giám sát nền, thu thập tín hiệu từ environment, hook, và kernel.
> Nó hợp nhất dữ liệu thành introspective packets và gửi lên Behavior Engine, Monitor hoặc Policy Guard.
> Daemon không quyết định — nó chỉ quan sát, chuẩn hóa và cung cấp sự thật.**

---


