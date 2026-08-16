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
const double maximum_transaction_online = 10000;
const double maximum_transaction_normal = 100000;
const double maximum_transaction_paya = 10000000;

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
    bool isValid(string code) {
        for (char c : code) {
            if (!isdigit(c)) return false;
        }
        if (code.length() < 8 || code.length() > 10) {
            return false;
        }
        while (code.length() < 10) {
            code = "0" + code;
        }
        int sum = 0;
        for (int i = 0; i < 9; i++) {
            sum += (code[i] - '0') * (10 - i);
        }
        int remainder = sum % 11;
        int checkDigit = code[9] - '0';
        if (remainder < 2) {
            return checkDigit == remainder;
        } else {
            return checkDigit == 11 - remainder;
        }
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
        void SetOTPTime (int idx, long long t) {
            Users[idx].OTP_start_time_in_MS = t;
            write_users();
        }
        void SetOTP (int idx, int otp){
            Users[idx].OTP = otp;
            write_users();
            return;
        }
        int getOTP (int idx){
            return Users[idx].OTP;
        }
        long long GetOTPTime(int idx) {
            return Users[idx].OTP_start_time_in_MS;
        }
        USER_Core(){
            read_users();
        }

        int mod97(const string &s){
            int rem = 0;
            for (char c : s){
                rem = (rem * 10 + (c - '0')) % 97;
            }
            return rem;
        }

        string add_iban(int user_index, int account_index){
            if (Users[user_index].ibans.size() < Users[user_index].id.size())
                Users[user_index].ibans.resize(Users[user_index].id.size());
            string clean_id = removeDashes(Users[user_index].id[account_index]);
            string body = "000000" + clean_id;
            string num = body + "182700";
            int check = 98 - mod97(num);
            string iban = "IR";
            if (check < 10)
                iban += "0";
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
            cout << "Rank : " << rank << '\n';
            cout << "Score: " << Users[idx].score << '\n';
            cout << "Level: " << getLevel(Users[idx].score) << '\n';
        }

        ~USER_Core(){
            write_users();
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
    void write_users() {
        json j;
        json jUsers = json::array();
        for(auto &userr : Users){
            json jAccs = json::array();
            for(auto &acc : userr.id){
                jAccs.push_back(acc);
            }
            json jibans = json::array();
            for(auto &iban : userr.ibans){
                jibans.push_back(iban);
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
                {"request_ids", jRequests},
                {"OTP", userr.OTP},
                {"OTP_start_time_in_MS", userr.OTP_start_time_in_MS},
                {"ibans", jibans}
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
    Str == "set_transfer_fee" || Str == "list_accounts" || Str == "create_branch" || Str == "EOF" || 
    Str == "branch_dashboard" || Str == "list_requests" || Str == "approve_request" || Str == "reject_request" || 
    Str == "show_ranking" || Str == "list_paya_requests" || Str == "reject_paya");
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
    //i used time(nullptr) insted of 0 cause it'll make OTP different with the other
    srand(time(nullptr));
    //ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
    int compile_status = system("g++ src/admin.cpp -o src/admin");
    if(compile_status != 0){
        cout << "Error: Core system has some bug" << '\n';
        return 1;
    }
    vector<string> payload;
    string result;
    USER_Core Ucore;
    string cmd;
    int User_idx = -1;
    string OTP_Code = "Error: No OTP request yet";
    ll OTP_Time = -1e9;
    while(cin >> cmd){
        payload.clear();
        if(cmd == "EOF"){
            payload.push_back("EOF");
            runAdmin(payload);
            break;
        }
        else if(cmd == "signup"){
            string codeMelli, pass;
            cin >> codeMelli;
            cin >> pass;
            if(User_idx != -1){
                cout << "Error: User already logged in." << '\n';
                continue;
            }
            Ucore.SignUP(codeMelli, pass);
            cout << '\n';
        }
        else if(cmd == "login"){
            string codeMelli, pass;
            cin >> codeMelli;
            cin >> pass;
            if(User_idx != -1){
                cout << "Error: User already logged in." << '\n';
                continue;
            }
            User_idx = Ucore.UserIDX(codeMelli, pass);
            cout << '\n';
        }
        else if(cmd == "logout"){
            if(User_idx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            User_idx = -1;
            cout << "Logged out" << '\n';
        }
        else if(cmd == "list_branches"){
            payload.push_back("list_branches");
            cout << runAdmin(payload) << '\n';
        }
        /*----------------------------------------------------------*/
        else if(cmd == "request_account"){
            string branch_id;
            cin >> branch_id;
            if(User_idx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            payload.push_back("is_branch_op");
            payload.push_back(branch_id);
            result = runAdmin(payload);
            auto Res = Translate(result);
            payload.clear();
            if(Res[0] != "Yes"){
                cout << "Error: Branch not found." << '\n';
                continue;
            }
            payload.push_back("add_account_request_op");
            payload.push_back(Ucore.UserCode(User_idx));
            payload.push_back(branch_id);
            result = runAdmin(payload);
            Res = Translate(result);
            cout << result << '\n';
            payload.clear();
            if(!isError(result)){
                Ucore.Add_Request(User_idx, stoi(Res[3]));
            }
        }
        else if(cmd == "my_requests"){
            if(User_idx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            auto RequestList = Ucore.UserRequestList(User_idx);
            for(auto id : RequestList){
                payload.push_back("print_request_op");
                payload.push_back(to_string(id));
                cout << runAdmin(payload) << '\n';
                payload.clear();
            }
        }
        else if(cmd == "cancel_request"){
            ll req_Id;
            cin >> req_Id;
            if(User_idx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            payload.push_back("cancel_request_op");
            payload.push_back(Ucore.UserCode(User_idx));
            payload.push_back(to_string(req_Id));
            cout << runAdmin(payload) << '\n';
        }
        else if(cmd == "activate_account"){
            string pass;
            ll req_Id;
            cin >> req_Id;
            cin >> pass;
            if(User_idx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            payload.push_back("is_request_usable_op");
            payload.push_back(Ucore.UserCode(User_idx));
            payload.push_back(to_string(req_Id));
            result = runAdmin(payload);
            payload.clear();
            if(isError(result)){
                cout << result << '\n';
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
            cout << result << '\n';//*/
        }
        /*---------------------------export_history-------------------------------*/
        else if(cmd == "export_history"){
            string account_number;
            cin >> account_number;
            if(User_idx == -1){
                cout << "Error: No user logged in." << '\n';
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
                cout << "Error: Account does not belong to user." << '\n';
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
            cout << "History exported to " << filename << '\n';

        }
        /*----------------------------------------------------------*/
        else if(cmd == "my_accounts"){
            if(User_idx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            vector<string> Accs = Ucore.AccList(User_idx);
            for(auto &name : Accs){
                payload.push_back("get_balance_op");
                payload.push_back(name);
            }
            cout << runAdmin(payload) << '\n';
        }
        else if(cmd == "delete_my_account"){
            string pass, name;
            cin >> name;
            cin >> pass;
            if(User_idx == -1){
                cout << "Error: No user logged in." << '\n';
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
                cout << result << '\n';
                continue;
            }
            if(Res[3][0] != '0'){
                cout << "Error: Account balance is positive" << '\n';
                continue;
            }
            int Acc_idx = Ucore.AccIDX(User_idx, name);
            if(Acc_idx == -1){
                cout << "Error: Account does not belong to user." << '\n';
                continue;
            }
            payload.push_back("delete_account_op");
            payload.push_back(name);
            payload.push_back(pass);
            result = runAdmin(payload);
            cout << result << '\n'; 

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
            cout << result << '\n';

            if (isError(result) && User_idx != -1) {
                Ucore.changeScore(User_idx, 1);
            }

        }
		else if (cmd == "withdraw_from"){
            string account_id;
            double amount;
            cin >> account_id;
            cin >> amount;
            string password;
            cin >> password;
            if (User_idx == -1){
                cout << "Error: No user logged in." << '\n';
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
                cout << "Error: Account does not belong to user." << '\n';
                continue;
            }
            payload.push_back("withdraw_op");
            payload.push_back(account_id);
            payload.push_back(to_string(amount));
            payload.push_back(password);
            result = runAdmin(payload);
            cout << result << '\n';

            if (isError(result)){
                Ucore.changeScore(User_idx, 1);
            }

        }
		else if (cmd == "send_money"){
            double amount;
            string from_acc, to_acc;
            cin >> from_acc >> to_acc >> amount;
            string password;
            cin >> password;
            if (User_idx == -1){
                cout << "Error: No user logged in." << '\n';
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
                cout << "Error: source Account does not belong to user." << '\n';
                continue;
            }
            if (amount > maximum_transaction_normal){
                cout << "Error: Transaction limit exceeded." << '\n';
                continue;
            }
            payload.push_back("transfer_op");
            payload.push_back(from_acc);
            payload.push_back(to_acc);
            payload.push_back(to_string(amount));
            payload.push_back(password);
            result = runAdmin(payload);
            cout << result << '\n';
            if (isError(result)) {
                Ucore.changeScore(User_idx, 2);
            }

		}
		else if (cmd == "balance_inquiry"){
			string account_id;
			cin >> account_id;
            if (User_idx == -1){
                cout << "Error: No user logged in." << '\n';
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
                cout << "Error: Account does not belong to user." << '\n';
                continue;
            }
            payload.push_back("get_balance");
			payload.push_back(account_id);
			result = runAdmin(payload);
			cout << result << '\n';
            if (isError(result)){
                Ucore.changeScore(User_idx, 1); 
            }
		}
        else if(cmd == "my_rank"){
            if(User_idx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            Ucore.ptrRank(User_idx);
        }
        else if(cmd == "delete_my_user"){
            string pass;
            cin >> pass;
            if(User_idx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            if(Ucore.AccList(User_idx).size() != 0){
                cout << "Error: User has accounts." << '\n';
                continue;
            }
            if(Ucore.RmvUser(User_idx, pass)){
                User_idx = -1;
            }
            cout << '\n';
        }
        else if (cmd == "request_OTP"){
            string account_id;
            cin >> account_id;
            if (User_idx == -1){
                cout << "Error: No user logged in." << '\n';
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
                cout << "Error: Account does not belong to user." << '\n';
                continue;
            }
            long long OTP_start_time_in_MS = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
            if(OTP_start_time_in_MS - OTP_Time < 120000){
                cout << "OTP: " << OTP_Code << '\n';
                cout << "expires in " << 120 - ((OTP_start_time_in_MS - OTP_Time) / 1000ll) << " seconds\n";
                continue;
            }
            OTP_Time = OTP_start_time_in_MS;
            int OTP = rand() * rand();
            OTP = abs(OTP);
            OTP %= 1000000;
            OTP_Code = to_string(OTP);
            Ucore.SetOTP(User_idx, OTP);
            Ucore.SetOTPTime(User_idx, OTP_start_time_in_MS);
            cout << "OTP: " << OTP << '\n';
            cout << "expires in 120 seconds\n";
        }
        else if (cmd == "online_payment"){
            double amount;
            string from_account, to_account;
            cin >> from_account >> to_account;
            cin >> amount;
            if (User_idx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            vector <string> Accs = Ucore.AccList(User_idx);
            bool belong_to = false;
            for (auto& name : Accs){
                if (name == from_account){
                    belong_to = true;
                }
            }
            if (!belong_to){
                cout << "Error: Account does not belong to user." << '\n';
                continue;
            }
            int entered_OTP;
            cin >> entered_OTP;
            if (entered_OTP != Ucore.getOTP(User_idx)){
                cout << "Error: Invalid OTP.\n";
                continue;
            }
            long long now_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
            if (now_time - Ucore.GetOTPTime(User_idx) > 120 * 1000){
                cout << "Error: OTP expired.\n";
                continue;
            }
            if (amount > maximum_transaction_online){
                cout << "Error: transaction limit exceeded.\n";
                continue;
            }
            payload.push_back("transfer_op_no_password");
            payload.push_back(from_account);
            payload.push_back(to_account);
            payload.push_back(to_string(amount));
            result = runAdmin(payload);
            cout << result << '\n';
        }
        else if (cmd == "show_iban"){
            string account_id;
            cin >> account_id;
            if (User_idx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            vector <string> Accs = Ucore.AccList(User_idx);
            bool belong_to = false;
            int account_index;
            for (int i = 0; i < Accs.size(); ++ i){
                if (Accs[i] == account_id){
                    belong_to = true;
                    account_index = i;
                    break;
                }
            }
            if (!belong_to){
                cout << "Error: Account does not belong to user." << '\n';
                continue;
            }
            //account_id = removeDashes(account_id);
            cout << "IBAN: " << Ucore.add_iban(User_idx, account_index) << '\n';
        }
        else if (cmd == "paya_transfer"){
            string from_account, destination_iban, pass;
            double amount;
            cin >> from_account >> destination_iban >> amount;
            //destination_iban = removeDashes(destination_iban);
            cin >> pass;
            if (User_idx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            vector <string> Accs = Ucore.AccList(User_idx);
            bool belong_to = false;
            for (auto& name : Accs){
                if (name == from_account){
                    belong_to = true;
                }
            }
            if (!belong_to){
                cout << "Error: source Account does not belong to user." << '\n';
                continue;
            }
            if (amount > maximum_transaction_paya){
                cout << "Error: Transaction limit exceeded." << '\n';
                continue;
            }
            payload.push_back("paya_transfer");
            payload.push_back(from_account);
            payload.push_back(destination_iban);
            payload.push_back(to_string(amount));
            payload.push_back(pass);
            cout << runAdmin(payload);
        }

        if(cmd == "EOF_server"){
            payload.push_back("EOF");
            runAdmin(payload);
            break;
        }
        else if(cmd == "signup_server"){
            int Uidx;
            cin >> Uidx;
            string codeMelli, pass;
            cin >> codeMelli;
            cin >> pass;
            if(Uidx != -1){
                cout << "Error: User already logged in." << '\n';
                continue;
            }
            Ucore.SignUP(codeMelli, pass);
            cout << '\n';
        }
        else if(cmd == "login_server"){
            int Uidx;
            cin >> Uidx;
            string codeMelli, pass;
            cin >> codeMelli;
            cin >> pass;
            if(Uidx != -1){
                cout << "Error: User already logged in." << '\n';
                continue;
            }
            User_idx = Ucore.UserIDX(codeMelli, pass);
            cout << '\n';
        }
        else if(cmd == "logout_server"){
            int Uidx;
            cin >> Uidx;
            if(Uidx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            User_idx = -1;
            cout << "Logged out" << '\n';
        }
        else if(cmd == "list_branches_server"){
            int Uidx;
            cin >> Uidx;
            payload.push_back("list_branches");
            cout << runAdmin(payload) << '\n';
        }
        /*----------------------------------------------------------*/
        else if(cmd == "request_account_server"){
            int Uidx;
            cin >> Uidx;
            string branch_id;
            cin >> branch_id;
            if(Uidx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            payload.push_back("is_branch_op");
            payload.push_back(branch_id);
            result = runAdmin(payload);
            auto Res = Translate(result);
            payload.clear();
            if(Res[0] != "Yes"){
                cout << "Error: Branch not found." << '\n';
                continue;
            }
            payload.push_back("add_account_request_op");
            payload.push_back(Ucore.UserCode(Uidx));
            payload.push_back(branch_id);
            result = runAdmin(payload);
            Res = Translate(result);
            cout << result << '\n';
            payload.clear();
            if(!isError(result)){
                Ucore.Add_Request(Uidx, stoi(Res[3]));
            }
        }
        else if(cmd == "my_requests_server"){
            int Uidx;
            cin >> Uidx;
            if(Uidx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            auto RequestList = Ucore.UserRequestList(Uidx);
            for(auto id : RequestList){
                payload.push_back("print_request_op");
                payload.push_back(to_string(id));
                cout << runAdmin(payload) << '\n';
                payload.clear();
            }
        }
        else if(cmd == "cancel_request_server"){
            int Uidx;
            cin >> Uidx;
            ll req_Id;
            cin >> req_Id;
            if(Uidx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            payload.push_back("cancel_request_op");
            payload.push_back(Ucore.UserCode(Uidx));
            payload.push_back(to_string(req_Id));
            cout << runAdmin(payload) << '\n';
        }
        else if(cmd == "activate_account_server"){
            int Uidx;
            cin >> Uidx;
            string pass;
            ll req_Id;
            cin >> req_Id;
            cin >> pass;
            if(Uidx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            payload.push_back("is_request_usable_op");
            payload.push_back(Ucore.UserCode(Uidx));
            payload.push_back(to_string(req_Id));
            result = runAdmin(payload);
            payload.clear();
            if(isError(result)){
                cout << result << '\n';
                continue;
            }
            auto Res = Translate(result);
            payload.push_back("create_account_op");
            payload.push_back(Res[2]);
            payload.push_back(pass);
            result = runAdmin(payload);
            if(!isError(result)){
                auto Res = Translate(result);
                Ucore.AccAdd(Uidx, Res[3]);
                Ucore.changeScore(Uidx, 3);
            }
            cout << result << '\n';//*/
        }
        /*---------------------------export_history-------------------------------*/
        else if(cmd == "export_history_server"){
            int Uidx;
            cin >> Uidx;
            string account_number;
            cin >> account_number;
            if(Uidx == -1){
                cout << "Error: No user logged in." << '\n';
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
            vector<string> Accs = Ucore.AccList(Uidx);
            bool check = false;
            for(auto &name : Accs){
                if(name == account_number){
                    check = true;
                    break;
                }
            }
            if(!check){
                cout << "Error: Account does not belong to user." << '\n';
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
            cout << "History exported to " << filename << '\n';

        }
        /*----------------------------------------------------------*/
        else if(cmd == "my_accounts_server"){
            int Uidx;
            cin >> Uidx;
            if(Uidx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            vector<string> Accs = Ucore.AccList(Uidx);
            for(auto &name : Accs){
                payload.push_back("get_balance_op");
                payload.push_back(name);
            }
            cout << runAdmin(payload) << '\n';
        }
        else if(cmd == "delete_my_account_server"){
            int Uidx;
            cin >> Uidx;
            string pass, name;
            cin >> name;
            cin >> pass;
            if(Uidx == -1){
                cout << "Error: No user logged in." << '\n';
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
                cout << result << '\n';
                continue;
            }
            if(Res[3][0] != '0'){
                cout << "Error: Account balance is positive" << '\n';
                continue;
            }
            int Acc_idx = Ucore.AccIDX(Uidx, name);
            if(Acc_idx == -1){
                cout << "Error: Account does not belong to user." << '\n';
                continue;
            }
            payload.push_back("delete_account_op");
            payload.push_back(name);
            payload.push_back(pass);
            result = runAdmin(payload);
            cout << result << '\n'; 

            if (isError(result)) {
                Ucore.RmvAcc(Uidx, Acc_idx);
                Ucore.changeScore(Uidx, -2);    
            }
      
        }
		else if (cmd == "deposit_to_server"){
            int Uidx;
            cin >> Uidx;
            string account_id;
            cin >> account_id;
            double amount;
            cin >> amount;
            payload.push_back("deposit");
            payload.push_back(account_id);
            payload.push_back(to_string(amount));
            string result = runAdmin(payload);
            cout << result << '\n';

            if (isError(result) && Uidx != -1) {
                Ucore.changeScore(Uidx, 1);
            }

        }
		else if (cmd == "withdraw_from_server"){
            int Uidx;
            cin >> Uidx;
            string account_id;
            double amount;
            cin >> account_id;
            cin >> amount;
            string password;
            cin >> password;
            if (Uidx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            vector <string> Accs = Ucore.AccList(Uidx);
            bool belong_to = false;
            for (auto& name : Accs){
                if (name == account_id){
                    belong_to = true;
                }
            }
            if (!belong_to){
                cout << "Error: Account does not belong to user." << '\n';
                continue;
            }
            payload.push_back("withdraw_op");
            payload.push_back(account_id);
            payload.push_back(to_string(amount));
            payload.push_back(password);
            result = runAdmin(payload);
            cout << result << '\n';

            if (isError(result)){
                Ucore.changeScore(Uidx, 1);
            }

        }
		else if (cmd == "send_money_server"){
            int Uidx;
            cin >> Uidx;
            double amount;
            string from_acc, to_acc;
            cin >> from_acc >> to_acc >> amount;
            string password;
            cin >> password;
            if (Uidx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            vector <string> Accs = Ucore.AccList(Uidx);
            bool belong_to = false;
            for (auto& name : Accs){
                if (name == from_acc){
                    belong_to = true;
                }
            }
            if (!belong_to){
                cout << "Error: source Account does not belong to user." << '\n';
                continue;
            }
            if (amount > maximum_transaction_normal){
                cout << "Error: Transaction limit exceeded." << '\n';
                continue;
            }
            payload.push_back("transfer_op");
            payload.push_back(from_acc);
            payload.push_back(to_acc);
            payload.push_back(to_string(amount));
            payload.push_back(password);
            result = runAdmin(payload);
            cout << result << '\n';
            if (isError(result)) {
                Ucore.changeScore(Uidx, 2);
            }

		}
		else if (cmd == "balance_inquiry_server"){
            int Uidx;
            cin >> Uidx;
            string account_id;
			cin >> account_id;
            if (Uidx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            vector<string> Accs = Ucore.AccList(Uidx);
            bool belong_to = false;
            for (auto &name : Accs){
                if (name == account_id){
                    belong_to = true;
                }
            }
            if (!belong_to){
                cout << "Error: Account does not belong to user." << '\n';
                continue;
            }
            payload.push_back("get_balance");
			payload.push_back(account_id);
			result = runAdmin(payload);
			cout << result << '\n';
            if (isError(result)){
                Ucore.changeScore(Uidx, 1); 
            }
		}
        else if(cmd == "my_rank_server"){
            int Uidx;
            cin >> Uidx;
            if(Uidx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            Ucore.ptrRank(Uidx);
        }
        else if(cmd == "delete_my_user_server"){
            int Uidx;
            cin >> Uidx;
            string pass;
            cin >> pass;
            if(Uidx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            if(Ucore.AccList(Uidx).size() != 0){
                cout << "Error: User has accounts." << '\n';
                continue;
            }
            if(Ucore.RmvUser(Uidx, pass)){
                User_idx = -1;
            }
            cout << '\n';
        }
        else if (cmd == "request_OTP_server"){
            int Uidx;
            cin >> Uidx;
            string account_id;
            cin >> account_id;
            if (Uidx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            vector <string> Accs = Ucore.AccList(Uidx);
            bool belong_to = false;
            for (auto& name : Accs){
                if (name == account_id){
                    belong_to = true;
                }
            }
            if (!belong_to){
                cout << "Error: Account does not belong to user." << '\n';
                continue;
            }
            long long OTP_start_time_in_MS = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
            if(OTP_start_time_in_MS - OTP_Time < 120000){
                cout << "OTP: " << OTP_Code << '\n';
                cout << "expires in " << 120 - ((OTP_start_time_in_MS - OTP_Time) / 1000ll) << " seconds\n";
                continue;
            }
            OTP_Time = OTP_start_time_in_MS;
            int OTP = rand() * rand();
            OTP = abs(OTP);
            OTP %= 1000000;
            OTP_Code = to_string(OTP);
            Ucore.SetOTP(Uidx, OTP);
            Ucore.SetOTPTime(Uidx, OTP_start_time_in_MS);
            cout << "OTP: " << OTP << '\n';
            cout << "expires in 120 seconds\n";
        }
        else if (cmd == "online_payment_server"){
            int Uidx;
            cin >> Uidx;
            double amount;
            string from_account, to_account;
            cin >> from_account >> to_account;
            cin >> amount;
            if (Uidx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            vector <string> Accs = Ucore.AccList(Uidx);
            bool belong_to = false;
            for (auto& name : Accs){
                if (name == from_account){
                    belong_to = true;
                }
            }
            if (!belong_to){
                cout << "Error: Account does not belong to user." << '\n';
                continue;
            }
            int entered_OTP;
            cin >> entered_OTP;
            if (entered_OTP != Ucore.getOTP(Uidx)){
                cout << "Error: Invalid OTP.\n";
                continue;
            }
            long long now_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
            if (now_time - Ucore.GetOTPTime(Uidx) > 120 * 1000){
                cout << "Error: OTP expired.\n";
                continue;
            }
            if (amount > maximum_transaction_online){
                cout << "Error: transaction limit exceeded.\n";
                continue;
            }
            payload.push_back("transfer_op_no_password");
            payload.push_back(from_account);
            payload.push_back(to_account);
            payload.push_back(to_string(amount));
            result = runAdmin(payload);
            cout << result << '\n';
        }
        else if (cmd == "show_iban_server"){
            int Uidx;
            cin >> Uidx;
            string account_id;
            cin >> account_id;
            if (Uidx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            vector <string> Accs = Ucore.AccList(Uidx);
            bool belong_to = false;
            int account_index;
            for (int i = 0; i < Accs.size(); ++ i){
                if (Accs[i] == account_id){
                    belong_to = true;
                    account_index = i;
                    break;
                }
            }
            if (!belong_to){
                cout << "Error: Account does not belong to user." << '\n';
                continue;
            }
            //account_id = removeDashes(account_id);
            cout << "IBAN: " << Ucore.add_iban(Uidx, account_index) << '\n';
        }
        else if (cmd == "paya_transfer_server"){
            int Uidx;
            cin >> Uidx;
            string from_account, destination_iban, pass;
            double amount;
            cin >> from_account >> destination_iban >> amount;
            //destination_iban = removeDashes(destination_iban);
            cin >> pass;
            if (Uidx == -1){
                cout << "Error: No user logged in." << '\n';
                continue;
            }
            vector <string> Accs = Ucore.AccList(Uidx);
            bool belong_to = false;
            for (auto& name : Accs){
                if (name == from_account){
                    belong_to = true;
                }
            }
            if (!belong_to){
                cout << "Error: source Account does not belong to user." << '\n';
                continue;
            }
            if (amount > maximum_transaction_paya){
                cout << "Error: Transaction limit exceeded." << '\n';
                continue;
            }
            payload.push_back("paya_transfer");
            payload.push_back(from_account);
            payload.push_back(destination_iban);
            payload.push_back(to_string(amount));
            payload.push_back(pass);
            cout << runAdmin(payload);
        }

        else {
            cout << "Error: Unknown command" << '\n';
        }
    }
}