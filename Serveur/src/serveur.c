#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/ioctl.h>

#include "validation.h"
#include "serveur.h"

#define joueurAdverse ((joueurEnCours == 0) ? 1 : 0)
#define TIME_MAX 6

void connexion(char **argv) {
	sockConx = socketServeur((ushort) atoi(argv[1])); // NOLINT(cert-err34-c)
	if (sockConx < 0) {
		perror("Erreur de création du socket, fermeture du serveur\n");
		exit(-1);
	}
}

void attenteJoueurs() {
	for (int nbJoueursConnect = 0; nbJoueursConnect < 2; nbJoueursConnect++) {
		if (nbJoueursConnect < 1) printf("Attente de connexion du premier joueur\n");
		else printf("Attente de connexion du second joueur\n");

		sizeAddr = sizeof(struct sockaddr_in);
		sockJoueurs[nbJoueursConnect] = accept(sockConx,
											   (struct sockaddr *) &addClient,
											   (socklen_t *) &sizeAddr);
		if (sockJoueurs[nbJoueursConnect] < 0) {
			perror("Erreur sur accept\n");
			exit(-2);
		}

		if (nbJoueursConnect < 1) printf("Premier joueur connecté\n");
		else printf("Second joueur connecté\n");
	}
}

void receivePartie() {
	if (nbJoueurInPartie == 2) {
		perror("Erreur les 2 joueurs sont déjà connectés demande refusée\n");
		recv(sockJoueurs[joueurEnCours], &partieReq, sizeof(TPartieReq), 0); // Juste pour vider le buffer de réception

		TPartieRep partieEnCoursRep;
		partieEnCoursRep.err = ERR_PARTIE;

		err = send(sockJoueurs[joueurEnCours], &partieEnCoursRep, sizeof(TPartieRep), 0);
		if (err != sizeof(TPartieRep))
			fprintf(stderr, "Erreur dans l'envoi de reponse partie au joueur %s\n",
					infoJoueur[joueurEnCours].nomJoueur);

		joueurEnCours = joueurEnCours == 0 ? 1 : 0;
		return;
	}

	err = recv(sockJoueurs[joueurEnCours], &partieReq, sizeof(TPartieReq), 0);
	if (err < 0) {
		perror("Erreur dans la reception de partie\n");
	} else {
		strcpy(infoJoueur[joueurEnCours].nomJoueur, partieReq.nomJoueur);
		infoJoueur[joueurEnCours].sensTetePiece = partieReq.piece;
	}

	nbJoueurInPartie++;

	if (nbJoueurInPartie == 2) {    // Dès que l'adversaire s'est connecté, envoi des confirmations aux 2 joueurs
		TPartieRep partieEnCoursRep;
		partieEnCoursRep.err = ERR_OK;
		strcpy(partieEnCoursRep.nomAdvers, infoJoueur[joueurAdverse].nomJoueur);
		partieEnCoursRep.validSensTete =
				infoJoueur[joueurAdverse].sensTetePiece == infoJoueur[joueurEnCours].sensTetePiece ? KO : OK;

		if (partieEnCoursRep.validSensTete ==
			KO)        // Correction du sens des pieces du joueur (coté serveur uniquement)
			infoJoueur[joueurEnCours].sensTetePiece = infoJoueur[joueurEnCours].sensTetePiece == NORD ? SUD : NORD;

		err = send(sockJoueurs[joueurEnCours], &partieEnCoursRep, sizeof(TPartieRep), 0);
		if (err != sizeof(TPartieRep))
			fprintf(stderr, "Erreur dans l'envoi de reponse partie au joueur %d\n",
					joueurEnCours);

		TPartieRep partieAdversRep;
		partieAdversRep.err = ERR_OK;
		strcpy(partieAdversRep.nomAdvers, infoJoueur[joueurEnCours].nomJoueur);
		partieAdversRep.validSensTete = OK;

		err = send(sockJoueurs[joueurAdverse], &partieAdversRep, sizeof(TPartieRep), 0);
		if (err != sizeof(TPartieRep))
			fprintf(stderr, "Erreur dans l'envoi de reponse partie au joueur adverse %s\n",
					infoJoueur[joueurAdverse].nomJoueur);

		initialiserPartie();
		joueurEnCours = infoJoueur[joueurEnCours].sensTetePiece != SUD ? joueurAdverse
																	   : joueurEnCours;    // Sélection du joueur sensTête SUD pour débuter la partie
		printf("Attente premier coup joueur %s\n", infoJoueur[joueurEnCours].nomJoueur);
		return;
	}
	joueurEnCours = joueurEnCours == 0 ? 1 : 0;
}

