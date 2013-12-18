package pl.waw.ipipan.morfeusz;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author mlenart
 */
public class TestMorfeusz {

    public TestMorfeusz() {
    }

    @BeforeClass
    public static void setUpClass() {
    }

    @AfterClass
    public static void tearDownClass() {
    }

    @Before
    public void setUp() {
    }

    @After
    public void tearDown() {
    }

    @Test
    public void testSimpleRecognize() {
        System.err.println("START TEST testSimpleRecognize");
        Morfeusz morfeusz = new Morfeusz();
        InterpsVector iv = new InterpsVector();
        morfeusz.analyze("ż", iv);
        assertEquals(1, iv.size());
        assertEquals(0, iv.get(0).getStartNode());
        assertEquals(1, iv.get(0).getEndNode());
        assertEquals("ż", iv.get(0).getOrth());
        assertEquals("+ż", iv.get(0).getLemma());
        assertEquals("qub", iv.get(0).getTag());
        assertEquals("pospolita", iv.get(0).getName());
        System.err.println("DONE testSimpleRecognize");
    }
    
    @Test
     public void testIgn() {
         System.err.println("START TEST testIgn");
         Morfeusz morfeusz = new Morfeusz();
         InterpsVector iv = new InterpsVector();
         morfeusz.analyze("ą", iv);
         assertEquals(1, iv.size());
         assertEquals(0, iv.get(0).getStartNode());
         assertEquals(1, iv.get(0).getEndNode());
         assertEquals("ą", iv.get(0).getOrth());
         assertEquals("ą", iv.get(0).getLemma());
         assertEquals("ign", iv.get(0).getTag());
         assertEquals("", iv.get(0).getName());
         System.err.println("DONE testIgn");
     }
}
