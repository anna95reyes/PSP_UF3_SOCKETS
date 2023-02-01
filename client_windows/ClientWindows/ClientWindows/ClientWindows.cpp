#include "../../../types.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include <cstring>
#include <string>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

char path_relatiu[255];
char SendBuff[512], RecvBuff[512];

void netejar_pantalla() {
	system("cls");
}


void construir_ruta(char* ruta_desti, char* path, char* ruta_origen) {
	
	strcpy(ruta_desti, path);
	strcat(ruta_desti,  "\\");
	strcat(ruta_desti, ruta_origen);
}


int get_menu() {

	int fun;

	netejar_pantalla();
	cout << "    MENU" << endl;
	cout << "=============" << endl;
	cout << "   " << LS << " - LS" << endl;
	cout << "   " << CD << " - CD" << endl;
	cout << "   " << MKDIR << " - MKDIR" << endl;
	cout << "   " << GET << " - GET" << endl;
	cout << "   " << WHOAMI << " - WHOAMI" << endl;
	cout << "   " << STAT << " - STAT" << endl;
	cout << endl << "   " << EXIT << " - EXIT" << endl;
	cout << "=============" << endl;
	cout << "Opcio: ";
	cin >> fun;

	return fun;
}

int codi_op_ls(SOCKET sock) {

	char nom_arxiu_temporal[30];
	FILE* fitxer_temporal;
	char llinea_llegida[255];
	char path_nom_arxiu_temporal[255];

	netejar_pantalla();

	if (recv(sock, nom_arxiu_temporal, sizeof(nom_arxiu_temporal), 0) != sizeof(nom_arxiu_temporal)) {
		perror("read nom_arxiu_temporal");
		return 1;
	}

	construir_ruta(path_nom_arxiu_temporal, (char*)PATH_WINDOWS, nom_arxiu_temporal);

	if ((fitxer_temporal = fopen(path_nom_arxiu_temporal, "r+")) == NULL) {
		perror("obrint arxiu temporal");
		return -1;
	}

	while (fscanf(fitxer_temporal, "%[^\n] ", llinea_llegida) != EOF) {
		cout << llinea_llegida << endl;
	}

	fclose(fitxer_temporal);
	if (remove(path_nom_arxiu_temporal) < 0) {
		perror("esborrant arxiu temporal");
		return -1;
	}
	
}

int main(int argc, char** argv)
{
	WSADATA wsaData;
	SOCKET conn_socket;
	struct sockaddr_in server;
	struct hostent* hp;
	int resp;
	int fun;
	char usuari[45];
	char contrasenya[128];
	char login_ok = 'n';
	int login_correcte;
	int ip_correcte;

	//Inicialitzem la DLL dels sockets
	if (resp = WSAStartup(MAKEWORD(2, 0), &wsaData)) {
		cout << "Error al inicialitzar socket" << endl;
		getchar();
		return -1;
	}

	//Obtenim la IP del servidor... 
	if (!(hp = (struct hostent*)gethostbyname(IP_SERVER))) {
		cout << "No s'ha trobat el servidor..." << endl;
		getchar();
		WSACleanup();
		return WSAGetLastError();
	}

	// socket
	if ((conn_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		cout << "Error al crear el socket" << endl;
		getchar();
		WSACleanup();
		return WSAGetLastError();
	}

	memset(&server, 0, sizeof(server));
	memcpy(&server.sin_addr, (*hp).h_addr, (*hp).h_length);
	server.sin_family = (*hp).h_addrtype;
	server.sin_port = htons(PORT);

	// connect
	if (connect(conn_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		cout << "Error al conectar-se amb el servidor" << endl;
		closesocket(conn_socket);
		getchar();
		WSACleanup();
		return WSAGetLastError();
	}
	cout << "Conexio establida amb: " << inet_ntoa(server.sin_addr) << endl;

	if (recv(conn_socket, (char *)&ip_correcte, sizeof(ip_correcte), 0) != sizeof(ip_correcte)) {
		perror("ERROR: read ip_correcte");
		return -1;
	}

	if (ip_correcte == 1) {

		netejar_pantalla();

		do {
			cout << "LOGIN" << endl;
			cout << "Usuari: ";
			cin >> usuari;
			cout << "Contrasenya: ";
			cin >> contrasenya;
			cout << "L'usuari es: " << usuari  << " i la contrasenya es: "<< contrasenya  << ", es correcte? (s/n): ";
			fflush(stdin);
			cin >> login_ok;
		} while (login_ok != 's');


		if (send(conn_socket, usuari, sizeof(usuari), 0) != sizeof(usuari)) {
			perror("ERROR: read ip_correcte");
			return -1;
		}

		if (send(conn_socket, contrasenya, sizeof(contrasenya), 0) != sizeof(contrasenya)) {
			perror("ERROR: write recurs");
			return -1;
		}

		if (recv(conn_socket, (char*)&login_correcte, sizeof(login_correcte), 0) != sizeof(login_correcte)) {
			perror("ERROR: read login_correcte");
			return -1;
		}

		if (login_correcte == 1) {
			do {
				fun = get_menu();

				if ((send(conn_socket, (char*)&fun, sizeof(int), 0)) != sizeof(int)) {
					perror("ERROR: write fun");
					return -1;
				}

				switch (fun) {
					case LS:
						codi_op_ls(conn_socket);
						break;
					case CD:
						//codi_op_cd(conn_socket);
						break;
					case MKDIR:
						//codi_op_mkdir(conn_socket);
						break;
					case GET:
						//codi_op_get(conn_socket);
						break;
					case WHOAMI:
						//codi_op_whoami(conn_socket);
						break;
					case STAT:
						//codi_op_stat(conn_socket);
						break;
					case EXIT:
						break;
				}

				if (fun != EXIT) {
					fflush(stdin);
					cout << endl;
					system("pause");
				}

			} while (fun != EXIT);
		}


	}

	// Cerramos el socket y liberamos la DLL de sockets
	closesocket(conn_socket);
	WSACleanup();
	return EXIT_SUCCESS;
}
