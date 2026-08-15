#include <bits/stdc++.h>
#include <unistd.h>
//#include "../include/unistd.h"
#include <sys/wait.h>
//#include "../include/src/sys/sys/wait.h"
#include <sstream>
#include <stdexcept>
#include "../include/picosha2.h"
#include "../include/json.hpp"
#include <ctime>
#include <fstream>
#include "../include/httplib.h"
using namespace std;
using json = nlohmann::json;

typedef long long ll;
typedef long double ld;

#define fi first
#define se second
#define pii pair <int, int>
#define pll pair <ll, ll>
#define mt make_tuple
#define mp make_pair
#define all(x) (x).begin(), (x).end()

const int TIMEOUT_MINUTES = 15;

string runAdmin(const vector<string>& inputs){
    if(inputs.size() == 0){
        return "";
    }
    int inpipe[2];
    int outpipe[2];
    if(pipe(inpipe) == -1 || pipe(outpipe) == -1){
        throw runtime_error("Pipe failed");
    }
    pid_t pid = fork();
    if(pid == -1){
        throw runtime_error("Fork failed");
    }
    if(pid == 0){
        dup2(inpipe[0], STDIN_FILENO);
        dup2(outpipe[1], STDOUT_FILENO);
        close(inpipe[1]);
        close(outpipe[0]);
        close(inpipe[0]);
        close(outpipe[1]);
        execl("./src/admin", "admin", NULL);
        _exit(1);
    }
    close(inpipe[0]);
    close(outpipe[1]);
    ostringstream ss;
    for(int i = 0, sz = (int)inputs.size(); i < sz; i++){
        ss << inputs[i];
        if (i < inputs.size() - 1) {
            ss << " ";
        }
    }
    ss << "\n";
    string payload = ss.str();
    write(inpipe[1], payload.c_str(), payload.size());
    close(inpipe[1]); 
    string result;
    char buffer[4096];
    ssize_t count;
    while ((count = read(outpipe[0], buffer, sizeof(buffer))) > 0){
        result.append(buffer, count);
    }
    close(outpipe[0]);
    waitpid(pid, nullptr, 0);
    return result;
}
vector<string> Translate(const string &result, bool Space = 0){
    vector<string> Res;
    Res.push_back("");
    for(auto c : result){
        if(c > 32){
            Res[Res.size() - 1] += c;
        }
        else if(c == 32 && Space){
            Res[Res.size() - 1] += c;
        }
        else if(Res.back() != ""){
            Res.push_back("");
        }
    }
    while(Res.back() == ""){
        Res.pop_back();
    }
    return Res;
}
bool Find(string &result, string Str){
    return (result.find(Str) != string::npos);
}
int Status(string result){
    if(!Find(result, "Error")){
        return 200;
    }
    else if(Find(result, "Unknown")){
        return 400;
    }
    else if(Find(result, "Invalid OTP")){
        return 403;
    }
    else if(Find(result, "Invalid") || Find(result, "Amount must be positive")){
        return 400;
    }
    else if(Find(result, "No user logged in") || Find(result, "No OTP request yet")){
        return 401;
    }
    else if(Find(result, "Wrong") || Find(result, "Unauthorized request") || 
    Find(result, "does not belong") || Find(result, "") || Find(result, "is inactive")){
        return 403;
    }
    else if(Find(result, "not found") || Find(result, "no account exists with this IBAN") || 
    Find(result, "No branches available") || Find(result, "Couldn't find the request") || 
    Find(result, "The branch doesnt exist") || Find(result, "is not") || Find(result, "") || 
    Find(result, "not cancellable") || Find(result, "Insufficient funds") || Find(result, "Account balance is positive") || 
    Find(result, "limit exceeded") || Find(result, "User has accounts") || Find(result, "OTP expired")){         
        return 422;                                     
    }
    else if(Find(result, "already")){
        return 409;
    }
    return 400;
}

string Hasher(string pass){
    return picosha2::hash256_hex_string(pass);
}
bool compare(string pass, string input){
    input = picosha2::hash256_hex_string(input);
    if(pass==input)
        return true ;
    return false ; 
}
struct Admin{
    string codeMelli, Hashpass;

    Admin (string codeMelli = "", string Hashpass = ""){
        this -> codeMelli = codeMelli;
        this -> Hashpass = Hashpass;
    }

    ~Admin (){}
};
struct Session{
    pair<string, string> user_pass;
    chrono::steady_clock::time_point last_activity;

