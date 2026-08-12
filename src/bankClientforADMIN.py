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


