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
		Grille grille = new Grille();
		numPartie = 1;
		cont = true;
		try{
			srv = new ServerSocket(Integer.parseInt(args[0]));
			sockComm = srv.accept();
			InputStream is = sockComm.getInputStream();
			DataInputStream dis = new DataInputStream(is);
			OutputStream os = sockComm.getOutputStream();
			DataOutputStream dos = new DataOutputStream(os);
			System.out.println("Début du serveur");
			
			SICStus sp = grille.connexionSicstus();
			
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
					System.out.println("je commence");
					tour = true;
				}else {
					System.out.println("je commence pas");
					tour = false;
				}
				//Mise à zero de la grille
				grille.initialisationGrille();
				while(cont) {
					if(tour) {

						//Trouver un coup avec Prolog + gerer si timeout recu -> numPartie++; break;
						String sens = "";
						if(TPartie.getSens() == Sens.NORD) {
							sens = "bottom";
						}else {
							sens = "top";
						}
						String requete = "yokaiRandom("+grille.getGrilleProlog()+","+sens+",Piece,Ind,NewInd,CarteCapturee)."; 
						
						//String requete = "yokai2("+grille.getGrilleProlog()+","+sens+",Piece,Ind,NewInd,CarteCapturee)."; 
						//System.out.println(requete);
						TCoup coup = coupProlog(requete, sp);
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
							/*coup.setPieceSens(TPartie.getSens());
							switch(TPartie.getSens()) {
							case NORD :
								dos.writeInt(0);
								break;
							case SUD :
								dos.writeInt(1);
								break;
							default :
								break;
							}*/

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
								System.out.println("envoie 4");
								dos.writeInt(4);
								break;
							case SUPER_ONI :
								dos.writeInt(5);
								break;
							default :
								break;
							}

							switch(coup.getTlgDep()) {
							case UN :
								dos.writeInt(0);
								break;
							case DEUX :
								dos.writeInt(1);
								break;
							case TROIS :
								dos.writeInt(2);
								break;
							case QUATRE :
								dos.writeInt(3);
								break;
							case CINQ :
								dos.writeInt(4);
								break;
							case SIX :
								dos.writeInt(5);
								break;
							default :
								break;
							}

							switch(coup.getTcolDep()) {
							case A :
								dos.writeInt(0);
								break;
							case B :
								dos.writeInt(1);
								break;
							case C :
								dos.writeInt(2);
								break;
							case D :
								dos.writeInt(3);
								break;
							case E :
								dos.writeInt(4);
								break;
							default :
								break;
							}

							if(coup.getCodeRep() == CodeRep.DEPLACER) {
								switch(coup.getTlgArr()) {
								case UN :
									dos.writeInt(0);
									break;
								case DEUX :
									dos.writeInt(1);
									break;
								case TROIS :
									dos.writeInt(2);
									break;
								case QUATRE :
									dos.writeInt(3);
									break;
								case CINQ :
									dos.writeInt(4);
									break;
								case SIX :
									dos.writeInt(5);
									break;
								default :
									break;
								}

								switch(coup.getTcolArr()) {
								case A :
									dos.writeInt(0);
									break;
								case B :
									dos.writeInt(1);
									break;
								case C :
									dos.writeInt(2);
									break;
								case D :
									dos.writeInt(3);
									break;
								case E :
									dos.writeInt(4);
									break;
								default :
									break;
								}

								if(coup.isEstCapt()) {
									dos.writeInt(1);
									switch(coup.get)
									TPartie.capture(coup.get);
								}else {
									dos.writeInt(0);
								}
								//Deplacer sur grille
								grille.miseAJourDeplacer(coup);
							}else {
								//Deposer sur grille
							}
							System.out.println("Coup envoye : "+coup.getCodeRep()+" "+coup.getPieceSens()+" "+coup.getPieceType()+" "+coup.getTlgDep()+" "+coup.getTcolDep()+" "+" "+coup.getTlgArr()+" "+coup.getTcolArr()+" "+coup.isEstCapt());							
						}else {
							System.out.println("Coup envoye : "+coup.getCodeRep());
						}
						tour = false;
						
					}else {	
						TCoup coup = new TCoup();
						System.out.println("Reception coup adverse");
						int cr = dis.readInt();
						//Réinitialisation
						if(cr == 0) {
							TPartie.setCodeReq(CodeReq.INIT);														
							System.out.println("Fin de la partie !");
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
							System.out.println(coup.getCodeRep());
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
								System.out.println(coup.getPieceType());
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
								System.out.println(coup.getPieceSens());

								switch(dis.readInt()) {
								case 0 :
									coup.setTcolDep(Colonne.A);
									break;
								case 1 :
									coup.setTcolDep(Colonne.B);
									break;
								case 2 :
									coup.setTcolDep(Colonne.C);
									break;
								case 3 :
									coup.setTcolDep(Colonne.D);
									break;
								case 4 :
									coup.setTcolDep(Colonne.E);
									break;
								default :
									break;
								}
								System.out.println(coup.getTcolDep());

								switch(dis.readInt()) {
								case 0 :
									coup.setTlgDep(Ligne.UN);
									break;
								case 1 :
									coup.setTlgDep(Ligne.DEUX);
									break;
								case 2 :
									coup.setTlgDep(Ligne.TROIS);
									break;
								case 3 :
									coup.setTlgDep(Ligne.QUATRE);
									break;
								case 4 :
									coup.setTlgDep(Ligne.CINQ);
									break;
								case 5 :
									coup.setTlgDep(Ligne.SIX);
									break;
								default :
									break;
								}
								System.out.println(coup.getTlgDep());

								if(coup.getCodeRep() == CodeRep.DEPLACER) {
									switch(dis.readInt()) {
									case 0 :
										coup.setTcolArr(Colonne.A);
										break;
									case 1 :
										coup.setTcolArr(Colonne.B);
										break;
									case 2 :
										coup.setTcolArr(Colonne.C);
										break;
									case 3 :
										coup.setTcolArr(Colonne.D);
										break;
									case 4 :
										coup.setTcolArr(Colonne.E);
										break;
									default :
										break;
									}
									System.out.println(coup.getTlgArr());

									switch(dis.readInt()) {
									case 0 :
										coup.setTlgArr(Ligne.UN);
										break;
									case 1 :
										coup.setTlgArr(Ligne.DEUX);
										break;
									case 2 :
										coup.setTlgArr(Ligne.TROIS);
										break;
									case 3 :
										coup.setTlgArr(Ligne.QUATRE);
										break;
									case 4 :
										coup.setTlgArr(Ligne.CINQ);
										break;
									case 5 :
										coup.setTlgArr(Ligne.SIX);
										break;
									default :
										break;
									}
									System.out.println(coup.getTlgArr());

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

									System.out.println(coup.isEstCapt());
									System.out.println("Coup recu : "+coup.getCodeRep()+" "+coup.getPieceSens()+" "+coup.getPieceType()+" "+coup.getTlgDep()+" "+coup.getTcolDep()+" "+" "+coup.getTlgArr()+" "+coup.getTcolArr()+" "+coup.isEstCapt());
									
									//Deplacer sur grille
									grille.miseAJourDeplacer(coup);
								}else if(coup.getCodeRep() == CodeRep.DEPOSER) {
									//Deposer sur grille
									//grille.miseAJourDeposer(TCoup);
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
			e.printStackTrace();
		}

	}
	
	public static TCoup coupProlog(String requete, SICStus sp) {
		TCoup coup = new TCoup();
		String saisie = new String(requete);

		
		// lecture au clavier d'une requète Prolog
		//System.out.print("| ?- ");
		//saisie = saisieClavier();

		// boucle pour saisir les informations
		//while (! saisie.equals("halt.")) {

		// HashMap utilisé pour stocker les solutions
		HashMap results = new HashMap();
		SPTerm piece = new SPTerm(sp);
		SPTerm ind = new SPTerm(sp);
		SPTerm newInd = new SPTerm(sp);
		SPTerm carteCapturee = new SPTerm(sp);
		try {

			// Creation d'une requete (Query) Sicstus
			//   - en fonction de la saisie de l'utilisateur
			//   - instanciera results avec les résultats de la requète
			Query qu = sp.openQuery(saisie,results);
			
			// parcours des solutions
			boolean moreSols = qu.nextSolution();

			// on ne boucle que si la liste des instanciations de variables est non vide
			boolean continuer = !(results.isEmpty());

			if (moreSols && continuer) {
				//Voir pour deposer
				System.out.println("coup prolog deplacer");
				coup.setCodeRep(CodeRep.DEPLACER);
				// chaque solution est sockée dans un HashMap
				// sous la forme : VariableProlog -> Valeur
				//System.out.println(results + " ? ");
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

				coup.setEstCapt((int) carteCapturee.getInteger());
				
				// demande à l'utilisateur de continuer ...
				//saisie = saisieClavier();
				if (saisie.equals(";")) {
					// solution suivante --> results contient la nouvelle solution
					moreSols = qu.nextSolution();
				}else {
					continuer = false;
				}
			}else {
				coup.setCodeRep(CodeRep.AUCUN);
				System.out.println("coup prolog aucun");
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

		//System.out.print("| ?- ");
		// lecture au clavier
		//saisie = saisieClavier();        
		//}
		System.out.println("End of jSicstus");
		System.out.println("Bye bye");
		return coup;
	}
}