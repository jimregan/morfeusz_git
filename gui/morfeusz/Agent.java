package morfeusz;

import java.io.*;
import java.util.*;

import pl.sgjp.morfeusz.*;

public class Agent 
{

	private Morfeusz controller;
	private String textToAnalyze;
	private String lemma;
	private String dictionary;
	private HashSet<Integer> interpBoundaries;
	private List<MorphInterpretation> analyzeResults;
	private List<MorphInterpretation> generateResults;
	private pl.sgjp.morfeusz.Morfeusz morfeusz;
	
	public static Agent createAgent(Morfeusz controller)
	{
		Agent agent = new Agent(controller);
		
		if (agent.morfeusz == null) return null;
		return agent;
	}
	
	public Agent(Morfeusz controller)
	{
		this.controller = controller;
		textToAnalyze = null;
		lemma = null;
		dictionary = null;
		interpBoundaries = null;
		analyzeResults = null;
		generateResults = null;
		initializeWithDictionaryName();
	}
	
	public void initialize()
	{
		Preferences prefs = controller.getPreferences();
		
		morfeusz.setAggl(prefs.aggl);
		morfeusz.setPraet(prefs.praet);
		morfeusz.setCaseHandling(prefs.caseHandling);
		morfeusz.setWhitespaceHandling(prefs.whitespaceHandling);
		morfeusz.setTokenNumbering(prefs.tokenNumbering);
	}
	
	public void initializeWithDictionaryName()
	{
		Preferences prefs = controller.getPreferences();
		
		morfeusz = null;
		if (prefs.dictionary != null) {
			File file = new File(prefs.dictionary);
			
			if (file.exists()) {
				try {
					morfeusz = pl.sgjp.morfeusz.Morfeusz.createInstance(prefs.dictionary, MorfeuszUsage.BOTH_ANALYSE_AND_GENERATE);
				}
				catch (Exception exception) {
					morfeusz = null;
					prefs.dictionary = null;
				}
			}
			else prefs.dictionary = null;
		}
		if (morfeusz == null) {
			try {
				morfeusz = pl.sgjp.morfeusz.Morfeusz.createInstance(MorfeuszUsage.BOTH_ANALYSE_AND_GENERATE);
			}
			catch (Exception exception) {
				morfeusz = null;
			}
		}
		if (morfeusz != null) initialize();
		else prefs.dictionary = null;
	}

	public void setTextToAnalyze(String text)
	{
		textToAnalyze = text;
	}
	
	public void setLemma(String text)
	{
		lemma = text;
	}
	
	public void clearAnalyzeResults()
	{
		analyzeResults.clear();
		analyzeResults = null;
		interpBoundaries = null;
		textToAnalyze = null;
	}
	
	public void clearGenerateResults()
	{
		generateResults.clear();
		generateResults = null;
		lemma = null;
	}
	
	public HashSet<Integer> getInterpBoundaries()
	{
		return interpBoundaries;
	}
	
	public List<MorphInterpretation> getAnalyzeResults()
	{
		return analyzeResults;
	}
	
	public List<MorphInterpretation> getGenerateResults()
	{
		return generateResults;
	}
	
	public boolean hasAnalyzeResults()
	{
		if (analyzeResults == null) return false;
		return !analyzeResults.isEmpty();
	}
	
	public boolean hasGenerateResults()
	{
		if (generateResults == null) return false;
		return !generateResults.isEmpty();
	}
	
	public boolean analyze()
	{
		analyzeResults = null;
		interpBoundaries = null;
		try {
			analyze(textToAnalyze);
		}
		catch (Exception exception) {
			analyzeResults = null;
			interpBoundaries = null;
			return false;
		}
		return true;
	}
	
	public void analyze(String text)
	{
		StringTokenizer tokenizer = new StringTokenizer(text, "\n");
		
		while (tokenizer.hasMoreTokens()) {
			String line = tokenizer.nextToken();
			List<MorphInterpretation> res = morfeusz.analyseAsList(line);
			
			if (analyzeResults == null) {
				analyzeResults = res;
				interpBoundaries = new HashSet<Integer>();
			}
			else analyzeResults.addAll(res);
			interpBoundaries.add(analyzeResults.size());
		}
	}
	
	public boolean generate()
	{
		generateResults = null;
		try {
			generateResults = morfeusz.generate(lemma);
		}
		catch (Exception exception) {
			generateResults = null;
			return false;
		}
		return true;
	}
	
	public boolean setDictionary(String name)
	{
		pl.sgjp.morfeusz.Morfeusz oldMorfeusz = morfeusz;
		
		try {
			if (name == null) {
				if (dictionary != null) {
					morfeusz = pl.sgjp.morfeusz.Morfeusz.createInstance(MorfeuszUsage.BOTH_ANALYSE_AND_GENERATE);
					initialize();
				}
			}
			else {
				morfeusz = pl.sgjp.morfeusz.Morfeusz.createInstance(name, MorfeuszUsage.BOTH_ANALYSE_AND_GENERATE);
				initialize();
			}
			dictionary = name;
		}
		catch (Exception exception) {
			morfeusz = oldMorfeusz;
			return false;
		}
		return true;
	}
	
	public void saveAnalyzeResults(PrintWriter pw)
	{
		if (hasAnalyzeResults()) {
			for (int i = 0; i < analyzeResults.size(); i++) {
				MorphInterpretation interp = analyzeResults.get(i);
				
				pw.print("[");
				pw.print(Integer.toString(interp.getStartNode()));
				pw.print(", ");
				pw.print(Integer.toString(interp.getEndNode()));
				pw.print(", ");
				pw.print(interp.getOrth());
				pw.print(", ");
				pw.print(interp.getLemma());
				pw.print(", ");
				pw.print(interp.getTag(morfeusz));
				pw.print(", ");
				pw.print(interp.getName(morfeusz));
				pw.print(", ");
				
				Collection<String> labels = interp.getLabels(controller.getMorfeusz());
				String str = null;
				boolean append = false;
				
				if (labels.size() > 0) {
					str = "";
					for (String s : labels) {
						if (append) str += "; ";
						str += s;
						append = true;
					}
				}

				pw.print(str);
				pw.println("]");
			}
		}
	}
	
	public void saveGenerateResults(PrintWriter pw)
	{
		if (hasAnalyzeResults()) {
			for (int i = 0; i < generateResults.size(); i++) {
				MorphInterpretation interp = generateResults.get(i);
				
				pw.print("[");
				pw.print(interp.getOrth());
				pw.print(", ");
				pw.print(interp.getLemma());
				pw.print(", ");
				pw.print(interp.getTag(morfeusz));
				pw.print(", ");
				pw.print(interp.getName(morfeusz));
				pw.print(", ");
				pw.print(interp.getName(morfeusz));
				pw.print(", ");
				
				Collection<String> labels = interp.getLabels(controller.getMorfeusz());
				String str = null;
				boolean append = false;
				
				if (labels.size() > 0) {
					str = "";
					for (String s : labels) {
						if (append) str += "; ";
						str += s;
						append = true;
					}
				}

				pw.print(str);
				pw.println("]");
			}
		}
	}
	
	public pl.sgjp.morfeusz.Morfeusz getMorfeusz()
	{
		return morfeusz;
	}
	
}
