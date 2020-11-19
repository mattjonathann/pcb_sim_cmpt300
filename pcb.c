#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include "list.h"
#include "pcb.h"

static int processIDcount = 0;          // PID 0 reserved for Init process
static List* pcbP0List;
static List* pcbP1List;
static List* pcbP2List;
static List* pcbBlockedList;
static List* pcbSendWaitList;
static List* pcbReceiveWaitList;

// bool runningCheck(void* pItem, void* pComparisonArg) {
//     if (pItem == pComparisonArg) {
//         return true;
//     }
//     else {
//         return false;
//     }
// }

void PCB_init(List* P0list, List* P1list, List* P2list, List* sendWait, List* receiveWait, List* blockedList) {
    pcbP0List = P0list;
    pcbP1List = P1list;
    pcbP2List = P2list;
    pcbSendWaitList = sendWait;
    pcbReceiveWaitList = receiveWait;
    Create(2);                          // Create Init Process with Priority 0
}

void PCB_addToBlockedList(int pid, int priority, int processState, char* messageStored ) {
    pcb_t* processPCB;
    processPCB->pid = pid;
    processPCB->priority = priority;
    processPCB->processState = processState;
    processPCB->messageStored = messageStored;
    List_add(pcbBlockedList, processPCB);
    printf("Process ID %d has been added to the blocked list \n", pid);
}

int Create(int priority) {

    if (priority < 0 || priority > 2) {
        printf("Priority %d is invalid.", priority);
        return EXIT_FAILURE;
    }

    pcb_t* processPCB;
    processPCB->priority = priority;
    processPCB->pid = processIDcount;

    if (processPCB->pid == 0) {
        processPCB->processState = 0;       // Running state on init
    }
    else {
        processPCB->processState = 1;       // Ready state on new processes
    }
    
    
    processIDcount++;

    if(priority == 0) {
        List_add(pcbP0List, processPCB);
        printf("Process ID %d has been sucessfully added to priority list %d\n", processPCB->pid, priority);
    }
    else if(priority = 1) {
        List_add(pcbP1List, processPCB);
        printf("Process ID %d has been sucessfully added to priority list %d\n", processPCB->pid, priority);
    }
    else if (priority == 2) {
        List_add(pcbP2List, processPCB);
        if ( processPCB->pid != 0) {
            printf("Process ID %d has been sucessfully added to priority list %d\n", processPCB->pid, priority);
        }
        else {
            printf("Init process has been created and added to priority list 2\n");
        }
    }
    

    return processPCB->pid;
}

int Fork()
 {
    int processPriority;
    int runningProcessFound = 0;               // Turns 1 if running process is found
    int forkedProcessPid;


    // Searching High priority list for running process
    pcbP0List->pCurrentNode = pcbP0List->pFirstNode;
    for (int i = 0; i < List_count(pcbP0List); i++) {
        pcb_t* currentItem = pcbP0List->pCurrentNode->pItem;
        if ( currentItem->processState == 0) {
            processPriority = currentItem->priority;
            Create(processPriority);
            pcb_t* copyItem = pcbP0List->pLastNode->pItem;
            copyItem->messageStored = currentItem->messageStored;
            copyItem->processState = currentItem->processState;
            forkedProcessPid = copyItem->pid;
            runningProcessFound = 1;
            break;
        }
        pcbP0List->pCurrentNode = pcbP0List->pCurrentNode->pNext;
    }

    if (runningProcessFound == 0) {
        // Searching Med priority list for running process
        pcbP1List->pCurrentNode = pcbP1List->pFirstNode;
        for (int i = 0; i < List_count(pcbP1List); i++) {
            pcb_t* currentItem = pcbP1List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {
                processPriority = currentItem->priority;
                Create(processPriority);
                pcb_t* copyItem = pcbP1List->pLastNode->pItem;
                copyItem->messageStored = currentItem->messageStored;
                copyItem->processState = currentItem->processState;
                forkedProcessPid = copyItem->pid;
                runningProcessFound = 1;
                break;
            }
            pcbP1List->pCurrentNode = pcbP1List->pCurrentNode->pNext;
        }
    }

    if (runningProcessFound = 0) {
        // Searching Low priority list for running process
        pcbP2List->pCurrentNode = pcbP2List->pFirstNode;
        for (int i = 0; i < List_count(pcbP2List); i++) {
            pcb_t* currentItem = pcbP2List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {
                processPriority = currentItem->priority;
                Create(processPriority);
                pcb_t* copyItem = pcbP2List->pLastNode->pItem;
                copyItem->messageStored = currentItem->messageStored;
                copyItem->processState = currentItem->processState;
                forkedProcessPid = copyItem->pid;
                runningProcessFound = 1;
                break;
            }
            pcbP2List->pCurrentNode = pcbP2List->pCurrentNode->pNext;
        }
    }

    if (runningProcessFound == 0) {
        printf("Failure to Fork, no running process available.\n");
    }
    else {
        printf("Successfully forked current running process. PID: %d\n", forkedProcessPid);
        return forkedProcessPid;
    }

 }

