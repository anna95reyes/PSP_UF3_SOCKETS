#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include <cstring>
#include <string>

#define MAX_CLIENTS 5
#define PORT 1115
#define BACK_LOG 10
#define PATH "/mnt/hgfs/UF3/1_Sockets/Practica"
#define PATH_FITXER_IPS "/mnt/hgfs/UF3/1_Sockets/Practica/ips_valides.txt"
#define PATH_SERVER "/mnt/hgfs/UF3/1_Sockets/Practica/directori_server"
#define PATH_CLIENT "/mnt/hgfs/UF3/1_Sockets/Practica/directori_client"
#define PATH_LOG "/mnt/hgfs/UF3/1_Sockets/Practica/log.txt"
#define IP_SERVER "192.168.1.138"

#define EXIT 0
#define LS 1
#define CD 2
#define MKDIR 3
#define GET 4
#define WHOAMI 5
#define STAT 6

using std::cout;
using std::cin;
using std::endl;
using std::string;

#pragma comment(lib, "ws2_32.lib")

char path_relatiu[255];

void netejar_pantalla() {
	system("cls");
}

string construir_ruta(string path, string ruta_origen) {
	return path + "/" + ruta_origen;
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

int main() 
{
	WSADATA wsaData;
	SOCKET conn_socket;
	struct sockaddr_in server;
	struct hostent* hp;
	int resp;


	//Inicialitzem la DLL dels sockets
	if (resp = WSAStartup(MAKEWORD(2, 0), &wsaData)) {
		cout << "Error al inicialitzar socket" << endl;
		char c = getchar();
		return -1;
	}

	//Obtenim la IP del servidor... 
	if (!(hp = (struct hostent*)gethostbyname(IP_SERVER))) {
		cout << "No s'ha trobat el servidor..." << endl;
		char c = getchar();
		WSACleanup();
		return WSAGetLastError();
	}

	// Creació del socket...
	if ((conn_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		cout << "Error al crear el socket" << endl;
		char c = getchar();
		WSACleanup();
		return WSAGetLastError();
	}

	memset(&server, 0, sizeof(server));
	memcpy(&server.sin_addr, (*hp).h_addr, (*hp).h_length);
	server.sin_family = (*hp).h_addrtype;
	server.sin_port = htons(PORT);

	// Ens connectem al servidor...
	if (connect(conn_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		cout << "Error al conectar-se amb el servidor" << endl;
		closesocket(conn_socket);
		char c = getchar();
		WSACleanup();
		return WSAGetLastError();
	}
	cout << "Conexio establida amb: " << inet_ntoa(server.sin_addr) << endl;



	

	// Cerramos el socket y liberamos la DLL de sockets
	closesocket(conn_socket);
	WSACleanup();
	return EXIT_SUCCESS;
}
