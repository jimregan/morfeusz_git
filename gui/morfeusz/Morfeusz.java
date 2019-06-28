package morfeusz;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.text.*;
import java.util.*;

import javax.swing.*;
import javax.swing.event.*;

import quitaboutpreferenceshandler.*;

@SuppressWarnings("serial")
public class Morfeusz extends JFrame
{

	public static final String WINDOW_TITLE = "Analizator Morfologiczny Morfeusz";
	public static final String STANDARD = "Standardowy (" + pl.sgjp.morfeusz.Morfeusz.getDefaultDictName() + ")";
	public static final String DICT_MESSAGE = "<html>Wyst\u0105pi\u0142 b\u0142\u0105d przy pr\u00F3bie zamiany s\u0142ownika.<br/>" + 
		"S\u0142ownik nie zosta\u0142 zmieniony.</html>";
	public static final String NATIVE_ERROR_MESSAGE = "Wyst\u0105pi\u0142 nieznany b\u0142\u0105d";
	public static final Color LIGHT_GRAY = new Color(240, 240, 240);
	public static final Color VLIGHT_BLUE	= new Color(237, 243, 254);
	public static final Font FONT = new Font(Font.SANS_SERIF, Font.PLAIN, 10);
	public static final int NUM_OF_FONT_SIZES = 10;
	
	private static boolean isMacOS;
	
	public static Font plainFont;
	public static Font boldFont;
	public static Font italicFont; 
	private java.util.List<String> defaultSearchPaths;
	private java.util.List<String> dictSearchPaths;
	private java.util.List<String> dictionaries;
	private Agent agent;
	private Preferences preferences;
	private JTabbedPane tabbedPane;
	private AnalyzeView analyzeView;
	private GenerateView generateView;
	private JTextArea textPane;
	private JTextField lemmaField;
	private ResultsPane analyzePane;
	private ResultsPane generatePane;
	private Action exitAction;
	private Action aboutAction;
	private Action optionsAction;
	private Action resetAction;
	private Action analyzeAction;
	private Action generateAction;
	private Action openAction;
	private Action saveActionA;
	private Action saveActionG;
	private Action changeAction;
	private Action changeToStandardAction;
	private Action editAction;
	private Action increaseFontAction;
	private Action decreaseFontAction;
	private Action insertNewLineAction;
	private JButton exitButton;
	private JButton resetButton;
	private JButton actionButton;
	private ToolBarButton returnButton;
	private ToolBarButton openButton;
	private ToolBarButton saveButtonA;
	private ToolBarButton saveButtonG;
	private ToolBarButton changeButtonA;
	private ToolBarButton changeButtonG;
	private ToolBarButton aboutButton;
	private ToolBarButton optionsButton;
	private ToolBarButton increaseFontButton;
	private ToolBarButton decreaseFontButton;
	private JCheckBox appendCheckBoxA;
	private JCheckBox appendCheckBoxG;
	private JPopupMenu changePopup;
	private JLabel analyzeDictionaryName;
	private JLabel generateDictionaryName;
	private File inputFile;
	private File outputFile;
	private DictFilter filter;
	private String dictionary;
	private boolean analyze;
	private Rectangle topLeft = new Rectangle(0, 0, 0, 0);
	
	public Morfeusz()
	{
		String str = System.getProperty("os.name");
		
		if (str != null && str.contains("Mac OS X")) {
			new QuitAboutPreferencesHandler() {
				public void handleQuitRequest() 
				{
					exit();
				}
				
				public void handleAboutRequest()
				{
					about();
				}

				public void handlePreferencesRequest() 
				{
					options();
				}
			};
			System.setProperty("apple.laf.useScreenMenuBar", "true");
			isMacOS = true;
		}
		else isMacOS = false;
		
		readPreferences();
		plainFont = FONT.deriveFont(Font.PLAIN, preferences.fontSize + 10);
		boldFont = FONT.deriveFont(Font.BOLD, preferences.fontSize + 10);
		italicFont = FONT.deriveFont(Font.ITALIC, preferences.fontSize + 10);
		initialize();
		agent = Agent.createAgent(this);
		if (agent == null) {
			JOptionPane.showMessageDialog(null, NATIVE_ERROR_MESSAGE, "Uwaga", JOptionPane.ERROR_MESSAGE);
			exit();
		}
		setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
		setTitle(WINDOW_TITLE);
		setSizeAndPosition();
		arrangeComponents();
		getRootPane().setDefaultButton(actionButton);
		checkComponents();
		addWindowListener(new WindowAdapter()
		{
			public void windowClosing(WindowEvent event)
			{
				exit();
			}
		});
	}
	
