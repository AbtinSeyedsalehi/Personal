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
    vector<string> Limited_IP = {};
    vector<string> Connected_IP = {};
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
    vector<string> getLimited_IP() {
        return this->Limited_IP;
    }
    vector<string> getConnected_IP() {
        return this->Connected_IP;
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
    void setLimited_IP(vector <string> Limited_IP) {
        this->Limited_IP = Limited_IP;
    }
    void setConnected_IP(vector <string> Connected_IP) {
        this->Connected_IP = Connected_IP;
    }
};
//debug function
void Debug(vector<Accesspoint> &Accesspoints) {
    for(int i = 0; i < Accesspoints.size(); i ++) {
        cout << "Device no" << i+1 << ":" << endl;
        cout << Accesspoints[i].getName() << " " << Accesspoints[i].getPassword() << endl;
        for(int j = 0; j < Accesspoints[i].getDevices().size(); j ++) {
            cout << Accesspoints[i].getDevices()[j] << " ";
        }
        cout << endl;
        cout << "Max Clients: " << Accesspoints[i].getMax() << endl;
        cout << "Limited IP's: " << endl;
        for(int j = 0; j < Accesspoints[i].getLimited_IP().size(); j ++) {
            cout << Accesspoints[i].getLimited_IP()[j] << "  ";
        }
        cout << endl;
        cout << "Connected IP's: " << endl;
        for(int j = 0; j < Accesspoints[i].getConnected_IP().size(); j ++) {
            cout << Accesspoints[i].getConnected_IP()[j] << "  ";
        }
        cout << endl;
        cout << "------------------------" << endl;
    }
}
//CAP: create access point, CAP functions
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
        cout << "limitation set" << endl;
        vector<string> temp_IP = Accesspoints[Accesspoint_Index].getLimited_IP();
        temp_IP.push_back(IP);
        Accesspoints[Accesspoint_Index].setLimited_IP(temp_IP);
    }
    else {
        cout << "invalid access point name" << endl;
    }
}
//CCAP : connect client to access point, CCAP functions
vector<string> CCAP_Command_Extractor(string Command) {
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
bool Is_IP_Valid(string IP) {
    IP.erase(IP.begin(), IP.begin()+10);
    int temp = stoi(IP);
    if(temp >= 0 && temp <= 255) {
        return true;
    }
    return false;
}
bool Is_AP_Password_Valid(vector<Accesspoint> &Accesspoints, string Password, int Accesspoint_Index) {
    if(Accesspoints[Accesspoint_Index].getPassword() == Password) {
        return true;
    }
    return false;
}
bool Is_IP_Limited(vector<Accesspoint> &Accesspoints, string IP, int Accesspoint_Index) {
    for(int i = 0; i < Accesspoints[Accesspoint_Index].getLimited_IP().size(); i ++) {
        if(Accesspoints[Accesspoint_Index].getLimited_IP()[i] == IP) {
            return true;
        }
    }
    return false;
}
void Turn_CCAP_Command_To_Action(vector<string> Result, vector<Accesspoint> &Accesspoints) {
    string IP = Result[2];
    string Name = Result[3];
    string Password = Result[4];
    int Accesspoint_Index = Is_AP_Name_Valid(Name, Accesspoints);
    if(Is_IP_Valid(IP)) {
        if(Accesspoint_Index != -1) {
            if(Is_AP_Password_Valid(Accesspoints, Password, Accesspoint_Index)) {
                if(!Is_IP_Limited(Accesspoints, IP, Accesspoint_Index) && Accesspoints[Accesspoint_Index].getConnected_IP().size() < Accesspoints[Accesspoint_Index].getMax()) {
                    cout << "client connected successfully" << endl;
                    vector<string> temp = Accesspoints[Accesspoint_Index].getConnected_IP();
                    temp.push_back(IP);
                    Accesspoints[Accesspoint_Index].setConnected_IP(temp);
                }
                else {
                    cout << "can not connect because of limitations" << endl;
                }
            }
            else {
                cout << "wrong password" << endl;
            }
        }
        else {
            cout << "invalid access point name" << endl;
        }
    }
    else {
        cout << "invalid IP" << endl;
    }
}
//APCL: access point client list, APCL functions
vector<string> APCL_Command_Extractor(string Command) {
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
void Turn_APCL_Command_To_Action(vector<string> Result, vector<Accesspoint> &Accesspoints) {
    string Name = Result[2];
    int Accesspoint_index = Is_AP_Name_Valid(Name, Accesspoints);
    if(Accesspoint_index != -1) {
        if(Accesspoints[Accesspoint_index].getConnected_IP().size() != 0) {
            vector<string> temp = Accesspoints[Accesspoint_index].getConnected_IP();
            for(int i = 0; i < temp.size(); i ++) {
                cout << "client" << i+1 << " <" << temp[i] << ">" << endl;
            }
        }
        else {
            cout << "no client connected yet" << endl;
        }
    }
    else {
        cout << "invalid access point name" << endl;
    }
}
//DCAP: delete client from access point, DCAP functions
vector<string> DCAP_Command_Extractor(string Command) {
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
int main() {
    vector<Accesspoint> Accesspoints;
    //setup
    regex pattern1(R"(^create Access point (\S+) (\S+)(?: (\S+))*)");//CAP regex
    regex pattern2(R"(access point (\S+) max client (-?\d+))");//APM regex
    regex pattern3(R"(limit client (192\.168\.1\.\d+) from access point (\S+))");//APM-IP regex
    regex pattern4(R"(connect client (192\.168\.1\.\d+) (\S+) (\S+))");//CCAP regex
    regex pattern5(R"(access point (\S+) clients list)");//APCL regex
    while(1) {
        string Command;
        getline(cin, Command);
        if(Command == "end") {
            break;
        }
        else if(regex_search(Command, pattern1)) {
            vector<string> Result = CAP_Command_Extractor(Command);
            Turn_CAP_Command_To_AP(Result, Accesspoints);
        }
        else if(regex_search(Command, pattern2)) {
            vector<string> Result = APM_Command_Extractor(Command);
            Turn_APM_Command_To_ML(Result, Accesspoints);
        }
        else if(regex_search(Command, pattern3)) {
            vector<string> Result = APM_Command_Extractor(Command);
            Turn_APM_IP_Command_To_ML(Result, Accesspoints);
        }
        else {
            cout << "invalid command" << endl;
        }
    }
    // cin debugging process
    Debug(Accesspoints);
    //loop
    while(1) {
        string Command;
        getline(cin, Command);
        if(Command == "end") {
            break;
        }
        else if(regex_search(Command, pattern4)) {
            vector<string> Result = CCAP_Command_Extractor(Command);
            Turn_CCAP_Command_To_Action(Result, Accesspoints);
        }
        else if(regex_search(Command, pattern5)) {
            vector<string> Result = APCL_Command_Extractor(Command);
            Turn_APCL_Command_To_Action(Result, Accesspoints);
        }
    }
    Debug(Accesspoints);
    return 0;
}