#pragma once
#include <iostream>
#include <vector>
#include <regex>
#include <iomanip>
#include <map>
#include <fstream>
#include "modelclasses.h"
#include "exceptions.h"
#include "circuitcontroller.h"
using namespace std;
class SchematicController {
protected:
    vector<string> file_names = {};
public:
    vector<string> Extractor(string line) {
        vector<string> derived_line;
        int i = 0;
        while (i < line.size()) {
            while (i < line.size() && line[i] == ' ') i++;
            string word = "";
            while (i < line.size() && line[i] != ' ') {
                word += line[i];
                i++;
            }
            if (!word.empty()) {
                derived_line.push_back(word);
            }
        }
        return derived_line;
    }
    void showExistingSchematics(bool& found) {
        if(file_names.empty()) {
            cout << "there was no schematics to be shown" << endl;
            found = true;
            return;
        }
        for(int i = 0; i < file_names.size(); i ++) {
            cout << i+1 << "-" << file_names[i] << endl;
        }
    }
    void isEneteredOptionValid(string line_num, string& main_file_name, bool& found, bool& emerg_ex) {
        if(line_num == "return") {
            emerg_ex = true;
            return;
        }
        for(int i = 0; i < line_num.size(); i ++) {
            if(line_num[i] <= 47 || line_num[i] >= 58) {
                throw InappropriateInput();
            }
        }
        if(stoi(line_num) <= 0 || stoi(line_num) > file_names.size()) {
            throw InappropriateInput();
        }
        main_file_name = file_names[stoi(line_num)-1];
        found = true;
    }
    string getFileNameFromPath(const string& path) {
        size_t lastSlash = path.find_last_of("/\\");
        string fullName = (lastSlash == string::npos) ? path : path.substr(lastSlash + 1);
        size_t dotPos = fullName.find_last_of('.');
        return (dotPos == string::npos) ? fullName : fullName.substr(0, dotPos);
    }
    void addToFileNames(string name) {
        this->file_names.push_back(name);
    }
    void showThisFile(string& main_file_name) {
        //address basis: /Users/abtin/Desktop/SpiceFiles/<file_name>.txt
        ifstream main_file("/Users/abtin/Desktop/SpiceFiles/" + main_file_name + ".txt");
        string line;
        while (getline(main_file, line)) {
            cout << line << endl;
        }
        main_file.close();
    }
    void showAllComponents(CircuitController* temp) {
        for(int i = 0; i < temp->getElements().size(); i ++) {
            if(temp->getElements()[i]->getType() == "SinusoidalVoltageSource") {
                cout << temp->getElements()[i]->getID() << " " << temp->getElements()[i]->getType() << " " << temp->getElements()[i]->getValue() << " " << temp->getElements()[i]->getValue1() << " " << temp->getElements()[i]->getValue2() << " " << temp->getElements()[i]->getStartNode()->getID() << " " << temp->getElements()[i]->getEndNode()->getID() << endl;
            }
            else if(temp->getElements()[i]->getType() == "Diode") {
                cout << temp->getElements()[i]->getID() << " " << temp->getElements()[i]->getType() << " " << temp->getElements()[i]->getType1() << " " << temp->getElements()[i]->getStartNode()->getID() << " " << temp->getElements()[i]->getEndNode()->getID() << endl;
            }
            else {
                cout << temp->getElements()[i]->getID() << " " << temp->getElements()[i]->getType() << " " << temp->getElements()[i]->getValue() << " " << temp->getElements()[i]->getStartNode()->getID() << " " << temp->getElements()[i]->getEndNode()->getID() << endl;
            }
        }
    }
    int findSchematicWithThisName(string& main_file_name, vector<CircuitController*>& Circuits) {
        for(int i = 0; i < Circuits.size(); i ++) {
            if(Circuits[i]->getName() == main_file_name) {
                return i;
            }
        }
        return -1;
    }
    void showThisSchematic(string& main_file_name, vector<CircuitController*>& Circuits) {
        int schem_index = findSchematicWithThisName(main_file_name, Circuits);
        vector<Element*> elements = Circuits[schem_index]->getElements();
        for(int i = 0; i < elements.size(); i ++) {
            if(elements[i]->getType() == "Resistor") {
                cout << "R" << " " << elements[i]->getID() << " " << elements[i]->getStartNode()->getID() << " " << elements[i]->getEndNode()->getID() << " " << elements[i]->getValue() << endl;
            }
            else if(elements[i]->getType() == "Capacitor") {
                cout << "C" << " " << elements[i]->getID() << " " << elements[i]->getStartNode()->getID() << " " << elements[i]->getEndNode()->getID() << " " << elements[i]->getValue() << endl;
            }
            else if(elements[i]->getType() == "Self") {
                cout << "L" << " " << elements[i]->getID() << " " << elements[i]->getStartNode()->getID() << " " << elements[i]->getEndNode()->getID() << " " << elements[i]->getValue() << endl;
            }
            else if(elements[i]->getType() == "Diode") {
                cout << "D" << " " << elements[i]->getID() << " " << elements[i]->getStartNode()->getID() << " " << elements[i]->getEndNode()->getID() << " " << elements[i]->getType1() << endl;
            }
            else if(elements[i]->getType() == "VoltageSource") {
                cout << "V" << " " << elements[i]->getID() << " " << elements[i]->getStartNode()->getID() << " " << elements[i]->getEndNode()->getID() << " " << elements[i]->getValue() << endl;
            }
            else if(elements[i]->getType() == "SinusoidalVoltageSource") {
                cout << "V" << " " << elements[i]->getID() << " " << elements[i]->getStartNode()->getID() << " " << elements[i]->getEndNode()->getID() << " " << elements[i]->getValue() << " " << elements[i]->getValue1() << " " << elements[i]->getValue2() << endl;
            }
            else if(elements[i]->getType() == "CurrentSource") {
                cout << "I" << " " << elements[i]->getID() << " " << elements[i]->getStartNode()->getID() << " " << elements[i]->getEndNode()->getID() << " " << elements[i]->getValue() << endl;
            }
            else if(elements[i]->getType() == "VoltageControlledVoltageSource") {
                cout << "E" << " " << elements[i]->getID() << " " << elements[i]->getStartNode()->getID() << " " << elements[i]->getEndNode()->getID() << " " << elements[i]->getValue() << " " << elements[i]->getControllingNode1()->getID() << " " << elements[i]->getControllingNode2()->getID() << endl;
            }
            else if(elements[i]->getType() == "VoltageControlledCurrentSource") {
                cout << "G" << " " << elements[i]->getID() << " " << elements[i]->getStartNode()->getID() << " " << elements[i]->getEndNode()->getID() << " " << elements[i]->getValue() << " " << elements[i]->getControllingNode1()->getID() << " " << elements[i]->getControllingNode2()->getID() << endl;
            }
            else if(elements[i]->getType() == "VoltageControlledVoltageSource") {
                cout << "H" << " " << elements[i]->getID() << " " << elements[i]->getStartNode()->getID() << " " << elements[i]->getEndNode()->getID() << " " << elements[i]->getValue() << " " << elements[i]->getControllingElement()->getID() << endl;
            }
            else if(elements[i]->getType() == "CurrentControlledCurrentSource") {
                cout << "F" << " " << elements[i]->getID() << " " << elements[i]->getStartNode()->getID() << " " << elements[i]->getEndNode()->getID() << " " << elements[i]->getValue() << " " << elements[i]->getControllingElement()->getID() << endl;
            }
        }
    }
    CircuitController*& FindCircuit(string main_file_name, vector<CircuitController*>& Circuits) {
        int circuit_index = findSchematicWithThisName(main_file_name, Circuits);
        return Circuits[circuit_index];
    }
    void GeneralInvalidities() {
        throw InvalidAddResistorFormat();
    }
};