package projet_ia;

public class TCoup {
	private int codeRep;	//deplacement 0, piece capturee 1, aucun 2
	private int pieceSens;	//sens de la piece
	private int pieceType;	//type de la piece
	private int TlgDep;		//si deplacement ligne depart
	private int TcolDep;	//si deplacement colonne depart
	private int TlgArr;		//si deplacement ligne arrivee
	private int TcolArr;	//si deplacement colonne arrivee
	private int estCapt;	//si piece capturee 1 sinon 0
	
	
	public int getCodeRep() {
		return codeRep;
	}
	public void setCodeRep(int codeRep) {
		this.codeRep = codeRep;
	}
	public int getPieceSens() {
		return pieceSens;
	}
	public void setPieceSens(int pieceSens) {
		this.pieceSens = pieceSens;
	}
	public int getPieceType() {
		return pieceType;
	}
	public void setPieceType(int pieceType) {
		this.pieceType = pieceType;
	}
	public int getTlgDep() {
		return TlgDep;
	}
	public void setTlgDep(int tlgDep) {
		TlgDep = tlgDep;
	}
	public int getTcolDep() {
		return TcolDep;
	}
	public void setTcolDep(int tcolDep) {
		TcolDep = tcolDep;
	}
	public int getTlgArr() {
		return TlgArr;
	}
	public void setTlgArr(int tlgArr) {
		TlgArr = tlgArr;
	}
	public int getTcolArr() {
		return TcolArr;
	}
	public void setTcolArr(int tcolArr) {
		TcolArr = tcolArr;
	}
	public int getEstCapt() {
		return estCapt;
	}
	public void setEstCapt(int estCapt) {
		this.estCapt = estCapt;
	}	
}