	public void initialize()
	{
		openAction = new AbstractAction("Czytaj z pliku") {
			public void actionPerformed(ActionEvent e)
			{
				open();
			}
		};

		optionsAction = new AbstractAction("Opcje programu") {
			public void actionPerformed(ActionEvent e)
			{
				options();
			}
		};
		
		saveActionA = new AbstractAction("Zapisz Wyniki Analizy") {
			public void actionPerformed(ActionEvent e)
			{
				save();
			}
		};
		
		saveActionG = new AbstractAction("Zapisz Wygenerowane Formy") {
			public void actionPerformed(ActionEvent e)
			{
				save();
			}
		};
		
		changeToStandardAction = new AbstractAction("S\u0142ownik Standardowy") {
			public void actionPerformed(ActionEvent e)
			{
				changeToStandardDictionary();
			}
		};
		
		changeAction = new AbstractAction() {
			public void actionPerformed(ActionEvent e)
			{
				changeDictionary();
			}
		};
		
		editAction = new AbstractAction("Ustaw \u015Acie\u017Ck\u0119 Wyszukiwania S\u0142ownik\u00F3w...") {
			public void actionPerformed(ActionEvent e)
			{
				editDictSearchPaths();
			}
		};
		
		exitAction = new AbstractAction("Zako\u0144cz") {
			public void actionPerformed(ActionEvent e)
			{
				exit();
			}
		};
		
		aboutAction = new AbstractAction("About") {
			public void actionPerformed(ActionEvent e)
			{
				about();
			}
		};
		
		increaseFontAction = new AbstractAction("Powi\u0119ksz czcionk\u0119") {
			public void actionPerformed(ActionEvent e)
			{
				changeFontSize(true);
			}
		};
		
		decreaseFontAction = new AbstractAction("Zmniejsz czcionk\u0119") {
			public void actionPerformed(ActionEvent e)
			{
				changeFontSize(false);
			}
		};
		
		resetAction = new AbstractAction("Wyczy\u015B\u0107") {
			public void actionPerformed(ActionEvent e)
			{
				reset();
			}
		};
		
		analyzeAction = new AbstractAction("Analizuj") {
			public void actionPerformed(ActionEvent e)
			{
				analyze();
			}
		};
		
		generateAction = new AbstractAction("Generuj") {
			public void actionPerformed(ActionEvent e)
			{
				generate();
			}
		};
		
		insertNewLineAction = new AbstractAction() {
			public void actionPerformed(ActionEvent e)
			{
				insertNewLine();
			}
		};
		
		filter = new DictFilter();
		dictSearchPaths = pl.sgjp.morfeusz.Morfeusz.getDictionarySearchPaths();
		defaultSearchPaths = new LinkedList<String>();
		for (String path : dictSearchPaths) defaultSearchPaths.add(path);
		
		if (preferences.dictPaths != null) {
			dictSearchPaths.clear();
			for (String path : preferences.dictPaths) {
				dictSearchPaths.add(path);
			}
		}
		else if (dictSearchPaths.size() > 0) {
			int i = 0;
			
			preferences.dictPaths = new String[dictSearchPaths.size()];
			for (String path : dictSearchPaths) {
				preferences.dictPaths[i++] = path;
			}
		}
		getDictionaries();
		if (preferences.dictionary == null) dictionary = STANDARD;
		else dictionary = preferences.dictionary;
		if (!dictionaries.contains(dictionary))  dictionary = STANDARD;
		analyze = true;
	}
	
