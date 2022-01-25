import java.awt.Dimension;
import java.util.ArrayList;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JPanel;

@SuppressWarnings("serial")
public class GUIBucketList extends JPanel {

	private static final int verticalSpacing = 10;
	
	private ArrayList<GUIBucket> bucketList;
	
	public GUIBucketList(CengBucketList bucketList)
	{
		this.bucketList = new ArrayList<GUIBucket>();
		
		setupStyle();
		setupBuckets(bucketList);
		
		this.revalidate();
		this.repaint();
	}
	
	public ArrayList<GUIBucket> getBuckets()
	{
		return this.bucketList;
	}
	
	public GUIBucket guiForBucket(CengBucket bucket)
	{
		for (GUIBucket guiBucket : bucketList)
		{
			if (guiBucket.ownsBucket(bucket))
			{
				return guiBucket;
			}
		}
		
		return null;
	}
	
	private void setupStyle()
	{
		this.setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
	}
	
	private void setupBuckets(CengBucketList bucketList)
	{
		for(int index = 0; index < bucketList.bucketCount(); index++)
		{
			CengBucket bucket = bucketList.bucketAtIndex(index);
			
			if (bucket != null)
			{
				GUIBucket guiBucket = new GUIBucket(bucket);
				this.add(guiBucket);
				this.bucketList.add(guiBucket);
				
				if (index != bucketList.bucketCount() - 1)
				{
					// If not last, add a box between.
					
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
