#ifndef client
#define client

void jouerPiece(TCoupReq* reqC, TCoupRep* repC, int sock, int numPartie);
void coupAdverse(TCoupReq* reqC, int sock);
int validationCoup(char joueur, TCoupRep* repC, int sock);

#endif
