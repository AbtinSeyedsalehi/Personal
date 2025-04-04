#include <iostream>
#include <string>
#include <vector>
#include <regex>
using namespace std;
// 00: heater, 01: television, 10: air_conditioner, 11: refrigerator
class Device {
private:
    string Name;
    bool Stat;//true: on, false: off
public:
    Device(string Name, bool Stat) {
        this->Name = Name;
        this->Stat = Stat;
    }
    string getName() {
        return this->Name;
    }
    bool getStat() {
        return this->Stat;
    }
    void setName(string Name) {
        this->Name = Name;
    }
    void setStat(bool Stat) {
        this->Stat = Stat;
    }
};
class Accesspoint {
private:
    string Name;
    string Password;
    vector<Device> Devices;//devices include: heater, television, air_conditioner, refrigerator
    int Max = 100;
    vector<string> Limited_IP = {};
    vector<string> Connected_IP = {};
    string Last_Device = "";
    string Last_IP = "";
public:
    Accesspoint(string Name, string Password, vector<Device> Devices) {
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
    vector<Device>& getDevices() {
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
    string getLast_Device() {
        return this->Last_Device;
    }
    string getLast_IP() {
        return this->Last_IP;
    }
    void setName(string Name) {
        this->Name = Name;
    }
    void setPassword(string Password) {
        this->Password = Password;
    }
    void setDevices(vector <Device> Devices) {
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
    void setLast_Device(string Last_Device) {
        this->Last_Device = Last_Device;
    }
    void setLast_IP(string Last_IP) {
        this->Last_IP = Last_IP;
    }
};
//general functions
void Debug(vector<Accesspoint> &Accesspoints) {
    for(int i = 0; i < Accesspoints.size(); i ++) {
        cout << "AP no" << i+1 << ":" << endl;
        cout << Accesspoints[i].getName() << " " << Accesspoints[i].getPassword() << endl;
        for(int j = 0; j < Accesspoints[i].getDevices().size(); j ++) {
            cout << Accesspoints[i].getDevices()[j].getName() << " ";
            if(Accesspoints[i].getDevices()[j].getStat()) {
                cout << "on";
            }
            else {
                cout << "off";
            }
            cout << endl;
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
vector<string> Command_Extractor(string Command) {
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
//CAP: create access point, CAP functions
bool Is_AP_Name_Free(Accesspoint temp, vector<Accesspoint> &Accesspoints) {
    for(int i = 0; i < Accesspoints.size(); i ++) {
        if(Accesspoints[i].getName() == temp.getName()) {
            return false;
        }
    }
    return true;
}
bool Is_AP_Password_Valid(Accesspoint temp) {
    if(temp.getPassword().size() >= 8 || temp.getPassword() == "") {
        return true;
    }
    return false;
}
int heater_counter = 0;
int television_counter = 0;
int air_conditioner_counter = 0;
int refrigerator_counter = 0;
void Turn_CAP_Command_To_AP(vector<string> Result, vector<Accesspoint> &Accesspoints, int &heater_counter, int &television_counter, int &air_conditioner_counter, int &refrigerator_counter) {
    // checking if we have an open access point(vacant password)
    if(Result[4] == "heater" || Result[4] == "television" || Result[4] == "air_conditioner" || Result[4] == "refrigerator") {
        vector<Device> temp1;
        for(int i = 4; i < Result.size(); i ++) {
            Device temp2 = Device(Result[i], false);
            temp1.push_back(temp2);
            if(Result[i] == "heater") {
                heater_counter ++;
            }
            else if(Result[i] == "television") {
                television_counter ++;
            }
            else if(Result[i] == "air_conditioner") {
                air_conditioner_counter ++;
            }
            else if(Result[i] == "refrigerator") {
                refrigerator_counter ++;
            }
        }
        string Name = Result[3];
        Accesspoint temp = Accesspoint(Name, "", temp1);
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
        string Name = Result[3];
        string Password = Result[4];
        vector<Device> temp1;
        for(int i = 5; i < Result.size(); i ++) {
            Device temp2 = Device(Result[i], false);
            temp1.push_back(temp2);
            if(Result[i] == "heater") {
                heater_counter ++;
            }
            else if(Result[i] == "television") {
                television_counter ++;
            }
            else if(Result[i] == "air_conditioner") {
                air_conditioner_counter ++;
            }
            else if(Result[i] == "refrigerator") {
                refrigerator_counter ++;
            }
        }
        Accesspoint temp = Accesspoint(Name, Password, temp1);
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
bool Is_IP_Valid(string IP) {
    string Prim_IP = "";
    for(int i = 0; i < 10; i ++) {
        Prim_IP += IP[i];
    }
    if(Prim_IP != "192.168.1.") {
        return false;
    }
    IP.erase(IP.begin(), IP.begin()+10);
    int temp = stoi(IP);
    if(temp >= 0 && temp <= 255) {
        return true;
    }
    return false;
}
bool Is_AP_Password_Valid(vector<Accesspoint> &Accesspoints, string Password, int Accesspoint_Index) {
    if(Accesspoints[Accesspoint_Index].getPassword() == "") {
        return true;
    }
    else if(Accesspoints[Accesspoint_Index].getPassword() == Password) {
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
    string Password;
    if(Result.size() == 5) {
        Password = Result[4];
    }
    else {
        Password = "";
    }
    int Accesspoint_Index = Is_AP_Name_Valid(Name, Accesspoints);
    if(Is_IP_Valid(IP)) {
        if(Accesspoint_Index != -1) {
            if(Is_AP_Password_Valid(Accesspoints, Password, Accesspoint_Index)) {
                if((!Is_IP_Limited(Accesspoints, IP, Accesspoint_Index)) && (Accesspoints[Accesspoint_Index].getConnected_IP().size() < Accesspoints[Accesspoint_Index].getMax())) {
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
void Turn_APCL_Command_To_Action(vector<string> Result, vector<Accesspoint> &Accesspoints) {
    string Name = Result[2];
    int Accesspoint_index = Is_AP_Name_Valid(Name, Accesspoints);
    if(Accesspoint_index != -1) {
        if(Accesspoints[Accesspoint_index].getConnected_IP().size() != 0) {
            vector<string> temp = Accesspoints[Accesspoint_index].getConnected_IP();
            for(int i = 0; i < temp.size(); i ++) {
                cout << "client" << i+1 << " " << temp[i] << endl;
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
int Is_IP_In_AP(string IP, Accesspoint Accesspoint) {
    vector<string> temp = Accesspoint.getConnected_IP();
    for(int i = 0; i < temp.size(); i ++) {
        if(temp[i] == IP) {
            return i;
        }
    }
    return -1;
}
void Turn_DCAP_Command_To_Action(vector<string> Result, vector<Accesspoint> &Accesspoints) {
    string Name = Result[6];
    string IP = Result[2];
    int Accesspoint_index = Is_AP_Name_Valid(Name, Accesspoints);
    if(Accesspoint_index != -1) {
        int IP_index = Is_IP_In_AP(IP, Accesspoints[Accesspoint_index]);
        if(IP_index != -1) {
            cout << "client deleted" << endl;
            vector<string> temp = Accesspoints[Accesspoint_index].getConnected_IP();
            temp.erase(temp.begin()+IP_index);
            Accesspoints[Accesspoint_index].setConnected_IP(temp);
        }
        else {
            cout << "this IP does not exist in the access point " << Accesspoints[Accesspoint_index].getName() << endl;
        }
    }
    else {
        cout << "invalid access point name" << endl;
    }
}
//Dis_CAP: disconnect client from access point, Dis_CAP functions
void Turn_Dis_CAP_To_Action(vector<string> Result, vector<Accesspoint> &Accesspoints) {
    string Name = Result[6];
    string IP = Result[2];
    int Accesspoint_index = Is_AP_Name_Valid(Name, Accesspoints);
    if(Accesspoint_index != -1) {
        int IP_index = Is_IP_In_AP(IP, Accesspoints[Accesspoint_index]);
        if(IP_index != -1) {
            cout << "client disconnected" << endl;
            vector<string> temp = Accesspoints[Accesspoint_index].getConnected_IP();
            temp.erase(temp.begin()+IP_index);
            Accesspoints[Accesspoint_index].setConnected_IP(temp);
        }
        else {
            cout << "no such client exist in this access point" << endl;
        }
    }
    else {
        cout << "this access point does not exist" << endl;
    }
}
//RCIP: Read from client in access point, RCIP functions
int Find_AP_with_IP(string IP, vector<Accesspoint> &Accesspoints) {
    for(int i = 0; i < Accesspoints.size(); i ++) {
        int IP_index = Is_IP_In_AP(IP, Accesspoints[i]);
        if(IP_index != -1) {
            return i;
        }
    }
    return -1;
}
int Is_Device_In_AP(string Bin_Device, Accesspoint Accesspoint) {
    string Transfered_Device;
    if(Bin_Device == "00") {
        Transfered_Device = "heater";
    }
    else if(Bin_Device == "01") {
        Transfered_Device = "television";
    }
    else if(Bin_Device == "10") {
        Transfered_Device = "air_conditioner";
    }
    else if(Bin_Device == "11") {
        Transfered_Device = "refrigerator";
    }
    vector<Device> temp = Accesspoint.getDevices();
    for(int i = 0; i < temp.size(); i ++) {
        if(Transfered_Device == temp[i].getName()) {
            return i;
        }
    }
    return -1;
}
void Is_Device_On_Or_Off(Device &Device, string Bin_Stat) {
    bool Current_Stat = Device.getStat();
    if(Current_Stat) {
        if(Bin_Stat == "1") {
            cout << Device.getName() << " was on" << endl;
        }
        else {
            cout << Device.getName() << " turns off" << endl;
            Device.setStat(false);
        }
    }
    else {
        if(Bin_Stat == "1") {
            cout << Device.getName() << " turns on" << endl;
            Device.setStat(true);
        }
        else {
            cout << Device.getName() << " was off" << endl;
        }
    }
}
int Find_IP_In_AP(string IP, Accesspoint Accesspoint) {
    for(int i = 0; i < Accesspoint.getConnected_IP().size(); i ++) {
        if(IP == Accesspoint.getConnected_IP()[i]) {
            return i;
        }
    }
    return -1;
}
string Turn_Bin_Device_To_Name(string Bin_Device) {
    if(Bin_Device == "00") {
        return "heater";
    }
    else if(Bin_Device == "01") {
        return "television";
    }
    else if(Bin_Device == "10") {
        return "air_conditioner";
    }
    else if(Bin_Device == "11") {
        return "refrigerator";
    }
}
void Turn_RCIP_To_Action(vector<string> Result, string Bin_Command, vector<Accesspoint> &Accesspoints, vector<int> &Spam_IP) {
    string IP = Result[3];
    string Bin_Stat;
    string Bin_Device = "";
    for(int i = 0; i < 2; i ++) {
        Bin_Device += Bin_Command[i];
        Bin_Stat = Bin_Command[2];
    }
    int Accesspoint_index = Find_AP_with_IP(IP, Accesspoints);
    if(Accesspoint_index != -1) {
        int Device_index = Is_Device_In_AP(Bin_Device, Accesspoints[Accesspoint_index]);
        if(Device_index != -1) {
            int IP_index = Find_IP_In_AP(IP, Accesspoints[Accesspoint_index]);
            string Device_Name = Turn_Bin_Device_To_Name(Bin_Device);
            if((Device_Name == Accesspoints[Accesspoint_index].getLast_Device()) && (IP == Accesspoints[Accesspoint_index].getLast_IP())) {
                Spam_IP[IP_index] ++;
                Accesspoints[Accesspoint_index].setLast_Device(Device_Name);
                Accesspoints[Accesspoint_index].setLast_IP(IP);
            }
            else {
                Spam_IP[IP_index] = 1;
                Accesspoints[Accesspoint_index].setLast_Device(Device_Name);
                Accesspoints[Accesspoint_index].setLast_IP(IP);
            }
            if(Spam_IP[IP_index] == 5) {
                cout << "client " << IP << " banned due to spam" << endl;
                vector<string> temp = Accesspoints[Accesspoint_index].getLimited_IP();
                temp.push_back(IP);
                Accesspoints[Accesspoint_index].setLimited_IP(temp);
                vector<string> temp_1 = Accesspoints[Accesspoint_index].getConnected_IP();
                temp_1.erase(temp_1.begin()+IP_index);
                Accesspoints[Accesspoint_index].setConnected_IP(temp_1);
                Spam_IP[IP_index] = 1;
            }
            else {
                Is_Device_On_Or_Off(Accesspoints[Accesspoint_index].getDevices()[Device_index], Bin_Stat);
            }
        }
        else {
            cout << "no such device found" << endl;
        }
    }
    else {
        cout << "invalid IP" << endl;
    }
}
int main() {
    int Spam_Counter = 1;
    vector<Accesspoint> Accesspoints;
    //setup
    regex pattern1(R"(create Access point (\S+) (\S+)(?: (\S+))*)");//CAP regex
    regex pattern2(R"(access point (\S+) max client (-?\d+))");//APM regex
    regex pattern3(R"(limit client (192\.168\.1\.\d+) from access point (\S+))");//APM-IP regex
    regex pattern4(R"(connect client ((?:\d{1,3}\.){3}\d{1,3}) (\S+)(?: (\S+))?)");//CCAP regex
    regex pattern5(R"(access point (\S+) clients list)");//APCL regex
    regex pattern6(R"(delete client (192\.168\.1\.\d+) from access point (\S+))");//DCAP regex
    regex pattern7(R"(disconnect client (192\.168\.1\.\d+) from access point (\S+))");//Dis_CAP regex
    regex pattern8(R"(read from client (192\.168\.1\.\d+))");//RCIP regex
    regex pattern0(R"(000||001||010||011||100||101||110||111)");
    string Command_1;
    // 00: heater, 01: television, 10: air_conditioner, 11: refrigerator
    int heater_counter = 0;
    int television_counter = 0;
    int air_conditioner_counter = 0;
    int refrigerator_counter = 0;
    while(1) {
        getline(cin, Command_1);
        if(regex_search(Command_1, pattern1)) {
            vector<string> Result = Command_Extractor(Command_1);
            Turn_CAP_Command_To_AP(Result, Accesspoints, heater_counter, television_counter, air_conditioner_counter, refrigerator_counter);
        }
        else if(regex_search(Command_1, pattern2)) {
            vector<string> Result = Command_Extractor(Command_1);
            Turn_APM_Command_To_ML(Result, Accesspoints);
        }
        else if(regex_search(Command_1, pattern3)) {
            vector<string> Result = Command_Extractor(Command_1);
            Turn_APM_IP_Command_To_ML(Result, Accesspoints);
        }
        else if((regex_search(Command_1, pattern4) || regex_search(Command_1, pattern5) ||regex_search(Command_1, pattern6) || regex_search(Command_1, pattern7) || regex_search(Command_1, pattern8)) && (television_counter >= 1) && (refrigerator_counter >= 1) && (heater_counter >= 1) && (air_conditioner_counter >= 1)) {
            break;
        }
        else {
            cout << "invalid command" << endl;
        }
    }
    // cin debugging process
    //Debug(Accesspoints);
    vector<int> Spam_IP(Accesspoints.size(), 0);
    string Command = Command_1;
    //loop
    while(1) {
        if(Command == "exit") {
            break;
        }
        else if(regex_search(Command, pattern7)) {
            vector<string> Result = Command_Extractor(Command);
            Turn_Dis_CAP_To_Action(Result, Accesspoints);
        }
        else if(regex_search(Command, pattern4)) {
            vector<string> Result = Command_Extractor(Command);
            Turn_CCAP_Command_To_Action(Result, Accesspoints);
        }
        else if(regex_search(Command, pattern5)) {
            vector<string> Result = Command_Extractor(Command);
            Turn_APCL_Command_To_Action(Result, Accesspoints);
        }
        else if(regex_search(Command, pattern6)) {
            vector<string> Result = Command_Extractor(Command);
            Turn_DCAP_Command_To_Action(Result, Accesspoints);
        }
        else if(regex_search(Command, pattern8)) {
            string Bin_Command;
            cin >> Bin_Command;
            vector<string> Result = Command_Extractor(Command);
            Turn_RCIP_To_Action(Result, Bin_Command, Accesspoints, Spam_IP);
        }
        else if(regex_search(Command, pattern0) && !regex_search(Command, pattern1) && !regex_search(Command, pattern2) && !regex_search(Command, pattern3)) {
            bool n;
        }
        else {
            cout << "invalid command" << endl;
        }
        getline(cin, Command);
    }
    //Debug(Accesspoints);
    return 0;
}