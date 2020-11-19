#ifndef _PCB_H_
#define _PCB_H_

typedef struct 
{
    int pid;                    // Process ID (PID)
    int priority;               // Priority 0, 1, 2
    int processState;           // State of the Process. (0 =  Running, 1 = Ready, 2 = Blocked)
    char* messageStored;        // Storage of message from send / reply functions
}pcb_t;

typedef struct
{
    int destinationPID;
    int sourcePID;
    char* message;
}pcb_messages;

typedef struct
{
    int semaphoreId;
    int value;
    List* pList;
} semaphore;


bool runningCheck(void* pItem, void* pComparisonArg);

void PCB_addToReadyList(List* readyList, int pid, int priority, int processState, char* messageStored);

void PCB_addToBlockedList(int pid, int priority, int processState, char* messageStored );

void PCB_addMessageToList(int destinationPid, int sourcePid, char* messageString);

void PCB_init(List* P0list, List* P1list, List* P2list, List* sendWait, List* receiveWait, List* blockedList, List* messagesList);

int Create(int priority);

int Fork();

void Kill(int pid);

void Exit();

void Quantum();

void Send(int pid, char* messageString);

void Receive();

void Reply(int pid, char* messageString);

void NewSemaphore(int semaphoreId, int value);

void SemaphoreP(int semaphoreId);

void SemaphoreV(int semaphoreId);

void Procinfo(int pid);

void Totalinfo();


#endif