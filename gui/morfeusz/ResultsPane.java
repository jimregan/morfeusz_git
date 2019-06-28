package morfeusz;

import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.text.*;
import java.util.*;

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.table.*;

import pl.sgjp.morfeusz.*;

@SuppressWarnings("serial")
public class ResultsPane extends JTable 
{

	public static final String[] HEADERS1 = { "", "", "Forma", "Lemat", "Tag", "Nazwa", "Kwalifikatory" };
	public static final String[] HEADERS2 = { "Forma", "Lemat", "Tag", "Nazwa", "Kwalifikatory" };
	public static final Border emptyBorder = BorderFactory.createEmptyBorder(0, 2, 0, 2);
	public static final Border lineBorder = BorderFactory.createCompoundBorder(
		BorderFactory.createCompoundBorder(
			BorderFactory.createEmptyBorder(0, -1, -1, -1),
			BorderFactory.createLineBorder(Color.lightGray)),
		BorderFactory.createEmptyBorder(0, 2, 0, 2));
	
	private Morfeusz controller;
	private JTable resultsTable;
	private ColumnSorter columnSorter;
	private java.util.List<MorphInterpretation> results;
	private HashSet<Integer> interpBoundaries;
	private PlaceHolder placeHolder;
	private String[] headers;
	private TableColumn[] tableColumns;
	private boolean analyze;
	private boolean columnsAreVisible;
	
	public ResultsPane(boolean analyze, Morfeusz controller, PlaceHolder placeHolder)
	{
		super();
		this.controller = controller;
		this.analyze = analyze;
		this.placeHolder = placeHolder;
		resultsTable = this;
		initTable();
	}
	
	public void initTable()
	{

		if (analyze) headers = HEADERS1;
		else headers = HEADERS2;
		
		TableViewModel model = new TableViewModel();
		FontMetrics fm = getFontMetrics(Morfeusz.boldFont);

		columnSorter = new ColumnSorter();
		setModel(model);
		setBorder(BorderFactory.createCompoundBorder(
				BorderFactory.createEmptyBorder(-1, -1 , 0, -1),
				BorderFactory.createLineBorder(Color.lightGray)));
		setBackground(Morfeusz.LIGHT_GRAY);
		setGridColor(Color.lightGray);
		setShowHorizontalLines(false);
		setAutoResizeMode(AUTO_RESIZE_OFF);
		setRowSelectionAllowed(false);
		setRowHeight(fm.getHeight() + 4);

		TableColumnModel columnModel = getColumnModel();
		TableCellRenderer cellRenderer = new RowRenderer();
		TableHeader tableHeader = new TableHeader(columnModel);
		setTableHeader(tableHeader);
		tableColumns = new TableColumn[headers.length];

		for (int i = 0; i < columnModel.getColumnCount(); i++) {
			TableColumn column = columnModel.getColumn(i);
			
			tableColumns[i] = column;
			column.setHeaderValue(headers[i]);
			column.setCellRenderer(cellRenderer);
			column.setHeaderRenderer(tableHeader.getDefaultRenderer());
		}
		
		getTableHeader().setReorderingAllowed(false);
		columnsAreVisible = true;
		showColumns(false);
	}
	
	public void sortTable(int sortedColumn)
	{
		if (controller.getPreferences().sortedColumn != sortedColumn) {
			controller.getPreferences().sortedColumn = sortedColumn;
			columnSorter.setSortedColumn(sortedColumn);
			resetResults();
		}
	}
	
	public void showColumns(boolean show)
	{
		if (columnsAreVisible != show) {
			columnsAreVisible = show;
			for (int i = 0; i < tableColumns.length; i++) {
				if (show) addColumn(tableColumns[i]);
				else removeColumn(tableColumns[i]);
			}
			if (show) {
				TableColumnModel columnModel = getColumnModel();
				
				for (int i = 0; i < columnModel.getColumnCount(); i++) {
					TableColumn column = columnModel.getColumn(i);
					
					column.setHeaderRenderer(tableHeader.getDefaultRenderer());
				}
			}
		}
		placeHolder.showBottomLine(show);
	}
	
