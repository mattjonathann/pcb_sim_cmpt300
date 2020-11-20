#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <stdbool.h>
#include "list.h"
#include "pcb.h"
#include "main.h"
#define MAX_LEN 40

static int processIDcount = 0;          // PID 0 reserved for Init process
static List* pcbP0List;
static List* pcbP1List;
static List* pcbP2List;
static List* pcbBlockedList;
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

void PCB_moveRunningProcessToBack() {
    // Make initial process ready
    int processPID;
    char* processMessage;


    bool found = false;
    if (!found && List_count(pcbP0List) != 0) {
        pcbP0List->pCurrentNode = pcbP0List->pFirstNode;
        for (int i = 0; i < List_count(pcbP0List); i++) {
            pcb_t* currentItem = pcbP0List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {
                processMessage = malloc(MAX_LEN);
                processPID = currentItem->pid;
                List_remove(pcbP0List);
                PCB_addToReadyList(pcbP0List, processPID, 0, 1, processMessage);
                found = true;
                break;
            }
            pcbP0List->pCurrentNode = pcbP0List->pCurrentNode->pNext;
        }
    }
    if (!found && List_count(pcbP1List) != 0) {
        pcbP1List->pCurrentNode = pcbP1List->pFirstNode;
        for (int i = 0; i < List_count(pcbP1List); i++) {
            pcb_t* currentItem = pcbP1List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {
                processMessage = malloc(MAX_LEN);
                processPID = currentItem->pid;
                List_remove(pcbP1List);
                PCB_addToReadyList(pcbP0List, processPID, 1, 1, processMessage);
                found = true;
                break;
            }
            pcbP1List->pCurrentNode = pcbP1List->pCurrentNode->pNext;
        }
    }
    if (!found && List_count(pcbP2List) != 0) {
        pcbP2List->pCurrentNode = pcbP2List->pFirstNode;
        for (int i = 0; i < List_count(pcbP2List); i++) {
            pcb_t* currentItem = pcbP2List->pCurrentNode->pItem;
            if ( currentItem->processState == 0 && currentItem->pid != 0) {
                processMessage = malloc(MAX_LEN);
                processPID = currentItem->pid;
                List_remove(pcbP2List);
                PCB_addToReadyList(pcbP0List, processPID, 2, 1, processMessage);
                found = true;
                break;
            }
            pcbP2List->pCurrentNode = pcbP2List->pCurrentNode->pNext;
        }
    }
    if (!found) {
        printf("There are no ready processes waiting in any queue\n");
    }
}
void PCB_makeProcessRun() {
    bool found = false;
    if (!found && List_count(pcbP0List) != 0) {
        pcbP0List->pCurrentNode = pcbP0List->pFirstNode;
        for (int i = 0; i < List_count(pcbP0List); i++) {
            pcb_t* currentItem = pcbP0List->pCurrentNode->pItem;
            if ( currentItem->processState == 1) {
                currentItem->processState = 0;
                printf("Process pid %d from priority 0 queue is now running.\n", currentItem->pid);
                found = true;
                break;
            }
            pcbP0List->pCurrentNode = pcbP0List->pCurrentNode->pNext;
        }
    }
    if (!found && List_count(pcbP1List) != 0) {
        pcbP1List->pCurrentNode = pcbP1List->pFirstNode;
        for (int i = 0; i < List_count(pcbP1List); i++) {
            pcb_t* currentItem = pcbP1List->pCurrentNode->pItem;
            if ( currentItem->processState == 1) {
                currentItem->processState = 0;
                printf("Process pid %d from priority 1 queue is now running.\n", currentItem->pid);
                found = true;
                break;
            }
            pcbP1List->pCurrentNode = pcbP1List->pCurrentNode->pNext;
        }
    }
    if (!found && List_count(pcbP2List) != 0) {
        pcbP2List->pCurrentNode = pcbP2List->pFirstNode;
        for (int i = 0; i < List_count(pcbP2List); i++) {
            pcb_t* currentItem = pcbP2List->pCurrentNode->pItem;
            if ( currentItem->processState == 1) {
                currentItem->processState = 0;
                printf("Process pid %d from priority 2 queue is now running.\n", currentItem->pid);
                found = true;
                break;
            }
            pcbP2List->pCurrentNode = pcbP2List->pCurrentNode->pNext;
        }
    }
    if (!found) {
        printf("There are no ready processes waiting in any queue\n");
    }
}

