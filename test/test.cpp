#define CATCH_CONFIG_MAIN
#include "catch/catch_amalgamated.hpp"

#include "../src/student.h"
#include "../src/Graph.h"
#include <vector>
#include <string>
using namespace std;

/*
===========================================================
 TEST 1: Incorrect commands / invalid insertions
===========================================================
*/
TEST_CASE("Incorrect commands / invalid student data") {
    Graph g;

    // invalid UFID (not 8 digits)
    REQUIRE(g.addStudent("John", 123, {"COP3530"}, 10) == false);

    // duplicate UFID
    REQUIRE(g.addStudent("Amy", 11112222, {"COP3502"}, 12) == true);
    REQUIRE(g.addStudent("Bob", 11112222, {"COP3503"}, 14) == false);

    // invalid class code patterns
    REQUIRE(Student::isValidClassCode("BAD") == false);
    REQUIRE(Student::isValidClassCode("123456") == false);
    REQUIRE(Student::isValidClassCode("COP3502") == true);
}

/*
===========================================================
 TEST 2: Edge-case operations
===========================================================
*/
TEST_CASE("Edge cases: removing nonexistent student, dropping nonexistent class") {
    Graph g;

    g.addStudent("Amy", 12345678, {"COP3530"}, 5);

    // removing nonexistent UFID
    REQUIRE(g.removeStudent(99999999) == false);

    // dropping class student does NOT have
    REQUIRE(g.dropClass(12345678, "COP3502") == false);

    // removing a class code that no student has
    REQUIRE(g.removeClass("COP9999") == 0);
}

/*
===========================================================
 TEST 3: dropClass, removeClass, remove, replaceClass
===========================================================
*/
TEST_CASE("dropClass / removeClass / removeStudent / replaceClass") {
    Graph g;

    g.addStudent("Amy", 22223333, {"COP3530", "MAC2311"}, 10);

    // dropClass
    REQUIRE(g.dropClass(22223333, "MAC2311") == true);
    REQUIRE(g.dropClass(22223333, "MAC2311") == false); // already gone

    // replaceClass
    REQUIRE(g.replaceClass(22223333, "COP3530", "COP3503") == true);
    REQUIRE(g.replaceClass(22223333, "COP3530", "COP3503") == false); // old no longer exists

    // removeClass globally
    // Only 1 student still has COP3503
    REQUIRE(g.removeClass("COP3503") == 1);

    // remove student
    REQUIRE(g.removeStudent(22223333) == true);
    REQUIRE(g.removeStudent(22223333) == false);
}

/*
===========================================================
 TEST 4: Shortest path becomes unreachable after edges toggle
===========================================================
*/
TEST_CASE("printShortestEdges logic: reachable then unreachable after edge closure") {
    Graph g;

    // Build small graph manually:
    // 1 --5--> 2 --5--> 3
    g.addEdge(1, 2, 5);
    g.addEdge(2, 3, 5);

    // Each class maps to a location
    g.setClassInfo("COP3530", ClassInfo{3, "10:00", "11:00"});

    g.addStudent("Sam", 44445555, {"COP3530"}, 1);

    // Should be reachable now
    auto res = g.dijkstra(1, 3);
    REQUIRE(res.totalCost == 10);

    // Now toggle both edges OFF
    REQUIRE(g.toggleEdgesClosure({{1,2}, {2,3}}) == true);

    // Now unreachable
    auto res2 = g.dijkstra(1, 3);
    REQUIRE(res2.totalCost == -1);
}

/*
===========================================================
 TEST 5: isConnected + checkEdgeStatus basic behavior
===========================================================
*/
TEST_CASE("Edge status and connectivity") {
    Graph g;

    g.addEdge(1, 2, 3);
    g.addEdge(2, 3, 3);

    REQUIRE(g.isConnected(1, 3) == true);

    // check edge status string
    REQUIRE(g.checkEdgeStatus(1,2) == "open");

    // close edge 1-2
    g.toggleEdgesClosure({{1,2}});

    REQUIRE(g.checkEdgeStatus(1,2) == "closed");
    REQUIRE(g.isConnected(1,3) == false);
}
