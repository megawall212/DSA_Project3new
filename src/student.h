#pragma once
#include <string>
#include <vector>
#include <algorithm> // Added for validators if implemented in header, but okay here too.

using namespace std;

/*
 * Student Class
 */
class Student {
private:
    string name;                // Full name of the student
    string ufid;                   // 8-digit UFID (int)
    // int age;                 // REMOVED: Not in project requirements
    int residence = 0;          // Residence location ID
    vector<string> classCodes;  // Classes

public:
    // Constructors
    Student() = default;
    
    // Updated Constructor: Removed age
    Student(const string& studentName, const string& ufid, const vector<string>& classes, int res);

    // Getters
    string getName() const;
    string getUFID() const;
    // int getAge() const;      we dont need age....
    int getResidence() const;
    int getNumberOfClasses() const;
    vector<string> getClasses() const;

    // Query
    bool hasClass(const string& classCode) const;

    // Class operations
    bool addClass(const string& classCode);
    bool removeClass(const string& classCode);
    bool replaceClass(const string& oldClass, const string& newClass);

    // Validators (static helpers)
    static bool isValidUFID(const string& ufid);
    static bool isValidName(const string& name);
    static bool isValidClassCode(const string& code);
};
