# API Documentation — Banking System (Phase 4)

This document follows section 10 of the Phase 4 assignment (optional bonus)
and specifies, for every endpoint: URL & Method, Request Body, Success
Response, and Error Response.

## General Notes (read before the tables)

**Base URLs:**
| Server | Address |
|---|---|
| Admin server | `http://127.0.0.1:8080` |
| User server | `http://127.0.0.1:8081` |

**Request body format:**
Request bodies are **not JSON** — they are plain text (`Content-Type:
text/plain`) using the same syntax the CLI binaries (`admin`/`user`)
accept: a command word followed by space-separated parameters. Example:
```
signup 0011234567 mypassword
deposit_to 1000-0000-0000-0001 50000
```
If a parameter itself contains spaces (e.g. a branch name), it is wrapped
in `"..."`; example: `create_branch "Central Branch"`.

**Authentication:**
After login, the received `token` must be sent on every protected request
in this header:
```
Authorization: Bearer <token>
```

**Standard response shape:**
```jsonc
// success
{ "ok": true, "message": "...", "data": [ { "key": "value" }, ... ] }

// failure
{ "ok": false, "error": "..." }   // or an array of error strings
```

**HTTP status code mapping** (based on the `Status()` function in both servers):

| Code | When it happens |
|---|---|
| `200` | Successful operation (even if the returned list is empty) |
| `400` | Invalid or unrecognized command/input |
| `401` | Missing token or not logged in (`No user/admin logged in`, `No token`) |
| `403` | Wrong password, invalid token, or unauthorized access to another user's/account's resource |
| `404` | Unknown route (endpoint) — response `{"ok": false, "error": "API not found"}` |
| `409` | Conflict with an existing active/duplicate state (e.g. "already ...") |
| `422` | Violation of a business rule (insufficient funds, expired OTP, transaction limit, ...) |

---

# 1. User Server — `http://127.0.0.1:8081`

## Authentication & Session Management

### `POST /auth/signup`
- **Body:** `signup <codeMelli> <password>`
- **Success (200):**
  ```json
  { "ok": true, "message": "User created.", "data": [] }
  ```
- **Error:** `409` if the national code is already registered.

### `POST /auth/login`
- **Body:** `login <codeMelli> <password>`
- **Requires token:** No
- **Success (200):**
  ```json
  { "ok": true, "message": "Logged in.", "data": [ { "token": "AbC123..." } ] }
  ```
- **Error:** `403` wrong password, `422` user not found.

### `DELETE /auth/session`
- **Description:** Logs out and invalidates the current token.
- **Body:** `logout`
- **Requires token:** Yes
- **Success (200):**
  ```json
  { "ok": true, "message": "Logged out.", "data": [] }
  ```
- **Error:** `401` no token sent.

---

## Branches & Account Requests

### `GET /branches`
- **Body:** `list_branches`
- **Requires token:** No
- **Success (200):**
  ```json
  { "ok": true, "message": "...", "data": [
      { "branches": [ { "branch_id": "1", "branch_name": "Central" } ] }
  ] }
  ```

### `POST /accounts/requests`
- **Description:** Submit an account-opening request at a branch.
- **Body:** `request_account <branch_id>`
- **Requires token:** Yes
- **Success (200):**
  ```json
  { "ok": true, "message": "...", "data": [ { "request_id": "2001" } ] }
  ```
- **Error:** `409` if the user already has an active request/account at the same branch; `422` branch not found.

### `GET /accounts/requests`
- **Description:** List the current user's account-opening requests.
- **Body:** `my_requests`
- **Requires token:** Yes
- **Success (200):**
  ```json
  { "ok": true, "message": "...", "data": [
      { "requests": [
          { "request_id": "2001", "branch_id": "1", "request_status": "pending", "request_time": "..." },
          { "request_id": "2000", "branch_id": "2", "request_status": "rejected", "request_reason": "..." }
      ] }
  ] }
  ```

