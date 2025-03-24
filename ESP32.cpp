#include <iostream>
#include <string>
#include <vector>
#include <regex>
using namespace std;
// 00: heater, 01: television, 10: air_conditioner, 11: refrigerator
class Accesspoint {
private:
    string Name;
    string Password;
    vector<string> Devices;//devices include: heater, television, air_conditioner, refrigerator
    int Max = 100;
    vector<string> IP = {};
public:
    Accesspoint(string Name, string Password, vector<string> Devices) {
        this->Name = Name;
        this->Password = Password;
        this->Devices = Devices;
    }
    string getName() {
        return this->Name;
    }
    string getPassword() {
        return this->Password;
    };
    vector<string> getDevices() {
        return this->Devices;
    }
    vector<string> getIP() {
        return this->IP;
    }
    int getMax() {
        return this->Max;
    }
    void setName(string Name) {
        this->Name = Name;
    }
    void setPassword(string Password) {
        this->Password = Password;
    }
    void setDevices(vector <string> Devices) {
        this->Devices = Devices;
    }
    void setMax(int Max) {
        this->Max = Max;
    }
    void setIP(vector <string> IP) {
        this->IP = IP;
    }
};
//CAP: create access point functions
vector<string> CAP_Command_Extractor(string Command) {
    vector<string> Result;
    int Num = Command.size();
    string Word = "";
    for(int i = 0; i < Num; i++) {
        if(Command[i] == ' ') {
            Result.push_back(Word);
            Word = "";
            continue;
        }
        Word += Command[i];
    }
    Result.push_back(Word);
    return Result;
}
bool Is_AP_Name_Free(Accesspoint temp, vector<Accesspoint> &Accesspoints) {
    for(int i = 0; i < Accesspoints.size(); i ++) {
        if(Accesspoints[i].getName() == temp.getName()) {
            return false;
        }
    }
    return true;
}
bool Is_AP_Password_Valid(Accesspoint temp) {
    if(temp.getPassword().size() == 8 || temp.getPassword() == "") {
        return true;
    }
    return false;
}
void Turn_CAP_Command_To_AP(vector<string> Result, vector<Accesspoint> &Accesspoints) {
    // checking if we have an open access point(vacant password)
    if(Result[4] == "heater" || Result[4] == "television" || Result[4] == "air_conditioner" || Result[4] == "refrigerator") {
        Result.erase(Result.begin(), Result.begin()+4);
        Accesspoint temp = Accesspoint(Result[3], "", vector<string> (Result));
        if(Is_AP_Name_Free(temp, Accesspoints)) {
            if(Is_AP_Password_Valid(temp)) {
                Accesspoints.push_back(temp);
                cout << "access point created successfully" << endl;
            }
            else {
                cout << "invalid password" << endl;
            }
        }
        else {
            cout << "this name already used" << endl;
        }
    }
    else {
        Result.erase(Result.begin(), Result.begin()+5);
        Accesspoint temp = Accesspoint(Result[3], Result[4], vector<string> (Result));
        if(Is_AP_Name_Free(temp, Accesspoints)) {
            if(Is_AP_Password_Valid(temp)) {
                Accesspoints.push_back(temp);
                cout << "access point created successfully" << endl;
            }
            else {
                cout << "invalid password" << endl;
            }
        }
        else {
            cout << "this name already used" << endl;
        }
    }
}
//APM: access point max, APM functions
//APM-IP: access point max IP based, APM-IP functions
vector<string> APM_Command_Extractor(string Command) {
    vector<string> Result;
    int Num = Command.size();
    string Word = "";
    for(int i = 0; i < Num; i++) {
        if(Command[i] == ' ') {
            Result.push_back(Word);
            Word = "";
            continue;
        }
        Word += Command[i];
    }
    Result.push_back(Word);
    return Result;
}
int Is_AP_Name_Valid(string Name, vector<Accesspoint> &Accesspoints) {
    for(int i = 0; i < Accesspoints.size(); i ++) {
        if(Accesspoints[i].getName() == Name) {
            return i;
        }
    }
    return -1;
}
bool Is_AP_Max_Valid(int Num) {
    if(Num < 0) {
        return false;
    }
    return true;
}
bool Is_IP_Valid(string IP) {
    IP.erase(IP.begin(), IP.begin()+12);
    int temp = stoi(IP);
    if(temp >= 0 && temp <= 255) {
        return true;
    }
    return false;
}
void Turn_APM_Command_To_ML(vector<string> Result, vector<Accesspoint> &Accesspoints) {
    string Name = Result[2];
    int Num = stoi(Result[5]);
    int Accesspoint_Index = Is_AP_Name_Valid(Name, Accesspoints);
    if(Accesspoint_Index != -1) {
        if(Is_AP_Max_Valid(Num)) {
            cout << "limitation set" << endl;
            Accesspoints[Accesspoint_Index].setMax(Num);
        }
        else {
            cout << "number of clients can not be negative" << endl;
        }
    }
    else {
        cout << "invalid access point name" << endl;
    }
}
void Turn_APM_IP_Command_To_ML(vector<string> Result, vector<Accesspoint> &Accesspoints) {
    string Name = Result[6];
    string IP = Result[2];
    int Accesspoint_Index = Is_AP_Name_Valid(Name, Accesspoints);
    if(Accesspoint_Index != -1) {

    }
    else {
        cout << "invalid access point name" << endl;
    }
}
int main() {
    vector<Accesspoint> Accesspoints;
    //setup
    regex pattern1(R"(^create Access point (\S+) (\S+)(?: (\S+))*)");//CAP regex
    regex pattern2(R"(access point (\S+) max client (-?\d+))");//APM regex
    regex pattern3(R"(limit client (192\.168\.1\.\d+) from access point (\S+))");//APM-IP regex
    while(1) {
        string Command;
        getline(cin, Command);
        if(Command == "end") {
            break;
        }
        if(regex_search(Command, pattern1)) {
            vector<string> Result = CAP_Command_Extractor(string (Command));
            Turn_CAP_Command_To_AP(Result, Accesspoints);
        }
        else if(regex_search(Command, pattern2)) {
            vector<string> Result = APM_Command_Extractor(string (Command));
            Turn_APM_Command_To_ML(Result, Accesspoints);
        }
        else if(regex_search(Command, pattern3)) {
            vector<string> Result = APM_Command_Extractor(string (Command));

        }
    }
    // cin debugging process
    for(int i = 0; i < Accesspoints.size(); i ++) {
        cout << "Device no" << i+1 << ":" << endl;
        cout << Accesspoints[i].getName() << " " << Accesspoints[i].getPassword() << endl;
        for(int j = 0; j < Accesspoints[i].getDevices().size(); j ++) {
            cout << Accesspoints[i].getDevices()[j] << " ";
        }
        cout << endl;
        cout << Accesspoints[i].getMax() << endl;
        for(int j = 0; j < Accesspoints[i].getDevices().size(); j ++) {
            cout << Accesspoints[i].getIP()[j] << "  ";
        }
        cout << endl << "------------------------" << endl;
    }
    //loop
    return 0;
}