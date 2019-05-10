package projet_ia;

//importation des classes utiles à Jasper
import se.sics.jasper.*;

//pour les lectures au clavier
import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
//pour utiliser les HashMap
import java.util.*;

import projet_ia.TCoup.CodeRep;
import projet_ia.TCoup.Colonne;
import projet_ia.TCoup.Ligne;
import projet_ia.TCoup.Type;
import projet_ia.TPartie.CodeReq;
import projet_ia.TPartie.Sens;


public class Moteur_IA {
	static Sens sens;
	static int numPartie;
	static boolean tour;
	static boolean cont;

	public static void main(String[] args) {
		ServerSocket srv; //Socket de connexion
		Socket sockComm; // Socket de communication
		TPartie TPartie = new TPartie();
		TCoup TCoup = new TCoup();
		numPartie = 1;
		cont = true;
		try{
			srv = new ServerSocket(2323);
			sockComm = srv.accept();
			InputStream is = sockComm.getInputStream();
			DataInputStream dis = new DataInputStream(is);
			OutputStream os = sockComm.getOutputStream();
			DataOutputStream dos = new DataOutputStream(os);
			System.out.println("Début du serveur");

			//Initialisation
			if(dis.readInt() == 0) {
				TPartie.setCodeReq(CodeReq.INIT);
			}
			if(dis.readInt() == 0) {
				TPartie.setSens(Sens.NORD);
			}else {
				TPartie.setSens(Sens.SUD);
			}			

			System.out.println("Reçu : "+TPartie.getCodeReq()+" "+TPartie.getSens()+" "+numPartie+" "+tour);

			while(numPartie < 3) {
				if((numPartie == 1 && TPartie.getSens() == Sens.SUD) || (numPartie == 2 && TPartie.getSens() == Sens.NORD)) {
					tour = true;
				}else {
					tour = false;
				}
				//Mise à zero de la grille
				while(cont) {
					if(tour) {

						//Trouver un coup avec Prolog + gerer si timeout recu

						//Envoie coup
						switch(TCoup.getCodeRep()) {
						case DEPLACER :
							dos.writeInt(1);
							break;
						case DEPOSER :
							dos.writeInt(2);
							break;
						case AUCUN :
							dos.writeInt(3);
							break;
						default :
							break;
						}

						if(TCoup.getCodeRep() != CodeRep.AUCUN) {
							switch(dis.readInt()) {
							case 0 :
								TCoup.setPieceSens(Sens.NORD);
								break;
							case 1 :
								TCoup.setPieceSens(Sens.SUD);
								break;
							default :
								break;
							}

							switch(dis.readInt()) {
							case 0 :
								TCoup.setPieceType(Type.KODAMA);
								break;
							case 1 :
								TCoup.setPieceType(Type.KODAMA_SAMOURAI);
								break;
							case 2 :
								TCoup.setPieceType(Type.KIRIN);
								break;
							case 3 :
								TCoup.setPieceType(Type.KOROPOKKURU);
								break;
							case 4 :
								TCoup.setPieceType(Type.ONI);
								break;
							case 5 :
								TCoup.setPieceType(Type.SUPER_ONI);
								break;
							default :
								break;
							}

							switch(dis.readInt()) {
							case 0 :
								TCoup.setTlgDep(Ligne.UN);
								break;
							case 1 :
								TCoup.setTlgDep(Ligne.DEUX);
								break;
							case 2 :
								TCoup.setTlgDep(Ligne.TROIS);
								break;
							case 3 :
								TCoup.setTlgDep(Ligne.QUATRE);
								break;
							case 4 :
								TCoup.setTlgDep(Ligne.CINQ);
								break;
							case 5 :
								TCoup.setTlgDep(Ligne.SIX);
								break;
							default :
								break;
							}

							switch(dis.readInt()) {
							case 0 :
								TCoup.setTcolDep(Colonne.A);
								break;
							case 1 :
								TCoup.setTcolDep(Colonne.B);
								break;
							case 2 :
								TCoup.setTcolDep(Colonne.C);
								break;
							case 3 :
								TCoup.setTcolDep(Colonne.D);
								break;
							case 4 :
								TCoup.setTcolDep(Colonne.E);
								break;
							default :
								break;
							}

							if(TCoup.getCodeRep() == CodeRep.DEPLACER) {
								switch(dis.readInt()) {
								case 0 :
									TCoup.setTlgArr(Ligne.UN);
									break;
								case 1 :
									TCoup.setTlgArr(Ligne.DEUX);
									break;
								case 2 :
									TCoup.setTlgArr(Ligne.TROIS);
									break;
								case 3 :
									TCoup.setTlgArr(Ligne.QUATRE);
									break;
								case 4 :
									TCoup.setTlgArr(Ligne.CINQ);
									break;
								case 5 :
									TCoup.setTlgArr(Ligne.SIX);
									break;
								default :
									break;
								}

								switch(dis.readInt()) {
								case 0 :
									TCoup.setTcolArr(Colonne.A);
									break;
								case 1 :
									TCoup.setTcolArr(Colonne.B);
									break;
								case 2 :
									TCoup.setTcolArr(Colonne.C);
									break;
								case 3 :
									TCoup.setTcolArr(Colonne.D);
									break;
								case 4 :
									TCoup.setTcolArr(Colonne.E);
									break;
								default :
									break;
								}

								switch(dis.readInt()) {
								case 0 :
									TCoup.setEstCapt(false);
									break;
								case 1 :
									TCoup.setEstCapt(true);
									break;
								default :
									break;
								}
								//Deplacer sur grille
							}else {
								//Deposer sur grille
							}
						}					
						tour = false;
					}else {					
						int cr = dis.readInt();
						//Réinitialisation
						if(cr == 0) {
							TPartie.setCodeReq(CodeReq.INIT);														
							System.out.println("Fin de la partie !");
							//Retour au debut !!!
						}else {
							//Reception coup adverse
							switch(cr) {
							case 1 :
								TCoup.setCodeRep(CodeRep.DEPLACER);
								break;
							case 2 :
								TCoup.setCodeRep(CodeRep.DEPOSER);
								break;
							case 3 :
								TCoup.setCodeRep(CodeRep.AUCUN);
								break;
							default :
								break;
							}
							if(TCoup.getCodeRep() != CodeRep.AUCUN) {
								switch(dis.readInt()) {
								case 0 :
									TCoup.setPieceType(Type.KODAMA);
									break;
								case 1 :
									TCoup.setPieceType(Type.KODAMA_SAMOURAI);
									break;
								case 2 :
									TCoup.setPieceType(Type.KIRIN);
									break;
								case 3 :
									TCoup.setPieceType(Type.KOROPOKKURU);
									break;
								case 4 :
									TCoup.setPieceType(Type.ONI);
									break;
								case 5 :
									TCoup.setPieceType(Type.SUPER_ONI);
									break;
								default :
									break;
								}

								switch(dis.readInt()) {
								case 0 :
									TCoup.setPieceSens(Sens.NORD);
									break;
								case 1 :
									TCoup.setPieceSens(Sens.SUD);
									break;
								default :
									break;
								}

								switch(dis.readInt()) {
								case 0 :
									TCoup.setTcolDep(Colonne.A);
									break;
								case 1 :
									TCoup.setTcolDep(Colonne.B);
									break;
								case 2 :
									TCoup.setTcolDep(Colonne.C);
									break;
								case 3 :
									TCoup.setTcolDep(Colonne.D);
									break;
								case 4 :
									TCoup.setTcolDep(Colonne.E);
									break;
								default :
									break;
								}

								switch(dis.readInt()) {
								case 0 :
									TCoup.setTlgDep(Ligne.UN);
									break;
								case 1 :
									TCoup.setTlgDep(Ligne.DEUX);
									break;
								case 2 :
									TCoup.setTlgDep(Ligne.TROIS);
									break;
								case 3 :
									TCoup.setTlgDep(Ligne.QUATRE);
									break;
								case 4 :
									TCoup.setTlgDep(Ligne.CINQ);
									break;
								case 5 :
									TCoup.setTlgDep(Ligne.SIX);
									break;
								default :
									break;
								}

								if(TCoup.getCodeRep() == CodeRep.DEPLACER) {
									switch(dis.readInt()) {
									case 0 :
										TCoup.setTcolArr(Colonne.A);
										break;
									case 1 :
										TCoup.setTcolArr(Colonne.B);
										break;
									case 2 :
										TCoup.setTcolArr(Colonne.C);
										break;
									case 3 :
										TCoup.setTcolArr(Colonne.D);
										break;
									case 4 :
										TCoup.setTcolArr(Colonne.E);
										break;
									default :
										break;
									}

									switch(dis.readInt()) {
									case 0 :
										TCoup.setTlgArr(Ligne.UN);
										break;
									case 1 :
										TCoup.setTlgArr(Ligne.DEUX);
										break;
									case 2 :
										TCoup.setTlgArr(Ligne.TROIS);
										break;
									case 3 :
										TCoup.setTlgArr(Ligne.QUATRE);
										break;
									case 4 :
										TCoup.setTlgArr(Ligne.CINQ);
										break;
									case 5 :
										TCoup.setTlgArr(Ligne.SIX);
										break;
									default :
										break;
									}

									switch(dis.readInt()) {
									case 0 :
										TCoup.setEstCapt(false);
										break;
									case 1 :
										TCoup.setEstCapt(true);
										break;
									default :
										break;
									}
									//Deplacer sur grille
								}else if(TCoup.getCodeRep() == CodeRep.DEPOSER) {
									//Deposer sur grille
								}
							}
						}
						tour = true;
					}
				}
				numPartie++;
			}
			dos.close();
			os.close();
			dis.close();
			is.close();
			sockComm.close();
			srv.close();
			System.out.println("Arrêt du serveur");
		}catch(IOException e){
			e.printStackTrace();}
	}

