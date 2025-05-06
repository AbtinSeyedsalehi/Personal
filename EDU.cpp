#include <iostream>
#include <vector>
#include <regex>
using namespace std;
class Person {
protected:
    string name;
    string national_id;
    string phone_number;
public:
    Person(string name, string national_id, string phone_number) {
        this->name = name;
        this->national_id = national_id;
        this->phone_number = phone_number;
    }
    virtual void displayinfo() {}
};
class Student : public Person {
protected:
    string major;
    string student_id;
    string entrance_year;
public:
    Student(string name, string national_id, string phone_number, string major, string student_id, string entrance_year) : Person(name, national_id, phone_number) {
        this->major = major;
        this->student_id = student_id;
        this->entrance_year = entrance_year;
    }
    void displayinfo() {}
    string getName() {return this->name;}
    string getNationalID() {return this->national_id;}
};
class Professor : public Person {
protected:
    string department;
    int salary;
public:
    void displayinfo() {}
};
class Assistant : public Student {
protected:
    string role;
public:
    void displayinfo() {}
};
class Course {
private:
    string course_name;
    Professor* instructor;
    vector<Assistant*> assistants;
    vector<Student*> students;
public:
    void displaycourseinfo() {}
};
class ProgramController {
private:
    vector<Student> all_students;
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
    void error() {
        cout << "OOPs, something went wrong!" << endl;
    }
    int isStudentValid(string fullname, string national_id) {
        for(int i = 0; i < this->all_students.size(); i ++) {
            if(this->all_students[i].getName() == fullname && this->all_students[i].getNationalID() == national_id) {
                return i;
            }
        }
        return -1;
    }
    void addstudent(string fullname, string national_id, string phone_number, string major, string student_id, string entrance_year) {
        Student temp = Student(fullname, national_id, phone_number, major, student_id, entrance_year);
        if(isStudentValid(fullname, national_id) == -1) {
            this->all_students.push_back(temp);
            cout << "Student added successfully!" << endl;
        }
        else {
            this->error();
        }
    }
    void showallstuds() {
        for(int i = 0; i < all_students.size(); i ++) {
            cout << all_students[i].getName() << " " << all_students[i].getNationalID() << endl;
        }
    }
};
int main() {
    regex pattern(R"(Add student (\S+) (\S+) (\d{10}) (\d{10}) (\S+) (\d{9}) (\d{4}))");
    ProgramController PC;
    while(1) {
        string line;
        getline(cin, line);
        if(line == "end") {
            break;
        }
        else if(regex_search(line, pattern)) {
            vector<string> command = PC.Extractor(line);
            PC.addstudent(command[2]+" "+command[3], command[4], command[5], command[6], command[7], command[8]);
        }
        else if(line == "student check") {
            PC.showallstuds();
        }
        else {
            cout << "OOPs, something went wrong!" << endl;
        }
    }
    return 0;
}