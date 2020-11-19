#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include "list.h"
#include "pcb.h"


int main() {
    List* P0List; P0List = List_Create();                   // Priority 1 queue
    List* P1List; P1List = List_Create();                   // Priority 2 queue
    List* P2List; P2List = List_Create();                   // Priority 3 queue
    List* BlockedList; BlockedList = List_Create();         // Blocked process queue
    List* SendWaitList; SendWaitList = List_Create();       // queue of processes waiting on a send operation
    List* ReceiveWaitList; ReceiveWaitList = List_Create(); // queue of processes waiting on a receive operation 
}