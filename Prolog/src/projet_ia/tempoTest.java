package projet_ia;

public class tempoTest {

	public static void main(String[] args) {
		// 1 => A 1
		// 5 => E 1
		// 6 => A 2

		TCoup tcoup = new TCoup();
		tcoup.convertCoordGrille(1);
		tcoup.convertCoordGrille(5);
		tcoup.convertCoordGrille(6);

		for (int i = 1; i < 30; i++) {
			System.out.print("Num : " + i + " ");
			System.out.println(tcoup.convertCoordGrille(i));
		}
	}
}
