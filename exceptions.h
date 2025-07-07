#pragma once
#include <iostream>
#include <vector>
#include <regex>
#include <iomanip>
#include <map>
using namespace std;
class SpiceExceptions : public exception {
public:
    explicit SpiceExceptions(string msg) : msg_(move(msg)) {}
    const char* what() const noexcept override { return msg_.c_str(); }
private:
    string msg_;
};
class InvalidValue : public SpiceExceptions {
public:
    InvalidValue(string type) : SpiceExceptions("Error: "+type+" cannot be zero or negative") {}
};
class InvalidAddResistorFormat : public SpiceExceptions {
public:
    InvalidAddResistorFormat() : SpiceExceptions("Error: Syntax error") {}
};
class InvalidElement : public SpiceExceptions {
public:
    InvalidElement(string name) : SpiceExceptions("Error: Element " + name + " not found in library") {}
};
class RemovingNonExistantResistor : public SpiceExceptions {
public:
    RemovingNonExistantResistor() : SpiceExceptions("Error: Cannot delete resistor; component not found") {}
};
class RemovingNonExistantCapacitor : public SpiceExceptions {
public:
    RemovingNonExistantCapacitor() : SpiceExceptions("Error: Cannot delete capacitor; component not found") {}
};
class RemovingNonExistantSelf : public SpiceExceptions {
public:
    RemovingNonExistantSelf() : SpiceExceptions("Error: Cannot delete inductor; component not found") {}
};
class AlreadyExistingResistor : public SpiceExceptions {
public:
    AlreadyExistingResistor(string name) : SpiceExceptions("Error: Resistor " + name + " already exists in the circuit") {}
};
class AlreadyExistingCapacitor : public SpiceExceptions {
public:
    AlreadyExistingCapacitor(string name) : SpiceExceptions("Error: Capacitor " + name + " already exists in the circuit") {}
};
class AlreadyExistingSelf : public SpiceExceptions {
public:
    AlreadyExistingSelf(string name) : SpiceExceptions("Error: Inductor " + name + " already exists in the circuit") {}
};
class AlreadyExistingDiode : public SpiceExceptions {
public:
    AlreadyExistingDiode(string name) : SpiceExceptions("Error: Diode " + name + " already exists in the circuit") {}
};
class AlreadyExistingNode : public SpiceExceptions {
public:
    AlreadyExistingNode(string name) : SpiceExceptions("Error: Node " + name + " already exists") {}
};
class AlreadyExistingVoltageSource : public SpiceExceptions {
public:
    AlreadyExistingVoltageSource(string name) : SpiceExceptions("Error: Voltage Source " + name + " already exists in the circuit") {}
};
class AlreadyExistingCurrentSource : public SpiceExceptions {
public:
    AlreadyExistingCurrentSource(string name) : SpiceExceptions("Error: Current Source " + name + " already exists in the circuit") {}
};
class InvalidDiodeModel : public SpiceExceptions {
public:
    InvalidDiodeModel(string model) : SpiceExceptions("Error: Model " + model + " not found in library") {}
};
class NonExsistantGNDNode : public SpiceExceptions {
public:
    NonExsistantGNDNode() : SpiceExceptions("Error: Node does not exist") {}
};
class NonExsistantNode : public SpiceExceptions {
public:
    NonExsistantNode(string name) : SpiceExceptions("Error: Node " + name + " does not exist in the circuit") {}
};
class InvalidRenameNodeFormat : public SpiceExceptions {
public:
    InvalidRenameNodeFormat(string old_name, string new_name) : SpiceExceptions("ERROR: Invalid syntax - correct format:\n"
                                                                                ".rename node " + old_name + " " + new_name) {}
};
class NoGroundNode : public SpiceExceptions {
public:
    NoGroundNode() : SpiceExceptions("Error: No ground node detected in the circuit.") {}
};
class InappropriateInput : public SpiceExceptions {
public:
    InappropriateInput() : SpiceExceptions("-Error : Inappropriate input") {}
};
class InvalidControlElement : public SpiceExceptions {
public:
    InvalidControlElement() : SpiceExceptions("Error: Dependent source has an undefined control element.") {}
};
class InvalidNodeName : public SpiceExceptions {
public:
    InvalidNodeName(string name) : SpiceExceptions("Node " + name + " not found in circuit") {}
    class InvalidComponentName : public SpiceExceptions {
    public:
        InvalidComponentName(string name) : SpiceExceptions("Component " + name + " not found in circuit") {}
    };
    class notEnoughGND : public SpiceExceptions {
    public:
        notEnoughGND() : SpiceExceptions("Error: No ground node detected in the circuit.") {}
    };