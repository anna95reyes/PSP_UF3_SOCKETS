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

int obtenir_data_hora_actuals(char * nom_arxiu_temporal){
	time_t t = time(&t);
	struct tm *local_time = localtime(&t);
	char data_hora[30];
	int hours, minutes, seconds, day, month, year;
	
	year = (*local_time).tm_year + 1900;
	month = (*local_time).tm_mon + 1;
	day = (*local_time).tm_mday;
	hours = (*local_time).tm_hour;
    minutes = (*local_time).tm_min;
    seconds = (*local_time).tm_sec;
 
	sprintf(data_hora, "%02d%02d%d%02d%02d%02d", year, month, day, hours, minutes, seconds);
	
	strcpy (nom_arxiu_temporal, data_hora);
	
	return strlen(nom_arxiu_temporal);
}

int codi_op_ls(int sock, int *data) {

	DIR * carpeta;
	struct dirent *arxiu;
	char nom_arxiu[50];
	char nom_arxiu_temporal[30];
	FILE * fitxer_temporal;
	
	obtenir_data_hora_actuals(nom_arxiu_temporal);
	strcat (nom_arxiu_temporal, ".txt");
	
	if ((fitxer_temporal = fopen(nom_arxiu_temporal, "w+")) < 0) {
		perror("obrint arxiu temporal");
		return -1;
	}

	carpeta = opendir(PATH);
	if (carpeta != NULL){
		while((arxiu = readdir(carpeta))){
			strcpy (nom_arxiu, (*arxiu).d_name);
			strcat (nom_arxiu, "\n");
			printf("%s", nom_arxiu);
			fputs(nom_arxiu, fitxer_temporal);
			
		}
		fclose(fitxer_temporal);
		closedir(carpeta);
	}
	
	//remove("nom_arxiu_temporal")
	
	
	
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
	
	do {
		
		if (read (sock, &fun, sizeof(int)) != sizeof(int)) {
			perror("read fun");
			return NULL;
		}
		
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
	
	inicialitzar_clients(clients);
	
	//Socket
	if ((fd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return 1;
	}
	
	memset(&server, 0, sizeof(server));
	
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY; //inet_addr(clients[pos].ip);
	
	//Bind
	if (bind(fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		perror("bind");
		return 1;
	}
	
	//Listen
	if (listen(fd, BACK_LOG) < 0) {
		perror("listen");
		return 1;
	}
	
	for (;;) {
	
		signal(SIGINT, (void (*)(int))wait_process);
		signal(SIGTSTP, (void (*)(int))wait_process);
	
		//Accept
		len = sizeof(server);
		
		if ((sock = accept(fd, (struct sockaddr*)&client, &len)) < 0) {
			perror("accept");
			return 1;
		}
		
		// Comprovacio per saber si puc atendre al client
		pthread_mutex_lock (&mut); 
		if (num_clients < MAX_CLIENTS) {
			pos = espai_lliure(clients);
			clients[pos].estat = 1;
			clients[pos].socket = sock;
			num_clients++;
		}
		pthread_mutex_unlock (&mut); 
		
		if (clients[pos].estat == 1) {
			pthread_create (&clients[pos].th, NULL, atendre_client, (void *)&clients[pos]);
			pthread_detach (clients[pos].th);
		} else {
			close(sock); //Socket que aten al client
		}
		
	}
	
	close(fd); //Socket que permet atendre nous clients
	
	return 0;
	
}