#define CATCH_CONFIG_MAIN
#include "catch/catch_amalgamated.hpp"

#include "../src/student.h"
#include "../src/Graph.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

TEST_CASE("Invalid commands (at least 5)") {
    Student s;

    // Invalid class codes
    REQUIRE(!s.isValidClassCode("A11y"));
    REQUIRE(!s.isValidClassCode("COP35")); 
    REQUIRE(!s.isValidClassCode("COPS3000A"));

    // Invalid UFIDs
    REQUIRE(!s.isValidUFID("1234567a"));
    REQUIRE(!s.isValidUFID("abcdef12"));
}

TEST_CASE("Edge cases (at least 3)") {
    Graph g;
    
    // Removing student that doesn't exist
    REQUIRE(!g.removeStudent(99999999));

    // Removing class that doesn't exist
    REQUIRE(!g.removeClass("CPSC9999"));

    // Toggle edge that doesn't exist
    REQUIRE(g.checkEdgeStatus(999, 1000) == "DNE");
}

TEST_CASE("Drop, remove, and replace class") {
    Graph g;

    // Load sample classes
    g.loadClassesCSV("../data/classes.csv");

    // Drop class
    g.dropClass("COP3530");
    REQUIRE(g.getClassLocation("COP3530") == -1);

    // Remove class
    g.removeClass("CDA3101");
    REQUIRE(g.getClassLocation("CDA3101") == -1);

    // Insert class and replace class code
    ClassInfo newClass;
    newClass.locationId = 101;
    newClass.startTime = "10:00 AM";
    newClass.endTime = "11:00 AM";
    g.setClassInfo("MAC2311", newClass);
    REQUIRE(g.getClassLocation("MAC2311") == 101);
}

TEST_CASE("PrintShortestEdges scenario") {
    Graph g;

    g.loadEdgesCSV("../data/edges.csv");
    g.loadClassesCSV("../data/classes.csv");

    // Add a student so residence exists
    g.addStudent("Brian", 35459999, 21, {"COP3530", "CDA3101", "MAC2311"}, 16);

    int startNode = g.getStudentResidence(35459999); 
    int classLocation = g.getClassLocation("MAC2311"); 

    PathResult res1 = g.dijkstra(startNode, classLocation);
    REQUIRE(res1.totalCost >= 0); // Path exists initially

    // Toggle edges along Brian's path to MAC2311 to simulate closure
    // (Ensure these edges exist in your edges.csv for this student)
    g.toggleEdgeClosure(16, 5);
    g.toggleEdgeClosure(16, 6);
    g.toggleEdgeClosure(16, 19);
    g.toggleEdgeClosure(16, 42);

    PathResult res2 = g.dijkstra(startNode, classLocation);
    REQUIRE(res2.totalCost == -1); // Path is now blocked
}

TEST_CASE("Sample input/output test") {
    Graph g;

    g.loadEdgesCSV("../data/edges.csv");
    g.loadClassesCSV("../data/classes.csv");

    // Insert students
    g.addStudent("Brandon", 45679999, 20, {"COP3530", "MAC2311"}, 40);
    g.addStudent("Brian", 35459999, 21, {"COP3530", "CDA3101", "MAC2311"}, 16);
    g.addStudent("Briana", 87879999, 22, {"CDA3101", "MAC2311", "EEL3701"}, 49);

    g.debugGraphState(); // Optional: verify state visually

    // Remove class COP3530
    REQUIRE(g.removeClass("COP3530") == 2);

    // Remove student Briana
    REQUIRE(g.removeStudent(87879999));

    // Remove class CDA3101
    REQUIRE(g.removeClass("CDA3101") == 1);

    // Get Brian's residence and class location
    int startNode = g.getStudentResidence(35459999);
    int mac2311Location = g.getClassLocation("MAC2311");
    cout << "Brian's residence: " << startNode << ", MAC2311 location: " << mac2311Location << endl;

    // Compute shortest path
auto shortestTimes = g.shortestTimesFromResidence(35459999, {{"MAC2311", mac2311Location}});
int travelTime = shortestTimes["MAC2311"];

// DEBUG: Print the full path and distances
 // Compute shortest path
// Compute shortest path
auto pathResult = g.dijkstra(startNode, mac2311Location);
travelTime = pathResult.totalCost; // remove 'int' here, reuse the existing variable

cout << "Brian's residence: " << startNode 
     << ", MAC2311 location: " << mac2311Location << endl;
cout << "Total travel time to MAC2311: " << travelTime << endl;

// Only check travel time if the residence and class location differ
if (startNode != mac2311Location) {
    REQUIRE(travelTime > 0); // positive travel time expected
} else {
    REQUIRE(travelTime == 0); // same node, zero travel time
}

}

// Add more test cases as needed to cover at least 20 in total