void PCB_findCurrentRunningProcess() {
    bool runningProcessFound = false;

    if (runningProcessFound == false && List_count(pcbP0List) != 0) {
        // Searching High priority list for running process
        List_first(pcbP0List);
        for (int i = 0; i < List_count(pcbP0List); i++) {
            pcb_t* currentItem = pcbP0List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {
                printf("Currently Running Process is : %d", currentItem->pid);
                runningProcessFound = true;
                break;
            }
            List_next(pcbP0List);
        }
    }

    if (runningProcessFound == false && List_count(pcbP1List) != 0) {
        // Searching Med priority list for running process
        List_first(pcbP1List);
        for (int i = 0; i < List_count(pcbP1List); i++) {
            pcb_t* currentItem = pcbP1List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {
                printf("Currently Running Process is : %d", currentItem->pid);
                runningProcessFound = true;
                break;
            }
            List_next(pcbP1List);
        }
    }

    if (runningProcessFound = false && List_count(pcbP2List) != 0) {
        // Searching Low priority list for running process
        List_first(pcbP2List);
        for (int i = 0; i < List_count(pcbP2List); i++) {
            pcb_t* currentItem = pcbP2List->pCurrentNode->pItem;
            if (currentItem->processState == 0 && currentItem->pid == 0) {
                printf("Init process can't be running.\n");
            }
            else if ( currentItem->processState == 0) {
                printf("Currently Running Process is : %d", currentItem->pid);
                runningProcessFound = true;
                break;
            }
            List_next(pcbP2List);
        }
    }

}

void PCB_init(List* P0list, List* P1list, List* P2list, List* receiveWait, List* blockedList, List* messagesList) {
    pcbP0List = P0list;
    pcbP1List = P1list;
    pcbP2List = P2list;
    pcbReceiveWaitList = receiveWait;
    pcbMessagesList = messagesList;
    pcbBlockedList = blockedList;
    Create(2);                          // Create Init Process with Priority 0
}

void PCB_addToReadyList(List* readyList, int pid, int priority, int processState, char* messageStored) {
    pcb_t* processPCB = malloc(sizeof(pcb_t));
    processPCB->messageStored = malloc(MAX_LEN);
    processPCB->pid = pid;
    processPCB->priority = priority;
    processPCB->processState = processState;
    processPCB->messageStored = messageStored;
    List_append(readyList, processPCB);
    printf("Process ID %d has been added back to priority %d queue \n", pid, processPCB->priority);
}

void PCB_addToBlockedList(int pid, int priority, int processState, char* messageStored ) {
    pcb_t* processPCB = malloc(sizeof(pcb_t));
    processPCB->messageStored = malloc(MAX_LEN);
    processPCB->pid = pid;
    processPCB->priority = priority;
    processPCB->processState = processState;
    processPCB->messageStored = messageStored;
    List_append(pcbBlockedList, processPCB);
    printf("Process ID %d has been added to the blocked list \n", pid);
}

void PCB_addToReceiveWaitList(int pid, int priority, int processState, char* messageStored ) {
    pcb_t* processPCB = malloc(sizeof(pcb_t));
    processPCB->messageStored = malloc(MAX_LEN);
    processPCB->pid = pid;
    processPCB->priority = priority;
    processPCB->processState = processState;
    processPCB->messageStored = messageStored;
    List_append(pcbReceiveWaitList, processPCB);
    printf("Process ID %d has been added to the blocked list \n", pid);
}

void PCB_addMessageToList(int destinationPid, int sourcePid, char* messageString) {

    pcb_messages* messageIntended = malloc(sizeof(pcb_messages));
    messageIntended->message = malloc(MAX_LEN);
    messageIntended->destinationPID = destinationPid;
    messageIntended->sourcePID = sourcePid;
    messageIntended->message = messageString;
    List_append(pcbMessagesList, messageIntended);

}

