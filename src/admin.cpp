#include <bits/stdc++.h>
#include <ctime>
#include <fstream>
#include "../include/picosha2.h"
#include "../include/json.hpp"

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

/*----------------------------------------------------*/

std::string removeDashes(std::string input) {
    input.erase(std::remove(input.begin(), input.end(), '-'), input.end());
    return input;
}

struct Account_Id{
    int n[4];

    string strid (){
        string s1 = to_string(n[0]), s2 = to_string(n[1]), s3 = to_string(n[2]), s4 = to_string(n[3]);
        return s1 + s2 + s3 + s4;
    }

    Account_Id (){
        for(int i = 0; i < 4; i++){
            n[i] = 0;
        }
    }
    Account_Id (int n1, int n2, int n3, int n4){
        this -> n[0] = n1;
        this -> n[1] = n2;
        this -> n[2] = n3;
        this -> n[3] = n4;
    }
    Account_Id (int BankID, int N){
        this -> n[0] = BankID;
        this -> n[1] = N / 1000000;
        this -> n[2] = (N % 1000000) / 1000;
        this -> n[3] = (N % 1000);
    }
    Account_Id (const string &ID){
        string tmp = "";
        for(int i = 0, j = 0, sz = (int)ID.size(); i < sz; i++){
            if(ID[i] == '-'){
                n[j++] = stoi(tmp);
                tmp = "";
                continue;
            }
            tmp += ID[i];
        }
        n[3] = stoi(tmp);
    }

    int& operator[] (int i){
        return n[i];
    }
    const int operator[] (int i) const{
        return n[i];
    }

    bool operator== (const Account_Id &A) const{
        for(int i = 0; i < 4; i++){
            if(n[i] != A[i]){
                return false;
            }
        }
        return true;
    }
    bool operator== (const string &Str) const{
        return (*this == Account_Id (Str));
    }
    bool operator!= (const Account_Id &A) const{
        return !(*this == A);
    }
    bool operator!= (const string &Str) const{
        return (*this != Account_Id (Str));
    }
    bool operator< (const Account_Id &A) const{
        for(int i = 0; i < 4; i++){
            if(n[i] != A[i]){
                return (n[i] < A[i]);
            }
        }
        return false;
    }
    bool operator<= (const Account_Id &A) const{
        return (*this < A || *this == A);
    }
    bool operator> (const Account_Id &A) const{
        for(int i = 0; i < 4; i++){
            if(n[i] != A[i]){
                return (n[i] > A[i]);
            }
        }
        return false;
    }
    bool operator>= (const Account_Id &A) const{
        return (*this > A || *this == A);
    }

    friend ostream& operator<< (ostream &O, const Account_Id &AI);
    string To_String(){
        string res = "";
        for(int i = 0; i < 4; i++){
            string Tmp = to_string(n[i]);
            for(int j = 0, sz = (int)Tmp.size(); j < sz - 4; j++){
                res += '0';
            }
            res += Tmp;
            if(i != 3){
                res += '-';
            }
        }
        return res;
    }

    Account_Id& operator= (const Account_Id &A){
        for(int i = 0; i < 4; i++){
            n[i] = A[i];
        }
        return (*this);
    }
    Account_Id& operator= (const string &Str){
        return (*this = Account_Id(Str));
    }
    Account_Id (const Account_Id &A){
        for(int i = 0; i < 4; i++){
            n[i] = A[i];
        }
    }
    ~Account_Id(){}
};
ostream& operator<< (ostream &O, const Account_Id &AI){
    for(int i = 0; i < 4; i++){
        string Tmp = to_string(AI[i]);
        for(int j = 0, sz = (int)Tmp.size(); j < sz - 4; j++){
            O << '0';
        }
        O << Tmp;
        if(i != 3){
            O << '-';
        }
    }
    return O;
}

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
    Time += Sec; // moshkel (it used to be minute, which seemed wrong, so changed it to second)
    return Time; 
};
string GetTime2(){
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
    return Time; 
};


struct Transaction{
    string Type;
    int ID;
    ld Val, BALANCE;
    string Time;
    string Origin, Destination;

    Transaction (){}

    Transaction (string Type, int ID, ld Val, ld BALANCE, string Origin, string Destination){
        this -> Type = Type;
        this -> ID = ID;
        this -> Val = Val;
        this -> BALANCE = BALANCE;
        this->Origin = Origin;
        this->Destination = Destination;
        Time = GetTime();
    }

    bool operator== (const Transaction &T) const{
        return (ID == T.ID);
    }
    bool operator!= (const Transaction &T) const{
        return (ID != T.ID);
    }
    bool operator< (const Transaction &T) const{
        return (ID < T.ID);
    }
    bool operator<= (const Transaction &T) const{
        return (ID <= T.ID);
    }
    bool operator> (const Transaction &T) const{
        return (ID > T.ID);
    }
    bool operator>= (const Transaction &T) const{
        return (ID >= T.ID);
    }

    ~Transaction (){}
};
/*---------------------------------------------------------*/
class Account{
    private:
        Account_Id AI;
        int Branch_Id;
        ld Coin;
        vector<Transaction> History;
        string IBAN; // save
        int mod97 (const string &s){
            int rem = 0;
            for (char c : s){
                rem = (rem * 10 + (c - '0')) % 97;
            }
            return rem;
        } 
        void make_IBAN (){
            string body = "000000" + AI.strid();
            string num = body + "182700";
            int check = 98 - mod97(num);
            string iban = "IR";
            if (check < 10){
                iban += "0";
            }
            iban += to_string(check);
            iban += body;
            IBAN = iban;
        }
    public:
        string HashPass;
        bool Active;

        string get_account_id (){
            return AI.strid();
        }

        string getIBAN () const{
            return IBAN;
        }
        void setIBAN (const string &iban){
            IBAN = iban;
        }

        Account (int BankID, int N, int Branch_Id, string HashPass, ld Coin = 0, bool Active = true){
            AI = Account_Id (BankID, N);
            this -> Branch_Id = Branch_Id;
            this -> HashPass = HashPass;
            this -> Coin = Coin;
            this -> Active = Active;
            make_IBAN ();
        }
        Account (int n1, int n2, int n3, int n4, int Branch_Id, string HashPass, ld Coin = 0, bool Active = true){
            AI = Account_Id (n1, n2, n3, n4);
            this -> Branch_Id = Branch_Id;
            this -> HashPass = HashPass;
            this -> Coin = Coin;
            this -> Active = Active;
            make_IBAN ();
        }
        Account (string Str, int Branch_Id, string HashPass, ld Coin = 0, bool Active = true){
            AI = Account_Id (Str);
            this -> Branch_Id = Branch_Id;
            this -> HashPass = HashPass;
            this -> Coin = Coin;
            this -> Active = Active;
            make_IBAN ();
        }
        int get_transactions_size (){
            return History.size();
        }
        Transaction& get_transaction (int idx){
            return History[idx];
        }
        void clear_history (){
            History.clear();
        }
        ld getCoin(){
            return Coin;
        }
        int getBranch(){
            return Branch_Id;
        }
        Account_Id getID() const{
            return AI;
        }
        string getIDStr() const {
            string res = "";
            for(int i = 0; i < 4; i++){
                string Tmp = to_string(AI[i]);
                for(int j = 0, sz = (int)Tmp.size(); j < sz - 4; j++){
                    res += '0';
                }
                res += Tmp;
                if(i != 3){
                    res += '-';
                }
            }
            return res;
        }

