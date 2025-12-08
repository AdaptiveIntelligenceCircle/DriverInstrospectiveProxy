# Kernel — Thiết kế tổng thể cho Driver Introspective Proxy

Tài liệu này mô tả thiết kế tổng thể của subsystem kernel cho dự án **Driver Introspective Proxy (DIP)**. Nó cung cấp cái nhìn toàn cảnh về các module, kiến trúc, interaction, và lộ trình phát triển.

---

## 1. Mục tiêu & Tầm nhìn

**DIP Kernel** là một framework để:
- **Quan sát (Introspect)** hành vi của các driver hệ thống và ứng dụng userland mà không thay đổi hành vi ban đầu.
- **Can thiệp có kiểm soát (Proxy)** vào các luồng quan trọng (I/O, network, memory) để thực thi chính sách bảo mật, giám sát, hoặc chẩn đoán.
- **Giao tiếp hiệu quả (Efficient Communication)** giữa kernel và userland thông qua giao thức rõ ràng, đơn giản.

Nguyên tắc cốt lõi:
- **Introspection first, minimal intervention**: Quan sát trước, can thiệp sau (chỉ khi cần).
- **Security by design**: Quyền tối thiểu, audit đầy đủ, fail-safe defaults.
- **Performance-aware**: Thiết kế cho hot-path hiệu suất cao, xử lý nặng đẩy sang userland.
- **Observability**: Mỗi sự kiện có trace ID, timestamp, context để tracking từ đầu đến cuối.

---

## 2. Kiến trúc module kernel

### 2.1 Sơ đồ tổng thể

```
┌─────────────────────────────────────────────────────────────────┐
│                         USERLAND                                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │  Control Agent / Policy Engine / Telemetry Collector     │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
          ↑                                                  ↓
          │  comm.ioctl / DeviceIoControl / read/write      │
          │  (request/response messages)                     │
          ↓                                                  ↑
┌─────────────────────────────────────────────────────────────────┐
│                      KERNEL SPACE                                │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │  COMM (Communication)                                    │   │
│  │  - Character device interface                            │   │
│  │  - Message marshalling/unmarshalling                     │   │
│  │  - IOCTL handler & request/response buffering            │   │
│  └──────────────────────────────────────────────────────────┘   │
│           ↑                                           ↓           │
│           │ Event queue                Request       │           │
│           │                                          │           │
│  ┌────────┴──────────────┬──────────────────────────┴──────┐   │
│  │                       │                                  │   │
│  │  HOOK                 │         MONITOR                 │   │
│  │  - Registration       │         - System stats          │   │
│  │  - Fast-path capture  │         - Resource tracking     │   │
│  │  - Event queue        │         - Health checks         │   │
│  │  - Callback mgmt      │         - Perf metrics          │   │
│  └───────┬────────────────┴──────────────────────────────────┘   │
│          │                                                        │
│          │  Events / Decisions                                   │
│          ↓                                                        │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │  MESSAGE (Internal protocol for hook↔policy decisions)   │   │
│  │  - Message queue struct                                  │   │
│  │  - Policy decision encoding                             │   │
│  │  - Event serialization                                  │   │
│  └──────────────────────────────────────────────────────────┘   │
│          ↑                                                        │
│          │  Hooks on OS/driver syscalls                         │
│          │                                                        │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │  CORE                                                    │   │
│  │  - Hook registration & lifecycle                        │   │
│  │  - Policy engine (optional, light-weight)               │   │
│  │  - Ring buffer management for events                    │   │
│  │  - Integration with OS (Windows/Linux)                  │   │
│  └──────────────────────────────────────────────────────────┘   │
│          ↑                                                        │
│          │  System calls / Driver callbacks                      │
│          ↓                                                        │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │  OS (Windows / Linux)                                    │   │
│  │  - Syscall infrastructure                               │   │
│  │  - Device driver framework                              │   │
│  │  - Interrupt/exception handlers                         │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

---

### 2.2 Module chi tiết

#### **comm** — Communication Layer
Nằm: `Kernel/comm/`

Chức năng:
- Cung cấp character device interface cho userland.
- Xử lý IOCTL (Windows) hoặc ioctl/read/write (Linux).
- Marshalling/unmarshalling messages (header + payload).
- Buffering request/response, timeout handling.

Key files:
- `comm.h`: Struct định nghĩa (`comm_hdr_t`, `comm_cmd_t`).
- `comm.c`: Initialization, device creation, IOCTL handler.
- `ctl_codes.h` (Windows) hoặc `ioctl_defs.h` (Linux): IOCTL codes.

Interface:
```c
typedef int (*comm_handler_t)(const comm_hdr_t *req, void *req_payload, 
                               comm_hdr_t *resp, void *resp_payload);
