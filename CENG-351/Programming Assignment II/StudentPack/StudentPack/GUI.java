import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;
import javax.swing.SwingUtilities;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;
import javax.swing.table.DefaultTableModel;

public class GUI {

	private JFrame frame;
	
	private JTable currentPokeTable;
	private JTable alreadyAddedPokeTable;
	private JTable deletePokeTable;
	
	private static final Object[] columnNames = new Object[]{"PokeKey", "PokeName", "PokePower", "PokeType"};
	private static final int horizontalSpacingBetween = 50;
	
	private ArrayList<CengPoke> currentPokes;
	private ArrayList<CengPoke> addedPokes;
	private ArrayList<CengPoke> deletedPokes;

	private JPanel bucketListPanel;
	private JPanel tablesPanel;
	
	private GUIConnectorContainer connectorContainerPanel;
	
	private GUIHashTable guiHashTable;
	private GUIBucketList guiBucketList;
	
	public GUI()
	{
		initialize();		
	}
	
	public void show()
	{
		frame.setVisible(true);
	}
	
	public void modelNeedsUpdate()
	{
		// System.out.println("Updating model...");
		
		updateBucketList();
		updateBuckets();
	}
	
	// Private methods
	
	private void updateBucketList()
	{
		frame.remove(bucketListPanel);
		addBucketListPanel();
		
		frame.getContentPane().revalidate();
		frame.getContentPane().repaint();
		
		frame.pack();
	}
	
	private void updateBuckets()
	{
		ArrayList<GUIHashRow> guiRows = guiHashTable.getHashRows();
		
		for(GUIHashRow guiHashRow : guiRows)
		{			
			CengBucket bucket = guiHashRow.bucketForRow();
			
			if (bucket != null)
			{
				GUIBucket guiBucket = guiBucketList.guiForBucket(bucket);
				
				if (guiBucket != null)
				{
					connectorContainerPanel.link(guiHashRow, guiBucket);
				}
			}
		}		
	}
	
	private void initialize()
	{
		currentPokes = new ArrayList<CengPoke>();
		addedPokes = new ArrayList<CengPoke>();
		deletedPokes = new ArrayList<CengPoke>();

		constructFrame();
		
		addBucketListPanel();
		addTablesPanel();
		
		addCurrentTable();
		addAlreadyAddedTable();
		addDeleteTable();
		
		updateBuckets();
		
        frame.pack();
	}
	
	private void constructFrame()
	{
		frame = new JFrame();
		frame.setSize(512, 384);
		frame.setLocationRelativeTo(null);
		frame.setVisible(true);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.getContentPane().setLayout(new BoxLayout(frame.getContentPane(), BoxLayout.X_AXIS));	
	}
	
	private void addBucketListPanel()
	{
		bucketListPanel = new JPanel();
		bucketListPanel.setLayout(new BoxLayout(bucketListPanel, BoxLayout.X_AXIS));
		
		CengHashTable hashTable = CengPokeKeeper.getHashTable();
		guiHashTable = new GUIHashTable(hashTable);
		
		bucketListPanel.add(guiHashTable);
		
		// Add spacing between table and the buckets
		
		bucketListPanel.add(Box.createRigidArea(new Dimension(horizontalSpacingBetween, 0)));

		CengBucketList bucketList = CengPokeKeeper.getBucketList();
		guiBucketList = new GUIBucketList(bucketList);
		
		bucketListPanel.add(guiBucketList);
		
		connectorContainerPanel = new GUIConnectorContainer();
		connectorContainerPanel.setLayout(null);
				
		frame.setGlassPane(connectorContainerPanel);
		
		connectorContainerPanel.setVisible(true);
		
		frame.getContentPane().add(bucketListPanel, 0);
	}
	
	private void addTablesPanel()
	{
		tablesPanel = new JPanel();
		tablesPanel.setLayout(new BoxLayout(tablesPanel, BoxLayout.Y_AXIS));
		
		frame.getContentPane().add(tablesPanel);
	}
	
