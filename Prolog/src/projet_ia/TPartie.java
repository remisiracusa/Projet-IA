package projet_ia;

import java.util.HashMap;

public class TPartie {
	private CodeReq codeReq;
	private Sens sens;
	private HashMap<String,Integer> capture;
	
	public TPartie() {
		//Reserve des pieces capturees
		capture = new HashMap<String, Integer>();
		capture.put("KODAMA", 0);
		capture.put("KIRIN", 0);
		capture.put("KOROPOKKURU", 0);
		capture.put("ONI", 0);
	}
	
	//Sens des pieces
	public enum Sens {
		  NORD,
		  SUD;	
	}
	
	//Code requete
	public enum CodeReq {
		  INIT;	
	}
	
	public CodeReq getCodeReq() {
		return codeReq;
	}
	public void setCodeReq(CodeReq codeReq) {
		this.codeReq = codeReq;
	}
	public Sens getSens() {
		return sens;
	}
	public void setSens(Sens sens) {
		this.sens = sens;
	}
	
	//Capture d'une piece
	public void capture(String piece) {
		capture.replace(piece, (capture.get(piece)+1));
	}
	
	//Deposer une piece
	public void relacher(String piece) {
		capture.replace(piece, (capture.get(piece)-1));
	}
}
