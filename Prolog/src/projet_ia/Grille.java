package projet_ia;

import java.util.ArrayList;
import java.util.List;

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
	/*
	public void miseAJourDeplacer(TCoup coup) {
		CoordGrille cgDep = coup.convertCoordGrilleToInt(coup.getTlgDep(), coup.getTcolDep());
		CoordGrille cgArr = coup.convertCoordGrilleToInt(coup.getTlgArr(), coup.getTcolArr());
		int caseDepart = cgDep.getNumCase();
		int caseArrivee = cgArr.getNumCase();
		
		grille.set(caseDepart, 0);
		grille.set(caseArrivee, coup.getPieceType());
		
	}*/
	
}
