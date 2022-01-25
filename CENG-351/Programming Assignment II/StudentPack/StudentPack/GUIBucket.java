import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.util.ArrayList;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.border.TitledBorder;

@SuppressWarnings("serial")
public class GUIBucket extends JPanel {
	
	private CengBucket bucket;
	
	private ArrayList<JLabel> labels;
	private ArrayList<JPanel> paddings;
	
	private static final int minYValue = 10;
	private static final int maxYValue = 30;

	public GUIBucket(CengBucket bucket)
	{
		this.bucket = bucket;
		this.labels = new ArrayList<JLabel>();
		this.paddings = new ArrayList<JPanel>();
		
		this.setupHeaderWith(bucket.getHashPrefix());
				
		for(int index = 0; index < bucket.pokeCount(); index++)
		{
			String keyString = String.valueOf(bucket.pokeAtIndex(index).pokeKey());
						
			JLabel keyLabel = new JLabel(keyString);
			keyLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
			keyLabel.setAlignmentY(Component.CENTER_ALIGNMENT);
			
			labels.add(keyLabel);
			
			this.add(this.getNewPadding());
			this.add(Box.createHorizontalStrut(5));
			this.add(keyLabel);
			this.add(Box.createHorizontalStrut(5));
		}
		
		if (bucket.pokeCount() != 0)
		{
			this.add(this.getNewPadding());
		}
		
		this.setMaximumSize(new Dimension(10000, 80));
		
		this.revalidate();
		this.repaint();
	}

	public boolean ownsBucket(CengBucket bucket)
	{
		return this.bucket.equals(bucket);
	}
	
	public boolean isVisited()
	{
		return this.bucket.isVisited();
	}
	
	private void setupHeaderWith(int prefixCount)
	{		
		Color borderColor;
		
		if(this.bucket.isVisited())
		{
			borderColor = Color.GREEN;
		}
		else
		{
			borderColor = Color.BLACK;
		}
		
		TitledBorder visitedBorder = BorderFactory.createTitledBorder(String.valueOf(prefixCount));
		visitedBorder.setBorder(BorderFactory.createMatteBorder(2, 2, 2, 2, borderColor));
		
		this.setBorder(visitedBorder);
	}
	
	private JPanel getNewPadding()
	{
		JPanel padding = new JPanel();
		padding.setAlignmentX(Component.CENTER_ALIGNMENT);
		padding.setAlignmentY(Component.CENTER_ALIGNMENT);
		padding.setBorder(BorderFactory.createMatteBorder(1, 1, 1, 1, Color.black));
		padding.setMaximumSize(new Dimension(10, maxYValue));
		padding.setPreferredSize(new Dimension(10, (maxYValue + minYValue) / 2));
		padding.setMinimumSize(new Dimension(10, minYValue));
		
		this.paddings.add(padding);
		
		return padding;
	}
}