void Kill(int pid) {
    int processFound = 0;               // Turns 1 if intended process is found

    if (processFound == 0 && List_count(pcbP0List) != 0) {
        // Searching High priority list for intended process
        pcbP0List->pCurrentNode = pcbP0List->pFirstNode;
        for (int i = 0; i < List_count(pcbP0List); i++) {
            pcb_t* currentItem = pcbP0List->pCurrentNode->pItem;
            if ( currentItem->pid == pid) {
                List_remove(pcbP0List);
                printf ("Process from P0 with PID %d killed sucessfully.\n", pid);
                processFound = 1;
                break;
            }
            List_next(pcbP0List);
        }
    }

    if (processFound == 0 && List_count(pcbP1List) != 0) {
        // Searching Med priority list for intended process
        pcbP1List->pCurrentNode = pcbP1List->pFirstNode;
        for (int i = 0; i < List_count(pcbP1List); i++) {
            pcb_t* currentItem = pcbP1List->pCurrentNode->pItem;
            if ( currentItem->pid == pid) {
                List_remove(pcbP1List);
                printf ("Process from P1 with PID %d killed sucessfully.\n", pid);
                processFound = 1;
                break;
            }
            List_next(pcbP1List);
        }
    }

    if (processFound == 0 && List_count(pcbP2List) != 0) {
        // Searching Low priority list for intended process
        pcbP2List->pCurrentNode = pcbP2List->pFirstNode;
        for (int i = 0; i < List_count(pcbP2List); i++) {
            pcb_t* currentItem = pcbP2List->pCurrentNode->pItem;
            if ( currentItem->pid == pid) {
                List_remove(pcbP2List);
                printf ("Process from P2 with PID %d killed sucessfully.\n", pid);
                processFound = 1;
                break;
            }
            List_next(pcbP2List);
        }
    }

    if (processFound == 0 && List_count(pcbBlockedList) != 0) {
        // Searching blocked list for intended process
        pcbBlockedList->pCurrentNode = pcbBlockedList->pFirstNode;
        for (int i = 0; i < List_count(pcbBlockedList); i++) {
            pcb_t* currentItem = pcbBlockedList->pCurrentNode->pItem;
            if ( currentItem->pid == pid) {
                List_remove(pcbBlockedList);
                printf ("Process from BlockedList with PID %d killed sucessfully.\n", pid);
                processFound = 1;
                break;
            }
            List_next(pcbBlockedList);
        }
    }

    if (processFound == 0) {
        printf("Process with pid %d not found.\n ", pid);
    }
}

void Exit() {
      int runningProcessFound = 0;               // Turns 1 if running process is found
      int processSource;

    if (runningProcessFound == 0 && List_count(pcbP0List) != 0) {
        // Searching High priority list for running process
        pcbP0List->pCurrentNode = pcbP0List->pFirstNode;
        for (int i = 0; i < List_count(pcbP0List); i++) {
            pcb_t* currentItem = pcbP0List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {
                List_remove(pcbP0List);
                printf ("Running Process from P0 exited sucessfully.\n");
                runningProcessFound = 1;
                processSource = 0;
                break;
            }
            pcbP0List->pCurrentNode = pcbP0List->pCurrentNode->pNext;
        }
    }

    if (runningProcessFound == 0 && List_count(pcbP1List) != 0) {
        // Searching Med priority list for running process
        pcbP1List->pCurrentNode = pcbP1List->pFirstNode;
        for (int i = 0; i < List_count(pcbP1List); i++) {
            pcb_t* currentItem = pcbP1List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {
                List_remove(pcbP1List);
                printf ("Running Process from P1 exited sucessfully.\n");
                runningProcessFound = 1;
                processSource = 1;
                break;
            }
            pcbP1List->pCurrentNode = pcbP1List->pCurrentNode->pNext;
        }
    }

    if (runningProcessFound == 0 && List_count(pcbP2List) != 0) {
        // Searching Low priority list for running process
        pcbP2List->pCurrentNode = pcbP2List->pFirstNode;
        for (int i = 0; i < List_count(pcbP2List); i++) {
            pcb_t* currentItem = pcbP2List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {

                if (currentItem->pid == 0 && List_count(pcbP0List) == 0 && List_count(pcbP1List) == 0 && List_count(pcbP2List) == 0) {
                    List_remove(pcbP2List);
                    printf("Running init process exited successfully\n");
                    runningProcessFound = 1;
                    processSource = 2;
                    return;
                }
                else if (currentItem->pid == 0) {
                    printf("Cant exit init process. Other lists are still populated\n")
                }

                List_remove(pcbP2List);
                printf ("Running Process from P2 exited sucessfully.\n");
                runningProcessFound = 1;
                processSource = 2;
                break;
            }
            pcbP2List->pCurrentNode = pcbP2List->pCurrentNode->pNext;
        }
    }

    int processID_number;

    if (runningProcessFound == 0) {
        printf("Failure to exit process.\n");
    }
    else {
        if (processSource == 0) {
            pcb_t* currItem = pcbP0List->pCurrentNode->pItem;
            processID_number = currItem->pid;

            printf("Process %d will now have control of the CPU\n");
        }
        // TODO: SCHEDULING
    }

}

