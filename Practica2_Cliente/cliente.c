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
#include <stdio.h>
#include <ws2tcpip.h>//Necesaria para las funciones IPv6
#include <conio.h>
#include "protocol.h"
#include <time.h>

#pragma comment(lib, "Ws2_32.lib")

int main(int *argc, char *argv[])
{
	SOCKET sockfd;
	struct sockaddr *server_in=NULL;
	struct sockaddr_in server_in4;
	struct sockaddr_in6 server_in6;
	int address_size = sizeof(server_in4);
	char buffer_in[1024], buffer_out[1024],input[1024];
	int recibidos=0,enviados=0;
	int estado=S_HELO;
	char option="0/";

	char remitente[10] = " ";
	char destinatario[10] = " ";
	char data[10] = " ";
	char fecha[100] = " ";
	char mensaje[1024] = " ";
	char cadena[1024] = ".";
	char asunto[1024] = " ";
	int comprobacion = 0;
	int comprobacion2 = 0;
	int comprobacion3 = 0;
	

	int ipversion=AF_INET;//IPv4 por defecto
	char ipdest[256];
	char ipdest1;
	char default_ip4[16]="127.0.0.1";
	char default_ip6[64]="::1";
	char fechayhora;

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
   
	//Inicialización Windows sockets - SOLO WINDOWS
	wVersionRequested=MAKEWORD(1,1);
	err=WSAStartup(wVersionRequested,&wsaData);
	if(err!=0)
		return(0);

	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1){
		WSACleanup();
		return(0);
	}
	//Fin: Inicialización Windows sockets
	
	printf("**************\r\nCLIENTE TCP SENCILLO SOBRE IPv4 o IPv6\r\n*************\r\n");
	

	do{

		printf("CLIENTE> ¿Qué versión de IP desea usar? 6 para IPv6, 4 para IPv4 [por defecto] ");
		gets_s(ipdest, sizeof(ipdest));

		if (strcmp(ipdest, "6") == 0) {
			ipversion = AF_INET6;

		}
		else { //Distinto de 6 se elige la versión 4
			ipversion = AF_INET;
		}

		sockfd=socket(ipversion,SOCK_STREAM,0);
		if(sockfd==INVALID_SOCKET){
			printf("CLIENTE> ERROR\r\n");
			exit(-1);
		}
		else{
			printf("CLIENTE> Introduzca la IP destino (pulsar enter para IP por defecto): ");
			gets_s(ipdest,sizeof(ipdest));

			//Dirección por defecto según la familia
			if(strcmp(ipdest,"")==0 && ipversion==AF_INET)
				strcpy_s(ipdest,sizeof(ipdest),default_ip4);

			if(strcmp(ipdest,"")==0 && ipversion==AF_INET6)
				strcpy_s(ipdest, sizeof(ipdest),default_ip6);

			if(ipversion==AF_INET){
				server_in4.sin_family=AF_INET;
				server_in4.sin_port=htons(TCP_SERVICE_PORT);
				//server_in4.sin_addr.s_addr=inet_addr(ipdest);
				inet_pton(ipversion,ipdest,&server_in4.sin_addr.s_addr);
				server_in=(struct sockaddr*)&server_in4;
				address_size = sizeof(server_in4);
			}

			if(ipversion==AF_INET6){
				memset(&server_in6, 0, sizeof(server_in6));
				server_in6.sin6_family=AF_INET6;
				server_in6.sin6_port=htons(TCP_SERVICE_PORT);
				inet_pton(ipversion,ipdest,&server_in6.sin6_addr);
				server_in=(struct sockaddr*)&server_in6;
				address_size = sizeof(server_in6);
			}

			estado=S_HELO;

			if (connect(sockfd, (struct sockaddr*) &server_in, sizeof(server_in)) == 0) {
				printf("CLIENTE> CONEXION ESTABLECIDA CON %s:%d\r\n", ipdest, TCP_SERVICE_PORT);
				//-------implementar recibidos--------
				recibidos = recv(sockfd, buffer_in, 512, 0);
				if (recibidos <= 0){
					DWORD error = GetLastError();
					if (recibidos < 0) {
						printf("<CLIENTE> Error %d en la recepción de datos /r/n", error);
						estado = S_QUIT;
					}
					else {
						printf("CLIENTE> Conexión con el servidor cerrada /r/n");
						estado = S_QUIT;
					}
				}
				else {
					buffer_in[recibidos] = 0x00;
					printf(buffer_in);

					//-------------------------------------------


						//Inicio de la máquina de estados
					do {
						switch (estado) {
						case S_HELO:

							//-------------------------------------
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s%s%s", HELO, SP, SERVER, CRLF);
							//-------------------------------------
							break;

						case S_MAIL_FROM:
							printf("Introdude remitente(rset para limpiar): ");
							gets_s(remitente, sizeof(remitente));
							//---------------immplentación rset-----------------------------
							if (strncmp(remitente, "rset", 4) == 0) {
								sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", remitente, CRLF);
							}
							else {
								sprintf_s(buffer_out, sizeof(buffer_out), "%s%s%s%s%s", RCPT, SP, TO, destinatario, CRLF);
							}
							break;

						case S_RCPT:
							printf("Introducir destinatario(rset para limpiar):");
							gets_s(destinatario, sizeof(destinatario));
							//----------------immplentaion rset-------------------
							if (strncmp(remitente, "rset", 4) == 0) {
								sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", remitente, CRLF);
							}
							else {
								sprintf_s(buffer_out, sizeof(buffer_out), "%s%s%s%s%s", RCPT, SP, TO, destinatario, CRLF);
							}
							break;

						case S_DATA:
							sprintf_s(buffer_out, sizeof(buffer_out), "DATA%s", CRLF);
							break;

						case S_MENSAJE:
							printf("Introduce el asunto: ");
							gets_s(asunto, sizeof(asunto));
							sprintf_s(mensaje, sizeof(mensaje), "DATE: %s%sSUBJECT: %s%sFROM: %s%sTO: %s%s%s", fecha, CRLF, asunto, CRLF, destinatario, CRLF, remitente, CRLF, CRLF);
							sprintf_s(buffer_out, sizeof(buffer_out), "%s", mensaje);

							enviados = send(sockfd, buffer_out, (int)strlen(buffer_out), 0);
							printf("Introducir mensaje para enviar (para finalizar introducir un punto(.)): ");

							do {
								gets_s(cadena, sizeof(cadena));
								sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", cadena, CRLF);
								enviados = send(sockfd, buffer_out, (int)strlen(buffer_out), 0);
							} while (strcmp(cadena, ".") != 0);

							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", cadena, CRLF);
							break;

						}


						enviados = send(sockfd, buffer_out, (int)strlen(buffer_out), 0);
						if (enviados == SOCKET_ERROR) {
							estado = S_QUIT;
							continue;
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
							if (strcmp(buffer_in, "5", 1) == 0) {
								printf("Cerrando el servidor");
								estado = S_QUIT;
							}
							else {
								if (estado == S_HELO && strcmp(buffer_in, "2", 1) == 0) {
									estado++;
								}
								else if (estado == S_MAIL_FROM && strcmp(buffer_in, "2", 1) == 0) {
									if (strncmp(buffer_out, "rset", 4) == 0) {
										estado = S_HELO;
									}
									else {
										estado++;
									}
								}
								else if (estado == S_RCPT) {
									if (strncmp(buffer_in, "2", 1) == 0) {
										if (strncmp(buffer_out, "rset", 4) == 0) {
											estado = S_HELO;
										}
										else {
											printf("¿Desea cambiar el remitente?");
											do {
												option = _getche();
												printf("%s", CRLF);
												if (option == 'S' || option == 's') {
													estado = S_RCPT;
													comprobacion3 = 1;
												}
												else if (option == 'N' || option == 'n') {
													estado++;
													comprobacion3 = 1;
												}
												else {
													printf("Incorrecto,vuelve a introducir una opcion (S/N)%s", CRLF);
													comprobacion3 = 0;
												}
											} while (comprobacion3 == 0);
										}
									}
									else {
										printf("ERROR, no existe un destinatario en el servidor%s", CRLF);
										estado = S_RCPT;
									}
								}
								else if (estado == S_DATA && strncmp(buffer_in, "3", 1) == 0) {
									estado++;
								}
								else if (estado == S_MENSAJE) {
									if (strcmp(cadena, ".") == 0 && strncmp(buffer_in, "2", 1) == 0) {
										printf("¿Desea enviar otro correo (S/N)?: ");

										do {
											option = _getche();
											printf("%s", CRLF);
											if (option == 'S' || option == 's') {
												estado = S_MAIL_FROM;
												comprobacion = 1;
											}
											else if (option == 'N' || option == 'n') {
												estado++;
												comprobacion = 1;
											}
											else {
												printf("opcion no permitida volver a introducir%s", CRLF);
												comprobacion = 0;
											}
										} while (comprobacion == 0);
									}
									else {
										estado = S_MENSAJE;
									}
								}
								else {}
							}
						}

					} while (estado != S_QUIT);
				}
			}
			else {
				int error_code=GetLastError();
				printf("CLIENTE> ERROR AL CONECTAR CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);
			}		
			// fin de la conexion de transporte
			closesocket(sockfd);
			
		}	
		printf("-----------------------\r\n\r\nCLIENTE> Volver a conectar (S/N)\r\n");
		option=_getche();

	}while(option!='n' && option!='N');

	return(0);
}