### `DELETE /accounts/requests/{request_id}`
- **Description:** Cancel a pending account-opening request.
- **Body:** `cancel_request <request_id>`
- **Requires token:** Yes
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "request_id": "2001" } ] }`
- **Error:** `422` request not found or not cancellable.

### `PATCH /accounts/{request_id}/activation`
- **Description:** Activate the account once the admin approves the request.
- **Body:** `activate_account <request_id> <account_password>`
- **Requires token:** Yes
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "account_id": "1000-..." } ] }`
- **Error:** `422` request is not approved.

---

## Accounts & Transactions

### `GET /accounts`
- **Body:** `my_accounts`
- **Requires token:** Yes
- **Success (200):**
  ```json
  { "ok": true, "message": "...", "data": [
      { "accounts": [ { "account_id": "1000-0000-0000-0001", "balance": "100000.00" } ] }
  ] }
  ```

### `DELETE /accounts/{account_id}`
- **Description:** Delete a bank account with zero balance.
- **Body:** `delete_my_account <account_id> <account_password>`
- **Requires token:** Yes
- **Error:** `403` wrong password or account belongs to another user; `422` balance is not zero (`Account balance is positive`).

### `POST /accounts/{account_id}/deposits`
- **Body:** `deposit_to <account_id> <amount>`
- **Requires token:** Yes
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "transaction_id": "1001" }, { "new_balance": "150000.00" } ] }`
- **Error:** `400` invalid amount (`Amount must be positive`).

### `POST /accounts/{account_id}/withdrawals`
- **Body:** `withdraw_from <account_id> <amount> <account_password>`
- **Requires token:** Yes
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "transaction_id": "1002" }, { "new_balance": "50000.00" } ] }`
- **Error:** `422` insufficient funds (`Insufficient funds`) or over the transaction limit (`limit exceeded`).

### `POST /transfers/card-to-card`
- **Body:** `send_money <from_account> <to_account> <amount> <from_account_password>`
- **Requires token:** Yes
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "transaction_id": "1003" }, { "new_balance": "..." } ] }`
- **Error:** `422` insufficient funds or transfer limit; `422` destination not found.

### `POST /accounts/{account_id}/balance-inquiries`
- **Description:** Balance inquiry (a fee is deducted).
- **Body:** `balance_inquiry <account_id>`
- **Requires token:** Yes
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "balance": "..." }, { "active": "true" }, { "branch_id": "1" } ] }`

### `GET /accounts/{account_id}/statement`
- **Description:** Get the full transaction statement of an account.
- **Body:** `get_history <account_id>`
- **Requires token:** Yes
- **Success (200):**
  ```json
  { "ok": true, "message": "...", "data": [
      { "history": [
          { "transaction_id": "1001", "transaction_time": "...", "transaction_type": "deposit",
            "transaction_amount": "50000.00", "new_balance": "150000.00" }
      ] }
  ] }
  ```

---

## OTP, Online Payment, Paya Transfer & IBAN

### `POST /auth/otp`
- **Description:** Request a one-time password (OTP) for an account.
- **Body:** `request_OTP <account_id>`
- **Requires token:** Yes
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "OTP": "482913" }, { "OTP_duration": "120" } ] }`

### `POST /payments/online`
- **Body:** `online_payment <from_account> <to_account> <amount> <otp>`
- **Requires token:** Yes
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "transaction_id": "1004" }, { "new_balance": "..." }, { "new_balance_destination": "..." } ] }`
- **Error:** `403` wrong OTP (`Invalid OTP`); `422` OTP expired (`OTP expired`).

### `GET /accounts/{account_id}/iban`
- **Body:** `show_iban <account_id>`
- **Requires token:** Yes
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "iban": "IR..." } ] }`

### `POST /transfers/paya`
- **Description:** Submit a Paya transfer request (requires admin approval).
- **Body:** `paya_transfer <from_account_id> <destination_iban> <amount> <from_account_password>`
- **Requires token:** Yes
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "paya_id": "3001" }, { "status": "pending" } ] }`
- **Error:** `422` destination IBAN not found or over the transaction limit.