	public void getDictionaries()
	{
		ListIterator<String> it;
		
		dictionaries = new LinkedList<String>();
		if (dictSearchPaths.size() > 0) {
			it = dictSearchPaths.listIterator();
			while (it.hasNext()) {
				String path = it.next();
				File dir = new File(path);
				
				if (dir.exists()) addDictionaries(dir);
			}
		}
		else {
			File currentDir = new File(System.getProperty("user.dir"));	
			
			addDictionaries(currentDir);
		}
		changePopup = new JPopupMenu();
		changePopup.add(changeToStandardAction);
		changePopup.addSeparator();
		if (!dictionaries.isEmpty()) {
			for (String d : dictionaries) {
				changePopup.add(new RecentFileAction(d, this));
			}
			changePopup.addSeparator();
		}
		changePopup.add(editAction);
	}
	
	public void addDictionaries(File dir)
	{
		String[] d = dir.list(filter);
		
		for (String str : d) {
			int index = str.lastIndexOf("-");
			
			str = str.substring(0, index);
			if (!dictionaries.contains(str)) dictionaries.add(str);
		}
	}
	
	public void setSizeAndPosition()
	{
		Toolkit	toolkit = Toolkit.getDefaultToolkit();
		Dimension dim = toolkit.getScreenSize();
		GraphicsDevice dev = GraphicsEnvironment.getLocalGraphicsEnvironment().getDefaultScreenDevice();
		GraphicsConfiguration conf = dev.getDefaultConfiguration();
		Insets insets	= toolkit.getScreenInsets(conf);
		Rectangle rect = new Rectangle();
		
		rect.x = (dim.width - insets.left - insets.right) / 4;
		rect.y = (dim.height - insets.top - insets.bottom) / 8;
		rect.width = 2 * rect.x;
		rect.height = 6 * rect.y;
		
		setBounds(rect);
	}
	
	public void arrangeComponents()
	{
		JPanel mainView = new JPanel(new BorderLayout());
		Box hbox, vbox;
		
		mainView.setBorder(BorderFactory.createEmptyBorder(16, 16, 16, 16));
		tabbedPane = new JTabbedPane();
		analyzeView = new AnalyzeView();
		generateView = new GenerateView();
		tabbedPane.setFocusable(false);
		tabbedPane.addTab("Analizator", analyzeView);
		tabbedPane.addTab("Generator", generateView);
		mainView.add(tabbedPane, BorderLayout.CENTER);
		
		exitButton = new JButton(exitAction);
		resetButton = new JButton(resetAction);
		actionButton = new JButton(analyzeAction);
		aboutButton = new ToolBarButton(aboutAction, "info");
		aboutButton.setToolTipText("Informacje o programie.");
		optionsButton = new ToolBarButton(optionsAction, "conf");
		optionsButton.setToolTipText("Parametry programu.");
		increaseFontButton = new ToolBarButton(increaseFontAction, "font_increase");
		increaseFontButton.setToolTipText("Powi\u0119ksz czcionk\u0119.");
		decreaseFontButton = new ToolBarButton(decreaseFontAction, "font_decrease");
		decreaseFontButton.setToolTipText("Zmniejsz czcionk\u0119.");
		increaseFontButton.setEnabled(preferences.fontSize < NUM_OF_FONT_SIZES - 1);
		decreaseFontButton.setEnabled(preferences.fontSize > 0);
		
		vbox = Box.createVerticalBox();
		if (!isMacOS) vbox.add(Box.createVerticalStrut(16));
		hbox = Box.createHorizontalBox();
		hbox.add(aboutButton);
		hbox.add(Box.createHorizontalStrut(8));
		hbox.add(optionsButton);
		hbox.add(Box.createHorizontalStrut(8));
		hbox.add(increaseFontButton);
		hbox.add(Box.createHorizontalStrut(8));
		hbox.add(decreaseFontButton);
		hbox.add(Box.createHorizontalStrut(8));
		hbox.add(exitButton);
		hbox.add(Box.createHorizontalStrut(8));
		hbox.add(resetButton);
		hbox.add(Box.createHorizontalGlue());
		hbox.add(Box.createHorizontalStrut(8));
		hbox.add(actionButton);
		vbox.add(hbox);
		mainView.add(vbox, BorderLayout.SOUTH);
		getContentPane().add(mainView);
		
		tabbedPane.addChangeListener(new ChangeListener() {
	        public void stateChanged(ChangeEvent e) 
	        {
	        	int i = tabbedPane.getSelectedIndex();
	        	
	        	if (i == 0) {
	        		actionButton.setAction(analyzeAction);
	        		analyze = true;
	        	}
	        	else {
	        		actionButton.setAction(generateAction);
	        		analyze = false;
	        	}
	        	checkComponents();
	        }
	    });
	}
	
