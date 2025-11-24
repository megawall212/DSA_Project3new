// our graph.h
// defines the Graph class and related structs
// include all necessary headers and use C++17 standard required...
#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include "student.h"

using namespace std;


// We need to define Edge, PathResult, ClassInfo, EdgeInfo structs here
// destination is the node ID reached from current node
// travelTime is the weight of the edge
// isOpen indicates whether the path is usable or not (true = open, false = closed)

struct Edge {
    int destination;
    int travelTime;
    bool isOpen;
    Edge(int dest = 0, int time = 0, bool open = true)
        : destination(dest), travelTime(time), isOpen(open) {}
};

// stores the result of a shortest path search
// totalcost is -1 if unreachable, otherwise the cost (total travel time)
// path is the ordered list of node IDs from source to destination (chosen route)
struct PathResult {
    int totalCost = -1;
    vector<int> path;
    PathResult() = default;
};

// information about a class
// locationId is the node ID where the class is held
// startTime and endTime represent the class schedule

struct ClassInfo {
    int locationId = -1;
    string startTime;
    string endTime;
    ClassInfo() = default;
};

// information about an edge in the graph for MST computation
// startNodeId and endNodeId are the two endpoints of the edge
// well this function should only be used internally to calculate MST cost

struct EdgeInfo {
    int startNodeId;
    int endNodeId;
    int weight;
    EdgeInfo(int u = 0, int v = 0, int w = 0) : startNodeId(u), endNodeId(v), weight(w) {}
};

// Graph class definition
// main part of our project...
class Graph {
public:

    // Constructors / Destructors

    Graph() = default;
    ~Graph() = default;

    // CSV loaders (debug: main.cpp will call using "../data/...")
    // be careful of the relative path when testing
    // loadEdgesCSV loads edges into the graph
    // loadClassesCSV loads class location and time info

    bool loadEdgesCSV(const string& filepath);
    bool loadClassesCSV(const string& filepath);

    // Class queries
    // getClassLocation returns location ID for classCode, of course -1 if DNE...
    // getClassInfo returns stored ClassInfo (literally class information) for classCode
    // setClassInfo sets/updates ClassInfo for classCode (insertion...)

    int getClassLocation(const string& classCode) const;
    ClassInfo getClassInfo(const string& classCode) const;
    void setClassInfo(const std::string& classCode, const ClassInfo& info);

    // Edge operations
    // addEdge adds an directed edge between u and v with given traveltime
    // toggle EdgesClosure toggles each edge in the input list (if exists) between open/closed
    // Well it should open/close state for multuple edges
    // checkEdgeStatus returns "open", "closed", or "DNE" for edge (u,v)
    
    void addEdge(int u, int v, int timeCost);
    bool toggleEdgesClosure(const vector<pair<int,int>>& edges); 
    string checkEdgeStatus(int u, int v) const;

    
    // Connectivity & shortest path
    // return true if there is a path from src to dst
    // we love dijkstra...
    // we only use open edges for dijkstra

    bool isConnected(int src, int dst) const;
    PathResult dijkstra(int src, int dst) const;
    PathResult shortestPath(int start, int end);


    // Student / course management
    // addStudent adds a new student, returns false if ufid exists or invalid data.
    // don't forget to validate classes and residence ID (I forgot that at first..)!
    // make more edge cases 
    // removeStudent removes student by ufid, returns true if successful (entirely removed)
    // removeClass removes class from all students, returns number of students affected
    // dropClass removes class from 1 specific student, returns true if successful
    // replaceClass replaces oldClass with newClass for a specific student, returns true if successful

    bool addStudent(const std::string& name, int ufid,const std::vector<std::string>& classes, int residence);
    bool removeStudent(int ufid);
    int removeClass(const std::string& classCode);
    bool dropClass(int ufid, const std::string& classCode);
    bool replaceClass(int ufid, const std::string& oldClass, const std::string& newClass);

    
    
    // Student helpers for main.cpp
    // getStudentName returns name by ufid, empty string if DNE
    // getStudentResidence returns residence ID by ufid, -1 if DNE
    // getStudentClasses returns vector/pair of (classCode, locationID) pairs for student

    string getStudentName(int ufid) const;
    int getStudentResidence(int ufid) const;
    vector<pair<string,int>> getStudentClasses(int ufid) const;

    // Times and student zone
    // shortestTimesFromResidence returns map 
    // of classCode to shortest time from residenceId
    // basically the shortest travel time from residence to each class location
    // if unreachable or invalid location, time is -1
    // computeStudentZoneCost computes the total cost of MST 
    // for the set of class locations from residenceId
    map<string,int> shortestTimesFromResidence(int residenceId, const vector<pair<string,int>>& classes) const;
    int computeStudentZoneCost(int residenceId, const vector<int>& classLocations) const;

    // Extra credit...
    // we check if the schedule has any conflicts
    vector<string> verifySchedule(const vector<ClassInfo>& classes) const;

    // Debug
    // We might print the adjacency list and class/student registries for debugging
    // It's really useful for you to see which function works and which doesn't
    void debugGraphState() const;

private:

    // internal data structures
    // adjacency list representation of the graph
    // classToLocation maps class codes to location IDs
    // classInfoMap maps class codes to ClassInfo structs
    // students maps UFID to Student objects

    unordered_map<int, vector<Edge>> adjList;
    unordered_map<string, int> classToLocation;
    unordered_map<string, ClassInfo> classInfoMap;
    unordered_map<int, Student> students;


    // Some internal helper functions for graph algorithms...
    // inducedSubgraphEdges returns all edges between the given set of vertices
    // computeMSTCost computes the total weight of the MST for the given vertices and edges
    // we can use either Kruskal or Prim algorithm learned in class

    vector<EdgeInfo> inducedSubgraphEdges(const unordered_set<int>& vertices) const;
    int computeMSTCost(const unordered_set<int>& vertices, const vector<EdgeInfo>& edges) const;

    //findEdge helps to find edge from u to v
    //findEdgeConst is the const version...

    Edge* findEdge(int u, int v);
    const Edge* findEdgeConst(int u, int v) const;

    // buildPath constructs the path from source to target
    // Of course we are using it from dijkstra parent map...

    vector<int> buildPath(int target, const unordered_map<int,int>& parent) const;
};