	public void calculateSizes()
	{
		TableColumnModel columnModel = getColumnModel();
		TableColumn tableColumn;
		TableCellRenderer renderer;
		Component component;
		Object value;
		int w;
		
		for (int column = 0; column < columnModel.getColumnCount(); column++) {
			w = 0;
			tableColumn = columnModel.getColumn(column);
			for (int row = 0; row < getRowCount(); row++) {
				renderer = getCellRenderer(row, column);
				value = getValueAt(row, column);
				component = renderer.getTableCellRendererComponent(this, 
					value, false, false, row, column);
				w = Math.max (component.getPreferredSize().width, w); 
			}
			renderer = tableColumn.getHeaderRenderer();
			value = tableColumn.getHeaderValue();
			component = renderer.getTableCellRendererComponent(this, 
				value, false, false, 0, column);
			w = Math.max (component.getPreferredSize().width, w);
			columnModel.getColumn(column).setPreferredWidth(w + 4);
		}
	}
	
	public void changeFont()
	{
		FontMetrics fm = getFontMetrics(Morfeusz.boldFont);
		TableHeader tableHeader = new TableHeader(columnModel);
		TableColumnModel columnModel = getColumnModel();
		
		setRowHeight(fm.getHeight() + 4);
		for (int i = 0; i < columnModel.getColumnCount(); i++) {
			TableColumn column = columnModel.getColumn(i);
			
			column.setHeaderRenderer(tableHeader.getDefaultRenderer());
		}
		calculateSizes();
		revalidate();
		repaint();
		getTableHeader().repaint();
	}
	
	public boolean isCellEditable(int row, int column)
	{
		return false;
	}
	
	public void setResults(java.util.List<MorphInterpretation> results, 
		HashSet<Integer> interpBoundaries)
	{
		this.results = results;
		this.interpBoundaries = interpBoundaries;
		resetResults();
	}
	
	public void resetResults()
	{
		if (results == null) showColumns(false);
		else {
			if (!analyze) Collections.sort(results, columnSorter);
			showColumns(true);
			calculateSizes();
			revalidate();
			repaint();
		}
		getTableHeader().repaint();
	}
	
	public boolean newNode(int row)
	{
		boolean f = true;
		
		if (row > 0) {
			MorphInterpretation i1 = results.get(row);
			MorphInterpretation i2 = results.get(row - 1);
			int sn1 = i1.getStartNode(), en1 = i1.getEndNode(),
				sn2 = i2.getStartNode(), en2 = i2.getEndNode();
			
			if (sn1 == sn2 && en1 == en2 && !interpBoundaries.contains(row)) f = false;
		}
		return f;
	}
	
	private class TableViewModel extends AbstractTableModel
	{
		
		public TableViewModel()
		{
			super();
		}
		
		public int getRowCount()
		{
			if (results == null) return 0;
			return (int)results.size();
		}
		
		public int getColumnCount()
		{
			return headers.length;
		}
		
		public Object getValueAt(int row, int column)
		{
			MorphInterpretation interp = results.get(row);
			boolean f;
			
			if (analyze) f = newNode(row);
			else {
				f = true;
				column += 2;
			}
				
			switch (column) {
				case 0 : if (f) return(Integer.toString(interp.getStartNode()));
						 return null;
				case 1 : if (f) return(Integer.toString(interp.getEndNode()));
						 return null;
				case 2 : if (f) {
							String str = interp.getOrth();
							
							if (str.contains(" ")) str = str.replace(' ', '\u23B5');
							return(str);
						 }
						 return null;
				case 3 : return(interp.getLemma());
				case 4 : return(interp.getTag(controller.getMorfeusz()));
				case 5 : return(interp.getName(controller.getMorfeusz()));
				case 6 : 
					Collection<String> labels = interp.getLabels(controller.getMorfeusz());
					String str = null;
					boolean append = false;
					
					if (labels.size() > 0) {
						str = "[";
						for (String s : labels) {
							if (append) str += "; ";
							str += s;
							append = true;
						}
						str += "]";
					}
					return str;
				default : return null;
			}
		}
		
	}
	
	private class RowRenderer extends DefaultTableCellRenderer
	{
		