void PCB_printSendReplyText(int sourcePID, int destinationPID, char* message) {
    printf("Reply received on pid %d from pid %d : %s\n", destinationPID, sourcePID, message);
}

int Create(int priority) {

    if (priority < 0 || priority > 2) {
        printf("Priority %d is invalid.", priority);
        return EXIT_FAILURE;
    }

    pcb_t* processPCB = malloc(sizeof(pcb_t));
    processPCB->messageStored = malloc(MAX_LEN);
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
        List_append(pcbP0List, processPCB);
        printf("Process ID %d has been sucessfully added to priority list %d\n", processPCB->pid, priority);
    }
    else if(priority == 1) {
        List_append(pcbP1List, processPCB);
        printf("Process ID %d has been sucessfully added to priority list %d\n", processPCB->pid, priority);
    }
    else if (priority == 2) {
        List_append(pcbP2List, processPCB);
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

    if (runningProcessFound == 0 && List_count(pcbP0List) != 0) {
        // Searching High priority list for running process
        pcbP0List->pCurrentNode = pcbP0List->pFirstNode;
        for (int i = 0; i < List_count(pcbP0List); i++) {
            pcb_t* currentItem = pcbP0List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {
                processPriority = currentItem->priority;
                Create(processPriority);
                pcb_t* copyItem = pcbP0List->pLastNode->pItem;
                copyItem->messageStored = currentItem->messageStored;
                copyItem->processState = 1;
                forkedProcessPid = copyItem->pid;
                
                printf("Successfully forked current running process. PID: %d\n", forkedProcessPid);
                return forkedProcessPid;
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
                processPriority = currentItem->priority;
                Create(processPriority);
                pcb_t* copyItem = pcbP1List->pLastNode->pItem;
                copyItem->messageStored = currentItem->messageStored;
                copyItem->processState = 1;
                forkedProcessPid = copyItem->pid;
                
                printf("Successfully forked current running process. PID: %d\n", forkedProcessPid);
                return forkedProcessPid;
                
            }
            pcbP1List->pCurrentNode = pcbP1List->pCurrentNode->pNext;
        }
    }

    if (runningProcessFound = 0 && List_count(pcbP2List) != 0) {
        // Searching Low priority list for running process
        List_first(pcbP2List);
        List_next(pcbP2List); 
        for (int i = 0; i < List_count(pcbP2List); i++) {
            pcb_t* currentItem = pcbP2List->pCurrentNode->pItem;
            if ( currentItem->processState == 0 && currentItem->pid != 0) {

                processPriority = currentItem->priority;
                Create(processPriority);
                pcb_t* copyItem = pcbP2List->pLastNode->pItem;
                copyItem->messageStored = currentItem->messageStored;
                copyItem->processState = 1;
                forkedProcessPid = copyItem->pid;
                
                printf("Successfully forked current running process. PID: %d\n", forkedProcessPid);
                return forkedProcessPid;
            
            }
            // else {
            //     printf("Cannot fork init process.\n");
            //     return 0;
            // }

            pcbP2List->pCurrentNode = pcbP2List->pCurrentNode->pNext;
        }
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
            List_next(pcbP0List);
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
            List_next(pcbP1List);
        }
    }

    if (runningProcessFound == 0 && List_count(pcbP2List) != 0) {
        // Searching Low priority list for running process
        pcbP2List->pCurrentNode = pcbP2List->pFirstNode;
        for (int i = 0; i < List_count(pcbP2List); i++) {
            pcb_t* currentItem = pcbP2List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {

                if (currentItem->pid == 0 && List_count(pcbP0List) == 0 && List_count(pcbP1List) == 0 && List_count(pcbP2List) == 1) {
                    List_remove(pcbP2List);
                    printf("Running init process exited successfully\n");
                    runningProcessFound = 1;
                    processSource = 2;
                    Main_requestShutdown(1);
                    break;
                }
                else if (currentItem->pid == 0) {
                    printf("Cant exit init process. Other lists are still populated\n");
                    return;
                }

                List_remove(pcbP2List);
                printf ("Running Process from P2 exited sucessfully.\n");
                runningProcessFound = 1;
                processSource = 2;
                break;
            }
            List_next(pcbP2List);
        }
    }

    int processID_number;

    if (runningProcessFound == 0) {
        printf("Failure to exit process.\n");
    }

}

