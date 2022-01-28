import java.lang.Math;
import java.util.ArrayList;

public class CengHashTable {

	private Integer global_depth;
	private ArrayList<CengHashRow> table;

	public CengHashTable()
	{
		// TODO: Create a hash table with only 1 row.
		this.global_depth = 0;
		this.table = new ArrayList<CengHashRow>();
		CengHashRow first_row = new CengHashRow("");
		first_row.setBucketIndex(0);
		table.add(first_row);
	}

	public void deletePoke(Integer pokeKey)
	{
		// TODO: Empty Implementation
	}

	public void addPoke(CengPoke poke)
	{			
		// TODO: Empty Implementation
		// String hash_val = Integer.toBinaryString(poke.pokeKey() % CengPokeKeeper.getHashMod());
		int hash_val = calculate_hash(poke.pokeKey() % CengPokeKeeper.getHashMod());
		CengHashRow proposed = this.table.get(hash_val);
		table.get(hash_val).getBucket().add_poke(poke);
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
	
	public int prefixBitCount()
	{
		// TODO: Return table's hash prefix length.
		return this.global_depth;
	}
	
	public int rowCount()
	{
		// TODO: Return the count of HashRows in table.
		return (int)Math.pow(2, this.global_depth);		
	}
	
	public CengHashRow rowAtIndex(int index)
	{
		// TODO: Return corresponding hashRow at index.
		return table.get(index);
	}
	
	// Own Methods

	public int calculate_hash(int val) {
        int digits = this.global_depth;
        int res = 0;
        int t = 1;
        for (int i = 0; i < digits; i++) {
            res |=  t << i;
        }
        return val & res;
    }
}
