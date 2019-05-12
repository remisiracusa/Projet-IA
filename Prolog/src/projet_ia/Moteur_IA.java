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
	static Sens sens;		//Sens des pieces attribuees
	static int numPartie;	//Compteur partie
	static boolean tour;	//Gestion du tour du joueur
	static boolean cont;	//Gestion d'arret de la partie

	public static void main(String[] args) {
		
		ServerSocket srv;					//Socket de connexion
		Socket sockComm; 					//Socket de communication
		TPartie TPartie = new TPartie();	
		Grille grille = new Grille();		//Création de la grille
		numPartie = 1;						
		cont = true;						
		try{
			//Lancement du serveur IA
			srv = new ServerSocket(Integer.parseInt(args[0]));
			sockComm = srv.accept();
			InputStream is = sockComm.getInputStream();
			DataInputStream dis = new DataInputStream(is);
			OutputStream os = sockComm.getOutputStream();
			DataOutputStream dos = new DataOutputStream(os);
			System.out.println("Début du serveur");
			
			//Connexion au prolog
			SICStus sp = connexionSicstus();
			
			//Initialisation
			if(dis.readInt() == 0) {
				TPartie.setCodeReq(CodeReq.INIT);
			}
			
			//Sens des pieces attribuees
			if(dis.readInt() == 0) {
				TPartie.setSens(Sens.NORD);
			}else {
				TPartie.setSens(Sens.SUD);
			}			

			//Gestion des parties
			while(numPartie < 3) {
				if((numPartie == 1 && TPartie.getSens() == Sens.SUD) || (numPartie == 2 && TPartie.getSens() == Sens.NORD)) {
					//Mon tour
					tour = true;
				}else {
					//Tour adverse
					tour = false;
				}
				
				//Mise à zero de la grille
				grille.initialisationGrille();
				
				//Boucle des tours de jeu
				while(cont) {
					if(tour) {
						
						//Je joue
						String sens = "";
						if(TPartie.getSens() == Sens.NORD) {
							sens = "bottom";
						}else {
							sens = "top";
						}
						
						//Solution pour choisir les coups aleatoirement
						//String requete = "yokaiRandom("+grille.getGrilleProlog()+","+sens+",Piece,Ind,NewInd,CarteCapturee)."; 
						
						//Solution pour choisir les coups selon l'algorithme minmax
						String requete = "launchMinMax("+grille.getGrilleProlog()+","+sens+",Piece,Ind,NewInd,CarteCapturee).";
						
						//Recuperation du coup a jouer
						TCoup coup = coupProlog(requete, sp, TPartie);
						
						//Envoie coup
						switch(coup.getCodeRep()) {
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

						if(coup.getCodeRep() != CodeRep.AUCUN) {
							
							switch(coup.getPieceType()) {
							case KODAMA :
								dos.writeInt(0);
								break;
							case KODAMA_SAMOURAI :
								dos.writeInt(1);
								break;
							case KIRIN :
								dos.writeInt(2);
								break;
							case KOROPOKKURU :
								dos.writeInt(3);
								break;
							case ONI :
								dos.writeInt(4);
								break;
							case SUPER_ONI :
								dos.writeInt(5);
								break;
							default :
								break;
							}
							
							dos.writeInt((coup.getTlgDep().getId()-1));
							dos.writeInt((coup.getTcolDep().getId()-1));

							if(coup.getCodeRep() == CodeRep.DEPLACER) {
								dos.writeInt((coup.getTlgArr().getId()-1));
								dos.writeInt((coup.getTcolArr().getId()-1));

								if(coup.isEstCapt()) {
									dos.writeInt(1);
								}else {
									dos.writeInt(0);
								}
								
								//Deplacement sur grille
								grille.miseAJourDeplacer(coup);
							}else {
								//Deposer piece sur grille
								grille.miseAJourDeposer(coup);
							}							
						}
						tour = false;
					}else {	
						
						//Tour adverse
						TCoup coup = new TCoup();
						int cr = dis.readInt();
						
						//Réinitialisation
						if(cr == 0) {
							
							//Fin de la partie
							TPartie.setCodeReq(CodeReq.INIT);
							break;
						}else {
							
							//Reception coup adverse
							switch(cr) {
							case 1 :
								coup.setCodeRep(CodeRep.DEPLACER);
								break;
							case 2 :
								coup.setCodeRep(CodeRep.DEPOSER);
								break;
							case 3 :
								coup.setCodeRep(CodeRep.AUCUN);
								break;
							default :
								break;
							}

							if(coup.getCodeRep() != CodeRep.AUCUN) {
								switch(dis.readInt()) {
								case 0 :
									coup.setPieceType(Type.KODAMA);
									break;
								case 1 :
									coup.setPieceType(Type.KODAMA_SAMOURAI);
									break;
								case 2 :
									coup.setPieceType(Type.KIRIN);
									break;
								case 3 :
									coup.setPieceType(Type.KOROPOKKURU);
									break;
								case 4 :
									coup.setPieceType(Type.ONI);
									break;
								case 5 :
									coup.setPieceType(Type.SUPER_ONI);
									break;
								default :
									break;
								}

								switch(dis.readInt()) {
								case 0 :
									coup.setPieceSens(Sens.NORD);
									break;
								case 1 :
									coup.setPieceSens(Sens.SUD);
									break;
								default :
									break;
								}

								coup.setTcolDep(Colonne.getEnum(dis.readInt()+1));
								coup.setTlgDep(Ligne.getEnum(dis.readInt()+1));
															
								if(coup.getCodeRep() == CodeRep.DEPLACER) {
									coup.setTcolArr(Colonne.getEnum(dis.readInt()+1));
									coup.setTlgArr(Ligne.getEnum(dis.readInt()+1));

									switch(dis.readInt()) {
									case 0 :
										coup.setEstCapt(false);
										break;
									case 1 :
										coup.setEstCapt(true);
										break;
									default :
										break;
									}
									
									//Deplacer sur grille
									grille.miseAJourDeplacer(coup);
								}else if(coup.getCodeRep() == CodeRep.DEPOSER) {
									//Deposer sur grille
									grille.miseAJourDeposer(coup);
								}
							}
						}
						tour = true;
					}
				}
				
				//Partie suivante
				numPartie++;
			}
			
			//Arret de la communication et du serveur
			dos.close();
			os.close();
			dis.close();
			is.close();
			sockComm.close();
			srv.close();
			System.out.println("Arrêt du serveur");
		}catch(IOException e){
			e.printStackTrace();
		}

	}
	
	//Renvoie le coup suivant a jouer
	public static TCoup coupProlog(String requete, SICStus sp, TPartie TPartie) {
		TCoup coup = new TCoup();
		String saisie = new String(requete);

		// HashMap utilisé pour stocker les solutions
		HashMap results = new HashMap();
		SPTerm piece = new SPTerm(sp);
		SPTerm ind = new SPTerm(sp);
		SPTerm newInd = new SPTerm(sp);
		SPTerm carteCapturee = new SPTerm(sp);

		try {
			// Creation d'une requete (Query) Sicstus
			//   - instanciera results avec les résultats de la requète
			Query qu = sp.openQuery(saisie,results);
			
			// parcours des solutions
			boolean moreSols = qu.nextSolution();

			// on ne boucle que si la liste des instanciations de variables est non vide
			boolean continuer = !(results.isEmpty());

			if (moreSols && continuer) {
				coup.setCodeRep(CodeRep.DEPLACER);
				coup.setPieceSens(TPartie.getSens());
				
				// chaque solution est sockée dans un HashMap
				// sous la forme : VariableProlog -> Valeur
				piece = (SPTerm) results.get("Piece");
				ind = (SPTerm) results.get("Ind");
				newInd = (SPTerm) results.get("NewInd");
				carteCapturee = (SPTerm) results.get("CarteCapturee");
				CoordGrille cg = coup.convertCoordGrille((int) ind.getInteger());
				CoordGrille cgNewInd = coup.convertCoordGrille((int) newInd.getInteger());
				
				coup.setTcolDep(cg.col);
				coup.setTlgDep(cg.row);
				coup.setTcolArr(cgNewInd.col);
				coup.setTlgArr(cgNewInd.row);
				
				switch((int) piece.getInteger()){
				case 1 :
					coup.setPieceType(Type.KODAMA);
					break;
				case 2 :
					coup.setPieceType(Type.KODAMA_SAMOURAI);
					break;
				case 3 :
					coup.setPieceType(Type.KIRIN);
					break;
				case 4 :
					coup.setPieceType(Type.KOROPOKKURU);
					break;
				case 5 :
					coup.setPieceType(Type.ONI);
					break;
				case 6 :
					coup.setPieceType(Type.SUPER_ONI);
					break;
				case 7 :
					coup.setPieceType(Type.KODAMA);
					break;
				case 8 :
					coup.setPieceType(Type.KODAMA_SAMOURAI);
					break;
				case 9 :
					coup.setPieceType(Type.KIRIN);
					break;
				case 10 :
					coup.setPieceType(Type.KOROPOKKURU);
					break;
				case 11 :
					coup.setPieceType(Type.ONI);
					break;
				case 12 :
					coup.setPieceType(Type.SUPER_ONI);
					break;
				default:
					break;
				}

				if(((int) carteCapturee.getInteger()) == 0) {
					coup.setEstCapt(false);
				}else {
					String p = "";
					switch((int) carteCapturee.getInteger()) {
					case 1 :
						p = "KODAMA";
						break;
					case 2 :
						p = "KODAMA";
						break;
					case 3 :
						p = "KIRIN";
						break;
					case 4 :
						p = "KOROPOKKURU";
						break;
					case 5 :
						p = "ONI";
						break;
					case 6 :
						p = "ONI";
						break;
					case 7 :
						p = "KODAMA";
						break;
					case 8 :
						p = "KODAMA";
						break;
					case 9 :
						p = "KIRIN";
						break;
					case 10 :
						p = "KOROPOKKURU";
						break;
					case 11 :
						p = "ONI";
						break;
					case 12 :
						p = "ONI";
						break;
					default:
						break;
					}
					
					TPartie.capture(p);
					coup.setEstCapt(true);
				}
			}else {
				//Aucun coup disponible
				coup.setCodeRep(CodeRep.AUCUN);
			}

			// fermeture de la requète
			qu.close();

		}
		catch (SPException e) {
			System.err.println("Exception prolog\n" + e);
		}
		// autres exceptions levées par l'utilisation du Query.nextSolution()
		catch (Exception e) {
			System.err.println("Other exception : " + e);
		}
		return coup;
	}
	
	public static SICStus connexionSicstus() {
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

		return sp;
	}
}