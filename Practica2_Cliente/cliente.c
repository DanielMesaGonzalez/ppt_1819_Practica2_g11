/*******************************************************
Protocolos de Transporte
Grado en Ingeniería Telemática
Dpto. Ingeníería de Telecomunicación
Univerisdad de Jaén

Fichero: cliente.c
Versión: 2.0
Fecha: 09/2018
Descripción:
	Cliente sencillo TCP para IPv4 e IPv6

Autor: 

Daniel Mesa González
Javier Sánchez Samper

*******************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <ws2tcpip.h>//Necesaria para las funciones IPv6
#include <conio.h>
#include "protocol.h"

#pragma comment(lib, "Ws2_32.lib")

int main(int *argc, char *argv[])
{
	SOCKET sockfd;
	struct sockaddr *server_in = NULL;
	struct sockaddr_in server_in4;
	struct sockaddr_in6 server_in6;
	int address_size = sizeof(server_in4);
	char buffer_in[1024], buffer_out[1024], input[1024];
	int recibidos = 0, enviados = 0;
	int estado = S_HELO;
	char remitente[10] = " ";
	char destinatario[10] = " ";
	char data[10] = " ";
	char option = "/0";
	char fecha[100] = " ";
	char mensaje[1024] = " ";
	char entrada[1024] = ".";
	char asunto[1024] = " ";
	int comprobacion = 0;
	int comprobacion2 = 0;
	int comprobacion3 = 0;
	char dests[50] = "";
	char dest[50];
	char ipdest1;


	int ipversion = AF_INET;//IPv4 por defecto
	char ipdest[256];
	char default_ip4[16] = "127.0.0.1";
	char default_ip6[64] = "::1";

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	//Inicialización Windows sockets - SOLO WINDOWS
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
		return(0);

	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		return(0);
	}
	//Fin: Inicialización Windows sockets

	printf("**************\r\nCLIENTE TCP SENCILLO SOBRE IPv4 o IPv6\r\n*************\r\n");


	do {

		printf("CLIENTE> ¿Qué versión de IP desea usar? 6 para IPv6, 4 para IPv4 [por defecto] ");
		gets_s(ipdest, sizeof(ipdest));

		if (strcmp(ipdest, "6") == 0) {
			ipversion = AF_INET6;

		}
		else { //Distinto de 6 se elige la versión 4
			ipversion = AF_INET;
		}

		sockfd = socket(ipversion, SOCK_STREAM, 0);
		if (sockfd == INVALID_SOCKET) {
			printf("CLIENTE> ERROR\r\n");
			exit(-1);
		}
		else {
		//	struct  in_addr address;

			printf("CLIENTE> Introduzca la IP destino (pulsar enter para IP por defecto): ");
			gets_s(ipdest, sizeof(ipdest));
			ipdest1 = inet_addr(ipdest);

		/*	if (ipdest1 == INADDR_NONE) {
				struct hostent*host;
				host = gethostbyname(ipdest);
				if (host != NULL);{
				memcpy(&address, host->h_addr_list[0], 4);
				printf("\nDireccion%s\n ", inet_ntoa(address));
			}
			strcpy_s(ipdest, sizeof(ipdest), inet_ntoa(address));
		}*/
			//Dirección por defecto según la familia
			if (strcmp(ipdest, "") == 0 && ipversion == AF_INET)
				strcpy_s(ipdest, sizeof(ipdest), default_ip4);

			if (strcmp(ipdest, "") == 0 && ipversion == AF_INET6)
				strcpy_s(ipdest, sizeof(ipdest), default_ip6);

			if (ipversion == AF_INET) {
				server_in4.sin_family = AF_INET;
				server_in4.sin_port = htons(TCP_SERVICE_PORT);
				//server_in4.sin_addr.s_addr=inet_addr(ipdest);
				inet_pton(ipversion, ipdest, &server_in4.sin_addr.s_addr);
				server_in = (struct sockaddr*)&server_in4;
				address_size = sizeof(server_in4);
			}

			if (ipversion == AF_INET6) {
				memset(&server_in6, 0, sizeof(server_in6));
				server_in6.sin6_family = AF_INET6;
				server_in6.sin6_port = htons(TCP_SERVICE_PORT);
				inet_pton(ipversion, ipdest, &server_in6.sin6_addr);
				server_in = (struct sockaddr*)&server_in6;
				address_size = sizeof(server_in6);
			}

			estado = S_HELO;

			if (connect(sockfd, server_in, address_size) == 0) {
				printf("CLIENTE> CONEXION ESTABLECIDA CON %s:%d\r\n", ipdest, TCP_SERVICE_PORT);
					//Inicio de la máquina de estados
				recibidos = recv(sockfd, buffer_in, 512, 0);
				buffer_in[recibidos] = 0x00;
				printf(buffer_in);
				do {
					switch (estado) {
					case S_HELO:

						sprintf_s(buffer_out, sizeof(buffer_out), "%s%s%s", HELO, SP, CRLF);
						estado++;
						break;

					case S_RSET:
						estado=S_RCPT;
				
					case S_MAIL_FROM:    //REMITENTE
						printf("Introduce remitente: ");
						gets_s(input, sizeof(input));
						//-- implementacion RSET
						if (strlen(input) == 0) {
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", SD, CRLF);
							estado = S_QUIT;
						}
						else if (strncmp(input, "RSET", 4) == 0) {
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", "RSET", CRLF);
							estado = S_RSET;
						}
						else {
							strcpy_s(remitente, sizeof(remitente), input);
							sprintf_s(buffer_out, sizeof(buffer_out), "MAIL FROM: <%s>%s", remitente, CRLF);

						}
						estado++;
						break;

					case S_RCPT:      //DESTINATARIO
						printf("Introducir destinatario:");
						gets_s(input, sizeof(input));
						if (strlen(input) == 0) {
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", SD, CRLF);
							estado = S_QUIT;
						}

						//-- implementacion RSET
						else if (strncmp(destinatario, "RSET", 4) == 0){
							estado = S_RSET;
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", "RSET", CRLF);
							break;
						}
						else {
							strcpy_s(destinatario, sizeof(destinatario), input);
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s%s%s%s%s", RCPT, SP, TO, SP, destinatario, CRLF);
						}
						//--
						printf("¿Quieres introducir otro destinatario? (S/N): \r\n");
						option = _getche();
						if (option == 'N' || option == 'n') {
							estado++;
						}
						break;

					case S_DATA:
						sprintf_s(buffer_out, sizeof(buffer_out), "DATA%s", CRLF);
						estado++;
						break;

					case S_MENSAJE:
						printf("Introduce el asunto: ");
						gets_s(asunto, sizeof(asunto));

						/*time_t t;
						struct tm *tm;
						char fechayhora[100];

						t = time(NULL);
						tm = localtime(&t);
						strftime(fechayhora, 100., "%H:%M:%S %d/%m/%Y", tm);
						strcpy_s(fecha, sizeof(fecha), fechayhora);*/

						// CABECERA DEL MENSAJE

						sprintf_s(mensaje, sizeof(mensaje), "DATE: %s%sSUBJECT: %s%sFROM: %s%sTO: %s%s%s", fecha, CRLF, asunto, CRLF, dests, CRLF, remitente, CRLF, CRLF);
						printf("\n MENSAJE: (inserta . para terminar)\r\n");

						do {
							gets(entrada);
							sprintf_s(mensaje, sizeof(mensaje), "%s%s%s", mensaje, CRLF, entrada);
						} while (strcmp(entrada, ".") != 0);
						sprintf_s(buffer_out, sizeof(mensaje), "%s%s", mensaje, CRLF);
						
						break;

					}

					
						enviados = send(sockfd, buffer_out, (int)strlen(buffer_out), 0);
						if (enviados <0) {
							DWORD error = GetLastError();
							printf("CLIENTE> Error %d en el envío de datos\r\n", error,CRLF);
							break;
						}
					

					recibidos = recv(sockfd, buffer_in, 512, 0);
					if (recibidos <= 0) {
						DWORD error = GetLastError();
						if (recibidos < 0) {
							printf("CLIENTE> Error %d en la recepción de datos\r\n", error);
							estado = S_QUIT;
						}
						else {
							printf("CLIENTE> Conexión con el servidor cerrada\r\n");
							estado = S_QUIT;
						}
					}
					else {
						buffer_in[recibidos] = 0x00;
						printf(buffer_in);
						//-- Implementacion de errores con switch
						switch (estado) {

						case S_MAIL_FROM:
							if (strncmp(buffer_in, "250", 3) == 0) {
								//estado++;
							}
							break;

						case S_RCPT:

							if (strncmp(buffer_in, "250", 3) == 0) {
								if (strcmp(dests, "") != 0) {
									strcat(dests, ",");
									strcat(dests, destinatario);
								}

								if (strcmp(dests, "") == 0) {
									strcpy_s(dests, sizeof(dests), destinatario);
								}
							}

							if (strncmp(buffer_in, "554", 3) == 0) {
								printf("Destinatario Incorrecto, vuelve a introducirlo: \r\n");
								estado = S_MAIL_FROM;
							}
							break;
							

						case S_DATA:
							break;

						case S_MENSAJE:
							if (strncmp(buffer_in, "250", 3) == 0) {
								printf("Mensaje enviado correctamente.\r\n");
								printf("Para escribir otro mensaje pulsa (S/N): \r\n");
								option = _getche();
								if (option == 'S' || option == 's') {
									estado = S_RSET;
								}
								else {
									estado = S_QUIT;
								}
							}
							break;

						}
					}

				} while (estado != S_QUIT);
				}

			else {
				int error_code = GetLastError();
				printf("CLIENTE> ERROR AL CONECTAR CON %s:%d\r\n", ipdest, TCP_SERVICE_PORT);
			}
			// fin de la conexion de transporte
			closesocket(sockfd);

			}
			printf("-----------------------\r\n\r\nCLIENTE> Volver a conectar (S/N)\r\n");
			option = _getche();

		}while (option != 'n' && option != 'N');

		return(0);
}
