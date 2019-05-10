package projet_ia;

import se.sics.jasper.Query;
import se.sics.jasper.SICStus;

import java.util.HashMap;

/**
 * Created by Valentin.
 */
public class SimpleTest {

	public static void main(String argv[]) {

		SICStus sp;
		Query query;
		HashMap WayMap = new HashMap();

		try {
			sp = new SICStus(argv, null);

			sp.restore("simple.sav");

			query = sp.openPrologQuery("connected('Orebro', 'Stockholm', Way, Way).", WayMap);

			try {
				while (query.nextSolution()) {
					System.out.println(WayMap);
				}
			} finally {
				query.close();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}