int comm_register_handler(uint16_t cmd, comm_handler_t handler);
int comm_send_event(uint16_t event_type, const void *data, uint32_t len);
```

---

#### **hook** — Introspection & Hook Management
Nằm: `Kernel/hook/`

Chức năng:
- Quản lý life-cycle hook points (register, unregister, enable, disable).
- Fast-path capture của events (minimal snapshot, ring buffer).
- Async processing worker để enrich events, policy check, send to userland.
- Callback dispatcher.

Key files:
- `hook.h`: Enum/struct (`hook_event_t`, `hook_action_t`, `hook_callback_t`).
- `hook.c`: Registration, event capture, worker management.
- `hook_linux.c` / `hook_windows.c`: Platform-specific hook injection (e.g., syscall interception, driver callback registration).

API:
```c
typedef hook_action_t (*hook_callback_t)(const hook_event_t *evt, void *ctx);
int hook_register(const char *name, hook_callback_t cb, void *ctx, uint32_t flags);
int hook_unregister(int hook_id);
int hook_enqueue_event(const hook_event_t *evt);
```

Ví dụ hook points:
- `hook.syscall.open` — File open syscall
- `hook.syscall.connect` — Network connect syscall
- `hook.driver.ioctl` — Driver IOCTL invocation
- `hook.memory.alloc` — Memory allocation

---

#### **message** — Internal Message Protocol
Nằm: `Kernel/message/`

Chức năng:
- Định nghĩa format cho thông điệp nội bộ giữa hook, policy engine, và comm layer.
- Serialization/deserialization của events.
- Policy decision encoding (ALLOW, DENY, MODIFY, DEFER_TO_USERLAND).

Key files:
- `message.h`: Event struct (`evt_msg_t`), decision struct (`policy_decision_t`).
- `message.c`: Serialization, validation.

---

#### **monitor** — System Monitoring & Telemetry
Nằm: `Kernel/monitor/`

Chức năng:
- Công cụ giám sát hệ thống (CPU, memory, I/O stats).
- Health checks và alerting.
- Performance metrics cho hook latency.
- Optional: trend analysis in-kernel hoặc push to userland.

Key files:
- `monitor.h`: Data struct (`sys_stats_t`).
- `monitor.c`: Collection routines (per-cpu counters, lock-free updates).
- `monitor_timer.c`: Periodic collection (workqueue/timer).

---

#### **core** — Core Management & Policy Engine
Nằm: `Kernel/core/`

Chức năng:
- Khởi tạo/cleanup các module khác.
- Quản lý ring buffer chung cho events.
- Lightweight policy engine (optional, cho quyết định nhanh).
- Versioning & compatibility checks.

Key files:
- `core.h`: Exports initialization, ring buffer APIs.
- `core.c`: Module init/exit, ring buffer alloc/free.
- `policy.c`: Simple policy rules (whitelist/blacklist, rate limits).

---

#### **include** — Headers chung
Nằm: `Kernel/include/`

Chứa:
- `dip.h`: Main header export cho tất cả public APIs.
- `types.h`: Kiểu chung (uint64_t, error codes).
- `config.h`: Build-time configuration.
- `debug.h`: Logging macros.

---

## 3. Data Flow & Interaction

### 3.1 Scenario: Introspection (capture-only, no intervention)

```
1. Hook point (e.g., syscall open) triggers
   ↓
