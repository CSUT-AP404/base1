import requests


class BaseClient:
    def __init__(self, baseLink):
        self.baseLink = baseLink.rstrip("/")
        self.token = None

    def _request(self, method, endpoint, payload=""):
        link = f"{self.baseLink}{endpoint}"

        headers = {
            "Content-Type": "text/plain"
        }

        if self.token:
            headers["Authorization"] = f"Bearer {self.token}"

        try:
            response = requests.request(
                method=method.upper(),
                url=link,
                headers=headers,
                data=payload,
                timeout=15
            )

            try:
                data = response.json()
            except ValueError:
                data = {
                    "ok": False,
                    "error": response.text
                }

            if response.status_code != 200:
                return {
                    "ok": False,
                    "status_code": response.status_code,
                    "error": data.get("error", f"HTTP {response.status_code}")
                }

            return data

        except requests.exceptions.RequestException as e:
            return {
                "ok": False,
                "error": f"Connection error: {str(e)}"
            }

    @staticmethod
    def _token_from_response(response):
        if not response.get("ok"):
            return None

        data = response.get("data")

        if isinstance(data, list) and data:
            if isinstance(data[0], dict):
                return data[0].get("token")

        if isinstance(data, dict):
            return data.get("token")

        return None


class AdminClient(BaseClient):
    def __init__(self, baseLink="http://127.0.0.1:8080"):
        super().__init__(baseLink)

    def login(self, codeMelli, password):
        response = self._request(
            "POST",
            "/admin/auth/login",
            f"login {codeMelli} {password}"
        )

        token = self._token_from_response(response)
        if token:
            self.token = token

        return response

    def logout(self):
        response = self._request(
            "DELETE",
            "/admin/auth/session",
            "logout"
        )

        if response.get("ok"):
            self.token = None

        return response

    def signup(self, codeMelli, password):
        return self._request(
            "POST",
            "/admin/auth/signup",
            f"signup {codeMelli} {password}"
        )

    def delete_admin(self, password):
        response = self._request(
            "DELETE",
            "/admin/admins/me",
            f"delete_admin {password}"
        )

        if response.get("ok"):
            self.token = None

        return response

    def create_branch(self, name):
        # admin.cpp expects the branch name inside quotes.
        safe_name = name.replace('"', '\\"')
        return self._request(
            "POST",
            "/admin/branches",
            f'create_branch "{safe_name}"'
        )

    def get_branches(self):
        return self._request(
            "GET",
            "/admin/branches",
            "list_branches"
        )

    def get_branch_dashboard(self, branch_id):
        return self._request(
            "GET",
            f"/admin/branches/{branch_id}/dashboard",
            f"branch_dashboard {branch_id}"
        )

    def get_account_requests(self, branch_id):
        return self._request(
            "GET",
            f"/admin/branches/{branch_id}/account-requests",
            f"list_requests {branch_id}"
        )

    def get_all_accounts(self):
        return self._request(
            "GET",
            "/admin/accounts",
            "list_accounts"
        )

    def create_account(self, branch_id, password):
        return self._request(
            "POST",
            "/admin/accounts",
            f"create_account {branch_id} {password}"
        )

    def approve_account_request(self, request_id):
        return self._request(
            "POST",
            f"/admin/account-requests/{request_id}/approve",
            f"approve_request {request_id}"
        )

    def reject_account_request(self, request_id, reason):
        return self._request(
            "POST",
            f"/admin/account-requests/{request_id}/reject",
            f"reject_request {request_id} {reason}"
        )

    def set_account_status(self, account_id, status):
        # status is expected to be understood by the backend's command layer.
        return self._request(
            "PATCH",
            f"/admin/accounts/{account_id}/status",
            f"set_account_status {account_id} {status}"
        )

    def close_account(self, account_id, password):
        return self._request(
            "DELETE",
            f"/admin/accounts/{account_id}",
            f"close_account_op {account_id} {password}"
        )

    def delete_account(self, account_id, password):
        return self._request(
            "DELETE",
            f"/admin/accounts/{account_id}",
            f"delete_account_op {account_id} {password}"
        )

    def deposit(self, account_id, amount):
        return self._request(
            "POST",
            f"/admin/accounts/{account_id}/deposits",
            f"deposit {account_id} {amount}"
        )

    def withdraw(self, account_id, amount, password):
        return self._request(
            "POST",
            f"/admin/accounts/{account_id}/withdrawals",
            f"withdraw {account_id} {amount} {password}"
        )

    def transfer(self, from_account, to_account, amount, password):
        return self._request(
            "POST",
            "/admin/transfers",
            f"transfer {from_account} {to_account} {amount} {password}"
        )

    def get_balance(self, account_id):
        return self._request(
            "GET",
            f"/admin/accounts/{account_id}/balance",
            f"get_balance {account_id}"
        )

    def get_account_transactions(self, account_id):
        return self._request(
            "GET",
            f"/admin/accounts/{account_id}/transactions",
            f"get_transaction {account_id}"
        )

    def get_account_history(self, account_id):
        return self._request(
            "GET",
            f"/admin/accounts/{account_id}/history",
            f"get_history {account_id}"
        )

    def clear_history(self, account_id):
        return self._request(
            "DELETE",
            "/admin/history",
            f"clear_history {account_id}"
        )

    def get_transaction(self, transaction_id):
        return self._request(
            "GET",
            "/admin/transfers/all",
            f"get_transaction {transaction_id}"
        )

    def get_paya_requests(self):
        return self._request(
            "GET",
            "/admin/transfers/paya",
            "list_paya_requests"
        )

    def approve_paya(self, paya_id):
        return self._request(
            "POST",
            f"/admin/transfers/paya/{paya_id}/approve",
            f"approve_paya {paya_id}"
        )

    def reject_paya(self, paya_id):
        return self._request(
            "POST",
            f"/admin/transfers/paya/{paya_id}/reject",
            f"reject_paya {paya_id}"
        )

    def set_transfer_fee(self, amount):
        return self._request(
            "PUT",
            "/admin/fees",
            f"set_transfer_fee {amount}"
        )

    def set_balance_inquiry_fee(self, amount):
        return self._request(
            "PUT",
            "/admin/fees",
            f"set_balance_inquiry_fee {amount}"
        )

    def show_fees(self):
        return self._request(
            "GET",
            "/admin/fees",
            "show_fees"
        )

    def show_ranking(self):
        return self._request(
            "GET",
            "/admin/rankings",
            "show_ranking"
        )

    def reset_all(self, answer):
        return self._request(
            "POST",
            "/admin/system/reset",
            f"reset_all {answer}"
        )