void Quantum();

void Send(int pid, char* messageString) {
    int processFound = 0;               // Turns 1 if intended process is found

    if (processFound == 0 && List_count(pcbP0List) != 0) {
        // Searching High priority list for intended process
        pcbP0List->pCurrentNode = pcbP0List->pFirstNode;
        for (int i = 0; i < List_count(pcbP0List); i++) {
            pcb_t* currentItem = pcbP0List->pCurrentNode->pItem;
            if ( currentItem->pid == pid) {
                currentItem->messageStored = messageString;
                printf ("Message sent succesfully to process from P0 with PID %d\n", pid);

                currentItem->processState = 2;
                PCB_addToBlockedList(currentItem->pid), currentItem->priority, currentItem->processState, currentItem->messageStored;
                
                processFound = 1;
                break;
            }
            List_next(pcbP0List);
        }
    }

    if (processFound == 0 && List_count(pcbP1List) != 0) {
        // Searching Med priority list for intended process
        pcbP1List->pCurrentNode = pcbP1List->pFirstNode;
        for (int i = 0; i < List_count(pcbP1List); i++) {
            pcb_t* currentItem = pcbP1List->pCurrentNode->pItem;
            if ( currentItem->pid == pid) {
                currentItem->messageStored = messageString;
                printf ("Message sent succesfully to process from P1 with PID %d\n", pid);

                currentItem->processState = 2;
                PCB_addToBlockedList(currentItem->pid, currentItem->priority, currentItem->processState, currentItem->messageStored);


                processFound = 1;
                break;
            }
            List_next(pcbP1List);
        }
    }

    if (processFound == 0 && List_count(pcbP2List) != 0) {
        // Searching Low priority list for intended process
        pcbP2List->pCurrentNode = pcbP2List->pFirstNode;
        for (int i = 0; i < List_count(pcbP2List); i++) {
            pcb_t* currentItem = pcbP2List->pCurrentNode->pItem;
            if ( currentItem->pid == pid) {
                currentItem->messageStored = messageString;
                printf ("Message sent succesfully to process from P2 with PID %d\n", pid);

                currentItem->processState = 2;
                PCB_addToBlockedList(currentItem->pid, currentItem->priority, currentItem->processState, currentItem->messageStored);

                processFound = 1;
                break;
            }
            List_next(pcbP2List);
        }
    }

    if (processFound == 0 && List_count(pcbBlockedList) != 0) {
        // Searching blocked list for intended process
        pcbBlockedList->pCurrentNode = pcbBlockedList->pFirstNode;
        for (int i = 0; i < List_count(pcbBlockedList); i++) {
            pcb_t* currentItem = pcbBlockedList->pCurrentNode->pItem;
            if ( currentItem->pid == pid) {
                currentItem->messageStored = messageString;
                printf ("Message sent succesfully to process from BlockedList with PID %d\n", pid);

                currentItem->processState = 2;
                PCB_addToBlockedList(currentItem->pid, currentItem->priority, currentItem->processState, currentItem->messageStored);

                processFound = 1;
                break;
            }
            List_next(pcbBlockedList);
        }
    }

    

}

void Receive();

void Reply(int pid, char* messageString);

void NewSemaphore(int semaphoreId, int value);

void SemaphoreP(int semaphoreId);

void SemaphoreV(int semaphoreId);

void Procinfo(int pid);

void Totalinfo();