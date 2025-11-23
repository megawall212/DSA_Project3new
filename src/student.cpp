#include "student.h"
#include <cctype>
#include <algorithm>
#include <iostream> // Added in case needed, though not strictly used in logic below

// Updated Constructor: Removed studentAge
Student::Student(const string& studentName, 
                 int ufid, 
                 const vector<string>& classes, 
                 int res)
    : name(studentName), ufid(ufid), residence(res), classCodes(classes) {}

string Student::getName() const { return name; }
int Student::getUFID() const { return ufid; }
// getAge() removed
int Student::getResidence() const { return residence; }
int Student::getNumberOfClasses() const { return static_cast<int>(classCodes.size()); }
vector<string> Student::getClasses() const { return classCodes; }

bool Student::hasClass(const string& classCode) const {
    return find(classCodes.begin(), classCodes.end(), classCode) != classCodes.end();
}

bool Student::addClass(const string& code) {
    if (hasClass(code)) return false;
    classCodes.push_back(code);
    return true;
}

bool Student::removeClass(const string& classCode) {
    auto it = find(classCodes.begin(), classCodes.end(), classCode);
    if (it == classCodes.end()) return false;
    classCodes.erase(it);
    return true;
}

bool Student::replaceClass(const string& oldClass, const string& newClass) {
    auto it = find(classCodes.begin(), classCodes.end(), oldClass);
    if (it == classCodes.end()) return false;
    if (hasClass(newClass)) return false;
    *it = newClass;
    return true;
}

bool Student::isValidUFID(const string& id) {
    if (id.size() != 8) return false;
    return all_of(id.begin(), id.end(), ::isdigit);
}

bool Student::isValidName(const string& n) {
    if (n.empty()) return false;
    // Allow alphabetic characters and spaces
    return all_of(n.begin(), n.end(), [](char c){ 
        return isalpha(static_cast<unsigned char>(c)) || c == ' '; 
    });
}

bool Student::isValidClassCode(const string& code) {
    if (code.size() != 7) return false;
    for (int i = 0; i < 3; ++i) if (!isupper(static_cast<unsigned char>(code[i]))) return false;
    for (int i = 3; i < 7; ++i) if (!isdigit(static_cast<unsigned char>(code[i]))) return false;
    return true;
}