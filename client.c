#include "types.h"

char path_relatiu[255];

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
	char path_nom_arxiu_temporal[255];

	netejar_pantalla();
	
	if (read (sock, &nom_arxiu_temporal, sizeof(nom_arxiu_temporal)) != sizeof(nom_arxiu_temporal)) {
		perror("read nom_arxiu_temporal");
		return 1;
	}
	
	construir_ruta (path_nom_arxiu_temporal, PATH, nom_arxiu_temporal);
	
	if ((fitxer_temporal = fopen(path_nom_arxiu_temporal, "r+")) < 0) {
		perror("obrint arxiu temporal");
		return -1;
	}
	
	while (fscanf(fitxer_temporal, "%[^\n] ", llinea_llegida) != EOF) {
		printf("%s\n", llinea_llegida);
	}
	
	fclose(fitxer_temporal);
	if (remove(path_nom_arxiu_temporal) < 0) {
		perror("esborrant arxiu temporal");
		return -1;
	}
	
}

int codi_op_cd(int sock){
	
	char path[255];
	char path_correcte = 'n';
	char path_absolut[255];
	
	int error;


	netejar_pantalla();
	do {
		printf("Path: ");
		scanf("%s", path);
		printf("El path es: \"%s\", es correcte? (s/n): ", path);
		__fpurge(stdin);
		scanf("%c", &path_correcte);
	} while (path_correcte != 's');
	
	if (write (sock, &path, sizeof(path)) != sizeof(path)){
		perror("ERROR: write path");
		return -1;
	}
	
	if (read (sock, &path_relatiu, sizeof(path_relatiu)) != sizeof(path_relatiu)){
		perror("ERROR: read path_relatiu");
		return -1;
	}
	
	if (read (sock, &error, sizeof(error)) != sizeof(error)){
		perror("ERROR: write path_relatiu");
		return -1;
	}
	
	if (error >= 0) {
		if (read (sock, &path_absolut, sizeof(path_absolut)) != sizeof(path_absolut)){
			perror("ERROR: write path_absolut");
		}
		printf("PATH ACTUAL: %s\n", path_absolut);
		
	} else {
		perror("ERROR: path erroni");
		return -1;
	}
	
	
}

int codi_op_mkdir(int sock){

	char nom_directori[255];
	char directori_correcte = 'n';
	int error;

	netejar_pantalla();
	
	do {
		printf("Nom del directori a creaer: ");
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
	long size_arxiu;
	long size_llegit = 0;
	char text[255];

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
		
		if ((arxiu = fopen(directori_nom_arxiu, "w+")) < 0) {
			perror("creant arxiu");
			return -1;
		}
		
		if (read (sock, &size_arxiu, sizeof(long)) != sizeof(long)){
			perror("ERROR: read arxiu trobat");
			return -1;
		}
		
		while (size_llegit < size_arxiu) {
			if (read (sock, &text, sizeof(text)) != sizeof(text)){
				perror("ERROR: read arxiu trobat");
				return -1;
			}
			fputs (text, arxiu);
			size_llegit += sizeof(text);
		}
		
		if (size_llegit >= size_arxiu) {
			printf("Arxiu \"%s\" transferit correctament\n", nom_arxiu);
		}
		
		fclose(arxiu);
	
	} else {
		perror("ERROR: l'arxiu no existeix");
	}
	
	
}

int codi_op_whoami(int sock){

	char login[45];

	netejar_pantalla();
	
	if (read (sock, &login, sizeof(login)) != sizeof(login)) {
		perror("ERROR: read nom directori");
		return 1;
	}
	
	printf("Usuari connectat: %s\n", login);
}

int codi_op_stat(int sock){

	char recurs[255];
	char recurs_correcte = 'n';
	int tipus_recurs;
	int mida;
	
	netejar_pantalla();
	
	do {
		printf("Nom del recurs: ");
		scanf("%s", recurs);
		printf("El recurs es: \"%s\", es correcte? (s/n): ", recurs);
		__fpurge(stdin);
		scanf("%c", &recurs_correcte);
	} while (recurs_correcte != 's');
	
	if (write (sock, &recurs, sizeof(recurs)) != sizeof(recurs)){
		perror("ERROR: write recurs");
		return -1;
	}
	
	if (read (sock, &tipus_recurs, sizeof(tipus_recurs)) != sizeof(tipus_recurs)){
		perror("ERROR: read tipus_recurs");
		return -1;
	}
	
	if (tipus_recurs != -1) {
		
		printf("Tipus: ");
		if (tipus_recurs == 0){
			printf("Directori\n");
		} else {
			printf("Arxiu\n");
			
			if (read (sock, &mida, sizeof(mida)) != sizeof(mida)){
				perror("ERROR: read mida");
				return -1;
			}
			
			printf("Mida: %d\n", mida);
			
		}
		
	} else {
		printf("Recurs no trobat");
	}
	
}

int main (int argc, char **argv) {
	
	struct sockaddr_in client;
	int fd, sock;
	int fun;
	char usuari[45];
	char contrasenya[128];
	char login_ok = 'n';
	int login_correcte;
	int ip_correcte;
	
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
		return -1;
	}
	
	if (read (fd, &ip_correcte, sizeof(ip_correcte)) != sizeof(ip_correcte)){
		perror("ERROR: read ip_correcte");
		return -1;
	}
	
	if (ip_correcte == 1) {
	
		netejar_pantalla();
	
		do {
			printf("LOGIN\n");
			printf("Usuari: ");
			scanf("%s", usuari);
			printf("Contrasenya: ");
			scanf("%s", contrasenya);
			printf("L'usuari es: \"%s\" i la contrasenya es: \"%s\", es correcte? (s/n): ", usuari, contrasenya);
			__fpurge(stdin);
			scanf("%c", &login_ok);
		} while (login_ok != 's');
		
		
		if (write (fd, &usuari, sizeof(usuari)) != sizeof(usuari)){
			perror("ERROR: write recurs");
			return -1;
		}
		
		if (write (fd, &contrasenya, sizeof(contrasenya)) != sizeof(contrasenya)){
			perror("ERROR: write recurs");
			return -1;
		}
		
		if (read (fd, &login_correcte, sizeof(login_correcte)) != sizeof(login_correcte)){
			perror("ERROR: read login_correcte");
			return -1;
		}
		
		if (login_correcte == 1) {
		
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
		
		} else {
			perror("ERROR: LOGIN INCORRECTE");
		}
	} else {
		perror("ERROR: IP NO AUTORITZADA");
	}
	
	close(fd);
	
	return 0;
	
}
