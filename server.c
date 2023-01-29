#include "types.h"

int num_clients = 0;
int fd;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
char path_relatiu[255] = PATH_SERVER;

typedef struct {
	int socket; //socket que aten al client
	pthread_t th; //thread que aten al client
	int estat; //saber en quin estat esta la cela per saber si es poden atendre mes clients
	char login[45];
	char ip[20];
} t_client;


int wait_process (int signum) {
	close(fd);
	exit(0);
}

void construir_ruta (char * ruta_desti, char * path, char * ruta_origen) {
	strcpy (ruta_desti, path);
	strcat(ruta_desti, "/");
	strcat(ruta_desti, ruta_origen);
}

int obtenir_data_hora_actuals(char * hora_actual){
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
 
	sprintf(data_hora, "%d%02d%02d%02d%02d%02d", year, month, day, hours, minutes, seconds);
	
	strcpy (hora_actual, data_hora);
	
	return strlen(hora_actual);
}

int obtenir_data_hora_actuals_formatada (char * hora_actual) {
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
 
	sprintf(data_hora, "%02d/%02d/%d %02d:%02d:%02d", day, month, year, hours, minutes, seconds);
	
	strcpy (hora_actual, data_hora);
	
	return strlen(hora_actual);
}

long get_file_size(char *filename) {

    FILE *fp = fopen(filename, "rb");

	if (fp==NULL)
	    return -1;

	if (fseek(fp, 0L, SEEK_END) < 0) {
		perror("ERROR: llegint arxiu\n");
	    fclose(fp);
	    return -1;
	}

	long size = ftell(fp);
	fclose(fp);
		
    return size;
}

int consulta_usuaris_mysql(char * usuari, char * contrasenya){
	
	MYSQL * conn; //variable de conexión para MySQL
	MYSQL_RES *res; //variable que contendra el resultado de la consuta
	MYSQL_ROW row; //variable que contendra los campos por cada registro consultado
	char *server = "localhost";
	char *user = "root";
	char *password = "Alumne1_";
	char *database = "psp";
	
	conn = mysql_init(NULL); //inicializacion a nula la conexión 
	
	//connectar a la base de dades
	if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)){ // definir los parámetros de la conexión antes establecidos
		fprintf(stderr, "%s\n", mysql_error(conn)); // si hay un error definir cual fue dicho error
		exit(1);
	}
	
	// enviar consulta SQL
	if (mysql_query(conn, "select * from usuari")) { // definicion de la consulta y el origen de la conexion
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}
	
	res = mysql_use_result(conn);
	while ((row = mysql_fetch_row(res)) != NULL) { // recorrer la variable res con todos los registros obtenidos para su uso
		if (strcmp (usuari, row[1]) == 0 && strcmp (contrasenya, row[2]) == 0) {
			return 1;
		}
	}
	
	// se libera la variable res y se cierra la conexión
	mysql_free_result(res);
	mysql_close(conn);
	
	return 0;
}


int consulta_ip_valida (char *ip) {
	
	FILE * fitxer;
	char llinea_llegida[20];
	
	if ((fitxer = fopen(PATH_FITXER_IPS, "r+")) < 0) {
		perror("obrint arxiu ips");
		return -1;
	}
	
	while (fscanf(fitxer, "%[^\n] ", llinea_llegida) != EOF) {
		if (strcmp (ip, llinea_llegida) == 0) {
			return 1;
		}
	}
	
	fclose(fitxer);
	
	return 0;
}

int codi_op_ls(int sock, int *data) {

	DIR * carpeta;
	struct dirent *arxiu;
	char nom_arxiu[50];
	char nom_arxiu_temporal[30];
	char path_nom_arxiu_temporal[255];
	FILE * fitxer_temporal;
	
	
	obtenir_data_hora_actuals(nom_arxiu_temporal);
	strcat (nom_arxiu_temporal, ".txt");
	
	
	construir_ruta (path_nom_arxiu_temporal, PATH, nom_arxiu_temporal);
	
	if ((fitxer_temporal = fopen(path_nom_arxiu_temporal, "w+")) < 0) {
		perror("ERROR: creant arxiu temporal");
		return -1;
	}

	carpeta = opendir(path_relatiu);
	if (carpeta != NULL){
		while((arxiu = readdir(carpeta))){
			strcpy (nom_arxiu, (*arxiu).d_name);
			if (strcmp (nom_arxiu, nom_arxiu_temporal) != 0) {
				strcat (nom_arxiu, "\n");
				fputs(nom_arxiu, fitxer_temporal);
			}
			
			
		}
		fclose(fitxer_temporal);
		closedir(carpeta);
	}
	
	if (write (sock, &nom_arxiu_temporal, sizeof(nom_arxiu_temporal)) != sizeof(nom_arxiu_temporal)){
		perror("ERROR: write name file nom_arxiu_temporal");
		return -1;
	}
	
	return 0;
	
}

