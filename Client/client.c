/*
 *****************************************************************************************************
 *
 *  Programme : client.c
 *
 *  resume :   Client qui a le rôle de joueur au jeu Yokai
 *
 *  ecrit par : Rémi Siracusa
 *
 *  date :      07 / 04 / 19
 *
 *****************************************************************************************************
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
#include <sys/ioctl.h>
#include "fonctionsTCP/fonctionsTCP.h"
#include "protocolYokai.h"
#include "protocol.h"
#include "client.h"

int main(int argc, char **argv) {

  int sock,               		// descripteur de la socket locale pour le serveur C
      sockIA,               	// descripteur de la socket locale pour le moteur IA
      err,                		// code d'erreur
      numPartie,				// numéro de la partie
      port,						// port du serveur
      tour,						// tour de partie
      cont;	    				// fin de partie
  char* host;					// nom de la machine
  char nomJoueur[T_NOM];		// nom du joueur
  TSensTetePiece sensP = NORD;   // Sens des pieces
  TPartieReq reqP;		 		// requete partie
  TPartieRep repP;				// reponse partie
  TCoupReq reqC;		 		// requete coup
  TCoupRep repC;				// reponse coup
  TPartieIA partieIA;			// structure de la requete avec le moteur IA
  TCoupIA coupIA;		 		// structure de la reponse avec le moteur IA

  // verification des arguments
  if (argc != 5) {
    printf("usage : %s hostServeur portServeur nomJoueur portIA\n", argv[0]);
    return -1;
  }
  
  host = argv[1];
  port = atoi(argv[2]);
  strcpy(nomJoueur, argv[3]);
  int portIA = atoi(argv[4]);
  
  // creation d'une socket, domaine AF_INET, protocole TCP
  sock = socketClient(host, port);
  numPartie = 1;


  // connexion avec le moteur IA
  sockIA = socketClient("127.0.0.1", portIA);


  // demande d'une nouvelle partie
  reqP.idReq = PARTIE;
  strcpy(reqP.nomJoueur, nomJoueur);
  reqP.piece = sensP;

  do{
      // envoi de la requete partie
      err = send(sock, &reqP, sizeof(TPartieReq), 0);
      if (err != sizeof(TPartieReq)) {
          perror("(client) erreur sur le send serveur");
          shutdown(sock, SHUT_RDWR); close(sock);
          return -5;
      }

      // reception de la reponse partie
      err = recv(sock, &repP, sizeof(TPartieRep), 0);
      if (err != sizeof(TPartieRep)) {
          perror("(client) erreur dans la reception serveur");
          shutdown(sock, SHUT_RDWR); close(sock);
          return -6;
      }
  }while(repP.err != ERR_OK);

  // gestion de la validation du sens des pieces
  if (repP.validSensTete == KO) {
      if (sensP == SUD) {
          sensP = NORD;
      }
  }

  // initialisation IA
  if(sensP == NORD){
      partieIA.sens = htonl(T_NORD);
  }else{
      partieIA.sens = htonl(T_SUD);
  }
  partieIA.codeReq = htonl(INIT);

  err = send(sockIA, &partieIA.codeReq, sizeof(int), 0);
  if (err != sizeof(int)) {
      perror("(client) erreur sur le send java");
      shutdown(sockIA, SHUT_RDWR); close(sockIA);
      shutdown(sock, SHUT_RDWR); close(sock);
      return -5;
  }

  err = send(sockIA, &partieIA.sens, sizeof(int), 0);
  if (err != sizeof(int)) {
    perror("(client) erreur sur le send");
    shutdown(sockIA, SHUT_RDWR);
    close(sockIA);
    shutdown(sock, SHUT_RDWR);
    close(sock);
    return -5;
  }

  // gestion de deux parties de jeu
  while (numPartie < 3) {
      cont = 1;
      if ((numPartie == 1 && sensP == SUD) || (numPartie == 2 && sensP == NORD)) {
          tour = 0;
      }else{
          tour = 1;
      }

      // gestion de la partie en cours
      while (cont) {
          if (tour) {
              // mon tour de jeu
              err = jouerPiece(&coupIA, sock, sockIA, numPartie, sensP);
              if(err != 0 && err != -1){
                  return err;
              }else if(err == -1){
                  // Timeout recu
                  printf("Fin de la partie\n");
                  break;
              }else{
                  printf("Mon tour attente validation\n");
                  cont = validationCoup('M', sock);
                  printf("cont = %i\n", cont);
                  if(cont != 0 && cont != 1){
                      return cont;
                  }
              }
              tour = 0;
          }else{
              // tour de jeu adverse
              printf("Tour adverse attente validation\n");
              cont = validationCoup('A', sock);
              printf("cont = %i\n", cont);
              if(cont != 0 && cont != 1){
                  return cont;
              }
              if (cont != 0) {
                  err = coupAdverse(&coupIA, sock, sockIA);
                  if(err != 0){
                      return err;
                  }
              }else{
                  printf("Fin de la partie\n");
                  break;
              }
              tour = 1;
          }
          printf("Tour suivant\n");
      }
      printf("Partie suivante\n");
      numPartie++;
      partieIA.codeReq = htonl(INIT);

      err = send(sockIA, &partieIA.codeReq, sizeof(int), 0);
      if (err != sizeof(int)) {
          perror("(client) erreur sur le send");
          shutdown(sockIA, SHUT_RDWR); close(sockIA);
          return -5;
      }
  }

  printf("(client) arret de la communication\n");

  // fermeture de la connexion et de la socket
  shutdown(sock, SHUT_RDWR);
  shutdown(sockIA, SHUT_RDWR);
  close(sock);
  close(sockIA);
  return 0;
}

int jouerPiece(TCoupIA* coupIA, int sock, int sockIA, int numPartie, TSensTetePiece sensP){
    int err;
	int err2 = 0;
	int on = 1;
	int off = 0;
    TCoupRep repC;
    TCoupReq reqC;

	//Rendre socket avec le serveur non bloquante
    err = ioctl(sock,FIONBIO,&on);
    if(err!=0){
        perror("erreur ioctl");
        return -6;
    }

	// Recevoir un coup de l'IA
	do{
		// verification si timeout recu
		err2 = recv(sock, &repC, sizeof(TCoupRep), MSG_PEEK);
		if(err2 == sizeof(TCoupRep)){
		    recv(sock, &repC, sizeof(TCoupRep),0);
			return -1;
		}
		err = recv(sockIA, &coupIA->codeRep, sizeof(int), MSG_PEEK);
	}while(err != sizeof(int));
    err = recv(sockIA, &coupIA->codeRep, sizeof(int), 0);
	if (err != sizeof(int)) {
		perror("(client) erreur dans la reception");
		shutdown(sockIA, SHUT_RDWR); close(sockIA);
		return -6;
	}
	coupIA->codeRep = htonl(coupIA->codeRep);
    reqC.idRequest = COUP;
	if (coupIA->codeRep != T_AUCUN) {
		/*do{
			// verification si timeout recu
			err2 = recv(sock, &repC, sizeof(TCoupRep), MSG_PEEK);
			if(err2 == sizeof(TCoupRep)){
                recv(sock, &repC, sizeof(TCoupRep), 0);
				return -1;
			}
			err = recv(sockIA, &coupIA->sensPiece, sizeof(int), MSG_PEEK);
		}while(err != sizeof(int));
        recv(sockIA, &coupIA->sensPiece, sizeof(int), 0);
		if (err != sizeof(int)) {
			perror("(client) erreur dans la reception");
			shutdown(sockIA, SHUT_RDWR); close(sockIA);
			return -6;
		}
		coupIA->sensPiece = ntohl(coupIA->sensPiece);
		if (coupIA->sensPiece == 0) {
			reqC->piece.sensTetePiece = NORD;
		}else{
			reqC->piece.sensTetePiece = SUD;
		}*/
        reqC.piece.sensTetePiece = sensP;
		do{
			// verification si timeout recu
			err2 = recv(sock, &repC, sizeof(TCoupRep), MSG_PEEK);
			if(err2 == sizeof(TCoupRep)){
                recv(sock, &repC, sizeof(TCoupRep), 0);
				return -1;
			}
			err = recv(sockIA, &coupIA->typePiece, sizeof(int), MSG_PEEK);
		}while(err != sizeof(int));
        recv(sockIA, &coupIA->typePiece, sizeof(int), 0);
		if (err != sizeof(int)) {
			perror("(client) erreur dans la reception");
			shutdown(sockIA, SHUT_RDWR); close(sockIA);
			return -6;
		}
		coupIA->typePiece = ntohl(coupIA->typePiece);
	
		switch (coupIA->typePiece) {
			case T_KODAMA:
				reqC.piece.typePiece = KODAMA;
				break;

			case T_KODAMA_SAMOURAI:
				reqC.piece.typePiece = KODAMA_SAMOURAI;
				break;

			case T_KIRIN:
				reqC.piece.typePiece = KIRIN;
				break;

			case T_KOROPOKKURU:
				reqC.piece.typePiece = KOROPOKKURU;
				break;

			case T_ONI:
				reqC.piece.typePiece = ONI;
				break;

			case T_SUPER_ONI:
				reqC.piece.typePiece = SUPER_ONI;
				break;

			default :
				break;
		}

		do{
			// verification si timeout recu
			err2 = recv(sock, &repC, sizeof(TCoupRep), MSG_PEEK);
			if(err2 == sizeof(TCoupRep)){
                recv(sock, &repC, sizeof(TCoupRep), 0);
				return -1;
			}
			err = recv(sockIA, &coupIA->TlgDep, sizeof(int), MSG_PEEK);
		}while(err != sizeof(int));
        err = recv(sockIA, &coupIA->TlgDep, sizeof(int), 0);
		if (err != sizeof(int)) {
			perror("(client) erreur dans la reception");
			shutdown(sockIA, SHUT_RDWR); close(sockIA);
			return -6;
		}

		coupIA->TlgDep = ntohl(coupIA->TlgDep);

        switch (coupIA->TlgDep) {
            case T_UN:
                reqC.params.deplPiece.caseDep.l = UN;
                break;

            case T_DEUX:
                reqC.params.deplPiece.caseDep.l = DEUX;
                break;

            case T_TROIS:
                reqC.params.deplPiece.caseDep.l = TROIS;
                break;

            case T_QUATRE:
                reqC.params.deplPiece.caseDep.l = QUATRE;
                break;

            case T_CINQ:
                reqC.params.deplPiece.caseDep.l = CINQ;
                break;

            case T_SIX:
                reqC.params.deplPiece.caseDep.l = SIX;
                break;

            default :
                break;
        }

		do{
			// verification si timeout recu
			err2 = recv(sock, &repC, sizeof(TCoupRep), MSG_PEEK);
			if(err2 == sizeof(TCoupRep)){
                recv(sock, &repC, sizeof(TCoupRep), 0);
				return -1;
			}
			err = recv(sockIA, &coupIA->TcolDep, sizeof(int), MSG_PEEK);
		}while(err != sizeof(int));
        err = recv(sockIA, &coupIA->TcolDep, sizeof(int), 0);
		if (err != sizeof(int)) {
			perror("(client) erreur dans la reception");
			shutdown(sockIA, SHUT_RDWR); close(sockIA);
			return -6;
		}

		coupIA->TcolDep = ntohl(coupIA->TcolDep);
        switch (coupIA->TcolDep) {
            case T_A:
                reqC.params.deplPiece.caseDep.c = A;
                break;

            case T_B:
                reqC.params.deplPiece.caseDep.c = B;
                break;

            case T_C:
                reqC.params.deplPiece.caseDep.c = C;
                break;

            case T_D:
                reqC.params.deplPiece.caseDep.c = D;
                break;

            case T_E:
                reqC.params.deplPiece.caseDep.c = E;
                break;

            default :
                break;
        }

		if (coupIA->codeRep == T_DEPLACER) {
			reqC.typeCoup = DEPLACER;
			do{
				// verification si timeout recu
				err2 = recv(sock, &repC, sizeof(TCoupRep), MSG_PEEK);
				if(err2 == sizeof(TCoupRep)){
                    recv(sock, &repC, sizeof(TCoupRep), 0);
					return -1;
				}
				err = recv(sockIA, &coupIA->TlgArr, sizeof(int), MSG_PEEK);
			}while(err != sizeof(int));
            err = recv(sockIA, &coupIA->TlgArr, sizeof(int), 0);
			if (err != sizeof(int)) {
				perror("(client) erreur dans la reception");
				shutdown(sockIA, SHUT_RDWR); close(sockIA);
				return -6;
			}

			coupIA->TlgArr = ntohl(coupIA->TlgArr);

            switch (coupIA->TlgArr) {
                case T_UN:
                    reqC.params.deplPiece.caseArr.l = UN;
                    break;

                case T_DEUX:
                    reqC.params.deplPiece.caseArr.l = DEUX;
                    break;

                case T_TROIS:
                    reqC.params.deplPiece.caseArr.l = TROIS;
                    break;

                case T_QUATRE:
                    reqC.params.deplPiece.caseArr.l = QUATRE;
                    break;

                case T_CINQ:
                    reqC.params.deplPiece.caseArr.l = CINQ;
                    break;

                case T_SIX:
                    reqC.params.deplPiece.caseArr.l = SIX;
                    break;

                default :
                    break;
            }



			do{
				// verification si timeout recu
				err2 = recv(sock, &repC, sizeof(TCoupRep), MSG_PEEK);
				if(err2 == sizeof(TCoupRep)){
                    recv(sock, &repC, sizeof(TCoupRep), 0);
					return -1;
				}
				err = recv(sockIA, &coupIA->TcolArr, sizeof(int), MSG_PEEK);
			}while(err != sizeof(int));
            err = recv(sockIA, &coupIA->TcolArr, sizeof(int), 0);
			if (err != sizeof(int)) {
				perror("(client) erreur dans la reception");
				shutdown(sockIA, SHUT_RDWR); close(sockIA);
				return -6;
			}

			coupIA->TcolArr = ntohl(coupIA->TcolArr);
            switch (coupIA->TcolArr) {
                case T_A:
                    reqC.params.deplPiece.caseArr.c = A;
                    break;

                case T_B:
                    reqC.params.deplPiece.caseArr.c = B;
                    break;

                case T_C:
                    reqC.params.deplPiece.caseArr.c = C;
                    break;

                case T_D:
                    reqC.params.deplPiece.caseArr.c = D;
                    break;

                case T_E:
                    reqC.params.deplPiece.caseArr.c = E;
                    break;

                default :
                    break;
            }
			do{
				// verification si timeout recu
				err2 = recv(sock, &repC, sizeof(TCoupRep), MSG_PEEK);
				if(err2 == sizeof(TCoupRep)){
                    recv(sock, &repC, sizeof(TCoupRep), 0);
					return -1;
				}
				err = recv(sockIA, &coupIA->estCapt, sizeof(int), MSG_PEEK);
			}while(err != sizeof(int));
            err = recv(sockIA, &coupIA->estCapt, sizeof(int), 0);
			if (err != sizeof(int)) {
				perror("(client) erreur dans la reception");
				shutdown(sockIA, SHUT_RDWR); close(sockIA);
				return -6;
			}

			coupIA->estCapt = ntohl(coupIA->estCapt);
			if (coupIA->estCapt) {
				reqC.params.deplPiece.estCapt = true;
			}else{
				reqC.params.deplPiece.estCapt = false;
			}

		}else if (coupIA->codeRep == T_DEPLACER) {
			reqC.typeCoup = DEPOSER;
		}
	}else{
		// Aucun
		reqC.typeCoup = AUCUN;
	}
    reqC.numPartie = numPartie;
    printf("%u\n",reqC.idRequest);
    printf("%u\n",reqC.numPartie);
    printf("%u\n",reqC.typeCoup);
    printf("%u\n",reqC.piece.sensTetePiece);
    printf("%u\n",reqC.piece.typePiece);
    printf("%u\n",reqC.params.deplPiece.caseDep.c);
    printf("%u\n",reqC.params.deplPiece.caseDep.l);
    printf("%u\n",reqC.params.deplPiece.caseArr.c);
    printf("%u\n",reqC.params.deplPiece.caseArr.l);
    printf("%u\n",reqC.params.deplPiece.estCapt);

    // envoi de la requete coup au serveur
	err = send(sock, &reqC, sizeof(TCoupReq), 0);
	if (err != sizeof(TCoupReq)) {
		perror("(client) erreur sur le send");
		shutdown(sock, SHUT_RDWR); close(sock);
		return -5;
	}

    // rendre socket avec le serveur bloquante
    err = ioctl(sock,FIONBIO,&off);
    if(err!=0){
        perror("erreur ioctl");
        return -6;
    }

	return 0;
}

