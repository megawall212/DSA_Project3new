#include "Graph.h"
#include "student.h"
#include <fstream>
#include <sstream>
#include <queue>
#include <limits>
#include <algorithm>
#include <iostream>
using namespace std;

/* ----------------------------
   Helper: Ensure node exists
   ---------------------------- */
void Graph::addNodeIfMissing(int nodeId) {
    if (adjList.find(nodeId) == adjList.end()) {
        adjList[nodeId] = {}; // create empty adjacency list
    }
}

/* ----------------------------
   CSV Loading Functions
   ---------------------------- */

// Load edges.csv into adjacency list
bool Graph::loadEdgesCSV(const string& filepath) {
    ifstream file(filepath);
    if (!file.is_open()) return false;

    string line;
    getline(file, line); // skip header

    while (getline(file, line)) {
        stringstream ss(line);
        string loc1Str, loc2Str, name1, name2, travelTimeStr;

        if (!getline(ss, loc1Str, ',')) continue;
        if (!getline(ss, loc2Str, ',')) continue;
        if (!getline(ss, name1, ',')) continue;
        if (!getline(ss, name2, ',')) continue;
        if (!getline(ss, travelTimeStr, ',')) continue;

        int loc1 = stoi(loc1Str);
        int loc2 = stoi(loc2Str);
        int travelTime = stoi(travelTimeStr);

        addNodeIfMissing(loc1);
        addNodeIfMissing(loc2);

        addEdge(loc1, loc2, travelTime);
    }

    file.close();
    return true;
}

// Load classes.csv into classToLocation and classInfoMap
bool Graph::loadClassesCSV(const string& filepath) {
    ifstream file(filepath);
    if (!file.is_open()) return false;

    string line;
    getline(file, line); // skip header

    while (getline(file, line)) {
        stringstream ss(line);
        string classCode, locStr, startTime, endTime;

        if (!getline(ss, classCode, ',')) continue;
        if (!getline(ss, locStr, ',')) continue;
        if (!getline(ss, startTime, ',')) continue;
        if (!getline(ss, endTime, ',')) continue;

        int locationId = stoi(locStr);
        addNodeIfMissing(locationId); // ensure class location exists

        classToLocation[classCode] = locationId;
        ClassInfo info;
        info.locationId = locationId;
        info.startTime = startTime;
        info.endTime = endTime;
        classInfoMap[classCode] = info;
    }

    file.close();
    return true;
}

/* ----------------------------
   Class Queries
   ---------------------------- */

int Graph::getClassLocation(const string& classCode) const {
    auto it = classToLocation.find(classCode);
    if (it != classToLocation.end()) return it->second;
    return -1;
}

ClassInfo Graph::getClassInfo(const string& classCode) const {
    auto it = classInfoMap.find(classCode);
    if (it != classInfoMap.end()) return it->second;
    return ClassInfo();
}

/* ----------------------------
   Edge Operations
   ---------------------------- */

void Graph::addEdge(int loc1, int loc2, int travelTime) {
    adjList[loc1].push_back(Edge(loc2, travelTime, true));
    adjList[loc2].push_back(Edge(loc1, travelTime, true));
}

void Graph::toggleEdgeClosure(int loc1, int loc2) {
    Edge* edge1 = findEdge(loc1, loc2);
    Edge* edge2 = findEdge(loc2, loc1);
    if (edge1) edge1->isOpen = !edge1->isOpen;
    if (edge2) edge2->isOpen = !edge2->isOpen;
}

void Graph::toggleMultipleEdges(const vector<pair<int,int>>& edges) {
    for (auto& p : edges) {
        toggleEdgeClosure(p.first, p.second);
    }
}

string Graph::checkEdgeStatus(int loc1, int loc2) const {
    const Edge* edge = findEdgeConst(loc1, loc2);
    if (!edge) return "DNE";
    return edge->isOpen ? "open" : "closed";
}

/* ----------------------------
   Connectivity & Shortest Paths
   ---------------------------- */

bool Graph::isConnected(int src, int dst) const {
    if (adjList.find(src) == adjList.end() || adjList.find(dst) == adjList.end()) return false;

    unordered_set<int> visited;
    queue<int> q;
    q.push(src);
    visited.insert(src);

    while (!q.empty()) {
        int current = q.front(); q.pop();
        if (current == dst) return true;

        for (const auto& edge : adjList.at(current)) {
            if (edge.isOpen && visited.find(edge.destination) == visited.end()) {
                visited.insert(edge.destination);
                q.push(edge.destination);
            }
        }
    }
    return false;
}