	public void open()
	{
		SelectFileDialog dialog = new SelectFileDialog(this, SelectFileDialog.LOAD);
		
		dialog.setVisible(true);
		if (dialog.accept()) {
			inputFile = dialog.getSelectedFile();
			readText(inputFile, dialog.getEncoding());
		}
		dialog.dispose();
	}
	
	public void readText(File file, String encoding)
	{
		try {
			BufferedReader reader = 
				new BufferedReader(new InputStreamReader(new FileInputStream(file), encoding));
			String line;
			boolean addNewLine = false;
			
			textPane.setText("");
			while ((line = reader.readLine()) != null) {
				if (addNewLine) textPane.append("\n");
				textPane.append(Normalizer.normalize(line, Normalizer.Form.NFC));
				addNewLine = true;
			}
			reader.close();
			EventQueue.invokeLater(new Runnable() {
				public void run()
				{
					textPane.scrollRectToVisible(topLeft);
				}
			});
		}
		catch (Exception exception) {
			exception.getStackTrace();
		}
	}
	
	public void save()
	{
		boolean select = true;
		boolean append = (analyze ? appendCheckBoxA.isSelected() : appendCheckBoxG.isSelected());
		
		if (append) {
			if (outputFile != null && outputFile.exists()) select = false;
		}
		if (select) {
			SelectFileDialog dialog = new SelectFileDialog(this, SelectFileDialog.SAVE);
			
			dialog.setSelectedFile(new File("Results.txt"));
			dialog.setVisible(true);
			if (dialog.accept()) {
				outputFile = dialog.getSelectedFile();
				save(outputFile, append);
				appendCheckBoxA.setSelected(true);
			}
		}
		else {
			save(outputFile, append);
		}
	}
	
	public void save(File file, boolean append)
	{
		try {
			OutputStreamWriter writer = new OutputStreamWriter(
				new FileOutputStream(file, append), "UTF8");
			BufferedWriter bw = new BufferedWriter(writer);
			PrintWriter pw = new PrintWriter(bw);
			
			if (append) pw.println();
			if (analyze) agent.saveAnalyzeResults(pw);
			else agent.saveGenerateResults(pw);
			pw.flush();
			pw.close();
		}
		catch (Exception exception) {
			exception.getStackTrace();
		}
	}
	
	public void changeDictionaryName(String name)
	{
		dictionary = name;
		if (name.equals(STANDARD)) {
			analyzeDictionaryName.setText(name);
			analyzeDictionaryName.setForeground(Color.gray);
			generateDictionaryName.setText(name);
			generateDictionaryName.setForeground(Color.gray);
		}
		else {
			analyzeDictionaryName.setText(name);
			analyzeDictionaryName.setForeground(Color.black);
			generateDictionaryName.setText(name);
			generateDictionaryName.setForeground(Color.black);
		}
	}
	
	public void changeToStandardDictionary()
	{
		preferences.dictionary = null;
		agent.setDictionary(null);
		refreshAnalyzeResults();
		refreshGenerateResults();
		changeDictionaryName(STANDARD);
	}
	
	public void changeDictionary()
	{
		if (analyze) changePopup.show(changeButtonA, 0, 0);
		else changePopup.show(changeButtonG, 0, 0);
	}
	
	public void changeDictionary(String name)
	{
		if (agent.setDictionary(name)) {
			preferences.dictionary = name;
			changeDictionaryName(name);
			refreshAnalyzeResults();
			refreshGenerateResults();
		}
		else {
			JOptionPane.showMessageDialog(null, DICT_MESSAGE, "Uwaga", JOptionPane.ERROR_MESSAGE);
			removeFromChangePopup(name);
		}
	}
	
