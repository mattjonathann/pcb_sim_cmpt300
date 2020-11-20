#ifndef _MAIN_H_
#define _MAIN_H_
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

int main();

void Main_requestShutdown(int x);


#endif