class UserClient(BaseClient):
    def __init__(self, baseLink="http://127.0.0.1:8081"):
        super().__init__(baseLink)

    def signup(self, national_id, password, **kwargs):
        # The actual user.cpp signup only accepts national code + password.
        return self._request(
            "POST",
            "/auth/signup",
            f"signup {national_id} {password}"
        )

    def login(self, codeMelli, password):
        response = self._request(
            "POST",
            "/auth/login",
            f"login {codeMelli} {password}"
        )

        token = self._token_from_response(response)
        if token:
            self.token = token

        return response

    def logout(self):
        response = self._request(
            "DELETE",
            "/auth/session",
            "logout"
        )

        if response.get("ok"):
            self.token = None

        return response

    def get_branches(self):
        return self._request(
            "GET",
            "/branches",
            "list_branches"
        )

    def create_account_request(self, branch_id):
        return self._request(
            "POST",
            "/accounts/requests",
            f"request_account {branch_id}"
        )

    def get_account_requests(self):
        return self._request(
            "GET",
            "/accounts/requests",
            "my_requests"
        )

    def cancel_account_request(self, request_id):
        return self._request(
            "DELETE",
            f"/accounts/requests/{request_id}",
            f"cancel_request {request_id}"
        )

    def activate_account(self, request_id, password):
        return self._request(
            "PATCH",
            f"/accounts/{request_id}/activation",
            f"activate_account {request_id} {password}"
        )

    def delete_my_account(self, account_id, password):
        return self._request(
            "DELETE",
            f"/accounts/{account_id}",
            f"delete_my_account {account_id} {password}"
        )

    def get_my_accounts(self):
        return self._request(
            "GET",
            "/accounts",
            "my_accounts"
        )

    def deposit(self, account_id, amount):
        return self._request(
            "POST",
            f"/accounts/{account_id}/deposits",
            f"deposit_to {account_id} {amount}"
        )

    def withdraw(self, account_id, amount, password):
        return self._request(
            "POST",
            f"/accounts/{account_id}/withdrawals",
            f"withdraw_from {account_id} {amount} {password}"
        )

    def transfer(self, from_account, to_account, amount, password):
        return self._request(
            "POST",
            "/transfers/card-to-card",
            f"send_money {from_account} {to_account} {amount} {password}"
        )

    # Keep the old method name too.
    def Transfer(self, from_account, to_account, amount, password):
        return self.transfer(from_account, to_account, amount, password)

    def paya_transfer(self, from_account_id, to_iban, amount, password):
        return self._request(
            "POST",
            "/transfers/paya",
            f"paya_transfer {from_account_id} {to_iban} {amount} {password}"
        )

    def get_balance(self, account_id):
        return self._request(
            "POST",
            f"/accounts/{account_id}/balance-inquiries",
            f"balance_inquiry {account_id}"
        )

    def get_account_statement(self, account_id):
        return self._request(
            "GET",
            f"/accounts/{account_id}/statement",
            f"get_history {account_id}"
        )

    def request_otp(self, account_id):
        return self._request(
            "POST",
            "/auth/otp",
            f"request_OTP {account_id}"
        )

    def online_payment(self, from_account, to_account, amount, otp):
        return self._request(
            "POST",
            "/payments/online",
            f"online_payment {from_account} {to_account} {amount} {otp}"
        )

    def show_iban(self, account_id):
        return self._request(
            "GET",
            f"/accounts/{account_id}/iban",
            f"show_iban {account_id}"
        )

    def delete_my_user(self, password):
        response = self._request(
            "DELETE",
            "/users/me",
            f"delete_my_user {password}"
        )

        if response.get("ok"):
            self.token = None

        return response

    def my_rank(self):
        return self._request(
            "GET",
            "/users/me/rank",
            "my_rank"
        )


