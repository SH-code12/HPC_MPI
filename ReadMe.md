# Parallel Maximum Finder Program

## Problem Statement

This program implements a parallel algorithm using MPI (Message Passing Interface) to find the maximum number in a given array. It follows the **Master-Slave paradigm** where the master process distributes the work to slave processes, receives results from them, and computes the final maximum value.

### Master Process:
- **Reads the size of the array** and the array elements.
- **Distributes the work** among slave processes by sending them the size of the array and the assigned portion of the array.
- After each slave finishes its work, the **master process receives the maximum value and its index** from each slave.
- **Computes the final maximum value** and outputs it along with the index in the original array.

### Slave Process:
- Receives the **size of the array**, the **partition of the array**, and the **starting index**.
- Computes the **maximum value in its assigned subarray**.
- Sends the **computed maximum value** and its **index** back to the master process.

### Note:
- The size of the array may not be divisible by the number of processes, so the program ensures that the remaining elements are handled properly.

---

## Compilation and Execution

### Requirements:
- **MPI library installed** (e.g., OpenMPI or MPICH).

### Compile and run the Program:

1. compile the program, use the following command:

```bash
mpicc -o test MaxNumber.c

```

2. Run executable file **test**

bash
```
mpiexec -np 6 ./test
```
