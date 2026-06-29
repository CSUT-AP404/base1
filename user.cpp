#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sstream>
#include <stdexcept>
#include "include/picosha2.h"
#include "data/json.hpp"
#include <ctime>
#include <fstream>
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

struct User {
    vector<string> id;
    string codeMelli;
    string Hashpass;
    User(){}
    User(string codeMelli, string Hashpass){
        this->codeMelli = codeMelli ;
        this->Hashpass = Hashpass ;
    }

    bool operator== (const User &U) const{
        return (codeMelli == U.codeMelli);
    }

    ~User(){}
};

string Hasher(string pass){
    return picosha2::hash256_hex_string(pass);
}
bool compare(string pass, string input){
    input = picosha2::hash256_hex_string(input);
    if(pass==input)
        return true ;
    return false ; 
}
class USER_Core{
    private : 
        vector<User> Users; 
    
        bool isValid(string &codeMelli){
            if(codeMelli.size() < 8 || codeMelli.size() > 10){
                return false;
            }
            while(codeMelli.size() < 10){
                string Tmp = codeMelli;
                codeMelli = "0";
                codeMelli += Tmp;
            }
            ll Tmp = 0;
            for(ll i = 2; i <= 10; i++){
                Tmp += (int)(codeMelli[i - 2] - '0') * i; 
            }
            Tmp %= 11;
            if(Tmp < 2 && (int)(codeMelli[9] - '0') == Tmp){
                return true;
            }
            if(Tmp >= 2 && (int)(codeMelli[9] - '0') == (11 - Tmp)){
                return true;
            }
            return false;
        }
    public :
        USER_Core(){
            read_users();
        }

        void SignUP(string &codeMelli, string &pass){
            if(!isValid(codeMelli)){
                cout << "Error: Invalid national code." << '\n';
                return;
            }
            for(auto &u : Users){
                if(u.codeMelli == codeMelli){
                    cout << "Error: User already exists." << '\n';
                    return;
                }
            }
            Users.push_back(User (codeMelli, Hasher(pass)));
            cout << "User created." << '\n';
            write_users();
        }
        int UserIDX(string &codeMelli, string &pass){
            for(int i = 0, sz = (int)Users.size(); i < sz; i++){
                if(Users[i].codeMelli == codeMelli){
                    if(!compare(Users[i].Hashpass, pass)){
                        cout << "Error: Wrong user password." << '\n';
                        return -1;
                    }
                    cout << "Logged in." << '\n';
                    return i;
                }
            }
            cout << "Error: User not found." << '\n';
            return -1;
        }

        ~USER_Core(){}

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
                for(auto &acc : userr["accounts"]){
                    u.id.push_back(acc);
                }
                Users.push_back(u);
            }
        }
        inFile.close();
        
    }
    void write_users() {
        json j;
        json jUsers = json::array();
        for(auto &userr : Users){
            json jAccs = json::array();
            for(auto &acc : userr.id){
                jAccs.push_back(acc);
            }
            jUsers.push_back({
                {"codeMelli", userr.codeMelli},
                {"pass", userr.Hashpass},
                {"accounts", jAccs}
            });
        }
        j["users"] = jUsers;
        ofstream inFile("data/Users.json");
        inFile << j.dump(4);
        inFile.close();
    }
};

bool isbad(string Str){
    return (Str == "reset_all" || Str == "clear_history" || Str == "set_balance_inquiry_fee" || Str == "show_fees" || 
    Str == "set_transfer_fee" || Str == "list_accounts" || Str == "create_branch" || Str == "EOF");
}
string runAdmin(const vector<string>& inputs){
    if(inputs.size() != 1 || inputs[0] != "EOF"){
        for(auto &v : inputs){
            if(isbad(v)){
                return "Error: Unauthorized request";
            }
        }
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
        execl("./admin", "admin", NULL);
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

int main(){
    ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
    int compile_status = system("g++ admin.cpp -o admin");
    if(compile_status != 0){
        cout << "Error: Core system has some bug" << endl;
        return 1;
    }
    /*---------------------------------Making sure we have a branch*/
    vector<string> payload;
    payload.push_back("list_branches");
    string result = runAdmin(payload);
    payload.clear();
    bool is_there_branch = 0;
    for(auto c : result){
        is_there_branch |= (c > 32);         // is there any branches
    }
    if(!is_there_branch){
        payload.push_back("create_branch ");
        payload.push_back("\"Main Branch\"");
        runAdmin(payload);
    }
    /*-----------------------------------*/
    USER_Core Ucore;
    string cmd;
    int User_idx = -1;
    while(cin >> cmd){
        payload.clear();
        if(cmd == "EOF"){
            payload.push_back("EOF");
            runAdmin(payload);
            break;
        }
        else if(cmd == "signup"){
            string codeMelli, pass;
            cout << "Enter national code: " << endl;
            cin >> codeMelli;
            cout << "Enter user password: " << endl;
            cin >> pass;
            if(User_idx != -1){
                cout << "Error: User already logged in." << endl;
                continue;
            }
            Ucore.SignUP(codeMelli, pass);
            cout << endl;
        }
        else if(cmd == "login"){
            string codeMelli, pass;
            cout << "Enter national code: " << endl;
            cin >> codeMelli;
            cout << "Enter user password: " << endl;
            cin >> pass;
            if(User_idx != -1){
                cout << "Error: User already logged in." << endl;
                continue;
            }
            User_idx = Ucore.UserIDX(codeMelli, pass);
            cout << endl;
        }
        else if(cmd == "logout"){
            if(User_idx == -1){
                cout << "Error: No user logged in." << endl;
                continue;
            }
            User_idx = -1;
            cout << "Logged out" << endl;
        }
        else if(cmd == "open_account"){
            string pass;
            cin >> pass;
            if(User_idx == -1){
                cout << "Error: No user logged in." << endl;
                continue;
            }
            payload.push_back("create_account_op");
            payload.push_back(to_string(10001));
            payload.push_back(pass);
            cout << runAdmin(payload) << endl;
        }
    }
}