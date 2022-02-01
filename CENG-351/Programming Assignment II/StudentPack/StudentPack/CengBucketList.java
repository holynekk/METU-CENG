import java.util.ArrayList;

public class CengBucketList {

	private int bucket_count;
	private static ArrayList<CengBucket> all_buckets;

	public CengBucketList()
	{
		// TODO: Empty Implementation
		this.all_buckets = new ArrayList<CengBucket>();
		this.all_buckets.add(new CengBucket(0));
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
		return all_buckets.size();		
	}
	
	public static CengBucket bucketAtIndex(int index)
	{
		// TODO: Return corresponding bucket at index.
		return all_buckets.get(index);
	}
	
	// Own Methods

	public static Integer find_bucket(CengBucket need_find) {
		return all_buckets.indexOf(need_find);
	}

	public static void add_bucket(CengBucket new_bucket, int index) {
		all_buckets.add(index, new_bucket);
	}

	public static Integer get_num_of_empty() {
		Integer result = 0;
		for(int i = 0; i < all_buckets.size(); i++) {
			if (all_buckets.get(i).pokeCount() == 0) result++;
		}
		return result;
	}
}
