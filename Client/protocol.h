#ifndef protocol
#define protocol

//Taille du buffer
#define TAIL_BUF 256
//Numero de port
#define PORT 2323
//Sens des pieces
#define T_NORD 0
#define T_SUD 1
//Code requete moteur IA
#define INIT 0
//Code reponse moteur IA
#define T_DEPLACER 0
#define T_DEPOSER 1
#define T_AUCUN 2
//Type piece
#define T_KODAMA 0
#define T_KODAMA_SAMOURAI 1
#define T_KIRIN 2
#define T_KOROPOKKURU 3
#define T_ONI 4
#define T_SUPER_ONI 5
//Ligne
#define T_UN 0
#define T_DEUX 1
#define T_TROIS 2
#define T_QUATRE 3
#define T_CINQ 4
#define T_SIX 5
//Colonne
#define T_A 0
#define T_B 1
#define T_C 2
#define T_D 3
#define T_E 4
//Capture
#define T_N 0
#define T_O 1

//Definition de la structure d'une requete
typedef struct TPartieIA {
	int codeReq;
	int sens;
	int numPartie;
	int tour;
} TPartieIA;

//Definition de la structure d'une reponse
typedef struct TCoupIA {
	int codeRep;
	int sensPiece;
	int typePiece;
	int TlgDep;
	int TcolDep;
	int TlgArr;
	int TcolArr;
	int estCapt;
} TCoupIA;

#endif