        void pushHistory(const Transaction &T){
            History.push_back(T);
        }
        void Deposit(ld val, const Transaction &T){
            Coin += val;
            History.push_back(T);
        }
        void WITHDRAWAL(ld val, const Transaction &T){
            Coin -= val;
            History.push_back(T);
        }

        bool operator== (const Account &A) const{
            return (AI == A.AI);
        }
        bool operator!= (const Account &A) const{
            return (AI != A.AI);
        }
        bool operator< (const Account &A) const{
            return (AI < A.AI);
        }
        bool operator<= (const Account &A) const{
            return (AI <= A.AI);
        }
        bool operator> (const Account &A) const{
            return (AI > A.AI);
        }
        bool operator>= (const Account &A) const{
            return (AI >= A.AI);
        }

        ~Account (){}//*/
};
/*---------------------------------------------------------*/
struct Request{
    string owner, Time, reason;
    int id, Branch_Id, status;

    Request (string owner, int id, int status, int Branch_Id){
        this -> owner = owner;
        this -> id = id;
        this -> status = status;
        this -> Branch_Id = Branch_Id;
        Time = GetTime();
    }
    string GetStatus() const{
        if(!status){
            return "PENDING";
        }
        if(status == 1){
            return "APPROVED";
        }
        if(status == 2){
            return "CANCELLED";
        }
        return "REJECTED";
    }

    bool operator== (const Request &R) const{
        return (id == R.id);
    }
    bool operator!= (const Request &R) const{
        return (id != R.id);
    }
    bool operator< (const Request &R) const{
        return (id < R.id);
    }

    ~Request (){}
};
struct Branch{
    vector<string> AIs;
    vector<Request> Requests;
    string name;
    int Id;
    
    Branch (string name, int Id){
        this -> name = name;
        this -> Id = Id;
    }

    bool operator== (const Branch &B) const{
        return (Id == B.Id);
    }
    bool operator!= (const Branch &B) const{
        return (Id != B.Id);
    }
    bool operator< (const Branch &B) const{
        return (Id < B.Id);
    }
    bool operator<= (const Branch &B) const{
        return (Id <= B.Id);
    }
    bool operator> (const Branch &B) const{
        return (Id > B.Id);
    }
    bool operator>= (const Branch &B) const{
        return (Id >= B.Id);
    }

    friend ostream& operator<< (ostream &O, const Branch &B);

    void Add_Request(const Request &R){
        Requests.push_back(R);
    }
    int RequestIDX(int Id){
        int l = 0, r = (int)Requests.size();
        if(Requests[r - 1].id < Id || Requests[l].id > Id){
            return -1;
        }
        while(l + 1 < r){
            int mid = (l + r) >> 1;
            if(Requests[mid].id <= Id){
                l = mid;
            }
            else{
                r = mid;
            }
        }
        if(Requests[l].id == Id){
            return l;
        }
        return -1;
    }
    bool isRepeat(string &codeMelli){
        for(auto &R : Requests){
            if(R.owner == codeMelli && R.status == 0){
                return true;
            }
        }
        return false;
    }
    int PendingCnt(){
        int res = 0;
        for(auto &R : Requests){
            res += (R.status == 0);
        }
        return res;
    }
    int RejectedToday(){
        string Time  = GetTime2();
        int sz = (int)Time.size(), res = 0;
        for(auto &R : Requests){
            if(R.Time.size() < sz){
                continue;
            }
            bool f = 1;
            for(int i = 0; i < sz && f == 1; i++){
                if(R.Time[i] != Time[i]){
                    f = 0;
                }
            }
            res += f;
        }
        return res;
    }
        
    Branch& operator= (const Branch &B){
        name = B.name;
        Id = B.Id;
        AIs = B.AIs;
        Requests = B.Requests;
        return (*this);
    }
    Branch (const Branch &B){
        name = B.name;
        Id = B.Id;
        AIs = B.AIs;
        Requests = B.Requests;
    }
    ~Branch (){}
};
ostream& operator<< (ostream &O, const Branch &B){
    O << B.Id << " | " << B.name;
    return O;
}
/*----------------------------------------------------------------------*/
string Hasher(string pass){
    return picosha2::hash256_hex_string(pass);
}
bool compare(string pass, string input){
    input = picosha2::hash256_hex_string(input);
    if(pass==input)
        return true ;
    return false ; 
}
struct Paya_Request {
    int id;
    string from_account;
    string destination_iban;
    long double amount;
    int status;          // 0=pending 1=approved 2=rejected
};
/*----------------------------------------------------------------------*/
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
/*----------------------------------------------------------------------*/
class Core{
    private:
        vector<User> Users;
        vector<Branch> Branches;
        vector<Account> FAccounts;
        vector<Account> BAccounts;
        vector<Transaction> Trans;
        vector<Paya_Request> paya_requests; // save
        int Account_Cnt, Trans_Cnt, Request_Cnt;
        ld transferFee, balanceInquiryFee;