def print_result(result):
    print(result)
    print()


def UserMenu(client):
    while True:
        print("=" * 15 + "User Menu" + "=" * 21)
        print("1. Get Branches")
        print("2. Create Account Request")
        print("3. View My Account Requests")
        print("4. Activate Account")
        print("5. Deposit")
        print("6. Withdraw")
        print("7. Card to Card Transfer")
        print("8. Paya Transfer")
        print("9. View My Accounts")
        print("10. Get Account Balance")
        print("11. Get Account Statement")
        print("12. Request OTP")
        print("13. Online Payment")
        print("14. Show IBAN")
        print("15. Delete My Account")
        print("16. My Rank")
        print("17. Delete My User")
        print("18. Logout")
        print("0. Back")
        print("=" * 50)

        req = input("Enter your request: ").strip()

        try:
            if req == "1":
                print_result(client.get_branches())

            elif req == "2":
                branch_id = int(input("Branch ID: "))
                print_result(client.create_account_request(branch_id))

            elif req == "3":
                print_result(client.get_account_requests())

            elif req == "4":
                request_id = int(input("Request ID: "))
                password = input("New account password: ")
                print_result(client.activate_account(request_id, password))

            elif req == "5":
                account_id = input("Account ID: ").strip()
                amount = float(input("Amount: "))
                print_result(client.deposit(account_id, amount))

            elif req == "6":
                account_id = input("Account ID: ").strip()
                amount = float(input("Amount: "))
                password = input("Account password: ")
                print_result(client.withdraw(account_id, amount, password))

            elif req == "7":
                from_account = input("From Account: ").strip()
                to_account = input("To Account: ").strip()
                amount = float(input("Amount: "))
                password = input("Account password: ")
                print_result(
                    client.transfer(
                        from_account,
                        to_account,
                        amount,
                        password
                    )
                )

            elif req == "8":
                from_account = input("From Account ID: ").strip()
                to_iban = input("Destination IBAN: ").strip()
                amount = float(input("Amount: "))
                password = input("Account password: ")
                print_result(
                    client.paya_transfer(
                        from_account,
                        to_iban,
                        amount,
                        password
                    )
                )

            elif req == "9":
                print_result(client.get_my_accounts())

            elif req == "10":
                account_id = input("Account ID: ").strip()
                print_result(client.get_balance(account_id))

            elif req == "11":
                account_id = input("Account ID: ").strip()
                print_result(client.get_account_statement(account_id))

            elif req == "12":
                account_id = input("Account ID: ").strip()
                print_result(client.request_otp(account_id))

            elif req == "13":
                from_account = input("From Account: ").strip()
                to_account = input("To Account: ").strip()
                amount = float(input("Amount: "))
                otp = input("OTP: ").strip()
                print_result(
                    client.online_payment(
                        from_account,
                        to_account,
                        amount,
                        otp
                    )
                )

            elif req == "14":
                account_id = input("Account ID: ").strip()
                print_result(client.show_iban(account_id))

            elif req == "15":
                account_id = input("Account ID: ").strip()
                password = input("Account password: ")
                print_result(client.delete_my_account(account_id, password))

            elif req == "16":
                print_result(client.my_rank())

            elif req == "17":
                password = input("User password: ")
                result = client.delete_my_user(password)
                print_result(result)
                if result.get("ok"):
                    break

            elif req == "18":
                print_result(client.logout())
                break

            elif req == "0":
                break

            else:
                print("Invalid!!")

        except ValueError:
            print("Invalid input.")


