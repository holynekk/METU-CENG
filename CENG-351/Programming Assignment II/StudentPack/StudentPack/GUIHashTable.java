import java.awt.Color;
import java.awt.Dimension;
import java.util.ArrayList;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.border.TitledBorder;

@SuppressWarnings("serial")
public class GUIHashTable extends JPanel {
	
	private static final int verticalSpacing = 5;
	
	private ArrayList<GUIHashRow> guiHashRows;
	
	public GUIHashTable(CengHashTable hashTable)
	{			
		this.guiHashRows = new ArrayList<GUIHashRow>();

		setupHeaderWith(hashTable.prefixBitCount());
		setupRowsWith(hashTable);
		
		this.revalidate();
		this.repaint();
	}
	
	public ArrayList<GUIHashRow> getHashRows()
	{
		return this.guiHashRows;
	}
	
	private void setupHeaderWith(int prefixCount)
	{
		this.setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		
		TitledBorder visitedBorder = BorderFactory.createTitledBorder(String.valueOf(prefixCount));
		visitedBorder.setBorder(BorderFactory.createMatteBorder(2, 2, 2, 2, Color.black));
		
		this.setBorder(visitedBorder);
	}
	
	private void setupRowsWith(CengHashTable hashTable)
	{
		for(int index = 0; index < hashTable.rowCount(); index++)
		{
			CengHashRow hashRow = hashTable.rowAtIndex(index);
			
			if (hashRow != null)
			{
				GUIHashRow guiRow = new GUIHashRow(hashRow);
				
				this.add(guiRow);
				this.guiHashRows.add(guiRow);
				
				if (index != hashTable.rowCount() - 1)
				{
					// If not last, add a box.
					
					this.addRigidBox();
				}
			}
		}
	}
	
	private void addRigidBox()
	{
		this.add(Box.createRigidArea(new Dimension(0, verticalSpacing)));
	}
}
