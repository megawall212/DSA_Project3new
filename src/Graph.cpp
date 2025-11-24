#include "Graph.h"
#include "student.h"
#include <fstream>
#include <sstream>
#include <queue>
#include <limits>
#include <algorithm>
#include <iostream>

using namespace std;


// load edges from CSV file
// each row = u,v, time
// build adjacency list
// return false if file cannot be opened or read 

bool Graph::loadEdgesCSV(const string& filepath) {
    ifstream file(filepath);
    if (!file.is_open()) return false;

    string line;
    getline(file, line); // this is for debugging... skip the header maybe...

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string loc1Str, loc2Str, name1, name2, travelTimeStr;

        if (!getline(ss, loc1Str, ',')) continue;
        if (!getline(ss, loc2Str, ',')) continue;
        if (!getline(ss, name1, ',')) continue;
        if (!getline(ss, name2, ',')) continue;
       

        if (!getline(ss, travelTimeStr)) continue;

        // the try catch here is to avoid invalid stoi conversion...
        // if any conversion fails, we just skip that line
        try {
            int loc1 = stoi(loc1Str);
            int loc2 = stoi(loc2Str);
            int travelTime = stoi(travelTimeStr);
            if (adjList.find(loc1) == adjList.end()) adjList[loc1] = {};
            if (adjList.find(loc2) == adjList.end()) adjList[loc2] = {};
            addEdge(loc1, loc2, travelTime);
        } catch (...) {
            continue;
        }
    }

    file.close();
    return true;
}


// load classes from CSV file
// remember it has classcode,location,start, and end time
// return false if file cannot be opened or read...
// we use classtolocation and classInfoMap functions here
// because we need to store class info too

bool Graph::loadClassesCSV(const string& filepath) {
    ifstream file(filepath);
    if (!file.is_open()) return false;

    string line;
    getline(file, line); // skip header line like the previous function
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string classCode, locStr, startTime, endTime;

        if (!getline(ss, classCode, ',')) continue;
        if (!getline(ss, locStr, ',')) continue;
        if (!getline(ss, startTime, ',')) continue;
        if (!getline(ss, endTime, ',')) continue;

        try {
            int locationId = stoi(locStr);
            classToLocation[classCode] = locationId;
            ClassInfo info;
            info.locationId = locationId;
            info.startTime = startTime;
            info.endTime = endTime;
            classInfoMap[classCode] = info;
            if (adjList.find(locationId) == adjList.end()) adjList[locationId] = {};
        } catch (...) {
            continue;
        }
    }

    file.close();
    return true;
}


// again getclasslocation returns locationid for classcode
// it here is -1 if DNE...
int Graph::getClassLocation(const string& classCode) const {
    auto it = classToLocation.find(classCode);
    return it != classToLocation.end() ? it->second : -1;
}
// getclassinfo returns stored classinfo for classcode

ClassInfo Graph::getClassInfo(const string& classCode) const {
    auto it = classInfoMap.find(classCode);
    return it != classInfoMap.end() ? it->second : ClassInfo();
}
// the set function sets/updates classinfo for classcode
// we've explained all these in the header file.
void Graph::setClassInfo(const std::string& classCode, const ClassInfo& info) {
    classInfoMap[classCode] = info;
    classToLocation[classCode] = info.locationId;
}

// insert edge into the adjacency list
// we assume undirected graph here for travel time between two locations
void Graph::addEdge(int loc1, int loc2, int travelTime) {
    if (adjList.find(loc1) == adjList.end()) adjList[loc1] = {};
    if (adjList.find(loc2) == adjList.end()) adjList[loc2] = {};
    adjList[loc1].push_back(Edge(loc2, travelTime, true));
    adjList[loc2].push_back(Edge(loc1, travelTime, true));
}

