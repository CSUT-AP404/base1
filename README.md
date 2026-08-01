# Banking System 
**Course:** Advanced Programming  
**Group:** base1  
**Members:** Matin Mohammadi — Rahan Hemmatinejad — Armin Sahraie

---

## Project Structure
```
├── src/            # Source files (.cpp)
├── include/        # Header files (.h)
├── data/           # JSON data files (git-ignored at runtime)
├── Makefile        # Build automation
└── README.md
```

---

## How to Compile & Run

This project uses a **Makefile** to make compiling easier.
Instead of typing a long g++ command every time, you just use `make`.

### Step 1 — Make sure g++ is installed
```bash
g++ --version
```
If you get a version number you're good. If not, install it:
- **Windows:** install [MinGW](https://www.mingw-w64.org/) and add it to your PATH
- **Linux:** `sudo apt install g++`
- **Mac:** `xcode-select --install`

### Step 2 — Open a terminal in the project folder
- In VS Code: **Terminal → New Terminal**
- Make sure you're in the root of the project (where the Makefile is)

### Step 3 — Compile
```bash
make
```
This reads the Makefile and compiles all `.cpp` files into an executable called `bank_system`.
You will see output like:
```
g++ -std=c++17 -Wall -o src/user src/user.cpp
g++ -std=c++17 -Wall -o src/admin src/admin.cpp
```

### Step 4 — Run admin or user
```bash
./admin
./user
```
On Windows:
```cmd
admin.exe
user.exe
```

### Step 5 — Clean compiled files
If you want to recompile everything from scratch:
```bash
make clean
```
This deletes all `.o` files and the executable. Then just run `make` again.

### Compile and run admin in one command
```bash
make run-admin
```
### Compile and run user in one command
```bash
make run-user
```

---

## Quick Reference

| Command | What it does |
|---|---|
| `make` | Compiles the project |
| `make run-admin` | Compiles and runs admin immediately |
| `make run-user` | Compiles and runs user immediately |
| `make clean` | Deletes compiled files |

---

## Features (Phase 1)
- Branch management: create and list branches
- Account management: create, close, delete, list accounts
- Transactions: deposit, withdraw, transfer
- History: get balance, transaction history, transaction details
- Data management: clear history, reset all
- Passwords hashed with SHA-256
- All data persisted in JSON files under data/

---
## Phase Two

In Phase Two, the banking system was extended while preserving the functionality and commands implemented in Phase One.

The main goal of this phase was to add a separate interface for ordinary users in addition to the administrative interface.

### New User Application

A new `user.cpp` application was added for ordinary users. Users can:

- Sign up using their national code and password.
- Log in and log out of the system.
- Open one or more bank accounts.
- View their own accounts and balances.
- Deposit money into their accounts.
- Withdraw money from their accounts.
- Transfer money between accounts.
- Delete their own bank accounts.
- Delete their user account.

The user application restricts access to accounts owned by the currently logged-in user. A user cannot view or operate on another user's accounts.

### Authentication and Security

Separate authentication mechanisms are used for users and bank accounts:

- Each user has a national code and a password.
- Each bank account has its own account password.
- Passwords are stored as hashes instead of plain text.
- Login and account operations validate the corresponding password before performing sensitive actions.
- Unauthenticated users cannot perform account operations.

### Account Management

Users can create multiple accounts. Every account receives a unique 16-digit account number displayed in the following format:
```text
XXXX-XXXX-XXXX-XXXX


## Notes
- Data files are created automatically on first run
- If no data files exist, system starts with empty state
- Never commit your data/ JSON files (already in .gitignore)
- Never push your passwords or any sensitive data
