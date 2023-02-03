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

int codi_op_cd(SOCKET sock) {
	
	char path[255];
	char path_correcte = 'n';
	char path_absolut[255];

	int error;


	netejar_pantalla();
	do {
		cout << "Path: ";
		cin >> path;
		cout << "El path es: \"" << path << "\", es correcte? (s/n): ";
		fflush(stdin);
		cin >> path_correcte;
	} while (path_correcte != 's');

	if (send(sock, path, sizeof(path), 0) != sizeof(path)) {
		perror("ERROR: write path");
		return -1;
	}

	if (recv(sock, path_relatiu, sizeof(path_relatiu), 0) != sizeof(path_relatiu)) {
		perror("ERROR: read path_relatiu");
		return -1;
	}

	if (recv(sock, (char*)&error, sizeof(error), 0) != sizeof(error)) {
		perror("ERROR: write path_relatiu");
		return -1;
	}

	if (error >= 0) {
		if (recv(sock, path_absolut, sizeof(path_absolut), 0) != sizeof(path_absolut)) {
			perror("ERROR: write path_absolut");
		}
		cout << "PATH ACTUAL: " << path_absolut << endl;

	}
	else {
		perror("ERROR: path erroni");
		return -1;
	}
}

int codi_op_mkdir(SOCKET sock) {
	
	char nom_directori[255];
	char directori_correcte = 'n';
	int error;

	netejar_pantalla();

	do {
		cout << "Nom del directori a creaer: ";
		cin >> nom_directori;
		cout << "El nom del directori a crear es: \"" << nom_directori<< "\", es correcte? (s/n): ";
		fflush(stdin);
		cin >> directori_correcte;
	} while (directori_correcte != 's');

	if (send(sock, nom_directori, sizeof(nom_directori), 0) != sizeof(nom_directori)) {
		perror("ERROR: write nom directori");
		return -1;
	}

	if (recv(sock, (char*)&error, sizeof(int), 0) != sizeof(int)) {
		perror("ERROR: mkdir");
		return -1;
	}

	if (error == 0) {
		cout << "Directori creat correctament" << endl;
	}
	else {
		perror("ERROR: El directori ja existeix");
	}

}

int codi_op_get(SOCKET sock) {
	
	char nom_arxiu[255];
	char directori_nom_arxiu[255];
	char arxiu_correcte = 'n';
	int error;
	bool trobat;
	FILE* arxiu;
	long size_arxiu;
	long size_llegit = 0;
	char text[255];

	netejar_pantalla();

	do {
		cout << "Nom del arxiu amb la extensio : ";
		cin >> nom_arxiu;
		cout << "El nom del arxiu es: \"" << nom_arxiu << "\", es correcte? (s/n): ";
		fflush(stdin);
		cin >> arxiu_correcte;
	} while (arxiu_correcte != 's');

	if (send(sock, nom_arxiu, sizeof(nom_arxiu), 0) != sizeof(nom_arxiu)) {
		perror("ERROR: write nom arxiu");
		return -1;
	}

	if (recv(sock, (char*)&trobat, sizeof(bool), 0) != sizeof(bool)) {
		perror("ERROR: read arxiu trobat");
		return -1;
	}

	if (trobat) {
		construir_ruta(directori_nom_arxiu, (char*)PATH_CLIENT_WINDOWS, nom_arxiu);

		if ((arxiu = fopen(directori_nom_arxiu, "w+")) < 0) {
			perror("creant arxiu");
			return -1;
		}

		if (recv(sock, (char*)&size_arxiu, sizeof(long), 0) != sizeof(long)) {
			perror("ERROR: read arxiu trobat");
			return -1;
		}

		while (size_llegit < size_arxiu) {
			if (recv(sock, text, sizeof(text), 0) != sizeof(text)) {
				perror("ERROR: read arxiu trobat");
				return -1;
			}
			fputs(text, arxiu);
			size_llegit += sizeof(text);
		}

		if (size_llegit >= size_arxiu) {
			cout << "Arxiu \"" << nom_arxiu << "\" transferit correctament" << endl;
		}

		fclose(arxiu);

	}
	else {
		perror("ERROR: l'arxiu no existeix");
	}
}

int codi_op_whoami(SOCKET sock) {
	
	char login[45];

	netejar_pantalla();

	if (recv(sock, (char*)&login, sizeof(login), 0) != sizeof(login)) {
		perror("ERROR: read nom directori");
		return 1;
	}

	cout << "Usuari connectat: " << login << endl;
}

int codi_op_stat(SOCKET sock) {

	char recurs[255];
	char recurs_correcte = 'n';
	int tipus_recurs;
	int mida;

	netejar_pantalla();

	do {
		cout << "Nom del recurs: ";
		cin >> recurs;
		cout << "El recurs es: \"" << recurs << "\", es correcte? (s/n): ";
		fflush(stdin);
		cin >> recurs_correcte;
	} while (recurs_correcte != 's');

	if (send(sock, recurs, sizeof(recurs), 0) != sizeof(recurs)) {
		perror("ERROR: write recurs");
		return -1;
	}

	if (recv(sock, (char*)&tipus_recurs, sizeof(tipus_recurs), 0) != sizeof(tipus_recurs)) {
		perror("ERROR: read tipus_recurs");
		return -1;
	}

	if (tipus_recurs != -1) {

		cout << "Tipus: ";
		if (tipus_recurs == 0) {
			cout << "Directori" << endl;
		}
		else {
			cout << "Arxiu" << endl;

			if (recv(sock, (char*)&mida, sizeof(mida), 0) != sizeof(mida)) {
				perror("ERROR: read mida");
				return -1;
			}

			cout << "Mida: " << mida << endl;

		}

	}
	else {
		cout << "Recurs no trobat" << endl;
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
						codi_op_cd(conn_socket);
						break;
					case MKDIR:
						codi_op_mkdir(conn_socket);
						break;
					case GET:
						codi_op_get(conn_socket);
						break;
					case WHOAMI:
						codi_op_whoami(conn_socket);
						break;
					case STAT:
						codi_op_stat(conn_socket);
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