    Session (pair<string, string> user_pass = mp("", ""), chrono::steady_clock::time_point last_activity = chrono::steady_clock::now()){
        this -> user_pass = user_pass;
        this -> last_activity = last_activity;
    }

    ~Session (){}
};
class Admin_Manager{
    private:
        vector<Admin> Admins;
        map<string, Session> active_sessions;
    
        int AdminIDX(string &codeMelli, string Hashpass){
            for(int i = 0, sz = (int)Admins.size(); i < sz; i++){
                if(Admins[i].codeMelli == codeMelli){
                    if(Admins[i].Hashpass == Hashpass){
                        return i;
                    }
                    return -2;
                }
            }
            return -1;
        }
        string generate_token(){
            const string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> dis(0, chars.size() - 1);
            string token;
            for(int i = 0; i < 32; i++){
                token += chars[dis(gen)];
            }
            return token;
        }
    public:
        Admin_Manager (){
            read_admins();
        }

        int is_authorized(const httplib::Request& req, int Admin_idx){
            if(req.has_header("Authorization") == 0 || Admin_idx != -1){
                return false;
            }
            string auth_header = req.get_header_value("Authorization");
            if(auth_header.find("Bearer ") != 0){
                return false;
            }
            string token = auth_header.substr(7);
            auto it = active_sessions.find(token);
            if(it == active_sessions.end()){
                return false;
            }
            if(active_sessions[token].user_pass.first != Admins[Admin_idx].codeMelli){
                return false;
            }
            auto now = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::minutes>(now - it->second.last_activity).count();
            if(elapsed > TIMEOUT_MINUTES){
                active_sessions.erase(it);
                return false;
            }
            it->second.last_activity = now;
            return true;
        }
        bool SignUp(int Admin_idx, string &codeMelli, string &pass){
            if(Admin_idx == -1 && Admins.empty() == 0){
                return false;
            }
            Admins.push_back(Admin (codeMelli, Hasher(pass)));
            write_admins();
            return true;
        }
        pair<int, string> Login(int &Admin_idx, string &codeMelli, string &pass){
            if(Admin_idx != -1){
                return mp(0, "");
            }
            int Tmp = AdminIDX(codeMelli, Hasher(pass));
            if(Tmp == -1){
                return mp(1, "");
            }
            else if(Tmp == -2){
                return mp(2, "");
            }
            Admin_idx = Tmp;
            string token = generate_token();
            active_sessions[token] = Session (mp(codeMelli, pass));
            return mp(3, token);
        }
        int Logout(const httplib::Request& req, int &Admin_idx){
            if(Admin_idx == -1){
                return 0;
            }
            if(req.has_header("Authorization")){
                string auth_header = req.get_header_value("Authorization");
                if(auth_header.find("Bearer ") == 0){
                    string token = auth_header.substr(7);
                    if(active_sessions.find(token) != active_sessions.end()){
                        if(active_sessions[token].user_pass.first != Admins[Admin_idx].codeMelli){
                            active_sessions.erase(token);
                            return 4;
                        }
                        return 3;
                    }
                    return 2;
                }
                return 1;
            }
            return 1;
        }
        int DeleteAdmin(const httplib::Request& req, int &Admin_idx, string &pass){
            if(Admin_idx == -1){
                return 0;
            }
            if(!compare(Admins[Admin_idx].Hashpass, pass)){
                return -1;
            }
            int Tmp = Admin_idx;
            int Tmp2 = Logout(req, Admin_idx);
            if(Tmp2 == 4){
                Admins.erase(Admins.begin() + Tmp);
            }
            return Tmp2;
        }

        ~Admin_Manager(){
            write_admins();
        }

