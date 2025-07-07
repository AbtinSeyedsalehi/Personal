#include "circuitcontroller.h"
#include "schematiccontroller.h"
#include "analyticscontroller.h"
#include <Eigen/Dense>
#include <sstream>
static bool isNumber(const std::string &s) {
    static const std::regex numberRx(R"(^[+\-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+\-]?\d+)?$)");
    return std::regex_match(s, numberRx);
}
using namespace std;
int main() {
    regex pattern00(R"(NewFile \S+)");
    regex pattern01(R"(add r(\w+) (\w+) (\w+) ([\d\.]+[a-zA-Z]*))", regex_constants::icase);//adding resistor
    regex pattern02(R"(delete r(\w+))", regex_constants::icase);//deleting resistor
    regex pattern03(R"(^add c(\w+) (\w+) (\w+) ([\d\.]+[a-zA-Z]*)$)", regex_constants::icase);//adding capacitor
    regex pattern04(R"(delete c(\w+))", regex_constants::icase);//deleting capacitor
    regex pattern05(R"(add l(\w+) (\w+) (\w+) ([\d\.]+[a-zA-Z]*))", regex_constants::icase);//adding inductor
    regex pattern06(R"(delete l(\w+))", regex_constants::icase);//deleting inductor
    regex pattern07(R"(add d(\w+) (\w+) (\w+) (\S*))", regex_constants::icase);//adding inductor
    regex pattern08(R"(delete d(\w+))", regex_constants::icase);//deleting inductor
    regex pattern09(R"(add gnd \w+)", regex_constants::icase);//adding ground
    regex pattern10(R"(delete gnd \w+)", regex_constants::icase);//adding ground
    regex pattern11(R"(\.nodes)");//showing all nodes
    regex pattern12(R"(\.list \w+)");//showing all components of certain type
    regex pattern13(R"(\.list)");//showing all components
    regex pattern14(R"(\.?rename\s+node\s+(\S+)\s+(\S+))", std::regex_constants::icase);//renaming nodes
    regex pattern15(R"(add VoltageSource(\w+) (\w+) (\w+) (\S*))", regex_constants::icase);//adding voltage source
    regex pattern16(R"(add CurrentSource(\w+) (\w+) (\w+) (\S*))", regex_constants::icase);//adding current source
    regex pattern17(R"(add v(\w+) (\w+) (\w+) SIN\(\S+ \S+ \S+\))", regex_constants::icase);//adding sinusoidal voltage source
    regex pattern18(R"(add E(\w+) (\w+) (\w+) (\w+) (\w+) (\S*))");//adding VCVS
    regex pattern19(R"(add G(\w+) (\w+) (\w+) (\w+) (\w+) (\S*))");//adding VCCS
    regex pattern20(R"(add H(\w+) (\w+) (\w+) (\w+) (\w+) (\S*))");//adding CCVS
    regex pattern21(R"(analyze)");//analyzing
    regex patternPrintTran(R"(^\.print\s+TRAN\s+(\S+)\s+(\S+)(?:\s+(\S+))?(?:\s+(\S+))?\s+((?:[VI]\([^)]+\)\s*)+)$)", regex_constants::icase);

    //regex pattern21(R"(\.print TRAN \S+ \S+ \[\S+\] \[\S+\] \w+)");
    vector<CircuitController*> Circiuts;
    SchematicController SC;
    string main_file_name;
    cout << "Welcome to (I didn't study physics over this)" << endl;
    while(true) {
        cout << ">";
        string line;
        getline(cin, line);
        if(line == "exit") {break;}
        else if(line == "files") {
            cout << "entered file management" << endl;
            while(line != "return") {
                getline(cin, line);
                if(line == "-show existing schematics") {
                    bool found = false;
                    bool emerg_ex = false;
                    main_file_name.clear();
                    SC.showExistingSchematics(found);
                    while(!found && !emerg_ex) {
                        try {
                            getline(cin, line);
                            SC.isEneteredOptionValid(line, main_file_name, found, emerg_ex);
                            SC.showThisSchematic(main_file_name, Circiuts);
                        } catch (SpiceExceptions &e) {
                            cout << e.what() << endl;
                        }
                    }
                }
                else if(regex_search(line, pattern00)) {
                    vector<string> command = SC.Extractor(line);
                    ofstream outfile(command[1]);
                    cout << "schematic was made successfully" << endl;
                    //address basis: /Users/abtin/Desktop/SpiceFiles/<file_name>.txt
                    main_file_name = SC.getFileNameFromPath(command[1]);
                    SC.addToFileNames(SC.getFileNameFromPath(command[1]));
                    CircuitController* temp = new CircuitController(main_file_name);
                    Circiuts.push_back(temp);
                    break;
                }
            }
            cout << main_file_name << endl;
        }
        else if(line == "edit") {
            cout << "entered editing schematics successfully" << endl;
            bool found = false;
            bool emerg_ex = false;
            main_file_name.clear();
            SC.showExistingSchematics(found);
            while(!found && !emerg_ex) {
                try {
                    getline(cin, line);
                    SC.isEneteredOptionValid(line, main_file_name, found, emerg_ex);
                    cout << "schematic " << main_file_name << " was opened successfully" << endl;
                    cout << "you can now edit this schematic with validated commands" << endl;
                } catch (SpiceExceptions &e) {
                    cout << e.what() << endl;
                }
            }
            getline(cin, line);
            CircuitController* CC = SC.FindCircuit(main_file_name, Circiuts);
            while(line != "return") {
                if (regex_search(line, pattern01) && found) {
                    try {
                        vector<string> command = CC->Extractor(line);
                        CC->addResistor(command[1], command[2], command[3], command[4]);
                    } catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                } else if (regex_search(line, pattern02) && found) {
                    try {
                        vector<string> command = CC->Extractor(line);
                        CC->deleteResistor(command[1]);
                    } catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                } else if (regex_search(line, pattern16) && found) {
                    try {
                        vector<string> command = CC->Extractor(line);
                        CC->addCurrentSource(command[1], command[2], command[3], command[4]);
                    } catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                } else if (regex_search(line, pattern03) && found) {
                    try {
                        vector<string> command = CC->Extractor(line);
                        CC->addCapacitor(command[1], command[2], command[3], command[4]);
                    } catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                } else if (regex_search(line, pattern04) && found) {
                    try {
                        vector<string> command = CC->Extractor(line);
                        CC->deleteCapacitor(command[1]);
                    } catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                } else if (regex_search(line, pattern05) && found) {
                    try {
                        vector<string> command = CC->Extractor(line);
                        CC->addSelf(command[1], command[2], command[3], command[4]);
                    } catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                } else if (regex_search(line, pattern06) && found) {
                    try {
                        vector<string> command = CC->Extractor(line);
                        CC->deleteSelf(command[1]);
                    } catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                } else if (regex_search(line, pattern07) && found) {
                    try {
                        vector<string> command = CC->Extractor(line);
                        CC->addDiode(command[1], command[2], command[3], command[4]);
                    } catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                } else if (regex_search(line, pattern08) && found) {
                    try {
                        vector<string> command = CC->Extractor(line);
                        CC->deleteDiode(command[1]);
                    } catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                } else if (regex_search(line, pattern09) && found) {
                    try {
                        vector<string> command = CC->Extractor(line);
                        CC->addGround(command[1], command[2]);
                    } catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                } else if (regex_search(line, pattern10) && found) {
                    try {
                        vector<string> command = CC->Extractor(line);
                        CC->deleteGround(command[2]);
                    } catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                } else if (regex_search(line, pattern11) && found) {
                    try {
                        //vector<string> command = PC.Extractor(line);
                        CC->showAllNodes();
                    } catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                } else if (regex_search(line, pattern12) && found) {
                    try {
                        vector<string> command = CC->Extractor(line);
                        CC->showAllComponentsOfCartainType(command[1]);
                    } catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                } else if (regex_search(line, pattern13) && found) {
                    try {
                        vector<string> command = CC->Extractor(line);
                        CC->showAllComponents();
                    } catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                } else if (regex_search(line, pattern14) && found) {
                    try {
                        vector<string> command = CC->Extractor(line);
                        CC->renameNode(line, command[2], command[3]);
                    } catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                } else if (regex_search(line, pattern15) && found) {
                    try {
                        vector<string> command = CC->Extractor(line);
                        CC->addVoltageSource(command[1], command[2], command[3], command[4]);
                    } catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                } else if (regex_search(line, pattern17) && found) {
                    try {
                        vector<string> command = CC->Extractor(line);
                        string offset = "";
                        string amplitude = "";
                        string frequency = "";
                        for(int i = 4; i < command[4].size(); i ++) {
                            offset += command[4][i];
                        }
                        for(int i = 0; i < command[5].size(); i ++) {
                            amplitude += command[5][i];
                        }
                        for(int i = 0; i < command[6].size()-1; i ++) {
                            frequency += command[6][i];
                        }
                        CC->addSinusoiⅾaⅼVoltageSource(command[1], command[2], command[3], offset, amplitude, frequency);
                    } catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                } else if (regex_search(line, pattern18) && found) {
                    try {
                        vector<string> command = CC->Extractor(line);
                        CC->addVCVS(command[1], command[2], command[3], command[4], command[5], command[6]);
                    } catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                }
                else {
                    try {
                        CC->GeneralInvalidities();
                    }
                    catch (SpiceExceptions &e) {
                        cout << e.what() << endl;
                    }
                }
                getline(cin, line);
            }
            Circiuts.push_back(CC);
        }
        else if(line == "analytics") {
            cout << "entered analyzing schematics successfully" << endl;
            bool found = false;
            bool emerg_ex = false;
            main_file_name.clear();
            SC.showExistingSchematics(found);
            while(!found && !emerg_ex) {
                try {
                    getline(cin, line);
                    SC.isEneteredOptionValid(line, main_file_name, found, emerg_ex);
                    cout << "schematic " << main_file_name << " was opened successfully" << endl;
                    cout << "you can now edit this schematic with validated commands" << endl;
                } catch (SpiceExceptions &e) {
                    cout << e.what() << endl;
                }
            }
            getline(cin, line);
            CircuitController* CC = SC.FindCircuit(main_file_name, Circiuts);
            while(line != "return") {
                if (line.rfind(".print TRAN", 0) == 0 && found) {
                    try {
                        CC->isThereEnoughGND();
                        vector<string> tokens = CC->Extractor(line);
                        if (tokens.size() < 4) {
                            goto NEXT_ANALYZE_LINE;
                        }
                        int idx = 2;
                        if (!isNumber(tokens[idx]) || !isNumber(tokens[idx+1])) {
                            goto NEXT_ANALYZE_LINE;
                        }
                        double tstep = stod(tokens[idx++]);
                        double tstop = stod(tokens[idx++]);
                        double tstart = 0.0, tmaxstep = tstep;
                        if (idx < (int)tokens.size() && isNumber(tokens[idx])) {
                            tstart = stod(tokens[idx++]);
                        }
                        if (idx < (int)tokens.size() && isNumber(tokens[idx])) {
                            tmaxstep = stod(tokens[idx++]);
                        }
                        vector<string> signalList;
                        for (; idx < (int)tokens.size(); ++idx) {
                            signalList.push_back(tokens[idx]);
                        }
                        vector<double> timeSteps;
                        for (double t = tstart; t <= tstop; t += tstep)
                            timeSteps.push_back(t);
                        SimulationResult result = simulateTransient(
                                CC->getElements(), CC->getNodes(), timeSteps, tstep
                        );
                        for (int ti = 0; ti < (int)timeSteps.size(); ++ti) {
                            cout << "t=" << timeSteps[ti] << "s: ";
                            for (auto &sig : signalList) {
                                if ((sig[0]=='V' || sig[0]=='v') && sig.find('(')!=string::npos) {
                                    string name = sig.substr(sig.find('(')+1,
                                                             sig.find(')')-sig.find('(')-1);
                                    int ni = CC->getNodeIndex(name);
                                    cout << "V("<<name<<")="<< result.nodeVoltages(ti,ni) <<" ";
                                }
                                else if ((sig[0]=='I' || sig[0]=='i') && sig.find('(')!=string::npos) {
                                    string name = sig.substr(sig.find('(')+1,
                                                             sig.find(')')-sig.find('(')-1);
                                    int ei = CC->getElementIndex(name);
                                    cout << "I("<<name<<")="<< result.elementCurrents(ti,ei) <<" ";
                                }
                            }
                            cout << "\n";
                        }
                    }
                    catch (std::exception &e) {
                        cout << e.what() << "\n";
                    }
                    NEXT_ANALYZE_LINE:
                    getline(cin, line);
                    continue;
                }
                getline(cin, line);
            }
        }
    }
    return 0;
}