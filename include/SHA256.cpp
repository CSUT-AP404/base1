/*
    you must only use Hasher and compare functions 
    dont forget to add (#include "picosha2.h") and (#include <fstream>)
*/
#include <iostream>
#include <string>
#include <fstream>
#include "picosha2.h"
using namespace std ; 

void SHA256(string str1 , string str2){
    string hash2 = picosha2::hash256_hex_string(str2);
    string hash1 = picosha2::hash256_hex_string(str1);
    cout << "the " << str1 << " : \n" << hash1 << endl << "the "<< str2 << " :\n" << hash2 <<endl;
    return;
}

string Hasher(string pass){
    return picosha2::hash256_hex_string(pass);
}

bool compare(string pass, string input){
    input = picosha2::hash256_hex_string(input);
/*
    if you tranform the normal pass into a hash you wouldnt find any need for this line
        pass = picosha2::hash256_hex_string(pass);
*/
    if(pass==input)
        return true ;
    return false ; 
}

int main(){
    //example 
    string str1,str2; 
    cin >> str1>>str2; 
    string p = SHA256(str1,str2);
}