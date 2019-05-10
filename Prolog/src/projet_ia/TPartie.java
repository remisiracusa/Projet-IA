package projet_ia;

public class TPartie {
	private CodeReq codeReq;
	private Sens sens;
	
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
}