    void read_admins() {
        ifstream inFile("data/Admins.json");
        if(!inFile.is_open()){
            return;
        }
        json j;
        inFile >> j;
        if(j.contains("admins")){
            for(auto &adminn : j["admins"]){
                Admin A;
                A.codeMelli = adminn["codeMelli"];
                A.Hashpass = adminn["pass"];
                Admins.push_back(A);
            }
        }
        inFile.close();
    }
    void write_admins() {
        json j;
        json jAdmins = json::array();
        for(auto &adminn : Admins){
            jAdmins.push_back({{"codeMelli", adminn.codeMelli}, {"pass", adminn.Hashpass}});
        }
        j["admins"] = jAdmins;
        ofstream inFile("data/Admins.json");
        inFile << j.dump(4);
        inFile.close();
    }
};
void Set_Response_Admin(const httplib::Request& req, httplib::Response& res, Admin_Manager &AM, int &Admin_idx){
    vector<string> payload = Translate(req.body);
    json response;
    if(payload.empty()){
        res.status = 200;
        return;
    }
    if(payload[0] == "signup"){               // inputs: codeMelli, password, note: Admin_idx != -1 is intended
        if(!AM.SignUp(Admin_idx, payload[1], payload[2])){
            response["ok"] = false;
            response["error"] = "Error: Bank has admins, only an admin can add another admin.";
            res.status = 403;
            res.set_content(response.dump(), "application/json");
            return;
        }
        response["ok"] = true;
        response["message"] = "Admin created.";
        res.status = 200;
        res.set_content(response.dump(), "application/json");
        return;
    }
    else if(payload[0] == "login"){           // inputs: codeMelli, password                         
        auto [E, token] = AM.Login(Admin_idx, payload[1], payload[2]);
        if(E == 0){
            response["ok"] = false;
            response["error"] = "Error: Admin already logged in.";
            res.status = 409;
            res.set_content(response.dump(), "application/json");
            return;
        }
        else if(E == 1){
            response["ok"] = false;
            response["error"] = "Error: Admin not found.";
            res.status = 422;
            res.set_content(response.dump(), "application/json");
            return;
        }
        else if(E == 2){
            response["ok"] = false;
            response["error"] = "Error: Wrong password.";
            res.status = 403;
            res.set_content(response.dump(), "application/json");
            return;
        }
        response["ok"] = true;
        response["message"] = "Logged in.";
        json jData = json::array();
        jData.push_back({"token", token});
        response["data"] = jData;
        res.status = 200;
        res.set_content(response.dump(), "application/json");
        return;
    }
    else if(payload[0] == "logout"){          // inputs: none          note: needs token
        int E = AM.Logout(req, Admin_idx);
        if(E == 0){
            response["ok"] = false;
            response["error"] = "Error: No admin logged in.";
            res.status = 401;
            res.set_content(response.dump(), "application/json");
            return;
        }
        else if(E == 1){
            response["ok"] = false;
            response["error"] = "Error: No token found.";
            res.status = 422;
            res.set_content(response.dump(), "application/json");
            return;
        }
        else if(E == 2){
            response["ok"] = false;
            response["error"] = "Error: Invalid token.";
            res.status = 400;
            res.set_content(response.dump(), "application/json");
            return;
        }
        else if(E == 3){
            response["ok"] = false;
            response["error"] = "Error: Wrong token.";
            res.status = 403;
            res.set_content(response.dump(), "application/json");
            return;
        }
        response["ok"] = true;
        response["message"] = "Logged out.";
        res.status = 200;
        res.set_content(response.dump(), "application/json");
        return;
    }
    else if(payload[0] == "delete_admin"){    // inputs: password      note: needs token
        int E = AM.DeleteAdmin(req, Admin_idx, payload[1]);
        if(E == 0){
            response["ok"] = false;
            response["error"] = "Error: No admin logged in.";
            res.status = 401;
            res.set_content(response.dump(), "application/json");
            return;
        }
        else if(E == -1){
            response["ok"] = false;
            response["error"] = "Error: Wrong password.";
            res.status = 403;
            res.set_content(response.dump(), "application/json");
            return;
        }
        else if(E == 1){
            response["ok"] = false;
            response["error"] = "Error: No token found.";
            res.status = 422;
            res.set_content(response.dump(), "application/json");
            return;
        }
        else if(E == 2){
            response["ok"] = false;
            response["error"] = "Error: Invalid token.";
            res.status = 400;
            res.set_content(response.dump(), "application/json");
            return;
        }
        else if(E == 3){
            response["ok"] = false;
            response["error"] = "Error: Wrong token.";
            res.status = 403;
            res.set_content(response.dump(), "application/json");
            return;
        }
        response["ok"] = true;
        response["message"] = "Admin deleted.";
        res.status = 200;
        res.set_content(response.dump(), "application/json");
        return;
    }
    int Token_status = AM.is_authorized(req, Admin_idx);
    if(Admin_idx != -1 && Token_status != 1){
        if(Token_status == -1){
            res.status = 401;
            response["ok"] = false;
            response["error"] = "Error: No token.";
            res.set_content(response.dump(), "application/json");
            return;
        }
        res.status = 403;
        response["ok"] = false;
        response["error"] = "Error: Wrong token.";
        res.set_content(response.dump(), "application/json");
        return;
    }
    string result = runAdmin(payload);
    res.status = Status(result);
    vector<string> Res = Translate(result), Res2 = Translate(result, 1);
    if(res.status == 200){
        response["ok"] = true;
        response["message"] = result;
        json jData = json::array();
        if(payload[0] == "create_branch"){
            jData.push_back({"branch_id", Res[3]});
        }
        else if(payload[0] == "list_branches"){
            json jBranches = json::array();
            for(auto B : Res2){
                vector<string> Tmp = Translate(B);
                jBranches.push_back({{"branch_id", Tmp[0]}, {"branch_name", Tmp[2]}});
            }
            jData.push_back({"branches", jBranches});
        }
        else if(payload[0] == "branch_dashboard"){
            jData.push_back({"branch_name", Res[1]});
            jData.push_back({"active_account_cnt", Res[4]});
            jData.push_back({"pending_request_cnt", Res[7]});
            jData.push_back({"rejected_today_cnt", Res[10]});
        }
        else if(payload[0] == "list_requests"){
            json jRequests = json::array();
            for(auto R : Res2){
                vector<string> Tmp = Translate(R);
                jRequests.push_back({{"request_id", Tmp[0]}, {"request_owner", Tmp[3]}, {"branch_id", Tmp[6]}, 
                {"request_time", Tmp[9]}, {"request_status", Tmp[10]}});
            }
            jData.push_back({"requests", jRequests});
        }
        else if(payload[0] == "approve_request"){
            jData.push_back({"request_id", Res[1]});
        }
        else if(payload[0] == "reject_request"){
            jData.push_back({"request_id", Res[1]});
        }
        else if(payload[0] == "create_account"){
            jData.push_back({"account_id", Res[3]});
        }
        else if(payload[0] == "list_accounts"){
            json jAccounts = json::array();
            for(auto A : Res2){
                vector<string> Tmp = Translate(A);
                jAccounts.push_back({{"account_id", Tmp[0]}, {"branch_id", Tmp[3]}, {"active", Tmp[6]}, 
                {"balance", Tmp[9]}});
            }
            jData.push_back({"accounts", jAccounts});
        }
        else if(payload[0] == "set_transfer_fee"){
            jData.push_back({"transfer_fee", Res[4]});
        }
        else if(payload[0] == "set_balance_inquiry_fee"){
            jData.push_back({"balance_inquiry_fee", Res[5]});
        }
        else if(payload[0] == "show_fees"){
            jData.push_back({"transfer_fee", Res[2]});
            jData.push_back({"balance_inquiry_fee", Res[6]});
        }
        else if(payload[0] == "deposit"){
            jData.push_back({"transaction_id", Res[2]}); 
            jData.push_back({"new_balance", Res[5]});
        }
        else if(payload[0] == "withdraw"){
            jData.push_back({"transaction_id", Res[2]});
            jData.push_back({"new_balance", Res[5]});
        }
        else if(payload[0] == "transfer"){
            jData.push_back({"transaction_id", Res[2]});
            jData.push_back({"new_balance", Res[5]});
        }
        else if(payload[0] == "show_ranking"){
            json jRanking = json::array();
            for(auto U : Res2){
                vector<string> Tmp = Translate(U);
                jRanking.push_back({{"rank", Tmp[0]}, {"codeMelli", Tmp[2]}, {"score", Tmp[4]}, {"level", Tmp[6]}});
            }
            jData.push_back({"ranking", jRanking});
        }
        else if(payload[0] == "get_balance"){
            int sz = (int)Res.size();
            if(sz == 6){
                jData.push_back({"balance", Res[1]});
                jData.push_back({"active", Res[3]}); 
                jData.push_back({"branch_id", Res[5]});
            }
            else{
                jData.push_back({"balance", Res[sz - 5]});
                jData.push_back({"balance_inquiry_free", Res[sz - 7]});
                jData.push_back({"active", Res[sz - 3]}); 
                jData.push_back({"branch_id", Res[sz - 1]});
            }
        }
        else if(payload[0] == "get_history"){
            json jHistory = json::array();
            for(auto T : Res2){
                vector<string> Tmp = Translate(T);
                jHistory.push_back({{"transaction_id", Tmp[0]}, {"transaction_time", Tmp[2]}, 
                {"transaction_type", Tmp[4]}, {"transaction_amount", Tmp[6]}, {"new_balance", Tmp[9]}});
            }
            jData.push_back({"history", jHistory});
        }
        else if(payload[0] == "get_transaction"){
            jData.push_back({"transaction_id", Res[1]});
            jData.push_back({"transaction_time", Res[3]});
            jData.push_back({"transaction_type", Res[5]});
            jData.push_back({"transaction_origin", Res[7]});
            jData.push_back({"transaction_destination", Res[9]});
            jData.push_back({"transaction_amount", Res[11]});
            jData.push_back({"new_balance", Res[14]});
        }
        else if(payload[0] == "clear_history"){
            jData.push_back({"account_id", Res[3]});
        }
        else if(payload[0] == "reset_all"){
            jData.push_back({"status", Res2.back()});
        }
        else if(payload[0] == "list_paya_requests"){
            json jPaya = json::array();
            for(int i = 0, sz = (int)Res.size(); i < sz; i += 13){
                jPaya.push_back({{"source", Res[i + 2]}, {"destination_iban", Res[i + 5]}, {"amount", Res[i + 7]}, 
                {"status", Res[9]}, {"paya_id", Res[12]}});
            }
            jData.push_back({"payas", jPaya});
        }
        else if(payload[0] == "approve_paya"){
            jData.push_back({"paya_id", Res[4]});
        }
        response["data"] = jData;
    }
    else{
        response["ok"] = false;
        json jError = json::array();
        for(auto E : Res2){
            jError.push_back(E);
        }
        response["error"] = jError;
    }
    res.set_content(response.dump(), "application/json");
}

