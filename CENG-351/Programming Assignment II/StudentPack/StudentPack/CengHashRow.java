public class CengHashRow {

	// GUI-Based Methods
	// These methods are required by GUI to work properly.

	private String hash_prefix;
	private int bucket_index;

	CengHashRow(String pre) {
		this.hash_prefix = pre;
	}
	
	public String hashPrefix()
	{
		// TODO: Return row's hash prefix (such as 0, 01, 010, ...)
		return hash_prefix;		
	}
	
	public CengBucket getBucket()
	{
		// TODO: Return the bucket that the row points at.
		return CengBucketList.bucketAtIndex(bucket_index);		
	}
	
	public boolean isVisited()
	{
		// TODO: Return whether the row is used while searching.
		return false;		
	}
	
	// Own Methods

	public void setHashPrefix(String new_prefix) {
		this.hash_prefix = new_prefix;	
	}

	public void setBucketIndex(int new_index) {
		this.bucket_index = new_index;
	}

	public int getBucketIndex() {
		return bucket_index;
	}
}
