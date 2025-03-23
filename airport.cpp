#include <iostream>
#include <vector>
using namespace std;
class Aircraft {
private:
    string ID;// a single string in
    int Status;
    int Line;
    // 1: in airport / doesn't occupy any runway
    // 2: in airport / does occupy runway / taking off
    // 3: in airport / does occupy runway / landing
    // 4: not in airport
public:
    Aircraft(string ID, int Status) {
        this -> ID = ID;
        this -> Status = Status;
        this -> Line = -1;
    }
    string getID() {
        return this -> ID;
    }
    int getStatus() {
        return this -> Status;
    }
    void setter(string ID, int Status) {
        this -> ID = ID;
        this -> Status = Status;
    }
    void setStatus(int Status) {
        this -> Status = Status;
    }
    void setLine(int Line) {
        this -> Line = Line;
    }
    int getLine() {
        return this -> Line;
    }
};
class Runway {
private:
    int Num;
    bool Status;
public:
    Runway(int Num, bool a) {
        this -> Num = Num;
        if(a) {
            this -> Status = true;
        }
        else {
            this -> Status = false;
        }
    }
    bool getStatus() {
        return this -> Status;
    }
    void setStatus(bool Status) {
        this -> Status = Status;
    }
};
int Find_Aircraft_With_ID(vector <Aircraft> &Aircrafts, string ID) {
    for(int i = 0; i < Aircrafts.size(); i ++) {
        if(Aircrafts[i].getID() == ID) {
            return i;
        }
    }
    return -1;
}
int Find_Aircraft_With_Line(vector <Aircraft> &Aircrafts, int Line) {
    for(int i = 0; i < Aircrafts.size(); i ++) {
        if(Aircrafts[i].getLine() == Line) {
            return i;
        }
    }
    return -1;
}
int Is_There_An_Empty_Runway_For_Taking_Off(vector <Runway> &Runways) {
    for(int i = 0; i < Runways.size(); i ++) {
        if(Runways[i].getStatus()) {
            return i;
        }
    }
    return -1;
}
int Is_There_An_Empty_Runway_For_Landing(vector <Runway> &Runways) {
    for(int i = Runways.size()-1; i >= 0; i --) {
        if(Runways[i].getStatus()) {
            return i;
        }
    }
    return -1;
}
int main() {
    int k;
    cin >> k;
    vector <Aircraft> Aircrafts;
    vector <Runway> Runways;
    // true: empty / false: occupied
    for(int i = 0; i < k; i ++) {
        Runway temp = Runway(i+1, true);
        Runways.push_back(temp);
    }
    while(1) {
        string command;
        cin >> command;
        if(command == "EXIT") {
            break;
        }
        else if(command == "ADD-PLANE") {
            string ID;
            cin >> ID;
            Aircraft temp = Aircraft(ID, 1);
            Aircrafts.push_back(temp);
        }
        else if(command == "TAKE-OFF") {
            string ID;
            cin >> ID;
            int Aircraft_index = Find_Aircraft_With_ID(Aircrafts, ID);
            if(Aircraft_index == -1) {
                cout << "YOU ARE NOT HERE" << endl;
            }
            else {
                if(Aircrafts[Aircraft_index].getStatus() == 4) {
                    cout << "YOU ARE NOT HERE" << endl;
                }
                else if(Aircrafts[Aircraft_index].getStatus() == 3) {
                    cout << "YOU ARE LANDING NOW" << endl;
                }
                else if(Aircrafts[Aircraft_index].getStatus() == 2) {
                    cout << "YOU ARE TAKING OFF" << endl;
                }
                else if(Aircrafts[Aircraft_index].getStatus() == 1) {
                    if(Is_There_An_Empty_Runway_For_Taking_Off(Runways) == -1) {
                        cout << "NO FREE BAND" << endl;
                    }
                    else {
                        Aircrafts[Aircraft_index].setStatus(2);
                        int Runway_index = Is_There_An_Empty_Runway_For_Taking_Off(Runways);
                        Aircrafts[Aircraft_index].setLine(Runway_index+1);
                        Runways[Runway_index].setStatus(false);
                        //cout << "Aircraft ID: " << Aircrafts[Aircraft_index].getID() << " Status: " << Aircrafts[Aircraft_index].getStatus() << " Line: " << Aircrafts[Aircraft_index].getLine() << endl;
                    }
                }
            }
        }
        else if(command == "LANDING") {
            string ID;
            cin >> ID;
            int Aircraft_index = Find_Aircraft_With_ID(Aircrafts, ID);
            if(Aircraft_index != -1) {
                if(Aircrafts[Aircraft_index].getStatus() == 1) {
                    cout << "YOU ARE HERE" << endl;
                }
                else if(Aircrafts[Aircraft_index].getStatus() == 2) {
                    cout << "YOU ARE TAKING OFF" << endl;
                }
                else if(Aircrafts[Aircraft_index].getStatus() == 3) {
                    cout << "YOU ARE LANDING NOW" << endl;
                }
                /*else if(Aircrafts[Aircraft_index].getStatus() == 4 && Is_There_An_Empty_Runway_For_Landing(Runways) == -1) {
                    cout << "NO FREE BAND" << endl;
                }*/
            }
            else {
                int Runway_index = Is_There_An_Empty_Runway_For_Landing(Runways);
                if(Is_There_An_Empty_Runway_For_Landing(Runways) != -1) {
                    Aircraft temp = Aircraft(ID, 3);
                    temp.setLine(Runway_index+1);
                    Aircrafts.push_back(temp);
                    Runways[Runway_index].setStatus(false);
                    //cout << "Aircraft ID: " << temp.getID() << " Status: " << temp.getStatus() << " Line: " << temp.getLine() << endl;
                }
                else if(Is_There_An_Empty_Runway_For_Landing(Runways) == -1) {
                    cout << "NO FREE BAND" << endl;
                }
            }
        }
        else if(command == "PLANE-STATUS") {
            string ID;
            cin >> ID;
            int Aircraft_index = Find_Aircraft_With_ID(Aircrafts, ID);
            if(Aircraft_index == -1) {
                cout << 4 << endl;
            }
            else {
                cout << Aircrafts[Aircraft_index].getStatus() << endl;
            }
        }
        else if(command == "BAND-STATUS") {
            int LINE;
            cin >> LINE;
            int Aircraft_index = Find_Aircraft_With_Line(Aircrafts, LINE);
            if(Aircraft_index == -1) {
                cout << "FREE" << endl;
            }
            else {
                cout << Aircrafts[Aircraft_index].getID() << endl;
            }
        }
    }
    /*for(int i = 0; i < Runways.size(); i ++) {
        cout << "Runway no: " << i+1 << " ";
        if(Runways[i].getStatus()) {
            cout << "empty" << endl;
        }
        else {
            cout << "full" << endl;
        }
    }
    cout << "------------------------------" << endl;
    for(int i = 0; i < Aircrafts.size(); i ++) {
        cout << "Aircraft ID: " << Aircrafts[i].getID() << "Status: " << Aircrafts[i].getStatus() << endl;
    }*/
    return 0;
}