		public Component getTableCellRendererComponent(JTable table, Object value,
				boolean isSelected, boolean hasFocus, int row, int column)
		{
			JLabel label = (JLabel)super.getTableCellRendererComponent(table, value, isSelected, hasFocus, row, column);

			if (analyze) {
				boolean f = newNode(row) && row > 0;
				
				if (column <= 1) label.setHorizontalAlignment(JLabel.CENTER);
				else label.setHorizontalAlignment(JLabel.LEFT);
				if (column == 2) setFont(Morfeusz.boldFont);
				else if (column == 5 || column == 6) setFont(Morfeusz.italicFont);
				else setFont(Morfeusz.plainFont);
				if (f) setBorder(lineBorder);
				else setBorder(emptyBorder);
			}
			else {
				if (column == 0) setFont(Morfeusz.boldFont);
				else if (column == 1) setFont(Morfeusz.plainFont);
				else setFont(Morfeusz.italicFont);
				setBorder(emptyBorder);
			}
			if (row % 2 == 0) setBackground(Morfeusz.VLIGHT_BLUE);
			else setBackground(Color.white);
			setForeground(Color.black);
			setValue(value);
			return this;
		}
		
	}
	
	private class TableHeader extends JTableHeader
	{
		
		private PlaceHolder corner = new PlaceHolder();
		private boolean initialized = false;
		private int cursorPos = -1;
		private int column = -1;
		
		public TableHeader(TableColumnModel model)
		{
			super(model);
			setDefaultRenderer(new HeaderRenderer());
			setResizingAllowed(true);
			corner.setLocation(0, 0);
			corner.showBottomLine(columnsAreVisible);
			add(corner);
			if (!analyze) {
				addMouseListener(new MouseAdapter() 
				{
					public void mouseClicked(MouseEvent event)
					{
						cursorPos = event.getX();
						
						TableColumnModel tcm = getColumnModel();
						
						column = tcm.getColumnIndexAtX(event.getX());
						
						if (column >= 0 && column <= 2) sortTable(column);
					}
					
					public void mouseReleased(MouseEvent event)
					{
					    cursorPos = event.getX();
					    
						TableColumnModel tcm = getColumnModel();
						
						column = tcm.getColumnIndexAtX(cursorPos);
						adjustCursor();
					}
				});
				addMouseMotionListener(new MouseMotionAdapter() {
					public void mouseMoved(MouseEvent event) 
					{
						cursorPos = event.getX();
						
					    TableColumnModel model = resultsTable.getColumnModel();
					    int c = model.getColumnIndexAtX(cursorPos);
					    
					    TableColumn col = null;
					    if (c >= 0 && c <= 2) col = model.getColumn(c);
					    if (col != null) {
					    	if (c != column) {
					    		switch (c) {
					    			case 0 : setToolTipText("Sortuj formy."); break;
					    			case 1 : setToolTipText("Sortuj lematy."); break;
					    			case 2 : setToolTipText("Sortuj tagi."); break;
					    			default : setToolTipText(null);
					    		}
					    	}
					    }
					    else setToolTipText(null);
					    if (c != column) column = c;
					    adjustCursor();
					}
				});
			}
		}
		
		public void adjustCursor()
		{
		    if (column >= 0 && column <= 2) {
		    	if (isResizeSpot(column, cursorPos)) 
		    		setCursor(Cursor.getPredefinedCursor(Cursor.E_RESIZE_CURSOR));
		    	else setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
		    }
		    else {
		    	if (isResizeSpot(column, cursorPos))
		    		setCursor(Cursor.getPredefinedCursor(Cursor.E_RESIZE_CURSOR));
		    	else setCursor(Cursor.getDefaultCursor());
		    }
		}
		
		public boolean isResizeSpot(int column, int x)
		{
			TableColumnModel model = resultsTable.getColumnModel();
			Rectangle rect = getHeaderRect(column);
			int left, right, count = model.getColumnCount();
			
			if (column < 0 || column >= count) return false;
			else if (column == 0) {
				left = rect.x;
				right = left + rect.width - 3;
			}
			else {
				left = rect.x += 3;
				right = left + rect.width - 6;
			}
			if (x >= left && x < right) return false;
			return true;
		}
		
		public Dimension getPreferredSize()
		{
			Dimension dim = super.getPreferredSize();
			FontMetrics fm = getFontMetrics(Morfeusz.boldFont);
			
			dim.height = fm.getHeight() + 4;
			return dim;
		}
		
		public void setBounds(int x, int y, int width, int height)
		{
			super.setBounds(x,  y,  width,  height);
			moveCorner();
			initialized = true;
		}
		
