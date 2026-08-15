import requests
class AdminClient:
    def __init__(self, baseLink: str = "http://127.0.0.1:47001"):
        self.baseLink = baseLink
        self.token = None

    def _request(self, method: str, endpoint: str, jsonData: dict = None, params: dict = None) -> dict:
        link  = f"{self.baseLink}{endpoint}"
        headers = {"Content-Type": "application/json"}

        if self.token:
            headers["Authorization"] = f"Bearer {self.token}"

        try:
            response = requests.request(
                method=method.upper(),
                url=link,
                headers=headers,
                json=jsonData,
                params=params,
                timeout=15 #این توی خود کتابخونه ریکوست هستش -----> اگه به مشکل بخوره میره توی اکسپشن
            )

            try:
                data = response.json()
            except ValueError:
                data = {"ok": False, "error": response.text}

            if response.status_code != 200:
                return {
                    "ok": False,
                    "status_code": response.status_code,
                    "error": data.get("error", f"HTTP {response.status_code}")
                }

            return data

        except requests.exceptions.RequestException as e:
            return {"ok": False, "error": f"Connection error: {str(e)}"}

    def login(self, username: str, password: str) -> dict:
        data = {"username": username, "password": password}
        response = self._request("POST", "/admin/auth/login", jsonData=data)

        if response.get("ok") and "data" in response:
            data_field = response["data"]
            if isinstance(data_field, list) and len(data_field) > 0:
                self.token = data_field[0].get("token")
            elif isinstance(data_field, dict):
                self.token = data_field.get("token")
        return response

    def create_branch(self, name: str, address: str) -> dict:
        return self._request("POST", "/admin/branches", jsonData={"name": name, "address": address})

    def get_branches(self) -> dict:
        return self._request("GET", "/admin/branches")

    def get_all_accounts(self) -> dict:
        return self._request("GET", "/admin/accounts")

    def get_account_requests(self, branch_id: int = None) -> dict:
        if branch_id:
            return self._request("GET", f"/admin/branches/{branch_id}/account-requests")
        #if you dont send anything itll show all
        return self._request("GET", "/admin/accounts")

    def approve_account_request(self, request_id: int) -> dict:
        return self._request("POST", f"/admin/account-requests/{request_id}/approve")

    def reject_account_request(self, request_id: int) -> dict:
        return self._request("POST", f"/admin/account-requests/{request_id}/reject")

    def paya_request(self) -> dict:
        return self._request("GET", "/admin/transfers/paya")

    def accept_paya(self, paya_id: int) -> dict:
        return self._request("POST", f"/admin/transfers/paya/{paya_id}/approve")

    def reject_paya(self, paya_id: int) -> dict:
        return self._request("POST", f"/admin/transfers/paya/{paya_id}/reject")


