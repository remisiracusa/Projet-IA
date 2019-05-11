//
// Created by Valentin on 07/04/2019.
//

#ifndef SERVEUR_SERVEUR_H
#define SERVEUR_SERVEUR_H

#include "fonctionsTCP.h"
#include "protocolYokai.h"

int sockConx;
int sizeAddr;
int sockJoueurs[2];
int nbJoueurInPartie;
int joueurEnCours;
clock_t timestampLastCoup;
bool partieFinie;

int numPartie;

typedef struct {
	char nomJoueur[T_NOM];
	TSensTetePiece sensTetePiece;
} InfoJoueur;

InfoJoueur infoJoueur[2];
ssize_t err;

struct sockaddr_in addClient;
TIdReq typeRequete;
TPartieReq partieReq;
TCoupReq coupReq;

void connexion(char **argv);

void attenteJoueurs();

void receivePartie();

void receiveCoup();

void loop();

#endif //SERVEUR_SERVEUR_H
