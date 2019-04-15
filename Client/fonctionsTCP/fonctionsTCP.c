/*
 **********************************************************
 *
 *  Programme : fonctionsTCP.c
 *
 *
 *  date :      16 / 01 / 19
 *
 ***********************************************************
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

int socketServeur(ushort nPort){
	int  sockConx,        /* descripteur socket connexion */
       sizeAddr,        /* taille de l'adresse d'une socket */
       err;	        /* code d'erreur */	
	
	struct sockaddr_in addServ;	/* adresse socket connex serveur */

	sockConx = socket(AF_INET, SOCK_STREAM, 0);
  if (sockConx < 0) {
    perror("(serveurTCP) erreur de socket");
    return -2;
  }
  
	/*
	 * reutilisation de la socket
	 */
	int enable = 1;
	if (setsockopt(sockConx, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    printf("(serveurTCP) erreur setsockopt(SO_REUSEADDR) failed");

  /* 
   * initialisation de l'adresse de la socket 
   */
  addServ.sin_family = AF_INET;
  addServ.sin_port = htons(nPort); // conversion en format réseau (big endian)
  addServ.sin_addr.s_addr = INADDR_ANY; 
  // INADDR_ANY : 0.0.0.0 (IPv4) donc htonl inutile ici, car pas d'effet
  bzero(addServ.sin_zero, 8);
  
  sizeAddr = sizeof(struct sockaddr_in);
	
  /* 
   * attribution de l'adresse a la socket
   */  
  err = bind(sockConx, (struct sockaddr *)&addServ, sizeAddr);
  if (err < 0) {
    perror("(serveurTCP) erreur sur le bind");
    close(sockConx);
    return -3;
  }
  
  /* 
   * utilisation en socket de controle, puis attente de demandes de 
   * connexion.
   */
  err = listen(sockConx, 1);
  if (err < 0) {
    perror("(serveurTCP) erreur dans listen");
    close(sockConx);
    return -4;
  }
  return sockConx;
}

int socketClient(char* ipMachServ, ushort nPort)
{
	int sock,                /* descripteur de la socket locale */
      err;                 /* code d'erreur */
	struct sockaddr_in addSockServ;
	socklen_t sizeAdd;       /* taille d'une structure pour l'adresse de socket */

	sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("(client) erreur sur la creation de socket");
    return -2;
  }
  
  /* 
   * initialisation de l'adresse de la socket - version inet_aton
   */
  
  addSockServ.sin_family = AF_INET;
  err = inet_aton(ipMachServ, &addSockServ.sin_addr);
  if (err == 0) { 
    perror("(client) erreur obtention IP serveur");
    close(sock);
    return -3;
  }
  
  addSockServ.sin_port = htons(nPort);
  bzero(addSockServ.sin_zero, 8);
 	
  sizeAdd = sizeof(struct sockaddr_in);
			     
  /* 
   * connexion au serveur 
   */
  err = connect(sock, (struct sockaddr *)&addSockServ, sizeAdd); 

  if (err < 0) {
    perror("(client) erreur a la connection de socket");
    close(sock);
    return -4;
  }
	return sock;
}
