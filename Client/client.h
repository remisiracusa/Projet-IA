#ifndef client
#define client

int jouerPiece(TCoupReq* reqC, TCoupRep* repC, TCoupIA* TCoupIA, int sock, int sockIA, int numPartie);
int coupAdverse(TCoupReq* reqC, TCoupIA* TCoupIA, int sock, int sockIA);
int validationCoup(char joueur, TCoupRep* repC, int sock);

#endif
