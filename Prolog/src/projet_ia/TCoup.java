package projet_ia;

import projet_ia.TPartie.Sens;

import java.util.HashMap;
import java.util.Map;

public class TCoup {
	private CodeRep codeRep;    //deplacement 0, piece capturee 1, aucun 2
	private Sens pieceSens;    //sens de la piece
	private Type pieceType;    //type de la piece
	private Ligne TlgDep;        //si deplacement ligne depart
	private Colonne TcolDep;    //si deplacement colonne depart
	private Ligne TlgArr;        //si deplacement ligne arrivee
	private Colonne TcolArr;    //si deplacement colonne arrivee
	private boolean estCapt;    //si piece capturee 1 sinon 0

	enum CodeRep {
		DEPLACER,
		DEPOSER,
		AUCUN;
	}

	enum Type {
		KODAMA,
		KODAMA_SAMOURAI,
		KIRIN,
		KOROPOKKURU,
		ONI,
		SUPER_ONI
	}

	enum Colonne {
		A(1),
		B(2),
		C(3),
		D(4),
		E(5);

		private int id;
		private static Map map = new HashMap<>();

		Colonne(int id) {
			this.id = id;
		}

		static {
			for (Colonne typeCodeReq : Colonne.values()) {
				//noinspection unchecked
				map.put(typeCodeReq.id, typeCodeReq);
			}
		}

		public static Colonne getEnum(int colonne) {
			return (Colonne) map.get(colonne);
		}

		public int getId() {
			return id;
		}
	}

	enum Ligne {
		UN(1),
		DEUX(2),
		TROIS(3),
		QUATRE(4),
		CINQ(5),
		SIX(6);

		private int id;
		private static Map map = new HashMap<>();

		Ligne(int id) {
			this.id = id;
		}

		static {
			for (Ligne typeCodeReq : Ligne.values()) {
				//noinspection unchecked
				map.put(typeCodeReq.id, typeCodeReq);
			}
		}

		public static Ligne getEnum(int ligne) {
			return (Ligne) map.get(ligne);
		}

		public int getId() {
			return id;
		}

	}

	public coordGrille convertCoordGrille(int numCase) {
		//System.out.println("Num : " + numCase);
		numCase--;
		int ligne = ((numCase / 5) + 1);
		int colonne = ((numCase % 5) + 1);
		/*System.out.println("Ligne : " + Ligne.getEnum(ligne));
		System.out.println("Colonne : " + Colonne.getEnum(colonne));
		System.out.println();*/
		return new coordGrille(Ligne.getEnum(ligne), Colonne.getEnum(colonne));
	}

	public CodeRep getCodeRep() {
		return codeRep;
	}

	public void setCodeRep(CodeRep codeRep) {
		this.codeRep = codeRep;
	}

	public Sens getPieceSens() {
		return pieceSens;
	}

	public void setPieceSens(Sens pieceSens) {
		this.pieceSens = pieceSens;
	}

	public Type getPieceType() {
		return pieceType;
	}

	public void setPieceType(Type pieceType) {
		this.pieceType = pieceType;
	}

	public Ligne getTlgDep() {
		return TlgDep;
	}

	public void setTlgDep(Ligne tlgDep) {
		TlgDep = tlgDep;
	}

	public Colonne getTcolDep() {
		return TcolDep;
	}

	public void setTcolDep(Colonne tcolDep) {
		TcolDep = tcolDep;
	}

	public Ligne getTlgArr() {
		return TlgArr;
	}

	public void setTlgArr(Ligne tlgArr) {
		TlgArr = tlgArr;
	}

	public Colonne getTcolArr() {
		return TcolArr;
	}

	public void setTcolArr(Colonne tcolArr) {
		TcolArr = tcolArr;
	}

	public boolean isEstCapt() {
		return estCapt;
	}

	public void setEstCapt(boolean estCapt) {
		this.estCapt = estCapt;
	}
}

class coordGrille {
	public TCoup.Colonne col;
	public TCoup.Ligne row;

	public coordGrille(TCoup.Ligne row, TCoup.Colonne col) {
		this.row = row;
		this.col = col;
	}

	@Override
	public String toString() {
		return "{" + col +
				", " + row +
				'}';
	}
}