Dưới đây là **models.md** — tài liệu chuẩn, rõ ràng, súc tích, mô tả đầy đủ chức năng của folder **models/** trong Driver Introspective Proxy (DIP).
Bạn có thể đưa trực tiếp vào repo: `models/models.md`.

---

# **Driver Introspective Proxy — Models Layer**

**Location:** `models/`
**Purpose:** Chứa toàn bộ **mô hình dữ liệu cốt lõi (core data models)** được sử dụng trong tất cả các thành phần của Driver Introspective Proxy.
Models đóng vai trò là “ngôn ngữ chung” giúp các module giao tiếp với nhau một cách ổn định, rõ ràng và type-safe.

---

# **1. Vì sao Models quan trọng?**

Trong DIP, mọi pipeline — daemon → inference → behavior → monitor → kernel — đều dựa trên **các cấu trúc dữ liệu nhất quán**.

Models đảm bảo:

### 1. **Consistency**

Tất cả các module dùng chung data contract → tránh mismatch.

### 2. **Predictability**

Các struct cố định→ dễ kiểm tra, dễ kiểm soát, dễ trace.

### 3. **Safety & Stability**

Không module nào được phép tự định nghĩa dữ liệu tùy tiện → giảm rủi ro logic.

### 4. **Auditable**

Model là nơi định nghĩa schema → phù hợp với auditing & reproduction.

Models chính là “schema layer” của Driver Introspective Proxy.

---

# **2. Nội dung thư mục models/**

Tùy thiết kế. Lấy ví dụ (có thể thay đổi) folder models thường bao gồm các file:

```
models/
   ├── env_signal.h
   ├── hook_event.h
   ├── kernel_signal.h
   ├── inference_input.h
   ├── inference_result.h
   ├── behavior_hint.h
   ├── introspective_packet.h
   ├── risk_profile.h
   ├── driver_state.h
   ├── metadata.h
   └── models.md
```

---

# **3. Các nhóm mô hình chính**

---

## **3.1. Environmental Models**

**File:** `env_signal.h`

Định nghĩa dữ liệu mô tả trạng thái môi trường:

* CPU load
* memory pressure
* device temperature
* I/O pressure
* environment risk score
* stability indicator

**Đầu vào:** từ daemon/observe_env
**Đầu ra:** inference engine

---

## **3.2. Hook Event Models**

**File:** `hook_event.h`

Ghi nhận sự kiện từ các hook:

* syscall sequences
* unauthorized access
* write/exec attempts
* unexpected behavior pattern

Trở thành một phần của **introspective_packet_t**.

---

## **3.3. Kernel Models**

**File:** `kernel_signal.h`

Mô tả:

* kernel warnings
* unstable driver states
* module issues
* low-level faults
* kernel drift score

---

## **3.4. Inference Input Models**

**File:** `inference_input.h`

Chứa dữ liệu **đã làm sạch** từ daemon:

```
typedef struct {
    env_signal_t env;
    kernel_signal_t kernel;
    hook_event_t hook;
    driver_state_t driver;
}
inference_input_t;
```

Dùng trong `inference_runtime.c`.

---

## **3.5. Inference Output Models**

**File:** `inference_result.h`

Gồm:

* risk_score
* anomaly_score
* kernel_health
* sensor_confidence
* recommended behavior
* meta info (timestamps, id…)

Được truyền cho Behavior Engine.

---

## **3.6. Behavior Models**

**File:** `behavior_hint.h`

Không phải quyết định → chỉ là gợi ý từ inference:

* SAFE_MODE
* SLOW_DOWN
* FALLBACK
* MAINTAIN
* REEVALUATE

Behavior Engine dựa vào hint + state + policy để quyết định.

---

## **3.7. Introspective Models**

**File:** `introspective_packet.h`

Gộp tất cả dữ liệu introspection vào một packet:

```
introspective_packet = {
    env_signal,
    kernel_signal,
    hook_event,
    risk_profile,
    meta
}
```

Được daemon → inference → behavior sử dụng liên tục.

---

## **3.8. Risk Profile Models**

**File:** `risk_profile.h`

Mô tả rủi ro tổng hợp:

* short-term risk
* long-term risk
* kernel vs environment weighting
* risk trend

Rất quan trọng trong mô hình Adaptive Behavior.

---

## **3.9. Driver State Models**

**File:** `driver_state.h`

Gồm:

* tốc độ
* góc xoay
* trạng thái phanh
* steering wheel
* vehicle mode
* flags (override, safe-mode, degraded…)

Được daemon + inference runtime sử dụng.

---

## **3.10. Metadata Models**

**File:** `metadata.h`

Lưu trữ metadata gắn với:

* phiên inference
* phiên behavior
* timestamp
* version hash
* flags của DIP kernel

Phục vụ audit & replay.

---

# **4. Dòng dữ liệu qua Models**

```
Daemon → env_signal / hook_event / kernel_signal
    ↓
models → inference_input
    ↓
Inference Runtime → inference_result
    ↓
models → behavior_hint
    ↓
Behavior Engine → hành vi cuối cùng
```

Models là lớp kết nối của toàn bộ hệ thống.

---

# **5. Nguyên tắc thiết kế Models**

1. **Không phụ thuộc vào module khác**
   → modules phụ thuộc models, không phải ngược lại.

2. **Tối giản**
   struct phải nhỏ, không chứa logic thừa.

3. **Không cấp phát động nếu không cần thiết**
   → giảm chi phí runtime.

4. **Immutable khi có thể**
   → đảm bảo inference reproducibility.

5. **Ổn định theo version**
   → thay đổi phải backward-compatible hoặc versioned.

---

# **6. Ngắn gọn nhất**

> **models/** lưu trữ toàn bộ định nghĩa dữ liệu cốt lõi.
> Đây là nền tảng để daemon, inference, behavior, policy và monitor giao tiếp với nhau một cách an toàn, ổn định, minh bạch và có thể audit.

---

