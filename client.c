#include "types.h"

int netejar_pantalla(){
	system("clear");
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

	netejar_pantalla();
	printf("OPCIO LS\n");
}

int codi_op_cd(int sock){
	
	char path[255];

	netejar_pantalla();
	printf("OPCIO CD\n");
	printf("PATH: ");
	scanf("%s", path);
	printf("%s\n", path);
}

int codi_op_mkdir(int sock){

	char recurs[255];

	netejar_pantalla();
	printf("OPCIO MKDIR\n");
	printf("RECURS: ");
	scanf("%s", recurs);
	printf("%s\n", recurs);
}

int codi_op_get(int sock){

	char recurs[255];
	
	netejar_pantalla();
	printf("OPCIO GET\n");
	printf("RECURS: ");
	scanf("%s", recurs);
	printf("%s\n", recurs);
}

int codi_op_whoami(int sock){

	netejar_pantalla();
	printf("OPCIO WHOAMI\n");
}

int codi_op_stat(int sock){

	char recurs[255];
	
	netejar_pantalla();
	printf("OPCIO STAT\n");
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
		perror("socket");
		return 1;
	}
	
	memset(&client, 0, sizeof(client));
	
	client.sin_family = AF_INET;
	client.sin_port = htons(PORT);
	client.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//Connect
	if ((sock = connect(fd, (struct sockaddr *)&client, sizeof(client))) < 0) {
		perror("connect");
		return 0;
	}
	
	do {
		fun = get_menu();
		printf("FUN: %d\n", fun);
		if ((write (fd, &fun, sizeof(int))) != sizeof(int)){
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
	} while (fun != EXIT);
	
	close(fd);
	
	return 0;
	
}