2. hook.c: Fast-path captures (pid, tid, timestamp, pointer snapshot)
   - Enqueue to per-cpu ring buffer
   - Return HOOK_CONTINUE (no intervention)
   ↓
3. Worker thread dequeues events periodically
   - Enrich (resolve PID→process name if needed)
   - Format into message_t
   ↓
4. Send event to comm layer via comm_send_event()
   ↓
5. comm.c: Buffered events sent to userland on next read/IOCTL
   ↓
6. Userland: receives event, updates UI / logs / analysis
```

### 3.2 Scenario: Proxying (with policy decision)

```
1. Hook point triggers
   ↓
2. hook.c: Fast-path captures, enqueue, return HOOK_CONTINUE
   ↓
3. Worker thread processes:
   - Check lightweight policy in kernel (fast path decision)
   - If decision made (e.g., whitelisted) → apply & done
   - If uncertain → query userland
   ↓
4. comm.c: Send COMM_CMD_POLICY_DECISION request to userland
   - Wait (timeout ~100ms configurable) for response
   ↓
5a. Userland responds ALLOW/DENY
    ↓ Worker applies enforcement (e.g., allow syscall to proceed)
    
5b. Timeout or no response
    ↓ Default decision (e.g., ALLOW for safety or DENY for security)
```

### 3.3 Scenario: Monitor polling

```
Periodic (e.g., 1s):
  monitor.c collects CPU, memory, I/O stats
  ↓
  If threshold exceeded, trigger alert event
  ↓
  Send via comm to userland
