#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include "Graph.h"

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Graph g;
    // Load dataset (autograder expects data/ placed at repo root)
    g.loadEdgesCSV("data/edges.csv");
    g.loadClassesCSV("data/classes.csv");

    string line;
    int numCommands;
    if (!(cin >> numCommands)) return 0;
    string dummy;
    getline(cin, dummy); // consume newline after number

    for (int i = 0; i < numCommands; ++i) {
        if (!getline(cin, line)) break;
        if (line.empty()) { cout << "unsuccessful\n"; continue; }
        stringstream ss(line);
        string cmd;
        ss >> cmd;

        if (cmd == "insert") {
            string name;
            int ufid,residence, n;
            ss >> ws;
            if (ss.peek() == '"') {       // handle quoted name
                ss.get();                 // consume opening "
                getline(ss, name, '"');
            } else {
                ss >> name;
            }
            if (!(ss >> ufid >> residence >> n)) {
                cout << "unsuccessful\n"; continue;
            }

            vector<string> classes;
            string cls;
            for (int j = 0; j < n; ++j) {
                if (!(ss >> cls)) { classes.clear(); break; }
                classes.push_back(cls);
            }

            if (classes.size() != (size_t)n) {
                cout << "unsuccessful\n"; continue;
            }

            bool ok = g.addStudent(name, ufid,classes, residence);
            cout << (ok ? "successful" : "unsuccessful") << "\n";
        }
        else if (cmd == "remove") {
            int ufid; ss >> ufid;
            bool ok = g.removeStudent(ufid);
            cout << (ok ? "successful" : "unsuccessful") << "\n";
        }
        else if (cmd == "dropClass") {
            int ufid; string classCode;
            ss >> ufid >> classCode;
            bool ok = g.dropClass(ufid, classCode);
            cout << (ok ? "successful" : "unsuccessful") << "\n";
        }
        else if (cmd == "replaceClass") {
            int ufid; string oldC, newC;
            ss >> ufid >> oldC >> newC;
            bool ok = g.replaceClass(ufid, oldC, newC);
            cout << (ok ? "successful" : "unsuccessful") << "\n";
        }
        else if (cmd == "removeClass") {
            string classCode; ss >> classCode;
            int count = g.removeClass(classCode);
            cout << count << "\n";
        }
        else if (cmd == "toggleEdgesClosure") {
            int N; ss >> N;
            vector<pair<int,int>> pairs;
            for (int k = 0; k < N; ++k) {
                int a, b;
                ss >> a >> b;
                pairs.emplace_back(a,b);
            }
            bool ok = g.toggleEdgesClosure(pairs);
            cout << (ok ? "successful" : "unsuccessful") << "\n";
        }
        else if (cmd == "checkEdgeStatus") {
            int a, b; ss >> a >> b;
            cout << g.checkEdgeStatus(a,b) << "\n";
        }
        else if (cmd == "isConnected") {
            int a, b; ss >> a >> b;
            cout << (g.isConnected(a,b) ? "successful" : "unsuccessful") << "\n";
        }
        else if (cmd == "printShortestEdges") {
            int ufid; ss >> ufid;
            int residence = g.getStudentResidence(ufid);
            auto classes = g.getStudentClasses(ufid);
            sort(classes.begin(), classes.end(), [](const pair<string,int>& A, const pair<string,int>& B){
                return A.first < B.first;
            });
            auto shortest = g.shortestTimesFromResidence(residence, classes);
            cout << "Name: " << g.getStudentName(ufid) << "\n";
            for (auto &p : classes) {
                auto it = shortest.find(p.first);
                int t = (it == shortest.end()) ? -1 : it->second;
                cout << p.first << " | Total Time: " << t << "\n";
            }
        }
        else if (cmd == "printStudentZone") {
            int ufid; ss >> ufid;
            int residence = g.getStudentResidence(ufid);
            vector<int> locs;
            auto classes = g.getStudentClasses(ufid);
            for (auto &p : classes) if (p.second != -1) locs.push_back(p.second);
            int cost = g.computeStudentZoneCost(residence, locs);
            cout << "Student Zone Cost For " << g.getStudentName(ufid) << ": " << cost << "\n";
        }
        else {
            cout << "unsuccessful\n";
        }
    }
    return 0;
}