		public void moveCorner()
		{
			TableColumnModel tcm = getColumnModel();
			Rectangle rect;
			
			rect = getHeaderRect(tcm.getColumnCount() - 1);
			corner.setLocation(rect.x + rect.width, 0);
			corner.setSize(getWidth() - rect.x - rect.width, getHeight());
		}
		
		public void resizeAndRepaint()
		{
			super.resizeAndRepaint();
			if (initialized) {
				moveCorner();
				corner.showBottomLine(columnsAreVisible);
			}
		}
	}
	
	private class HeaderRenderer implements TableCellRenderer
	{
		
		private JLabel renderer;
		private Icon startNode, endNode, downArrow;
		
		public HeaderRenderer()
		{
			Border headerBorder = UIManager.getBorder("TableHeader.cellBorder");
			URL url;
			
			renderer = new JLabel();
			renderer.setFont(Morfeusz.boldFont);
			renderer.setBorder(headerBorder);
			renderer.setBackground(UIManager.getColor("TableHeader.background"));
			renderer.setForeground(UIManager.getColor("TableHeader.foreground"));
			if (analyze) {
				url = ClassLoader.getSystemClassLoader().getResource("start_node.gif");
				startNode = new ImageIcon(url);
				url = ClassLoader.getSystemClassLoader().getResource("end_node.gif");
				endNode = new ImageIcon(url);
			}
			else {
				url = ClassLoader.getSystemClassLoader().getResource("down.gif");
				downArrow = new ImageIcon(url);
			}
		}
		
		public Component getTableCellRendererComponent(JTable table,
			Object value, boolean isSelected, boolean hasFocus, int row, int column) 
		{
			renderer.setText((String)value);
			if (analyze) {
				if (column == 0) {
					renderer.setIcon(startNode);
					renderer.setHorizontalAlignment(JLabel.CENTER);
				}
				else if (column == 1) {
					renderer.setIcon(endNode);
					renderer.setHorizontalAlignment(JLabel.CENTER);
				}
				else {
					renderer.setIcon(null);
					renderer.setHorizontalAlignment(JLabel.LEFT);
				}
			}
			else {
				int sortedColumn = controller.getPreferences().sortedColumn;
				
				if (column == sortedColumn)	renderer.setIcon(downArrow);
				else renderer.setIcon(null);
				renderer.setHorizontalAlignment(JLabel.LEFT);
			}
			return renderer;
		}
		
	}
	
	private class ColumnSorter implements Comparator<MorphInterpretation>
	{

		private Collator collator = Collator.getInstance(new Locale("pl", "PL"));
		private int sortedColumn;
		
		public ColumnSorter()
		{
			sortedColumn = controller.getPreferences().sortedColumn;
		}
		
		public void setSortedColumn(int sortedColumn)
		{
			this.sortedColumn = sortedColumn;
		}
		
		public int compare(MorphInterpretation mi1, MorphInterpretation mi2) 
		{
			if (sortedColumn == 0) return compareForms(mi1, mi2);
			else if (sortedColumn == 1) return compareLemmas(mi1, mi2);
			return compareTags(mi1, mi2);
		}
		
		public int compareForms(MorphInterpretation mi1, MorphInterpretation mi2)
		{
			String s1 = mi1.getOrth(), s2 = mi2.getOrth();
			int res = collator.compare(s1, s2);
			
			if (res == 0) {
				s1 = mi1.getLemma(); 
				s2 = mi2.getLemma();
				res = collator.compare(s1, s2);
			}
			return res;
		}
		
		public int compareLemmas(MorphInterpretation mi1, MorphInterpretation mi2)
		{
			String s1 = mi1.getLemma(), s2 = mi2.getLemma();
			int res = collator.compare(s1, s2);
			
			if (res == 0) {
				s1 = mi1.getOrth();
				s2 = mi2.getOrth();
				res = collator.compare(s1, s2);
			}
			return res;
		}
		
		public int compareTags(MorphInterpretation mi1, MorphInterpretation mi2)
		{
			String s1 = mi1.getTag(controller.getMorfeusz()), s2 = mi2.getTag(controller.getMorfeusz());
			
			return collator.compare(s1,  s2);
		}
		
	}
	
}