// toggle edges between open/closed
// be careful.. we need to return false immediately if any edge DNE
bool Graph::toggleEdgesClosure(const vector<pair<int,int>>& edges) {
    for (const auto& p : edges) {
        int u = p.first, v = p.second;
        Edge* e1 = findEdge(u, v);
        Edge* e2 = findEdge(v, u);
        if (!e1 || !e2) return false; // fail immediately
        e1->isOpen = !e1->isOpen;
        e2->isOpen = !e2->isOpen;
    }
    return true;
}

// check if a specific edge is open/closed/DNE
string Graph::checkEdgeStatus(int loc1, int loc2) const {
    const Edge* edge = findEdgeConst(loc1, loc2);
    if (!edge) return "DNE";
    return edge->isOpen ? "open" : "closed";
}


// we need to do a simple BFS to test connectivity using only open edges
bool Graph::isConnected(int src, int dst) const {
    if (adjList.find(src) == adjList.end() || adjList.find(dst) == adjList.end()) return false;
    unordered_set<int> visited;
    queue<int> q;
    q.push(src);
    visited.insert(src);
    while (!q.empty()) {
        int cur = q.front(); q.pop();
        if (cur == dst) return true;
        for (const auto& e : adjList.at(cur)) {
            if (!e.isOpen) continue;
            if (visited.find(e.destination) == visited.end()) {
                visited.insert(e.destination);
                q.push(e.destination);
            }
        }
    }
    return false;
}

// We love dijkstra...
// if startnode or endnode DNE, return totalcost -1 /no path...
// remember the step.. initialize infinity distance for every reachable node...
// then use a min heap...
// finally build the path and return the result...

PathResult Graph::dijkstra(int src, int dst) const {
    PathResult result;
    result.totalCost = -1;
    if (adjList.find(src) == adjList.end()) return result;

    const int INF = numeric_limits<int>::max();
    unordered_map<int,int> dist;
    unordered_map<int,int> parent;

    for (const auto& kv : adjList) {
        dist[kv.first] = INF;
        for (const auto& e : kv.second) if (dist.find(e.destination) == dist.end()) dist[e.destination] = INF;
    }

    if (dist.find(dst) == dist.end()) return result;

    dist[src] = 0;
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [dcur, node] = pq.top(); pq.pop();
        if (dcur > dist[node]) continue;
        if (node == dst) break;
        for (const auto& e : adjList.at(node)) {
            if (!e.isOpen) continue;
            int nxt = e.destination;
            long long cand = (long long)dcur + e.travelTime;
            if (cand < dist[nxt]) {
                dist[nxt] = static_cast<int>(cand);
                parent[nxt] = node;
                pq.push({dist[nxt], nxt});
            }
        }
    }

    if (dist[dst] == INF) {
        result.totalCost = -1;
        return result;
    }

    result.totalCost = dist[dst];
    result.path = buildPath(dst, parent);
    return result;
}

// shortestPath simply calls dijkstra internally
PathResult Graph::shortestPath(int start, int end) {
    return dijkstra(start, end);
}

// buildpath constructs the path from source to target
// Of course we are using it from dijkstra parent map...
vector<int> Graph::buildPath(int target, const unordered_map<int,int>& parent) const {
    vector<int> path;
    int cur = target;
    while (parent.find(cur) != parent.end()) {
        path.push_back(cur);
        cur = parent.at(cur);
    }
    path.push_back(cur);
    reverse(path.begin(), path.end());
    return path;
}


//geneate all the edges in the induced subgraph
vector<EdgeInfo> Graph::inducedSubgraphEdges(const unordered_set<int>& vertices) const {
    vector<EdgeInfo> edges;
    for (int u : vertices) {
        auto it = adjList.find(u);
        if (it == adjList.end()) continue;
        for (const auto& e : it->second) {
            int v = e.destination;

            // Notice that we need to force u < v to avoid duplicates
            // because this graph is undirected...
            if (vertices.find(v) != vertices.end() && u < v && e.isOpen) {
                edges.push_back(EdgeInfo(u, v, e.travelTime));
            }
        }
    }
    return edges;
}