void Quantum() {

    // Quantum based on FCFS on 3 levels of priority.
    // Always finishes priority 0 first before going to 1 and 2.
    // If a new process is added to 0, then that process is next to run.
    PCB_moveRunningProcessToBack();
    PCB_makeProcessRun();
}

void Send(int pid, char* messageString) {
    int processFound = 0;               // Turns 1 if intended process is found

    if (processFound == 0 && List_count(pcbP0List) != 0) {
        // Searching High priority list for intended process
        List_first(pcbP2List);
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
        List_first(pcbP1List);
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
        List_first(pcbP2List);
        for (int i = 0; i < List_count(pcbP2List); i++) {
            pcb_t* currentItem = pcbP2List->pCurrentNode->pItem;
            if ( currentItem->pid == 0) {
                printf("Init process cannot send a message\n");
                return;
            }

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

    PCB_makeProcessRun();

}

void Receive() {

    int runningProcessFound = 0;
    int receiveProcessPid = -1;
    int runningProcessSource = -1;
    char* messageStorageForMove;
    int messageForMoveSourcePid;
    bool messageFound = false;

    if (runningProcessFound == 0 && List_count(pcbP0List) != 0) {
        // Searching High priority list for running process
        List_first(pcbP0List);
        for (int i = 0; i < List_count(pcbP0List); i++) {
            pcb_t* currentItem = pcbP0List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {
                receiveProcessPid = currentItem->pid;
                runningProcessFound = 1;
                runningProcessSource = 0;
                break;
            }
            List_next(pcbP0List);
        }
    }

    if (runningProcessFound == 0 && List_count(pcbP1List) != 0) {
        // Searching Med priority list for running process
        List_first(pcbP1List);
        for (int i = 0; i < List_count(pcbP1List); i++) {
            pcb_t* currentItem = pcbP1List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {
                receiveProcessPid = currentItem->pid;
                runningProcessFound = 1;
                runningProcessSource = 1;
                break;
            }
            List_next(pcbP1List);
        }
    }

    if (runningProcessFound = 0 && List_count(pcbP2List) != 0) {
        // Searching Low priority list for running process
        List_first(pcbP2List);
        for (int i = 0; i < List_count(pcbP2List); i++) {
            pcb_t* currentItem = pcbP2List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {
                receiveProcessPid = currentItem->pid;
                runningProcessFound = 1;
                runningProcessSource = 2;
                break;
            }
            List_next(pcbP2List);
        }
    }

    if (runningProcessFound == 0) {
        printf("No running process found to receive.\n");
    }
    else {
        if (List_count(pcbMessagesList) != 0)
        {
            List_first(pcbMessagesList);
            for (int i = 0; i < List_count(pcbMessagesList); i++) {
                pcb_messages* currentItem = pcbMessagesList->pCurrentNode->pItem;
                if ( currentItem->destinationPID == receiveProcessPid) {
                    messageStorageForMove = currentItem->message;
                    messageForMoveSourcePid = currentItem->sourcePID;
                    messageFound = true;
                    break;
                }
            List_next(pcbMessagesList);
            }

            if (messageFound) {
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
                printf("No message waiting for running process, sending process to receive waiting list.\n");

                if (runningProcessSource == 0) {
                    pcb_t* pcurrentItem = pcbP0List->pCurrentNode->pItem;
                    PCB_addToReceiveWaitList(pcurrentItem->pid, pcurrentItem->priority, pcurrentItem->processState, pcurrentItem->messageStored);
                    List_remove(pcbP0List);
                }
                else if (runningProcessSource == 1) {
                    pcb_t* pcurrentItem = pcbP1List->pCurrentNode->pItem;
                    PCB_addToReceiveWaitList(pcurrentItem->pid, pcurrentItem->priority, pcurrentItem->processState, pcurrentItem->messageStored);
                    List_remove(pcbP1List);
                }
                else if (runningProcessSource == 2) {
                    pcb_t* pcurrentItem = pcbP2List->pCurrentNode->pItem;
                    PCB_addToReceiveWaitList(pcurrentItem->pid, pcurrentItem->priority, pcurrentItem->processState, pcurrentItem->messageStored);
                    List_remove(pcbP2List);
                }
                else
                {
                    printf("Invalid process source (Error from receive())\n");
                }
                
            }
        }
        else {
            printf("Message inbox is empty. Sending process to receive waiting list.\n");
            if (runningProcessSource == 0) {
                    pcb_t* pcurrentItem = pcbP0List->pCurrentNode->pItem;
                    PCB_addToReceiveWaitList(pcurrentItem->pid, pcurrentItem->priority, pcurrentItem->processState, pcurrentItem->messageStored);
                    List_remove(pcbP0List);
                }
                else if (runningProcessSource == 1) {
                    pcb_t* pcurrentItem = pcbP1List->pCurrentNode->pItem;
                    PCB_addToReceiveWaitList(pcurrentItem->pid, pcurrentItem->priority, pcurrentItem->processState, pcurrentItem->messageStored);
                    List_remove(pcbP1List);
                }
                else if (runningProcessSource == 2) {
                    pcb_t* pcurrentItem = pcbP2List->pCurrentNode->pItem;
                    PCB_addToReceiveWaitList(pcurrentItem->pid, pcurrentItem->priority, pcurrentItem->processState, pcurrentItem->messageStored);
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

    int runningProcessFound = 0;
    int sourcePID;

    if (runningProcessFound == 0 && List_count(pcbP0List) != 0) {
        // Searching High priority list for running process
        List_first(pcbP0List);
        for (int i = 0; i < List_count(pcbP0List); i++) {
            pcb_t* currentItem = pcbP0List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {
                sourcePID = currentItem->pid;
                break;
            }
            List_next(pcbP0List);
        }
    }

    if (runningProcessFound == 0 && List_count(pcbP1List) != 0) {
        // Searching Med priority list for running process
        List_first(pcbP1List);
        for (int i = 0; i < List_count(pcbP1List); i++) {
            pcb_t* currentItem = pcbP1List->pCurrentNode->pItem;
            if ( currentItem->processState == 0) {
                sourcePID = currentItem->pid;
                break;
            }
            List_next(pcbP1List);
        }
    }

    if (runningProcessFound = 0 && List_count(pcbP2List) != 0) {
        // Searching Low priority list for running process
        List_first(pcbP2List);
        for (int i = 0; i < List_count(pcbP2List); i++) {
            pcb_t* currentItem = pcbP2List->pCurrentNode->pItem;
            if (currentItem->processState == 0 && currentItem->pid == 0) {
                printf("Init process can't be running.\n");
            }
            else if ( currentItem->processState == 0) {
                sourcePID = currentItem->pid;
                break;
            }
            List_next(pcbP2List);
        }
    }

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

                PCB_printSendReplyText(sourcePID, currentItem->pid, currentItem->messageStored);

                List_remove(pcbBlockedList);
                processFound = 1;
                break;
            }
           List_next(pcbBlockedList);
        }
    }

    if (processFound == 0) {
        printf("No process with pid %d can be found in the blocked list.\n", pid);
    }
    else {
        printf("Reply to process pid %d has been sent sucessfully, pid %d has been sent back to the ready queue.\n", pid, pid);
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

void SemaphoreP(int semaphoreId) {

}

void SemaphoreV(int semaphoreId){}

void Procinfo(int pid) {
    bool found = false;

    if(pid == 0) {
        printf("Process pid 0 from priority 2 queue info:.\n");
        printf("PID: 0 (Init process)\n");
        printf("Message Stored: N/A\n");
        printf("Process Priority: Low\n");
        return;
    }

    if (!found && List_count(pcbP0List) != 0) {
        pcbP0List->pCurrentNode = pcbP0List->pFirstNode;
        for (int i = 0; i < List_count(pcbP0List); i++) {
            pcb_t* currentItem = pcbP0List->pCurrentNode->pItem;
            if ( currentItem->pid == pid) {
                printf("Process pid %d from priority 0 queue info:.\n", currentItem->pid);
                printf("PID: %d\n", currentItem->pid);
                printf("Message Stored: %s\n", currentItem->messageStored);
                printf("Process Priority: High\n");

                if (currentItem->processState == 0) {
                    printf("Process Status: Running\n");
                }
                else if (currentItem->processState == 1) {
                    printf("Process Status: Ready\n");
                }
                found = true;
                break;
            }
            List_next(pcbP0List);
        }
    }
    if (!found && List_count(pcbP1List) != 0) {
        pcbP1List->pCurrentNode = pcbP1List->pFirstNode;
        for (int i = 0; i < List_count(pcbP1List); i++) {
            pcb_t* currentItem = pcbP1List->pCurrentNode->pItem;
            if ( currentItem->pid == pid) {
                printf("Process pid %d from priority 1 queue info:.\n", currentItem->pid);
                printf("PID: %d\n", currentItem->pid);
                printf("Message Stored: %s\n", currentItem->messageStored);
                printf("Process Priority: Medium\n");

                if (currentItem->processState == 0) {
                    printf("Process Status: Running\n");
                }
                else if (currentItem->processState == 1) {
                    printf("Process Status: Ready\n");
                }
                found = true;
                break;
            }
            List_next(pcbP1List);
        }
    }
    if (!found && List_count(pcbP2List) != 0) {
        pcbP2List->pCurrentNode = pcbP2List->pFirstNode;
        for (int i = 0; i < List_count(pcbP2List); i++) {
            pcb_t* currentItem = pcbP2List->pCurrentNode->pItem;
            if ( currentItem->pid == pid) {
                printf("Process pid %d from priority 2 queue info:.\n", currentItem->pid);
                printf("PID: %d\n", currentItem->pid);
                printf("Message Stored: %s\n", currentItem->messageStored);
                printf("Process Priority: Low\n");

                if (currentItem->processState == 0) {
                    printf("Process Status: Running\n");
                }
                else if (currentItem->processState == 1) {
                    printf("Process Status: Ready\n");
                }
                found = true;
                break;
            }
            List_next(pcbP2List);
        }
    }
    if (!found && List_count(pcbBlockedList) != 0) {
        pcbBlockedList->pCurrentNode = pcbBlockedList->pFirstNode;
        for (int i = 0; i < List_count(pcbBlockedList); i++) {
            pcb_t* currentItem = pcbBlockedList->pCurrentNode->pItem;
            if ( currentItem->pid == pid) {
                printf("Process pid %d from Blocked queue info:.\n", currentItem->pid);
                printf("PID: %d\n", currentItem->pid);
                printf("Message Stored: %s\n", currentItem->messageStored);

                if (currentItem->priority == 0) {
                    printf("Process Priority: High\n");
                }
                else if (currentItem->priority == 1)
                {
                    printf("Process Priority: Medium\n");
                }
                else if (currentItem->priority == 2) {
                    printf("Process Priority: Low\n");
                }

                printf("Process Status: Blocked\n");
                found = true;
                break;
            }
            List_next(pcbBlockedList);
        }
    }
    if (!found) {
        printf("There is no processes with pid %d in any queue\n", pid);
    }
}

void Totalinfo() {
    printf("Total processes in High priority queue: %d\n", List_count(pcbP0List));
    printf("Total processes in Medium priority queue: %d\n", List_count(pcbP1List));
    printf("Total processes in Low priority queue (includes init process): %d\n", List_count(pcbP2List));
    printf("Total processes in Blocked queue: %d\n", List_count(pcbBlockedList));
    printf("Total messages waiting in message queue: %d\n", List_count(pcbMessagesList));

}