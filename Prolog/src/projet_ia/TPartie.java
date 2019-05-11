package projet_ia;

import java.util.HashMap;

public class TPartie {
	private CodeReq codeReq;
	private Sens sens;
	private HashMap<String,Integer> capture;
	
	public TPartie() {
		capture = new HashMap<String, Integer>();
		capture.put("KODAMA", 0);
		capture.put("KIRIN", 0);
		capture.put("KOROPOKKURU", 0);
		capture.put("ONI", 0);
	}
	
	public enum Sens {
		  NORD,
		  SUD;	
	}
	
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
	
	public void capture(String piece) {
		System.out.println(piece + " capture !");
		capture.replace(piece, (capture.get(piece)+1));
	}
	
	public void relacher(String piece) {
		System.out.println(piece + " relache !");
		capture.replace(piece, (capture.get(piece)-1));
	}
}
