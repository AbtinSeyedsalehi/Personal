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
    void setName(string Name) {
        this->Name = Name;
    }
    void setPassword(string Password) {
        this->Password = Password;
    }
    void setDevices(vector <string> Devices) {
        this->Devices = Devices;
    };
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
//APM: access point max functions
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
bool Is_AP_Name_Valid(string Name, vector<Accesspoint> &Accesspoints) {
    for(int i = 0; i < Accesspoints.size(); i ++) {
        if(Accesspoints[i].getName() == Name) {
            return true;
        }
    }
    return false;
}
bool Is_AP_Max_Valid(int Num) {
    if(Num < 0) {
        return false;
    }
    return true;
}
int main() {
    vector<Accesspoint> Accesspoints;
    //setup
    regex pattern1(R"(^create Access point (\S+) (\S+)(?: (\S+))*)");
    regex pattern2(R"(access point (\S+) max client (\d+))");
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

        }
    }
    // cin debugging process
    for(int i = 0; i < Accesspoints.size(); i ++) {
        cout << Accesspoints[i].getName() << " " << Accesspoints[i].getPassword() << endl;
        for(int j = 0; j < Accesspoints[i].getDevices().size(); j ++) {
            cout << Accesspoints[i].getDevices()[j] << " ";
        }
        cout << endl;
    }
    //loop
    return 0;
}