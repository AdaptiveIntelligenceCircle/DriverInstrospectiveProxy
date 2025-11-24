# Security Policy

Because DIP interacts with low-level driver components and introspective runtime layers, we treat every potential issue seriously.

---

## 📬 Reporting a Vulnerability

Please send all security-related reports to:
`<security_contact_email>`

Do not open public issues for sensitive vulnerabilities.

---

## ⚙️ Handling Process

1. We will acknowledge receipt within **72 hours**.
2. A fix or patch will be issued within **14–21 days**.
3. Public disclosure only after fix confirmation.

---

## 🔐 Scope

- `/core/` and `/runtime/` — kernel introspection, behavior sandbox
- `/modules/` — proxy layers, adaptation bridges
- `/language/` — introspective interpreter logic
