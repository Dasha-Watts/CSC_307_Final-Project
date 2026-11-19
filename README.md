# CSC_307_Final-Project
Fall 2026 final project for CSC Data structures: Help Desk Ticket Manager 
# Helpdesk Ticket Manager

**Authors:** Sujit Khatri, Shidisha Watts, and Anthony Shircel  
**Team:** Tickets Titans  
**Course:** CSC 307 H003  
**Semester:** Fall 2026

Helpdesk Ticket Manager is a C++ command-line program for managing unresolved IT support tickets. It uses a custom binary max-heap to find the next ticket to address. Higher priorities come first. When two tickets have the same priority, the earlier ticket comes first.

## 1. Prerequisites and Environment

- A terminal and a C++ compiler that supports C++17.
- The build commands below use `clang++` on macOS.
- The program uses the C++ standard library and requires no third-party libraries.
- Git is optional if you download or extract the project files directly.

## 2. Installation and Setup

Download or clone the team repository, or extract the submitted ZIP. Open a terminal in the project folder. Keep `main.cpp`, `TicketMaxHeap.cpp`, `TicketStorage.cpp`, and the shared header files together in that folder.

## 3. Build Instructions

From the project folder, run:

```bash
clang++ -std=c++17 -Wall -Wextra -pedantic main.cpp TicketMaxHeap.cpp TicketStorage.cpp -o helpdesk
```

This creates an executable named `helpdesk`.

## 4. Execution Guide

From the same project folder, run:

```bash
./helpdesk
```

The program takes no command-line arguments. Use the numbered menu to add, search, update, and remove tickets, view the next ticket or all tickets, and save or load records. Priority values are **1 = Low**, **2 = Medium**, and **3 = High**. **View next ticket** displays the next ticket without removing it. **View all tickets** displays tickets in heap order, so the full list is not sorted by priority.

Choose **Save tickets** to write the current records to `tickets.csv` in the working directory. Choose **Load tickets** to restore saved records. The program prompts before replacing unsaved changes or exiting with unsaved changes.

## 5. Troubleshooting and Known Issues

- **Undefined symbols when building:** Compile all three `.cpp` files together and make sure the heap and storage implementations are present.
- **`./helpdesk` is missing:** Check that the build succeeded and that the terminal is in the project folder.
- **Cannot load `tickets.csv`:** Check that you have saved tickets and that you are running the program from the folder containing the CSV. If a load fails, the current tickets stay in memory.
- **Cannot save `tickets.csv`:** Check that you have permission to write to the working directory.
