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
void Set_Response_USER(httplib::Response& res, vector<string> payload){
    if(payload.empty()){
        res.status = 200;
        return;
    }
    string result = runUser(payload);
    res.status = Status(result);
    vector<string> Res = Translate(result), Res2 = Translate(result, 1);
    json response;
    if(res.status == 200){
        response["ok"] = true;
        response["message"] = result;
        json jData = json::array();
        if(payload[0] == "list_branches"){
            json jBranches = json::array();
            for(auto B : Res2){
                vector<string> Tmp = Translate(B);
                jBranches.push_back({{"branch_id", Tmp[0]}, {"branch_name", Tmp[2]}});
            }
            jData.push_back({"branches", jBranches});
        }
        else if(payload[0] == "request_account"){
            jData.push_back({"request_id", Res[3]});
        }
        else if(payload[0] == "my_requests"){
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
        else if(payload[0] == "cancel_request"){
            jData.push_back({"request_id", payload[1]});
        }
        else if(payload[0] == "activate_account"){
            jData.push_back({"account_id", Res[3]});
        }
        else if(payload[0] == "my_accounts"){
            json jAccounts = json::array();
            for(int i = 0, sz = (int)Res.size(); i < sz; i += 3){
                jAccounts.push_back({{"account_id", Res[i]}, {"balance", Res[i + 2]}});
            }
            jData.push_back({"accounts", jAccounts});
        }
        else if(payload[0] == "deposit_to"){
            jData.push_back({"transaction_id", Res[2]}); 
            jData.push_back({"new_balance", Res[5]});
        }
        else if(payload[0] == "withdraw_from"){
            jData.push_back({"transaction_id", Res[2]});
            jData.push_back({"new_balance", Res[5]});
        }
        else if(payload[0] == "send_money"){
            jData.push_back({"transaction_id", Res[2]});
            jData.push_back({"new_balance", Res[5]});
        }
        else if(payload[0] == "balance_inquiry"){
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
        else if(payload[0] == "my_rank"){
            jData.push_back({"rank", Res[2]});
            jData.push_back({"score", Res[4]});
            jData.push_back({"level", Res[6]});
        }
        else if(payload[0] == "request_OTP"){
            jData.push_back({"OTP", Res[1]});
            jData.push_back({"OTP_duration", Res[4]});
        }
        else if(payload[0] == "online_payment"){
            jData.push_back({"transaction_id", Res[2]});
            jData.push_back({"new_balance", Res[5]});
            jData.push_back({"new_balance_destination", Res[11]});
        }
        else if(payload[0] == "show_iban"){
            jData.push_back({"iban", Res[1]});
        }
        else if(payload[0] == "paya_transfer"){
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
}

int main(){
    int compile_status = system("g++ src/user.cpp -o src/user");
    if(compile_status != 0){
        cout << "Error: User source has some bug" << endl;
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
    server.Post("/auth/signup", [](const httplib::Request& req, httplib::Response& res){
        Set_Response_USER(res, Translate(req.body));
    });


    server.listen("127.0.0.1", 8080);
}