int codi_op_cd(int sock, int *data) {
	
	char path[255];
	char path_anterior[255];
	int error;
	char * res;
	char path_absolut[255];
	
	if (read (sock, &path, sizeof(path)) != sizeof(path)){
		perror("ERROR: read path");
		return -1;
	}
	
	strcpy (path_anterior, path_relatiu);
	
	if (path[0] == '/') {
		strcpy (path_relatiu, path);
	} else {
		construir_ruta (path_relatiu, path_relatiu, path);
	}
	
	if (write (sock, &path_relatiu, sizeof(path_relatiu)) != sizeof(path_relatiu)){
		perror("ERROR: write path_relatiu");
		return -1;
	}
	
	error = chdir(path_relatiu);
	if (error < 0) {
    	perror("ERROR: chdir");
    	strcpy (path_relatiu, path_anterior);
    	if (write (sock, &error, sizeof(error)) != sizeof(error)){
			perror("ERROR: write path_relatiu");
		}
		printf("PATH ACTUAL: %s\n", path_relatiu);
    	return -1;
    } else {
    	if (write (sock, &error, sizeof(error)) != sizeof(error)){
			perror("ERROR: write path_relatiu");
		}
		res = realpath(path_relatiu, path_absolut);
		if (write (sock, &path_absolut, sizeof(path_absolut)) != sizeof(path_absolut)){
			perror("ERROR: write path_absolut");
		}
		strcpy (path_relatiu, path_absolut);
		printf("PATH ACTUAL: %s\n", path_relatiu);
    }
    
    return 0;
    
}

int codi_op_mkdir(int sock, int *data) {

	char nom_directori[255];
	int error;
	char directori[255];

	if (read (sock, &nom_directori, sizeof(nom_directori)) != sizeof(nom_directori)) {
		perror("ERROR: read nom directori");
		return -1;
	}
	
	construir_ruta (directori, path_relatiu, nom_directori);
	
	error = mkdir(directori, 0777);
	
	if (write (sock, &error, sizeof(int)) != sizeof(int)){
		perror("ERROR: mkdir");
		return -1;
	}
	
	if (error < 0) {
		perror("ERROR: mkdir directori");
		return -1;
	}
	
	return 0;
}

int codi_op_get(int sock, int *data) {
	
	DIR * carpeta;
	struct dirent *arxiu;
	char nom_arxiu_server[255];
	char nom_arxiu_peticio[255];
	int error;
	bool trobat = false;
	FILE * file;
	char directori_nom_arxiu[255];
	long size_arxiu;
	long size_llegit = 0;
	char text[255];
	

	if (read (sock, &nom_arxiu_peticio, sizeof(nom_arxiu_peticio)) != sizeof(nom_arxiu_peticio)) {
		perror("ERROR: read nom directori");
		return -1;
	}
	
	carpeta = opendir(path_relatiu);
	if (carpeta != NULL){
		while((arxiu = readdir(carpeta))){
			if (strcmp ((*arxiu).d_name, nom_arxiu_peticio) == 0) {
				strcpy (nom_arxiu_server, (*arxiu).d_name);
				trobat = true;
			}
		}
		closedir(carpeta);
	}
	
	if (write (sock, &trobat, sizeof(bool)) != sizeof(bool)){
		perror("ERROR: write arxiu trobat");
		return -1;
	}
	
	if (trobat) {
		
		construir_ruta (directori_nom_arxiu, PATH_SERVER, nom_arxiu_server);
	
		size_arxiu =  get_file_size(directori_nom_arxiu);
		
		if (write (sock, &size_arxiu, sizeof(long)) != sizeof(long)){
			perror("ERROR: write arxiu trobat");
			return -1;
		}
		
		if ((file = fopen(directori_nom_arxiu, "rb")) < 0) {
			perror("llegint arxiu");
			return -1;
		}
		
		while (size_llegit < size_arxiu) {
			fgets (text, sizeof(text), file);
			if (write (sock, &text, sizeof(text)) != sizeof(text)){
				perror("ERROR: write arxiu trobat");
				return -1;
			}
			size_llegit += sizeof(text);
		}
		
	} else {
		return -1;
	}
	
	return 0;
}

