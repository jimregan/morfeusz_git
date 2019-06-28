package morfeusz;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.datatransfer.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;

import javax.swing.*;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.table.*;

@SuppressWarnings("serial")
public class DictSearchPathsDialog extends JPanel
{
	
	private JDialog dialog;
	private PathList pathList;
	private JButton acceptButton;
	private JButton cancelButton;
	private JButton addButton;
	private JButton removeButton;
	private JButton defaultsButton;
	private List<String> searchPaths;
	private List<String> defaultSearchPaths;
	private int ret;

	public DictSearchPathsDialog(List<String> searchPaths, List<String> defaultSearchPaths)
	{
		super(new BorderLayout());
		setBorder(BorderFactory.createEmptyBorder(16, 16, 16, 16));
		setPreferredSize(new Dimension(600, 200));
		this.searchPaths = searchPaths;
		this.defaultSearchPaths = defaultSearchPaths;
		arrangeComponents();
	}
	
	public void arrangeComponents()
	{
		JLabel label = new JLabel("\u015Acie\u017Cka wyszukiwania s\u0142ownik\u00F3w:");
		Box hbox = Box.createHorizontalBox();
		JScrollPane sp;
		Dimension dim;
		
		label.setBorder(BorderFactory.createEmptyBorder(0, 0, 2, 0));
		add(label, BorderLayout.NORTH);
		pathList = new PathList();
		sp = new JScrollPane(pathList);
		sp.setBorder(BorderFactory.createEtchedBorder());
		sp.getViewport().setBackground(Color.white);
		add(sp, BorderLayout.CENTER);
		addButton = new JButton("Dodaj");
		removeButton = new JButton("Usu\u0144");
		defaultsButton = new JButton("Domy\u015Blna");
		cancelButton = new JButton("Anuluj");
		acceptButton = new JButton("Akceptuj");
		dim = defaultsButton.getPreferredSize();
		addButton.setPreferredSize(dim);
		removeButton.setPreferredSize(dim);
		cancelButton.setPreferredSize(dim);
		acceptButton.setPreferredSize(dim);
		hbox.setBorder(BorderFactory.createEmptyBorder(16, 0, 0, 0));
		hbox.add(addButton);
		hbox.add(Box.createHorizontalStrut(8));
		hbox.add(removeButton);
		hbox.add(Box.createHorizontalStrut(8));
		hbox.add(Box.createHorizontalGlue());
		hbox.add(defaultsButton);
		hbox.add(Box.createHorizontalGlue());
		hbox.add(Box.createHorizontalStrut(8));
		hbox.add(cancelButton);
		hbox.add(Box.createHorizontalStrut(8));
		hbox.add(acceptButton);
		add(hbox, BorderLayout.SOUTH);
		removeButton.setEnabled(false);
		addButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent event) 
			{
				addPath();
			}
		});
		removeButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent event) 
			{
				removePath();
			}
		});
		defaultsButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent event) 
			{
				defaults();
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
				modifySearchPaths();
				ret = 1;
				dialog.setVisible(false);
			}
		});
	}
	
	public void addPath()
	{
		JFileChooser chooser = new JFileChooser();
		
		chooser.setDialogTitle("Select Files");
		chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
		chooser.setMultiSelectionEnabled(true);
		
		int state = chooser.showOpenDialog(this);
		if (state == JFileChooser.APPROVE_OPTION) {
			File[] files = chooser.getSelectedFiles();
			DefaultTableModel model = (DefaultTableModel)pathList.getModel();
			Object[] row = new Object[1];
			
			for (int i = 0; i < files.length; i++) {
				row[0] = files[i].getPath();
				model.addRow(row);
			}
		}
	}
	
	public void removePath()
	{
		int[] rows = pathList.getSelectedRows();
		DefaultTableModel model = (DefaultTableModel)pathList.getModel();
		
		for (int i = 0; i < rows.length; i++) {
			model.removeRow(rows[i] - i);
		}
	}
	
	public void defaults()
	{
		DefaultTableModel model = (DefaultTableModel)pathList.getModel();
		int count = model.getRowCount();
		
		for (int i = 0; i < count; i++) {
			model.removeRow(0);
		}
		if (defaultSearchPaths != null && !defaultSearchPaths.isEmpty()) {
			Object[] row = new Object[1];
			
			for (String path : defaultSearchPaths) {
				row[0] = path;
				model.addRow(row);
			}
		}
	}
	
	public void modifySearchPaths()
	{
		DefaultTableModel model = (DefaultTableModel)pathList.getModel();
		int count = model.getRowCount();
		
		searchPaths.clear();
		for (int i = 0; i < count; i++) {
			String path = (String)model.getValueAt(i, 0);
			
			searchPaths.add(path);
		}
	}
	
	public int doDialog()
	{
		ret = 0;
		dialog = new JDialog();
		dialog.setModal(true);
		dialog.setDefaultCloseOperation(JDialog.HIDE_ON_CLOSE);
		dialog.setTitle("Ustawianie \u015Bcie\u017Cki wyszukiwania s\u0142ownik\u00F3w");
		dialog.setResizable(true);
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

	private class PathList extends JTable
	{
		
		public PathList()
		{
			super(new PathListModel());
			
			setBorder(BorderFactory.createEmptyBorder());
			setBackground(Color.white);
			setTableHeader(null);
			setRowSelectionAllowed(true);
			setDragEnabled(true);
			setShowHorizontalLines(false);
			setShowVerticalLines(false);
			setTransferHandler(new PathListTransferHandler());
			getSelectionModel().addListSelectionListener(new ListSelectionListener() {
				public void valueChanged(ListSelectionEvent event) 
				{
					removeButton.setEnabled(getSelectedRowCount() > 0);
				}
			});
			
			DefaultTableModel model = (DefaultTableModel)getModel();
			Object[] row = new Object[1];

			for (String path : searchPaths) {
				row[0] = path;
				model.addRow(row);
			}
		}
		
	}
	
	private class PathListModel extends DefaultTableModel
	{
	
		public PathListModel()
		{
			super(0, 1);
		}
		
		public Class<? extends Object> getColumnClass(int col)
		{
			Vector<?> v = (Vector<?>)getDataVector().elementAt(0);
			return v.elementAt(col).getClass();
		}
		
		public boolean isCellEditable(int row, int column)
		{
			return false;
		}
		
	}
	
	private class PathListTransferHandler extends TransferHandler
	{
		
		private boolean canImport;
		private boolean accept;
		private int[]	rows;
		private int		addIndex;
		private int		addCount;
	
		public PathListTransferHandler()
		{
			canImport  = false;
			accept = false;
			rows = null;
			addIndex = -1;
			addCount = 0;
		}
		
		protected Transferable createTransferable(JComponent c)
		{
			accept = true;
			return new StringSelection(exportString(c));
		}
		
		protected String exportString(JComponent c)
		{
			JTable table = (JTable)c;
			StringBuffer buff = new StringBuffer();
			
			rows = table.getSelectedRows();
			for (int i = 0; i < rows.length; i++) {
				String s = (String)table.getValueAt(rows[i], 0);
				buff.append(s);
				if (i != rows.length - 1) buff.append("\n");
			}
			return buff.toString();
		}
		
		protected void importString(JComponent c, String str)
		{
			JTable target = (JTable)c;
			DefaultTableModel model = (DefaultTableModel)target.getModel();
			int index = target.getSelectedRow();
			
			if (rows != null && index >= rows[0] - 1 && 
				index <= rows[rows.length - 1])
			{
				rows = null;
				return;
			}
			
			int max = model.getRowCount();
			
			if (index < 0) index = max;
			else {
				index++;
				if (index > max) index = max;
			}
			
			addIndex = index;
			String[] rvalues = str.split("\n");
			addCount = rvalues.length;
			
			for (int i = 0; i < rvalues.length; i++) {
				Object[] row = new Object[1];
				
				row[0] = rvalues[i];
				model.insertRow(index++, row);
			}
		}
		
		public int getSourceActions(JComponent c)
		{
			return COPY_OR_MOVE;
		}
		
		public boolean importData(JComponent c, Transferable t)
		{
			if (canImport(c, t.getTransferDataFlavors())) {
				canImport = true;
				try {
					String str = (String)t.getTransferData(DataFlavor.stringFlavor);
					importString(c, str);
					return true;
				}
				catch (UnsupportedFlavorException ufe) {
				}
				catch (IOException ioe) {
				}
			}
			return false;
		}
		
		protected void exportDone(JComponent c, Transferable data, int action)
		{
			if (canImport) {
				JTable source = (JTable)c;
				
				if (rows != null) {
					DefaultTableModel model = (DefaultTableModel)source.getModel();
					
					if (addCount > 0) {
						for (int i = 0; i < rows.length; i++) {
							if (rows[i] > addIndex) rows[i] += addCount;
						}
					}
					for (int i = rows.length - 1; i >= 0; i--) model.removeRow(rows[i]);
				}
				rows = null;
				addCount = 0;
				addIndex = -1;
				canImport = false;
			}
			accept = false;
		}
		
		public boolean canImport(JComponent c, DataFlavor[] flavors)
		{
			if (accept) {
				for (int i = 0; i < flavors.length; i++) {
					if (DataFlavor.stringFlavor.equals(flavors[i])) return true;
				}
			}
			return false;
		}
				
	}
	
}
