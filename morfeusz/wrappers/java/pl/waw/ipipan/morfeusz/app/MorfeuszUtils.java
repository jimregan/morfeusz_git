package pl.waw.ipipan.morfeusz.app;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import pl.waw.ipipan.morfeusz.MorphInterpretation;

/**
 *
 * @author mlenart
 */
public class MorfeuszUtils {

    public static String getInterpretationString(MorphInterpretation interp) {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        PrintStream ps = new PrintStream(baos);
        ps.printf("%d %d %s %s %s %s", 
                interp.getStartNode(),
                interp.getEndNode(),
                interp.getOrth(),
                interp.getLemma(),
                interp.getTag(),
                interp.getName());
        ps.flush();
        return baos.toString();
    }
}
