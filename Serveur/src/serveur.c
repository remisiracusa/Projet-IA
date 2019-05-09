#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

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

void receivePartie(int joueurEnCours) {
	if (nbJoueurInPartie == 2) {
		perror("Erreur les 2 joueurs sont déjà connectés demande refusée\n");
		recv(sockJoueurs[joueurEnCours], &partieReq, sizeof(TPartieReq), 0); // Juste pour vider le buffer de réception

		TPartieRep partieEnCoursRep;
		partieEnCoursRep.err = ERR_PARTIE;

		err = send(sockJoueurs[joueurEnCours], &partieEnCoursRep, sizeof(TPartieRep), 0);
		if (err != sizeof(TPartieRep))
			fprintf(stderr, "Erreur dans l'envoi de reponse partie au joueur %d\n", joueurEnCours);

		return;
	}

	err = recv(sockJoueurs[joueurEnCours], &partieReq, sizeof(TPartieReq), 0);
	if (err < 0) {
		perror("Erreur dans la reception de partie\n");
	} else {
		strcpy(infoJoueur[joueurEnCours].nomJoueur, partieReq.nomJoueur);
		infoJoueur[joueurEnCours].piece = partieReq.piece;
	}

	nbJoueurInPartie++;

	if (nbJoueurInPartie == 2) {    // Dès que l'adversaire s'est connecté, envoi des confirmations aux 2 joueurs
		TPartieRep partieEnCoursRep;
		partieEnCoursRep.err = ERR_OK;
		strcpy(partieEnCoursRep.nomAdvers, infoJoueur[joueurAdverse].nomJoueur);
		partieEnCoursRep.validSensTete = infoJoueur[joueurAdverse].piece == infoJoueur[joueurEnCours].piece ? KO : OK;

		if (partieEnCoursRep.err == KO)        // Correction du sens des pieces du joueur (coté serveur uniquement)
			infoJoueur[joueurEnCours].piece = infoJoueur[joueurAdverse].piece == NORD ? SUD : NORD;

		err = send(sockJoueurs[joueurEnCours], &partieEnCoursRep, sizeof(TPartieRep), 0);
		if (err != sizeof(TPartieRep))
			fprintf(stderr, "Erreur dans l'envoi de reponse partie au joueur %d\n", joueurEnCours);

		TPartieRep partieAdversRep;
		partieAdversRep.err = ERR_OK;
		strcpy(partieAdversRep.nomAdvers, infoJoueur[joueurEnCours].nomJoueur);
		partieAdversRep.validSensTete = OK;

		err = send(sockJoueurs[joueurAdverse], &partieAdversRep, sizeof(TPartieRep), 0);
		if (err != sizeof(TPartieRep))
			fprintf(stderr, "Erreur dans l'envoi de reponse partie au joueur %d\n", joueurAdverse);

		initialiserPartie();
	}
}

void receiveCoup(int joueurEnCours) {
	err = recv(sockJoueurs[joueurEnCours], &coupReq, sizeof(TCoupReq), 0);
	if (err < 0) {
		perror("Erreur dans la reception de coup");
	} else {
		TCoupRep coupRep;
		if (coupReq.numPartie != numPartie) {
			perror("Erreur le numero de partie n'est pas identique entre le serveur et le joueur\n");
			coupRep.err = ERR_COUP;

			err = send(sockJoueurs[joueurEnCours], &coupRep, sizeof(TCoupRep), 0);
			if (err != sizeof(TCoupRep))
				fprintf(stderr, "Erreur dans l'envoi de reponse coup au joueur %d\n", joueurEnCours);
			return;
		}

		coupRep.validCoup = validationCoup((infoJoueur[joueurEnCours].piece == SUD && numPartie == 1 ? 1 : 2),
										   coupReq.typeCoup,
										   coupRep.propCoup) ? VALID : TRICHE;

		switch (coupRep.propCoup) {
			case GAGNE:
			case NUL:
			case PERDU:
				err = send(sockJoueurs[joueurEnCours], &coupRep, sizeof(TCoupRep), 0);
				if (err != sizeof(TCoupRep))
					fprintf(stderr, "Erreur dans l'envoi de reponse coup au joueur %d\n", joueurEnCours);
				timestampLastCoup = -1;
				partieFinie = true;
				break;
			case CONT:
				printf("Coup vérifié, en attente prochain coup\n");
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
	int joueurEnCours = 0;
	nbJoueurInPartie = 0;
	int sockJoueur;
	numPartie = 1;
	timestampLastCoup = -1;
	partieFinie = false;
	while (numPartie <= 2) {
		sockJoueur = sockJoueurs[joueurEnCours];

		if (partieFinie) {
			printf("Partie terminée\n");
			if (numPartie != 2) {
				printf("Début de la revanche\n");
				joueurEnCours = infoJoueur[joueurEnCours].piece != NORD ? joueurAdverse : joueurEnCours;	// Sélection du joueur NORD pour la revanche
				numPartie++;
			} else {
				printf("Jeu terminé\n");
			}
			partieFinie = false;
		}

		if (timestampLastCoup != -1 && ((clock() - timestampLastCoup) / CLOCKS_PER_SEC) >= TIME_MAX) {
			printf("Timeout atteint, au tour de l'adversaire\n");

			TCoupRep coupRep;
			coupRep.err = ERR_OK;
			coupRep.validCoup = TIMEOUT;
			err = send(sockJoueurs[joueurEnCours], &coupRep, sizeof(TCoupRep), 0);
			if (err != sizeof(TCoupRep))
				fprintf(stderr, "Erreur dans l'envoi de reponse timeout au joueur %d\n", joueurEnCours);
			err = send(sockJoueurs[joueurAdverse], &coupRep, sizeof(TCoupRep), 0);
			if (err != sizeof(TCoupRep))
				fprintf(stderr, "Erreur dans l'envoi de reponse timeout au joueur %d\n", joueurAdverse);

			printf("Début décompte\n");
			timestampLastCoup = clock();
			joueurEnCours = joueurEnCours == 0 ? 1 : 0;
		}

		err = recv(sockJoueur, &typeRequete, sizeof(TIdReq), MSG_PEEK);
		if (err < 0) {
			perror("Erreur dans la reception\n");
		} else {
			switch (typeRequete) {
				case PARTIE:
					receivePartie(joueurEnCours);
					joueurEnCours = joueurEnCours == 0 ? 1 : 0;
					break;
				case COUP:
					receiveCoup(joueurEnCours);
					joueurEnCours = joueurEnCours == 0 ? 1 : 0;
					break;
				default:
					perror("Impossible de déterminer le type de requete\n");
					TCoupRep coupRep;
					coupRep.err = ERR_TYP;
					err = send(sockJoueur, &coupRep, sizeof(TCoupRep), 0);
					if (err != sizeof(TCoupRep)) {
						perror("Erreur dans la reception impossible de déterminer le type\n");
					}
			}
		}
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