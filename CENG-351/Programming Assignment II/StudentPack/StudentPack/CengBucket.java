import java.util.ArrayList;

public class CengBucket {

	// GUI-Based Methods
	// These methods are required by GUI to work properly.

	private ArrayList<CengPoke> poke_list;

	public CengBucket() {
		poke_list = new ArrayList<CengPoke>();
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
		return poke_list.size();
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
}