	private void addCurrentTable()
	{
		JPanel tablePanel = new JPanel();
		tablePanel.setLayout(new BoxLayout(tablePanel, BoxLayout.Y_AXIS));
				
		DefaultTableModel currentGiftTableModel = new DefaultTableModel(columnNames, 0);

		currentPokeTable = new JTable(currentGiftTableModel)
		{
			// Anon class
			private static final long serialVersionUID = 1L;

			public boolean isCellEditable(int row, int column) 
	        {
                return false;               	
	        };
		};
		
		modifyTable(currentPokeTable);
		
		ArrayList<CengPoke> inputPokes = CengPokeParser.parsePokeFile(CengPokeKeeper.getInputFileName());
		
		for(CengPoke poke : inputPokes)
		{
			addPokeToTable(poke, currentPokeTable);
		}
		
		if(currentPokes.size() == 0)
		{
			currentPokes.addAll(inputPokes);
		}
		
        final JButton addButton = new JButton("Add Selected Poke");
        
        addButton.addActionListener(new ActionListener() 
        {
			@Override
			public void actionPerformed(ActionEvent e) 
			{
				addSelectedPoke();
			}
        });
        
        addButton.setAlignmentX(Component.CENTER_ALIGNMENT);
        tablePanel.add(addButton);
        tablePanel.add(currentPokeTable.getTableHeader());
		tablePanel.add(currentPokeTable);
	
		JScrollPane scrollPaneCurrentGifts = new JScrollPane(tablePanel);
		
		// Change width manually
        // scrollPaneCurrentGifts.getViewport().setPreferredSize(new Dimension(400, scrollPaneCurrentGifts.getViewport().getPreferredSize().height));

		tablesPanel.add(scrollPaneCurrentGifts);
	}

