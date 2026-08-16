# Banking System 
**Course:** Advanced Programming  
**Group:** base1  
**Members:** Matin Mohammadi — Rahan Hemmatinejad — Armin Sahraei

---

## Project Structure
```

base1/
├── src/          # Source files
│   ├── admin.cpp
│   ├── user.cpp
│   ├── admin-server.cpp
│   ├── user-server.cpp
│   └── client.py
│
├── include/       # Header files (.h)
│   ├── httplib.h
│   ├── json.hpp
│   ├── picosha2.h
│   └── SHA256.cpp
│
├── data/          # JSON data files (git-ignored at runtime)
│   ├── Admins.json
│   ├── Bank_Data.json
│   └── setting.json
│
├── CMakeLists.txt  
├── Makefile
├── requirements.txt
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

## Phase One
In this phase of the project, the main core of the banking system has been implemented. This phase focuses on the fundamental aspects of the system!

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
```
## Phase Three

Phase Three extends the banking system by replacing direct account creation with a branch-based request workflow and introducing more advanced electronic banking features. This phase preserves the capabilities of the previous phases while changing the account opening process from an immediate action into a multi-step branch approval flow.

### Branch-Based Account Opening

The direct `open_account` workflow is replaced with a formal request process. A user must first choose a branch and submit an account request. The administrator of the system can then review the request and either approve or reject it. If the request is approved, the account is still not created immediately. The user must later activate the approved request and set the account password to finalize the account creation process.

Typical flow:
1. User checks available branches with `list_branches`
2. User submits a request with `request_account <branch_id>`
3. Admin reviews branch requests with `list_requests <branch_id>`
4. Admin decides with `approve_request <request_id>` or `reject_request <request_id>`
5. User completes the process with `activate_account <request_id>`

Rules:
- Each user can have at most one active account or one pending request in the same branch.
- An approved request is not the same as an active account.
- Account creation is completed only after user activation.
- Rejected requests should remain stored with their rejection reason for later inspection.

### User Commands

The user-side workflow in this phase includes the following commands:

- `list_branches` — display all available branches
- `request_account <branch_id>` — submit a new account opening request for a branch
- `my_requests` — show the current user’s submitted requests and their statuses
- `activate_account <request_id>` — activate an approved request and create the account

Expected error cases include:
- `Error: No user logged in.`
- `Error: Branch not found.`
- `Error: You already have a pending or active account in this branch.`
- `Error: Request not found.`
- `Error: Request does not belong to user.`
- `Error: Request is not approved.`

### Administrative Commands

The administrator is responsible for supervising branch requests and approving or rejecting them. Phase Three introduces the following administrative commands:

- `branch_dashboard <branch_id>` — show branch information and request/account statistics
- `list_requests <branch_id>` — list pending requests for a branch
- `approve_request <request_id>` — approve a pending request
- `reject_request <request_id>` — reject a pending request and record the reason

The branch dashboard should present a compact operational summary for a branch, including:
- branch name
- number of active accounts
- number of pending requests
- number of requests rejected on the current day

### Advanced Transactions

Phase Three also introduces more advanced banking operations and transfer control policies.

#### Transaction Limits
Different transaction types should respect defined transfer limits. A recommended policy is:

`online transfer limit < regular transfer limit < Paya transfer limit`

When a transaction exceeds its allowed threshold, the system should reject it with:

`Error: Transaction limit exceeded.`

#### IBAN Support
Each account should generate and store a valid Iranian IBAN. This makes interbank transfer scenarios possible and prepares the system for more realistic banking operations.

#### Online Payment with OTP
Online card/account-style transfers are introduced through:

`online_payment <from_account> <to_account> <amount>`

This operation should require a time-limited one-time password (OTP). The system is expected to validate both correctness and expiration of the OTP before completing the transfer.

Possible errors:
- `Error: OTP expired.`
- `Error: Invalid OTP.`

#### Paya Transfers
Interbank transfers are supported through:

`paya_transfer <from_account> <destination_iban> <amount>`

This type of transfer is not completed immediately. Instead:
- the request is registered as pending
- the amount is reserved or deducted from the source account
- the administrator reviews the request
- approval completes the transfer
- rejection restores the amount to the source account

### Administrative Paya Management

To support delayed interbank transfer handling, the administrator is expected to manage Paya requests with commands such as:

- `list_paya_requests` — show registered Paya transfer requests
- `approve_paya <request_id>` — approve and finalize a Paya transfer
- `reject_paya <request_id>` — reject a Paya transfer and return funds

### Persistence and Reliability

All new entities introduced in this phase must be persistent. This includes:
- account opening requests
- request IDs
- request statuses
- rejection reasons
- timestamps
- IBAN values
- OTP-related data needed by the workflow
- Paya transfer requests and their statuses
- related transaction records

After restarting the program, the system should restore these records correctly from JSON storage.

The request ID sequence should begin at `2001` and continue incrementally.

As in previous phases, malformed input or invalid commands must not crash the program. The system should handle bad input safely and return meaningful error messages instead of terminating unexpectedly.

A final implementation note is that filename consistency for the bank data file is important. If the project uses both `data/BankـData.json` and `data/Bank_Data.json` in different parts of the code, that mismatch should be corrected to prevent reset/load inconsistencies and possible data loss.

