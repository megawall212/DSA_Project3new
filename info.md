
## 🧪 Test Environment Template
- Project 3 Template [Link](https://github.com/COP3530/P3-Catch-Template)  
  - Like Project 1, this template sets up an environment with Catch for developing and testing your code.  
  - Works with **VSCode** and **CLion**, and can be used remotely in a **GitHub Codespace**.  

---

## 📄 Problem Statement
The University of Florida plans to develop an application to help students access key information about:
- Classes
- Residences
- Study locations

Due to campus construction and major events such as football games or guest lectures:
- Some roads and sidewalks may be closed.
- Shorter routes may become congested, making longer paths with less traffic faster.

> Note: All numbers between nodes are **random** and not accurate distances.

**Task:**  
Develop a **C++ program** to manage on-campus roads and sidewalks connecting student residences and classrooms, supporting:
- Student and logistic management
- Adding/removing students
- Managing classes
- Opening/closing roads
- Finding shortest routes from residences to classes, even with closures

Responsibilities:
- Designing interfaces, modules, and functions for commands
- Loading and managing campus data
- Parsing and validating user input
- Implementing `main()` that processes commands
- Testing the program under all constraints

---

## 🏗 Functionality

### 1️⃣ Load Data
- Load **two CSV files** from `data/`:
  1. `edges.csv` – Edge list with travel time between locations
     - Columns: `LocationID_1`, `LocationID_2`, `Name_1`, `Name_2`, `Time`
  2. `classes.csv` – Maps class code to location ID and times
     - Columns: `ClassCode`, `LocationID`, `Start Time`, `End Time`
- [CSV loading guide](https://medium.com/ryan/reading-csv-in-cpp)  
- [CSV file management in C++](https://www.geeksforgeeks.org/csv-file-management-using-cpp/)

> **Autograder requires this structure:**
your_root_dir/src/main.cpp (and other .h / .cpp files)
your_root_dir/data/edges.csv
your_root_dir/data/classes.csv


---

### 2️⃣ Command Parsing & Class Representation
- Use `std::cin` or `std::getline` to read commands
- Execute corresponding functions and print results using `std::cout`
- Operates on:
  - `Graph` class
  - Additional data structures for students/classes

**Supported Commands:**

#### Student Management
- `insert "NAME" UFID RESIDENCE N CLASS_1 ... CLASS_N` – Add student
- `remove UFID` – Remove student
- `dropClass UFID CLASSCODE` – Drop class for student
- `replaceClass UFID OLD_CLASS NEW_CLASS` – Replace a class
- `removeClass CLASSCODE` – Remove class from all students

#### Graph Management
- `toggleEdgesClosure N LOC_ID_X LOC_ID_Y ...` – Close/open N edges
- `checkEdgeStatus LOC_ID_X LOC_ID_Y` – Prints `open`, `closed`, or `DNE`
- `isConnected LOC_ID_1 LOC_ID_2` – Checks connectivity
- `printShortestEdges UFID` – Prints shortest walking time to each class
- `printStudentZone UFID` – Prints MST cost connecting residence + classes

#### Extra Credit
- `verifySchedule UFID` – Checks if student can reach classes in allotted time

---

### 3️⃣ Graph Data Structure
- Use adjacency list with **Edge struct**: destination, weight, open/closed
- Shortest path via **Dijkstra’s algorithm**
- Student class stores: UFID, name, residence, classes
- Validation:
  - UFID: 8 digits, unique
  - Name: only letters and spaces
  - Class Code: 3 capital letters + 4 digits
  - 1–6 classes per student
- Student zone: MST-based minimal travel cost
- CSV loading skips invalid rows

---

### 4️⃣ Testing Constraints
- 1 ≤ Number of commands ≤ 1000
- 6 ≤ Length of a command ≤ 1000
- Each command runs on a single line
- Data validation prints `"unsuccessful"` if invalid

---

