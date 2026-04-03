# DriverInstrospectiveProxy

**DriverInstrospectiveProxy** is an experimental project that explores the integration of AI into the kernel/driver space—where introspection, real-time responsiveness, and low-level security are critical.

<img width="1024" height="1024" alt="image" src="https://github.com/user-attachments/assets/a6169fcb-dd2d-4861-85a4-bd0d4057439e" />

> AIC/HMN is licensed under GPL-3.0. Any fork or derivative work must keep the same license and respect the core principles: zero-donation, third path independence, and ethical-from-kernel. The project is currently maintained by the founder. During the founder’s mandatory military service (expected 2027–2029), the project will enter maintenance mode. All code remains public and transparent.”

---

## 🚀 Purpose

To build a smart proxy operating at the driver level, capable of:
- Monitoring system behavior in real time
- Reacting or alerting upon anomalies
- Communicating with AI modules to make context-aware decisions

## 🔍 Why "Instrospective"?

Because this proxy doesn’t just relay data—it **observes**, **analyzes**, and **responds**. It acts as a conscious intermediary, making the system more adaptive and secure.

## 🧠 AI Integration

- Lightweight AI models designed for constrained environments
- Supports local inference or IPC with external AI daemons
- Extensible to connect with systems like Safe AI or IBCS

## 📦 Project Structure
```
DriverInstropectiveProxy/
├── kernel/
│   ├── hook/              # Mã nguồn liên quan đến driver hook
│   ├── monitor/           # Theo dõi hành vi hệ thống
│   └── comm/              # Giao tiếp IPC với AI daemon
│
├── ai/
│   ├── models/            # Mô hình AI nhúng hoặc lightweight
│   ├── inference/         # Logic suy diễn cục bộ
│   └── daemon/            # AI daemon chạy nền (Python hoặc C++)
│
├── config/
│   ├── rules.yaml         # Quy tắc phát hiện bất thường
│   └── settings.json      # Cấu hình hệ thống
│
├── docs/
│   ├── architecture.md    # Thiết kế tổng thể
│   ├── protect.md         # Bảo vệ khỏi dữ liệu độc hại
│   └── api.md             # Giao diện IPC/API
│
├── scripts/
│   ├── build.sh           # Script build cho Linux
│   └── install.ps1        # Script cài đặt cho Windows
│
├── tests/
│   ├── unit/              # Kiểm thử đơn vị
│   └── integration/       # Kiểm thử tích hợp
│
├── examples/
│   └── anomaly_case/      # Tình huống phát hiện bất thường
│
├── LICENSE
└── README.md


```

## ⚙️ System Requirements

- OS with driver-level hook support (Linux kernel module, Windows WDM/UMDF)
- Compiler: GCC/Clang or MSVC
- Python (optional, for AI daemon support)

## 🧬 Related Projects
- Adaptive OS : https://github.com/CodelikeC/AdaptiveOS

- AIForDriver : https://github.com/CodelikeC/AIForDriver

## Badges 

![Status](https://img.shields.io/badge/status-experimental-orange?style=flat-square&logo=github)
![Release](https://img.shields.io/github/v/release/<YOUR-USERNAME>/<YOUR-REPO>?style=flat-square&logo=semver)
![Pre-Release](https://img.shields.io/github/v/release/<YOUR-USERNAME>/<YOUR-REPO>?include_prereleases&label=pre-release&color=blue&style=flat-square)

![Build](https://img.shields.io/badge/build-passing-brightgreen?style=flat-square&logo=cmake)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows-blue?style=flat-square&logo=linux&logo=windows)
![Language](https://img.shields.io/badge/language-C%2B%2B23-blueviolet?style=flat-square&logo=c%2B%2B)
![Language](https://img.shields.io/badge/language-Lua-lightgrey?style=flat-square&logo=lua)

![License](https://img.shields.io/github/license/<YOUR-USERNAME>/<YOUR-REPO>?style=flat-square)
![Issues](https://img.shields.io/github/issues/<YOUR-USERNAME>/<YOUR-REPO>?style=flat-square)
![Stars](https://img.shields.io/github/stars/<YOUR-USERNAME>/<YOUR-REPO>?style=flat-square&logo=github)



