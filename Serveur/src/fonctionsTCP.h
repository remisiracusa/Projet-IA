#ifndef TP6C_BONNAL_FONCTIONSTCP_H
#define TP6C_BONNAL_FONCTIONSTCP_H

int socketClient(char *nomMachine, ushort nPort);

void closeSocket(int sock);

int socketServeur(ushort nPort);

#endif //TP6C_BONNAL_FONCTIONSTCP_H