	public static void jasper() {
		/*
		String saisie = new String("");

		SICStus sp = null;

		try {

			// Creation d'un object SICStus
			sp = new SICStus();

			// Chargement d'un fichier prolog .pl
			sp.load("./src/projet_ia/yokai.pl");

		}
		// exception déclanchée par SICStus lors de la création de l'objet sp
		catch (SPException e) {
			System.err.println("Exception SICStus Prolog : " + e);
			e.printStackTrace();
			System.exit(-2);
		}

		// lecture au clavier d'une requète Prolog
		System.out.print("| ?- ");
		saisie = saisieClavier();

		// boucle pour saisir les informations
		while (! saisie.equals("halt.")) {

			// HashMap utilisé pour stocker les solutions
			HashMap results = new HashMap();

			try {

				// Creation d'une requete (Query) Sicstus
				//   - en fonction de la saisie de l'utilisateur
				//   - instanciera results avec les résultats de la requète
				Query qu = sp.openQuery(saisie,results);

				// parcours des solutions
				boolean moreSols = qu.nextSolution();

				// on ne boucle que si la liste des instanciations de variables est non vide
				boolean continuer = !(results.isEmpty());

				while (moreSols && continuer) {

					// chaque solution est sockée dans un HashMap
					// sous la forme : VariableProlog -> Valeur
					System.out.print(results + " ? ");

					// demande à l'utilisateur de continuer ...
					saisie = saisieClavier();
					if (saisie.equals(";")) {
						// solution suivante --> results contient la nouvelle solution
						moreSols = qu.nextSolution();
					}
					else {
						continuer = false;
					}
				}

				if (moreSols) {
					// soit :
					//  - il y a encore des solutions et (continuer == false)
					//  - le prédicat a réussi mais (results.isEmpty() == true)
					System.out.println("yes");
				}
				else {
					// soit :
					//    - on est à la fin des solutions
					//    - il n'y a pas de solutions (le while n'a pas été exécuté)
					System.out.println("no");
				}

				// fermeture de la requète
				System.err.println("Fermeture requete");
				qu.close();

			}
			catch (SPException e) {
				System.err.println("Exception prolog\n" + e);
			}
			// autres exceptions levées par l'utilisation du Query.nextSolution()
			catch (Exception e) {
				System.err.println("Other exception : " + e);
			}

			System.out.print("| ?- ");
			// lecture au clavier
			saisie = saisieClavier();        
		}
		System.out.println("End of jSicstus");
		System.out.println("Bye bye");*/
	}

	public static String saisieClavier() {

		// declaration du buffer clavier
		BufferedReader buff = new BufferedReader(new InputStreamReader(System.in));

		try {
			return buff.readLine();
		}
		catch (IOException e) {
			System.err.println("IOException " + e);
			e.printStackTrace();
			System.exit(-1);
		}
		return ("halt.");
	}	
}