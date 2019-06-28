package morfeusz;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.GroupLayout.*;

import pl.sgjp.morfeusz.*;

@SuppressWarnings("serial")
public class OptionsDialog extends JPanel
{

	private JDialog dialog;
	private JButton acceptButton;
	private JButton cancelButton;
	private JButton defaultsButton;
	private JComboBox<String> agglCombo;
	private JComboBox<String> praetCombo;
	private JComboBox<String> caseCombo;
	private JComboBox<String> spaceCombo;
	private JComboBox<String> tokenCombo;
	private Morfeusz controller;
	private int ret;
	
	public OptionsDialog(Morfeusz controller)
	{
		super(new BorderLayout());
		this.controller = controller;
		arrangeComponents();
		initiateValues();
	}
	
	public void arrangeComponents()
	{
		JLabel label1, label2, label3, label4, label5;
		JPanel paramPanel = new JPanel();
		Box bottomPanel = Box.createHorizontalBox();
		GroupLayout gl = new GroupLayout(paramPanel);
		GroupLayout.SequentialGroup hGroup = gl.createSequentialGroup();
		GroupLayout.SequentialGroup vGroup = gl.createSequentialGroup();
		Dimension dim;
		
		acceptButton = new JButton("Akceptuj");
		cancelButton = new JButton("Anuluj");
		defaultsButton = new JButton("Warto\u015Bci domy\u015Blne");
		label1 = new JLabel("aggl: ");
		label2 = new JLabel("praet: ");
		label3 = new JLabel("case-handling: ");
		label4 = new JLabel("whitespace-handling: ");
		label5 = new JLabel("token-numbering: ");
		agglCombo = new JComboBox<String>();
		for (String str : controller.getMorfeusz().getAvailableAgglOptions()) agglCombo.addItem(str);
		praetCombo = new JComboBox<String>();
		for (String str : controller.getMorfeusz().getAvailablePraetOptions()) praetCombo.addItem(str);
		caseCombo = new JComboBox<String>();
		for (CaseHandling ch : CaseHandling.values()) caseCombo.addItem(ch.name());
		spaceCombo = new JComboBox<String>();
		for (WhitespaceHandling wh : WhitespaceHandling.values()) spaceCombo.addItem(wh.name());
		tokenCombo = new JComboBox<String>();
		for (TokenNumbering tn : TokenNumbering.values()) tokenCombo.addItem(tn.name());
		if (!Morfeusz.isMacOS()) {
			gl.setAutoCreateGaps(true);
			gl.setAutoCreateContainerGaps(true);
		}
		paramPanel.setLayout(gl);
		hGroup.addGroup(gl.createParallelGroup().addComponent(label1).
			addComponent(label2).addComponent(label3).
			addComponent(label4).addComponent(label5));
		hGroup.addGroup(gl.createParallelGroup().addComponent(agglCombo).
			addComponent(praetCombo).addComponent(caseCombo).
			addComponent(spaceCombo).addComponent(tokenCombo));
		gl.setHorizontalGroup(hGroup);
	    vGroup.addGroup(gl.createParallelGroup(Alignment.BASELINE).
		    addComponent(label1).addComponent(agglCombo));
	    vGroup.addGroup(gl.createParallelGroup(Alignment.BASELINE).
			    addComponent(label2).addComponent(praetCombo));
	    vGroup.addGroup(gl.createParallelGroup(Alignment.BASELINE).
			    addComponent(label3).addComponent(caseCombo));
	    vGroup.addGroup(gl.createParallelGroup(Alignment.BASELINE).
			    addComponent(label4).addComponent(spaceCombo));
	    vGroup.addGroup(gl.createParallelGroup(Alignment.BASELINE).
			    addComponent(label5).addComponent(tokenCombo));
		gl.setVerticalGroup(vGroup);
		bottomPanel.setBorder(BorderFactory.createEmptyBorder(16, 0, 0, 0));
		bottomPanel.add(defaultsButton);
		bottomPanel.add(Box.createHorizontalStrut(8));
		bottomPanel.add(Box.createHorizontalGlue());
		bottomPanel.add(cancelButton);
		bottomPanel.add(Box.createHorizontalStrut(8));
		bottomPanel.add(acceptButton);
		dim = acceptButton.getPreferredSize();
		cancelButton.setPreferredSize(dim);
		setBorder(BorderFactory.createEmptyBorder(16, 16, 16, 16));
		add(paramPanel, BorderLayout.CENTER);
		add(bottomPanel, BorderLayout.SOUTH);
		defaultsButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent event) 
			{
				setDefaults();
			}
		});
		cancelButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent event) 
			{
				dialog.setVisible(false);
			}
		});
		acceptButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent event) 
			{
				ret = 1;
				dialog.setVisible(false);
			}
		});
	}
	
	public void setDefaults()
	{
		Preferences prefs = new Preferences();
		
		prefs.setMorfeuszDefaults();
		initiateValues(prefs);
	}
	
	public void initiateValues()
	{
		initiateValues(controller.getPreferences());
	}
	
	public void initiateValues(Preferences prefs)
	{
		agglCombo.setSelectedItem(prefs.aggl);
		praetCombo.setSelectedItem(prefs.praet);
		caseCombo.setSelectedItem(prefs.caseHandling.name());
		spaceCombo.setSelectedItem(prefs.whitespaceHandling.name());
		tokenCombo.setSelectedItem(prefs.tokenNumbering.name());
	}
	
	public void transfer(Preferences prefs)
	{
		prefs.aggl = (String) agglCombo.getSelectedItem();
		prefs.praet = (String) praetCombo.getSelectedItem();
		prefs.caseHandling = CaseHandling.values()[caseCombo.getSelectedIndex()];
		prefs.whitespaceHandling = WhitespaceHandling.values()[spaceCombo.getSelectedIndex()];
		prefs.tokenNumbering = TokenNumbering.values()[tokenCombo.getSelectedIndex()];
	}
	
	public int doDialog()
	{
		ret = 0;
		dialog = new JDialog();
		dialog.setModal(true);
		dialog.setDefaultCloseOperation(JDialog.HIDE_ON_CLOSE);
		dialog.setTitle("Opcje programu");
		dialog.setResizable(false);
		dialog.getContentPane().add(this);
		dialog.pack();
		dialog.setLocationRelativeTo(null);
		dialog.setVisible(true);
		return ret;
	}
	
	public void dispose()
	{
		if (dialog != null) {
			dialog.dispose();
			dialog = null;
		}
	}
	
}
