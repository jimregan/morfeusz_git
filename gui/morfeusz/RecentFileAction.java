package morfeusz;

import java.awt.event.ActionEvent;
import javax.swing.AbstractAction;
import javax.swing.Action;

@SuppressWarnings("serial")
public class RecentFileAction extends AbstractAction 
{

	private Morfeusz controller;
	private String name;
	
	public RecentFileAction(String name, Morfeusz controller)
	{
		super();
		this.name = name;
		this.controller = controller;
		
		putValue(Action.NAME, name);
	}

	public void actionPerformed(ActionEvent e) 
	{
		controller.changeDictionary(name);
	}
	
	public String getName()
	{
		return name;
	}
	
}
