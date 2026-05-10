# 🏦 Bank Management System 

A fully-featured, terminal-based Bank Management System written in **C++17** using Object-Oriented Programming principles and file-based persistent storage. Features a colourful ANSI UI, robust input validation, a 30-day soft-delete trash bin with restore support, and a secured admin panel.

---

## 📋 Table of Contents

- [Features](#features)
- [OOP Design](#oop-design)
- [Getting Started](#getting-started)
- [Admin Credentials](#admin-credentials)
- [User Operations](#user-operations)
- [Admin Panel](#admin-panel)
- [Trash & Restore](#trash--restore)
- [Input Validation Rules](#input-validation-rules)
- [File Storage](#file-storage)
- [Account Types](#account-types)
- [Project Structure](#project-structure)

---

## ✨ Features

| Feature | Details |
|---|---|
| **Colourful ANSI UI** | Every screen uses distinct colour themes (green = deposit, red = withdraw, blue = inquiry, yellow = admin) |
| **Account Management** | Create Savings, Current, or Fixed Deposit accounts |
| **Deposits & Withdrawals** | With per-account-type business rule enforcement |
| **Balance Inquiry** | Full summary with interest rate and opening date |
| **Fund Transfer** | Between any two active accounts |
| **Transaction History** | Colour-coded, timestamped ledger per account |
| **Soft Delete + Trash** | Deleted accounts go to a 30-day trash bin before permanent deletion |
| **Restore from Trash** | Recover a deleted account along with its full transaction history |
| **Admin Panel** | Protected by an alphanumeric PIN |
| **Input Validation** | All inputs validated before processing — no crashes on bad input |
| **Press Enter to Continue** | After every operation, the menu only reappears after the user presses Enter |
| **Activity Log** | Every action is timestamped and written to `bank_log.txt` |
| **Persistent Storage** | All data survives restarts via `bank_data.dat` and `bank_trash.dat` |

---

## 🏗️ OOP Design

```
Account  (abstract base class)
├── SavingsAccount       → 4.0% p.a.  | Min balance: INR 500
├── CurrentAccount       → 2.0% p.a.  | Overdraft: up to INR 5,000
└── FixedDepositAccount  → 6.5% p.a.  | No withdrawals before maturity
```

### Concepts Applied

| OOP Concept | Where Used |
|---|---|
| **Abstraction** | `Account` declares pure virtual methods `accountType()`, `interestRate()`, `typeColour()` |
| **Inheritance** | All three account types inherit from `Account` |
| **Polymorphism** | `withdraw()` and `deposit()` behave differently per account subclass |
| **Encapsulation** | All member data is `protected`/`private`; accessed only via methods |
| **Composition** | `Bank` owns a vector of `Account*` and a vector of `TrashedEntry` |

### Supporting Classes

| Class / Struct | Role |
|---|---|
| `Transaction` | Stores a single ledger entry; handles serialisation/deserialisation |
| `TrashedEntry` | Holds soft-deleted account data including history and deletion timestamp |
| `Utils` namespace | All shared helpers: validators, readers, visual printers |
| `C` namespace | ANSI colour/style constants |

---

## 🚀 Getting Started

### Prerequisites

- A C++17-compatible compiler (`g++`, `clang++`)
- A terminal that supports ANSI escape codes (Linux, macOS, Windows Terminal, Git Bash)

### Compile

```bash
g++ -std=c++17 -Wall -o bank bank_management.cpp
```

### Run

```bash
./bank
```

Data files (`bank_data.dat`, `bank_trash.dat`, `bank_log.txt`) are created automatically in the same directory on first run.

---

## 🔐 Admin Credentials

> **⚠️ Important — keep this confidential in a real deployment.**

| Field | Value |
|---|---|
| **Admin PIN** | `Bank@2025` |
| **PIN Rules** | 6–12 characters, must contain at least one letter and at least one digit |
| **Allowed characters** | Letters (`a-z`, `A-Z`), digits (`0-9`), `@`, `_` |

### How to Change the Admin PIN

Open `bank_management.cpp` and locate this line inside the `Bank` class:

```cpp
const string ADMIN_PIN = "Bank@2025";
```

Replace the value with your new PIN (must satisfy the alphanumeric rule above), then recompile.

---

## 👤 User Operations

| Option | Description |
|---|---|
| `[1]` Create New Account | Choose account type, enter name, set a 4–6 digit PIN, make initial deposit |
| `[2]` Deposit Funds | Authenticated deposit into any account |
| `[3]` Withdraw Funds | Authenticated withdrawal; enforces per-type rules |
| `[4]` Balance Inquiry | Shows full account summary |
| `[5]` Fund Transfer | Transfer between two accounts; both balances displayed after |
| `[6]` Transaction History | Colour-coded full ledger with timestamps |
| `[0]` Exit | Saves all data and exits gracefully |

---

## 🛠️ Admin Panel

Access via **Main Menu → [7] Admin Panel** and enter the admin PIN.

| Option | Description |
|---|---|
| `[1]` View All Accounts | Tabular list of every active account with balances |
| `[2]` Search by Name | Finds and displays only the matching account(s) — not all accounts |
| `[3]` Delete Account | Moves account to 30-day Trash (soft delete) |
| `[4]` View Trash | Lists all soft-deleted accounts with days remaining |
| `[5]` Restore from Trash | Restores a trashed account (with full history) back to active |
| `[6]` View System Log | Shows last 25 audit log entries |
| `[0]` Back to Main Menu | Returns to the main menu |

---

## 🗑️ Trash & Restore

### How Soft Delete Works

1. Admin selects **Delete Account** and confirms.
2. The account (including full transaction history and PIN) is moved to `bank_trash.dat`.
3. The account is removed from active accounts in `bank_data.dat`.
4. The entry lives in trash for **30 days**, displayed with a day counter.
5. Entries with **≤ 7 days remaining** are highlighted in **red** as a warning.
6. On day 30, the entry is **permanently and irreversibly deleted** (purged automatically on next launch or trash view).

### How Restore Works

1. Admin selects **[5] Restore Account from Trash**.
2. A numbered list of all trashed accounts is shown.
3. Admin enters the number of the account to restore.
4. The account is rebuilt from stored data — including all transaction history.
5. If an active account with the same ID already exists, the restore is blocked with an error.
6. Once restored, the entry is removed from trash and the account is fully active again.

---

## ✅ Input Validation Rules

| Input | Rule |
|---|---|
| **Account holder name** | 2–50 characters, letters and spaces only |
| **User PIN** | 4–6 numeric digits only (`0-9`) |
| **Admin PIN** | 6–12 characters; must contain ≥ 1 letter and ≥ 1 digit; allows `@` and `_` |
| **Account ID** | Must start with `ACC` followed by digits (e.g. `ACC1001`) |
| **Deposit / withdrawal amount** | Must be a positive number greater than zero |
| **Menu choices** | Only valid integers in the displayed range accepted |

All invalid inputs display a red error message and re-prompt — the program never crashes on bad input.

---

## 📁 File Storage

| File | Purpose |
|---|---|
| `bank_data.dat` | Active accounts + transaction histories (pipe-delimited text) |
| `bank_trash.dat` | Soft-deleted accounts with deletion timestamps |
| `bank_log.txt` | Append-only audit trail with timestamps for every action |

All files are created automatically. Do **not** manually edit them while the program is running.

---

## 💳 Account Types

### Savings Account
- **Interest:** 4.0% p.a.
- **Minimum Balance:** INR 500.00 must be maintained at all times
- **Withdrawals:** Blocked if balance would fall below INR 500

### Current Account
- **Interest:** 2.0% p.a.
- **Overdraft:** Withdrawals up to INR 5,000 beyond current balance are permitted
- **Use case:** Businesses and frequent transactions

### Fixed Deposit Account
- **Interest:** 6.5% p.a. (highest rate)
- **Deposits:** Not accepted after account opening
- **Withdrawals:** Blocked before maturity (contact branch manager to close early)

---

## 🗂️ Project Structure

```
bank_management.cpp     ← Single-file C++ source (all classes included)
bank_data.dat           ← Auto-generated: active account data
bank_trash.dat          ← Auto-generated: soft-deleted account data
bank_log.txt            ← Auto-generated: audit activity log
README.md               ← This file
```

---

## 📌 Notes

- PINs are stored in plain text in the `.dat` files — suitable for academic/demo use. For production, apply a hash function (e.g. SHA-256) before storing.
- The application is single-user (one session at a time). Concurrent access is not handled.
- ANSI colours require a compatible terminal. On older Windows `cmd.exe`, run in **Windows Terminal** or enable virtual terminal processing.

---

*Built as a C++ OOP + File Handling academic project.*
