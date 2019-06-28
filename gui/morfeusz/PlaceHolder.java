package morfeusz;

import java.awt.*;

import javax.swing.*;

@SuppressWarnings("serial")
public class PlaceHolder extends JPanel
{
	private boolean show;
	
	public PlaceHolder()
	{
		super();
		show = false;
		setBackground(Morfeusz.LIGHT_GRAY);
	}
	
	public void showBottomLine(boolean f)
	{
		if (show != f) {
			show = f;
			repaint();
		}
	}
	
	public void paintComponent(Graphics graphics)
	{
		super.paintComponent(graphics);
		if (show) {
			graphics.setColor(Color.lightGray);
			if (Morfeusz.isMacOS()) {
				graphics.drawLine(0,  getHeight() - 2,  getWidth(), getHeight() - 2);
				graphics.setColor(Color.white);
				graphics.drawLine(0,  getHeight() - 1,  getWidth(), getHeight() - 1);
			}
			else graphics.drawLine(0,  getHeight() - 1,  getWidth(),  getHeight() - 1);
		}
	}
	
}
