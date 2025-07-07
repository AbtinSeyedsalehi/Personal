#pragma once
#include <iostream>
#include <vector>
#include <regex>
#include <iomanip>
#include <map>
using namespace std;
class Node {
protected:
    string ID;
    bool is_GND = false;
    double voltage;
public:
    Node(string ID) : ID(ID) {}
    string getID() {return this->ID;}
    void makeGround() {this->is_GND = true;}
    void RemoveAsGround() {this->is_GND = false;}
    bool isGround() {return this->is_GND;}
    void setID(string new_ID) {this->ID = new_ID;}
};
class Element {
protected:
    string ID;
    Node* node1;
    Node* node2;
public:
    Element(string ID, Node* node1, Node* node2) : ID(ID), node1(node1), node2(node2) {}
    string getID() {return this->ID;}
    Node* getStartNode() {return this->node1;}
    Node* getEndNode() {return this->node2;}
    virtual string getType() = 0;
    virtual string getType1() = 0;
    virtual double getValue() = 0;
    virtual double getValue1() = 0;
    virtual double getValue2() = 0;
    virtual Node* getControllingNode1() = 0;
    virtual Node* getControllingNode2() = 0;
    virtual Element* getControllingElement() = 0;
};
class Resistor : public Element {
protected:
    string type = "Resistor";
    double resistance;
public:
    Resistor(string ID, Node* node1, Node* node2, double Resistance) : Element(ID, node1, node2), resistance(Resistance) {}
    string getType() override {return this->type;}
    double getValue() override {return this->resistance;}
    string getType1() override {return "";}
    double getValue1() override {return 0;}
    double getValue2() override {return 0;}
    Node* getControllingNode1() override {return nullptr;}
    Node* getControllingNode2() override {return nullptr;}
    Element* getControllingElement() override {return nullptr;}
};
class Capacitor : public Element {
protected:
    string type = "Capacitor";
    double capacitance;
public:
    Capacitor(string ID, Node* node1, Node* node2, double Capacitance) : Element(ID, node1, node2), capacitance(Capacitance) {}
    string getType() override {return this->type;}
    double getValue() override {return this->capacitance;}
    string getType1() override {return "";}
    double getValue1() override {return 0;}
    double getValue2() override {return 0;}
    Node* getControllingNode1() override {return nullptr;}
    Node* getControllingNode2() override {return nullptr;}
    Element* getControllingElement() override {return nullptr;}

};
class Self : public Element {
protected:
    string type = "Self";
    double inductance;
public:
    Self(string ID, Node* node1, Node* node2, double Inductance) : Element(ID, node1, node2), inductance(Inductance) {}
    string getType() override {return this->type;}
    double getValue() override {return this->inductance;}
    string getType1() override {return "";}
    double getValue1() override {return 0;}
    double getValue2() override {return 0;}
    Node* getControllingNode1() override {return nullptr;}
    Node* getControllingNode2() override {return nullptr;}
    Element* getControllingElement() override {return nullptr;}
};
class Diode : public Element {
protected:
    string type = "Diode";
    string model;
public:
    Diode(string ID, Node* node1, Node* node2, string model) : Element(ID, node1, node2), model(model) {}
    string getModel() {return this->model;}
    string getType() override {return this->type;}
    double getValue() override {return 0;}
    string getType1() override {return this->model;}
    double getValue1() override {return 0;}
    double getValue2() override {return 0;}
    Node* getControllingNode1() override {return nullptr;}
    Node* getControllingNode2() override {return nullptr;}
    Element* getControllingElement() override {return nullptr;}
};
class VoltageSource : public Element {
protected:
    string type = "VoltageSource";
    double voltage;
public:
    VoltageSource(string ID, Node* node1, Node* node2, double Voltage) : Element(ID, node1, node2), voltage(Voltage) {}
    string getType() override {return this->type;}
    double getValue() override {return this->voltage;}
    string getType1() override {return "";}
    double getValue1() override {return 0;}
    double getValue2() override {return 0;}
    Node* getControllingNode1() override {return nullptr;}
    Node* getControllingNode2() override {return nullptr;}
    Element* getControllingElement() override {return nullptr;}
};
class SinusoiⅾaⅼVoltageSource : public Element {
protected:
    //sinusoidal voltage based on Asin(2pift)
    string type = "SinusoiⅾaⅼVoltageSource";
    double offset;
    double amplitude;
    double frequency;
public:
    SinusoiⅾaⅼVoltageSource(string ID, Node* node1, Node* node2, double offset, double amplitude, double frequency) : Element(ID, node1, node2), offset(offset), amplitude(amplitude), frequency(frequency) {}
    string getType() override {return this->type;}
    double getValue() override {return this->offset;}
    string getType1() override {return "";}
    double getValue1() override {return this->amplitude;}
    double getValue2() override {return this->frequency;}
    Node* getControllingNode1() override {return nullptr;}
    Node* getControllingNode2() override {return nullptr;}
    Element* getControllingElement() override {return nullptr;}
};
/*class PulseVoltageSource : public Element {
protected:
    string type = "PulseVoltageSource";
    double value;
public:
    PulseVoltageSource(string ID, Node* node1, Node* node2, double value) : Element(ID, node1, node2), value(value) {}
    string getType() override {return this->type;}
    double getValue() override {return this->offset;}
};*/
class CurrentSource : public Element {
protected:
    string type = "CurrentSource";
    double current;
public:
    CurrentSource(string ID, Node* node1, Node* node2, double Current) : Element(ID, node1, node2), current(Current) {}
    string getType() override {return this->type;}
    double getValue() override {return this->current;}
    string getType1() override {return "";}
    double getValue1() override {return 0;}
    double getValue2() override {return 0;}
    Node* getControllingNode1() override {return nullptr;}
    Node* getControllingNode2() override {return nullptr;}
    Element* getControllingElement() override {return nullptr;}
};
class VCVS : public Element {
protected:
    string type = "VoltageControlledVoltageSource";
    Node* node_controlling_1;
    Node* node_controlling_2;
    double gain;
public:
    VCVS(string ID, Node* node1, Node* node2, Node* node_controlling_1, Node* node_controlling_2, double gain) : Element(ID, node1, node2), node_controlling_1(node_controlling_1), node_controlling_2(node_controlling_2), gain(gain) {}
    string getType() override {return this->type;}
    double getValue() override {return this->gain;}
    string getType1() override {return "";}
    double getValue1() override {return 0;}
    double getValue2() override {return 0;}
    Node* getControllingNode1() override {return this->node_controlling_1;}
    Node* getControllingNode2() override {return this->node_controlling_2;}
    Element* getControllingElement() override {return nullptr;}
};
class VCCS : public Element {
protected:
    string type = "VoltageControlledCurrentSource";
    Node* node_controlling_1;
    Node* node_controlling_2;
    double gain;
public:
    VCCS(string ID, Node* node1, Node* node2, Node* node_controlling_1, Node* node_controlling_2, double gain) : Element(ID, node1, node2), node_controlling_1(node_controlling_1), node_controlling_2(node_controlling_2), gain(gain) {}
    string getType() override {return this->type;}
    double getValue() override {return this->gain;}
    string getType1() override {return "";}
    double getValue1() override {return 0;}
    double getValue2() override {return 0;}
    Node* getControllingNode1() override {return this->node_controlling_1;}
    Node* getControllingNode2() override {return this->node_controlling_2;}
    Element* getControllingElement() override {return nullptr;}
};
class CCVS : public Element {
protected:
    string type = "VoltageControlledVoltageSource";
    Element* V_controlling;
    double gain;
public:
    CCVS(string ID, Node* node1, Node* node2, Element* V_controlling, double gain) : Element(ID, node1, node2), V_controlling(V_controlling), gain(gain) {}
    string getType() override {return this->type;}
    double getValue() override {return this->gain;}
    string getType1() override {return "";}
    double getValue1() override {return 0;}
    double getValue2() override {return 0;}
    Node* getControllingNode1() override {return nullptr;}
    Node* getControllingNode2() override {return nullptr;}
    Element* getControllingElement() override {return this->V_controlling;}
};
class CCCS : public Element {
protected:
    string type = "CurrentControlledCurrentSource";
    Element* V_controlling;
    double gain;
public:
    CCCS(string ID, Node* node1, Node* node2, Element* V_controlling, double gain) : Element(ID, node1, node2), V_controlling(V_controlling), gain(gain) {}
    string getType() override {return this->type;}
    double getValue() override {return this->gain;}
    string getType1() override {return "";}
    double getValue1() override {return 0;}
    double getValue2() override {return 0;}
    Node* getControllingNode1() override {return nullptr;}
    Node* getControllingNode2() override {return nullptr;}
    Element* getControllingElement() override {return this->V_controlling;}
};