int main(){
    int compile_status = system("g++ src/admin.cpp -o src/admin");
    if(compile_status != 0){
        cout << "Error: Admin source has some bug" << endl;
        return 1;
    }
    httplib::Server server;                 //http://127.0.0.1:8080
    Admin_Manager AM;
    int Admin_idx = -1;
    
    server.set_error_handler([](const httplib::Request& req, httplib::Response& res){
        if(res.status == 404){
            json response;
            response["ok"] = false;
            response["error"] = "API not found";
            res.set_content(response.dump(), "application/json");
        }
    });
    /*---------------------------Admin handling---------------------------------*/
    server.Post("/admin/auth/signup", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Post("/admin/auth/login", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Delete("/admin/auth/session", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Delete("/admin/admins/me", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    /*---------------------------------------------------------------------------*/
    server.Post("/admin/branches", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Get("/admin/branches", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Get(R"(/admin/branches/(\d+)/dashboard)", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    //list_requests
    server.Get(R"(/admin/branches/(\d+)/requests)", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Get("/admin/accounts", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Post("/admin/accounts", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Patch(R"(/admin/accounts/([^/]+)/status)", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Delete(R"(/admin/accounts/([^/]+))", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Post(R"(/admin/accounts/([^/]+)/deposits)", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Post(R"(/admin/accounts/([^/]+)/withdrawals)", [&AM, &Admin_idx]
    (const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Post("/admin/transfers", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Get(R"(/admin/accounts/([^/]+)/balance)", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Get(R"(/admin/accounts/([^/]+)/transactions)", [&AM, &Admin_idx]
    (const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    // get_history
    server.Get(R"(/admin/accounts/([^/]+)/history)", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Get(R"(/admin/branches/(\d+)/account-requests)", [&AM, &Admin_idx]
    (const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Post(R"(/admin/account-requests/(\d+)/approve)", [&AM, &Admin_idx]
    (const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Post(R"(/admin/account-requests/(\d+)/reject)", [&AM, &Admin_idx]
    (const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Get("/admin/transfers/paya", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    // get_transaction
    server.Get("/admin/transfers/all", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Post(R"(/admin/transfers/paya/(\d+)/approve)", [&AM, &Admin_idx]
    (const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Post(R"(/admin/transfers/paya/(\d+)/reject)", [&AM, &Admin_idx]
    (const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Put("/admin/fees", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Get("/admin/fees", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Get("/admin/rankings", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    //export_history
    server.Put("/admin/statement", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){    
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Delete("/admin/history", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });
    server.Post("/admin/system/reset", [&AM, &Admin_idx](const httplib::Request& req, httplib::Response& res){
        Set_Response_Admin(req, res, AM, Admin_idx);
    });    

    server.listen("127.0.0.1", 8080);
}