        pair<int, pair<int, int>> RequestIDXs(int idx){
            int Tmp;
            for(int i = 0, sz = (int)Branches.size(); i < sz; i++){
                Tmp = Branches[i].RequestIDX(idx);
                if(Tmp != -1){
                    return make_pair(Branches[i].Id, make_pair(Tmp, i));
                }
            }
            return make_pair(-1, make_pair(-1, -1));
        }
        int FAccountsIDX(string &AI){
            for(int i = 0, sz = (int)FAccounts.size(); i < sz; i++){
                if(AI == FAccounts[i].getIDStr()){
                    return i;
                }
            }
            return -1;
        }
        int BAccountsIDX(string &AI){
            for(int i = 0, sz = (int)BAccounts.size(); i < sz; i++){
                if(AI == BAccounts[i].getIDStr()){
                    return i;
                }
            }
            return -1;
        }
        int ActiveAccountCnt(int idx){
            int res = 0;
            for(auto &AI : Branches[idx].AIs){
                res += (FAccountsIDX(AI) != -1);
            }
            return res;
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
    public:
        int BankID;
        string getstatus (int i){
            if (i == 0){
                return "pending";
            }
            if (i == 1){
                return "approved";
            }
            return "rejected";
        }
        void list_paya (){
            for (int i = 0; i < paya_requests.size(); ++ i){
                cout << "Source Account " << paya_requests[i].from_account << endl << "Destination IBAN: " << paya_requests[i].destination_iban << endl << "Amount: " << paya_requests[i].amount << endl << "Status: " << getstatus(paya_requests[i].status) << endl << "Request ID: " << paya_requests[i].id << endl;
            }
            return;
        }
        string get_id_from_iban (string id){
            for(int i = 0; i < FAccounts.size(); ++ i){
                //cout << FAccounts[i].getIBAN() << " " << FAccounts[i].get_account_id() << endl;
                if (FAccounts[i].getIBAN() == id){
                     return FAccounts[i].get_account_id();
                }
            }
            return "NULL";
        }
        void approve_paya (int payaid){
            for (int i = 0; i < paya_requests.size(); ++ i){
                if (paya_requests[i].id == payaid){
                    if (paya_requests[i].status){
                        cout << "Error: paya request is not pending." << endl;
                        return;
                    }
                    paya_requests[i].status = 1;
                    string id = get_id_from_iban(paya_requests[i].destination_iban);
                    if (id == "NULL"){
                        cout << "Error: no account exists with this IBAN." << endl;
                        paya_requests[i].status = 0;
                        return;
                    }
                    Deposit (id, paya_requests[i].amount);
                    cout << "Paya approved. Transaction ID: " << payaid << '\n';
                    break;
                }
            }
            return;
        }
        void reject_paya (int payaid){
            for (int i = 0; i < paya_requests.size(); ++ i){
                if (paya_requests[i].id == payaid){
                    if (paya_requests[i].status){
                        cout << "Error: paya request is not pending." << endl;
                        return;
                    }
                    paya_requests[i].status = 2;
                    Deposit (paya_requests[i].from_account, paya_requests[i].amount);
                    cout << "Paya rejected. Amount returned to source account.\n";
                    break;
                }
            }
            return;
        }
        int new_request (){
            Request_Cnt ++;
            write ();
            return Request_Cnt;
        }
        int add_paya (Paya_Request paya, string pass){
            paya_requests.push_back(paya);
            int status = WITHDRAWAL_no_output (paya.from_account, pass, paya.amount);
            if (status){
                //Deposit (paya.from_account, paya.amount);
                paya_requests.pop_back();
                return 1;
            }
            return 0;
        }

        int IBANIDX (string &iban){
           for(int i = 0; i < FAccounts.size(); ++ i){
                if (FAccounts[i].getIBAN() == iban){
                     return i;
                }
            }
            return -1;
        }

        Core(int BankID = 5022){
            this -> BankID = BankID;
            Account_Cnt = 0;
            Trans_Cnt = 1001;
            transferFee = 0.00;
            Request_Cnt = 0;
            balanceInquiryFee = 0.00 ;
            read();//load
            read_setting();
            read_users();
        }
        
        ld Get_Transfer_Fee(){
            return transferFee;
        }
        ld Get_Balance_Inquiry_Fee(){
            return balanceInquiryFee;
        }

        void Add_Branch(string &name, bool Print = 1){
            int id = 10001 + (int)Branches.size();
            Branches.push_back(Branch (name, id));
            if(Print){
                cout << "Branch created. ID: " << id << '\n';
            }
            write();//save
        }
        void List_Branch(){
            if(Branches.empty()){
                cout << "Error: No branches available" << '\n';
                return;
            }
            for(auto &B : Branches){
                cout << B << '\n';
            }
        } 
        bool isBranch(ll Id){
            return (Id >= 10001 && Id < 10001 + (int)Branches.size());
        }
        void Branch_Dashboard(ll branch_id){
            if(!isBranch(branch_id)){
                cout << "Error: Branch not found." << '\n';
                return;
            }
            int idx = branch_id - 10001;
            cout << "Branch: " << Branches[idx].name << "\n Active accounts: " << ActiveAccountCnt(idx)
            << "\n Pending requests: " << Branches[idx].PendingCnt() << "\n Rejected (today): "
            << Branches[idx].RejectedToday() << '\n';
        }
        void Add_Request(ll Id, string &codeMelli){
            const int idx = Id - 10001;
            if(Branches[idx].isRepeat(codeMelli)){
                cout << "Error: You already have a pending or active account in this branch." << '\n';
                return;
            }
            Branches[idx].Add_Request(Request(codeMelli, Request_Cnt, 0, Id));
            Request_Cnt++;
            cout << "Request submitted. ID: " << Request_Cnt - 1 << '\n';
            write();
        }
        void printUserRequest(int idx){
            auto [branch_Id, Tmp] = RequestIDXs(idx);
            auto [req_idx, branch_id] = Tmp;
            if(branch_Id != -1 && Branches[branch_id].Requests[req_idx].status != 3){
                cout << idx << " | Branch: " << branch_Id << " | Status: " << 
                Branches[branch_id].Requests[req_idx].GetStatus() << 
                " | " << Branches[branch_id].Requests[req_idx].Time << '\n';
                return;
            }
            else if(branch_Id != -1){
                cout << idx << " | Branch: " << branch_Id << " | Status: " << 
                Branches[branch_id].Requests[req_idx].GetStatus() << 
                " | " << Branches[branch_id].Requests[req_idx].reason << '\n';
                return;
            }
            cout << "Error: Couldn't find the request" << '\n';
        }
        void ListRequests(ll branch_Id){
            if(!isBranch(branch_Id)){
                cout << "Error: Branch not found." << '\n';
                return;
            }
            const int branch_id = branch_Id - 10001;
            if(!Branches[branch_id].PendingCnt()){
                cout << "No pending requests for this branch." << '\n';
                return;
            }
            for(auto &R : Branches[branch_id].Requests){
                if(!R.status){
                    cout << R.id << " | User: " << R.owner << " | Branch: " << branch_Id << " | " << 
                    R.Time << "PENDING\n";
                }
            }
        }
        void Cancel_Request(int idx, string &codeMelli){
            auto [branch_Id, Tmp] = RequestIDXs(idx);
            auto [req_idx, branch_id] = Tmp;
            if(req_idx == -1){
                cout << "Error: Request not found." << '\n';
                return;
            }
            if(Branches[branch_id].Requests[req_idx].owner != codeMelli){
                cout << "Error: Request does not belong to user." << '\n';
                return;
            }
            if(Branches[branch_id].Requests[req_idx].status){
                cout << "Error: Request is not cancellable." << '\n';
                return;
            }
            Branches[branch_id].Requests[req_idx].status = 2;
            cout << "Request " << idx << " cancelled." << '\n';
        }
        void Is_Request_Usable(int idx, string &codeMelli){
            auto [branch_Id, Tmp] = RequestIDXs(idx);
            auto [req_idx, branch_id] = Tmp;
            if(req_idx == -1){
                cout << "Error: Request not found." << '\n';
                return;
            }
            if(Branches[branch_id].Requests[req_idx].owner != codeMelli){
                cout << "Error: Request does not belong to user." << '\n';
                return;
            }
            if(Branches[branch_id].Requests[req_idx].status != 1){
                cout << "Error: Request is not approved." << '\n';
                return;
            }
            cout << "Branch Id: " << branch_Id << '\n';
        }
        void Approve_Request(int request_Id){
            auto [branch_Id, Tmp] = RequestIDXs(request_Id);
            auto [req_idx, branch_id] = Tmp;
            if(branch_Id == -1){
                cout << "Error: Request not found." << '\n';
                return;
            }
            if(Branches[branch_id].Requests[req_idx].status){
                cout << "Error: Request is not pending." << '\n';
                return;
            }
            Branches[branch_id].Requests[req_idx].status = 1;
            cout << "Request " << request_Id << " approved. Waiting for user activation." << '\n';
            write();
        }
        void Reject_Request(int request_Id, string &Reason){
            auto [branch_Id, Tmp] = RequestIDXs(request_Id);
            auto [req_idx, branch_id] = Tmp;
            if(branch_Id == -1){
                cout << "Error: Request not found." << '\n';
                return;
            }
            if(Branches[branch_id].Requests[req_idx].status){
                cout << "Error: Request is not pending." << '\n';
                return;
            }
            Branches[branch_id].Requests[req_idx].reason = Reason;
            Branches[branch_id].Requests[req_idx].status = 3;
            cout << "Request " << request_Id << " rejected." << '\n';
        }

        void show_fees(){
            cout << fixed << setprecision(2);
            cout << "Transfer fee: " << transferFee << '\n';
            cout << "Balance inquiry fee: " << balanceInquiryFee << '\n';
        }
        void set_transfer_fee(ld amount){
            if(amount < 0){
                cout << "Error: Invalid fee amount." << '\n';
                return;
            }
            transferFee = amount;
            cout << fixed << setprecision(2) ; 
            cout << "Transfer fee set to " << amount << '\n';
            write_setting();
        }
        void set_balance_inquiry_fee(ld amount){
            if(amount < 0){
                cout << "Error: Invalid fee amount." << '\n';
                return;
            }
            balanceInquiryFee = amount;
            cout << fixed << setprecision(2) ; 
            cout << "Balance inquiry fee set to " << amount << '\n';
            write_setting(); 
        }

        void Create_Account(int Branch_Id, string &Pass){
            if(!isBranch(Branch_Id)){ 
                cout << "Error: The branch doesnt exist" << '\n'; 
                return; 
            }
            FAccounts.push_back(Account (BankID, Account_Cnt++, Branch_Id, Hasher(Pass)));
            cout << "Account created. Number: " << FAccounts.back().getID() << '\n';
            Branches[Branch_Id - 10001].AIs.push_back(FAccounts.back().getIDStr());
            write();
        }
        void Close_Account(string &Pass, string &ID){
            Account_Id AI(ID);
            for(auto &A : BAccounts){
                if(AI == A.getID()){
                    cout << "Account Already Closed" << '\n';
                    return;
                }
            }
            for(int i = 0, sz = (int)FAccounts.size(); i < sz; i++){
                if(AI == FAccounts[i].getID()){
                    if(!compare(FAccounts[i].HashPass, Pass)){
                        cout << "Error: Wrong password." << '\n';
                        return;
                    }
                    FAccounts[i].Active = false;
                    BAccounts.push_back(FAccounts[i]);
                    FAccounts.erase(FAccounts.begin() + i);
                    cout << "Account closed." << '\n';
                    write();
                    return;
                }
            }
            cout << "Error: Account not found." << '\n';
        }
        void Delete_Account(string &Pass, string &ID){
            Account_Id AI(ID);
            for(int i = 0, sz = (int)BAccounts.size(); i < sz; i++){
                if(AI == BAccounts[i].getID()){
                    if(!compare(BAccounts[i].HashPass, Pass)){
                        cout << "Error: Wrong password." << '\n';
                        return;
                    }
                    BAccounts.erase(BAccounts.begin() + i);
                    cout << "Account deleted." << '\n';
                    write();
                    return;
                }
            }
            for(int i = 0, sz = (int)FAccounts.size(); i < sz; i++){
                if(AI == FAccounts[i].getID()){
                    if(!compare(FAccounts[i].HashPass, Pass)){
                        cout << "Error: Wrong password." << '\n';
                        return;
                    }
                    FAccounts.erase(FAccounts.begin() + i);
                    cout << "Account deleted." << '\n';
                    write();
                    return;
                }
            }
            cout << "Error: Account not found." << '\n';
        }
        void Account_List(){
            cout << fixed << setprecision(2);
            for(auto &A : FAccounts){
                cout << A.getID() << " | Branch: " << A.getBranch() << " | Active: Yes | Balance: " << A.getCoin() << '\n';
            }
            for(auto &A : BAccounts){
                cout << A.getID() << " | Branch: " << A.getBranch() << " | Active: No | Balance: " << A.getCoin() << '\n';
            }
        }
        void Deposit(string &Num, ld val){
            ///cout << endl << Num << " --" << endl;
            Account_Id AI(Num);
            for(auto &A : BAccounts){
                if(removeDashes(Num) == A.getID().strid()){
                    cout << "Error: Account is inactive." << '\n';
                    return;
                }
            }
            for(auto &A : FAccounts){
                ///cout << AI.strid() << " " << A.getID().strid() << endl;
                if(removeDashes(Num) == A.getID().strid()){
                    Transaction T("DEPOSIT", Trans_Cnt++, val, A.getCoin() + val, Num, Num);
                    cout << "Transaction ID: " << T.ID << '\n';
                    cout << fixed << setprecision(2) << "New balance: " << A.getCoin() + val << '\n';
                    A.Deposit(val, T);
                    Trans.push_back(T);
                    write();
                    return;
                }
            }
            cout << "Error: Account not found." << '\n';
        }
        int WITHDRAWAL(string &Num, string &Pass, ld val){
            Account_Id AI(Num);
            for(auto &A : BAccounts){
                if(removeDashes(Num) == A.getID().strid()){
                    cout << "Error: Account is inactive." << '\n';
                    return 1;
                }
            }
            for(auto &A : FAccounts){
                if(removeDashes(Num) == A.getID().strid()){
                    if(!compare(A.HashPass, Pass)){
                        cout << "Error: Wrong password." << '\n'; 
                        return 1;
                    }
                    if(A.getCoin() < val){
                        cout << "Error: Insufficient funds." << '\n';
                        return 1;
                    }
                    Transaction T("WITHDRAWAL", Trans_Cnt++, -val, A.getCoin() - val, Num, Num);
                    cout << "Transaction ID: " << T.ID << '\n';
                    cout << fixed << setprecision(2) << "New balance: " << A.getCoin() - val << '\n';
                    A.WITHDRAWAL(val, T);
                    Trans.push_back(T);
                    write();
                    return 0;
                }
            }
            cout << "Error: Account not found." << '\n';
            return 1;
        }
        
        int WITHDRAWAL_no_output (string &Num, string &Pass, ld val){
            Account_Id AI(Num);
            for(auto &A : BAccounts){
                if(removeDashes(Num) == A.getID().strid()){
                    cout << "Error: Account is inactive." << '\n';
                    return 1;
                }
            }
            for(auto &A : FAccounts){
                if(removeDashes(Num) == A.getID().strid()){
                    if(!compare(A.HashPass, Pass)){
                        cout << "Error: Wrong password." << '\n'; 
                        return 1;
                    }
                    if(A.getCoin() < val){
                        cout << "Error: Insufficient funds." << '\n';
                        return 1;
                    }
                    Transaction T("WITHDRAWAL", Trans_Cnt++, -val, A.getCoin() - val, Num, Num);
                    //cout << "Transaction ID: " << T.ID << '\n';
                    //cout << fixed << setprecision(2) << "New balance: " << A.getCoin() - val << '\n';
                    A.WITHDRAWAL(val, T);
                    Trans.push_back(T);
                    write();
                    return 0;
                }
            }
            cout << "Error: Account not found." << '\n';
            return 1;
        }

        void Transfer(string &Num1, string &Num2, string &Pass, ld val){
            Account_Id AI1(Num1), AI2(Num2);
            for(auto &A : BAccounts){
                if(AI1 == A.getID()){
                    cout << "Error: Account is inactive." << '\n';
                    return;
                }
                if(AI2 == A.getID()){
                    cout << "Error: Destination account is inactive" << '\n';
                    return;
                }
            }
            int j = -1;
            for(int i = 0, sz = (int)FAccounts.size(); i < sz; i++){
                if(AI2 == FAccounts[i].getID()){
                    j = i;
                    break;
                }
            }
            if(j == -1){
                cout << "Error: Destination account not found." << '\n';
                return;
            }
            for(auto &A : FAccounts){
                if(AI1 == A.getID()){
                    if(!compare(A.HashPass, Pass)){
                        cout << "Error: Wrong password." << '\n'; 
                        return;
                    }
                    if(A.getCoin() < val){
                        cout << "Error: Insufficient funds." << '\n';
                        return;
                    }
                    Transaction T1("TRANSFER", Trans_Cnt, -val, A.getCoin() - val, Num1, Num2);
                    Transaction T2("TRANSFER", Trans_Cnt++, val, FAccounts[j].getCoin() + val, Num1, Num2);
                    cout << "Transaction ID: " << T1.ID << '\n';
                    cout << fixed << setprecision(2) << "New balance: " << A.getCoin() - val << '\n';
                    A.WITHDRAWAL(val, T1);
                    FAccounts[j].Deposit(val, T2);
                    Trans.push_back(T1);
                    write();
                    return;
                }
            }
            cout << "Error: Account not found." << '\n';
        }
        void Transfer_no_password (string &Num1, string &Num2, ld val){
            Account_Id AI1(Num1), AI2(Num2);
            for(auto &A : BAccounts){
                if(AI1 == A.getID()){
                    cout << "Error: Account is inactive." << '\n';
                    return;
                }
                if(AI2 == A.getID()){
                    cout << "Error: Destination account is inactive" << '\n';
                    return;
                }
            }
            int j = -1;
            for(int i = 0, sz = (int)FAccounts.size(); i < sz; i++){
                if(AI2 == FAccounts[i].getID()){
                    j = i;
                    break;
                }
            }
            if(j == -1){
                cout << "Error: Destination account not found." << '\n';
                return;
            }
            for(auto &A : FAccounts){
                if(AI1 == A.getID()){
                    if(A.getCoin() < val){
                        cout << "Error: Insufficient funds." << '\n';
                        return;
                    }
                    Transaction T1("TRANSFER", Trans_Cnt, -val, A.getCoin() - val, Num1, Num2);
                    Transaction T2("TRANSFER", Trans_Cnt++, val, FAccounts[j].getCoin() + val, Num1, Num2);
                    cout << "Transaction ID: " << T1.ID << '\n';
                    cout << fixed << setprecision(2) << "New balance: " << A.getCoin() - val << '\n';
                    A.WITHDRAWAL(val, T1);
                    FAccounts[j].Deposit(val, T2);
                    Trans.push_back(T1);
                    write();
                    return;
                }
            }
            cout << "Error: Account not found." << '\n';
        }
        void give_balance(int idx, bool isBlocked, bool SPECIAL = 0){
            if(!isBlocked){
                if(SPECIAL == 0 && FAccounts[idx].getCoin() < balanceInquiryFee){
                    cout << "Error: Insufficient funds." << '\n';
                    return;
                }
                if(SPECIAL == 0){
                    Transaction T("Balance inquiry fee", Trans_Cnt++, -balanceInquiryFee, FAccounts[idx].getCoin() - balanceInquiryFee, 
                    FAccounts[idx].getIDStr(), "Bank");
                    FAccounts[idx].WITHDRAWAL(balanceInquiryFee, T);
                    cout << "Balance inquiry fee: " << balanceInquiryFee << '\n';
                    cout << fixed << setprecision(2);
                    cout << "Balance: " << FAccounts[idx].getCoin() << '\n';
                    cout << "Active: Yes" << '\n';
                    cout << "Branch: " << FAccounts[idx].getBranch() << '\n';
                    cout << defaultfloat << setprecision(6);
                }
                else{
                    cout << FAccounts[idx].getIDStr() << " | " << "Balance: " << FAccounts[idx].getCoin() << '\n';
                }
                write();
                return;
            }     
            if(SPECIAL == 0){   
                cout << fixed << setprecision(2);
                cout << "Balance: " << BAccounts[idx].getCoin() << '\n';
                cout << "Active: No" << '\n';
                cout << "Branch: " << BAccounts[idx].getBranch() << '\n';
                cout << setprecision(6);
                return;
            }
            cout << BAccounts[idx].getIDStr() << " | " << "Balance: " << FAccounts[idx].getCoin() << '\n';
        }
        void get_balance (string s, bool SPECIAL = 0){
            Account_Id accid(s);
            int idx = -1;
            for (int i = 0; i < (int)FAccounts.size(); ++ i){
                if(FAccounts[i].getID() == accid){
                    idx = i;
                    break;
                }
            }
            if(idx != -1){
                return give_balance(idx, 0, SPECIAL);
            }
            else{
                for (int i = 0; i < (int)BAccounts.size(); ++ i){
                    if(BAccounts[i].getID() == accid){
                        idx = i;
                        break;
                    }
                }
                if(idx == -1){ 
                    cout << "Error: Account not found." << '\n'; return;
                 }
                give_balance(idx, 1);
            }
        }
        void get_history (string s){
            int idx = -1;
            Account_Id accid(s);
            for (int i = 0; i < (int)FAccounts.size(); ++ i){
                if(FAccounts[i].getID() == accid){
                    idx = i;
                    break;
                }
            }
            if(idx != -1){
                cout << fixed << setprecision(2);
                for (int i = 0; i < FAccounts[idx].get_transactions_size(); ++ i){
                    Transaction trns = FAccounts[idx].get_transaction(i);
                    cout << trns.ID << " | " << trns.Time << " | " << trns.Type << " | ";
                    if(trns.Val >= 0){
                        cout << '+';
                    }
                    cout << trns.Val << " | Balance: " << trns.BALANCE << '\n';
                }
            }
            else{
                for (int i = 0; i < (int)BAccounts.size(); ++ i){
                    if(BAccounts[i].getID() == accid){
                        idx = i;
                        break;
                    }
                }
                if(idx == -1){ cout << "Error: Account not found." << '\n'; return; }
                cout << fixed << setprecision(2);
                for (int i = 0; i < BAccounts[idx].get_transactions_size(); ++ i){
                    Transaction trns = BAccounts[idx].get_transaction(i);
                    cout << trns.ID << " | " << trns.Time << " | " << trns.Type << " | ";
                    if(trns.Val >= 0){
                        cout << '+';
                    }
                    cout << trns.Val << " | Balance: " << trns.BALANCE << '\n';
                }
            }
        }
        void get_transaction (int id){
            bool found = false;
            for (int i = 0; i < (int)Trans.size(); ++ i){
                if(Trans[i].ID == id){
                    found = true;
                    cout << fixed << setprecision(2);
                    cout << "ID: "             << id << '\n';
                    cout << "Time: "           << Trans[i].Time << '\n';
                    cout << "Type: "           << Trans[i].Type << '\n';
                    cout << "From: "           << Trans[i].Origin << '\n';
                    cout << "To: "             << Trans[i].Destination << '\n';
                    cout << "Amount: "         << Trans[i].Val << '\n';
                    cout << "Balance after: "  << Trans[i].BALANCE << '\n';
                    cout << setprecision(6);
                    break;
                }
            }
            if(!found){
                cout << "Error: Transaction not found." << '\n';
            }
        }
        void clear_history (string s){
            Account_Id accid(s);
            int idx = -1;
            for (int i = 0; i < (int)FAccounts.size(); ++ i){
                if(FAccounts[i].getID() == accid){
                    idx = i;
                }
            }
            if(idx != -1){
                string pass;
                cout << "Enter password:" << '\n';
                cin >> pass;
                if(compare(FAccounts[idx].HashPass, pass)){
                    FAccounts[idx].clear_history();
                    cout << "History cleared for " << s << '\n';
                    write();
                }
                else{
                    cout << "Error: Wrong password." << '\n';
                }
            }
            else{
                for (int i = 0; i < (int)BAccounts.size(); ++ i){
                    if(BAccounts[i].getID() == accid){
                        idx = i;
                    }
                }
                if(idx == -1){ cout << "Error: Account not found." << '\n'; return; }
                string pass;
                cout << "Enter password:" << '\n';
                cin >> pass;
                if(compare(BAccounts[idx].HashPass, pass)){
                    BAccounts[idx].clear_history();
                    cout << "History cleared for " << s << '\n';
                    write();
                }
                else{
                    cout << "Error: Wrong password." << '\n';
                }
            }
        }
        void reset_all (){
            cout << "Are you sure? This deletes everything. (yes/no): " << '\n';
            string is_sure;
            cin >> is_sure;
            if(is_sure == "yes"){
                remove("../data/Users.json");
                remove("../data/Bank_Data.json");
                Users.clear();
                Branches.clear();
                FAccounts.clear();
                BAccounts.clear();
                paya_requests.clear();
                Trans.clear();
                Account_Cnt = 0;
                Trans_Cnt = 1001;
                Request_Cnt = 0;
                transferFee = 0;
                balanceInquiryFee = 0;
                write();
                write_setting();
                cout << "All data cleared." << '\n';
            }
            else{
                cout << "Cancelled." << '\n';
            }
        }

        void show_ranking(){
            vector<User> sortedUsers = Users;
            sort(sortedUsers.begin(), sortedUsers.end());
            for(int i = 0, sz = (int)Users.size(); i < sz; i++){
                cout << i + 1 << " | " << Users[i].codeMelli << " | " << Users[i].score << 
                " | " << getLevel(Users[i].score) << '\n';
            }
        }
    /*--------------------------------------------------*/
    void read(){
        ifstream inFile("../data/BankـData.json");
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
    void write(){    
        json j ; 
        json jBranches = json::array();
        for(auto &B : Branches){
            json jRequests = json::array();
            for(auto &R : B.Requests){
                jRequests.push_back({
                    {"id", R.id},
                    {"owner", R.owner},
                    {"status", R.status},
                    {"Branch_Id", R.Branch_Id},
                    {"time", R.Time},
                    {"reason", R.reason}
                });
            }
            jBranches.push_back({
                {"id", B.Id},
                {"name", B.name},
                {"accounts", B.AIs},
                {"requests", jRequests}
            });
        }
        j["Branches"] = jBranches;
        json jFAccounts = json::array();
        for(auto &A : FAccounts){
            json jHistory = json::array();
            for(int i = 0; i < A.get_transactions_size(); i++){
                Transaction &T = A.get_transaction(i);
                jHistory.push_back({
                    {"type",T.Type},
                    {"id",T.ID},
                    {"val",(double)T.Val},
                    {"balance",(double)T.BALANCE},
                    {"time",T.Time},
                    {"origin",T.Origin},
                    {"destination",T.Destination}
                });
            }
            jFAccounts.push_back({
                {"id",A.getIDStr()},
                {"branch", A.getBranch()},
                {"hash_pass", A.HashPass},
                {"coin", (double)A.getCoin()},
                {"active", true},
                {"history", jHistory}
            });
        }
        j["active_accounts"] = jFAccounts;
        json jBAccounts = json::array();
        for(auto &A : BAccounts){
            json jHistory = json::array();
            for(int i = 0; i < A.get_transactions_size(); i++){
                Transaction &T = A.get_transaction(i);
                jHistory.push_back({
                    {"type",T.Type},
                    {"id", T.ID},
                    {"val", (double)T.Val},
                    {"balance", (double)T.BALANCE},
                    {"time", T.Time},
                    {"origin", T.Origin},
                    {"destination", T.Destination}
                });
            }
            jBAccounts.push_back({
                {"id", A.getIDStr()},
                {"branch", A.getBranch()},
                {"hash_pass", A.HashPass},
                {"coin", (double)A.getCoin()},
                {"active", false},
                {"history", jHistory}
            });
        }
        j["closed_accounts"] = jBAccounts; 

        json jpaya = json::array();
        for (auto &p : paya_requests){
            jpaya.push_back({
                {"id", p.id},
                {"from_account", p.from_account},
                {"destination_iban", p.destination_iban},
                {"amount", p.amount},
                {"status", p.status}
            });
        }

        json jTrans = json::array();
        for(auto &T : Trans){
            jTrans.push_back({
                {"type", T.Type},
                {"id", T.ID},
                {"val", (double)T.Val},
                {"balance", (double)T.BALANCE},
                {"time", T.Time},
                {"origin", T.Origin},
                {"destination", T.Destination}
            });
        }
        j["paya"] = jpaya;
        j["transactions"] = jTrans;
        j["Account_Cnt"] = Account_Cnt;
        j["Trans_Cnt"]   = Trans_Cnt;
        j["Request_Cnt"] = Request_Cnt;

        ofstream outFile("../data/BankـData.json");
        outFile << j.dump(4);
        outFile.close();
    }
    /*--------------------------------------------------*/
    void read_setting() {
        ifstream inFile("../data/setting.json");
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
    void write_setting(){
        json j;
        j["transfer_fee"] = transferFee;
        j["balance_inquiry_fee"] = balanceInquiryFee;
        ofstream inFile("../data/setting.json");
        inFile << j.dump(4);
        inFile.close();
    }
    /*--------------------------------------------------*/
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
};
/*------------------------------------------------------------------*/
bool isAccNumber(string &name){
    vector<int> idx;
    for(int i = 0, sz = (int)name.size(); i < sz; i++){
        if(name[i] != '-' && (name[i] < '0' || name[i] > '9')){
            return false;
        }
        else if(name[i] == '-'){
            idx.push_back(i);
            if((int)idx.size() > 3){
                return false;
            }
        }
    }
    if((int)idx.size() != 3){
        return false;
    }
    if(idx[0] == 0 || idx[2] == (int)name.size() - 1 || idx[1] == idx[0] + 1 || idx[2] == idx[1] + 1){
        return false;
    }
    return true;
}

int main(){
    //ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
    Core core;
    string cmd;
    while(cin >> cmd){
        if(cmd=="EOF"){
            core.write();
            exit(0);
        }
        /*-------------Branches----------------------*/
        else if(cmd == "create_branch"){
            string name, Tmp;
            cin >> Tmp;
            while(Tmp.back() != '"'){
                name += Tmp;
                cin >> Tmp;
            }
            name += Tmp;
            name = name.substr(1);
            name.pop_back();
            core.Add_Branch(name);
            continue; 
        }
        else if(cmd == "list_branches"){
            core.List_Branch();
            continue ; 
        }
        else if(cmd == "branch_dashboard"){
            ll branch_id;
            cin >> branch_id;
            core.Branch_Dashboard(branch_id);
            continue;            
        }
        else if(cmd == "is_branch_op"){
            ll id;
            cin >> id;
            if(core.isBranch(id)){
                cout << "Yes" << '\n';
            }
            else{
                cout << "No" << '\n';
            }
            continue;
        }
        /*-------------------------------------------*/
        /*------------------Requests-----------------------*/
        else if(cmd == "list_requests"){
            ll branch_id;
            cin >> branch_id;
            core.ListRequests(branch_id);
            continue;
        }
        else if(cmd == "add_account_request_op"){
            string codeMelli;
            ll branch_id;
            cin >> codeMelli >> branch_id;
            core.Add_Request(branch_id, codeMelli);
            continue;
        }
        else if(cmd == "print_request_op"){
            int Id;
            cin >> Id;
            core.printUserRequest(Id);
            continue;
        }
        else if(cmd == "cancel_request_op"){
            string codeMelli;
            int request;
            cin >> codeMelli >> request;
            core.Cancel_Request(request, codeMelli);
            continue;
        }
        else if(cmd == "approve_request"){
            int request;
            cin >> request;
            core.Approve_Request(request);
            continue;
        }
        else if(cmd == "reject_request"){
            int request;
            cin >> request;
            string reason;
            cout << "Enter rejection reason: " << '\n';
            cin >> reason;
            core.Reject_Request(request, reason);
            continue;
        }
        else if(cmd == "is_request_usable_op"){
            string codeMelli;
            int request;
            cin >> codeMelli >> request;
            core.Is_Request_Usable(request, codeMelli);
            continue;
        }
        /*-----------------------------------------------------*/
        /*----------------Accounts-----------------------------*/
        else if(cmd == "create_account"){
            int num; 
            cin >> num;
            cout << "Enter password:" << '\n';
            string pass;
            cin >> pass;
            core.Create_Account(num,pass);
            continue ; 
        }
        else if(cmd == "close_account"){
            string id; 
            cin >> id;
            cout << "Enter password:" << '\n';
            string pass;
            cin >> pass;
            if(!isAccNumber(id)){
                cout << "Error: Invalid account number." << '\n';
                continue;
            }
            core.Close_Account(pass, id);
            continue ; 
        }
        else if (cmd == "delete_account"){
            string id; 
            cin >> id;
            cout << "Enter password:" << '\n';
            string pass;
            cin >> pass;
            if(!isAccNumber(id)){
                cout << "Error: Invalid account number." << '\n';
                continue;
            }
            core.Delete_Account(pass, id);
            continue ; 
        }
        else if(cmd == "create_account_op"){
            int num; 
            cin >> num;
            string pass;
            cin >> pass;
            core.Create_Account(num,pass);
            continue ; 
        }
        else if(cmd == "close_account_op"){
            string id; 
            cin >> id;
            string pass;
            cin >> pass;
            if(!isAccNumber(id)){
                cout << "Error: Invalid account number." << '\n';
                continue;
            }
            core.Close_Account(pass, id);
            continue ; 
        }
        else if(cmd == "delete_account_op"){
            string id; 
            cin >> id;
            string pass;
            cin >> pass;
            if(!isAccNumber(id)){
                cout << "Error: Invalid account number." << '\n';
                continue;
            }
            core.Delete_Account(pass, id);
            continue ; 
        }
        else if(cmd == "list_accounts"){
            core.Account_List();
            continue ; 
        }
        /*--------------------------------------------------------------*/
        /*--------------------------Transfer Fees---------------------------------*/
        else if(cmd == "set_transfer_fee"){
            ld amount;
            cin >> amount;
            core.set_transfer_fee(amount);
            continue;
        }
        else if(cmd == "set_balance_inquiry_fee"){
            ld amount;
            cin >> amount;
            core.set_balance_inquiry_fee(amount);
            continue;
        }
        else if(cmd == "show_fees"){
            core.show_fees();
            continue;
        }
        /*---------------------------------------------------------------*/
        /*----------------------------Changing Balance----------------------------------------*/
        else if(cmd == "deposit"){
            string num; 
            double val;
            cin >> num >> val;
            if(val <= 0){
                cout << "Error: Amount must be positive." << '\n';
                continue;
            }
            if(!isAccNumber(num)){
                cout << "Error: Invalid account number." << '\n';
                continue;
            }
            core.Deposit(num, val);
            continue ; 
        }
        else if(cmd == "withdraw"){
            string num; 
            double val;
            cin >> num >> val;
            cout << "Enter password:" << '\n';
            string pass;
            cin >> pass;
            if(val <= 0){
                cout << "Error: Amount must be positive." << '\n';
                continue;
            }
            if(!isAccNumber(num)){
                cout << "Error: Invalid account number." << '\n';
                continue;
            }
            core.WITHDRAWAL(num, pass, val);
            continue ; 
        }
        else if(cmd == "transfer"){
            string from, to; 
            double val;
            cin >> from >> to >> val;
            cout << "Enter password:" << '\n';
            string pass;
            cin >> pass;
            if(val <= 0){
                cout << "Error: Amount must be positive." << '\n';
                continue;
            }
            if(!isAccNumber(from)){
                cout << "Error: Invalid account number." << '\n';
                continue;
            }
            if(!isAccNumber(to)){
                cout << "Error: Invalid account number." << '\n';
                continue;
            }
            core.Transfer(from, to, pass, val + core.Get_Transfer_Fee());
            continue ; 
        }
        else if(cmd == "withdraw_op"){
            string num; 
            double val;
            cin >> num >> val;
            string pass;
            cin >> pass;
            if(val <= 0){
                cout << "Error: Amount must be positive." << '\n';
                continue;
            }
            if(!isAccNumber(num)){
                cout << "Error: Invalid account number." << '\n';
                continue;
            }
            core.WITHDRAWAL(num, pass, val);
            continue ; 
        }
        else if(cmd == "transfer_op"){
            string from, to; 
            double val;
            cin >> from >> to >> val;
            string pass;
            cin >> pass;
            if(val <= 0){
                cout << "Error: Amount must be positive." << '\n';
                continue;
            }
            if(!isAccNumber(from)){
                cout << "Error: Invalid account number." << '\n';
                continue;
            }
            if(!isAccNumber(to)){
                cout << "Error: Invalid account number." << '\n';
                continue;
            }
            core.Transfer(from, to, pass, val + core.Get_Transfer_Fee());
            continue ; 
        }
        else if (cmd =="transfer_op_no_password"){
            string from, to; 
            double val;
            cin >> from >> to >> val;
            if(val <= 0){
                cout << "Error: Amount must be positive." << '\n';
                continue;
            }
            if(!isAccNumber(from)){
                cout << "Error: Invalid account number." << '\n';
                continue;
            }
            if(!isAccNumber(to)){
                cout << "Error: Invalid account number." << '\n';
                continue;
            }
            core.Transfer_no_password(from, to, val + core.Get_Transfer_Fee());
            continue ; 
        }
        /*-----------------------------------------------------------------------*/
        /*----------------------------Ranking------------------------------------*/
        else if(cmd == "show_ranking"){
            core.show_ranking();
            continue;
        }
        /*-----------------------------------------------------------------------*/
        /*----------------------------History---------------------------------------*/
        else if(cmd == "get_balance"){
            string num; 
            cin >> num;
            if(!isAccNumber(num)){
                cout << "Error: Invalid account number." << '\n';
                continue;
            }
            core.get_balance(num);
            continue ; 
        }
        else if(cmd == "get_balance_op"){
            string num; 
            cin >> num;
            if(!isAccNumber(num)){
                cout << "Error: Invalid account number." << '\n';
                continue;
            }
            core.get_balance(num, 1);
            continue ; 
        }
        else if(cmd == "get_history"){
            string num; 
            cin >> num;
            if(!isAccNumber(num)){
                cout << "Error: Invalid account number." << '\n';
                continue;
            }
            core.get_history(num);
            continue ; 
        }
        else if(cmd == "get_transaction"){
            int id; 
            cin >> id;
            core.get_transaction(id);
            continue ; 
        }
        else if(cmd == "clear_history"){
            string num; 
            cin >> num;
            if(!isAccNumber(num)){
                cout << "Error: Invalid account number." << '\n';
                continue;
            }
            core.clear_history(num);
            continue ; 
        }
        else if(cmd == "reset_all"){
            core.reset_all();
            continue ; 
        }
        else if (cmd == "paya_transfer"){
            double amount;
            string from_account, destination_iban, pass;
            cin >> from_account >> destination_iban >> amount >> pass;
            //cout << "get " << destination_iban << endl;
            int iban_index = core.IBANIDX(destination_iban);
            Paya_Request paya;
            paya.from_account= from_account;
            paya.destination_iban = destination_iban;
            paya.amount = amount;
            paya.status = 0;
            paya.id = core.new_request (); // check
            int ret = core.add_paya(paya, pass);
            if (ret){
                continue;
            }
            cout << "Paya request registered" << endl;
            cout << "Request ID: " << paya.id << endl;
            cout << "Status: Pending" << endl;
            continue;
        }
        else if (cmd == "list_paya_requests"){
            core.list_paya();
            continue;
        }
        else if (cmd == "approve_paya"){
            int id;
            cin >> id;
            core.approve_paya(id);
            continue;
        }
        else if (cmd == "reject_paya"){
            int id;
            cin >> id;
            core.reject_paya(id);
            continue;
        }
        cout << "Error: Unknown command" << '\n';
    }
    core.write();
    return 0;
}