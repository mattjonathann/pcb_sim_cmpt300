#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <stdbool.h>
#include "list.h"
#include "pcb.h"
#define MAX_LEN 40

bool runningCondition = true;

int main() {
    List* P0List; P0List = List_create();                   // Priority 1 queue
    List* P1List; P1List = List_create();                   // Priority 2 queue
    List* P2List; P2List = List_create();                   // Priority 3 queue
    List* BlockedList; BlockedList = List_create();         // Blocked process queue
    List* ReceiveWaitList; ReceiveWaitList = List_create(); // queue of processes waiting on a receive operation 
    List* MessagesList; MessagesList = List_create();       // list of messages sent by a process using send()

    // Initialize PCB at program start
    PCB_init(P0List, P1List, P2List, ReceiveWaitList, BlockedList, MessagesList);

    char* keyboardUserInput;
    char* messageInput;
    keyboardUserInput = malloc(MAX_LEN);

    printf("Welcome to PCB Simulation.\n");
    printf("Maximum character length for messages : 40\n");
    printf("Type 'H' for a list of commands.\n");


    while(runningCondition) {
        fgets(keyboardUserInput, MAX_LEN, stdin);

        if ( keyboardUserInput[0] == 'F' && keyboardUserInput[2] == '\0') {
            Fork();
        }
        else if (keyboardUserInput[0] == 'Q' && keyboardUserInput[2] == '\0') { // ADD SAFEGUARD SO ONLY 1 PROCESS CAN RUN AT A TIME
            Quantum();
        }
        else if (keyboardUserInput[0] == 'E' && keyboardUserInput[2] == '\0') { // DONE
            Exit();
        }
        else if ( keyboardUserInput[0] == 'C' && keyboardUserInput[2] == '\0') { // DONE
            int priority;
            printf("What priority should the new process be? (0-2)\n");
            scanf("%d", &priority);
            Create(priority);
        }
        else if (keyboardUserInput[0] == 'R' && keyboardUserInput[2] == '\0') {     // Faulty, cant find active process
            Receive();
        }
        else if (keyboardUserInput[0] == 'S' && keyboardUserInput[2] == '\0') {  // Done
            int pidDestination;
            messageInput = malloc(MAX_LEN);
            printf("Which process PID to send message to?\n");
            scanf("%d", &pidDestination);
            printf("Enter Message :\n");
            scanf("%s", messageInput);
            getchar();
            Send(pidDestination, messageInput);

        }
        else if (keyboardUserInput[0] == 'I' && keyboardUserInput[2] == '\0') { // Done
            printf("Enter pid of process to get info from: \n");
            int pidDestination;
            scanf("%d", &pidDestination);
            Procinfo(pidDestination);

        }
        else if (keyboardUserInput[0] == 'T' && keyboardUserInput[2] == '\0') {
            Totalinfo();
        }
        else if (keyboardUserInput[0] == 'Y' && keyboardUserInput[2] == '\0') {
            int pidDestination;
            messageInput = malloc(MAX_LEN);
            printf("Enter pid of process to reply to: \n");
            scanf("%d", &pidDestination);
            printf("Enter Message :\n");
            scanf("%s", messageInput);
            getchar();
            Reply(pidDestination, messageInput); 
        }
         else if (keyboardUserInput[0] == 'N' && keyboardUserInput[2] == '\0') {
            int semaphoreID;
            int value;
            printf("Enter sempahore ID (0-4): \n");
            scanf("%d", &semaphoreID);
            printf("Enter semaphore value (0 or higher): \n");
            scanf("%d", &value);
            NewSemaphore(semaphoreID, value);

        }
         else if (keyboardUserInput[0] == 'P' && keyboardUserInput[2] == '\0') {
            int semaphoreID;
            printf("Enter sempahore ID (0-4): \n");
            scanf("%d", &semaphoreID);
            SemaphoreP(semaphoreID);
        }
         else if (keyboardUserInput[0] == 'V' && keyboardUserInput[2] == '\0') {
            int semaphoreID;
            printf("Enter sempahore ID (0-4): \n");
            scanf("%d", &semaphoreID);
            SemaphoreV(semaphoreID);
        }

        // Developer commands
        else if (keyboardUserInput[0] == '[' && keyboardUserInput[2] == '\0') {                     // Find currently running process
            PCB_findCurrentRunningProcess();
        }
        else if (keyboardUserInput[0] == '!' && keyboardUserInput[2] == '\0') {                     // Force Quit
            runningCondition = false;
        }
        
            
    }
    free(keyboardUserInput);
    printf("========Done execution. Simulation shutting down.========\n");
    
}

void Main_requestShutdown(int x) {
    if (x == 1) {
        runningCondition = false;
    }
}