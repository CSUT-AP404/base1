#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>
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

string GetTime(){
    time_t now = time(0);
    tm *ltm = localtime(&now);
    string Time = to_string(1900 + ltm->tm_year) + '-';
    // added +1 to month
    string Month = to_string(ltm->tm_mon + 1); 
    if((int)Month.size() == 1){
        Time += '0';
    }
    Time += Month;
    Time += '-';
    string Day = to_string(ltm->tm_mday);
    if((int)Day.size() == 1){
        Time += '0';
    }
    Time += Day;
    Time += ' ';
    string Hour = to_string(ltm->tm_hour);
    if((int)Hour.size() == 1){
        Time += '0';
    }
    Time += Hour;
    Time += ':';
    string Min = to_string(ltm->tm_min);
    if((int)Min.size() == 1){
        Time += '0';
    }
    Time += Min;
    Time += ':';
    string Sec = to_string(ltm->tm_sec);
    if((int)Sec.size() == 1){
        Time += '0';
    }
    Time += Sec;
    return Time; 
};
string removeDashes(string input) {
    input.erase(remove(input.begin(), input.end(), '-'), input.end());
    return input;
}
struct User {
    vector <int> Request_Ids;
    vector <string> id;
    vector <string> ibans; // save
    string codeMelli;
    string Hashpass;
    int score = 0;
    string signup_time;
    long long OTP_start_time_in_MS = 0;
    int OTP;
    User(){}
    User(string codeMelli, string Hashpass){
        this->codeMelli = codeMelli ;
        this->Hashpass = Hashpass ;
        this->score = 0;
        this->signup_time = GetTime();
    }

    void erase(int idx){
        id.erase(id.begin() + idx);
    }

    bool operator== (const User &U) const{
        return (codeMelli == U.codeMelli);
    }
    bool operator< (const User &U) const{
        if(score != U.score){
            return score > U.score;
        }
        return signup_time < U.signup_time;
    }

    ~User(){}
};

