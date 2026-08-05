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
vector<string> Translate(const string &result){
    vector<string> Res;
    Res.push_back("");
    for(auto c : result){
        if(c > 32){
            Res[Res.size() - 1] += c;
        }
        else if(Res.back() != ""){
            Res.push_back("");
        }
    }
    return Res;
}
bool Find(string &result, string Str){
    return (result.find(Str) != string::npos);
}
int Status(const vector<string> payload, string result){
    vector<string> Res = Translate(result);
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
}

int main(){
    int compile_status = system("g++ src/admin.cpp -o src/admin");
    if(compile_status != 0){
        cout << "Error: Admin source has some bug" << endl;
        return 1;
    }
    
    httplib::Server server;                 //http://127.0.0.1:8080
    
    server.set_error_handler([](const httplib::Request& req, httplib::Response& res){
        if(res.status == 404){
            res.set_content(
                R"({"error":"API not found"})",
                "application/json"
            );
        }
    });

    server.listen("127.0.0.1", 8080);
}