PathResult Graph::dijkstra(int src, int dst) const {
    PathResult result;
    // source must exist in adjList
    if (adjList.find(src) == adjList.end()) {
        result.totalCost = -1;
        return result;
    }

    // initialize dist & parent for every node that appears anywhere in the graph:
    unordered_map<int,int> dist;
    unordered_map<int,int> parent;
    const int INF = numeric_limits<int>::max();

    for (const auto& kv : adjList) {
        dist[kv.first] = INF;
        for (const auto& e : kv.second) {
            // ensure destination nodes also get initialized (in case they are not keys)
            if (dist.find(e.destination) == dist.end()) dist[e.destination] = INF;
        }
    }

    // if dst is not present anywhere (neither key nor destination) -> unreachable
    if (dist.find(dst) == dist.end()) {
        result.totalCost = -1;
        return result;
    }

    dist[src] = 0;

    // min-heap: (distance, node)
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [curDist, node] = pq.top(); pq.pop();

        // ignore stale entries
        if (curDist > dist[node]) continue;

        // optional early stop (safe because we always pop the smallest dist first)
        if (node == dst) break;

        auto it = adjList.find(node);
        if (it == adjList.end()) continue;

        for (const auto& edge : it->second) {
            if (!edge.isOpen) continue;
            int nextNode = edge.destination;
            // if nextNode wasn't in dist for some reason, treat as INF (shouldn't happen now)
            int known = dist.count(nextNode) ? dist.at(nextNode) : INF;
            if (curDist == INF) continue; // defensive
            int newDist = curDist + edge.travelTime;
            if (newDist < known) {
                dist[nextNode] = newDist;
                parent[nextNode] = node;
                pq.push({newDist, nextNode});
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


//  tests that call shortestPath(...) use the dijkstra implementation
PathResult Graph::shortestPath(int start, int end) {
    return dijkstra(start, end);
}



vector<int> Graph::buildPath(int target, const unordered_map<int,int>& parent) const {
    vector<int> path;
    int current = target;
    while (parent.find(current) != parent.end()) {
        path.push_back(current);
        current = parent.at(current);
    }
    path.push_back(current);
    reverse(path.begin(), path.end());
    return path;
}





void Graph::debugGraphState() const {
    cout << "Adjacency List:\n";
    for (auto& [node, edges] : adjList) {
        cout << "Node " << node << ": ";
        for (auto& e : edges) cout << "(" << e.destination << ", " << e.travelTime << ", " << (e.isOpen?"open":"closed") << ") ";
        cout << "\n";
    }

    cout << "\nClass Locations:\n";
    for (auto& [code, loc] : classToLocation) cout << code << " -> " << loc << "\n";

    cout << "\nStudents:\n";
    for (auto& [ufid, student] : students) {
        cout << ufid << ": " << student.getName() << " Classes: ";
        for (auto& c : student.getClasses()) cout << c << " ";
        cout << "\n";
    }
}





map<string,int> Graph::shortestTimesFromResidence(int residenceId, const vector<pair<string,int>>& classes) const {
    map<string,int> result;
    if (adjList.find(residenceId) == adjList.end()) return result;

    for (const auto& [classCode, locId] : classes) {
        if (adjList.find(locId) == adjList.end()) {
            result[classCode] = -1;
            continue;
        }
        PathResult pathRes = dijkstra(residenceId, locId);
        result[classCode] = pathRes.totalCost;
    }
    return result;
}

/* ----------------------------
   Student Zone (MST)
   ---------------------------- */

vector<EdgeInfo> Graph::inducedSubgraphEdges(const unordered_set<int>& vertices) const {
    vector<EdgeInfo> edges;
    for (int u : vertices) {
        for (const auto& edge : adjList.at(u)) {
            int v = edge.destination;
            if (vertices.find(v) != vertices.end() && u < v && edge.isOpen) {
                edges.push_back(EdgeInfo(u, v, edge.travelTime));
            }
        }
    }
    return edges;
}

int Graph::computeMSTCost(const unordered_set<int>& vertices, const vector<EdgeInfo>& edges) const {
    if (vertices.empty()) return 0;

    unordered_set<int> inMST;
    map<int,int> key;
    for (int v : vertices) key[v] = numeric_limits<int>::max();

    int start = *vertices.begin();
    key[start] = 0;
    int totalCost = 0;

    while (inMST.size() < vertices.size()) {
        int u = -1;
        int minKey = numeric_limits<int>::max();
        for (auto& kv : key) {
            if (inMST.find(kv.first) == inMST.end() && kv.second < minKey) {
                minKey = kv.second;
                u = kv.first;
            }
        }
        if (u == -1) break; // disconnected

        inMST.insert(u);
        totalCost += key[u];

        for (const auto& edge : edges) {
            int a = edge.startNodeId, b = edge.endNodeId, w = edge.weight;
            if (inMST.find(a) != inMST.end() && inMST.find(b) == inMST.end() && w < key[b]) key[b] = w;
            else if (inMST.find(b) != inMST.end() && inMST.find(a) == inMST.end() && w < key[a]) key[a] = w;
        }
    }

    return totalCost;
}

int Graph::computeStudentZoneCost(int residenceId, const vector<int>& classLocations) const {
    unordered_set<int> vertices = {residenceId};
    vertices.insert(classLocations.begin(), classLocations.end());

    vector<EdgeInfo> subEdges = inducedSubgraphEdges(vertices);
    return computeMSTCost(vertices, subEdges);
}

/* ----------------------------
   Extra Credit: Schedule Verification
   ---------------------------- */

vector<string> Graph::verifySchedule(const vector<ClassInfo>& classes) const {
    vector<string> results;

    auto timeToMinutes = [](const string& s) -> int {
        int h = stoi(s.substr(0,2));
        int m = stoi(s.substr(3,2));
        bool pm = s.find("PM") != string::npos;
        if (pm && h != 12) h += 12;
        if (!pm && h == 12) h = 0;
        return h*60 + m;
    };

    for (size_t i = 0; i + 1 < classes.size(); ++i) {
        PathResult pathRes = dijkstra(classes[i].locationId, classes[i+1].locationId);
        if (pathRes.totalCost == -1) {
            results.push_back("Cannot make it!");
            continue;
        }

        int gap = timeToMinutes(classes[i+1].startTime) - timeToMinutes(classes[i].endTime);
        results.push_back(gap >= pathRes.totalCost ? "Can make it!" : "Cannot make it!");
    }

    return results;
}

/* ----------------------------
   Helper functions: find edges
   ---------------------------- */

Edge* Graph::findEdge(int u, int v) {
    auto it = adjList.find(u);
    if (it == adjList.end()) return nullptr;
    for (auto& edge : it->second) if (edge.destination == v) return &edge;
    return nullptr;
}

const Edge* Graph::findEdgeConst(int u, int v) const {
    auto it = adjList.find(u);
    if (it == adjList.end()) return nullptr;
    for (const auto& edge : it->second) if (edge.destination == v) return &edge;
    return nullptr;
}

/* ----------------------------
   Validation Helpers
   ---------------------------- */

bool Graph::isValidClassCode(const string& s) {
    if (s.length() != 7) return false;
    for (int i = 0; i < 3; ++i) if (!isupper(s[i])) return false;
    for (int i = 3; i < 7; ++i) if (!isdigit(s[i])) return false;
    return true;
}

bool Graph::isValidUFID(const string& s) {
    if (s.length() != 8) return false;
    for (char c : s) if (!isdigit(c)) return false;
    return true;
}

bool Graph::isValidTime(const string& s) {
    if (s.length() < 7) return false;
    if (!isdigit(s[0]) || !isdigit(s[1]) || s[2] != ':' || !isdigit(s[3]) || !isdigit(s[4])) return false;
    if (s.find("AM") == string::npos && s.find("PM") == string::npos) return false;
    return true;
}

bool Graph::isValidName(const string& s) {
    for (char c : s) if (!isalpha(c) && c != ' ') return false;
    return true;
}

/* ----------------------------
   Student/Class Management
   ---------------------------- */

bool Graph::addStudent(const std::string& name, int ufid, int age, const std::vector<std::string>& classes, int residence) {
    if (students.find(ufid) != students.end()) return false;
    students[ufid] = Student(name, ufid, age, classes, residence);
    return true;
}


bool Graph::removeStudent(int ufid) {
    return students.erase(ufid) > 0;
}

int Graph::removeClass(const std::string& classCode) {
    int count = 0;
    for (auto& [ufid, student] : students) {
        if (student.removeClass(classCode)) count++;
    }

    classToLocation.erase(classCode);
    classInfoMap.erase(classCode);

    return count;
}

void Graph::dropClass(const std::string& classCode) {
    for (auto& [ufid, student] : students) student.removeClass(classCode);

    classToLocation.erase(classCode);
    classInfoMap.erase(classCode);
}

bool Graph::replaceClass(int ufid, const std::string& oldClass, const std::string& newClass) {
    auto it = students.find(ufid);
    if (it == students.end()) return false;
    return it->second.replaceClass(oldClass, newClass);
}

void Graph::setClassInfo(const std::string& classCode, const ClassInfo& info) {
    classInfoMap[classCode] = info;
    classToLocation[classCode] = info.locationId; 
}

ClassInfo Graph::getClassInfoPublic(const std::string& classCode) const {
    auto it = classInfoMap.find(classCode);
    if (it != classInfoMap.end()) return it->second;
    return ClassInfo();
}


string Graph::getStudentName(int ufid) const {
    auto it = students.find(ufid);
    return it != students.end() ? it->second.getName() : "";
}

int Graph::getStudentResidence(int ufid) const {
    auto it = students.find(ufid);
    return it != students.end() ? it->second.getResidence() : -1;
}

vector<pair<string, int>> Graph::getStudentClasses(int ufid) const {
    auto it = students.find(ufid);
    vector<pair<string,int>> classes;
    if (it != students.end()) {
        for (const auto& code : it->second.getClasses()) {
            auto locIt = classToLocation.find(code);
            int loc = (locIt != classToLocation.end()) ? locIt->second : -1;
            classes.emplace_back(code, loc);
        }
    }
    return classes;
}




