# MemoryAllocation
Contiguous memory allocation using different strategies: First Fit, Best Fit, and Worst Fit.
This project is a simulation of contiguous memory allocation using different strategies: First Fit, Best Fit, and Worst Fit. The program manages a contiguous region of memory of size up to 1 MB, where addresses range from 0 to 1048575. It responds to user commands to request memory, release memory, and report the status of memory. This project is designed to demonstrate the principles of dynamic memory allocation and management in operating systems.

Features:
- Implements First Fit, Best Fit, and Worst Fit algorithms for memory allocation.
- Tracks allocated and free memory blocks, combining adjacent free blocks when memory is released.
- Compacts free memory into a single large hole to optimize space.
- Supports commands to request memory, release memory, compact memory, display memory status, and exit the program.

How to Run:
Compile the code using a C compiler.
Run the program with the initial memory size as a command line argument.
Enter commands interactively to manage memory.

Skills:
C Programming
Dynamic Memory Allocation
Operating Systems
Data Structures 
Command Line Interface (CLI) Handling

Technologies Used:
C, Command Line Interface (CLI)
