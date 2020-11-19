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
static List* pcbMessagesList;

// bool runningCheck(void* pItem, void* pComparisonArg) {
//     if (pItem == pComparisonArg) {
//         return true;
//     }
//     else {
//         return false;
//     }
// }

void PCB_init(List* P0list, List* P1list, List* P2list, List* sendWait, List* receiveWait, List* blockedList, List* messagesList) {
    pcbP0List = P0list;
    pcbP1List = P1list;
    pcbP2List = P2list;
    pcbSendWaitList = sendWait;
    pcbReceiveWaitList = receiveWait;
    pcbMessagesList = messagesList;
    Create(2);                          // Create Init Process with Priority 0
}

void PCB_addToReadyList(List* readyList, int pid, int priority, int processState, char* messageStored) {
    pcb_t* processPCB;
    processPCB->pid = pid;
    processPCB->priority = priority;
    processPCB->processState = processState;
    processPCB->messageStored = messageStored;
    List_add(readyList, processPCB);
    printf("Process ID %d has been added to the blocked list \n", pid);
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
                    printf("Cant exit init process. Other lists are still populated\n");
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

void PCB_addMessageToList(int destinationPid, int sourcePid, char* messageString) {

    pcb_messages* messageIntended;
    messageIntended->destinationPID = destinationPid;
    messageIntended->sourcePID = sourcePid;
    messageIntended->message = messageString;
    List_add(pcbMessagesList, messageIntended);

}

void Send(int pid, char* messageString) {
    int processFound = 0;               // Turns 1 if intended process is found

    

    if (processFound == 0 && List_count(pcbP0List) != 0) {
        // Searching High priority list for intended process
        pcbP0List->pCurrentNode = pcbP0List->pFirstNode;
        for (int i = 0; i < List_count(pcbP0List); i++) {
            pcb_t* currentItem = pcbP0List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {
                
                

                currentItem->processState = 2;
                PCB_addToBlockedList(currentItem->pid, currentItem->priority, currentItem->processState, currentItem->messageStored);
                List_remove(pcbP0List);

                PCB_addMessageToList(pid, currentItem->pid, messageString);
                printf ("Message sent to storage for process with PID %d\n", pid);
                
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
            if ( currentItem->processState == 0) {
                
                

                currentItem->processState = 2;
                PCB_addToBlockedList(currentItem->pid, currentItem->priority, currentItem->processState, currentItem->messageStored);

                PCB_addMessageToList(pid, currentItem->pid, messageString);
                printf ("Message sent to storage for process with PID %d\n", pid);

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
            if ( currentItem->processState == 0) {
                

                currentItem->processState = 2;
                PCB_addToBlockedList(currentItem->pid, currentItem->priority, currentItem->processState, currentItem->messageStored);

                PCB_addMessageToList(pid, currentItem->pid, messageString);
                printf ("Message sent to storage for process with PID %d\n", pid);

                processFound = 1;
                break;
            }
            List_next(pcbP2List);
        }
    }

    if (processFound == 0) {
        printf("No process with PID %d found\n", pid);
    }

}

void Receive() {

    int runningProcessFound = 0;
    int receiveProcessPid = -1;
    int runningProcessSource = -1;

    if (runningProcessFound == 0 && List_count(pcbP0List) != 0) {
        // Searching High priority list for running process
        pcbP0List->pCurrentNode = pcbP0List->pFirstNode;
        for (int i = 0; i < List_count(pcbP0List); i++) {
            pcb_t* currentItem = pcbP0List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {
                receiveProcessPid = currentItem->pid;
                runningProcessFound = 1;
                runningProcessSource = 0;
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
                receiveProcessPid = currentItem->pid;
                runningProcessFound = 1;
                runningProcessSource = 1;
                break;
            }
            pcbP1List->pCurrentNode = pcbP1List->pCurrentNode->pNext;
        }
    }

    if (runningProcessFound = 0 && List_count(pcbP2List) != 0) {
        // Searching Low priority list for running process
        pcbP2List->pCurrentNode = pcbP2List->pFirstNode;
        for (int i = 0; i < List_count(pcbP2List); i++) {
            pcb_t* currentItem = pcbP2List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {
                receiveProcessPid = currentItem->pid;
                runningProcessFound = 1;
                runningProcessSource = 2;
                break;
            }
            pcbP2List->pCurrentNode = pcbP2List->pCurrentNode->pNext;
        }
    }

    char* messageStorageForMove;
    int messageForMoveSourcePid;

    if (runningProcessFound == 0) {
        printf("No running process found to receive.\n");
    }
    else {
        if (List_Count(pcbMessagesList) != 0)
        {
            pcbMessagesList->pCurrentNode = pcbMessagesList->pFirstNode;
            for (int i = 0; i < List_count(pcbMessagesList); i++) {
                pcb_messages* currentItem = pcbMessagesList->pCurrentNode->pItem;
                if ( currentItem->destinationPID == receiveProcessPid) {
                    messageStorageForMove = currentItem->message;
                    messageForMoveSourcePid = currentItem->sourcePID;
                    break;
                }
            pcbBlockedList->pCurrentNode = pcbBlockedList->pCurrentNode->pNext;
            }

            if (runningProcessSource == 0) {
                pcb_t* pcurrentItem = pcbP0List->pCurrentNode->pItem;
                pcurrentItem->messageStored = messageStorageForMove;
                List_remove(pcbMessagesList);
            }
            else if (runningProcessSource == 1) {
                pcb_t* pcurrentItem = pcbP1List->pCurrentNode->pItem;
                pcurrentItem->messageStored = messageStorageForMove;
                List_remove(pcbMessagesList);
            }
            else if (runningProcessSource == 2) {
                pcb_t* pcurrentItem = pcbP2List->pCurrentNode->pItem;
                pcurrentItem->messageStored = messageStorageForMove;
                List_remove(pcbMessagesList);
            }
        }
        else {
            printf("No message waiting for running process, sending process to blocked list.\n");

            if (runningProcessSource == 0) {
                pcb_t* pcurrentItem = pcbP0List->pCurrentNode->pItem;
                PCB_addToBlockedList(pcurrentItem->pid, pcurrentItem->priority, pcurrentItem->processState, pcurrentItem->messageStored);
                List_remove(pcbP0List);
            }
            else if (runningProcessSource == 1) {
                pcb_t* pcurrentItem = pcbP1List->pCurrentNode->pItem;
                PCB_addToBlockedList(pcurrentItem->pid, pcurrentItem->priority, pcurrentItem->processState, pcurrentItem->messageStored);
                List_remove(pcbP1List);
            }
            else if (runningProcessSource == 2) {
                pcb_t* pcurrentItem = pcbP2List->pCurrentNode->pItem;
                PCB_addToBlockedList(pcurrentItem->pid, pcurrentItem->priority, pcurrentItem->processState, pcurrentItem->messageStored);
                List_remove(pcbP2List);
            }
            else
            {
                printf("Invalid process source (Error from receive())\n");
            }
            
        }
    }

}

void Reply(int pid, char* messageString) {

    int processFound = 0;    
     if (List_count(pcbBlockedList) != 0) {
        // Searching Blocked list for intended process
        pcbBlockedList->pCurrentNode = pcbBlockedList->pFirstNode;
        for (int i = 0; i < List_count(pcbBlockedList); i++) {
            pcb_t* currentItem = pcbBlockedList->pCurrentNode->pItem;
            if ( currentItem->pid == pid) {
                currentItem->messageStored = messageString;
                currentItem->processState = 1;
                if (currentItem->priority == 0) {
                    PCB_addToReadyList(pcbP0List, currentItem->pid, currentItem->priority, currentItem->processState, currentItem->messageStored);
                }
                else if (currentItem->priority == 1) {
                    PCB_addToReadyList(pcbP1List, currentItem->pid, currentItem->priority, currentItem->processState, currentItem->messageStored);
                }
                else if (currentItem->priority == 2)
                {
                    PCB_addToReadyList(pcbP2List, currentItem->pid, currentItem->priority, currentItem->processState, currentItem->messageStored);
                }

                List_remove(pcbBlockedList);
                processFound = 1;
                break;
            }
            pcbP0List->pCurrentNode = pcbP0List->pCurrentNode->pNext;
        }
    }

    if (processFound == 0) {
        printf("No process with pid %d can be found in the blocked list.\n", pid);
    }
    else {
        printf("Reply to process pid %d has been sent sucessfully, pid %d has been sent back to the ready queue.\n", pid);
    }
}

void NewSemaphore(int semaphoreId, int value) {

    if (value < 0 || value > 4) {
        printf("Invalid semaphore value! Can only be 0 to 4.\n");
        return;
    }

    semaphore* newSemaphore;
    List* semaphoreProcessList; List_create(semaphoreProcessList);
    newSemaphore->semaphoreId = semaphoreId;
    newSemaphore->value = value;
    newSemaphore->pList = semaphoreProcessList;
}

void SemaphoreP(int semaphoreId);

void SemaphoreV(int semaphoreId);

void Procinfo(int pid);

void Totalinfo();