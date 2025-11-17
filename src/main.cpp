#include <iostream>
#include <sstream>
#include <vector>
#include "Graph.h"

using namespace std;

int main() {
    Graph g;

    // Load CSV data
    if (!g.loadEdgesCSV("../data/edges.csv") || !g.loadClassesCSV("../data/classes.csv")) {
        cout << "Failed to load CSV files." << endl;
        return 1;
    }

    string line;
    int numCommands;
    cin >> numCommands;
    cin.ignore(); // consume newline after number

    for (int i = 0; i < numCommands; ++i) {
        getline(cin, line);
        stringstream ss(line);
        string command;
        ss >> command;

        if (command == "insert") {
            string name;
            int ufid, age, residence, n;
            ss >> ws; // skip whitespace
            getline(ss, name, '"'); // skip initial quote
            getline(ss, name, '"'); // read name
            ss >> ufid >> age >> residence >> n;
            
            vector<string> classes(n);
            for (int j = 0; j < n; ++j) ss >> classes[j];
            bool success = g.addStudent(name, ufid, age, classes, residence);

            cout << (success ? "successful" : "unsuccessful") << endl;
        }
        else if (command == "remove") {
            int ufid;
            ss >> ufid;
            bool success = g.removeStudent(ufid);
            cout << (success ? "successful" : "unsuccessful") << endl;
        }
        else if (command == "dropClass") {
            int ufid;
            string classCode;
            ss >> ufid >> classCode;
            bool success = g.replaceClass(ufid, classCode, ""); // or implement dropClass properly
            cout << (success ? "successful" : "unsuccessful") << endl;
        }
        else if (command == "printShortestEdges") {
            int ufid;
            ss >> ufid;
            int residence = g.getStudentResidence(ufid);
            auto studentClasses = g.getStudentClasses(ufid); // implement helper to get classCode/location pairs
            auto shortest = g.shortestTimesFromResidence(residence, studentClasses);
            cout << "Name: " << g.getStudentName(ufid) << endl;
            for (auto &[classCode, time] : shortest)
                cout << classCode << " | Total Time: " << time << endl;
        }
        // Add other commands similarly...
    }

    return 0;
}
