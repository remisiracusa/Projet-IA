package projet_ia;

import java.util.ArrayList;
import java.util.List;

import projet_ia.TCoup.Ligne;
import projet_ia.TCoup.Type;
import projet_ia.TPartie.Sens;
import se.sics.jasper.SICStus;
import se.sics.jasper.SPException;

public class Grille {
	//Gestion de la grille du jeu
	private List<Integer> grille;
	
	public Grille() {
		super();
		this.grille = new ArrayList<Integer>();
		initialisationGrille();
	}
	
	public List<Integer> getGrille() {
		return grille;
	}
	
	//Mise en forme de la grille pour l'envoie a prolog
	public String getGrilleProlog() {
		String s = "[";
		for(int i = 0; i < grille.size(); i++) {
			s += grille.get(i);
			if(i < grille.size()-1) {
				s += ",";
			}
		}
		s += "]";
		return s;
	}
	
	//Mise a jour de la grille lors d'un deplacement
	public void miseAJourDeplacer(TCoup coup) {
		CoordGrille cgDep = coup.convertCoordGrilleToInt(coup.getTcolDep(), coup.getTlgDep());
		CoordGrille cgArr = coup.convertCoordGrilleToInt(coup.getTcolArr(), coup.getTlgArr());
		int piece = 0;
		switch(coup.getPieceType()){
		case KODAMA :
			piece = 1;
			break;
		case KODAMA_SAMOURAI :
			piece = 2;
			break;
		case KIRIN :
			piece = 3;
			break;
		case KOROPOKKURU :
			piece = 4;
			break;
		case ONI :
			piece = 5;
			break;
		case SUPER_ONI :
			piece = 6;
			break;
		default:
			break;
		}
		if(coup.getPieceSens() == Sens.SUD) {
			if((piece == 1 || piece == 5) && (cgArr.row.getId() >= Ligne.CINQ.getId())) {
				piece += 1;
			}
			piece += 6;
		}

		grille.set(cgDep.numCase-1, 0);
		grille.set(cgArr.numCase-1, piece);
		
	}
	
	//Mise a jour de la grille lors d'un deposement de piece
	public void miseAJourDeposer(TCoup coup) {
		CoordGrille cgDep = coup.convertCoordGrilleToInt(coup.getTcolDep(), coup.getTlgDep());
		int piece = 0;
		switch(coup.getPieceType()){
		case KODAMA :
			piece = 1;
			break;
		case KODAMA_SAMOURAI :
			piece = 2;
			break;
		case KIRIN :
			piece = 3;
			break;
		case KOROPOKKURU :
			piece = 4;
			break;
		case ONI :
			piece = 5;
			break;
		case SUPER_ONI :
			piece = 6;
			break;
		default:
			break;
		}
		if(coup.getPieceSens() == Sens.SUD) {
			piece += 6;
		}
		grille.set(cgDep.numCase-1, piece);		
	}
	
	//Mise a zero de la grille pour commencer le jeu
	public void initialisationGrille() {
		this.grille.clear();
		grille.add(11);
		grille.add(9);
		grille.add(10);
		grille.add(9);
		grille.add(11);
		grille.add(0);
		grille.add(0);
		grille.add(0);
		grille.add(0);
		grille.add(0);
		grille.add(0);
		grille.add(7);
		grille.add(7);
		grille.add(7);
		grille.add(0);
		grille.add(0);
		grille.add(1);
		grille.add(1);
		grille.add(1);
		grille.add(0);
		grille.add(0);
		grille.add(0);
		grille.add(0);
		grille.add(0);
		grille.add(0);
		grille.add(5);
		grille.add(3);
		grille.add(4);
		grille.add(3);
		grille.add(5);
	}
}