int codi_op_whoami(int sock, int *data) {
	
	t_client *client = (t_client *)data;
	char login[45];
	
	strcpy (login, (*client).login);
	
	if (write (sock, &login, sizeof(login)) != sizeof(login)) {
		perror("ERROR: read nom directori");
		return -1;
	}
	
	return 0;
}

int isFileOrDirectory(char* recurs) {
    DIR* carpeta;
    struct dirent *arxiu;
	char path_recurs[255];


	construir_ruta (path_recurs, PATH_SERVER, recurs);
	
	carpeta = opendir(path_recurs);
    if(carpeta != NULL) {
     	closedir(carpeta);
     	return 0; //Directori
    } else {
    	carpeta = opendir(PATH_SERVER);
		if (carpeta != NULL){
			while((arxiu = readdir(carpeta))){
				if (strcmp ((*arxiu).d_name, recurs) == 0) {
					return 1; //Arxiu
				}
			}
			closedir(carpeta);
		}
    }

    return -1; //Recurs no trobat
}

int get_directory_size(char *path) {

    DIR* carpeta;
    struct dirent *arxiu;
    int size = 0;
		
	carpeta = opendir(path);
	if (carpeta != NULL){
		while((arxiu = readdir(carpeta))){
			printf("%s - %hu\n", (*arxiu).d_name, (*arxiu).d_reclen);
			size += (*arxiu).d_reclen;
		}
		closedir(carpeta);
	}
		
    return size;
}

int codi_op_stat(int sock, int *data) {
	
	char recurs[255];
	int tipus_recurs;
	char path_recurs[255];
	int mida;
	struct stat *buf; 
	
	if (read (sock, &recurs, sizeof(recurs)) != sizeof(recurs)){
		perror("ERROR: read recurs");
		return -1;
	}
	
	tipus_recurs = isFileOrDirectory(recurs);
	
	if (write (sock, &tipus_recurs, sizeof(tipus_recurs)) != sizeof(tipus_recurs)){
		perror("ERROR: write tipus_recurs");
		return -1;
	}
	
	construir_ruta (path_recurs, PATH_SERVER, recurs);
	if (tipus_recurs != -1) {
		if (tipus_recurs == 1) {
			mida = get_file_size(path_recurs);
			if (write (sock, &mida, sizeof(mida)) != sizeof(mida)){
				perror("ERROR: write mida");
				return -1;
			}
		}
		
	} else {
		return -1;
	}
	
	return 0;
	
}

int escriure_en_log(char * text) {

	FILE * arxiu_log;
	
	if ((arxiu_log = fopen(PATH_LOG, "a+")) < 0) {
		perror("obrint log");
		return -1;
	}
	
	fputs (text, arxiu_log);
	
	fclose(arxiu_log);
}

