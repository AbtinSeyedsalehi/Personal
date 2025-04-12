#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <algorithm>
using namespace std;
//======model======
class Manager {
private:
    string firstname;
    string lastname;
    string ID;
public:
    Manager(string firstname, string lastname, string ID) {
        this->firstname = firstname;
        this->lastname = lastname;
        this->ID = ID;
    }
    string getID() {return this->ID;}
    string getFirstName() {return this->firstname;}
    string getLastName() {return this->lastname;}
};
class Guest {
private:
    string firstname;
    string lastname;
    string ID;
    string phonenumber;
    int lenghtofstay = 0;
    int totalcost = 0;
    int usedservicescount = 0;
public:
    Guest(string firstname, string lastname, string ID, string phonenumber) {
        this->firstname = firstname;
        this->lastname = lastname;
        this->ID = ID;
        this->phonenumber = phonenumber;
    }
    void addtoUsedServicesCount(int add) {this->usedservicescount += add;}
    void addToTotalCost(int add) {this->totalcost += add;}
    void setLengthofStay(int lenghtofstay) {this->lenghtofstay = lenghtofstay;}
    string getID() {return this->ID;}
    string getFirstName() {return this->firstname;}
    string getLastName() {return this->lastname;}
    string getPhoneNumber() {return this->phonenumber;}
    int getLengthofStay() {return this->lenghtofstay;}
    int getTotalCost() {return this->totalcost;}
    int getUsedServicesCount() {return this->usedservicescount;}
};
class Service {
private:
    string name;
    string description;
    int basecost = 0;
    bool isSpecial;
public:
    Service(string name) {this->name = name;}
    bool getisSpecial() {return this->isSpecial;}
    int getBaseCost() {return this->basecost;}
    string getName() {return this->name;}
    void setBaseCost(int basecost) {this->basecost = basecost;}
    void setisSpecial(bool isSpecial) {this->isSpecial = isSpecial;}
};
class Room {
private:
    //vector<Service> specialservices;
    string ID;
    string type;
    int bedsnumber;
    bool isOccupied = false;
    bool isAvailable = true;
    string assignedspecialservice = "";
    int price = 0;
    Guest guest = Guest("", "", "", "");
public:
    Room(string ID, string type, int bedsnumber) {
        this->ID = ID;
        this-> type = type;
        this-> bedsnumber = bedsnumber;
    }
    Guest getGuest() {return this->guest;}
    string getAssignedSpecialService() {return this->assignedspecialservice;}
    bool getAvailablity() {return this->isAvailable;}
    //vector<Service> getSpecialServices() {return this->specialservices;}
    //void addTOSpecialServices(Service specialservice) {this->specialservices.push_back(specialservice);}
    string getID() {return this->ID;}
    string getType() {return this->type;}
    int getBedsNumber() {return this->bedsnumber;}
    void setOccupation(bool temp) {this->isOccupied = temp;}
    bool getOccupation() {return this->isOccupied;}
    int getPrice() {return this->price;}
    void setPrice(int price) {this->price = price;}
    void setAvailablity(bool isAvailable) {this->isAvailable = isAvailable;}
    void setGuest(Guest guest) {this->guest = guest;}
    void setAssignedSpecialService(string assignedspecialservice) {this->assignedspecialservice = assignedspecialservice;}
};
//=======controller=======
class ProgramController {
private:
    vector<Guest> guests;
    vector<Manager> managers;
    vector<Service> services;
    vector<Room> rooms;
public:
    vector<string> Extractor(string command) {
        vector<string> result;
        int num = command.size();
        string word = "";
        bool isprevspace = true;
        for(int i = 0; i < num; i++) {
            if(command[i] == ' ') {
                if (!isprevspace) {
                    result.push_back(word);
                    word = "";
                }
                isprevspace = true;
            }
            else {
                word += command[i];
                isprevspace = false;
            }
        }
        if(!word.empty()) {
            result.push_back(word);
        }
        return result;
    }
    int findManager(Manager temp) {
        for(int i = 0; i < managers.size(); i ++) {
            if(managers[i].getID() == temp.getID()) {
                return i;
            }
        }
        return -1;
    }
    int findGuest(Guest temp) {
        for(int i = 0; i < guests.size(); i ++) {
            if(guests[i].getID() == temp.getID()) {
                return i;
            }
        }
        return -1;
    }
    int findService(Service temp) {
        for(int i = 0; i < services.size(); i ++) {
            if(services[i].getName() == temp.getName()) {
                return i;
            }
        }
        return -1;
    }
    int findRoom(Room temp) {
        for(int i = 0; i < rooms.size(); i ++) {
            if(rooms[i].getID() == temp.getID() && rooms[i].getAvailablity()) {
                return i;
            }
        }
        return -1;
    }
    void registerManager(Manager temp) {
        int managerindex = findManager(temp);
        if(managerindex == -1) {
            managers.push_back(temp);
            cout << "manager " << temp.getFirstName() << " " << temp.getLastName() << " has been registered successfully" << endl;
        }
        else {
            cout << "manager " << temp.getFirstName() << " " << temp.getLastName() << " has been registered already" << endl;
        }
    }
    void registerGuest(Guest temp) {
        int guestindex = findGuest(temp);
        if(guestindex == -1) {
            guests.push_back(temp);
            cout << "guest " << temp.getID() << " has been registered successfully" << endl;
        }
        else {
            cout << "guest " << temp.getID() << " has been registered already" << endl;
        }
    }
    void assignTypetoService(Service temp) {
        if(temp.getName() == "Haunted Call") {
            temp.setisSpecial(true);
            temp.setBaseCost(15);
        }
        else if(temp.getName() == "Vampire Dining") {
            temp.setisSpecial(true);
            temp.setBaseCost(50);
        }
        else if(temp.getName() == "Mystic Encounter") {
            temp.setisSpecial(true);
            temp.setBaseCost(40);
        }
        else if(temp.getName() == "Ghoul's Meal") {
            temp.setisSpecial(false);
            temp.setBaseCost(30);
        }
        else if(temp.getName() == "Ghost Laundry") {
            temp.setisSpecial(false);
            temp.setBaseCost(20);
        }
        else if(temp.getName() == "Torture Gym") {
            temp.setisSpecial(false);
            temp.setBaseCost(10);
        }
        else if(temp.getName() == "Beast Massage") {
            temp.setisSpecial(false);
            temp.setBaseCost(50);
        }
        else if(temp.getName() == "Room Cleaning") {
            temp.setisSpecial(false);
            temp.setBaseCost(15);
        }
        else if(temp.getName() == "Moonlight Feast") {
            temp.setisSpecial(false);
            temp.setBaseCost(40);
        }
    }
    void addNoramlService(Service temp, Manager temp1) {
        int serviceindex = findService(temp);
        int managerindex = findManager(temp1);
        if(managerindex != -1) {
            if(serviceindex == -1) {
                assignTypetoService(temp);
                services.push_back(temp);
                cout << "service " << temp.getName() << " has been added successfully" << endl;
            }
            else {
                cout << "service " << temp.getName() << " has been added already" << endl;
            }
        }
        else {
            cout << "manager " << temp1.getID() << " is not registered" << endl;
        }
    }
    void removeNormalService(Service temp, Manager temp1) {
        int serviceindex = findService(temp);
        int managerindex = findManager(temp1);
        if(managerindex != -1) {
            if(serviceindex != -1) {
                services.erase(services.begin()+serviceindex-1);
                cout << "service " << services[serviceindex].getName() << " has been removed successfully" << endl;
            }
            else {
                cout << "service " << temp.getName() << " does not exist" << endl;
            }
        }
        else {
            cout << "manager " << temp1.getID() << " is not registered" << endl;
        }
    }
    void assignTypeToRoom(Room temp) {
        if(temp.getType() == "Coffin Retreat") {
            temp.setAssignedSpecialService("Haunted Call");
            temp.setPrice(50);
        }
        else if(temp.getType() == "Dungeon Royal") {
            temp.setPrice(80);
        }
        else if(temp.getType() == "Count's Suite") {
            temp.setAssignedSpecialService("Vampire Dining");
            temp.setPrice(150);
        }
        else if(temp.getType() == "Haunted Chamber") {
            temp.setAssignedSpecialService("Mystic Encounter");
            temp.setPrice(90);
        }
        else if(temp.getType() == "Shadow Penthouse") {
            temp.setPrice(200);
        }
    }
    void addRoom(Room temp, Manager temp1) {
        int roomindex = findRoom(temp);
        int managerindex = findManager(temp1);
        if(managerindex != -1) {
            if(roomindex != -1) {
                cout << "room " << temp.getID() << " has been added already" << endl;
            }
            else {
                assignTypeToRoom(temp);
                rooms.push_back(temp);
                cout << "room " << temp.getID() << " with type of " << temp.getType() << " has been added successfully" << endl;
            }
        }
        else {
            cout << "manager " << temp1.getID() << " is not registered" << endl;
        }
    }
    void removeRoom(Room temp, Manager temp1) {
        int roomindex = findRoom(temp);
        int managerindex = findManager(temp1);
        if(managerindex != -1) {
            if(roomindex != -1) {
                if(!rooms[roomindex].getOccupation()) {
                    rooms[roomindex].setAvailablity(false);
                    cout << "room " << rooms[roomindex].getID() << " with type of " << rooms[roomindex].getType() << " has been removed successfully" << endl;
                }
                else {
                    cout << "room " << temp.getID() << " is not empty" << endl;
                }
            }
            else {
                cout << "room " << temp.getID() << " does not exist" << endl;
            }
        }
        else {
            cout << "manager " << temp1.getID() << " is not registered" << endl;
        }
    }
    void CheckInWithReserve(Guest temp, Room temp1, int lengthofstay) {
        int guestindex = findGuest(temp);
        int roomindex = findRoom(temp1);
        if((guestindex != -1) && (temp.getFirstName() == guests[guestindex].getFirstName()) && (temp.getLastName() == guests[guestindex].getLastName())) {
            if(roomindex != -1) {
                if(!rooms[roomindex].getOccupation() && rooms[roomindex].getAvailablity()) {
                    guests[guestindex].setLengthofStay(lengthofstay);
                    rooms[roomindex].setOccupation(true);
                    rooms[roomindex].setGuest(guests[guestindex]);
                    cout << "guest " << temp.getFirstName() << " " << temp.getLastName() << " with ID " << temp.getID() << " has been checked in successfully" << endl;
                }
                else  {
                    cout << "room " << temp1.getID() << " is not empty" << endl;
                }
            }
            else {
                cout << "room " << temp1.getID() << " does not exist" << endl;
            }
        }
        else {
            cout << "guest " << temp.getFirstName() << " " << temp.getLastName() << " with ID " << temp.getID() << " has not been registered yet" << endl;
        }
    }
    int findRoomWithoutSpecialServices() {
        for(int i = 0; i < rooms.size(); i ++) {
            if(rooms[i].getAssignedSpecialService() == "" && rooms[i].getAvailablity()) {
                return i;
            }
        }
        return -1;
    }
    int findRoomWithLeastPrice() {
        int index = 0;
        vector<int> indexes;
        for(int i = 1; i < rooms.size(); i ++) {
            if(rooms[i].getPrice() < rooms[index].getPrice()) {
                index = i;
            }
        }
        for(int i = 1; i < rooms.size(); i ++) {
            if(rooms[i].getPrice() == rooms[index].getPrice()) {
                indexes.push_back(i);
            }
        }
        for(int i = 0; i < indexes.size(); i ++) {
            if(!rooms[indexes[i]].getOccupation() && rooms[indexes[i]].getAvailablity()) {
                return indexes[i];
            }
        }
        return -1;
    }
    //this function should be changed
    void sortRoomsbasedOnLexicograpghy() {
        vector<string> roomIDs;
        for(int i = 0; i < rooms.size(); i ++) {
            roomIDs.push_back(rooms[i].getID());
        }
        vector<Room> newrooms;
        sort(roomIDs.begin(), roomIDs.end());
        for(int i = 0; i < roomIDs.size(); i ++) {
            for(int j = 0; j < rooms.size(); j ++) {
                if(roomIDs[i] == rooms[j].getID()) {
                    newrooms.push_back(rooms[j]);
                    break;
                }
            }
        }
        this->rooms = newrooms;
        /*for(int i = 0; i < rooms.size(); i ++) {
            cout << rooms[i].getID() << endl;
        }*/
    }
    int findEmptyRoom() {
        for(int i = 0; i < this->rooms.size(); i ++) {
            if(!rooms[i].getOccupation() && rooms[i].getAvailablity()) {
                return i;
            }
        }
        return -1;
    }
    int findRoomforReserve() {
        if(findRoomWithoutSpecialServices() != -1 && rooms[findRoomWithoutSpecialServices()].getAvailablity()) {
            return findRoomWithoutSpecialServices();
        }
        else {
            if(findRoomWithLeastPrice() != -1 && rooms[findRoomWithLeastPrice()].getAvailablity()) {//can take errors
                return findRoomWithLeastPrice();
            }
            else {
                sortRoomsbasedOnLexicograpghy();
                int roomindex = findEmptyRoom();
                if(roomindex != -1 && rooms[roomindex].getAvailablity()) {
                    return roomindex;
                }
                else {
                    return -1;
                }
            }
        }
    }
    void CheckInWithoutReserve(Guest temp, int lengthofstay, int roomindex) {
        int guestindex = findGuest(temp);
        if(guestindex != -1) {
            if(roomindex != -1) {
                cout << "guest " << guests[guestindex].getFirstName() << " " << guests[guestindex].getLastName() << " with ID " << guests[guestindex].getID() << "has been checked into room " << rooms[roomindex].getID() << endl;
                rooms[roomindex].setOccupation(true);
                guests[guestindex].setLengthofStay(lengthofstay);
                rooms[roomindex].setGuest(guests[guestindex]);
            }
            else {
                cout << "there are no empty rooms" << endl;
            }
        }
        else {
            cout << "guest " << temp.getFirstName() << " " << temp.getLastName() << " with ID " << temp.getID() << " has not been registered yet" << endl;
        }
    }
    int findRoomWithGuestID(string ID) {
        for(int i = 0; i < rooms.size(); i ++) {
            if(rooms[i].getGuest().getID() == ID) {
                return i;
            }
        }
        return -1;
    }
    void CheckOut(Guest temp) {
        int guestindex = findGuest(temp);
        if((guestindex != -1) && (temp.getFirstName() == guests[guestindex].getFirstName()) && (temp.getLastName() == guests[guestindex].getLastName())) {
            if(guests[guestindex].getLengthofStay() != 0) {
                int roomindex = findRoomWithGuestID(temp.getID());
                rooms[roomindex].setGuest(guests[guestindex]);
                rooms[roomindex].setOccupation(false);
                cout << "guest with ID " << temp.getID() << " has checked out with a cost of " << temp.getTotalCost() << endl;
            }
            else {
                cout << "person " << temp.getID() << " is not a guest" << endl;
            }
        }
        else {
            cout << "guest " << temp.getFirstName() << " " << temp.getLastName() << " with ID " << temp.getID() << " has not been registered yet" << endl;
        }
    }
    void useService(Service temp, Guest temp1) {
        int guestindex = findGuest(temp1);
        int serviceindex = findService(temp);
        if(guestindex != -1) {
            if(serviceindex != -1) {
                cout << "guest " << temp1.getID() << " has used the service " << temp.getName() << " successfully" << endl;
            }
            else {
                cout << "service " << services[serviceindex].getName() << " does not exist" << endl;
            }
        }
        else {
            cout << "guest with ID " << temp1.getID() << " has not been registered or checked in yet" << endl;
        }
    }
    void showGuestInfo(Guest temp) {
        int guestindex = findGuest(temp);
        if(guestindex != -1) {
            cout << "name: " << guests[guestindex].getFirstName() << " " << guests[guestindex].getLastName() << endl;
            cout << "phone number: " << guests[guestindex].getPhoneNumber() << endl;
            cout << "ID number: " << guests[guestindex].getID() << endl;
            cout << "length of stay: " << guests[guestindex].getLengthofStay() << endl;
            cout << "used services: " << guests[guestindex].getUsedServicesCount() << endl;
        }
        else {
            cout << "guest " << temp.getID() << " has not been registered yet" << endl;
        }
    }
    void showRoomInfo(Room temp) {
        int roomindex = findRoom(temp);
        if(roomindex != -1) {
            cout << "ID number: " << rooms[roomindex].getID() << endl;
            cout << "type: " << rooms[roomindex].getType() << endl;
            cout << "number of beds: " << rooms[roomindex].getBedsNumber() << endl;
            //this line should be checked : total income not defined properly
            //cout << "total income: " << rooms[roomindex].get
            if(rooms[roomindex].getGuest().getID() != "") {
                cout << "guest: " << rooms[roomindex].getGuest().getID() << endl;
            }
            else {
                cout << "guest: empty" << endl;
            }
        }
        else {
            cout << "room " << temp.getID() << " does not exist" << endl;
        }
    }
};
int main() {
    ProgramController PC;
    regex pattern("(^exit$)");
    regex pattern0("(^register manager \\S+ \\S+ with ID \\S+$)");
    regex pattern1("(^register guest \\S+ \\S+ with ID \\S+ and phone number \\S+$)");
    regex pattern2("(^add service (.+) by manager \\S+$)");
    regex pattern3("(^remove service (.+) by manager \\S+$)");
    regex pattern4("(^add room \\S+ \\S+ \\d+ by manager \\S+$)");
    regex pattern5("(^remove room \\S+ by manager \\S+$)");
    regex pattern6("(^check in guest \\S+ \\S+ \\S+ in room \\S+ for \\d+ nights$)");
    regex pattern7("(^check in guest \\S+ \\S+ \\S+ for \\d+ nights$)");
    regex pattern8("(^check out guest \\S+ \\S+ \\S+$)");
    regex pattern9("(^use service \\S+ by guest \\S+$)");
    regex pattern10("(^show guest information \\S+$)");
    regex pattern11("(^show room information \\S+$)");
    regex pattern12("(^$)");
    regex pattern13("(^$)");
    regex pattern14("(^$)");
    regex pattern15("(^$)");
    while(1) {
        string line;
        getline(cin, line);
        if(regex_search(line, pattern)) {
            break;
        }
        else if(regex_search(line, pattern0)) {
            Manager temp = Manager(PC.Extractor(line)[2], PC.Extractor(line)[3], PC.Extractor(line)[6]);
            PC.registerManager(temp);
        }
        else if(regex_search(line, pattern1)) {
            Guest temp = Guest(PC.Extractor(line)[2], PC.Extractor(line)[3], PC.Extractor(line)[6], PC.Extractor(line)[10]);
            PC.registerGuest(temp);
        }
        else if(regex_search(line, pattern2)) {
            int startindex, finishindex;
            for(int i = 0; i < PC.Extractor(line).size(); i ++) {
                if(PC.Extractor(line)[i] == "service") {
                    startindex = i;
                }
                else if(PC.Extractor(line)[i] == "by") {
                    finishindex = i;
                }
            }
            string name = "";
            for(int i = startindex+1; i < finishindex; i ++) {
                name += PC.Extractor(line)[i];
                if(i != finishindex-1) {
                    name += " ";
                }
            }
            int IDindex;
            for(int i = 0; i < PC.Extractor(line).size(); i ++) {
                if(PC.Extractor(line)[i] == "manager") {
                    IDindex = i;
                    break;
                }
            }
            Service temp = Service(name);
            Manager temp1 = Manager("", "", PC.Extractor(line)[IDindex+1]);
            PC.addNoramlService(temp, temp1);
        }
        else if(regex_search(line, pattern3)) {
            int startindex, finishindex;
            for(int i = 0; i < PC.Extractor(line).size(); i ++) {
                if(PC.Extractor(line)[i] == "service") {
                    startindex = i;
                }
                else if(PC.Extractor(line)[i] == "by") {
                    finishindex = i;
                }
            }
            string name = "";
            for(int i = startindex+1; i < finishindex; i ++) {
                name += PC.Extractor(line)[i];
                if(i != finishindex-1) {
                    name += " ";
                }
            }
            int IDindex;
            for(int i = 0; i < PC.Extractor(line).size(); i ++) {
                if(PC.Extractor(line)[i] == "manager") {
                    IDindex = i;
                    break;
                }
            }
            Service temp = Service(name);
            Manager temp1 = Manager("", "", PC.Extractor(line)[IDindex+1]);
            PC.removeNormalService(temp, temp1);
        }
        else if(regex_search(line, pattern4)) {
            Room temp = Room(PC.Extractor(line)[2], PC.Extractor(line)[3], stoi(PC.Extractor(line)[4]));
            Manager temp1 = Manager("", "", PC.Extractor(line)[7]);
            PC.addRoom(temp, temp1);
        }
        else if(regex_search(line, pattern5)) {
            Room temp = Room(PC.Extractor(line)[2], "", 0);
            Manager temp1 = Manager("", "", PC.Extractor(line)[5]);
            PC.removeRoom(temp, temp1);
        }
        else if(regex_search(line, pattern6)) {
            Guest temp = Guest(PC.Extractor(line)[3], PC.Extractor(line)[4], PC.Extractor(line)[5], "");
            Room temp1 = Room(PC.Extractor(line)[8], "", 0);
            int lengthofstay = stoi(PC.Extractor(line)[10]);
            PC.CheckInWithReserve(temp, temp1, lengthofstay);
        }
        else if(regex_search(line, pattern7)) {
            Guest temp = Guest(PC.Extractor(line)[3], PC.Extractor(line)[4], PC.Extractor(line)[5], "");
            int lengthofstay = stoi(PC.Extractor(line)[7]);
            int roomindex = PC.findRoomforReserve();
            PC.CheckInWithoutReserve(temp, lengthofstay, roomindex);
        }
        else if(regex_search(line, pattern8)) {
            Guest temp = Guest(PC.Extractor(line)[3], PC.Extractor(line)[4], PC.Extractor(line)[5], "");
            PC.CheckOut(temp);
        }
        else if(regex_search(line, pattern9)) {
            Service temp = Service(PC.Extractor(line)[2]);
            Guest temp1 = Guest("", "", PC.Extractor(line)[5], "");
            PC.useService(temp, temp1);
        }
        else if(regex_search(line, pattern10)) {
            Guest temp = Guest("", "", PC.Extractor(line)[3], "");
            PC.showGuestInfo(temp);
        }
        else if(regex_search(line, pattern11)) {
            Room temp = Room(PC.Extractor(line)[3], "", 0);
            PC.showRoomInfo(temp);
        }
        else if(line == "sort") {
            PC.sortRoomsbasedOnLexicograpghy();
        }
    }
    return 0;
}