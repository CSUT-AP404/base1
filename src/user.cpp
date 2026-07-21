#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sstream>
#include <stdexcept>
#include "../include/picosha2.h"
#include "../include/json.hpp"
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

const double maximum_transaction = 10000000;


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
struct User {
    vector<int> Request_Ids;
    vector<string> id;
    vector <string> ibans;
    string codeMelli;
    string Hashpass;
    int score = 0;
    string signup_time;

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
        bool isValid(string id){
            if(id.length() < 8 || id.length() > 10){
                return false;
            }
            for(char c : id){
                if(!isdigit(c)){
                    return false;
                }
            }
            while(id.length() < 10){
                id = "0" + id;
            }
            int sum = 0;
            for (int i = 0; i < 9; i++) {
                sum += (id[i] - '0') * (10 - i);
            }
            int remainder = sum % 11;
            int controlDigit = id[9] - '0';
            if(remainder < 2){
                return (controlDigit == remainder);
            } 
            return (controlDigit == (11 - remainder));
        }
        string getLevel(int score) {
            if(score <= 4) 
                return "Bronze";
            if(score <= 9) 
                return "Silver";
            if(score <= 14) 
                return "Gold";
            return "Diamond";
        }
    public :
        USER_Core(){
            read_users();
        }
        int mod97(const string &s) {
            int rem = 0;
            for (char c : s){
                rem = (rem * 10 + (c - '0')) % 97;
            }
            return rem;
        } 
        string add_iban(int user_index, int account_index) {
            if (Users[user_index].ibans.size() < Users[user_index].id.size()){
                Users[user_index].ibans.resize(Users[user_index].id.size());
            }
            string body = "000000" + Users[user_index].id[account_index];
            string num = body + "182700";
            int check = 98 - mod97(num);
            string iban = "IR";
            if (check < 10){
                iban += "0";
            }
            iban += to_string(check);
            iban += body;
            Users[user_index].ibans[account_index] = iban;
            return iban;
        }
        vector<string> AccList(int idx){
            return Users[idx].id;
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
        string UserCode(int idx){
            return Users[idx].codeMelli;
        }
        void Add_Request(int idx, int Request_id){
            Users[idx].Request_Ids.push_back(Request_id);
            write_users();
        }
        vector<int> UserRequestList(int idx) const{
            return Users[idx].Request_Ids;
        }

        void AccAdd(int idx, string &name){
            Users[idx].id.push_back(name);
            write_users();
        }
        int AccIDX(int idx, string &name){
            for(int i = 0, sz = Users[idx].id.size(); i < sz; i++){
                if(Users[idx].id[i] == name){
                    return i;
                }
            }
            return -1;
        }
        void RmvAcc(int idx, int idx2){
            if(idx2 == -1){
                return;
            }
            Users[idx].erase(idx2);
            write_users();
        }
        bool RmvUser(int idx, string &pass){
            if(!compare(Users[idx].Hashpass, pass)){
                cout << "Error: Wrong user password." << '\n';
                return false;
            }
            Users.erase(Users.begin() + idx);
            cout << "User deleted." << '\n';
            write_users();
            return true;
        }
        void changeScore(int idx, int amount) {
            if (idx >= 0 && idx < (int)Users.size()) {
                Users[idx].score += amount;
                if (Users[idx].score < 0) Users[idx].score = 0;
                write_users();
            }
        }
        void ptrRank(int idx) {
            if(idx < 0 || idx >= (int)Users.size()){
                return;
            }
            vector<User> sortedUsers = Users;
            sort(sortedUsers.begin(), sortedUsers.end());
            
            int rank = 1;
            for(int i = 0; i < (int)sortedUsers.size(); i++) {
                if(sortedUsers[i].codeMelli == Users[idx].codeMelli) {
                    rank = i + 1;
                    break;
                }
            }
            cout << "Rank : " << rank << endl;
            cout << "Score: " << Users[idx].score << endl;
            cout << "Level: " << getLevel(Users[idx].score) << endl;
        }

        ~USER_Core(){}

    void read_users() {
        ifstream inFile("../data/Users.json");
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
    void write_users() {
        json j;
        json jUsers = json::array();
        for(auto &userr : Users){
            json jAccs = json::array();
            for(auto &acc : userr.id){
                jAccs.push_back(acc);
            }
            json jRequests = json::array();
            for(auto &R : userr.Request_Ids){
                jRequests.push_back(R);
            }
            jUsers.push_back({
                {"codeMelli", userr.codeMelli},
                {"pass", userr.Hashpass},
                {"score", userr.score},
                {"signup_time", userr.signup_time},
                {"accounts", jAccs},
                {"request_ids", jRequests}
            });
        }
        j["users"] = jUsers;
        ofstream inFile("../data/Users.json");
        inFile << j.dump(4);
        inFile.close();
    }

};

bool isbad(string Str){
    return (Str == "reset_all" || Str == "clear_history" || Str == "set_balance_inquiry_fee" || Str == "show_fees" || 
    Str == "set_transfer_fee" || Str == "list_accounts" || Str == "create_branch" || Str == "EOF");
}
string runAdmin(const vector<string>& inputs){
    if(inputs.size() == 0){
        return "";
    }
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
vector<string> Translate(string &result){
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
bool isError(string &result){
    auto Res = Translate(result);
    for(auto &Str: Res){
        if(Str == "Error:"){
            return true;
        }
    }
    return false;
}

int main(){
    srand(0);
    ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
    int compile_status = system("g++ admin.cpp -o admin");
    if(compile_status != 0){
        cout << "Error: Core system has some bug" << endl;
        return 1;
    }
    vector<string> payload;
    string result;
    USER_Core Ucore;
    string cmd;
    int User_idx = -1;
    int OTP;
    auto OTP_start_time = chrono::high_resolution_clock::now();
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
        else if(cmd == "list_branches"){
            payload.push_back("list_branches");
            cout << runAdmin(payload) << endl;
        }
        /*----------------------------------------------------------*/
        /*-----------------------construction zone-------------------------------*/
        else if(cmd == "request_account"){
            string branch_id;
            cin >> branch_id;
            if(User_idx == -1){
                cout << "Error: No user logged in." << endl;
                continue;
            }
            payload.push_back("is_branch_op");
            payload.push_back(branch_id);
            result = runAdmin(payload);
            auto Res = Translate(result);
            payload.clear();
            if(Res[0] == "No"){
                cout << "Error: Branch not found." << endl;
            }
            payload.push_back("add_account_request_op");
            payload.push_back(Ucore.UserCode(User_idx));
            payload.push_back(branch_id);
            result = runAdmin(payload);
            Res = Translate(result);
            cout << result << endl;
            payload.clear();
            if(!isError(result)){
                Ucore.Add_Request(User_idx, stoi(Res[3]));
            }
        }
        else if(cmd == "my_requests"){
            if(User_idx == -1){
                cout << "Error: No user logged in." << endl;
                continue;
            }
            auto RequestList = Ucore.UserRequestList(User_idx);
            for(auto id : RequestList){
                payload.push_back("print_request_op");
                payload.push_back(to_string(id));
                cout << runAdmin(payload) << endl;
                payload.clear();
            }
        }
        else if(cmd == "cancel_request"){
            ll req_Id;
            cin >> req_Id;
            if(User_idx == -1){
                cout << "Error: No user logged in." << endl;
                continue;
            }
            payload.push_back("cancel_request_op");
            payload.push_back(Ucore.UserCode(User_idx));
            payload.push_back(to_string(req_Id));
            cout << runAdmin(payload) << endl;
        }
        else if(cmd == "activate_account"){
            string pass;
            ll req_Id;
            cin >> req_Id;
            cout << "Enter account password: " << endl;
            cin >> pass;
            if(User_idx == -1){
                cout << "Error: No user logged in." << endl;
                continue;
            }
            payload.push_back("is_request_usable_op");
            payload.push_back(Ucore.UserCode(User_idx));
            payload.push_back(to_string(req_Id));
            result = runAdmin(payload);
            payload.clear();
            if(isError(result)){
                cout << result << endl;
                continue;
            }
            auto Res = Translate(result);
            payload.push_back("create_account_op");
            payload.push_back(Res[2]);
            payload.push_back(pass);
            result = runAdmin(payload);
            if(!isError(result)){
                auto Res = Translate(result);
                Ucore.AccAdd(User_idx, Res[3]);
                Ucore.changeScore(User_idx, 3);
            }
            cout << result << endl;//*/
        }
        /*---------------------------export_history-------------------------------*/
        else if(cmd == "export_history"){
            string account_number;
            cin >> account_number;
            if(User_idx == -1){
                cout << "Error: No user logged in." << endl;
                continue;
            }
            payload.push_back("get_balance_op");
            payload.push_back(account_number);
            string checkResult = runAdmin(payload);
            payload.clear();
            if(checkResult.rfind("Error:", 0) == 0){
                cout << checkResult;
                continue;
            }
            vector<string> Accs = Ucore.AccList(User_idx);
            bool check = false;
            for(auto &name : Accs){
                if(name == account_number){
                    check = true;
                    break;
                }
            }
            if(!check){
                cout << "Error: Account does not belong to user." << endl;
                continue;
            }
            payload.push_back("get_history");
            payload.push_back(account_number);
            result = runAdmin(payload);
            string filename = "history_" + account_number + ".csv";
            ofstream outFile(filename);
            outFile << "id,timestamp,type,amount,balance_after\n";
            int start = 0;
            int siz = result.size();
            while(start < siz){
                int end = start;
                while(end < siz && result[end] != '\n'){
                    end++;
                }
                string line = result.substr(start, end-start);
                if(line != ""){
                    vector<string> parts;
                    int prev = 0;
                    int pos = 0;
                    while((pos = line.find(" | ", prev)) != -1){
                        parts.push_back(line.substr(prev, pos - prev));
                        prev = pos + 3;
                    }
                    parts.push_back(line.substr(prev));

                    if(parts.size() == 5){
                        string id = parts[0];
                        string timestamp = parts[1];
                        string type = parts[2];
                        string amount = parts[3];
                        string balance = parts[4].substr(9);

                        outFile << id << "," << timestamp << "," << type << "," << amount << "," << balance << "\n";
                    }
                }

                start = end + 1;
            }

            outFile.close();
            cout << "History exported to " << filename << endl;

        }

        /*----------------------------------------------------------*/
        else if(cmd == "my_accounts"){
            if(User_idx == -1){
                cout << "Error: No user logged in." << endl;
                continue;
            }
            vector<string> Accs = Ucore.AccList(User_idx);
            for(auto &name : Accs){
                payload.push_back("get_balance_op");
                payload.push_back(name);
            }
            cout << runAdmin(payload) << endl;
        }
        else if(cmd == "delete_my_account"){
            string pass, name;
            cin >> name;
            cout << "Enter account password: " << endl;
            cin >> pass;
            if(User_idx == -1){
                cout << "Error: No user logged in." << endl;
                continue;
            }
            payload.push_back("get_balance_op");
            payload.push_back(name);
            result = runAdmin(payload);
            payload.clear();
            auto Res = Translate(result);
            bool f1 = 0;
            for(auto &str : Res){
                if(str == "Error:"){
                    f1 = 1;
                    break;
                }
            }
            if(f1){
                cout << result << endl;
                continue;
            }
            if(Res[3][0] != '0'){
                cout << "Error: Account balance is positive" << endl;
                continue;
            }
            int Acc_idx = Ucore.AccIDX(User_idx, name);
            if(Acc_idx == -1){
                cout << "Error: Account does not belong to user." << endl;
                continue;
            }
            payload.push_back("delete_account_op");
            payload.push_back(name);
            payload.push_back(pass);
            result = runAdmin(payload);
            cout << result << endl; 

            if (isError(result)) {
                Ucore.RmvAcc(User_idx, Acc_idx);
                Ucore.changeScore(User_idx, -2);    
            }
      
        }
		else if (cmd == "deposit_to"){
            string account_id;
            cin >> account_id;
            double amount;
            cin >> amount;
            payload.push_back("deposit");
            payload.push_back(account_id);
            payload.push_back(to_string(amount));
            string result = runAdmin(payload);
            cout << result << endl;

            if (isError(result) && User_idx != -1) {
                Ucore.changeScore(User_idx, 1);
            }

        }
		else if (cmd == "withdraw_from"){
            string account_id;
            double amount;
            cin >> account_id;
            cin >> amount;
            cout << "Enter account password: " << endl;
            string password;
            cin >> password;
            if (User_idx == -1){
                cout << "Error: No user logged in." << endl;
                continue;
            }
            vector <string> Accs = Ucore.AccList(User_idx);
            bool belong_to = false;
            for (auto& name : Accs){
                if (name == account_id){
                    belong_to = true;
                }
            }
            if (!belong_to){
                cout << "Error: Account does not belong to user." << endl;
                continue;
            }
            payload.push_back("withdraw_op");
            payload.push_back(account_id);
            payload.push_back(to_string(amount));
            payload.push_back(password);
            result = runAdmin(payload);
            cout << result << endl;

            if (isError(result)){
                Ucore.changeScore(User_idx, 1);
            }

        }
		else if (cmd == "send_money"){
            double amount;
            string from_acc, to_acc;
            cin >> from_acc >> to_acc >> amount;
            cout << "Enter account password: " << endl;
            string password;
            cin >> password;
            if (User_idx == -1){
                cout << "Error: No user logged in." << endl;
                continue;
            }
            vector <string> Accs = Ucore.AccList(User_idx);
            bool belong_to = false;
            for (auto& name : Accs){
                if (name == from_acc){
                    belong_to = true;
                }
            }
            if (!belong_to){
                cout << "Error: source Account does not belong to user." << endl;
                continue;
            }
            payload.push_back("transfer_op");
            payload.push_back(from_acc);
            payload.push_back(to_acc);
            payload.push_back(to_string(amount));
            payload.push_back(password);
            result = runAdmin(payload);
            cout << result << endl;
            if (isError(result)) {
                Ucore.changeScore(User_idx, 2);
            }

		}
		else if (cmd == "balance_inquiry"){
			string account_id;
			cin >> account_id;
            if (User_idx == -1){
                cout << "Error: No user logged in." << endl;
                continue;
            }
            vector<string> Accs = Ucore.AccList(User_idx);
            bool belong_to = false;
            for (auto &name : Accs){
                if (name == account_id){
                    belong_to = true;
                }
            }
            if (!belong_to){
                cout << "Error: Account does not belong to user." << endl;
                continue;
            }
            payload.push_back("get_balance");
			payload.push_back(account_id);
			result = runAdmin(payload);
			cout << result << endl;
            if (isError(result)){
                Ucore.changeScore(User_idx, 1); 
            }
		}
        else if(cmd == "my_rank"){
            if(User_idx == -1){
                cout << "Error: No user logged in." << endl;
                continue;
            }
            Ucore.ptrRank(User_idx);
        }
        else if(cmd == "delete_my_user"){
            string pass;
            cout << "Enter user password: " << endl;
            cin >> pass;
            if(User_idx == -1){
                cout << "Error: No user logged in." << endl;
                continue;
            }
            if(Ucore.AccList(User_idx).size() != 0){
                cout << "Error: User has accounts." << endl;
                continue;
            }
            if(Ucore.RmvUser(User_idx, pass)){
                User_idx = -1;
            }
            cout << endl;
        }
        else if (cmd == "request_OTP"){
            string account_id;
            cin >> account_id;
            vector <string> Accs = Ucore.AccList(User_idx);
            bool belong_to = false;
            for (auto& name : Accs){
                if (name == account_id){
                    belong_to = true;
                }
            }
            if (!belong_to){
                cout << "Error: Account does not belong to user." << endl;
                continue;
            }
            OTP = rand() * rand();
            OTP = abs(OTP);
            OTP %= 1000000;
            auto start_time = chrono::high_resolution_clock::now();
            cout << "OTP: " << OTP << '\n';
            cout << "expires in 120 seconds\n";
        }
        else if (cmd == "online_payment"){
            double amount;
            string from_account, to_account;
            cin >> from_account >> to_account;
            cin >> amount;
            vector <string> Accs = Ucore.AccList(User_idx);
            bool belong_to = false;
            for (auto& name : Accs){
                if (name == from_account){
                    belong_to = true;
                }
            }
            if (!belong_to){
                cout << "Error: Account does not belong to user." << endl;
                continue;
            }
            cout << "Enter OTP:\n";
            int entered_OTP;
            cin >> entered_OTP;
            if (entered_OTP != OTP){
                cout << "Error: Invalid OTP.\n";
                continue;
            }
            auto now_time = chrono::high_resolution_clock::now();
            double time_since_OTP_request = chrono::duration<double>(now_time - OTP_start_time).count();
            if (time_since_OTP_request > 120){
                cout << "Error: OTP expired.\n";
                continue;
            }
            if (amount > maximum_transaction){
                cout << "Error: transaction limit exceeded.\n";
                continue;
            }
            // TODO runAdmin
        }
        else if (cmd == "show_iban"){
            string account_id;
            cin >> account_id;
            int account_index;
            vector <string> Accs = Ucore.AccList(User_idx);
            bool belong_to = false;
            for (int i = 0; i < Accs.size(); ++ i){
                if (Accs[i] == account_id){
                    belong_to = true;
                    account_index = i;
                }
            }
            if (!belong_to){
                cout << "Error: Account does not belong to user." << endl;
                continue;
            }
            string iban = Ucore.add_iban(User_idx, account_index);
            cout << "IBAN: IR12 " << iban << '\n';
        }
        else {
            cout << "Error: Unknown command" << endl;
        }
    }
    Ucore.write_users();
}