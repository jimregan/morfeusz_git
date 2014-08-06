package pl.waw.ipipan.morfeusz.app;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import pl.waw.ipipan.morfeusz.MorphInterpretation;
import pl.waw.ipipan.morfeusz.Morfeusz;

/**
 *
 * @author mlenart
 */
public class MorfeuszUtils {

    public static String getInterpretationString(MorphInterpretation interp, Morfeusz morfeusz) {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        PrintStream ps = new PrintStream(baos);
        ps.printf("%d %d %s %s %s %s", 
                interp.getStartNode(),
                interp.getEndNode(),
                interp.getOrth(),
                interp.getLemma(),
                morfeusz.getIdResolver().getTag(interp.getTagId()),
                morfeusz.getIdResolver().getName(interp.getNameId()),
                morfeusz.getIdResolver().getLabelsAsString(interp.getLabelsId()));
        ps.flush();
        return baos.toString();
    }
}
