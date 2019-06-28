package morfeusz;

import java.awt.Insets;
import java.net.URL;

import javax.swing.Action;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JButton;

@SuppressWarnings("serial")
public class ToolBarButton extends JButton 
{
	
	private String iconName;

	public ToolBarButton(Action action, String iconName) 
	{
		super(action);
		setText(null);
		this.iconName = iconName;
 		initButton();
	}

	private void initButton() 
	{
		URL url;
		Icon icon;
		
		setRolloverEnabled(true);
		setRequestFocusEnabled(false);
		setMargin(new Insets(0, 0, 0, 0));
		putClientProperty("JToolBar.isRollover", Boolean.TRUE);
		setBorderPainted(false);
		setContentAreaFilled(false);
		url = ClassLoader.getSystemClassLoader().getResource(iconName + ".png");
		if (url != null) {
			icon = new ImageIcon(url);
			setIcon(icon);
		}
		url = ClassLoader.getSystemClassLoader().getResource(iconName + "_disabled.png");
		if (url != null) {
			icon = new ImageIcon(url);
			setDisabledIcon(icon);
		}
		url = ClassLoader.getSystemClassLoader().getResource(iconName + "_roll_over.png");
		if (url != null) {
			icon = new ImageIcon(url);
			setRolloverIcon(icon);
		}
		url = ClassLoader.getSystemClassLoader().getResource(iconName + "_pressed.png");
		if (url != null) {
			icon = new ImageIcon(url);
			setPressedIcon(icon);
		}
	}

	public boolean isFocusTraversable() 
	{
		return isRequestFocusEnabled();
	}
  
}