	public void removeFromChangePopup(String name)
	{
		JMenuItem item = null;
		int count = changePopup.getComponentCount() - 2;
		
		for (int i = 2; i < count; i++) {
			JMenuItem menuItem = (JMenuItem)changePopup.getComponent(i);
			RecentFileAction action = (RecentFileAction)menuItem.getAction();
			
			if (action.getName().equals(name)) {
				item = menuItem;
				break;
			}
		}
		if (item != null) {
			changePopup.remove(item);
			if (count == 3) changePopup.remove(2);
		}
	}
	
	public void exit()
	{
		savePreferences();
		System.exit(0);
	}
	
	public void insertNewLine()
	{
		textPane.insert("\n", textPane.getCaretPosition());
	}
	
	public void options()
	{
		OptionsDialog dialog = new OptionsDialog(this);
		
		if (dialog.doDialog() != 0) {
			dialog.transfer(preferences);
			agent.initialize();
			refreshAnalyzeResults();
			refreshGenerateResults();
		}
		dialog.dispose();
	}
	
	public void editDictSearchPaths()
	{
		DictSearchPathsDialog dialog = new DictSearchPathsDialog(dictSearchPaths, defaultSearchPaths);
		
		if (dialog.doDialog() != 0) {
			getDictionaries();
			preferences.dictPaths = null;
			if (dictSearchPaths.size() > 0) {
				int i = 0;
				
				preferences.dictPaths = new String[dictSearchPaths.size()];
				for (String path : dictSearchPaths) {
					preferences.dictPaths[i++] = path;
				}
			}
		}
		dialog.dispose();
	}
	
	public void readPreferences()
	{
		String path = System.getProperty("user.home") + File.separator + ".morfeuszGUI";
		File file = new File(path);
		ObjectInputStream input;
		
		try {
			input = new ObjectInputStream(new FileInputStream(file));
			preferences = (Preferences)input.readObject();
			input.close();
		}
		catch (Exception exception) {
			preferences = new Preferences();
			preferences.setDefaults();
		}
	}
	
	public void savePreferences()
	{
		String path = System.getProperty("user.home") + File.separator + ".morfeuszGUI";
		File file = new File(path);
		ObjectOutputStream output;

		try {
			output = new ObjectOutputStream(new FileOutputStream(file));
			output.writeObject(preferences);
			output.close();
		}
		catch (Exception exception) {
		}
	}
	
	public Preferences getPreferences()
	{
		return preferences;
	}
	
	public void about()
	{
		AboutDialog dialog = new AboutDialog(agent.getMorfeusz());
		
		dialog.doDialog();
	}
	
	public void changeFontSize(boolean increase)
	{
		if (increase) preferences.fontSize++;
		else preferences.fontSize--;
		increaseFontButton.setEnabled(preferences.fontSize < NUM_OF_FONT_SIZES - 1);
		decreaseFontButton.setEnabled(preferences.fontSize > 0);
		plainFont = new Font(plainFont.getFontName(), plainFont.getStyle(), preferences.fontSize + 10);
		boldFont = new Font(boldFont.getFontName(), boldFont.getStyle(), preferences.fontSize + 10);
		italicFont = new Font(italicFont.getFontName(), italicFont.getStyle(), preferences.fontSize + 10);
		textPane.setFont(plainFont);
		analyzeView.changeFont();
		generateView.changeFont();
	}
	
	public pl.sgjp.morfeusz.Morfeusz getMorfeusz()
	{
		if (agent != null) return agent.getMorfeusz();
		return null;
	}
	
	public void reset()
	{
		if (analyze) {
			textPane.setText(null);
			textPane.requestFocusInWindow();
			analyzePane.showColumns(false);
			agent.clearAnalyzeResults();
		}
		else {
			lemmaField.setText(null);
			lemmaField.requestFocusInWindow();
			generatePane.showColumns(false);
			agent.clearGenerateResults();
		}
		checkComponents();
		System.gc();
	}
	
