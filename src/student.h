#pragma once
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/*
 * Student Class
 */
class Student {

private:
    string name;                // Full name of the student
    int ufid;                   // 8-digit UFID (int!)
    int age;                     // Student age
    int residence = 0;           // Residence location ID
    vector<string> classCodes;   // Classes

public:
    // Constructors
    Student() = default;
    Student(const string& studentName, int ufid, int studentAge, const vector<string>& classes, int res = 0);

    // Getters
    string getName() const;
    int getUFID() const;
    int getAge() const;
    int getResidence() const;           
    int getNumberOfClasses() const;
    vector<string> getClasses() const;

    // Class operations
    bool addClass(const string& classCode);
    bool removeClass(const string& classCode);
    bool replaceClass(const string& oldClass, const string& newClass);

    // Validators
    static bool isValidUFID(const string& ufid);
    static bool isValidName(const string& name);
    static bool isValidClassCode(const string& code);
};
