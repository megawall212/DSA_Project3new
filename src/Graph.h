#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include "student.h"

using namespace std;

struct Edge {
    int destination;
    int travelTime;
    bool isOpen;
    Edge(int dest = 0, int time = 0, bool open = true)
        : destination(dest), travelTime(time), isOpen(open) {}
};

struct PathResult {
    int totalCost = -1;
    vector<int> path;
    PathResult() = default;
};

struct ClassInfo {
    int locationId = -1;
    string startTime;
    string endTime;
    ClassInfo() = default;
};

struct EdgeInfo {
    int startNodeId;
    int endNodeId;
    int weight;
    EdgeInfo(int u = 0, int v = 0, int w = 0) : startNodeId(u), endNodeId(v), weight(w) {}
};

class Graph {
public:
    Graph() = default;
    ~Graph() = default;

    // CSV loaders (main.cpp will call using "../data/...")
    bool loadEdgesCSV(const string& filepath);
    bool loadClassesCSV(const string& filepath);

    // Class queries
    int getClassLocation(const string& classCode) const;
    ClassInfo getClassInfo(const string& classCode) const;
    void setClassInfo(const std::string& classCode, const ClassInfo& info);

    // Edge operations
    void addEdge(int u, int v, int timeCost);
    bool toggleEdgesClosure(const vector<pair<int,int>>& edges); // toggles each pair
    string checkEdgeStatus(int u, int v) const;

    // Connectivity & shortest path
    bool isConnected(int src, int dst) const;
    PathResult dijkstra(int src, int dst) const;
    PathResult shortestPath(int start, int end);

    // Student / course management
    bool addStudent(const std::string& name, int ufid,const std::vector<std::string>& classes, int residence);
    bool removeStudent(int ufid);
    int removeClass(const std::string& classCode);
    bool dropClass(int ufid, const std::string& classCode);
    bool replaceClass(int ufid, const std::string& oldClass, const std::string& newClass);

    // Student helpers for main
    string getStudentName(int ufid) const;
    int getStudentResidence(int ufid) const;
    vector<pair<string,int>> getStudentClasses(int ufid) const;

    // Times and student zone
    map<string,int> shortestTimesFromResidence(int residenceId, const vector<pair<string,int>>& classes) const;
    int computeStudentZoneCost(int residenceId, const vector<int>& classLocations) const;

    // Extra credit
    vector<string> verifySchedule(const vector<ClassInfo>& classes) const;

    // Debug
    void debugGraphState() const;

private:
    unordered_map<int, vector<Edge>> adjList;
    unordered_map<string, int> classToLocation;
    unordered_map<string, ClassInfo> classInfoMap;
    unordered_map<int, Student> students;

    vector<EdgeInfo> inducedSubgraphEdges(const unordered_set<int>& vertices) const;
    int computeMSTCost(const unordered_set<int>& vertices, const vector<EdgeInfo>& edges) const;

    Edge* findEdge(int u, int v);
    const Edge* findEdgeConst(int u, int v) const;

    vector<int> buildPath(int target, const unordered_map<int,int>& parent) const;
};
