import java.util.ArrayList;

public class CengBucketList {

	private int bucket_count;
	private static ArrayList<CengBucket> all_buckets;

	public CengBucketList()
	{
		// TODO: Empty Implementation
		bucket_count = 1;
		this.all_buckets = new ArrayList<CengBucket>();
		this.all_buckets.add(new CengBucket());
	}

	public void deletePoke(Integer pokeKey)
	{
		// TODO: Empty Implementation
	}

	public void addPoke(CengPoke poke)
	{
		// TODO: Empty Implementation
	}
	
	public void searchPoke(Integer pokeKey)
	{
		// TODO: Empty Implementation
	}
	
	public void print()
	{
		// TODO: Empty Implementation
	}
	
	// GUI-Based Methods
	// These methods are required by GUI to work properly.
	
	public int bucketCount()
	{
		// TODO: Return all bucket count.
		return this.bucket_count;		
	}
	
	public static CengBucket bucketAtIndex(int index)
	{
		// TODO: Return corresponding bucket at index.
		return all_buckets.get(index);
	}
	
	// Own Methods
}