## Phase Four

Phase Four extends the banking system by introducing a client-server architecture.
Instead of interacting with the banking logic directly through the console applications,
users and administrators can communicate with the system through HTTP requests.

The main goal of this phase is to separate the banking logic from the communication layer
and provide dedicated clients for ordinary users and administrators.

### Client-Server Architecture

The system is divided into several components:

- `user.cpp` — contains the main banking logic and user-side operations.
- `admin.cpp` — contains administrative operations.
- `user-server.cpp` — provides an HTTP interface for user operations.
- `admin-server.cpp` — provides an HTTP interface for administrative operations.
- `bankClient.py` — provides Python clients for communicating with the servers.

The general architecture is:

```text
                    HTTP Requests
                         |
          +--------------+--------------+
          |                             |
          v                             v
   User Client                    Admin Client
   (Python)                       (Python)
          |                             |
          v                             v
   User Server                    Admin Server
          |                             |
          v                             v
      user.cpp                    admin.cpp
          \                             /
           \                           /
            +-------- JSON Data -------+
```

The client is responsible for communication with the server, while the server is
responsible for receiving requests, translating them into banking commands, executing
the corresponding operation, and returning a JSON response.

### HTTP Communication

Phase Four introduces HTTP endpoints for the operations that were previously available
through direct commands.

Requests contain the banking command and its required arguments in the request body.
The server translates the request body into the corresponding command before passing it
to the banking logic.

For example:

```text
signup 1234567890 password
```

is sent by the client to the appropriate user endpoint.

Similarly, an account deposit can be represented as:

```text
deposit_to <account_id> <amount>
```

The server processes the command and returns the result as a JSON response.

This approach keeps the existing banking commands while adding a network interface
around them.

### User Server

The user server provides HTTP endpoints for ordinary users.

The server supports operations including:

- User signup
- User login and logout
- Listing available branches
- Creating account opening requests
- Viewing account requests
- Activating approved account requests
- Viewing user accounts
- Depositing money
- Withdrawing money
- Card-to-card transfers
- Paya transfers
- Balance inquiries
- Account history
- OTP-related operations
- Online payments
- IBAN operations
- User account deletion
- User ranking

The user server listens on:

```text
127.0.0.1:8080
```

### Admin Server

The admin server provides HTTP endpoints for administrative operations.

The administrator can use the server to perform operations such as:

- Admin authentication
- Branch management
- Branch dashboards
- Reviewing account opening requests
- Approving and rejecting account requests
- Managing accounts
- Managing Paya requests
- Viewing transactions and histories
- Managing transfer and inquiry fees
- Viewing rankings
- System management operations

The admin server is separated from the user server so that administrative operations
are exposed through a dedicated interface.

### Authentication

Phase Four introduces authentication at the HTTP layer.

After a successful login, the server returns an authentication token to the client.
The Python client stores this token and sends it with subsequent authenticated requests.

Authenticated requests use the following HTTP header:

```text
Authorization: Bearer <token>
```

This prevents unauthenticated clients from directly accessing protected banking
operations.

The client also removes its stored token after a successful logout.

### Python Client

A Python client was added to simplify communication with the HTTP servers.

The client provides two main classes:

```python
UserClient
AdminClient
```

Both clients share common HTTP functionality such as:

- Sending HTTP requests
- Setting request headers
- Sending command payloads
- Handling JSON responses
- Handling connection errors
- Storing authentication tokens

The user and admin clients then implement their own banking and administrative
operations on top of this common communication layer.

### Error Handling

The client handles communication failures without terminating the program.

For example, if the corresponding server is not running, the client reports a
connection error instead of crashing:

```text
Connection error: ...
```

HTTP errors are also converted into structured responses containing information
about the failure.

The server continues to return JSON responses so that the client can process
successful and unsuccessful operations consistently.

### Running Phase Four
First of all, requires this:
```bash
python -m pip install -r requirements.txt
```
The servers must be started before using the Python client.

The project should be run from the root directory of the repository because the
server source code uses paths relative to the project root.

For example:

```bash
cd base1-phase4-continue
./src/user-server
```

The user server then listens on:

```text
127.0.0.1:8080
```

The corresponding client can then be started separately:

```bash
python3 bankClient.py
```

The administrative server should also be started before performing administrative
operations.

### Important Execution Note

The HTTP server compiles the corresponding banking source during startup.
Therefore, the server should be launched from the project root directory.

For example, the following is recommended:

```bash
cd base1-phase4-continue
./src/user-server
```

Running the server from inside the `src/` directory can cause relative paths such as:

```text
src/user.cpp
```

to be resolved incorrectly as:

```text
src/src/user.cpp
```

which prevents the server from starting.

### Phase Four Summary

Phase Four separates the banking system into a communication layer and a banking
logic layer.

The resulting system consists of:

```text
Python Client
     |
     | HTTP
     v
HTTP Server
     |
     | Banking Commands
     v
Banking Logic
     |
     v
JSON Data
```

This architecture makes it possible to interact with the banking system remotely
through HTTP while keeping the existing banking functionality and persistent data
management.

## Phase Four

- **Admin Password:** `wopwop`