	private void addDeleteTable() {
		JPanel tablePanel = new JPanel();
		tablePanel.setLayout(new BoxLayout(tablePanel, BoxLayout.Y_AXIS));

		DefaultTableModel deleteGiftTableModel = new DefaultTableModel(columnNames, 0);

		deletePokeTable = new JTable(deleteGiftTableModel)
		{
			// Anon class
			private static final long serialVersionUID = 1L;

			public boolean isCellEditable(int row, int column)
			{
				return false;
			};
		};

		modifyTable(deletePokeTable);

		JPanel buttonPanel = new JPanel();
		buttonPanel.setLayout(new BoxLayout(buttonPanel, BoxLayout.X_AXIS));

		final JButton deleteButton = new JButton("Delete Selected Poke");

		deleteButton.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				deleteSelectedPoke();
			}
		});

		deleteButton.setAlignmentX(Container.CENTER_ALIGNMENT);

		buttonPanel.add(deleteButton);

		tablePanel.add(buttonPanel);

		tablePanel.add(deletePokeTable.getTableHeader());
		tablePanel.add(deletePokeTable);

		JScrollPane scrollPaneAddedGifts = new JScrollPane(tablePanel);

		frame.getContentPane().add(scrollPaneAddedGifts);

	}
	
	private void addAlreadyAddedTable()
	{
		JPanel tablePanel = new JPanel();
		tablePanel.setLayout(new BoxLayout(tablePanel, BoxLayout.Y_AXIS));
		
		DefaultTableModel addedGiftTableModel = new DefaultTableModel(columnNames, 0);
		
		alreadyAddedPokeTable = new JTable(addedGiftTableModel)
		{
			// Anon class
			private static final long serialVersionUID = 1L;

			public boolean isCellEditable(int row, int column) 
	        {
                return false;               	
	        };
		};
		
		modifyTable(alreadyAddedPokeTable);

		JPanel buttonPanel = new JPanel();
		buttonPanel.setLayout(new BoxLayout(buttonPanel, BoxLayout.X_AXIS));

		final JButton searchButton = new JButton("Search Selected Poke");
        
        searchButton.addActionListener(new ActionListener() 
        {
			@Override
			public void actionPerformed(ActionEvent e) 
			{
				searchSelectedPoke();
			}
        });
        
        searchButton.setAlignmentX(Container.CENTER_ALIGNMENT);
        
        buttonPanel.add(searchButton);
        
        tablePanel.add(buttonPanel);
        
        tablePanel.add(alreadyAddedPokeTable.getTableHeader());
		tablePanel.add(alreadyAddedPokeTable);
		
		JScrollPane scrollPaneAddedGifts = new JScrollPane(tablePanel);
		
		frame.getContentPane().add(scrollPaneAddedGifts);
	}

	private void deleteSelectedPoke() {
		if (alreadyAddedPokeTable.getSelectedRow() == -1) {
			return;
		}

		CengPoke selectedPoke = getSelectedPokeFromTable(deletePokeTable);

		System.out.println("delete\t" + selectedPoke.pokeKey());

		CengPokeKeeper.deletePoke(selectedPoke.pokeKey());

		addedPokes.remove(selectedPoke);

		Integer selectedRowIndex = deletePokeTable.getSelectedRow();

		removeSelectedPokeFromTable(alreadyAddedPokeTable, selectedRowIndex);

		deletedPokes.remove(selectedPoke);

		removeSelectionFromTable(deletePokeTable);

		currentPokes.add(selectedPoke);

		addPokeToTable(selectedPoke, currentPokeTable);
	}
	
	private void addSelectedPoke()
	{		
		if(currentPokeTable.getSelectedRow() == -1)
		{			
			return;
		}
		
		CengPoke selectedPoke = getSelectedPokeFromTable(currentPokeTable);
		
		System.out.println("add\t" + selectedPoke.fullName()); // Only for visual purposes
		
		CengPokeKeeper.addPoke(selectedPoke);
		
		currentPokes.remove(selectedPoke);

		addPokeToTable(selectedPoke, alreadyAddedPokeTable);
		
		addedPokes.add(selectedPoke);
		
		removeSelectionFromTable(currentPokeTable);

		deletedPokes.add(selectedPoke);

		addPokeToTable(selectedPoke, deletePokeTable);

	}

	private void searchSelectedPoke()
	{
		if(alreadyAddedPokeTable.getSelectedRow() == -1)
		{
			return;
		}
		
		CengPoke selectedPoke = getSelectedPokeFromTable(alreadyAddedPokeTable);
		
		System.out.println("search|" + selectedPoke.pokeKey()); // Only for visual purposes

		CengPokeKeeper.searchPoke(selectedPoke.pokeKey());
	}
	
	private CengPoke getSelectedPokeFromTable(JTable table)
	{
		Integer selectedRowIndex = table.getSelectedRow();
		
		if(table.equals(currentPokeTable))
		{
			return currentPokes.get(selectedRowIndex);
		}
		else
		{
			return addedPokes.get(selectedRowIndex);
		}
	}
	
	// Table Handlers
	
	private void addPokeToTable(CengPoke poke, JTable table)
	{
		DefaultTableModel tableModel = (DefaultTableModel) table.getModel();

		tableModel.addRow(new Object[]{poke.pokeKey(), poke.pokeName(), poke.pokePower(), poke.pokeType()});
	}

	private void removeSelectedPokeFromTable(JTable table, Integer selectedPokeIndex)
	{
		DefaultTableModel tableModel = (DefaultTableModel) table.getModel();
		tableModel.removeRow(selectedPokeIndex);
	}
	
	private void removeSelectionFromTable(JTable table)
	{
		Integer selectedRowIndex = table.getSelectedRow();

		DefaultTableModel tableModel = (DefaultTableModel) table.getModel();
		tableModel.removeRow(selectedRowIndex);		
	}
	
	private void modifyTable(final JTable table)
	{
		table.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		
		final DefaultTableModel tableModel = (DefaultTableModel) table.getModel();

	    	if(tableModel.getRowCount() != 0)
	    	{
	    		// Initialize table with first row selected.
	        	table.addRowSelectionInterval(0, 0);
	    	}
    	
		tableModel.addTableModelListener(new TableModelListener() {      
		    @Override
		    public void tableChanged(TableModelEvent e) 
		    {
		        SwingUtilities.invokeLater(new Runnable() 
		        {
		            @Override
		            public void run()
		            {
			            	if(tableModel.getRowCount() != 0)
			            	{
				            	table.addRowSelectionInterval(0, 0);
			            	}
		            }
		        });
		    }
		});
	}
}
