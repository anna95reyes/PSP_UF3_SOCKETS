#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MAX_CLIENTS 5
#define SORTIR 0
#define PORT 1112
#define BACK_LOG 10

typedef enum {
	LS = 1,
	CD = 2,
	MKDIR = 3,
	GET = 4,
	WHOAMI = 5,
	STAT = 6,
	EXIT = SORTIR
} e_fun;


