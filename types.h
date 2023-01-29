#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <dirent.h>
#include <stdint.h>
#include <fcntl.h>
#include <mysql/mysql.h>

#define MAX_CLIENTS 5
#define PORT 1115
#define BACK_LOG 10
#define PATH "/mnt/hgfs/UF3/1_Sockets/Practica"
#define PATH_FITXER_IPS "/mnt/hgfs/UF3/1_Sockets/Practica/ips_valides.txt"
#define PATH_SERVER "/mnt/hgfs/UF3/1_Sockets/Practica/directori_server"
#define PATH_CLIENT "/mnt/hgfs/UF3/1_Sockets/Practica/directori_client"
#define PATH_LOG "/mnt/hgfs/UF3/1_Sockets/Practica/log.txt"

#define EXIT 0
#define LS 1
#define CD 2
#define MKDIR 3
#define GET 4
#define WHOAMI 5
#define STAT 6