// Well heads or tail?
// I decided to use Prim's algorithm to calculate....
// the MST cost for this induced subgraph...

int Graph::computeMSTCost(const unordered_set<int>& vertices, const vector<EdgeInfo>& edges) const {
    if (vertices.empty()) return 0;
    unordered_map<int, vector<pair<int,int>>> g;
    for (int v : vertices) g[v] = {};
    for (const auto& e : edges) {
        g[e.startNodeId].push_back({e.endNodeId, e.weight});
        g[e.endNodeId].push_back({e.startNodeId, e.weight});
    }

    unordered_set<int> inMST;
    unordered_map<int,int> minEdge;

    // initialize minEdge to infinity
    for (auto &kv : g) minEdge[kv.first] = numeric_limits<int>::max();

    int start = *vertices.begin();
    minEdge[start] = 0;
    int total = 0;

    while (inMST.size() < vertices.size()) {
        int pick = -1;
        int best = numeric_limits<int>::max();
        for (auto &kv : minEdge) {
            if (inMST.find(kv.first) == inMST.end() && kv.second < best) {
                best = kv.second; pick = kv.first;
            }
        }
        if (pick == -1) break;
        inMST.insert(pick);
        total += minEdge[pick];
        for (auto &nei : g[pick]) {
            int v = nei.first, w = nei.second;
            if (inMST.find(v) == inMST.end() && w < minEdge[v]) minEdge[v] = w;
        }
    }
    return total;
}

// computeStudentZoneCost computes the total cost of MST 
// for the set of class locations from residenceId
// we first need to build the induced subgraph including residence and all class locations
// then we call computeMSTCost to get the result
// think about this as connecting all class locations and residence with minimum total travel time

int Graph::computeStudentZoneCost(int residenceId, const vector<int>& classLocations) const {
    unordered_set<int> vertices;
    vertices.insert(residenceId);
    for (int loc : classLocations) {
        PathResult pr = dijkstra(residenceId, loc);
        if (pr.totalCost == -1) continue;
        for (int node : pr.path) vertices.insert(node);
    }
    vector<EdgeInfo> edges = inducedSubgraphEdges(vertices);
    return computeMSTCost(vertices, edges);
}


// Ok here's what I failed one of the test cases...
// we need to validate everything before adding a student...
// including name, ufid, classes, residence...

bool Graph::addStudent(const std::string& name, int ufid,const std::vector<std::string>& classes, int residence) {
    // Validate UFID and name
    string ufidStr = to_string(ufid);
    if (!Student::isValidUFID(ufidStr)) return false;
    if (!Student::isValidName(name)) return false;

    // Validate class count
    if (classes.size() < 1 || classes.size() > 6) return false;

    // Check UFID uniqueness
    if (students.find(ufid) != students.end()) return false;

    // Validate class format only (do NOT check existence in classToLocation here)
    for (const auto& c : classes) {
        if (!Student::isValidClassCode(c)) return false;
        if (classToLocation.find(c) == classToLocation.end()) return false; 
    }

    // validate resident ID (it should exists as a node in the graph)

    if (adjList.find(residence) == adjList.end())
        return false;

    // Finally we can add student...
    students[ufid] = Student(name, ufid,classes, residence);
    return true;
}

// remove student by ufid
bool Graph::removeStudent(int ufid) {
    return students.erase(ufid) > 0;
}

// remove class from all students
// well remove the student entirely too if no classes left after removal
int Graph::removeClass(const std::string& classCode) {
    int count = 0;
    vector<int> toErase;
    for (auto &kv : students) {
        int uf = kv.first;
        Student &s = kv.second;
        if (s.hasClass(classCode)) {
            if (s.removeClass(classCode)) {
                count++;
                if (s.getNumberOfClasses() == 0) toErase.push_back(uf);
            }
        }
    }
    for (int u : toErase) students.erase(u);
    classToLocation.erase(classCode);
    classInfoMap.erase(classCode);
    return count;
}

