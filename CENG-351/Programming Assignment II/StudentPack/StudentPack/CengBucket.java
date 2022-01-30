import java.util.ArrayList;

public class CengBucket {

	// GUI-Based Methods
	// These methods are required by GUI to work properly.

	private ArrayList<CengPoke> poke_list;
	private Integer local_depth;

	public CengBucket(int local) {
		this.poke_list = new ArrayList<CengPoke>();
		this.local_depth = local;
	}
	
	public int pokeCount()
	{
		// TODO: Return the pokemon count in the bucket.
		return poke_list.size();		
	}
	
	public CengPoke pokeAtIndex(int index)
	{
		// TODO: Return the corresponding pokemon at the index.
		return poke_list.get(index);
	}
	
	public int getHashPrefix()
	{
		// TODO: Return hash prefix length.
		return this.local_depth;
	}
	
	public Boolean isVisited()
	{
		// TODO: Return whether the bucket is found while searching.
		return false;		
	}
	
	// Own Methods

	public void add_poke(CengPoke poke) {
		this.poke_list.add(poke);
	}

	public void inc_local_depth() {
		this.local_depth++;
	}

	public void remove_all_pokes() {
		poke_list.clear();
	}
}
