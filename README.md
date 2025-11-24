# 🎓 DSA Project 3 – UF Campus Compass & Student Scheduler

**Author:** Zeyu Li  
**Course:** COP3530 – Data Structures & Algorithms  
**Semester:** Fall 2025  

---

## 📌 Table of Contents

1. [Project Overview](#project-overview)  
2. [Features](#features)  
3. [Project Structure](#project-structure)  
4. [Dependencies](#dependencies)  
5. [Building the Project](#building-the-project)  
6. [Running the Application](#running-the-application)  
7. [Running Unit Tests](#running-unit-tests)  
8. [Commands Supported](#commands-supported)  
9. [Implementation Details](#implementation-details)  
10. [Notes & Edge Cases](#notes--edge-cases)  

---

## 📝 Project Overview

This project implements a **UF campus navigation & student class scheduler system** using C++.  
For more info about this project, see [info.md](info.md).  
For more info about setting up the Catch2 environment, see [catch2.md](catch2.md).  

✅ **Main functionalities:**

- Model campus buildings as a weighted undirected graph  
- Manage students with residences, classes, and UFIDs  
- Compute shortest travel times from student residences to classes  
- Simulate edge closures (restricted paths)  
- Verify if students can attend consecutive classes  

The project emphasizes **data structures**, **graph algorithms**, and **robust validation**.

---

## ✨ Features

| Feature | Description |
|---------|-------------|
| 🏫 Graph Management | Load campus edges, toggle closures, check connectivity, shortest paths (Dijkstra) |
| 📚 Class Management | Load class info, query locations, verify student schedules |
| 👨‍🎓 Student Management | Add/remove students, drop/replace classes, validate UFID & names |
| 🌐 Student Zone | Compute minimum travel cost using MST-based approach |
| ✅ Unit Testing | Catch2 framework, covers invalid insertions, edge cases, and graph scenarios |

---

## 🗂 Project Structure

DSA_Project3new/  
├─ build/ # CMake build folder  
├─ data/ # CSV files for edges and classes  
├─ src/  
│ ├─ student.h / student.cpp  
│ ├─ Graph.h / Graph.cpp  
│ └─ main.cpp  
├─ test/  
│ └─ test.cpp # Catch2 unit tests  
├─ CMakeLists.txt  
└─ README.md  

![Structure](images/structure.png)


## 📂 Project Source Files Overview

This project contains the following main source and header files:

| File | Type | Purpose |
|------|------|---------|
| `src/main.cpp` | 📝 CPP | Entry point of the program. Loads CSV data, parses user commands, calls Graph and Student methods, and prints results. |
| `src/Graph.h` / `src/Graph.cpp` | 📊 Header & CPP | Implements the Graph data structure representing UF campus locations. Handles edges, connectivity, shortest paths (Dijkstra), closures, MST for student zones, and related operations. |
| `src/student.h` / `src/student.cpp` | 👤 Header & CPP | Implements the Student class. Manages student information, class enrollments, UFID validation, and helper methods for data integrity. |
| `test/test.cpp` | 🧪 CPP | Unit tests using Catch2 framework. Tests student operations, edge cases, graph connectivity, shortest path, and class management functions. |

### 🔹 Notes
- Each `.h` file contains declarations for its corresponding `.cpp` implementation file.  
- `main.cpp` should not contain Graph or Student implementation code—only usage and command parsing.  
- `test.cpp` is independent and used exclusively for automated testing with Catch2.  




- ## ⚙️ Dependencies  
- **C++17** or later  
- **CMake** 3.16+  
- **Catch2** (integrated via CMake FetchContent)  
- Compatible with **Windows (MinGW/MSVC)**, **Linux**, and **macOS**

- ## 🏗 Building the Project  
- Open terminal in project root:  
  ```powershell
  cd DSA_Project3new
  ```
- Remove old build folder (if exists):  
  ```powershell
  Remove-Item -Recurse -Force build
  ```
- Create new build folder & configure CMake:  
  ```powershell
  mkdir build
  cd build
  cmake ..
  ```
- Build the project:  
  ```powershell
  cmake --build .
  ```
- ✅ Executables (`Main.exe` and `test.exe`) will appear in the **build** folder.

- ## ▶️ Running the Application  
- From the build folder:  
  ```powershell
  .\Main.exe
  ```
- Example commands:  
  ```text
  insert "Josh Smith" 12345678 5 2 COP3530 MAC2311
  dropClass 12345678 COP3530
  replaceClass 12345678 MAC2311 COP3503
  ```

- ## 🧪 Running Unit Tests  
- From the build folder:  
  ```powershell
  .\test.exe
  ```
- Catch2 framework output includes:  
  - Invalid student insertions (UFID, name, class code)  
  - Edge cases for removing non-existent students/classes  
  - Drop, remove, replace class commands  
  - Graph reachability scenarios after edge closures  

- ## 📜 Commands Supported  
- **Student Management**
  - `insert "NAME" UFID RESIDENCE N CLASS_1 ... CLASS_N` → Add a student with N classes  
  - `dropClass UFID CLASSCODE` → Remove a class from student  
  - `replaceClass UFID OLD_CLASS NEW_CLASS` → Replace a student's class  
  - `removeStudent UFID` → Remove a student completely  
- **Graph Management**
  - `shortestPath START END` → Compute shortest path between two locations  
  - `toggleEdgesClosure {EDGE_LIST}` → Simulate closed buildings/paths  
  - `isConnected START END` → Check if two locations are connected  

- ## ⚙️ Implementation Details  
- **Graph**: adjacency list, `Edge` struct (destination, weight, open/closed)  
- **Shortest path**: Dijkstra algorithm (`Graph::dijkstra`)  
- **Student**: `Student` class stores UFID, name, residence, classes  
- **Validation**: `Student::isValidUFID`, `isValidName`, `isValidClassCode`  
- **Student Zone**: MST-based minimal travel cost (`computeStudentZoneCost`)  

- ## ⚠️ Notes & Edge Cases  
- Students with zero classes are automatically removed  
- Adding duplicate UFID or invalid UFID/name/class fails  
- Dropping/replacing non-existent classes returns false  
- Shortest path returns `-1` if unreachable  
- CSV loading skips invalid rows  

- ## 🏆 References  
- Assignment specifications from **UF COP3530**  
- **Catch2 Testing Framework**