---

## Other User Features

### `DELETE /users/me`
- **Description:** Delete the user account (requires having no open bank accounts).
- **Body:** `delete_my_user <password>`
- **Requires token:** Yes
- **Error:** `422` user still has bank accounts (`User has accounts`).

### `GET /users/me/rank`
- **Body:** `my_rank`
- **Requires token:** Yes
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "rank": "3" }, { "score": "150" }, { "level": "Gold" } ] }`

---

# 2. Admin Server — `http://127.0.0.1:8080`

> Default password of the first admin in the system: `wopwop`

## Admin Authentication & Accounts

### `POST /admin/auth/signup`
- **Description:** Add a new admin (only allowed while logged in as an existing admin).
- **Body:** `signup <codeMelli> <password>`
- **Requires token:** Yes
- **Error:** `403` if the bank already has admins and the current caller is not one.

### `POST /admin/auth/login`
- **Body:** `login <password>`
- **Requires token:** No
- **Success (200):** `{ "ok": true, "message": "Logged in.", "data": [ { "token": "..." } ] }`
- **Error:** `403` wrong password; `409` already logged in; `422` admin not found.

### `DELETE /admin/auth/session`
- **Description:** Admin logout.
- **Body:** `logout`
- **Requires token:** Yes
- **Error:** `401` no admin logged in; `422` no token sent.

### `DELETE /admin/admins/me`
- **Description:** Delete the current admin's account.
- **Body:** `delete_admin <password>`
- **Requires token:** Yes
- **Error:** `403` wrong password.

---

## Branches

### `POST /admin/branches`
- **Body:** `create_branch "<branch_name>"`
- **Requires token:** Yes
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "branch_id": "3" } ] }`

### `GET /admin/branches`
- **Body:** `list_branches`
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "branches": [ { "branch_id": "1", "branch_name": "Central" } ] } ] }`

### `GET /admin/branches/{branch_id}/dashboard`
- **Body:** `branch_dashboard <branch_id>`
- **Success (200):**
  ```json
  { "ok": true, "message": "...", "data": [
      { "branch_name": "Central" }, { "active_account_cnt": "12" },
      { "pending_request_cnt": "3" }, { "rejected_today_cnt": "1" }
  ] }
  ```

### `GET /admin/branches/{branch_id}/account-requests`
- **Description:** List a branch's account-opening requests.
- **Body:** `list_requests <branch_id>`
- **Success (200):**
  ```json
  { "ok": true, "message": "...", "data": [
      { "requests": [
          { "request_id": "2001", "request_owner": "0011234567", "branch_id": "1",
            "request_time": "...", "request_status": "pending" }
      ] }
  ] }
  ```

### `POST /admin/account-requests/{request_id}/approve`
- **Body:** `approve_request <request_id>`
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "request_id": "2001" } ] }`

### `POST /admin/account-requests/{request_id}/reject`
- **Body:** `reject_request <request_id> <reason>`
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "request_id": "2001" } ] }`

---

## Accounts & Financial Operations

### `GET /admin/accounts`
- **Body:** `list_accounts`
- **Success (200):**
  ```json
  { "ok": true, "message": "...", "data": [
      { "accounts": [
          { "account_id": "1000-...-0001", "branch_id": "1", "active": "true", "balance": "100000.00" }
      ] }
  ] }
  ```

### `POST /admin/accounts`
- **Description:** Directly create an account as an admin (bypassing the request/approval flow).
- **Body:** `create_account <branch_id> <account_password>`
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "account_id": "1000-..." } ] }`

### `PATCH /admin/accounts/{account_id}/status`
- **Body:** `set_account_status <account_id> <active|inactive>`
- **Error:** `404`/`422` account not found.

### `DELETE /admin/accounts/{account_id}`
- **Description:** Close or fully delete an account (two different commands on the same route).
- **Body:** `close_account_op <account_id> <password>` **or** `delete_account_op <account_id> <password>`
- **Error:** `403` wrong password.

### `POST /admin/accounts/{account_id}/deposits`
- **Body:** `deposit <account_id> <amount>`
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "transaction_id": "1005" }, { "new_balance": "..." } ] }`