bool Graph::dropClass(int ufid, const std::string& classCode) {
    auto it = students.find(ufid);
    if (it == students.end()) return false;
    Student &s = it->second;
    if (!s.hasClass(classCode)) return false;
    s.removeClass(classCode);
    if (s.getNumberOfClasses() == 0) students.erase(ufid);
    return true;
}

bool Graph::replaceClass(int ufid, const std::string& oldClass, const std::string& newClass) {
    auto it = students.find(ufid);
    if (it == students.end()) return false;
    Student &s = it->second;
    if (!s.hasClass(oldClass)) return false;
    if (s.hasClass(newClass)) return false;
    if (classToLocation.find(newClass) == classToLocation.end()) return false;
    return s.replaceClass(oldClass, newClass);
}

// Student helpers for main.cpp...
// I explained these in the header file...

string Graph::getStudentName(int ufid) const {
    auto it = students.find(ufid);
    return it != students.end() ? it->second.getName() : "";
}

int Graph::getStudentResidence(int ufid) const {
    auto it = students.find(ufid);
    return it != students.end() ? it->second.getResidence() : -1;
}

vector<pair<string,int>> Graph::getStudentClasses(int ufid) const {
    vector<pair<string,int>> result;
    auto it = students.find(ufid);
    if (it == students.end()) return result;
    for (const auto &code : it->second.getClasses()) {
        result.emplace_back(code, getClassLocation(code));
    }
    return result;
}


// shortestTimesFromResidence computes shortest travel times
// from residenceId to each class location in classes

map<string,int> Graph::shortestTimesFromResidence(int residenceId, const vector<pair<string,int>>& classes) const {
    map<string,int> out;
    if (adjList.find(residenceId) == adjList.end()) {
        // but still return mapping with -1 for each class
        for (const auto &p : classes) out[p.first] = -1;
        return out;
    }
    for (const auto &p : classes) {
        const string &classCode = p.first;
        int loc = p.second;
        if (loc == -1 || adjList.find(loc) == adjList.end()) {
            out[classCode] = -1;
            continue;
        }
        PathResult pr = dijkstra(residenceId, loc);
        out[classCode] = pr.totalCost;
    }
    return out;
}


// Ok finally it's time for extra credit...
// we check if the schedule has any conflicts...
// we simply check if we can make it from one class to the next
// even it's back to back classes...

vector<string> Graph::verifySchedule(const vector<ClassInfo>& classes) const {
    vector<string> out;
    if (classes.size() <= 1) return out;
    // Since I used HH/MM style, we need to convert them to minutes...
    auto timeToMinutes = [](const string& s)->int {
        int h = stoi(s.substr(0,2));
        int m = stoi(s.substr(3,2));
        return h*60 + m;
    };

    // check each consecutive class pair
    for (size_t i = 0; i+1 < classes.size(); ++i) {
        PathResult pr = dijkstra(classes[i].locationId, classes[i+1].locationId);
        if (pr.totalCost == -1) { out.push_back("Cannot make it!"); continue; }
        // calculate the gap time between classes
        int gap = timeToMinutes(classes[i+1].startTime) - timeToMinutes(classes[i].endTime);
        out.push_back(gap >= pr.totalCost ? "Can make it!" : "Cannot make it!");
    }
    return out;
}

// the edge helpers to find edges in the adjacency list...

Edge* Graph::findEdge(int u, int v) {
    auto it = adjList.find(u);
    if (it == adjList.end()) return nullptr;
    for (auto &e : it->second) if (e.destination == v) return &e;
    return nullptr;
}

const Edge* Graph::findEdgeConst(int u, int v) const {
    auto it = adjList.find(u);
    if (it == adjList.end()) return nullptr;
    for (const auto &e : it->second) if (e.destination == v) return &e;
    return nullptr;
}


// again we just print graph and student counts for debugging...
void Graph::debugGraphState() const {
    cerr << "Nodes: " << adjList.size() << "\n";
    cerr << "Students: " << students.size() << "\n";
}