int coupAdverse(TCoupIA* coupIA, int sock, int sockIA){
    TCoupReq reqC;
	// reception du coup adverse
	int err = recv(sock, &reqC, sizeof(TCoupReq), 0);
	if (err != sizeof(TCoupReq)) {
		perror("(client) erreur dans la reception");
		shutdown(sock, SHUT_RDWR); close(sock);
		return -6;
	}

	// transmission coup adverse a l'IA
	switch (reqC.typeCoup) {
		case DEPLACER :
				coupIA->codeRep = htonl(T_DEPLACER);
				break;

		case DEPOSER :
				coupIA->codeRep = htonl(T_DEPOSER);
				break;

		case AUCUN :
				coupIA->codeRep = htonl(T_AUCUN);
				break;

		default :
			break;
	}

	err = send(sockIA, &coupIA->codeRep, sizeof(int), 0);
	if (err != sizeof(int)) {
		perror("(client) erreur sur le send");
		shutdown(sockIA, SHUT_RDWR); close(sockIA);
		return -5;
	}
	
	if(reqC.typeCoup != AUCUN){
		switch(reqC.piece.typePiece) {
			case KODAMA:
				coupIA->typePiece = htonl(T_KODAMA);
				break;

			case KODAMA_SAMOURAI:
				coupIA->typePiece = htonl(T_KODAMA_SAMOURAI);
				break;

			case KIRIN:
				coupIA->typePiece = htonl(T_KIRIN);
				break;

			case KOROPOKKURU:
				coupIA->typePiece = htonl(T_KOROPOKKURU);
				break;

			case ONI:
				coupIA->typePiece = htonl(T_ONI);
				break;

			case SUPER_ONI:
				coupIA->typePiece = htonl(T_SUPER_ONI);
				break;

			default :
				break;
		}

		if (reqC.piece.sensTetePiece == NORD) {
			coupIA->sensPiece = htonl(T_NORD);
		}else{
			coupIA->sensPiece = htonl(T_SUD);
		}

		err = send(sockIA, &coupIA->typePiece, sizeof(int), 0);
		if (err != sizeof(int)) {
			perror("(client) erreur sur le send");
			shutdown(sockIA, SHUT_RDWR); close(sockIA);
			return -5;
		}

		err = send(sockIA, &coupIA->sensPiece, sizeof(int), 0);
		if (err != sizeof(int)) {
			perror("(client) erreur sur le send");
			shutdown(sockIA, SHUT_RDWR); close(sockIA);
			return -5;
		}

		switch (reqC.params.deplPiece.caseDep.c) {
			case A:
				coupIA->TcolDep = htonl(T_A);
				break;

			case B:
				coupIA->TcolDep = htonl(T_B);
				break;

			case C:
				coupIA->TcolDep = htonl(T_C);
				break;

			case D:
				coupIA->TcolDep = htonl(T_D);
				break;

			case E:
				coupIA->TcolDep = htonl(T_E);
				break;

			default :
				break;
		}

		switch (reqC.params.deplPiece.caseDep.l) {
			case UN:
				coupIA->TlgDep = htonl(T_UN);
				break;

			case DEUX:
				coupIA->TlgDep = htonl(T_DEUX);
				break;

			case TROIS:
				coupIA->TlgDep = htonl(T_TROIS);
				break;

			case QUATRE:
				coupIA->TlgDep = htonl(T_QUATRE);
				break;

			case CINQ:
				coupIA->TlgDep = htonl(T_CINQ);
				break;

			case SIX:
				coupIA->TlgDep = htonl(T_SIX);
				break;

			default :
				break;
		}
			
		err = send(sockIA, &coupIA->TcolDep, sizeof(int), 0);
		if (err != sizeof(int)) {
			perror("(client) erreur sur le send");
			shutdown(sockIA, SHUT_RDWR); close(sockIA);
			return -5;
		}

		err = send(sockIA, &coupIA->TlgDep, sizeof(int), 0);
		if (err != sizeof(int)) {
			perror("(client) erreur sur le send");
			shutdown(sockIA, SHUT_RDWR); close(sockIA);
			return -5;
		}

		if (coupIA->codeRep == T_DEPLACER) {
			switch (reqC.params.deplPiece.caseArr.c) {
					case A:
						coupIA->TcolArr = htonl(T_A);
						break;

					case B:
						coupIA->TcolArr = htonl(T_B);
						break;

					case C:
						coupIA->TcolArr = htonl(T_C);
						break;

					case D:
						coupIA->TcolArr = htonl(T_D);
						break;

					case E:
						coupIA->TcolArr = htonl(T_E);
						break;

					default :
						break;
			}

			switch (reqC.params.deplPiece.caseArr.l) {
					case UN:
						coupIA->TlgArr = htonl(T_UN);
						break;

					case DEUX:
						coupIA->TlgArr = htonl(T_DEUX);
						break;

					case TROIS:
						coupIA->TlgArr = htonl(T_TROIS);
						break;

					case QUATRE:
						coupIA->TlgArr = htonl(T_QUATRE);
						break;

					case CINQ:
						coupIA->TlgArr = htonl(T_CINQ);
						break;

					case SIX:
						coupIA->TlgArr = htonl(T_SIX);
						break;

					default :
						break;
			}

			if (reqC.params.deplPiece.estCapt) {
				coupIA->estCapt = htonl(T_O);
			}else{
				coupIA->estCapt = htonl(T_N);
			}

			err = send(sockIA, &coupIA->TcolArr, sizeof(int), 0);
			if (err != sizeof(int)) {
				perror("(client) erreur sur le send");
				shutdown(sockIA, SHUT_RDWR); close(sockIA);
				return -5;
			}

			err = send(sockIA, &coupIA->TlgArr, sizeof(int), 0);
			if (err != sizeof(int)) {
				perror("(client) erreur sur le send");
				shutdown(sockIA, SHUT_RDWR); close(sockIA);
				return -5;
			}

			err = send(sockIA, &coupIA->estCapt, sizeof(int), 0);
			if (err != sizeof(int)) {
				perror("(client) erreur sur le send");
				shutdown(sockIA, SHUT_RDWR); close(sockIA);
				return -5;
			}
		}
	}
	return 0;
}

int validationCoup(char joueur, int sock){
	// reception de la validation d'un coup
	TCoupRep repC;
	int cont = 1;
	int err = recv(sock, &repC, sizeof(TCoupRep), 0);
	if (err != sizeof(TCoupRep)) {
		perror("(client) erreur dans la reception");
		shutdown(sock, SHUT_RDWR); close(sock);
		return -6;
	}
    printf("repC.err : %u\n", repC.err);
	if(repC.err != ERR_OK){
		cont = 0;
		printf("validCoup : %u\n", repC.validCoup);
		switch (repC.validCoup) {
				case VALID :
						break;
				        printf("VAL²ID\n");

				case TIMEOUT :
						printf("Erreur TIMEOUT\n");
						break;

				case TRICHE :
						printf("Erreur TRICHE\n");
						break;

				default :
					break;
		}
		if (joueur == 'M') {		
			switch (repC.propCoup) {
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
			switch (repC.propCoup) {
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