### `POST /admin/accounts/{account_id}/withdrawals`
- **Body:** `withdraw <account_id> <amount> <account_password>`
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "transaction_id": "1006" }, { "new_balance": "..." } ] }`
- **Error:** `422` insufficient funds.

### `POST /admin/transfers`
- **Body:** `transfer <from_account> <to_account> <amount> <from_account_password>`
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "transaction_id": "1007" }, { "new_balance": "..." } ] }`

### `GET /admin/accounts/{account_id}/balance`
- **Body:** `get_balance <account_id>`
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "balance": "..." }, { "active": "true" }, { "branch_id": "1" } ] }`

### `GET /admin/accounts/{account_id}/transactions` and `GET /admin/accounts/{account_id}/history`
- **Description:** Both routes run the same underlying command (the second is an alias of the first).
- **Body:** `get_history <account_id>`
- **Success (200):** Same shape as `history` in the user section.

### `GET /admin/transfers/all`
- **Description:** Details of a specific transaction by its ID.
- **Body:** `get_transaction <transaction_id>`
- **Success (200):**
  ```json
  { "ok": true, "message": "...", "data": [
      { "transaction_id": "1007" }, { "transaction_time": "..." }, { "transaction_type": "transfer" },
      { "transaction_origin": "..." }, { "transaction_destination": "..." },
      { "transaction_amount": "..." }, { "new_balance": "..." }
  ] }
  ```

### `DELETE /admin/history`
- **Description:** Clear the transaction history of an account.
- **Body:** `clear_history <account_id>`
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "account_id": "..." } ] }`

---

## Paya Transfers, Fees, Rankings, System Reset

### `GET /admin/transfers/paya`
- **Body:** `list_paya_requests`
- **Success (200):**
  ```json
  { "ok": true, "message": "...", "data": [
      { "payas": [
          { "source": "1000-...", "destination_iban": "IR...", "amount": "200000",
            "status": "pending", "paya_id": "3001" }
      ] }
  ] }
  ```

### `POST /admin/transfers/paya/{paya_id}/approve`
- **Body:** `approve_paya <paya_id>`
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "paya_id": "3001" } ] }`

### `POST /admin/transfers/paya/{paya_id}/reject`
- **Description:** Reject a Paya request; the amount is refunded to the source account.
- **Body:** `reject_paya <paya_id>`
- **Success (200):** `{ "ok": true, "message": "...", "data": [] }`

### `PUT /admin/fees`
- **Body:** `set_transfer_fee <amount>` **or** `set_balance_inquiry_fee <amount>`
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "transfer_fee": "1000" } ] }` (or `balance_inquiry_fee`)

### `GET /admin/fees`
- **Body:** `show_fees`
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "transfer_fee": "1000" }, { "balance_inquiry_fee": "500" } ] }`

### `GET /admin/rankings`
- **Body:** `show_ranking`
- **Success (200):**
  ```json
  { "ok": true, "message": "...", "data": [
      { "ranking": [ { "rank": "1", "codeMelli": "0011234567", "score": "150", "level": "Gold" } ] }
  ] }
  ```

### `POST /admin/system/reset`
- **Description:** Fully reset the system (all data is wiped).
- **Body:** `reset_all <yes/confirmation>`
- **Success (200):** `{ "ok": true, "message": "...", "data": [ { "status": "reset" } ] }`

---

## Unknown Routes (404)

Any request to a path or method not listed above receives the following
response with HTTP status `404`:
```json
{ "ok": false, "error": "API not found" }
```
