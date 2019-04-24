
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "fonctionsTCP.h"

int socketClient(char *nomMachine, ushort nPort) {

	int sock,                /* descripteur de la socket locale */
			port,                /* variables de lecture */
			err;                 /* code d'erreur */
	char *ipMachServ;        /* pour solution inet_aton */
	struct sockaddr_in addSockServ;
	/* adresse de la socket connexion du serveur */
	socklen_t sizeAdd;       /* taille d'une structure pour l'adresse de socket */

	ipMachServ = nomMachine;
	port = nPort;

	/*
	 * creation d'une socket, domaine AF_INET, protocole TCP
	 */
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

	addSockServ.sin_port = htons((uint16_t) port);
	bzero(addSockServ.sin_zero, 8);

	sizeAdd = sizeof(struct sockaddr_in);


	/*
	 * connexion au serveur
	 */
	err = connect(sock, (struct sockaddr *) &addSockServ, sizeAdd);

	if (err < 0) {
		perror("(client) erreur a la connection de socket");
		close(sock);
		return -4;
	}

	/*
	 * fermeture de la connexion et de la socket
	 */
	//shutdown(sock, SHUT_RDWR);
	//close(sock);

	return sock;
}


void closeSocket(int sock) {
	shutdown(sock, SHUT_RDWR);
	close(sock);
}

int socketServeur(ushort nPort) {
	int sockConx,        /* descripteur socket connexion */
			port,            /* numero de port */
			sizeAddr,        /* taille de l'adresse d'une socket */
			err;            /* code d'erreur */

	struct sockaddr_in addServ;    /* adresse socket connex serveur */

	port = nPort;

	/*
	 * creation de la socket, protocole TCP
	 */
	sockConx = socket(AF_INET, SOCK_STREAM, 0);
	if (sockConx < 0) {
		perror("(serveurTCP) erreur de socket");
		return -2;
	}

	/*
	 * initialisation de l'adresse de la socket
	 */
	addServ.sin_family = AF_INET;
	addServ.sin_port = htons((uint16_t) port); // conversion en format réseau (big endian)
	addServ.sin_addr.s_addr = INADDR_ANY;
	// INADDR_ANY : 0.0.0.0 (IPv4) donc htonl inutile ici, car pas d'effet
	bzero(addServ.sin_zero, 8);

	sizeAddr = sizeof(struct sockaddr_in);

	/*
	 * attribution de l'adresse a la socket
	 */

	int enable = 1;
	if (setsockopt(sockConx, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		perror("(serveurTCP) erreur sur le setsockopt");
		return -1;
	}

	err = bind(sockConx, (struct sockaddr *) &addServ, (socklen_t) sizeAddr);
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


	/*
	 * arret de la connexion et fermeture
	 */
	//shutdown(sockTrans, SHUT_RDWR); close(sockTrans);
	//close(sockConx);

	return sockConx;
}
