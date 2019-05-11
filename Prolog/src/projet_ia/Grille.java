package projet_ia;

import java.util.ArrayList;
import java.util.List;

import projet_ia.TCoup.Type;
import projet_ia.TPartie.Sens;
import se.sics.jasper.SICStus;
import se.sics.jasper.SPException;

public class Grille {
	private List<Integer> grille;
	//[11,9,10,9,11,0,0,0,0,0,0,7,7,7,0,0,1,1,1,0,0,0,0,0,0,5,3,4,3,5]
	public Grille() {
		super();
		this.grille = new ArrayList<Integer>();
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
	
	public List<Integer> getGrille() {
		return grille;
	}
	
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
			piece += 6;
		}
		grille.set(cgDep.numCase-1, 0);
		grille.set(cgArr.numCase-1, piece);
		
	}
	
	public SICStus connexionSicstus() {
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
