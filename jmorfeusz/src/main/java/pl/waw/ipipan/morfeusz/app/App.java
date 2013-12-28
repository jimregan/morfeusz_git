package pl.waw.ipipan.morfeusz.app;

import pl.waw.ipipan.morfeusz.Morfeusz;
import pl.waw.ipipan.morfeusz.MorphInterpretation;
import pl.waw.ipipan.morfeusz.ResultsIterator;

/**
 *
 * @author mlenart
 */
public class App {

    static {
        System.loadLibrary("morfeusz");
        System.err.println("LOADED MORFEUSZ");
        System.loadLibrary("jmorfeusz");
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        Morfeusz m = new Morfeusz();
        ResultsIterator resIt = m.analyze("Ala ma kota żółć.");
        while (resIt.hasNext()) {
            MorphInterpretation mi = resIt.getNext();
            System.out.printf(
                    "%d %d %s %s %s %s%n",
                    mi.getStartNode(), mi.getEndNode(),
                    mi.getOrth(), mi.getLemma(),
                    mi.getTag(), mi.getName());
        }
    }

}