	public void analyze()
	{
		agent.setTextToAnalyze(textPane.getText());
		if (agent.analyze()) {
			if (agent.hasAnalyzeResults()) {
				analyzePane.setResults(agent.getAnalyzeResults(), agent.getInterpBoundaries());
				analyzePane.scrollRectToVisible(topLeft);
			}
			else analyzePane.setResults(null, null);
			checkComponents();
		}
		else {
			JOptionPane.showMessageDialog(null, NATIVE_ERROR_MESSAGE, "Uwaga", JOptionPane.ERROR_MESSAGE);
			analyzePane.showColumns(false);
			agent.clearAnalyzeResults();
		}
		System.gc();
	}
	
	public void refreshAnalyzeResults()
	{
		if (agent.hasAnalyzeResults()) {
			if (agent.analyze()) {
				if (agent.hasAnalyzeResults()) {
					analyzePane.setResults(agent.getAnalyzeResults(), agent.getInterpBoundaries());
					analyzePane.scrollRectToVisible(topLeft);
				}
				else analyzePane.setResults(null, null);
				checkComponents();
			}
			else {
				JOptionPane.showMessageDialog(null, NATIVE_ERROR_MESSAGE, "Uwaga", JOptionPane.ERROR_MESSAGE);
				analyzePane.showColumns(false);
				agent.clearAnalyzeResults();
			}
			System.gc();
		}
	}
	
	public void generate()
	{
		String text = lemmaField.getText();
		String[] t;
		
		text = text.trim();
		t = text.split(" ");
		text = t[0];
		lemmaField.setText(text);
		agent.setLemma(lemmaField.getText());
		if (agent.generate()) {
			if (agent.hasGenerateResults()) {
				generatePane.setResults(agent.getGenerateResults(), null);
				generatePane.scrollRectToVisible(topLeft);
			}
			else generatePane.setResults(null, null);
			checkComponents();
		}
		else {
			JOptionPane.showMessageDialog(null, NATIVE_ERROR_MESSAGE, "Uwaga", JOptionPane.ERROR_MESSAGE);
			generatePane.showColumns(false);
			agent.clearGenerateResults();
		}
		System.gc();
	}
	
	public void refreshGenerateResults()
	{
		if (agent.hasGenerateResults()) {
			if (agent.generate()) {
				if (agent.hasGenerateResults()) {
					generatePane.setResults(agent.getGenerateResults(), null);
					generatePane.scrollRectToVisible(topLeft);
				}
				else generatePane.setResults(null, null);
				checkComponents();
			}
			else {
				JOptionPane.showMessageDialog(null, NATIVE_ERROR_MESSAGE, "Uwaga", JOptionPane.ERROR_MESSAGE);
				generatePane.showColumns(false);
				agent.clearGenerateResults();
			}
			System.gc();
		}
	}
	
	public void checkComponents()
	{
		boolean f1 = (analyze && textPane.getDocument().getLength() > 0),
				f2 = (!analyze && lemmaField.getDocument().getLength() > 0),
				f3 = (analyze && agent.hasAnalyzeResults()),
				f4 = (!analyze && agent.hasGenerateResults());
		
		analyzeAction.setEnabled(f1);
		generateAction.setEnabled(f2);
		saveActionA.setEnabled(f3);
		saveActionG.setEnabled(f4);
		resetAction.setEnabled(f1 || f2 || f3 || f4);
	}
	
	public static boolean isMacOS()
	{
		return isMacOS;
	}
	
	public static void main(String[] args) 
	{
		Morfeusz morfeuszWnd = new Morfeusz();
		
		morfeuszWnd.setVisible(true);
	}

	private class AnalyzeView extends JPanel
	{
		
		public AnalyzeView()
		{
			super(new BorderLayout());
			arrangeComponents();
		}
		
