package morfeusz;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;
import javax.swing.filechooser.*;

@SuppressWarnings("serial")
public class SelectFileDialog extends JDialog 
{

	public static final int LOAD 	   = 0;
	public static final int SAVE 	   = 1;
	
	private int mode;
	private boolean accepted;
	private JFileChooser fileChooser;
	private JComboBox<String> encodingsCombo;
	
	public SelectFileDialog(Frame owner, int mode)
	{
		super(owner, true);
		this.mode = mode;
		initDialog();
		pack();
		setLocationRelativeTo(owner);
	}
	
	public void initDialog()
	{
		accepted = false;
		setPreferredSize(new Dimension(640, 480));
		fileChooser = new JFileChooser();
		fileChooser.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent e)
			{
				approveSelection((String)e.getActionCommand());
			}
		});
		if (mode == LOAD) {
			JPanel panel = new JPanel(new BorderLayout());
			JPanel accessoryContainer = new JPanel(new FlowLayout(FlowLayout.CENTER, 0, 0));
			JPanel accessory = new JPanel();
			JLabel label;
			
			setTitle("Select File");
			fileChooser.setDialogType(JFileChooser.OPEN_DIALOG);
			fileChooser.setApproveButtonText("Select");
			fileChooser.setApproveButtonMnemonic('S');
			fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
			fileChooser.addChoosableFileFilter(new FileNameExtensionFilter("Plain Text Files (*.txt)", "txt"));
			
			encodingsCombo = new JComboBox<String>();
			encodingsCombo.addItem("UTF8 (Eight-bit Unicode Transformation Format)");
			encodingsCombo.addItem("Cp1250 (Windows Eastern European)");
			encodingsCombo.addItem("Cp852 (MS-DOS Latin-2)");
			encodingsCombo.addItem("Cp870 (IBM Multilingual Latin-2)");
			encodingsCombo.addItem("ISO-8859-2 (ISO 8859-2, Latin Alphabet No. 2)");
			encodingsCombo.addItem("MacCentralEurope (Macintosh Latin-2)");
			
			accessoryContainer.setBorder(BorderFactory.createEmptyBorder(0, 0, 12, 0));
			label = new JLabel("Encoding: ");
			accessory.add(label);
			accessory.add(encodingsCombo);
		    accessoryContainer.add(accessory);
			panel.add(fileChooser, BorderLayout.CENTER);
			panel.add(accessoryContainer, BorderLayout.SOUTH);
			getContentPane().add(panel);
		}
		else {
			setTitle("Save Results");
			fileChooser.setDialogType(JFileChooser.SAVE_DIALOG);
			fileChooser.setApproveButtonText("Save");
			fileChooser.addChoosableFileFilter(new FileNameExtensionFilter("Plain Text Files (*.txt)", "txt"));
			getContentPane().add(fileChooser);
		}
	}
	
	public void approveSelection(String action)
	{
		File file;
		
		if (action.equals(JFileChooser.APPROVE_SELECTION)) {
			if (mode == SAVE) {
				file = fileChooser.getSelectedFile();
				if (file.exists()) {
					Object[] existMsg = new Object[] {
						"Plik \"" + file.getName() + "\" ju\u017C istnieje.",
						"Czy chcesz go zamieni\u0107?"
					};
					int answer = JOptionPane.showConfirmDialog(this, existMsg, "File Exists", 
						JOptionPane.YES_NO_CANCEL_OPTION, JOptionPane.QUESTION_MESSAGE);
					if (answer == JOptionPane.YES_OPTION) {
						accepted = true;
						setVisible(false);
					}
					else if (answer == JOptionPane.NO_OPTION) {
						accepted = false;
						setVisible(false);
					}
					else accepted = false;
				}
				else {
					accepted = true;
					setVisible(false);
				}
			}
			else {
				accepted = true;
				setVisible(false);
			}
		}
		else {
			accepted = false;
			setVisible(false);
		}
	}
	
	public void setSelectedFile(File file)
	{
		fileChooser.setSelectedFile(file);
	}
	
	public File getSelectedFile()
	{
		return fileChooser.getSelectedFile();
	}
	
	public void setCurrentDirectory(File dir)
	{
		fileChooser.setCurrentDirectory(dir);
	}
	
	public File getCurrentDirectory()
	{
		return fileChooser.getCurrentDirectory();
	}
	
	public String getEncoding()
	{
		String[] encoding = ((String)encodingsCombo.getSelectedItem()).split(" ");
		
		return encoding[0].trim();
	}
	
	public boolean accept()
	{
		return accepted;
	}
	
}
