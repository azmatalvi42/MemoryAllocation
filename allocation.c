#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_MEMORY_SIZE 1048576  // Maximum memory size (1 MB)

// Memory Block Struct
typedef struct {
    int startAddress; // Starting address of the block
    int blockSize;    // Size of the block
    bool isFree;      
    int processID;    // ID of the process occupying the block
} MemoryBlock;

MemoryBlock memoryBlocks[MAX_MEMORY_SIZE]; // Array to hold memory blocks
int totalBlocks = 1; // Number of blocks

// Initialize memory with a single free block of input size
void initialize_memory(int size) {
    memoryBlocks[0].startAddress = 0;
    memoryBlocks[0].blockSize = size; //input value
    memoryBlocks[0].isFree = true;
    memoryBlocks[0].processID = -1;
}

// Extract process ID from string P0, P1, P2
int get_process_id(const char* processIDStr) {
    if (processIDStr[0] == 'P') {
        return atoi(processIDStr + 1); // Convert the number part to integer
    }
    return -1;
}

// Allocate memory using the specified strategy: First Fit, Best Fit, or Worst Fit
int allocate_memory(int requestSize, int processID, char strategy) {
    int chosenBlockIndex = -1;

    // Determine which block to use based on the strategy
    if (strategy == 'F') {
        // First Fit: Find the first block that is free and large enough
        for (int i = 0; i < totalBlocks; i++) {
            if (memoryBlocks[i].isFree && memoryBlocks[i].blockSize >= requestSize) {
                chosenBlockIndex = i;
                break;
            }
        }
    } else if (strategy == 'B') {
        // Best Fit: Find the smallest free block that is large enough
        int bestFitSize = MAX_MEMORY_SIZE + 1;  // Initialize to a large value
        for (int i = 0; i < totalBlocks; i++) {
            if (memoryBlocks[i].isFree && memoryBlocks[i].blockSize >= requestSize && memoryBlocks[i].blockSize < bestFitSize) {
                chosenBlockIndex = i;
                bestFitSize = memoryBlocks[i].blockSize;
            }
        }
    } else if (strategy == 'W') {
        // Worst Fit: Find the largest free block
        int worstFitSize = -1;
        for (int i = 0; i < totalBlocks; i++) {
            if (memoryBlocks[i].isFree && memoryBlocks[i].blockSize >= requestSize && memoryBlocks[i].blockSize > worstFitSize) {
                chosenBlockIndex = i;
                worstFitSize = memoryBlocks[i].blockSize;
            }
        }
    }

    // If no suitable block,return an error
    if (chosenBlockIndex == -1) {
        printf("No hole of sufficient size\n");
        return -1;
    }

    MemoryBlock *chosenBlock = &memoryBlocks[chosenBlockIndex];

    // Allocate memory and split block if necessary
    if (chosenBlock->blockSize > requestSize) {
        for (int j = totalBlocks; j > chosenBlockIndex + 1; j--) {
            memoryBlocks[j] = memoryBlocks[j - 1];
        }
        memoryBlocks[chosenBlockIndex + 1].startAddress = chosenBlock->startAddress + requestSize;
        memoryBlocks[chosenBlockIndex + 1].blockSize = chosenBlock->blockSize - requestSize;
        memoryBlocks[chosenBlockIndex + 1].isFree = true;
        memoryBlocks[chosenBlockIndex + 1].processID = -1;
        totalBlocks++;
    }

    chosenBlock->blockSize = requestSize;
    chosenBlock->isFree = false;
    chosenBlock->processID = processID;

    return chosenBlock->startAddress;
}

// Release memory that has been allocated to a process
int release_memory(int processID) {
    printf("Releasing memory for process P%d\n", processID);
    for (int i = 0; i < totalBlocks; i++) {
        if (memoryBlocks[i].processID == processID && !memoryBlocks[i].isFree) {
            memoryBlocks[i].isFree = true;
            // Merge with previous block if free
            if (i > 0 && memoryBlocks[i - 1].isFree) {
                memoryBlocks[i - 1].blockSize += memoryBlocks[i].blockSize;
                for (int j = i; j < totalBlocks - 1; j++) {
                    memoryBlocks[j] = memoryBlocks[j + 1];
                }
                totalBlocks--;
                i--;
            }
            // Merge with next block if free
            if (i < totalBlocks - 1 && memoryBlocks[i + 1].isFree) {
                memoryBlocks[i].blockSize += memoryBlocks[i + 1].blockSize;
                for (int j = i + 1; j < totalBlocks - 1; j++) {
                    memoryBlocks[j] = memoryBlocks[j + 1];
                }
                totalBlocks--;
            }
            return 0;
        }
    }
    return -1;
}

