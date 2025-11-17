#include "Student.h"
#include <cctype>
#include <algorithm>
#include <iostream>

// Constructor with residence
Student::Student(const string& studentName,
                 int ufid,
                 int studentAge,
                 const vector<string>& classes,
                 int res)
    : name(studentName),
      ufid(ufid),
      age(studentAge),
      residence(res),classCodes(classes) {}

// Getters
string Student::getName() const { return name; }
int Student::getUFID() const { return ufid; }
int Student::getAge() const { return age; }
int Student::getResidence() const { return residence; }   // NEW
int Student::getNumberOfClasses() const { return static_cast<int>(classCodes.size()); }
vector<string> Student::getClasses() const { return classCodes; }

// Class operations
bool Student::addClass(const string& code) {
    if (find(classCodes.begin(), classCodes.end(), code) != classCodes.end())
        return false;
    classCodes.push_back(code);
    return true;
}

bool Student::removeClass(const string& code) {
    auto it = find(classCodes.begin(), classCodes.end(), code);
    if (it == classCodes.end()) return false;
    classCodes.erase(it);
    return true;
}

bool Student::replaceClass(const string& oldC, const string& newC) {
    auto it = find(classCodes.begin(), classCodes.end(), oldC);
    if (it == classCodes.end()) return false;
    if (find(classCodes.begin(), classCodes.end(), newC) != classCodes.end()) return false;
    *it = newC;
    return true;
}

// Validators
bool Student::isValidUFID(const string& id) {
    if (id.size() != 8) return false;
    return all_of(id.begin(), id.end(), ::isdigit);
}

bool Student::isValidName(const string& n) {
    if (n.empty()) return false;
    return all_of(n.begin(), n.end(), [](char c){ return isalpha(c) || c==' '; });
}

bool Student::isValidClassCode(const string& code) {
    if (code.size() != 7) return false;
    for (int i = 0; i < 3; i++)
        if (!isupper(code[i])) return false;
    for (int i = 3; i < 7; i++)
        if (!isdigit(code[i])) return false;
    return true;
}
