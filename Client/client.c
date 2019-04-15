/*
 **********************************************************
 *
 *  Programme : client.c
 *
 *  resume :   Client qui a le rôle de joueur au jeu Yokai
 *
 *  ecrit par : Rémi Siracusa
 *
 *  date :      07 / 04 / 19
 *
 ***********************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include "fonctionsTCP/fonctionsTCP.h"
#include "protocolYokai.h"
#include "client.h"

int main(int argc, char **argv) {

  int sock,               		// descripteur de la socket locale
      err,                		// code d'erreur
			numPartie,							// numéro de la partie
			port,										// port du serveur
			tour,										// tour de partie
			cont = 0;								// fin de partie
  char* host;									// nom de la machine
	char nomJoueur[T_NOM];			// nom du joueur
	TSensTetePiece sensP = SUD; // Sens des pieces
	TPartieReq reqP;		 			 	// requete partie
	TPartieRep repP;						// reponse partie
	TCoupReq reqC;		 					// requete coup
	TCoupRep repC;							// reponse coup

	// verification des arguments
  if (argc != 4) {
    printf("usage : %s hostServeur portServeur nomJoueur\n", argv[0]);
    return -1;
  }
  
  host = argv[1];
  port = atoi(argv[2]);
  strcpy(nomJoueur, argv[3]);
  
	// creation d'une socket, domaine AF_INET, protocole TCP
	sock = socketClient(host, port);
  numPartie = 1;

	// demande d'une nouvelle partie
	reqP.idReq = PARTIE;
	strcpy(reqP.nomJoueur, nomJoueur);
	reqP.piece = sensP;

	// envoi de la requete partie
	err = send(sock, &reqP, sizeof(TPartieReq), 0);
	if (err != sizeof(TPartieReq)) {
		perror("(client) erreur sur le send");
		shutdown(sock, SHUT_RDWR); close(sock);
		return -5;
	}

	// reception de la reponse partie
	err = recv(sock, &repP, sizeof(TPartieRep), 0);
	if (err != sizeof(TPartieRep)) {
		perror("(client) erreur dans la reception");
		shutdown(sock, SHUT_RDWR); close(sock);
		return -6;
	}

	// gestion de la validation du sens des pieces
	if(repP.validSensTete == KO){
		if(sensP == SUD){
			sensP = NORD;
		}else{
			sensP = SUD;
		}
	}

	//initialiser IA

	//gestion de deux parties de jeu
	while(numPartie < 3){
		if((numPartie == 1 && sensP == SUD) || (numPartie == 2 && sensP == NORD)){
			tour = 1;
		}else{
			tour = 0;
		}
		
		// gestion de la partie en cours
		while(cont == 1){
			if(tour == 1){

				// mon tour de jeu
				jouerPiece(&reqC, &repC, sock, numPartie);
				cont = validationCoup('M', &repC, sock);
				if(cont != 0){	
					cont = validationCoup('A', &repC, sock);
					if(cont != 0){
						coupAdverse(&reqC, sock);
						tour = 0;
					}	
				}
			}else{

				// tour de jeu adverse
				cont = validationCoup('A', &repC, sock);
				coupAdverse(&reqC, sock);

				jouerPiece(&reqC, &repC, sock, numPartie);
				cont = validationCoup('M', &repC, sock);
				tour = 1;
			}
		}
		numPartie++;

		// réinitialiser IA
	}

	printf("(client) arret de la communication\n");

	// fermeture de la connexion et de la socket 
	shutdown(sock, SHUT_RDWR);
	close(sock);
	return 0;
}

void jouerPiece(TCoupReq* reqC, TCoupRep* repC, int sock, int numPartie){
	// demander un coup a l'IA
	// gerer le cas ou un timeout est recu
	reqC->idRequest = COUP;
	reqC->numPartie = numPartie;
	reqC->typeCoup = DEPLACER;
	reqC->piece.sensTetePiece = SUD;
	reqC->piece.typePiece = ONI;
	int p = 0; // action donnee par l'IA
	if(p == 0){

		// si on deplace une piece
		reqC->params.deplPiece.caseDep.c = A;
		reqC->params.deplPiece.caseDep.l = UN;
		reqC->params.deplPiece.caseArr.c = A;
		reqC->params.deplPiece.caseArr.l = DEUX;
		reqC->params.deplPiece.estCapt = false;
	}else{

		// si on place une piece capturee
		reqC->params.deposerPiece.c = D;
		reqC->params.deposerPiece.l = DEUX;
	}

	// envoi de la requete coup
	int err = send(sock, &reqC, sizeof(TCoupReq), 0);
	if (err != sizeof(TCoupReq)) {
		perror("(client) erreur sur le send");
		shutdown(sock, SHUT_RDWR); close(sock);
		// return -5;
	}
}

void coupAdverse(TCoupReq* reqC, int sock){

	// reception du coup adverse
	int err = recv(sock, &reqC, sizeof(TPartieReq), 0);
	if (err != sizeof(TPartieReq)) {
		perror("(client) erreur dans la reception");
		shutdown(sock, SHUT_RDWR); close(sock);
		// return -6;
	}

	// transmettre coup adverse a l'IA
}

int validationCoup(char joueur, TCoupRep* repC, int sock){

	// reception de la validation de mon coup
	int cont = 1;
	int err = recv(sock, &repC, sizeof(TCoupRep), 0);
	if (err != sizeof(TCoupRep)) {
		perror("(client) erreur dans la reception");
		shutdown(sock, SHUT_RDWR); close(sock);
		// return -6;
	}
	if(repC->propCoup != CONT){
		cont = 0;
		switch(repC->validCoup){
				case VALID :
						break;
				case TIMEOUT :
						printf("Erreur TIMEOUT\n");
						break;
				case TRICHE :
						printf("Erreur TRICHE\n");
						break;
				default :
					break;
		}
		if(joueur == 'M'){		
			switch(repC->propCoup){
				case GAGNE :
						printf("Gagne");
						break;
				case NUL :
						printf("Perdu match null");
						break;
				case PERDU :
						printf("Perdu");
						break;
				default :
					break;
			}
		}else{
			switch(repC->propCoup){
				case GAGNE :
						printf("Perdu");
						break;
				case NUL :
						printf("Perdu match null");
						break;
				case PERDU :
						printf("Gagne");
						break;
				default :
					break;
			}
		}
	}
	return cont;
}