string runUser(const vector<string>& inputs){
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
        execl("./src/user", "user", NULL);
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
    while(!Res.empty() && Res.back() == ""){
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
    Find(result, "does not belong") || Find(result, "is inactive")){
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

bool compare(string pass, string input){
    input = picosha2::hash256_hex_string(input);
    if(pass==input)
        return true ;
    return false ; 
}
struct Session{
    pair<string, string> user_pass;
    chrono::steady_clock::time_point last_activity;

    Session (pair<string, string> user_pass = mp("", ""), chrono::steady_clock::time_point last_activity = chrono::steady_clock::now()){
        this -> user_pass = user_pass;
        this -> last_activity = last_activity;
    }

    ~Session (){}
};
class Token_Manager{
    private:
        vector<User> Users;
        map<string, Session> active_sessions;
    
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
        Token_Manager(){
            read_users();
        }

        int UserIDX(string &codeMelli, string &pass){
            for(int i = 0, sz = (int)Users.size(); i < sz; i++){
                if(Users[i].codeMelli == codeMelli){
                    if(!compare(Users[i].Hashpass, pass)){
                        return -1;
                    }
                    return i;
                }
            }
            return -1;
        }
        int is_authorized(const httplib::Request& req, int &User_idx){
            User_idx = -1;
            if(req.has_header("Authorization") == 0){
                return -1;
            }
            string auth_header = req.get_header_value("Authorization");
            if(auth_header.find("Bearer ") != 0){
                return -1;
            }
            string token = auth_header.substr(7);
            auto it = active_sessions.find(token);
            if(it == active_sessions.end()){
                return 0;
            }
            auto now = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::minutes>(now - it->second.last_activity).count();
            if(elapsed > TIMEOUT_MINUTES){
                active_sessions.erase(it);
                return 0;
            }
            string codeMelli = it->second.user_pass.first;
            for(int i = 0, sz = (int)Users.size(); i < sz; i++){
                if(Users[i].codeMelli == codeMelli){
                    User_idx = i;
                    it->second.last_activity = now;
                    return 1;
                }
            }
            return 0;
        }
        string Login(string &codeMelli, string &pass){       //runs only if login is ok
            string token = generate_token();
            active_sessions[token] = Session (mp(codeMelli, pass));
            return token;
        }
        void Logout(const httplib::Request& req, int id){
            if(req.has_header("Authorization")){
                string auth_header = req.get_header_value("Authorization");
                if(auth_header.find("Bearer ") == 0){
                    string token = auth_header.substr(7);
                    active_sessions.erase(token);
                }
            }
        }

        void UPD(){
            Users.clear();
            read_users();
        }

        ~Token_Manager(){}
    
    void read_users() {
        ifstream inFile("data/Users.json");
        if(!inFile.is_open()){
            return;
        }
        json j;
        inFile >> j;
        if(j.contains("users")){
            for(auto &userr : j["users"]){
                User u;
                u.codeMelli = userr["codeMelli"];
                u.Hashpass = userr["pass"];
                u.score = userr.value("score", 0);
                u.signup_time = userr.value("signup_time", GetTime());
                u.OTP = userr["OTP"];
                u.OTP_start_time_in_MS = userr["OTP_start_time_in_MS"];
                for(auto &iban : userr["ibans"]){
                    u.ibans.push_back(iban);
                }
                for(auto &acc : userr["accounts"]){
                    u.id.push_back(acc);
                }
                for(auto &req : userr["request_ids"]){
                    u.Request_Ids.push_back(req);
                }
                Users.push_back(u);
            }
        }
        inFile.close();
    }
};
void Set_Response_USER(const httplib::Request& req, httplib::Response& res, Token_Manager &TM){
    vector<string> payload = Translate(req.body);
    json response;
    if(payload.empty()){
        res.status = 200;
        return;
    }
    int User_idx = -1;
    int Token_status = TM.is_authorized(req, User_idx);
    if(req.has_header("Authorization") && Token_status != 1){
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
    payload[0] += "_server";
    payload.insert(payload.begin() + 1, to_string(User_idx));
    string result = runUser(payload);
    res.status = Status(result);
    vector<string> Res = Translate(result), Res2 = Translate(result, 1);
    if(res.status == 200){
        response["ok"] = true;
        response["message"] = result;
        json jData = json::array();
        if(payload[0] == "login_server"){
            User_idx = TM.UserIDX(payload[2], payload[3]);
            jData.push_back({"token", TM.Login(payload[2], payload[3])});
        }
        else if(payload[0] == "logout_server"){
            TM.Logout(req, User_idx);
            User_idx = -1;
        }
        else if(payload[0] == "delete_my_user_server"){
            TM.Logout(req, User_idx);
            User_idx = -1;
        }
        else if(payload[0] == "list_branches_server"){
            json jBranches = json::array();
            for(auto B : Res2){
                vector<string> Tmp = Translate(B);
                jBranches.push_back({{"branch_id", Tmp[0]}, {"branch_name", Tmp[2]}});
            }
            jData.push_back({"branches", jBranches});
        }
        else if(payload[0] == "request_account_server"){
            jData.push_back({"request_id", Res[3]});
        }
        else if(payload[0] == "my_requests_server"){
            json jRequests = json::array();
            for(auto R : Res2){
                vector<string> Tmp = Translate(R);
                int sz = (int)Tmp.size();
                if(sz == 9){
                    jRequests.push_back({{"request_id", Tmp[0]}, {"branch_id", Tmp[3]}, {"request_status", Tmp[6]},
                    {"request_time", Tmp[8]}});
                }
                else if(sz == 10){
                    jRequests.push_back({{"request_id", Tmp[0]}, {"branch_id", Tmp[3]}, {"request_status", Tmp[6]},
                    {"request_reason", Tmp[9]}});
                }
            }
            jData.push_back({"requests", jRequests});
        }
        else if(payload[0] == "cancel_request_server"){
            jData.push_back({"request_id", Res[1]});
        }
        else if(payload[0] == "activate_account_server"){
            jData.push_back({"account_id", Res[3]});
        }
        else if(payload[0] == "my_accounts_server"){
            json jAccounts = json::array();
            for(int i = 0, sz = (int)Res.size(); i < sz; i += 3){
                jAccounts.push_back({{"account_id", Res[i]}, {"balance", Res[i + 2]}});
            }
            jData.push_back({"accounts", jAccounts});
        }
        else if(payload[0] == "deposit_to_server"){
            jData.push_back({"transaction_id", Res[2]}); 
            jData.push_back({"new_balance", Res[5]});
        }
        else if(payload[0] == "withdraw_from_server"){
            jData.push_back({"transaction_id", Res[2]});
            jData.push_back({"new_balance", Res[5]});
        }
        else if(payload[0] == "send_money_server"){
            jData.push_back({"transaction_id", Res[2]});
            jData.push_back({"new_balance", Res[5]});
        }
        else if(payload[0] == "balance_inquiry_server"){
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
        else if(payload[0] == "my_rank_server"){
            jData.push_back({"rank", Res[2]});
            jData.push_back({"score", Res[4]});
            jData.push_back({"level", Res[6]});
        }
        else if(payload[0] == "request_OTP_server"){
            jData.push_back({"OTP", Res[1]});
            jData.push_back({"OTP_duration", Res[4]});
        }
        else if(payload[0] == "online_payment_server"){
            jData.push_back({"transaction_id", Res[2]});
            jData.push_back({"new_balance", Res[5]});
            jData.push_back({"new_balance_destination", Res[11]});
        }
        else if(payload[0] == "show_iban_server"){
            jData.push_back({"iban", Res[1]});
        }
        else if(payload[0] == "paya_transfer_server"){
            jData.push_back({"paya_id", Res[5]});
            jData.push_back({"status", Res[7]});
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
    TM.UPD();
}

int main(){
    int compile_status = system("g++ src/user.cpp -o src/user");
    if(compile_status != 0){
        cout << "Error: User source has some bug" << endl;
        return 1;
    }
    httplib::Server server;                 //http://127.0.0.1:8080
    Token_Manager TM;
    
    server.set_error_handler([](const httplib::Request& req, httplib::Response& res){
        if(res.status == 404){
            json response;
            response["ok"] = false;
            response["error"] = "API not found";
            res.set_content(response.dump(), "application/json");
        }
    });
    server.Post("/auth/signup", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Post("/auth/login", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Delete("/auth/session", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    //list_branches
    server.Get("/branches", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Post("/accounts/requests", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Get("/accounts/requests", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Delete(R"(/accounts/requests/(\d+))", [&TM]
    (const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Patch(R"(/accounts/([^/]+)/activation)", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Delete(R"(/accounts/([^/]+))", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Post(R"(/accounts/([^/]+)/deposits)", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Post(R"(/accounts/([^/]+)/withdrawals)", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Post(R"(/accounts/([^/]+)/balance-inquiries)", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Get(R"(/accounts/([^/]+)/iban)", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Get(R"(/accounts/([^/]+)/statement)", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Get("/accounts", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Post("/transfers/card-to-card", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Post("/auth/otp", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Post("/payments/online", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Post("/transfers/paya", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Delete("/users/me", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.Get("/users/me/rank", [&TM](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(req, res, TM);
    });
    server.listen("127.0.0.1", 8081);
}