#include <iostream>
#include <Eigen/Dense>
#include "circuitcontroller.h"
#include "schematiccontroller.h"
#include "exceptions.h"
struct SimulationResult {
    Eigen::MatrixXd nodeVoltages;
    Eigen::MatrixXd elementCurrents;
};
SimulationResult simulateTransient(const vector<Element*>& elements, const vector<Node*>& nodes, const vector<double>& timeSteps, double h) {
    map<Node*, int> nodeIndex;
    int idx = 0;
    for(Node* n : nodes) {
        if(!n->isGround()) {
            nodeIndex[n] = idx++;
        }
    }
    int N = idx;
    vector<Element*> extraSources;
    for(Element* e : elements) {
        string t = e->getType();
        if(t == "VoltageSource" || t == "SinusoidalVoltageSource" || t == "Inductor") {
            extraSources.push_back(e);
        }
    }
    int M = extraSources.size();
    int dim = N + M;
    int T = (int)timeSteps.size();
    map<Element*, double> capPrevVolt;
    map<Element*, double> indPrevCurr;
    SimulationResult res;
    res.nodeVoltages = Eigen::MatrixXd::Zero(T, N);
    res.elementCurrents = Eigen::MatrixXd::Zero(T, elements.size());
    for(int t = 0; t < T; ++t) {
        Eigen::MatrixXd A = Eigen::MatrixXd::Zero(dim, dim);
        Eigen::VectorXd b = Eigen::VectorXd::Zero(dim);
        int extraIdx = 0;
        for(size_t i = 0; i < elements.size(); ++i) {
            Element* e = elements[i];
            Node* n1 = e->getStartNode();
            Node* n2 = e->getEndNode();
            int i1 = nodeIndex.count(n1) ? nodeIndex[n1] : -1;
            int i2 = nodeIndex.count(n2) ? nodeIndex[n2] : -1;
            string type = e->getType();
            if(type == "Resistor") {
                double G = 1.0 / e->getValue();
                if(i1 >= 0) A(i1,i1) += G;
                if(i2 >= 0) A(i2,i2) += G;
                if(i1 >= 0 && i2 >= 0) {
                    A(i1,i2) -= G;
                    A(i2,i1) -= G;
                }
            }
            else if(type == "CurrentSource") {
                double I = e->getValue();
                if(i1 >= 0) b(i1) -= I;
                if(i2 >= 0) b(i2) += I;
            }
            else if(type == "Capacitor") {
                double C = e->getValue();
                double Gc = C / h;
                if(i1 >= 0) A(i1,i1) += Gc;
                if(i2 >= 0) A(i2,i2) += Gc;
                if(i1 >= 0 && i2 >= 0) {
                    A(i1,i2) -= Gc;
                    A(i2,i1) -= Gc;
                }
                double Vprev = capPrevVolt[e];
                if(i1 >= 0) b(i1) += Gc * Vprev;
                if(i2 >= 0) b(i2) -= Gc * Vprev;
            }
            else if(type == "Inductor") {
                int p = N + extraIdx;
                if(i1 >= 0) A(i1,p) += 1;
                if(i2 >= 0) A(i2,p) -= 1;
                if(i1 >= 0) A(p,i1) += 1;
                if(i2 >= 0) A(p,i2) -= 1;
                double L = e->getValue();
                double Rl = h / L;
                A(p,p) -= Rl;
                double Iprev = indPrevCurr[e];
                b(p) -= Iprev * Rl;
                extraIdx++;
            }
            else if(type == "VoltageSource" || type == "SinusoidalVoltageSource") {
                int p = N + extraIdx;
                if(i1 >= 0) A(i1,p) += 1;
                if(i2 >= 0) A(i2,p) -= 1;
                if(i1 >= 0) A(p,i1) += 1;
                if(i2 >= 0) A(p,i2) -= 1;
                double Vsrc = e->getValue();
                if(type == "SinusoidalVoltageSource") {
                    double offset = e->getValue();
                    double amp = e->getValue1();
                    double freq = e->getValue2();
                    Vsrc = offset + amp * sin(2 * M_PI * freq * timeSteps[t]);
                }
                b(p) = Vsrc;
                extraIdx++;
            }
        }
        Eigen::VectorXd x = A.fullPivLu().solve(b);
        for(auto& p : nodeIndex) {
            res.nodeVoltages(t, p.second) = x(p.second);
        }
        extraIdx = 0;
        for(size_t i = 0; i < elements.size(); ++i) {
            Element* e = elements[i];
            Node* n1 = e->getStartNode();
            Node* n2 = e->getEndNode();
            int i1 = nodeIndex.count(n1) ? nodeIndex[n1] : -1;
            int i2 = nodeIndex.count(n2) ? nodeIndex[n2] : -1;
            string type = e->getType();
            double I = 0;
            if(type == "Resistor") {
                double G = 1.0 / e->getValue();
                double v1 = (i1 >= 0 ? x(i1) : 0);
                double v2 = (i2 >= 0 ? x(i2) : 0);
                I = G * (v1 - v2);
            }
            else if(type == "CurrentSource") {
                I = e->getValue();
            }
            else if(type == "Capacitor") {
                double C = e->getValue();
                double v1 = (i1 >= 0 ? x(i1) : 0);
                double v2 = (i2 >= 0 ? x(i2) : 0);
                I = C * ((v1 - v2) - capPrevVolt[e]) / h;
                capPrevVolt[e] = v1 - v2;
            }
            else if(type == "Inductor") {
                int p = N + extraIdx;
                I = x(p);
                indPrevCurr[e] = I;
                extraIdx++;
            }
            else if(type == "VoltageSource" || type == "SinusoidalVoltageSource") {
                int p = N + extraIdx;
                I = x(p);
                extraIdx++;
            }
            res.elementCurrents(t, i) = I;
        }
    }
    return res;
}
