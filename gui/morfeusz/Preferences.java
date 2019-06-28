package morfeusz;

import java.io.Serializable;

import pl.sgjp.morfeusz.*;

@SuppressWarnings("serial")
public class Preferences implements Serializable
{

	public String aggl;
	public String praet;
	public CaseHandling caseHandling;
	public WhitespaceHandling whitespaceHandling;
	public TokenNumbering tokenNumbering;
	public int fontSize;
	public int sortedColumn;
	public String dictionary;
	public String[] dictPaths;
	
	public Preferences()
	{
	}
	
	public void setDefaults()
	{
		setMorfeuszDefaults();
		fontSize = 2;
		sortedColumn = 0;
		dictionary = null;
		dictPaths = null;
	}
	
	public void setMorfeuszDefaults()
	{
		aggl = "strict";
		praet = "split";
		caseHandling = CaseHandling.CONDITIONALLY_CASE_SENSITIVE;
		whitespaceHandling = WhitespaceHandling.SKIP_WHITESPACES;
		tokenNumbering = TokenNumbering.SEPARATE_NUMBERING;
	}
	
}
