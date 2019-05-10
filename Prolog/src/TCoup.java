package projet_ia;

import projet_ia.TPartie.Sens;

public class TCoup {
	private CodeRep codeRep;	//deplacement 0, piece capturee 1, aucun 2
	private Sens pieceSens;	//sens de la piece
	private Type pieceType;	//type de la piece
	private Ligne TlgDep;		//si deplacement ligne depart
	private Colonne TcolDep;	//si deplacement colonne depart
	private Ligne TlgArr;		//si deplacement ligne arrivee
	private Colonne TcolArr;	//si deplacement colonne arrivee
	private boolean estCapt;	//si piece capturee 1 sinon 0
	
	enum CodeRep{
		DEPLACER,
		DEPOSER,
		AUCUN;
	}
	
	enum Type{
		KODAMA,
		KODAMA_SAMOURAI,
		KIRIN,
		KOROPOKKURU,
		ONI,
		SUPER_ONI
	}
	
	enum Colonne{
		A,
		B,
		C,
		D,
		E;
	}
	
	enum Ligne{
		UN,
		DEUX,
		TROIS,
		QUATRE,
		CINQ,
		SIX;
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