// Compact the set of holes into one larger hole 
void compact_memory() {
    int freeIndex = 0;
    for (int i = 0; i < totalBlocks; i++) {
        if (!memoryBlocks[i].isFree) {
            if (i != freeIndex) {
                memoryBlocks[freeIndex] = memoryBlocks[i];
                memoryBlocks[freeIndex].startAddress = (freeIndex == 0) ? 0 : memoryBlocks[freeIndex - 1].startAddress + memoryBlocks[freeIndex - 1].blockSize;
            }
            freeIndex++;
        }
    }
    memoryBlocks[freeIndex].startAddress = (freeIndex == 0) ? 0 : memoryBlocks[freeIndex - 1].startAddress + memoryBlocks[freeIndex - 1].blockSize;
    memoryBlocks[freeIndex].blockSize = MAX_MEMORY_SIZE - memoryBlocks[freeIndex].startAddress;
    memoryBlocks[freeIndex].isFree = true;
    memoryBlocks[freeIndex].processID = -1;
    totalBlocks = freeIndex + 1;
}

// Output the status of memory allocation
void display_status() {
    int allocatedMemory = 0;
    int freeMemory = 0;

    // Calculate allocated and free memory
    printf("Partitions [Allocated memory= ");
    for (int i = 0; i < totalBlocks; i++) {
        if (!memoryBlocks[i].isFree) {
            allocatedMemory += memoryBlocks[i].blockSize;
        } else {
            freeMemory += memoryBlocks[i].blockSize;
        }
    }
    printf("%d]:\n", allocatedMemory);

    // Display allocated blocks
    for (int i = 0; i < totalBlocks; i++) {
        if (!memoryBlocks[i].isFree) {
            printf("Address [%d:%d] Process P%d\n", memoryBlocks[i].startAddress, memoryBlocks[i].startAddress + memoryBlocks[i].blockSize - 1, memoryBlocks[i].processID);
        }
    }

    // Display free blocks
    printf("\nHoles [Free memory= %d]:\n", freeMemory);
    for (int i = 0; i < totalBlocks; i++) {
        if (memoryBlocks[i].isFree) {
            printf("Address [%d:%d] len = %d\n", memoryBlocks[i].startAddress, memoryBlocks[i].startAddress + memoryBlocks[i].blockSize - 1, memoryBlocks[i].blockSize);
        }
    }
}

// Main function to handle user commands
int main(int argc, char *argv[]) {
    // Check if initial memory size is provided
    if (argc != 2) {
        printf("Usage: %s <initial_memory_size>\n", argv[0]);
        return 1;
    }

    int initialMemorySize = atoi(argv[1]);
    if (initialMemorySize <= 0) {
        printf("Invalid memory size\n");
        return 1;
    }

    initialize_memory(initialMemorySize); // Initialize memory with the provided size
    printf("The Best Fit approach has been implemented and the allocated %d bytes of memory.\n", initialMemorySize);

    char command[20];
    while (true) {
        printf("allocator> ");
        scanf("%s", command);

        if (strcmp(command, "RQ") == 0) {
            // Request memory allocation
            char processIDStr[10];
            int size;
            char strategy;
            scanf("%s %d %c", processIDStr, &size, &strategy);
            int processID = get_process_id(processIDStr);
            if (processID == -1) {
                printf("Invalid process ID format\n");
                continue;
            }
            int address = allocate_memory(size, processID, strategy);
            if (address != -1) {
                printf("Successfully allocated %d to process %s\n", size, processIDStr);
            } else {
                printf("Failed to allocate %d to process %s\n", size, processIDStr);
            }
        } else if (strcmp(command, "RL") == 0) {
            // Release memory
            char processIDStr[10];
            scanf("%s", processIDStr);
            int processID = get_process_id(processIDStr);
            if (processID == -1) {
                printf("Invalid process ID format\n");
                continue;
            }
            if (release_memory(processID) == 0) {
                printf("Successfully released memory for process %s\n", processIDStr);
            } else {
                printf("Failed to release memory for process %s\n", processIDStr);
            }
        } else if (strcmp(command, "C") == 0) {
            // Compact memory
            compact_memory();
            printf("Compaction process is successful\n");
        } else if (strcmp(command, "Status") == 0) {
            // Display memory status
            display_status();
        } else if (strcmp(command, "Exit") == 0) {
            // Exit the program
            break;
        } else {
            printf("Unknown command\n");
        }
    }

    return 0;
}

