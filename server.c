#include "types.h"

int num_clients = 0;
int fd;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
	int socket; //socket que aten al client
	pthread_t th; //thread que aten al client
	int estat; //saber en quin estat esta la cela per saber si es poden atendre mes clients
} t_client;


int wait_process (int signum) {
	close(fd);
	exit(0);
}

int codi_op_ls(int sock, int *data) {
	printf("OPCIO LS\n");
}

int codi_op_cd(int sock, int *data) {
	
}

int codi_op_mkdir(int sock, int *data) {
	
}

int codi_op_get(int sock, int *data) {
	
}

int codi_op_whoami(int sock, int *data) {
	
}

int codi_op_stat(int sock, int *data) {
	
}

void *atendre_client (void *data) {
	
	t_client *client = (t_client *)data;
	int sock = (*client).socket;
	//e_fun fun;
	int res;
	int fun;
	
	printf("12\n");
	do {
		
		printf("sock %d\n", sock);
		printf("13\n");
		if (read (sock, &fun, sizeof(int)) != sizeof(int)) {
			perror("read fun");
			return NULL;
		}
		printf("14\n");
		printf("fun %d\n", fun);
		//Controla si el client esta mes actualitzat que el servidor, perque no peti
		res = 1;
		switch(fun) {
			case LS:
			case CD:
			case MKDIR:
			case GET:
			case WHOAMI:
			case STAT:
				res = 0;
				break;
		}
		printf("res %d\n", res);
		if(res == 0) {
			switch (fun) {
				case LS:
					codi_op_ls(sock, data);
					break;
				case CD:
					codi_op_cd(sock, data);
					break;
				case MKDIR:
					codi_op_mkdir(sock, data);
					break;
				case GET:
					codi_op_get(sock, data);
					break;
				case WHOAMI:
					codi_op_whoami(sock, data);
					break;
				case STAT:
					codi_op_stat(sock, data);
					break;
				case EXIT:
					break;
			}
		} else {
			printf("Funcionalitat no implementada en el servidor");
		}
		
	} while (fun != EXIT);
	
	close(sock);
	
	pthread_mutex_lock(&mut);
	(*client).estat = 0;
	num_clients--;
	pthread_mutex_unlock(&mut);
	
	return NULL;
	
}

void inicialitzar_clients (t_client *clients) {
	for (int i = 0; i < MAX_CLIENTS; i++){
		clients[i].estat = 0;
	}
}

int espai_lliure (t_client *clients) {
	for (int i = 0; i < MAX_CLIENTS; i++){
		if (clients[i].estat == 0) {
			return i;
		}
	}
	return 1;
}

int main (int argc, char **argv) {
	
	t_client clients[MAX_CLIENTS];
	struct sockaddr_in server, client;
	int sock, len, pos;
	
	printf("1\n");
	inicialitzar_clients(clients);
	
	printf("2\n");
	//Socket
	if ((fd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return 1;
	}
	
	printf("3\n");
	memset(&server, 0, sizeof(server));
	
	printf("4\n");
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY; //inet_addr(clients[pos].ip);
	
	printf("5\n");
	//Bind
	if (bind(fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		perror("bind");
		return 1;
	}
	
	printf("6\n");
	//Listen
	if (listen(fd, BACK_LOG) < 0) {
		perror("listen");
		return 1;
	}
	
	printf("7\n");
	for (;;) {
	
		signal(SIGINT, (void (*)(int))wait_process);
		signal(SIGTSTP, (void (*)(int))wait_process);
	
		//Accept
		len = sizeof(server);
		
		printf("8\n");
		if ((sock = accept(fd, (struct sockaddr*)&client, &len)) < 0) {
			perror("accept");
			return 1;
		}
		
		printf("9\n");
		// Comprovacio per saber si puc atendre al client
		pthread_mutex_lock (&mut); 
		if (num_clients < MAX_CLIENTS) {
			pos = espai_lliure(clients);
			clients[pos].estat = 1;
			clients[pos].socket = sock;
			num_clients++;
		}
		pthread_mutex_unlock (&mut); 
		
		printf("10\n");
		if (clients[pos].estat == 1) {
			printf("11\n");
			pthread_create (&clients[pos].th, NULL, atendre_client, (void *)&clients[pos]);
			pthread_detach (clients[pos].th);
		} else {
			close(sock); //Socket que aten al client
		}
		
	}
	
	close(fd); //Socket que permet atendre nous clients
	
	return 0;
	
}