		public void arrangeComponents()
		{
			Box hbox, vbox;
			JLabel label;
			JScrollPane sp;
			PlaceHolder placeHolder;
			KeyStroke enterStroke = KeyStroke.getKeyStroke(KeyEvent.VK_ENTER, 0);
			InputMap im;
			int x = (isMacOS() ? -4 : -2);
			
			vbox = Box.createVerticalBox();
			vbox.setBorder(BorderFactory.createEmptyBorder(16, 16, 8, 16));
			hbox = Box.createHorizontalBox();
			hbox.setBorder(BorderFactory.createCompoundBorder(
				BorderFactory.createTitledBorder("S\u0142ownik:"),
				BorderFactory.createEmptyBorder(4, 4, 8, 4)));
			analyzeDictionaryName = new JLabel(dictionary);
			if (dictionary.equals(STANDARD)) analyzeDictionaryName.setForeground(Color.gray);
			hbox.add(analyzeDictionaryName);
			hbox.add(Box.createHorizontalGlue());
			hbox.add(Box.createHorizontalStrut(8));
			changeButtonA = new ToolBarButton(changeAction, "book_open");
			changeButtonA.setFocusable(false);
			changeButtonA.setToolTipText("Zmie\u0144 s\u0142ownik.");
			hbox.add(changeButtonA);
			vbox.add(hbox);
			vbox.add(Box.createVerticalStrut(16));
			hbox = Box.createHorizontalBox();
			hbox.setBorder(BorderFactory.createEmptyBorder(0, 0, x, 0));
			label = new JLabel("Tekst:");
			hbox.add(label);
			hbox.add(Box.createHorizontalGlue());
			hbox.add(Box.createHorizontalStrut(32));
			returnButton = new ToolBarButton(insertNewLineAction, "return");
			returnButton.setFocusable(false);
			returnButton.setToolTipText("Nowa linia.");
			hbox.add(returnButton);
			hbox.add(Box.createHorizontalGlue());
			hbox.add(Box.createHorizontalStrut(32));
			openButton = new ToolBarButton(openAction, "open");
			openButton.setFocusable(false);
			openButton.setToolTipText("Czytaj tekst z pliku.");
			hbox.add(openButton);
			vbox.add(hbox);
			
			textPane = new JTextArea();
			textPane.setFont(plainFont);
			textPane.setLineWrap(true);
			textPane.setWrapStyleWord(true);
			textPane.setBorder(BorderFactory.createEmptyBorder(2, 4, 2, 4));
			im = textPane.getInputMap();
			textPane.getActionMap().put(im.get(enterStroke), analyzeAction);
			sp = new JScrollPane(textPane);
			sp.setPreferredSize(new Dimension(0, 100));
			add(sp);
			vbox.add(sp);
			add(vbox, BorderLayout.NORTH);
			
			vbox = Box.createVerticalBox();
			vbox.setBorder(BorderFactory.createEmptyBorder(8, 16, 16, 16));
			hbox = Box.createHorizontalBox();
			hbox.setBorder(BorderFactory.createEmptyBorder(0, 0, x, 0));
			label = new JLabel("Analiza morfologiczna:");
			hbox.add(label);
			hbox.add(Box.createHorizontalGlue());
			appendCheckBoxA = new JCheckBox("Dopisz   ");
			appendCheckBoxA.setFocusable(false);
			hbox.add(appendCheckBoxA);
			saveButtonA = new ToolBarButton(saveActionA, "save");
			saveButtonA.setFocusable(false);
			saveButtonA.setToolTipText("Zapisz wynik analizy do pliku.");
			hbox.add(saveButtonA);
			vbox.add(hbox);
			
			placeHolder = new PlaceHolder();
			analyzePane = new ResultsPane(true, Morfeusz.this, placeHolder);
			sp = new JScrollPane(analyzePane);
			sp.getViewport().setBackground(LIGHT_GRAY);
			sp.setCorner(JScrollPane.UPPER_RIGHT_CORNER, placeHolder);
			vbox.add(sp);
			add(vbox, BorderLayout.CENTER);
			
			textPane.getDocument().addDocumentListener(new DocumentListener() {
				public void changedUpdate(DocumentEvent event) 
				{
				}

				public void insertUpdate(DocumentEvent event) 
				{
					checkComponents();
				}

				public void removeUpdate(DocumentEvent event) 
				{
					checkComponents();
				}
			});
			
			appendCheckBoxA.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent event) 
				{
					appendCheckBoxG.setSelected(appendCheckBoxA.isSelected());
				}
			});
		}
		
		public void changeFont()
		{
			analyzePane.changeFont();
		}
		
	}
	
	private class GenerateView extends JPanel
	{
		
		public GenerateView()
		{
			super(new BorderLayout());
			arrangeComponents();
		}
		
		public void arrangeComponents()
		{
			Box hbox, vbox;
			JLabel label;
			JScrollPane sp;
			PlaceHolder placeHolder;
			Dimension dim;
			int x = (isMacOS() ? -4 : -2);
			
			vbox = Box.createVerticalBox();
			hbox = Box.createHorizontalBox();
			hbox.setBorder(BorderFactory.createCompoundBorder(
				BorderFactory.createTitledBorder("S\u0142ownik:"),
				BorderFactory.createEmptyBorder(4, 4, 8, 4)));
			generateDictionaryName = new JLabel(dictionary);
			if (dictionary.equals(STANDARD)) generateDictionaryName.setForeground(Color.gray);
			hbox.add(generateDictionaryName);
			hbox.add(Box.createHorizontalGlue());
			hbox.add(Box.createHorizontalStrut(8));
			changeButtonG = new ToolBarButton(changeAction, "book_open");
			changeButtonG.setFocusable(false);
			changeButtonG.setToolTipText("Zmie\u0144 s\u0142ownik.");
			hbox.add(changeButtonG);
			vbox.add(hbox);
			vbox.add(Box.createVerticalStrut(20));
			hbox = Box.createHorizontalBox();
			vbox.setBorder(BorderFactory.createEmptyBorder(16, 16, 8, 16));
			label = new JLabel("Lemat:");
			hbox.add(label);
			hbox.add(Box.createHorizontalStrut(8));
			lemmaField = new JTextField(30);
			hbox.add(lemmaField);
			dim = lemmaField.getPreferredSize();
			lemmaField.setMaximumSize(dim);
			hbox.add(Box.createHorizontalGlue());
			vbox.add(hbox);
			add(vbox, BorderLayout.NORTH);
			
			vbox = Box.createVerticalBox();
			vbox.setBorder(BorderFactory.createEmptyBorder(8, 16, 16, 16));
			hbox = Box.createHorizontalBox();
			hbox.setBorder(BorderFactory.createEmptyBorder(0, 0, x, 0));
			label = new JLabel("Wygenerowane formy:");
			hbox.add(label);
			hbox.add(Box.createHorizontalGlue());
			appendCheckBoxG = new JCheckBox("Dopisz   ");
			appendCheckBoxG.setFocusable(false);
			hbox.add(appendCheckBoxG);
			saveButtonG = new ToolBarButton(saveActionG, "save");
			saveButtonG.setFocusable(false);
			saveButtonG.setToolTipText("Zapisz wygenerowane formy wyrazowe do pliku.");
			hbox.add(saveButtonG);
			vbox.add(hbox);
			
			placeHolder = new PlaceHolder();
			generatePane = new ResultsPane(false, Morfeusz.this, placeHolder);
			sp = new JScrollPane(generatePane);
			sp.getViewport().setBackground(LIGHT_GRAY);
			sp.setCorner(JScrollPane.UPPER_RIGHT_CORNER, placeHolder);
			vbox.add(sp);
			add(vbox, BorderLayout.CENTER);
			
			lemmaField.getDocument().addDocumentListener(new DocumentListener() {
				public void changedUpdate(DocumentEvent event) 
				{
				}

				public void insertUpdate(DocumentEvent event) 
				{
					checkComponents();
				}

				public void removeUpdate(DocumentEvent event) 
				{
					checkComponents();
				}
			});
			
			appendCheckBoxG.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent event) 
				{
					appendCheckBoxA.setSelected(appendCheckBoxG.isSelected());
				}
			});
		}
		
		public void changeFont()
		{
			generatePane.changeFont();
		}

	}
	
	public class DictFilter implements FilenameFilter
	{

		public boolean accept(File dir, String name) 
		{
			return (name.endsWith("-a.dict") || name.endsWith("-s.dict"));
		}
		
	}
	
}
