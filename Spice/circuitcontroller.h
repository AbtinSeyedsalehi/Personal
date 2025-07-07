#pragma once
#include <iostream>
#include <vector>
#include <regex>
#include <iomanip>
#include <map>
#include "modelclasses.h"
#include "exceptions.h"
using namepspace std;
class CircuitController {
protected:
    string name;
    vector<Node*> nodes;
    vector<Element*> elements;
public:
    CircuitController(string name) : name(name) {}
    string getName() {return this->name;}
    vector<Node*>& getNodes() {return this->nodes;}
    vector<Element*>& getElements() {return this->elements;}
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
    int findNodeWithID(string ID) {
        for(int i = 0; i < this->nodes.size(); i ++) {
            if(this->nodes[i]->getID() == ID) {
                return i;
            }
        }
        return -1;
    }
    bool isThisNodeInUse(string ID) {
        for(int i = 0; i < elements.size(); i ++) {
            if(elements[i]->getStartNode()->getID() == ID || elements[i]->getEndNode()->getID() == ID) {
                return true;
            }
        }
        return false;
    }
    bool isResistorNameFromatValid(string ID) {
        if(ID[0] != 'R') {
            return false;
        }
        for(int i = 0; i < ID.size(); i ++) {
            if(ID[i] <= 47 || (ID[i] >= 58 && ID[i] <= 64) || (ID[i] >= 91 && ID[i] <= 96) || ID[i] >= 123) {
                return false;
            }
        }
        return true;
    }
    bool isCapacitorNameFromatValid(string ID) {
        if(ID[0] != 'C') {
            return false;
        }
        for(int i = 0; i < ID.size(); i ++) {
            if(ID[i] <= 47 || (ID[i] >= 58 && ID[i] <= 64) || (ID[i] >= 91 && ID[i] <= 96) || ID[i] >= 123) {
                return false;
            }
        }
        return true;
    }
    bool isSelfNameFromatValid(string ID) {
        if(ID[0] != 'L') {
            return false;
        }
        for(int i = 0; i < ID.size(); i ++) {
            if(ID[i] <= 47 || (ID[i] >= 58 && ID[i] <= 64) || (ID[i] >= 91 && ID[i] <= 96) || ID[i] >= 123) {
                return false;
            }
        }
        return true;
    }
    bool isDiodeNameFromatValid(string ID) {
        if(ID[0] != 'D' && ID[0] != 'Z') {
            return false;
        }
        for(int i = 0; i < ID.size(); i ++) {
            if(ID[i] <= 47 || (ID[i] >= 58 && ID[i] <= 64) || (ID[i] >= 91 && ID[i] <= 96) || ID[i] >= 123) {
                return false;
            }
        }
        return true;
    }
    bool isDiodeModelFormatValid(string model) {
        if(model != "D" && model != "Z") {
            return false;
        }
        return true;
    }
    bool isGroundNameFromatValid(string ID) {
        if(ID != "GND") {
            return false;
        }
        for(int i = 0; i < ID.size(); i ++) {
            if(ID[i] <= 47 || (ID[i] >= 58 && ID[i] <= 64) || (ID[i] >= 91 && ID[i] <= 96) || ID[i] >= 123) {
                return false;
            }
        }
        return true;
    }
    bool isVoltageSourceNameFromatValid(string ID) {
        if(ID[0] != 'V') {
            return false;
        }
        for(int i = 0; i < ID.size(); i ++) {
            if(ID[i] <= 47 || (ID[i] >= 58 && ID[i] <= 64) || (ID[i] >= 91 && ID[i] <= 96) || ID[i] >= 123) {
                return false;
            }
        }
        return true;
    }
    bool isCurrentSourceNameFromatValid(string ID) {
        string temp = "";
        for(int i = 0; i < 13; i ++) {
            temp += ID[i];
        }
        if(temp != "CurrentSource") {
            return false;
        }
        for(int i = 0; i < ID.size(); i ++) {
            if(ID[i] <= 47 || (ID[i] >= 58 && ID[i] <= 64) || (ID[i] >= 91 && ID[i] <= 96) || ID[i] >= 123) {
                return false;
            }
        }
        return true;
    }
    int doesElementWithThisNameAndTypeAlreadyExist(string ID, string type) {
        for(int i = 0; i < elements.size(); i ++) {
            if(elements[i]->getType() == type && elements[i]->getID() == ID) {
                return i;
            }
        }
        return -1;
    }
    double parseSpiceNumber(const string& input) {
        static const std::map<std::string, double> prefixes = {
                {"T", 1e12},
                {"G", 1e9},
                {"MEG", 1e6},
                {"Meg", 1e6},
                {"K", 1e3},
                {"k", 1e3},
                {"m", 1e-3},
                {"u", 1e-6},
                {"n", 1e-9},
                {"p", 1e-12},
                {"f", 1e-15}
        };
        for (const auto& [prefix, factor] : prefixes) {
            if (input.size() > prefix.size() &&
                input.substr(input.size() - prefix.size()) == prefix) {
                string numberPart = input.substr(0, input.size() - prefix.size());
                try {
                    double base = stod(numberPart);
                    return base * factor;
                } catch (...) {
                    throw runtime_error("Failed");
                }
            }
        }
        try {
            return stod(input);
        } catch (...) {
            throw runtime_error("Failed");
        }
    }
    void addResistor(string ID, string node1_ID, string node2_ID, string combined_resistance) {
        double resistance = parseSpiceNumber(combined_resistance);
        //cout << resistance << endl;
        if(resistance <= 0) {
            throw InvalidValue("Resistance");
        }
        if(!isResistorNameFromatValid(ID)) {
            throw InvalidElement(ID);
        }
        int resistor_index = doesElementWithThisNameAndTypeAlreadyExist(ID, "Resistor");
        if(resistor_index != -1) {
            throw AlreadyExistingResistor(ID);
        }
        int node1_index = findNodeWithID(node1_ID);
        int node2_index = findNodeWithID(node2_ID);
        if(node1_index != -1) {
            if(node2_index != -1) {
                Resistor* temp = new Resistor(ID, this->nodes[node1_index], this->nodes[node2_index], resistance);
                this->elements.push_back(temp);
            }
            else {
                Node* temp = new Node(node2_ID);
                this->nodes.push_back(temp);
                node2_index = findNodeWithID(node2_ID);
                Resistor* temp1 = new Resistor(ID, this->nodes[node1_index], this->nodes[node2_index], resistance);
                this->elements.push_back(temp1);
            }
        }
        else {
            if(node2_index != -1) {
                Node* temp = new Node(node1_ID);
                this->nodes.push_back(temp);
                node1_index = findNodeWithID(node1_ID);
                Resistor* temp1 = new Resistor(ID, this->nodes[node1_index], this->nodes[node2_index], resistance);
                this->elements.push_back(temp1);
            }
            else {
                Node* temp = new Node(node1_ID);
                this->nodes.push_back(temp);
                Node* temp1 = new Node(node2_ID);
                this->nodes.push_back(temp1);
                node1_index = findNodeWithID(node1_ID);
                node2_index = findNodeWithID(node2_ID);
                Resistor* temp2 = new Resistor(ID, this->nodes[node1_index], this->nodes[node2_index], resistance);
                this->elements.push_back(temp2);
            }
        }
    }
    void deleteResistor(string ID) {
        int resistor_index = doesElementWithThisNameAndTypeAlreadyExist(ID, "Resistor");
        if(resistor_index == -1) {
            throw RemovingNonExistantResistor();
        }
        int node1_index = findNodeWithID(elements[resistor_index]->getStartNode()->getID());
        int node2_index = findNodeWithID(elements[resistor_index]->getEndNode()->getID());
        this->elements.erase(elements.begin()+resistor_index);
        string node1_ID = this->nodes[node1_index]->getID();
        string node2_ID = this->nodes[node2_index]->getID();
        if (!isThisNodeInUse(node1_ID) && !isThisNodeInUse(node2_ID)) {
            if (node1_index > node2_index) {
                nodes.erase(nodes.begin() + node1_index);
                nodes.erase(nodes.begin() + node2_index);
            } else {
                nodes.erase(nodes.begin() + node2_index);
                nodes.erase(nodes.begin() + node1_index);
            }
        } else {
            if (!isThisNodeInUse(node1_ID)) {
                nodes.erase(nodes.begin() + node1_index);
            }
            if (!isThisNodeInUse(node2_ID)) {
                nodes.erase(nodes.begin() + node2_index);
            }
        }
    }
    void addCapacitor(string ID, string node1_ID, string node2_ID, string combined_capacitance) {
        double capacitance = parseSpiceNumber(combined_capacitance);
        //cout << capacitance << endl;
        if(capacitance <= 0) {
            throw InvalidValue("Capacitance");
        }
        if(!isCapacitorNameFromatValid(ID)) {
            throw InvalidElement(ID);
        }
        int capacitor_index = doesElementWithThisNameAndTypeAlreadyExist(ID, "Capacitor");
        if(capacitor_index != -1) {
            throw AlreadyExistingCapacitor(ID);
        }
        int node1_index = findNodeWithID(node1_ID);
        int node2_index = findNodeWithID(node2_ID);
        if(node1_index != -1) {
            if(node2_index != -1) {
                Capacitor* temp = new Capacitor(ID, this->nodes[node1_index], this->nodes[node2_index], capacitance);
                this->elements.push_back(temp);
            }
            else {
                Node* temp = new Node(node2_ID);
                this->nodes.push_back(temp);
                node2_index = findNodeWithID(node2_ID);
                Capacitor* temp1 = new Capacitor(ID, this->nodes[node1_index], this->nodes[node2_index], capacitance);
                this->elements.push_back(temp1);
            }
        }
        else {
            if(node2_index != -1) {
                Node* temp = new Node(node1_ID);
                this->nodes.push_back(temp);
                node1_index = findNodeWithID(node1_ID);
                Capacitor* temp1 = new Capacitor(ID, this->nodes[node1_index], this->nodes[node2_index], capacitance);
                this->elements.push_back(temp1);
            }
            else {
                Node* temp = new Node(node1_ID);
                this->nodes.push_back(temp);
                Node* temp1 = new Node(node2_ID);
                this->nodes.push_back(temp1);
                node1_index = findNodeWithID(node1_ID);
                node2_index = findNodeWithID(node2_ID);
                Capacitor* temp2 = new Capacitor(ID, this->nodes[node1_index], this->nodes[node2_index], capacitance);
                this->elements.push_back(temp2);
            }
        }
    }
    void deleteCapacitor(string ID) {
        int capacitor_index = doesElementWithThisNameAndTypeAlreadyExist(ID, "Capacitor");
        if(capacitor_index == -1) {
            throw RemovingNonExistantCapacitor();
        }
        int node1_index = findNodeWithID(elements[capacitor_index]->getStartNode()->getID());
        int node2_index = findNodeWithID(elements[capacitor_index]->getEndNode()->getID());
        this->elements.erase(elements.begin()+capacitor_index);
        string node1_ID = this->nodes[node1_index]->getID();
        string node2_ID = this->nodes[node2_index]->getID();
        if (!isThisNodeInUse(node1_ID) && !isThisNodeInUse(node2_ID)) {
            if (node1_index > node2_index) {
                nodes.erase(nodes.begin() + node1_index);
                nodes.erase(nodes.begin() + node2_index);
            } else {
                nodes.erase(nodes.begin() + node2_index);
                nodes.erase(nodes.begin() + node1_index);
            }
        } else {
            if (!isThisNodeInUse(node1_ID)) {
                nodes.erase(nodes.begin() + node1_index);
            }
            if (!isThisNodeInUse(node2_ID)) {
                nodes.erase(nodes.begin() + node2_index);
            }
        }
    }
    void addSelf(string ID, string node1_ID, string node2_ID, string combined_inductance) {
        double inductance = parseSpiceNumber(combined_inductance);
        //cout << inductance << endl;
        if(inductance <= 0) {
            throw InvalidValue("Inductance");
        }
        if(!isSelfNameFromatValid(ID)) {
            throw InvalidElement(ID);
        }
        int self_index = doesElementWithThisNameAndTypeAlreadyExist(ID, "Self");
        if(self_index != -1) {
            throw AlreadyExistingSelf(ID);
        }
        int node1_index = findNodeWithID(node1_ID);
        int node2_index = findNodeWithID(node2_ID);
        if(node1_index != -1) {
            if(node2_index != -1) {
                Self* temp = new Self(ID, this->nodes[node1_index], this->nodes[node2_index], inductance);
                this->elements.push_back(temp);
            }
            else {
                Node* temp = new Node(node2_ID);
                this->nodes.push_back(temp);
                node2_index = findNodeWithID(node2_ID);
                Self* temp1 = new Self(ID, this->nodes[node1_index], this->nodes[node2_index], inductance);
                this->elements.push_back(temp1);
            }
        }
        else {
            if(node2_index != -1) {
                Node* temp = new Node(node1_ID);
                this->nodes.push_back(temp);
                node1_index = findNodeWithID(node1_ID);
                Self* temp1 = new Self(ID, this->nodes[node1_index], this->nodes[node2_index], inductance);
                this->elements.push_back(temp1);
            }
            else {
                Node* temp = new Node(node1_ID);
                this->nodes.push_back(temp);
                Node* temp1 = new Node(node2_ID);
                this->nodes.push_back(temp1);
                node1_index = findNodeWithID(node1_ID);
                node2_index = findNodeWithID(node2_ID);
                Self* temp2 = new Self(ID, this->nodes[node1_index], this->nodes[node2_index], inductance);
                this->elements.push_back(temp2);
            }
        }
    }
    void deleteSelf(string ID) {
        int self_index = doesElementWithThisNameAndTypeAlreadyExist(ID, "Self");
        if(self_index == -1) {
            throw RemovingNonExistantSelf();
        }
        int node1_index = findNodeWithID(elements[self_index]->getStartNode()->getID());
        int node2_index = findNodeWithID(elements[self_index]->getEndNode()->getID());
        this->elements.erase(elements.begin()+self_index);
        string node1_ID = this->nodes[node1_index]->getID();
        string node2_ID = this->nodes[node2_index]->getID();
        if (!isThisNodeInUse(node1_ID) && !isThisNodeInUse(node2_ID)) {
            if (node1_index > node2_index) {
                nodes.erase(nodes.begin() + node1_index);
                nodes.erase(nodes.begin() + node2_index);
            } else {
                nodes.erase(nodes.begin() + node2_index);
                nodes.erase(nodes.begin() + node1_index);
            }
        } else {
            if (!isThisNodeInUse(node1_ID)) {
                nodes.erase(nodes.begin() + node1_index);
            }
            if (!isThisNodeInUse(node2_ID)) {
                nodes.erase(nodes.begin() + node2_index);
            }
        }
    }
    void addDiode(string ID, string node1_ID, string node2_ID, string model) {
        if(!isDiodeModelFormatValid(model)) {
            throw InvalidDiodeModel(model);
        }
        if(!isDiodeNameFromatValid(ID)) {
            throw InvalidElement(ID);
        }
        int diode_index = doesElementWithThisNameAndTypeAlreadyExist(ID, "Diode");
        if(diode_index != -1) {
            throw AlreadyExistingDiode(ID);
        }
        int node1_index = findNodeWithID(node1_ID);
        int node2_index = findNodeWithID(node2_ID);
        if(node1_index != -1) {
            if(node2_index != -1) {
                Diode* temp = new Diode(ID, this->nodes[node1_index], this->nodes[node2_index], model);
                this->elements.push_back(temp);
            }
            else {
                Node* temp = new Node(node2_ID);
                this->nodes.push_back(temp);
                node2_index = findNodeWithID(node2_ID);
                Diode* temp1 = new Diode(ID, this->nodes[node1_index], this->nodes[node2_index], model);
                this->elements.push_back(temp1);
            }
        }
        else {
            if(node2_index != -1) {
                Node* temp = new Node(node1_ID);
                this->nodes.push_back(temp);
                node1_index = findNodeWithID(node1_ID);
                Diode* temp1 = new Diode(ID, this->nodes[node1_index], this->nodes[node2_index], model);
                this->elements.push_back(temp1);
            }
            else {
                Node* temp = new Node(node1_ID);
                this->nodes.push_back(temp);
                Node* temp1 = new Node(node2_ID);
                this->nodes.push_back(temp1);
                node1_index = findNodeWithID(node1_ID);
                node2_index = findNodeWithID(node2_ID);
                Diode* temp2 = new Diode(ID, this->nodes[node1_index], this->nodes[node2_index], model);
                this->elements.push_back(temp2);
            }
        }
    }
    void deleteDiode(string ID) {
        int diode_index = doesElementWithThisNameAndTypeAlreadyExist(ID, "Diode");
        if(diode_index == -1) {
            throw RemovingNonExistantSelf();
        }
        int node1_index = findNodeWithID(elements[diode_index]->getStartNode()->getID());
        int node2_index = findNodeWithID(elements[diode_index]->getEndNode()->getID());
        this->elements.erase(elements.begin()+diode_index);
        string node1_ID = this->nodes[node1_index]->getID();
        string node2_ID = this->nodes[node2_index]->getID();
        if (!isThisNodeInUse(node1_ID) && !isThisNodeInUse(node2_ID)) {
            if (node1_index > node2_index) {
                nodes.erase(nodes.begin() + node1_index);
                nodes.erase(nodes.begin() + node2_index);
            } else {
                nodes.erase(nodes.begin() + node2_index);
                nodes.erase(nodes.begin() + node1_index);
            }
        } else {
            if (!isThisNodeInUse(node1_ID)) {
                nodes.erase(nodes.begin() + node1_index);
            }
            if (!isThisNodeInUse(node2_ID)) {
                nodes.erase(nodes.begin() + node2_index);
            }
        }
    }
    void addGround(string ID, string node1_ID) {
        if(!isGroundNameFromatValid(ID)) {
            throw InvalidElement(ID);
        }
        int node1_index = findNodeWithID(node1_ID);
        if(node1_index != -1) {
            this->nodes[node1_index]->makeGround();
        }
        else {
            Node* temp = new Node(node1_ID);
            temp->makeGround();
            this->nodes.push_back(temp);
        }
    }
    void deleteGround(string ID) {
        int node_index = findNodeWithID(ID);
        if(node_index == -1) {
            throw NonExsistantGNDNode();
        }
        this->nodes[node_index]->RemoveAsGround();
    }
    void addVoltageSource(string ID, string node1_ID, string node2_ID, string combined_voltage) {
        string temp_ID = "";
        for(int i = 13; i < ID.size(); i ++) {
            temp_ID += ID[i];
        }
        ID = temp_ID;
        double voltage = parseSpiceNumber(combined_voltage);
        //cout << voltage << endl;
        if(voltage <= 0) {
            throw InvalidValue("Voltage");
        }
        if(!isVoltageSourceNameFromatValid(ID)) {
            throw InvalidElement(ID);
        }
        int voltage_index = doesElementWithThisNameAndTypeAlreadyExist(ID, "VoltageSource");
        if(voltage_index != -1) {
            throw AlreadyExistingVoltageSource(ID);
        }
        int node1_index = findNodeWithID(node1_ID);
        int node2_index = findNodeWithID(node2_ID);
        if(node1_index != -1) {
            if(node2_index != -1) {
                VoltageSource* temp = new VoltageSource(ID, this->nodes[node1_index], this->nodes[node2_index], voltage);
                this->elements.push_back(temp);
            }
            else {
                Node* temp = new Node(node2_ID);
                this->nodes.push_back(temp);
                node2_index = findNodeWithID(node2_ID);
                VoltageSource* temp1 = new VoltageSource(ID, this->nodes[node1_index], this->nodes[node2_index], voltage);
                this->elements.push_back(temp1);
            }
        }
        else {
            if(node2_index != -1) {
                Node* temp = new Node(node1_ID);
                this->nodes.push_back(temp);
                node1_index = findNodeWithID(node1_ID);
                VoltageSource* temp1 = new VoltageSource(ID, this->nodes[node1_index], this->nodes[node2_index], voltage);
                this->elements.push_back(temp1);
            }
            else {
                Node* temp = new Node(node1_ID);
                this->nodes.push_back(temp);
                Node* temp1 = new Node(node2_ID);
                this->nodes.push_back(temp1);
                node1_index = findNodeWithID(node1_ID);
                node2_index = findNodeWithID(node2_ID);
                VoltageSource* temp2 = new VoltageSource(ID, this->nodes[node1_index], this->nodes[node2_index], voltage);
                this->elements.push_back(temp2);
            }
        }
    }
    void addCurrentSource(string ID, string node1_ID, string node2_ID, string combined_current) {
        double current = parseSpiceNumber(combined_current);
        //cout << current << endl;
        if(current <= 0) {
            throw InvalidValue("Current");
        }
        if(!isCurrentSourceNameFromatValid(ID)) {
            throw InvalidElement(ID);
        }
        int current_index = doesElementWithThisNameAndTypeAlreadyExist(ID, "CurrentSource");
        if(current_index != -1) {
            throw AlreadyExistingCurrentSource(ID);
        }
        int node1_index = findNodeWithID(node1_ID);
        int node2_index = findNodeWithID(node2_ID);
        string temp_ID = "";
        for(int i = 13; i < ID.size(); i ++) {
            temp_ID += ID[i];
        }
        ID = temp_ID;
        if(node1_index != -1) {
            if(node2_index != -1) {
                CurrentSource* temp = new CurrentSource(ID, this->nodes[node1_index], this->nodes[node2_index], current);
                this->elements.push_back(temp);
            }
            else {
                Node* temp = new Node(node2_ID);
                this->nodes.push_back(temp);
                node2_index = findNodeWithID(node2_ID);
                CurrentSource* temp1 = new CurrentSource(ID, this->nodes[node1_index], this->nodes[node2_index], current);
                this->elements.push_back(temp1);
            }
        }
        else {
            if(node2_index != -1) {
                Node* temp = new Node(node1_ID);
                this->nodes.push_back(temp);
                node1_index = findNodeWithID(node1_ID);
                CurrentSource* temp1 = new CurrentSource(ID, this->nodes[node1_index], this->nodes[node2_index], current);
                this->elements.push_back(temp1);
            }
            else {
                Node* temp = new Node(node1_ID);
                this->nodes.push_back(temp);
                Node* temp1 = new Node(node2_ID);
                this->nodes.push_back(temp1);
                node1_index = findNodeWithID(node1_ID);
                node2_index = findNodeWithID(node2_ID);
                CurrentSource* temp2 = new CurrentSource(ID, this->nodes[node1_index], this->nodes[node2_index], current);
                this->elements.push_back(temp2);
            }
        }
    }
    void addSinusoiⅾaⅼVoltageSource(string ID, string node1_ID, string node2_ID, string combined_offset, string combined_amplitude, string combined_frequency) {
        double offset = parseSpiceNumber(combined_offset);
        double amplitude = parseSpiceNumber(combined_amplitude);
        double frequency = parseSpiceNumber(combined_frequency);
        if(offset <= 0 || amplitude <= 0) {
            throw InvalidValue("Voltage");
        }
        if(!isVoltageSourceNameFromatValid(ID)) {
            throw InvalidElement(ID);
        }
        int voltage_index = doesElementWithThisNameAndTypeAlreadyExist(ID, "SinusoiⅾaⅼVoltageSource");
        if(voltage_index != -1) {
            throw AlreadyExistingVoltageSource(ID);
        }
        int node1_index = findNodeWithID(node1_ID);
        int node2_index = findNodeWithID(node2_ID);
        if(node1_index != -1) {
            if(node2_index != -1) {
                SinusoiⅾaⅼVoltageSource* temp = new SinusoiⅾaⅼVoltageSource(ID, this->nodes[node1_index], this->nodes[node2_index], offset, amplitude, frequency);
                this->elements.push_back(temp);
            }
            else {
                Node* temp = new Node(node2_ID);
                this->nodes.push_back(temp);
                node2_index = findNodeWithID(node2_ID);
                SinusoiⅾaⅼVoltageSource* temp1 = new SinusoiⅾaⅼVoltageSource(ID, this->nodes[node1_index], this->nodes[node2_index], offset, amplitude, frequency);
                this->elements.push_back(temp1);
            }
        }
        else {
            if(node2_index != -1) {
                Node* temp = new Node(node1_ID);
                this->nodes.push_back(temp);
                node1_index = findNodeWithID(node1_ID);
                SinusoiⅾaⅼVoltageSource* temp1 = new SinusoiⅾaⅼVoltageSource(ID, this->nodes[node1_index], this->nodes[node2_index], offset, amplitude, frequency);
                this->elements.push_back(temp1);
            }
            else {
                Node* temp = new Node(node1_ID);
                this->nodes.push_back(temp);
                Node* temp1 = new Node(node2_ID);
                this->nodes.push_back(temp1);
                node1_index = findNodeWithID(node1_ID);
                node2_index = findNodeWithID(node2_ID);
                SinusoiⅾaⅼVoltageSource* temp2 = new SinusoiⅾaⅼVoltageSource(ID, this->nodes[node1_index], this->nodes[node2_index], offset, amplitude, frequency);
                this->elements.push_back(temp2);
            }
        }
    }
    void addVCVS(string ID, string node1_ID, string node2_ID, string controlling1_ID, string controlling2_ID, string combined_gain) {
        string temp_ID = "";
        for(int i = 13; i < ID.size(); i ++) {
            temp_ID += ID[i];
        }
        ID = temp_ID;
        double gain = parseSpiceNumber(combined_gain);
        //cout << voltage << endl;
        if(gain <= 0) {
            throw InvalidValue("Gain");
        }
        if(!isVoltageSourceNameFromatValid(ID)) {
            throw InvalidElement(ID);
        }
        int voltage_index = doesElementWithThisNameAndTypeAlreadyExist(ID, "VoltageControlledVoltageSource");
        if(voltage_index != -1) {
            throw AlreadyExistingVoltageSource(ID);
        }
        int node1_index = findNodeWithID(node1_ID);
        int node2_index = findNodeWithID(node2_ID);
        int node3_index = findNodeWithID(controlling1_ID);
        int node4_index = findNodeWithID(controlling2_ID);
        if(node3_index == -1 || node4_index == -1) {
            throw InvalidControlElement();
        }
        if(node1_index != -1) {
            if(node2_index != -1) {
                VCVS* temp = new VCVS(ID, this->nodes[node1_index], this->nodes[node2_index], this->nodes[node3_index], this->nodes[node4_index], gain);
                this->elements.push_back(temp);
            }
            else {
                Node* temp = new Node(node2_ID);
                this->nodes.push_back(temp);
                node2_index = findNodeWithID(node2_ID);
                VCVS* temp1 = new VCVS(ID, this->nodes[node1_index], this->nodes[node2_index], this->nodes[node3_index], this->nodes[node4_index], gain);
                this->elements.push_back(temp1);
            }
        }
        else {
            if(node2_index != -1) {
                Node* temp = new Node(node1_ID);
                this->nodes.push_back(temp);
                node1_index = findNodeWithID(node1_ID);
                VCVS* temp1 = new VCVS(ID, this->nodes[node1_index], this->nodes[node2_index], this->nodes[node3_index], this->nodes[node4_index], gain);
                this->elements.push_back(temp1);
            }
            else {
                Node* temp = new Node(node1_ID);
                this->nodes.push_back(temp);
                Node* temp1 = new Node(node2_ID);
                this->nodes.push_back(temp1);
                node1_index = findNodeWithID(node1_ID);
                node2_index = findNodeWithID(node2_ID);
                VCVS* temp2 = new VCVS(ID, this->nodes[node1_index], this->nodes[node2_index], this->nodes[node3_index], this->nodes[node4_index], gain);
                this->elements.push_back(temp2);
            }
        }
    }
    void showAllNodes() {
        if(this->nodes.empty()) {
            return;
        }
        cout << "Available nodes:" << endl;
        for(int i = 0; i < this->nodes.size(); i ++) {
            if(i == this->nodes.size()-1) {
                cout << this->nodes[i]->getID() << endl;
            }
            else {
                cout << this->nodes[i]->getID() << ", ";
            }
        }
    }
    void showAllComponentsOfCartainType(string type) {
        for(int i = 0; i < this->elements.size(); i ++) {
            if(this->elements[i]->getType() == type) {
                cout << this->elements[i]->getID() << " " << this->elements[i]->getValue() << " " << this->elements[i]->getStartNode()->getID() << " " << this->elements[i]->getEndNode()->getID() << endl;
            }
        }
    }
    void showAllComponents() {
        for(int i = 0; i < this->elements.size(); i ++) {
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
    void renameNode(string line, string old_name, string new_name) {
        regex pattern(R"(.rename node \w+ \w+)");
        if(!regex_match(line, pattern)) {
            throw InvalidRenameNodeFormat(old_name, new_name);
        }
        int old_node_index = findNodeWithID(old_name);
        if(old_node_index == -1) {
            throw NonExsistantNode(old_name);
        }
        int new_node_index = findNodeWithID(new_name);
        if(new_node_index != -1) {
            throw AlreadyExistingNode(new_name);
        }
        this->nodes[old_node_index]->setID(new_name);
        cout << "Node renamed from " << old_name << " to " << new_name << endl;
    }
    void checkForNoGND() {
        for(int i = 0; i < nodes.size(); i ++) {
            if(nodes[i]->isGround()) {
                return;
            }
        }
        throw NoGroundNode();
    }
    int getNodeIndex(string name) {
        for(int i = 0; i < this->nodes.size(); i ++) {
            if(this->nodes[i]->getID() == name) {
                return i;
            }
        }
        throw InvalidNodeName(name);
    };
    int getElementIndex(string name) {
        for(int i = 0; i < this->elements.size(); i ++) {
            if(this->elements[i]->getID() == name) {
                return i;
            }
        }
        throw InvalidComponentName(name);
    };
    void GeneralInvalidities() {
        throw InvalidAddResistorFormat();
    }
    void isThereEnoughGND() {
        int counter = 0;
        for(int i = 0; i < this->nodes.size(); i ++) {
            if(this->nodes[i]->isGround()) {
                counter ++;
            }
        }
        if(counter > 1 || counter == 0) {
            throw notEnoughGND();
        }
    }
};