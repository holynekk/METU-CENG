import java.lang.Math;
import java.util.ArrayList;

public class CengHashTable {

	private Integer global_depth;
	private ArrayList<CengHashRow> table;
	private Integer global_tab;

	public CengHashTable()
	{
		// TODO: Create a hash table with only 1 row.
		this.global_depth = 0;
		this.global_tab = 0;
		this.table = new ArrayList<CengHashRow>();
		CengHashRow first_row = new CengHashRow("0");
		first_row.setBucketIndex(0);
		table.add(first_row);
	}

	public void deletePoke(Integer pokeKey)
	{
		// TODO: Empty Implementation
		int hash_val = calculate_hash(pokeKey % CengPokeKeeper.getHashMod(), this.global_depth);
		CengBucket delete_from = table.get(hash_val).getBucket();
		delete_from.delete_poke(pokeKey);

		System.out.print("\"delete\": {\n");
		System.out.print("\temptyBucketNum: " + CengBucketList.get_num_of_empty() + '\n');
		System.out.print("}\n");
	}

	public void addPoke(CengPoke poke)
	{			
		// TODO: Empty Implementation
		int hash_val = calculate_hash(poke.pokeKey() % CengPokeKeeper.getHashMod(), this.global_depth);
		CengBucket sugg_bucket = table.get(hash_val).getBucket();
		if (sugg_bucket.pokeCount() < CengPokeKeeper.getBucketSize()) {
			// System.out.print("ahash");
			sugg_bucket.add_poke(poke);
		} else {
			// 1 - increment local depth
			sugg_bucket.inc_local_depth();

			// 2 - allocate new page with local depth
			CengBucket new_bucket = new CengBucket(sugg_bucket.getHashPrefix());

			// 3 - re-distribute records of original page
			// Store all to redistribute ----------------------------------
			ArrayList<CengPoke> to_be_added = new ArrayList<CengPoke>();
			for(int k = 0; k < sugg_bucket.pokeCount(); k++) {
				to_be_added.add(sugg_bucket.pokeAtIndex(k));
			}
			to_be_added.add(poke);
			// ------------------------------------------------------------

			sugg_bucket.remove_all_pokes();

			for(int k = 0; k < to_be_added.size(); k++) {
				int ex_hash = calculate_hash(to_be_added.get(k).pokeKey() % CengPokeKeeper.getHashMod(), sugg_bucket.getHashPrefix());
				String binary_string = Integer.toBinaryString(ex_hash);
				if (binary_string.charAt(binary_string.length() - 1) == '0') {
					sugg_bucket.add_poke(to_be_added.get(k));
				} else if (binary_string.charAt(binary_string.length() - 1) == '1') {
					new_bucket.add_poke(to_be_added.get(k));
				}
			}

			// 4 - add entry for the new page to the directory
			CengBucketList.add_bucket(new_bucket, CengBucketList.find_bucket(sugg_bucket) + 1);


			// 5 - double the directory if necessary
			if (sugg_bucket.getHashPrefix() > global_depth) {
				if (global_depth == 0) {
					table.get(0).setHashPrefix("");
				} else;


				for(int i = 0; i < rowCount()*2; i+=2) {
					CengHashRow temp = new CengHashRow(table.get(i).hashPrefix() + "1");
					table.get(i).setHashPrefix(table.get(i).hashPrefix() + "0");
					table.add(i+1, temp);
					table.get(i+1).setBucketIndex(table.get(i).getBucketIndex());
				}
				this.global_depth++;
			} else;

			hash_val = calculate_hash(poke.pokeKey() % CengPokeKeeper.getHashMod(), this.global_depth);
			String xx = Integer.toBinaryString(hash_val);
			if (xx.charAt(xx.length() - 1) == '0') {
				table.get(hash_val).setBucketIndex(CengBucketList.find_bucket(sugg_bucket));
			} else {
				table.get(hash_val).setBucketIndex(CengBucketList.find_bucket(sugg_bucket) + 1);
			}
			for(int k = hash_val + 1; k < (int)Math.pow(2, this.global_depth); k++) {
				table.get(k).setBucketIndex(table.get(k).getBucketIndex() + 1);
			}
		}
	}
	
	public void searchPoke(Integer pokeKey)
	{
		// TODO: Empty Implementation
		int hash_val = calculate_hash(pokeKey % CengPokeKeeper.getHashMod(), this.global_depth);
		System.out.print("\"search\": {\n");
		System.out.print("\t\"row\": {\n");
		System.out.print("\t\"hashPref\": {\n");
		System.out.print("\t}\n");
		System.out.print("}\n");
	}
	
	public void print()
	{
		// TODO: Empty Implementation
		System.out.print("\"table\": {\n");
		for(int i = 0; i < rowCount(); i++) {
			System.out.print("\t\"row\": {\n");
			System.out.print("\t\t\"hashPref\": " + table.get(i).hashPrefix() + ",\n");
			System.out.print("\t\t\"bucket\": {\n");
			System.out.print("\t\t\t\"hashLength\": " + table.get(i).getBucket().getHashPrefix() + ",\n");
			System.out.print("\t\t\t\"pokes\": [\n");
			for(int j = 0; j < table.get(i).getBucket().pokeCount(); j++) {
				System.out.print("\t\t\t\t\"poke\": {\n");
				System.out.print("\t\t\t\t\t\"hash\": " + string_hash(table.get(i).getBucket().pokeAtIndex(j).pokeKey()) + ",\n");
				System.out.print("\t\t\t\t\t\"pokeKey\": " + table.get(i).getBucket().pokeAtIndex(j).pokeKey() + ",\n");
				System.out.print("\t\t\t\t\t\"pokeName\": " + table.get(i).getBucket().pokeAtIndex(j).pokeName() + ",\n");
				System.out.print("\t\t\t\t\t\"pokePower\": " + table.get(i).getBucket().pokeAtIndex(j).pokePower() + ",\n");
				System.out.print("\t\t\t\t\t\"pokeType\": " + table.get(i).getBucket().pokeAtIndex(j).pokeType() + "\n");
				System.out.print("\t\t\t\t}\n");
			}
			System.out.print("\t\t\t]\n");
			System.out.print("\t\t}\n");
			System.out.print("\t}\n");
		}
		System.out.print("}\n");
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

	public int calculate_hash(int val, int depth) {
		if (depth == 0) return 0;
		String hash = Integer.toBinaryString(val);
		int ex_bit_size = Integer.toBinaryString(CengPokeKeeper.getHashMod()).length()-1;
		while (true) {
			if (hash.length() == ex_bit_size) {
				break;
			} else {
				hash = "0" + hash;
			}
		}
        String result_hash = hash.substring(0, depth);
        return Integer.parseInt(result_hash, 2);
    }

	public String string_hash(int poke_key) {
		String converted = Integer.toBinaryString(poke_key % CengPokeKeeper.getHashMod());
		int expected_hash_length = (int)(Math.log(CengPokeKeeper.getHashMod())/Math.log(2));
		while (true) {
			if (converted.length() == expected_hash_length) {
				break;
			} else {
				converted = "0" + converted;
			}
		}
		return converted;
	}
}