def admin_menu(client):
    while True:
        print("=" * 15 + "Admin Menu" + "=" * 20)
        print("1. Create New Branch")
        print("2. View All Branches")
        print("3. Branch Dashboard")
        print("4. View Account Requests")
        print("5. View All Accounts")
        print("6. Accept Account Request")
        print("7. Reject Account Request")
        print("8. View Paya Requests")
        print("9. Accept Paya Request")
        print("10. Reject Paya Request")
        print("11. Deposit")
        print("12. Withdraw")
        print("13. Transfer")
        print("14. Get Balance")
        print("15. Get Account History")
        print("16. Get Transaction")
        print("17. Show Fees")
        print("18. Set Transfer Fee")
        print("19. Set Balance Inquiry Fee")
        print("20. Show Ranking")
        print("21. Logout")
        print("0. Back")
        print("=" * 50)

        req = input("Enter your request: ").strip()

        try:
            if req == "1":
                name = input("Branch Name: ")
                print_result(client.create_branch(name))

            elif req == "2":
                print_result(client.get_branches())

            elif req == "3":
                branch_id = int(input("Branch ID: "))
                print_result(client.get_branch_dashboard(branch_id))

            elif req == "4":
                branch_id = int(input("Branch ID: "))
                print_result(client.get_account_requests(branch_id))

            elif req == "5":
                print_result(client.get_all_accounts())

            elif req == "6":
                request_id = int(input("Request ID: "))
                print_result(client.approve_account_request(request_id))

            elif req == "7":
                request_id = int(input("Request ID: "))
                reason = input("Rejection reason (one word): ").strip()
                print_result(
                    client.reject_account_request(
                        request_id,
                        reason
                    )
                )

            elif req == "8":
                print_result(client.get_paya_requests())

            elif req == "9":
                paya_id = int(input("Paya ID: "))
                print_result(client.approve_paya(paya_id))

            elif req == "10":
                paya_id = int(input("Paya ID: "))
                print_result(client.reject_paya(paya_id))

            elif req == "11":
                account_id = input("Account ID: ").strip()
                amount = float(input("Amount: "))
                print_result(client.deposit(account_id, amount))

            elif req == "12":
                account_id = input("Account ID: ").strip()
                amount = float(input("Amount: "))
                password = input("Account password: ")
                print_result(client.withdraw(account_id, amount, password))

            elif req == "13":
                from_account = input("From Account: ").strip()
                to_account = input("To Account: ").strip()
                amount = float(input("Amount: "))
                password = input("Account password: ")
                print_result(
                    client.transfer(
                        from_account,
                        to_account,
                        amount,
                        password
                    )
                )

            elif req == "14":
                account_id = input("Account ID: ").strip()
                print_result(client.get_balance(account_id))

            elif req == "15":
                account_id = input("Account ID: ").strip()
                print_result(client.get_account_history(account_id))

            elif req == "16":
                transaction_id = int(input("Transaction ID: "))
                print_result(client.get_transaction(transaction_id))

            elif req == "17":
                print_result(client.show_fees())

            elif req == "18":
                amount = float(input("Transfer fee: "))
                print_result(client.set_transfer_fee(amount))

            elif req == "19":
                amount = float(input("Balance inquiry fee: "))
                print_result(client.set_balance_inquiry_fee(amount))

            elif req == "20":
                print_result(client.show_ranking())

            elif req == "21":
                print_result(client.logout())
                break

            elif req == "0":
                break

            else:
                print("Invalid!!")

        except ValueError:
            print("Invalid input.")


def main():
    admin_client = AdminClient()
    user_client = UserClient()

    while True:
        print("=" * 15 + "Banking System" + "=" * 21)
        print("1. User")
        print("2. Admin")
        print("0. Exit")
        print("=" * 50)

        req = input("Enter to system: ").strip()

        if req == "1":
            while True:
                print("\n--- User Section ---")
                print("1. Signup")
                print("2. Login")
                print("0. Back")

                sub = input("Enter: ").strip()

                if sub == "1":
                    national_id = input("National ID: ").strip()
                    password = input("Password: ")
                    print_result(
                        user_client.signup(
                            national_id,
                            password
                        )
                    )

                elif sub == "2":
                    code_melli = input("National ID: ").strip()
                    password = input("Password: ")

                    result = user_client.login(
                        code_melli,
                        password
                    )

                    if result.get("ok"):
                        print("Login successful.")
                        UserMenu(user_client)
                        break
                    else:
                        print("Login failed.")
                        print_result(result)

                elif sub == "0":
                    break

                else:
                    print("Invalid!!")

        elif req == "2":
            code_melli = input("Admin National ID: ").strip()
            password = input("Admin Password: ")

            result = admin_client.login(
                code_melli,
                password
            )

            if result.get("ok"):
                print("Admin login successful.")
                admin_menu(admin_client)
            else:
                print("Login failed.")
                print_result(result)

        elif req == "0":
            print("Exiting...")
            break

        else:
            print("Invalid!!")


if __name__ == "__main__":
    main()
