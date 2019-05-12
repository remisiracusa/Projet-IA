#ifndef client
#define client

//Jouer un coup
int jouerPiece(TCoupIA* TCoupIA, int sock, int sockIA, int numPartie, TSensTetePiece sensP);
//Coup adverse
int coupAdverse(TCoupIA* TCoupIA, int sock, int sockIA);
//Validation d'un coup
int validationCoup(char joueur, int sock);

#endif
