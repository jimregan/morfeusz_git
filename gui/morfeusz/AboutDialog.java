package morfeusz;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

@SuppressWarnings("serial")
public class AboutDialog extends JPanel
{
	
	private static final String INFO = "<html><center><h3>Morfeusz</h3></center><br/>" + 
									    "<center>Analizator morfologiczny dla j\u0119zyka polskiego.<br/><br/>" +
									    "Wersja: " + pl.sgjp.morfeusz.Morfeusz.getVersion() + "</center></html>";
	private JDialog dialog;
	private JCheckBox pr = new JCheckBox("Program");
	private JCheckBox dict = new JCheckBox("S\u0142ownik");
	private JTextArea ta = new JTextArea(4, 58);
	private Rectangle rect = new Rectangle(0, 0, 0, 0);
	private String copyrights;
	private String dictCopyrights;
	
	public AboutDialog(pl.sgjp.morfeusz.Morfeusz morfeusz)
	{
		super(new BorderLayout());
		
		JLabel label = new JLabel(INFO);
		JPanel panel = new JPanel();
		JScrollPane sp = new JScrollPane(ta);
		ButtonGroup group = new ButtonGroup();
		Box vbox = Box.createVerticalBox();
		Box hbox = Box.createHorizontalBox();
		
		setBorder(BorderFactory.createEmptyBorder(8, 8, 8, 8));
		vbox.setBorder(BorderFactory.createEmptyBorder(16, 0, 0, 0));
		ta.setMargin(new Insets(4, 4, 4, 4));
		ta.setEditable(false);
		ta.setFont(Morfeusz.plainFont);
		panel.add(label);
		add(panel, BorderLayout.CENTER);
		label = new JLabel("Prawa autorskie: ");
		hbox.add(label);
		hbox.add(pr);
		hbox.add(dict);
		hbox.add(Box.createHorizontalGlue());
		group.add(pr);;
		group.add(dict);
		pr.setSelected(true);
		vbox.add(hbox);
		vbox.add(sp);
		add(vbox, BorderLayout.SOUTH);
		pr.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) 
			{
				setText();
			}
		});
		dict.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) 
			{
				setText();
			}
		});
		copyrights = pl.sgjp.morfeusz.Morfeusz.getCopyright();
		copyrights = copyrights.replaceFirst("Science\n", "Sciences\n");
		copyrights = copyrights.replaceFirst("\n$", "");
		dictCopyrights = morfeusz.getDictCopyright();
		dictCopyrights = dictCopyrights.replaceFirst("\n$", "");
		setText();
	}
	
	public void setText()
	{
		if (pr.isSelected()) ta.setText(copyrights);
		else ta.setText(dictCopyrights);
		EventQueue.invokeLater(new Runnable() {
			public void run() 
			{
				ta.scrollRectToVisible(rect);
			}
		});
	}
	
	public void doDialog()
	{
		dialog = new JDialog();
		dialog.setModal(true);
		dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
		dialog.setTitle("Informacje o programie");
		dialog.setResizable(false);
		dialog.getContentPane().add(this);
		dialog.pack();
		dialog.setLocationRelativeTo(null);
		dialog.setVisible(true);
	}
	
}
