# **Driver Introspective Proxy – Kernel Architecture (kernel.md)**

*(Compact, Technical, Architectural — bản final đề xuất dùng trong repo)*

---

# **1. Tổng quan Kernel**

Kernel của **Driver Introspective Proxy (DIP)** là **tầng lõi chạy thời gian thực**, nơi mọi tín hiệu vật lý – logic – introspective đều được đồng bộ, kiểm soát và bảo vệ.

Kernel đảm nhiệm 5 chức năng bất biến:

1. **Real-time control & data flow** (I/O → Behavior → Safety → Actuators)
2. **Enforce hard-safety & policy**
3. **Manage drivers & hardware abstraction**
4. **Secure message routing & protocol handling**
5. **Deep internal monitoring (introspection, anomaly, watchdog)**

---

# **2. Cấu trúc tổng thể Kernel**

```
kernel/
 ├── core/            ← vòng điều khiển, state machine, scheduler
 ├── hook/            ← điểm nối vào behavior, plugins, introspection
 ├── include/         ← headers thống nhất API nội bộ kernel
 ├── message/         ← protocol adapter, envelope, routing
 ├── monitor/         ← health, anomaly, risk, watchdog
 ├── drivers/         ← hardware abstraction layer
 └── policy/          ← hard-rules, policy loader, permission gate
```

Mỗi folder giữ **1 vai trò duy nhất**, loại bỏ hoàn toàn overlap.

---

# **3. core/** — *Real-time execution layer*

`core/` chứa toàn bộ logic điều khiển chính của Kernel. 

**Chức năng chính của core/**:

1. Thu thập sensor data từ `drivers/`
2. Gửi data → hook/behavior → nhận decision
3. Áp dụng safety từ `policy/`
4. Gửi command tới drivers → actuators
5. Xuất telemetry → message/
6. Log introspection → monitor/

> **Core là trái tim của DIP**, mọi cycle đều đi qua core → hook → drivers.

---

# **4. hook/** — *Behavior, Plugin & Introspective Interfaces*

`hook/` tạo các điểm nối an toàn giữa:

* Kernel ↔ Behavior Engine
* Kernel ↔ Plugin System
* Kernel ↔ Introspection Engine

**Vai trò:**

1. Behavior Engine không bao giờ chạm kernel trực tiếp → chỉ qua hook.
2. Plugin chỉ được quyền gọi API hạn chế (read telemetry, publish message).
3. Introspection Engine có thể nhìn vào state nhưng không được ghi.

**Hook = interface an toàn + (optional) sandbox injection.**

---

# **5. include/** — *Kernel API & shared headers*

`include/` định nghĩa toàn bộ API nội bộ của kernel. 

Mọi file trong kernel sẽ include từ đây, đảm bảo **nhất quán** và **dễ maintain**.

---

# **6. message/** — *Protocol, Envelope, and Routing*

Chi phối toàn bộ giao tiếp với thế giới bên ngoài.

**Nhiệm vụ của message/:**

1. Nhận Envelope → verify signature → policy check → route
2. Gửi command/telemetry/alert theo protocol
3. Tạo secure channel (nonce, session key, handshake)
4. Liên kết message vào monitor (anomaly) và policy

> Message layer **không bao giờ** bypass policy/monitor.

---

# **7. monitor/** — *Health, Risk, Watchdog & Introspection*

Monitor là lớp quan sát kernel theo thời gian thực.

**Vai trò của monitor/**:

* Khi risk > threshold → gửi event → core vào safe-mode
* Khi driver mất tín hiệu → degrade mode
* Khi plugin sai hành vi → unload plugin
* Khi handshake bất thường → raise COMM_ANOMALY

> Monitor là “tai mắt” của kernel, đảm bảo DIP không bao giờ hoạt động mù.

---

# **8. drivers/** — *Hardware Abstraction Layer*

Mỗi thiết bị đều có driver riêng.

Ví dụ:

```
drivers/
   ├── motor_driver.cpp
   ├── imu_driver.cpp
   ├── torque_sensor.cpp
   ├── power_monitor.cpp
   ├── lidar_driver.cpp
   └── can_bus.cpp
```
** lưu ý: Sẽ được cải tiến trong thời gian tới ** 

Tất cả driver tuân theo 1 interface:

```cpp
struct IDriver {
    bool connect();
    bool read(DeviceState&);
    bool write(const DeviceCommand&);
    DriverHealth health();
};
```

**Drivers:**

* Không được giao tiếp trực tiếp với plugin
* Gửi health info về monitor
* Tích hợp vào control loop thông qua core/

---

# **9. policy/** — *Hard Rules, Runtime Policy & Permission Gate*

Folder này điều phối toàn bộ chính sách của kernel:

* policy_index.json
* default_policy.json
* secure_policy.json
* hard_rules.json
* `policy_manager.cpp/h`

**Nhiệm vụ:**

* Kiểm tra mọi hành động nhạy cảm
* Không cho phép override hard rules
* Kiểm tra manifest, signature plugin
* Kiểm tra command từ remote
* Apply deny-by-default

Flow đơn giản:

```
kernel receives event
→ policy_manager.allow(action, context)
→ yes → process
→ no → block + log to monitor/anomaly
```

**Policy là “lớp da bảo vệ” xung quanh kernel.**

---

# **10. Luồng xử lý chính của Kernel (tối giản)**

## **Cycle Loop**

```
drivers.read()
→ hook.behavior_decide()
→ apply_safety(policy)
→ drivers.write()
→ message.publish_telemetry()
→ monitor.record_trace()
```

## **Receive Command**

```
message.receive() 
→ verify signature 
→ policy check 
→ hook.behavior_input() 
→ core.apply_behavior()
```

## **Risk / Anomaly**

```
monitor.risk > threshold
→ core.safe_mode()
→ if critical → core.emergency_stop()
```

---

# **11. Điều Kernel không bao giờ cho phép**

* Plugin ghi trực tiếp vào driver
* Behavior override hard safety
* Command không ký / sai checksum
* Remote override safe mode
* Transport gửi gói không đúng schema
* Policy bị thay đổi bởi plugin/behavior

Kernel = **cuối cùng quyết định**, kể cả Behavior và Adaptive Layer cũng không thể vượt.

---

# **12. Định nghĩa ngắn gọn nhất**

> Kernel của DIP là trung tâm điều khiển thời gian thực, kết nối hardware–behavior–security, thi hành chính sách an toàn, giám sát rủi ro và bảo vệ hệ thống trước mọi tình huống.

---

