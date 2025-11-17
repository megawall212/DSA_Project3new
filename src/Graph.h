// This is the header file for the Graph class
// We declare the Graph class and its member functions here with public API
// First file we are gonna write 
// We need to define adjacency list of our graph
// First we need to include necessary libraries

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <optional>
#include "student.h"

using namespace std;

/*
 First of all, we store undirected edges as two entries 
 (u->v and v->u).
*/
struct Edge {
    int destination;     // destination location id (node id)
    int travelTime;      // travel time 
    bool isOpen;         // true if edge is usable; false if toggled closed

    // Constructor with default values
    Edge(int dest = 0, int time = 0, bool open = true)
        : destination(dest), travelTime(time), isOpen(open) {}
};

/*
 PathResult:
 - totalCost: -1 if unreachable, otherwise it should be a non-negative integer 
 - path: sequence of node IDs from source to target (empty if unreachable)

Holds the total cost (time) and the actual path of nodes to reach the destination
*/
struct PathResult {
    int totalCost;
    vector<int> path;
    PathResult() = default;
};

// Struct for class information
struct ClassInfo {
    int locationId = -1;         // Location ID of the classroom
    string startTime;            // Start time in AM/PM format (e.g., "10:40 AM")
    string endTime;              // End time in AM/PM format (e.g., "11:30 AM")
    ClassInfo() = default;
};

// For MST calculations
struct EdgeInfo {
    int startNodeId;   // ID of the starting node of the edge
    int endNodeId;     // ID of the ending node of the edge
    int weight;        // Weight of the edge (e.g., travel time between nodes)

    // Default constructor
    EdgeInfo(int u = 0, int v = 0, int w = 0)
        : startNodeId(u), endNodeId(v), weight(w) {}
};

/*
 Now we define the Graph class - represents campus network
 Functionalities to be implemented:
 1. Store adjacency list of locations and edges
 2. Store mapping from class code -> ClassInfo
 3. Provide graph algorithms: connectivity, shortest paths, MST (student zone)
 4. Provide edge operations in the rubric: toggle open/closed, check status
 5. Load CSV data for edges and classes
*/
class Graph {
public:
    // Constructor / Destructor
    Graph() = default;
    ~Graph() = default;

    // Setter to update classInfoMap
    void setClassInfo(const std::string& classCode, const ClassInfo& info);

    PathResult shortestPath(int start, int end);

    // Optional getter if you want to read it in tests
    ClassInfo getClassInfoPublic(const std::string& classCode) const;

    void debugGraphState() const;

    void addNodeIfMissing(int nodeId); 

    // Load the graph data from CSV files we have 
    bool loadEdgesCSV(const string& filepath);    // edges.csv
    bool loadClassesCSV(const string& filepath);  // classes.csv

    // Insert class for test purposes
    void insertClassForTest(const string& classCode, const ClassInfo& info) {
        classInfoMap[classCode] = info;
        classToLocation[classCode] = info.locationId;
    }

    // Class location/time queries
    // 2 functions to get class location and class info

    // returns -1 if not found
    int getClassLocation(const string& classCode) const; 

    // returns empty ClassInfo if not found
    ClassInfo getClassInfo(const string& classCode) const;    

    // Edge operations
    // Listed in the rubrics

    // Add a new edge between two locations
    void addEdge(int u, int v, int timeCost);

    // Toggle open/closed status of an edge
    void toggleEdgeClosure(int u, int v);

    // Toggle open/closed status of multiple edges
    void toggleMultipleEdges(const vector<pair<int,int>>& edges);

    // Check the status of an edge (open, closed, or does not exist)
    string checkEdgeStatus(int u, int v) const;

    // Connectivity and shortest paths
    // Check if a path exists between two locations using open edges
    bool isConnected(int src, int dst) const;

    // Compute shortest path between two nodes using Dijkstra's algorithm
    // Returns PathResult containing totalCost and path vector
    PathResult dijkstra(int src, int dst) const;

    // Student/Class Management

    // Adds a student. Returns true if successful, false if UFID already exists.
    bool addStudent(const std::string& name, int ufid, int age, const std::vector<std::string>& classes, int residence);
    // Removes a student by UFID. Returns true if removed, false if not found.
    bool removeStudent(int ufid);

    // Removes all students enrolled in a class. Returns the number of students removed.
    int removeClass(const std::string& classCode);

    // Drops a class from all students' schedules.
    void dropClass(const std::string& classCode);

    // Replaces a student's old class with a new class. Returns true if successful.
    bool replaceClass(int ufid, const std::string& oldClass, const std::string& newClass);

    // Compute shortest times from a residence to each class
    // Returns map: classCode -> travelTime
    // If class is unreachable, set travelTime = -1
    map<string, int> shortestTimesFromResidence(
        int residenceId,
        const vector<pair<string,int>>& classes
    ) const;

    // Student Zone (MST)
    // Compute the total cost of edges connecting residence 
    // We'll also need to consider all class locations
    int computeStudentZoneCost(int residenceId, const vector<int>& classLocations) const;

    // Extra Credit: verifySchedule
    // Checks if student can reach classes on time based on shortest paths
    vector<string> verifySchedule(const vector<ClassInfo>& classes) const;

    // --- Student getters for main.cpp ---
    string getStudentName(int ufid) const;
    vector<pair<string,int>> getStudentClasses(int ufid) const;
    int getStudentResidence(int ufid) const;

private:
    // Student registry: Map UFID -> Student object
    unordered_map<int, Student> students;

    // Adjacency list: From node ID -> vector of edges
    unordered_map<int, vector<Edge>> adjList; 

    // Class code -> location ID
    unordered_map<string, int> classToLocation; 

    // Class code -> ClassInfo
    unordered_map<string, ClassInfo> classInfoMap;           

    // Private helper functions

    // Dijkstra helper: returns parent map and distance map
    pair<unordered_map<int,int>, unordered_map<int,int>> dijkstraAll(int source, int target = -1) const;

    // Builds path from source to target using parent map
    vector<int> buildPath(int target, const unordered_map<int,int>& parent) const;

    // Get all edges for MST calculation for a subset of vertices
    vector<EdgeInfo> inducedSubgraphEdges(const unordered_set<int>& vertices) const;

    // Compute MST cost using Prim's algorithm
    int computeMSTCost(const unordered_set<int>& vertices, const vector<EdgeInfo>& edges) const;

    // Find edge in adjacency list; returns nullptr if not found
    Edge* findEdge(int u, int v);
    const Edge* findEdgeConst(int u, int v) const;

    // Validation helper functions
    static bool isValidClassCode(const string& s);  // 3 letters + 4 digits
    static bool isValidUFID(const string& s);       // Must be exactly 8 digits
    static bool isValidTime(const string& s);       // Validates time in "HH:MM AM/PM" format
    static bool isValidName(const string& s);       // Validates names (letters + spaces only)
};
