# Driver Introspective Proxy - API Reference

## Overview
DIP cung cấp một tập hợp API cho việc:
- Quan sát (observe) hành vi driver.
- Suy luận (inference) từ dữ liệu.
- Giao tiếp (comm) với các module bên ngoài.
- Quản lý rollback & recovery.

Tất cả API được thiết kế **lightweight**, **có khả năng mở rộng**, và **không intrusive**.

---

## Namespaces

### 1. Observation API
Các hàm dùng để hook và theo dõi môi trường driver.

```c
// Khởi tạo lớp quan sát
int observe_init();

// Hook vào kernel events
int observe_kernel_event(const char *event_name, void (*callback)(void*));

// Ghi log từ observation layer
int observe_log(const char *message, int severity);