void *atendre_client (void *data) {
	
	t_client *client = (t_client *)data;
	int sock = (*client).socket;
	int res;
	int fun;
	int login_correcte;
	int ip_correcte;
	char usuari[45];
	char contrasenya[128];
	char ip[20];
	char data_hora_actual[50];
	char text[255];
	char funcio[20];
	int funcio_valida;
	
	obtenir_data_hora_actuals_formatada(data_hora_actual);
	
	strcpy (ip, (*client).ip);
	ip_correcte = consulta_ip_valida(ip);
	
	if (write (sock, &ip_correcte, sizeof(ip_correcte)) != sizeof(ip_correcte)){
		perror("ERROR: write ip_correcte");
		return NULL;
	}
	
	if (ip_correcte == 1) {
	
		if (read (sock, &usuari, sizeof(usuari)) != sizeof(usuari)){
			perror("ERROR: read usuari");
			return NULL;
		}
		
		if (read (sock, &contrasenya, sizeof(contrasenya)) != sizeof(contrasenya)){
			perror("ERROR: read contrasenya");
			return NULL;
		}
		
		login_correcte = consulta_usuaris_mysql(usuari, contrasenya);
		
		if (write (sock, &login_correcte, sizeof(login_correcte)) != sizeof(login_correcte)){
			perror("ERROR: write login_correcte");
			return NULL;
		}
		
		if (login_correcte == 1) {
		
			pthread_mutex_lock(&mut);
			strcpy ((*client).login, usuari);
			pthread_mutex_unlock(&mut);
			
			pthread_mutex_lock(&mut);
			sprintf(text, "%s -- IP: %s - Usuari: %s - LOGIN\n", data_hora_actual, ip, usuari);
			escriure_en_log(text);
			pthread_mutex_unlock(&mut);
		
			do {
				
				if (read (sock, &fun, sizeof(int)) != sizeof(int)) {
					perror("ERROR: read fun");
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
							funcio_valida = codi_op_ls(sock, data);
							strcpy (funcio, "ls");
							break;
						case CD:
							funcio_valida = codi_op_cd(sock, data);
							strcpy (funcio, "cd");
							break;
						case MKDIR:
							funcio_valida = codi_op_mkdir(sock, data);
							strcpy (funcio, "mkdir");
							break;
						case GET:
							funcio_valida = codi_op_get(sock, data);
							strcpy (funcio, "get");
							break;
						case WHOAMI:
							funcio_valida = codi_op_whoami(sock, data);
							strcpy (funcio, "whoami");
							break;
						case STAT:
							funcio_valida = codi_op_stat(sock, data);
							strcpy (funcio, "stat");
							break;
						case EXIT:
							break;
					}
					
					if (funcio_valida == -1) {
						pthread_mutex_lock(&mut);
						sprintf(text, "%s -- ERROR --> IP: %s - Usuari: %s - Funcio: %s\n", data_hora_actual, ip, usuari, funcio);
						escriure_en_log(text);
						pthread_mutex_unlock(&mut);
					} else {
						pthread_mutex_lock(&mut);
						sprintf(text, "%s -- IP: %s - Usuari: %s - Funcio: %s\n", data_hora_actual, ip, usuari, funcio);
						escriure_en_log(text);
						pthread_mutex_unlock(&mut);
					}
				} else {
					if (fun != EXIT) {
						printf("Funcionalitat no implementada en el servidor\n");
						pthread_mutex_lock(&mut);
						sprintf(text, "%s -- ERROR --> IP: %s - Usuari: %s - FUNCIO NO IMPLEMENTADA\n", data_hora_actual, ip, usuari);
						escriure_en_log(text);
						pthread_mutex_unlock(&mut);
					} else {
						pthread_mutex_lock(&mut);
						sprintf(text, "%s -- IP: %s - Usuari: %s - LOGOUT\n", data_hora_actual, ip, usuari);
						escriure_en_log(text);
						pthread_mutex_unlock(&mut);
					}
				}
				
			} while (fun != EXIT);
		
		} else {
			//login
			pthread_mutex_lock(&mut);
			sprintf(text, "%s -- ERROR --> IP: %s - Usuari: %s - LOGIN INCORRECTE\n", data_hora_actual, ip, usuari);
			escriure_en_log(text);
			pthread_mutex_unlock(&mut);
		}
	} else {
		//ip
		pthread_mutex_lock(&mut);
		sprintf(text, "%s -- ERROR --> IP: %s - IP NO AUTORITZADA\n", data_hora_actual, ip);
		escriure_en_log(text);
		pthread_mutex_unlock(&mut);
	}
	
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
	struct sockaddr_in *addr_in;
	char *ip;
	
	
	inicialitzar_clients(clients);
	
	//Socket
	if ((fd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("ERROR: socket");
		return -1;
	}
	
	memset(&server, 0, sizeof(server));
	
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY; //inet_addr(clients[pos].ip);
	
	//Bind
	if (bind(fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		perror("ERROR: bind");
		return -1;
	}
	
	//Listen
	if (listen(fd, BACK_LOG) < 0) {
		perror("ERROR: listen");
		return -1;
	}
	
	for (;;) {
	
		signal(SIGINT, (void (*)(int))wait_process);
		signal(SIGTSTP, (void (*)(int))wait_process);
	
		//Accept
		len = sizeof(server);
		
		if ((sock = accept(fd, (struct sockaddr*)&client, &len)) < 0) {
			perror("ERROR: accept");
			return -1;
		}
		
		addr_in = (struct sockaddr_in *)&client;
		ip = inet_ntoa(addr_in->sin_addr);
		
		// Comprovacio per saber si puc atendre al client
		pthread_mutex_lock (&mut); 
		if (num_clients < MAX_CLIENTS) {
			pos = espai_lliure(clients);
			clients[pos].estat = 1;
			clients[pos].socket = sock;
			strcpy (clients[pos].ip, ip);
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