class UserClient:
    def __init__(self, baseLink: str = "http://127.0.0.1:47002"):
        self.baseLink = baseLink
        self.token = None

    def _request(self, method: str, endpoint: str, jsonData: dict = None, params: dict = None) -> dict:
        link  = f"{self.baseLink}{endpoint}"
        headers = {"Content-Type": "application/json"}

        if self.token:
            headers["Authorization"] = f"Bearer {self.token}"

        try:
            response = requests.request(
                method=method.upper(),
                url=link ,
                headers=headers,
                json=jsonData,
                params=params,
                timeout=15 #این توی خود کتابخونه ریکوست هستش -----> اگه به مشکل بخوره میره توی اکسپشن
            )

            try:
                data = response.json()
            except ValueError:
                data = {"ok": False, "error": response.text}

            if response.status_code != 200:
                return {
                    "ok": False,
                    "status_code": response.status_code,
                    "error": data.get("error", f"HTTP {response.status_code}")
                }

            return data

        except requests.exceptions.RequestException as e:
            return {"ok": False, "error": f"Connection error: {str(e)}"}

    def signup(self, national_id: str, name: str, family: str, birth_date: str, username: str, password: str) -> dict:
        data = {
            "national_id": national_id,
            "name": name,
            "family": family,
            "birth_date": birth_date,
            "username": username,
            "password": password
        }
        return self._request("POST", "/auth/signup", jsonData=data)

    def login(self, username: str, password: str) -> dict:
        data = {"username": username, "password": password}
        response = self._request("POST", "/auth/login", jsonData=data)

        if response.get("ok") and "data" in response:
            data_field = response["data"]
            if isinstance(data_field, list) and len(data_field) > 0:
                self.token = data_field[0].get("token")
            elif isinstance(data_field, dict):
                self.token = data_field.get("token")
        return response

    def logout(self) -> dict:
        response = self._request("DELETE", "/auth/session")
        if response.get("ok"):
            self.token = None
        return response

    def get_branches(self) -> dict:
        return self._request("GET", "/branches")

    def create_account_request(self, branch_id: int) -> dict:
        return self._request("POST", "/accounts/requests", jsonData={"branch_id": branch_id})

    def get_account_requests(self) -> dict:
        return self._request("GET", "/accounts/requests")

    def cancel_account_request(self, request_id: int) -> dict:
        return self._request("DELETE", f"/accounts/requests/{request_id}")

    def get_my_accounts(self) -> dict:
        return self._request("GET", "/accounts")

    def deposit(self, account_id: int, amount: float, password: str) -> dict:
        return self._request("POST", f"/accounts/{account_id}/deposits", jsonData={"amount": amount, "password": password})

    def withdraw(self, account_id: int, amount: float, password: str) -> dict:
        return self._request("POST", f"/accounts/{account_id}/withdrawals", jsonData={"amount": amount, "password": password})

    def Transfer(self, from_card: str, to_card: str, amount: float, password: str) -> dict:
        data = {
            "from_card": from_card,
            "to_card": to_card,
            "amount": amount,
            "password": password
        }
        return self._request("POST", "/transfers/card-to-card", jsonData=data)

    def paya_transfer(self, from_account_id: int, to_iban: str, amount: float, password: str) -> dict:
        data = {
            "from_account_id": from_account_id,
            "to_iban": to_iban,
            "amount": amount,
            "password": password
        }
        return self._request("POST", "/transfers/paya", jsonData=data)

    def get_balance(self, account_id: int, password: str) -> dict:
        return self._request("POST", f"/accounts/{account_id}/balance-inquiries", jsonData={"password": password})

    def get_account_statement(self, account_id: int) -> dict:
        return self._request("GET", f"/accounts/{account_id}/statement")


# ============================================

def UserMenu(client: UserClient):
    while True:
        print("="*15+"User Menu"+"="*21)
        print("1. Get Branches")
        print("2. Create Account Request")
        print("3. View My Account Requests")
        print("4. Deposit")
        print("5. Withdraw")
        print("6. Card to Card Transfer")
        print("7. Paya Transfer")
        print("8. View My Accounts")
        print("9. Get Account Balance")
        print("10.Get Account Statement")
        print("11.Logout")
        print("0. Back")
        print("="*50)

        req = input("Enter your request: ").strip()

        if req == "1":
            print(client.get_branches())

        elif req == "2":
            try:
                branch_id = int(input("Branch ID: "))
                print(client.create_account_request(branch_id))
            except ValueError:
                print("Please enter a valid number.")

        elif req == "3":
            print(client.get_account_requests())

        elif req == "4":
            try:
                acc_id = int(input("Account ID: "))
                amount = float(input("Amount: "))
                password = input("Password: ")
                print(client.deposit(acc_id, amount, password))
            except ValueError:
                print("Invalid input.")

        elif req == "5":
            try:
                acc_id = int(input("Account ID: "))
                amount = float(input("Amount: "))
                password = input("Password: ")
                print(client.withdraw(acc_id, amount, password))
            except ValueError:
                print("Invalid input.")

        elif req == "6":
            from_card = input("From Card: ")
            to_card = input("To Card: ")
            amount = float(input("Amount: "))
            password = input("Password: ")
            print(client.Transfer(from_card, to_card, amount, password))

        elif req == "7":
            try:
                from_acc = int(input("From Account ID: "))
                to_iban = input("To IBAN: ")
                amount = float(input("Amount: "))
                password = input("Password: ")
                print(client.paya_transfer(from_acc, to_iban, amount, password))
            except ValueError:
                print("Invalid input.")

        elif req == "8":
            print(client.get_my_accounts())

        elif req == "9":
            try:
                acc_id = int(input("Account ID: "))
                password = input("Password: ")
                print(client.get_balance(acc_id, password))
            except ValueError:
                print("Invalid input.")

        elif req == "10":
            try:
                acc_id = int(input("Account ID: "))
                print(client.get_account_statement(acc_id))
            except ValueError:
                print("Invalid input.")

        elif req == "11":
            print(client.logout())
            print("Logged out...")
            break
        elif req == "0":
            break
        else:
            print("Invalid!!")

