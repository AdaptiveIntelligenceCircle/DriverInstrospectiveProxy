# **Driver Introspective Proxy — Inference Layer**

**Location:** `inference/`
**Purpose:** Thực thi *Inference Pipeline* — xử lý dữ liệu đầu vào (từ daemon + kernel + policy + user events), suy luận hành vi, chuẩn hóa kết quả và cung cấp thông tin cho Behavior Engine hoặc DIP Core.

Inference Layer là “bộ não tính toán” (computational brain) trước khi hành vi được kích hoạt.

---

# **1. Mục tiêu của Inference Layer**

1. **Nhận dữ liệu đa nguồn** (driver, kernel, daemon, sensors, DIP messages)
2. **Biến đổi/chuẩn hóa** thành cấu trúc inference-friendly
3. **Chạy logic suy luận** (rule-based, statistical hoặc signature-based)
4. **Kết hợp state + policy** để tạo **inference_result**
5. **Gửi kết quả sang Behavior Engine hoặc Monitor**
6. **Duy trì một runtime inference state** nhất quán và an toàn

Inference không quyết định hành vi —
**nó cung cấp “đánh giá” để Behavior Engine quyết định.**

---

# **2. Cấu trúc thư mục inference/**

```
inference/
   ├── inference_behavior.c
   ├── comm.c
   ├── data.c
   ├── inference_manager.c
   ├── inference_runtime.c
   └── inference.md
```

---

# **3. Mô tả từng file**

---

## **3.1. inference_behavior.c**

**Purpose:** Ánh xạ giữa kết quả inference → hành vi tiềm năng.

Chứa logic:

* kiểm tra pattern bất thường
* phân loại mức rủi ro inference
* gửi tín hiệu gợi ý (behavior hints) cho Behavior Engine

Ví dụ chức năng:

* nếu inference_result.speed_risk > 0.8 → gợi ý “slowdown”
* nếu inference_result.kernel_signal == UNSTABLE → gợi ý “fallback”
* nếu inference_result.sensor_confidence thấp → chuyển chế độ SAFE_MODE

Chức năng này mô tả **"cách inference ảnh hưởng đến hành vi"**, nhưng **không quyết định trực tiếp**.

**Đầu vào:** inference_result
**Đầu ra:** behavior_hint_t

---

## **3.2. comm.c**

**Purpose:** Giao tiếp giữa inference layer và các lớp khác.

Bao gồm:

* receive() và send() API dùng để trao đổi dữ liệu với:

  * daemon
  * DIP core
  * behavior engine
  * sensor stream
* chuyển đổi dữ liệu vào/ra thành *inference_packet_t*

Nó định nghĩa các luồng:

```
daemon → inference
kernel → inference
monitor → inference
behavior engine ← inference
```

Comm không xử lý logic — **chỉ truyền và đóng gói dữ liệu**.

---

## **3.3. data.c**

**Purpose:** Xử lý dữ liệu trước khi chạy inference.

Bao gồm:

* noise reduction
* normalization
* smoothing
* merge signals
* feature extraction từ raw data
* lịch sử inference window (sliding window 5–50ms)

Ngoài ra:

* cung cấp struct `inference_input_t`
* quản lý queue dữ liệu
* kiểm tra tính hợp lệ (validity checks)

Inference Layer chỉ chính xác khi data clean → đây là phần quan trọng nhất trước khi suy luận.

---

## **3.4. inference_manager.c**

**Purpose:** Điều phối toàn bộ inference pipeline.
Là **“bộ điều hành trung tâm”**.

Nhiệm vụ:

### **1) Lifecycle quản lý inference**

* init()
* start()
* step()
* stop()

### **2) Nhận dữ liệu từ comm.c và daemon**

* tập hợp dữ liệu
* gửi vào data.c để chuẩn hóa
* đưa vào inference_runtime.c để suy luận

### **3) Kết hợp với policy/monitor**

Nếu policy yêu cầu:

* lock inference
* giới hạn giá trị
* yêu cầu hard-safe rewrite

### **4) Xuất inference_result**

Gửi kết quả đến:

* behavior engine
* monitor
* logging subsystem
* dip kernel interface

**inference_manager là trung tâm pipeline.**

---

## **3.5. inference_runtime.c**

**Purpose:** Nơi diễn ra **Inference Logic chính xác**.

Đây là phần chính của Inference Engine:

* rule-based inference
* statistical / threshold inference
* pattern match từ kernel signals
* risk scoring
* anomaly scoring
* weighting từ history
* stateful inference (dựa trên runtime state)
* cập nhật inference_state

Kết quả cuối cùng là **inference_result_t**, chứa:

```
{
  risk_score,
  anomaly_score,
  state_flags,
  behavior_hint,
  sensor_confidence,
  kernel_health,
  meta_info
}
```

Runtime là nơi diễn ra:

* tính toán
* suy luận
* đánh giá rủi ro

---

# **4. Pipeline tổng quan của Inference Layer**

```
               daemon signals
                     ↓
                 comm.c
                     ↓
                data.c (clean, merge, normalize)
                     ↓
         inference_runtime.c (main logic – inference computation)
                     ↓
                environment.c
                     ↓   
           inference_behavior.c (convert to behavior hints)
                     ↓
      inference_manager.c (dispatch to Behavior Engine / Monitor)
```

---

# **5. Mục tiêu và nguyên tắc thiết kế**

### **1. Deterministic inference**

Cùng input → luôn ra cùng output.

### **2. Low-latency (<5ms)**

Inference phải chạy cực nhanh cho hệ thống real-time.

### **3. Resilient**

Khi data lỗi → fallback sang chế độ safe-inference.

### **4. Explainable**

Mọi inference phải có lý do → traceable → dùng trong DIP auditing.

### **5. Không phụ thuộc AI/ML phức tạp**

Inference ở DIP là:

* rule-based
* state-based
* introspection-based

→ *tốt cho reproducibility, kiểm soát, và an toàn.*

---

# **6. Đầu ra của Inference Layer**

### **inference_result_t**

* risk_score
* anomaly index
* suggested behavior
* environment modifiers
* kernel health
* sensor confidence

### **behavior_hint_t**

* slow_down
* fallback
* maintain
* reevaluate
* safe_mode

### **monitor_report_t**

* dùng cho logging và audit

---

# **7. Ngắn gọn nhất**

> **Inference Layer** là pipeline xử lý & suy luận trước khi Behavior Engine quyết định hành vi.
> Nó nhận dữ liệu từ daemon/kernel, chuẩn hóa, suy luận rủi ro, và cung cấp “Behavior Hints” giúp hệ thống tự thích ứng một cách an toàn và minh bạch.

---

