#include <bits/stdc++.h>
#include <ctime>
#include <fstream>
#include "include/picosha2.h"
#include "data/json.hpp"

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
struct Account_Id{
    int n[4];

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
        for(int i = 0, j = 0; i < 19; i++){
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
    string Month = to_string(ltm->tm_mon);
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
    Time += Min;
    return Time; 
}
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
    public:
        string HashPass;
        bool Active;

        Account (int BankID, int N, int Branch_Id, string HashPass, ld Coin = 0, bool Active = true){
            AI = Account_Id (BankID, N);
            this -> Branch_Id = Branch_Id;
            this -> HashPass = HashPass;
            this -> Coin = Coin;
            this -> Active = Active;
        }
        Account (int n1, int n2, int n3, int n4, int Branch_Id, string HashPass, ld Coin = 0, bool Active = true){
            AI = Account_Id (n1, n2, n3, n4);
            this -> Branch_Id = Branch_Id;
            this -> HashPass = HashPass;
            this -> Coin = Coin;
            this -> Active = Active;
        }
        Account (string Str, int Branch_Id, string HashPass, ld Coin = 0, bool Active = true){
            AI = Account_Id (Str);
            this -> Branch_Id = Branch_Id;
            this -> HashPass = HashPass;
            this -> Coin = Coin;
            this -> Active = Active;
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
            string s = "";
            for(int i = 0; i < 4; i++){
                s += to_string(AI[i]);
            }
            return s;
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
class Branch{
    private:
        vector<Account_Id> AIs;
    public:
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

        void Add_Account(Account_Id A){
            AIs.push_back(A);
        }
        
        Branch& operator= (const Branch &B){
            name = B.name;
            Id = B.Id;
            return (*this);
        }
        Branch (const Branch &B){
            name = B.name;
            Id = B.Id;
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


struct User {
    vector<string> id;
    string codeMelli;
    string pass;
    User(){}
    User(string codeMelli, string pass){
        this->codeMelli = codeMelli ;
        this->pass = pass ;
    }
};


class Core{
    private:
        vector<Branch> Branches;
        vector<Account> FAccounts;
        vector<Account> BAccounts;
        vector<Transaction> Trans;
        vector<User> Users; 
        int Account_Cnt, Trans_Cnt;
        ld transferFee, balanceInquiryFee;
    public:
        int BankID;

        Core(int BankID = 5022){
            this -> BankID = BankID;
            Account_Cnt = 0;
            Trans_Cnt = 1001;
            transferFee = 0.00;
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
            for(auto &B : Branches){
                cout << B << '\n';
            }
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
            bool found = false;
            for(auto &B : Branches){
                if(B.Id == Branch_Id){
                    found = true; 
                    break;
                }
            }   
            if(!found){ 
                cout << "The branch doesnt exist" << '\n'; 
                return; 
            }
            FAccounts.push_back(Account (BankID, Account_Cnt++, Branch_Id, Hasher(Pass)));
            cout << "Account created. Number: " << FAccounts.back().getID() << '\n';
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
            Account_Id AI(Num);
            for(auto &A : BAccounts){
                if(AI == A.getID()){
                    cout << "Error: Account is inactive." << '\n';
                    return;
                }
            }
            for(auto &A : FAccounts){
                if(AI == A.getID()){
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
        void WITHDRAWAL(string &Num, string &Pass, ld val){
            Account_Id AI(Num);
            for(auto &A : BAccounts){
                if(AI == A.getID()){
                    cout << "Error: Account is inactive." << '\n';
                    return;
                }
            }
            for(auto &A : FAccounts){
                if(AI == A.getID()){
                    if(!compare(A.HashPass, Pass)){
                        cout << "Error: Wrong password." << '\n'; 
                        return;
                    }
                    if(A.getCoin() < val){
                        cout << "Error: Insufficient funds." << '\n';
                        return;
                    }
                    Transaction T("WITHDRAWAL", Trans_Cnt++, -val, A.getCoin() - val, Num, Num);
                    cout << "Transaction ID: " << T.ID << '\n';
                    cout << fixed << setprecision(2) << "New balance: " << A.getCoin() - val << '\n';
                    A.WITHDRAWAL(val, T);
                    Trans.push_back(T);
                    write();
                    return;
                }
            }
            cout << "Error: Account not found." << '\n';
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
        void give_balance(int idx, bool isBlocked, bool SPECIAL = 0){
            if(!isBlocked){
                if(SPECIAL == 0 && FAccounts[idx].getCoin() < balanceInquiryFee){
                    cout << "Error: Insufficient funds." << '\n';
                    return;
                }
                Transaction T("Bank inquiry fee", Trans_Cnt++, -balanceInquiryFee, FAccounts[idx].getCoin() - balanceInquiryFee, 
                FAccounts[idx].getID().To_String(), "Bank");
                FAccounts[idx].WITHDRAWAL(balanceInquiryFee, T);
                cout << "Balance inquiry fee: " << balanceInquiryFee << '\n';
                cout << fixed << setprecision(2);
                cout << "Balance: " << FAccounts[idx].getCoin() << '\n';
                cout << "Active: Yes" << '\n';
                cout << "Branch: " << FAccounts[idx].getBranch() << '\n';
                cout << defaultfloat << setprecision(6);
                return;
            }        
            cout << fixed << setprecision(2);
            cout << "Balance: " << BAccounts[idx].getCoin() << '\n';
            cout << "Active: No" << '\n';
            cout << "Branch: " << BAccounts[idx].getBranch() << '\n';
            cout << setprecision(6);
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
                if(idx == -1){ cout << "Error: Account not found." << '\n'; return; }
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
                Users.clear();
                Branches.clear();
                FAccounts.clear();
                BAccounts.clear();
                Trans.clear();
                Account_Cnt = 0;
                Trans_Cnt = 1001;
                write();
                write_setting();
                write_users(); 
                cout << "All data cleared." << '\n';
            }
            else{
                cout << "Cancelled." << '\n';
            }
        }
    /*--------------------------------------------------*/
    void read(){
        ifstream inFile("BankـData.json");
        if(!inFile.is_open()){
            return;
        }
        json j;
        inFile >> j;
        inFile.close();


        Account_Cnt = j["Account_Cnt"];
        Trans_Cnt   = j["Trans_Cnt"];
        
        
        for(auto &item : j["Branches"]){
            Branches.push_back(Branch(item["name"], item["id"]));
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
    }
    /*--------------------------------------------------*/
    void write(){
        json j;
        json jBranches = json::array();
        for(auto &B : Branches){
            jBranches.push_back({
                {"id",   B.Id},
                {"name", B.name}
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
        j["transactions"] = jTrans;
        j["Account_Cnt"] = Account_Cnt;
        j["Trans_Cnt"]   = Trans_Cnt;

        ofstream outFile("BankـData.json");
        outFile << j.dump(4);
        outFile.close();
    }
    /*--------------------------------------------------*/
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
    void write_setting(){
        json j;
        j["transfer_fee"] = transferFee;
        j["balance_inquiry_fee"] = balanceInquiryFee;
        ofstream inFile("data/setting.json");
        inFile << j.dump(4);
        inFile.close();
    }
    /*--------------------------------------------------*/
    void read_users() {
        ifstream inFile("Users.json");
        if(!inFile.is_open()){
            return;
        }
        json j;
        inFile >> j;
        if(j.contains("users")){
            for(auto &userr : j["users"]){
                User u;
                u.codeMelli = userr["codeMelli"];
                u.pass = userr["pass"];
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
                {"pass", userr.pass},
                {"accounts", jAccs}
            });
        }
        j["users"] = jUsers;
        ofstream inFile("Users.json");
        inFile << j.dump(4);
        inFile.close();
    }

};
/*------------------------------------------------------------------*/

int main(){
    ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
    Core core;
    string cmd;
    while(cin >> cmd){
        if(cmd=="EOF"){
            exit(0);
        }
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
            core.Close_Account(pass, id);
            continue ; 
        }
        else if(cmd == "delete_account"){
            string id; 
            cin >> id;
            cout << "Enter password:" << '\n';
            string pass;
            cin >> pass;
            core.Delete_Account(pass, id);
            continue ; 
        }
        else if(cmd == "list_accounts"){
            core.Account_List();
            continue ; 
        }
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
        else if(cmd == "deposit"){
            string num; 
            double val;
            cin >> num >> val;
            if(val <= 0){
                cout << "Error: Amount must be positive." << '\n';
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
            core.Transfer(from, to, pass, val + core.Get_Transfer_Fee());
            continue ; 
        }
        else if(cmd == "get_balance"){
            string num; 
            cin >> num;
            core.get_balance(num);
            continue ; 
        }
        else if(cmd == "get_history"){
            string num; 
            cin >> num;
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
            core.clear_history(num);
            continue ; 
        }
        else if(cmd == "reset_all"){
            core.reset_all();
            continue ; 
        }
        cout << "Unknown command" << '\n';
    }
    return 0;
}
