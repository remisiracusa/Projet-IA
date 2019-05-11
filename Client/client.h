#ifndef client
#define client

int jouerPiece(TCoupIA* TCoupIA, int sock, int sockIA, int numPartie, TSensTetePiece sensP);
int coupAdverse(TCoupIA* TCoupIA, int sock, int sockIA);
int validationCoup(char joueur, int sock);

#endif
