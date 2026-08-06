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
void Set_Response_Admin(httplib::Response& res, vector<string> payload){
    if(payload.empty()){
        res.status = 200;
        return;
    }
    string result = runAdmin(payload);
    res.status = Status(result);
    vector<string> Res = Translate(result), Res2 = Translate(result, 1);
    json response;
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

class DataCenter{
    private:
        vector<User> Users;
        vector<Branch> Branches;
        vector<Account> FAccounts;
        vector<Account> BAccounts;
        vector<Transaction> Trans;
        vector<Paya_Request> paya_requests; // save
        int Account_Cnt, Trans_Cnt, Request_Cnt, BankID;
        ld transferFee, balanceInquiryFee;
    public:
        DataCenter (int BankID = 5022){
            this -> BankID = BankID;
            Account_Cnt = 0;
            Trans_Cnt = 1001;
            transferFee = 0.00;
            Request_Cnt = 0;
            balanceInquiryFee = 0.00 ;
            read();
            read_setting();
            read_users();
        }

        ~DataCenter(){}

    void read(){
        ifstream inFile("data/BankـData.json");
        if(!inFile.is_open()){
            return;
        }
        json j;
        inFile >> j;
        inFile.close();
        Branches.clear();
        FAccounts.clear();
        BAccounts.clear();
        paya_requests.clear();
        Trans.clear();
        Account_Cnt = j["Account_Cnt"];
        Trans_Cnt   = j["Trans_Cnt"];
        Request_Cnt = j["Request_Cnt"];

        for(auto &item : j["Branches"]){
            Branch B(item["name"], item["id"]);
            if(item.contains("accounts")){
                for(auto &acc : item["accounts"]){
                    B.AIs.push_back(acc.get<string>());
                }
            }
            if(item.contains("requests")){
                for(auto &b : item["requests"]){
                    Request R(b["owner"].get<string>(), b["id"].get<int>(), b["status"].get<int>(), b["Branch_Id"].get<int>());
                    R.Time = b["time"].get<string>();
                    if(b.contains("reason")){
                        R.reason = b["reason"].get<string>();
                    }
                    B.Add_Request(R);
                }
            }
            Branches.push_back(B);
        }

        for(auto &item : j["active_accounts"]){
            Account A(
                item["id"].get<string>(),
                item["branch"].get<int>(),
                item["hash_pass"].get<string>(),
                (ld)item["coin"].get<double>(),
                item["active"].get<bool>()
            );
            for(auto &t : item["history"]){
                Transaction T;
                T.Type = t["type"];
                T.ID = t["id"];
                T.Val = (ld)t["val"].get<double>();
                T.BALANCE = (ld)t["balance"].get<double>();
                T.Time = t["time"];
                T.Origin = t["origin"];
                T.Destination = t["destination"];
                A.pushHistory(T);
            }
            FAccounts.push_back(A);
        }

        for(auto &item : j["closed_accounts"]){
            Account A(
                item["id"].get<string>(),
                item["branch"].get<int>(),
                item["hash_pass"].get<string>(),
                (ld)item["coin"].get<double>(),
                item["active"].get<bool>()
            );
            for(auto &t : item["history"]){
                Transaction T;
                T.Type = t["type"];
                T.ID = t["id"];
                T.Val = (ld)t["val"].get<double>();
                T.BALANCE = (ld)t["balance"].get<double>();
                T.Time = t["time"];
                T.Origin = t["origin"];
                T.Destination = t["destination"];
                A.pushHistory(T);
            }
            BAccounts.push_back(A);
        }

        for(auto &t : j["transactions"]){
            Transaction T;
            T.Type = t["type"];
            T.ID = t["id"];
            T.Val = (ld)t["val"].get<double>();
            T.BALANCE = (ld)t["balance"].get<double>();
            T.Time = t["time"];
            T.Origin = t["origin"];
            T.Destination = t["destination"];
            Trans.push_back(T);
        }
        for (auto &p : j["paya"]){
            Paya_Request paya;
            paya.id = p["id"];
            paya.from_account = p["from_account"];
            paya.destination_iban = p["destination_iban"];
            paya.amount = p["amount"].get<double>();
            paya.status = p["status"];
            paya_requests.push_back(paya);
        }
    }
    void read_setting() {
        ifstream inFile("data/setting.json");
        if(!inFile.is_open()){
            return;
        }
        json j;
        inFile >> j;
        if(j.contains("transfer_fee"))
            transferFee = j["transfer_fee"];
        if(j.contains("balance_inquiry_fee")) 
            balanceInquiryFee = j["balance_inquiry_fee"];
        inFile.close();
    }
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

int main(){
    int compile_status = system("g++ src/admin.cpp -o src/admin");
    if(compile_status != 0){
        cout << "Error: Admin source has some bug" << endl;
        return 1;
    }
    httplib::Server server;                 //http://127.0.0.1:8080
    
    server.set_error_handler([](const httplib::Request& req, httplib::Response& res){
        if(res.status == 404){
            json response;
            response["ok"] = false;
            response["error"] = "API not found";
            res.set_content(response.dump(), "application/json");
        }
    });

    server.listen("127.0.0.1", 8080);
}