#==========================================================

def admin_menu(client: AdminClient):
    while True:
        print("="*15+"Admin Menu"+"="*21)
        print("1. Create New Branch")
        print("2. View All Branches")
        print("3. View All Accounts")
        print("4. View Account Requests")
        print("5. Accepting Account Request")
        print("6. Reject Account Request")
        print("7. View Paya Requests")
        print("8. Accepting Paya Request")
        print("9. Reject Paya Request")
        print("10.Logout")
        print("0. Back")
        print("="*50)

        req = input("Enter your request: ").strip()
        if req == "1":
            name = input("Branch Name: ")
            address = input("Branch Address: ")
            print(client.create_branch(name, address))

        elif req == "2":
            print(client.get_branches())

        elif req == "3":
            print(client.get_all_accounts())

        elif req == "4":
            branch = input("Branch ID : ").strip()     # اگه خالی بزاره کلشو میده
            if branch:
                branch_id = int(branch)
            else:
                branch_id = None
            print(client.get_account_requests(branch_id))

        elif req == "5":
            try:
                req_id = int(input("Request ID: "))
                print(client.approve_account_request(req_id))
            except ValueError:
                print("Please enter a valid number.")
        elif req == "6":
            try:
                req_id = int(input("Request ID: "))
                print(client.reject_account_request(req_id))
            except ValueError:
                print("Please enter a valid number.")
        
        elif req == "7":
            print(client.paya_request())

        elif req == "8":
            try:
                paya_id = int(input("Paya ID: "))
                print(client.accept_paya(paya_id))
            except ValueError:
                print("Please enter a valid number.")

        elif req == "9":
            try:
                paya_id = int(input("Paya ID: "))
                print(client.reject_paya(paya_id))
            except ValueError:
                print("Please enter a valid number.")

        elif req == "10":
            print("Logged out...")
            break

        elif req == "0":
            break
        else:
            print("Invalid!!")


#=========================================================

def main():
    Admin_Client = AdminClient()
    User_Client = UserClient()
    while True:
        print("="*15+"Banking System"+"="*21)
        print("1. User")
        print("2. Admin")
        print("0. Exit")
        print("="*50)
        Req = input("Enter to system: ").strip()
        if Req == "1":
            while True:
                print("\n--- User Section ---")
                print("1. Signup")
                print("2. Login")
                print("0. Back")
                sub = input("Enter : ").strip()
                if sub == "1":
                    data = {
                        "national_id": input("National ID: "),
                        "name": input("Name: "),
                        "family": input("Family: "),
                        "birth_date": input("Birth Date (YYYY-MM-DD): "),
                        "username": input("Username: "),
                        "password": input("Password: ")
                    }
                    print(User_Client.signup(**data))
                elif sub == "2":
                    username = input("Username: ")
                    password = input("Password: ")
                    result = User_Client.login(username, password)
                    if result.get("ok"):
                        print("Login successful. Entering User Menu...") 
                        #after login
                        UserMenu(User_Client)
                        break
                    else:
                        print("Login failed. \n Your Username or Password might be wrong, please try again !!")
                elif sub == "0":
                    break

        elif Req == "2":
            username = input("Admin Username: ")
            password = input("Admin Password: ")
            result = Admin_Client.login(username, password)
            if result.get("ok"):
                print("Admin login successful. Entering Admin Menu...")
                #after login
                admin_menu(Admin_Client)
            else:
                print("Login failed!!!")

        elif Req == "0":
            print("Exiting...")
            break
        else:
            print("Invalid!!")


if __name__ == "__main__":
    main()
