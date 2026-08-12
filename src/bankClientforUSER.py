import requests

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