```

---

## 4. Cross-platform considerations

### Linux
- **Hook mechanism**: kprobes, tracepoints, or eBPF (depending on version).
- **Character device**: /dev/driver_inspect (via misc device or custom).
- **IOCTL**: _IOWR, _IOR, _IOW macros.

### Windows
- **Hook mechanism**: kernel callbacks (registry, file, network), driver callbacks.
- **Device object**: DEVICE_OBJECT + symbolic link `\\.\\DriverInspect`.
- **IOCTL**: CTL_CODE macro, METHOD_BUFFERED, METHOD_OUT_DIRECT.

Common abstraction layer in `core/`:
```c
#ifdef __linux__
#  include "core_linux.h"
#else
#  include "core_windows.h"
#endif
```

---

## 5. Security Design

### 5.1 Access Control
- **Linux**: UID/GID + capability checks (CAP_SYS_ADMIN or custom).
- **Windows**: Token-based (SYSTEM or admin token).
- Device open limited by ACL (Linux: 0600 root-only; Windows: DACL).

### 5.2 Input Validation
- All payloads checked for size, alignment, null-termination.
- Pointer dereference in kernel guarded (copy_from_user / ProbeForRead).

### 5.3 Rate Limiting & DoS protection
- Per-hook event rate limit (configurable).
- Ring buffer overflow → oldest events dropped, counter incremented.
- Timeout on userland requests to prevent kernel blocking.

### 5.4 Audit
- Critical actions logged: hook register, policy decision made, enforce action.
- Sequence numbers for reconstruction.

---

## 6. Performance Considerations

### 6.1 Hot-path latency
- Fast-path hook callback: < 1µs (no heap alloc, no lock contention).
- Ring buffer: per-CPU, lock-free enqueue.
- Worker processing: batched (e.g., 100 events per wakeup).

### 6.2 Buffering
- Per-CPU ring buffer (e.g., 4KB per CPU).
- If full, drop oldest or drop new (configurable).
- Userland reads periodically or on high watermark.

### 6.3 Memory footprint
- Core: ~100KB code + 50KB data + ring buffer (proportional to cores/config).
- Hook metadata: ~64 bytes per registered hook.

---

## 7. Implementation Roadmap

### Phase 1: Core Foundation
- [ ] Implement `core/core.c` (module init/exit, ring buffer).
- [ ] Implement `comm/` (character device, IOCTL handler).
- [ ] Define `message.h` and `hook.h` structs.

### Phase 2: Hook System
- [ ] Implement `hook/hook.c` (registration, fast-path capture).
- [ ] Platform-specific: `hook/hook_linux.c` + `hook/hook_windows.c`.
- [ ] Worker thread + event processing.

### Phase 3: Integration
- [ ] Connect hook events → comm layer (send_event).
- [ ] Implement userland request handlers in comm.
- [ ] Add simple policy rules in `core/policy.c`.

### Phase 4: Monitoring & Polish
- [ ] Implement `monitor/` (stats collection).
- [ ] Add performance telemetry.
- [ ] Documentation + examples.
- [ ] Unit & integration tests.

---

## 8. Key Design Principles (Recap)

| Principle | Implementation |
|-----------|-----------------|
| **Introspection first** | Hooks default to PASS-THROUGH, require explicit policy to BLOCK. |
| **Non-invasive** | Fast-path captures metadata only; processing deferred to worker. |
| **Fail-safe** | If hook/policy fails, return PASS. Errors logged. |
| **Observable** | All events have seq, pid, ts; can trace end-to-end. |
| **Performant** | Per-CPU ring buffer, lock-free enqueue, async worker. |
| **Secure** | ACL on device, input validation, rate limiting, audit. |
| **Scalable** | Per-CPU state, configurable buffer sizes, extensible hook points. |

---

## 9. Testing Strategy

### Unit Tests
- Ring buffer: insert, overflow, read operations.
- Message serialization: encode/decode events.
- Policy rules: match/evaluate conditions.

### Integration Tests
- Enable hook on known syscall, verify events captured.
- Send IOCTL from userland, verify response.
- Policy decision: request → userland → enforcement.

### Stress Tests
- High-frequency hook triggering (e.g., 10k syscalls/sec).
- Multiple processes triggering hooks concurrently.
- Memory pressure (verify no panic, graceful degradation).

### Security Tests
- Unauthorized device open (verify EACCES).
- Oversized payload (verify rejected).
- Invalid policy decision (verify ignored).

---

## 10. File Structure

```
Kernel/
├── Kernel.md                 ← This file
├── comm/
│   ├── comm.md
│   ├── comm_channel.h
│   ├── comm_protocol.c
│   ├── comm_utils.h
│   └── test_comm.c 
├── hook/
│   ├── hook.md
│   ├── driver_hook.h/syscall_hook.c
│   ├── test_hook.c
│   ├── hook_linux.c
│   └── hook_windows.c
|   |__ hook_manager.h/.c
├── message/
│   ├── comm_channel.h/.c
│   └── comm_protocol.c/comm_utils.c
├── monitor/
│   ├── drivers/
│   ├── policy/
│   └── .cpp/.h
├── core/
│   ├── dip_init.c
│   ├── event_dispatch.c
│   ├── logging.c / build_module.bat
│   ├── memory_manager.h
│   └── unload_sip.sh/load_dip.bat
├── include/
│   ├── dip_common.h / dip_event.h
│   ├── hook_manager.h
│   ├── logging.h
│   └── monitor.h
└── tests/
    ├── test_ring_buffer.c
    ├── test_hook_register.c
    ├── test_comm_ioctl.c
    └── test_policy.c
```

---

## 11. Next Steps

1. **Review & Feedback**: Đọc tài liệu này và các chi tiết trong `comm.md`, `hook.md`. Xác nhận scope và design fit với yêu cầu của bạn.

2. **Skeleton Code**: Tạo các file header & implement skeleton (empty functions with comments).

3. **Ring Buffer Prototype**: Implement core ring buffer trước (dùng được cho tất cả modules).

4. **Integration Test**: Viết một scenario đơn giản (e.g., hook on malloc, capture event, read from userland).

---