void receiveCoup() {
	err = recv(sockJoueurs[joueurEnCours], &coupReq, sizeof(TCoupReq), 0);
	if (err < 0) {
		perror("Erreur dans la reception de coup");
	} else {
		TCoupRep coupRep;
		if (coupReq.numPartie != numPartie) {
			fprintf(stderr, "Erreur le numero de partie n'est pas identique entre le serveur et le joueur %s\n",
					infoJoueur[joueurEnCours].nomJoueur);
			coupRep.err = ERR_COUP;

			err = send(sockJoueurs[joueurEnCours], &coupRep, sizeof(TCoupRep), 0);
			if (err != sizeof(TCoupRep))
				fprintf(stderr, "Erreur dans l'envoi de reponse coup au joueur %s\n",
						infoJoueur[joueurEnCours].nomJoueur);
			return;
		}

		printf("Le joueur %s sens %s veut déplacer une piece sens %s\n", infoJoueur[joueurEnCours].nomJoueur,
			   infoJoueur[joueurEnCours].sensTetePiece == NORD ? "NORD" : "SUD",
			   coupReq.piece.sensTetePiece == NORD ? "NORD" : "SUD");

		coupRep.validCoup = validationCoup((infoJoueur[joueurEnCours].sensTetePiece == SUD && numPartie == 1 ? 0 : 1),
										   coupReq,
										   &coupRep.propCoup) ? VALID
															  : TRICHE;        // /!\ Le .h fourni est erroné, le joueur qui commence est le 0 (est non 1) et l'adverse est le 1 (est non 2)

		switch (coupRep.propCoup) {
			case GAGNE:
			case NUL:
			case PERDU:
				err = send(sockJoueurs[joueurEnCours], &coupRep, sizeof(TCoupRep), 0);
				if (err != sizeof(TCoupRep))
					fprintf(stderr, "Erreur dans l'envoi de reponse coup au joueur %s\n",
							infoJoueur[joueurEnCours].nomJoueur);
				timestampLastCoup = -1;
				partieFinie = true;
				break;
			case CONT:
				printf("Coup vérifié pour le joueur %s, en attente prochain coup du joueur %s\n",
					   infoJoueur[joueurEnCours].nomJoueur,
					   infoJoueur[joueurAdverse].nomJoueur);
				err = send(sockJoueurs[joueurAdverse], &coupReq, sizeof(TCoupReq), 0);
				if (err != sizeof(TCoupReq)) {
					perror("Erreur dans la reception de coup\n");
				}

				printf("Début décompte\n");
				timestampLastCoup = clock();
			default:
				break;
		}
	}
}

void loop() {
	joueurEnCours = 0;
	nbJoueurInPartie = 0;
	int sockJoueur;
	numPartie = 1;
	timestampLastCoup = -1;
	partieFinie = false;

	int on = 1;
	err = ioctl(sockJoueurs[0], FIONBIO, &on);
	err = ioctl(sockJoueurs[1], FIONBIO, &on);

	while (numPartie <= 2) {
		sockJoueur = sockJoueurs[joueurEnCours];

		if (partieFinie) {
			printf("Partie terminée\n");
			if (numPartie != 2) {
				printf("Début de la revanche\n");
				joueurEnCours = infoJoueur[joueurEnCours].sensTetePiece != NORD ? joueurAdverse
																				: joueurEnCours;    // Sélection du joueur sensTête NORD pour la revanche
				printf("Le joueur %s commence", infoJoueur[joueurEnCours].nomJoueur);
				initialiserPartie();
				numPartie++;
			} else {
				printf("Jeu terminé\n");
			}
			partieFinie = false;
		}

		if (timestampLastCoup != -1 && ((clock() - timestampLastCoup) / CLOCKS_PER_SEC) >= TIME_MAX) {
			printf("Timeout atteint, au tour de l'adversaire %s\n", infoJoueur[joueurAdverse].nomJoueur);

			TCoupRep coupRep;
			coupRep.err = ERR_OK;
			coupRep.validCoup = TIMEOUT;
			err = send(sockJoueurs[joueurEnCours], &coupRep, sizeof(TCoupRep), 0);
			if (err != sizeof(TCoupRep))
				fprintf(stderr, "Erreur dans l'envoi de reponse timeout au joueur %s\n",
						infoJoueur[joueurEnCours].nomJoueur);
			err = send(sockJoueurs[joueurAdverse], &coupRep, sizeof(TCoupRep), 0);
			if (err != sizeof(TCoupRep))
				fprintf(stderr, "Erreur dans l'envoi de reponse timeout au joueur %s\n",
						infoJoueur[joueurAdverse].nomJoueur);

			printf("Début décompte\n");
			timestampLastCoup = clock();
			joueurEnCours = joueurEnCours == 0 ? 1 : 0;
		}


		err = recv(sockJoueur, &typeRequete, sizeof(TIdReq), MSG_PEEK);
		if (err < 0) {
			//perror("Erreur dans la reception\n");
		} else {
			switch (typeRequete) {
				case PARTIE:
					printf("Réception partie joueur %d\n", joueurEnCours);
					receivePartie();
					break;
				case COUP:
					printf("Réception coup joueur %s\n", infoJoueur[joueurEnCours].nomJoueur);
					receiveCoup();
					joueurEnCours = joueurEnCours == 0 ? 1 : 0;
					break;
				default:
					perror("Impossible de déterminer le type de requete\n");
					while (err != recv(sockJoueur, &typeRequete, sizeof(TIdReq), 0)) {
						printf("Purge de la requete incorrecte\n");
					}

					TCoupRep coupRep;
					coupRep.err = ERR_TYP;
					err = send(sockJoueur, &coupRep, sizeof(TCoupRep), 0);
					if (err != sizeof(TCoupRep)) {
						perror("Erreur dans la reception impossible de déterminer le type\n");
					}
			}
		}
		usleep(500);
	}

}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("usage : %s portServ\n", argv[0]);
		return -1;
	}

	printf("Lancement du serveur\n");
	connexion(argv);
	attenteJoueurs();
	printf("Debut de la partie\n");
	loop();
	printf("Fin du traitement\n");

	return 0;
}