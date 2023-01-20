#include "types.h"

int netejar_pantalla(){
	system("clear");
}

void construir_ruta (char * ruta_desti, char * path, char * ruta_origen) {
	strcpy (ruta_desti, path);
	strcat(ruta_desti, "/");
	strcat(ruta_desti, ruta_origen);
}

int get_menu () {
	
	int fun;
	
	netejar_pantalla();
	printf("    MENU\n");
	printf("=============\n");
	printf("   %d - LS\n", LS);
	printf("   %d - CD\n", CD);
	printf("   %d - MKDIR\n", MKDIR);
	printf("   %d - GET\n", GET);
	printf("   %d - WHOAMI\n", WHOAMI);
	printf("   %d - STAT\n", STAT);
	printf("\n   %d - EXIT\n", EXIT);
	printf("=============\n");
	printf("Opcio: ");
	scanf("%d", &fun);
	
	return fun;
}

int codi_op_ls(int sock){

	char nom_arxiu_temporal[30];
	FILE * fitxer_temporal;
	char llinea_llegida[255];

	netejar_pantalla();
	if (read (sock, &nom_arxiu_temporal, sizeof(nom_arxiu_temporal)) != sizeof(nom_arxiu_temporal)) {
		perror("read nom_arxiu_temporal");
		return 1;
	}
	
	if ((fitxer_temporal = fopen(nom_arxiu_temporal, "r+")) < 0) {
		perror("obrint arxiu temporal");
		return -1;
	}
	
	while (fscanf(fitxer_temporal, "%[^\n] ", llinea_llegida) != EOF) {
		printf("%s\n", llinea_llegida);
	}
	
	fclose(fitxer_temporal);
	if (remove(nom_arxiu_temporal) < 0) {
		perror("esborrant arxiu temporal");
		return -1;
	}
	
}

int codi_op_cd(int sock){
	
	char path[255];

	netejar_pantalla();
	printf("PATH: ");
	scanf("%s", path);
	printf("%s\n", path);
}

int codi_op_mkdir(int sock){

	char nom_directori[255];
	char directori_correcte = 'n';
	int error;

	netejar_pantalla();
	
	do {
		printf("Nom del direcoti a creaer: ");
		scanf("%s", nom_directori);
		printf("El nom del directori a crear es: \"%s\", es correcte? (s/n): ", nom_directori);
		__fpurge(stdin);
		scanf("%c", &directori_correcte);
	} while (directori_correcte != 's');
	
	if (write (sock, &nom_directori, sizeof(nom_directori)) != sizeof(nom_directori)){
		perror("ERROR: write nom directori");
		return -1;
	}
	
	if (read (sock, &error, sizeof(int)) != sizeof(int)){
		perror("ERROR: mkdir");
		return -1;
	}
	
	if (error == 0) {
		printf("Directori creat correctament\n");
	} else {
		perror("ERROR: El directori ja existeix");
	}
}

int codi_op_get(int sock){

	char nom_arxiu[255];
	char directori_nom_arxiu[255];
	char arxiu_correcte = 'n';
	int error;
	bool trobat;
	FILE * arxiu;

	netejar_pantalla();
	
	do {
		printf("Nom del arxiu amb la extensio: ");
		scanf("%s", nom_arxiu);
		printf("El nom del arxiu es: \"%s\", es correcte? (s/n): ", nom_arxiu);
		__fpurge(stdin);
		scanf("%c", &arxiu_correcte);
	} while (arxiu_correcte != 's');
	
	if (write (sock, &nom_arxiu, sizeof(nom_arxiu)) != sizeof(nom_arxiu)){
		perror("ERROR: write nom arxiu");
		return -1;
	}
	
	if (read (sock, &trobat, sizeof(bool)) != sizeof(bool)){
		perror("ERROR: read arxiu trobat");
		return -1;
	}
	
	if (trobat) {
		construir_ruta (directori_nom_arxiu, PATH_CLIENT, nom_arxiu);
		
		printf("Directori: %s\n", directori_nom_arxiu);
		
		if ((arxiu = fopen(directori_nom_arxiu, "w+")) < 0) {
			perror("creant arxiu");
			return -1;
		}
		
		//TODO: queda llegir el arxiu del servidor i transferir-lo al client
		
		fclose(arxiu);
	
	} else {
		perror("ERROR: l'arxiu no existeix");
	}
	
	
}

int codi_op_whoami(int sock){

	netejar_pantalla();
}

int codi_op_stat(int sock){

	char recurs[255];
	
	netejar_pantalla();
	printf("RECURS: ");
	scanf("%s", recurs);
	printf("%s\n", recurs);
}

int main (int argc, char **argv) {
	
	//e_fun fun;
	struct sockaddr_in client;
	int fd, sock;
	int fun;
	
	//Socket
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("ERROR: socket");
		return -1;
	}
	
	memset(&client, 0, sizeof(client));
	
	client.sin_family = AF_INET;
	client.sin_port = htons(PORT);
	client.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//Connect
	if ((sock = connect(fd, (struct sockaddr *)&client, sizeof(client))) < 0) {
		perror("ERROR: connect");
		return 0;
	}
	
	do {
		fun = get_menu();
		if ((write (fd, &fun, sizeof(int))) != sizeof(int)){
			perror("ERROR: write fun");
			return -1;
		}
		switch (fun) {
			case LS:
				codi_op_ls(fd);
				break;
			case CD:
				codi_op_cd(fd);
				break;
			case MKDIR:
				codi_op_mkdir(fd);
				break;
			case GET:
				codi_op_get(fd);
				break;
			case WHOAMI:
				codi_op_whoami(fd);
				break;
			case STAT:
				codi_op_stat(fd);
				break;
			case EXIT:
				break;
		}
		
		if (fun != EXIT) {
			__fpurge(stdin);
			getchar();
		}
		
	} while (fun != EXIT);
	
	close(fd);
	
	return 0;
	
}
