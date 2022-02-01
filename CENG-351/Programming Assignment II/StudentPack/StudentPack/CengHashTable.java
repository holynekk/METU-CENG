import java.lang.Math;
import java.util.ArrayList;

public class CengHashTable {

	private Integer global_depth;
	private ArrayList<CengHashRow> table;
	private Integer global_tab;

	public CengHashTable()
	{
		this.global_depth = 0;
		this.global_tab = 0;
		this.table = new ArrayList<CengHashRow>();
		CengHashRow first_row = new CengHashRow("0");
		first_row.setBucketIndex(0);
		table.add(first_row);
	}

	public void deletePoke(Integer pokeKey)
	{
		int hash_val = calculate_hash(pokeKey % CengPokeKeeper.getHashMod(), this.global_depth);
		CengBucket delete_from = table.get(hash_val).getBucket();
		delete_from.delete_poke(pokeKey);
		// Printing deletion info
		System.out.print("\"delete\": {\n");
		System.out.print("\temptyBucketNum: " + CengBucketList.get_num_of_empty() + '\n');
		System.out.print("}\n");
	}

	public void addPoke(CengPoke poke)
	{			
		// Calculate hash value for corresponding poke
		int hash_val = calculate_hash(poke.pokeKey() % CengPokeKeeper.getHashMod(), this.global_depth);
		// Find its suggeted bucket inside hashtable
		CengBucket sugg_bucket = table.get(hash_val).getBucket();
		// if it's not full -> add it directly | else -> split && maybe double hash table
		if (sugg_bucket.pokeCount() < CengPokeKeeper.getBucketSize()) {
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
			// Empty bucket to fill it later
			sugg_bucket.remove_all_pokes();
			// Fill splitted buckets
			for(int k = 0; k < to_be_added.size() - 1; k++) {
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
				// Doubling HashRows
				for(int i = 0; i < rowCount()*2; i+=2) {
					CengHashRow temp = new CengHashRow(table.get(i).hashPrefix() + "1");
					table.get(i).setHashPrefix(table.get(i).hashPrefix() + "0");
					table.add(i+1, temp);
					table.get(i+1).setBucketIndex(table.get(i).getBucketIndex());
				}
				this.global_depth++;
			} else;

			// Setting arrows between HashRows and Buckets
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
			// Add the poke recursively. (Addition is happening recursively here. That's because we don't
			// know if suggested bucket will be full again even after doubling HashRows)
			addPoke(poke);
		}
	}
	
	public void searchPoke(Integer pokeKey)
	{
		int hash_val = calculate_hash(pokeKey % CengPokeKeeper.getHashMod(), this.global_depth);
		CengBucket found_bucket = table.get(hash_val).getBucket();
		boolean flag = false, for_row = true;
		for(int i = 0; i < found_bucket.pokeCount(); i++) {
			if (found_bucket.pokeAtIndex(i).pokeKey() == pokeKey) {
				flag = true;
				break;
			} else;
		}
		// Printing search info
		System.out.print("\"search\": {\n");
		if (flag) {
			for(int i = 0; i < rowCount(); i++) {
				CengBucket tmp = table.get(i).getBucket();
				if (tmp == found_bucket) {
					if (for_row) {
						for_row = false;
					} else {
						System.out.print("\t},\n");
					}
					System.out.print("\t\"row\": {\n");
					System.out.print("\t\t\"hashPref\": " + table.get(i).hashPrefix() + ",\n");
					System.out.print("\t\t\"bucket\": {\n");
					System.out.print("\t\t\t\"hashLength\": " + tmp.getHashPrefix() + ",\n");
					System.out.print("\t\t\t\"pokes\": [\n");
					for(int k = 0; k < tmp.pokeCount(); k++) {
						if (tmp.pokeAtIndex(k).pokeKey() == pokeKey) {
							System.out.print("\t\t\t\t\"poke\": {\n");
							System.out.print("\t\t\t\t\t\"hash\": " + string_hash(pokeKey) + ",\n");
							System.out.print("\t\t\t\t\t\"pokeKey\": " + pokeKey + ",\n");
							System.out.print("\t\t\t\t\t\"pokeName\": " + tmp.pokeAtIndex(k).pokeName() + ",\n");
							System.out.print("\t\t\t\t\t\"pokePower\": " + tmp.pokeAtIndex(k).pokePower() + ",\n");
							System.out.print("\t\t\t\t\t\"pokeType\": " + tmp.pokeAtIndex(k).pokeType() + "\n");
							System.out.print("\t\t\t\t}\n");
						} else;
					}
					System.out.print("\t\t\t]\n");
					System.out.print("\t\t}\n");
					// System.out.print("\t}\n");
				} else;
			}
			System.out.print("\t}\n");
		} else;
		System.out.print("}\n");
	}
	
	public void print()
	{
		boolean first = true, second = true;
		System.out.print("\"table\": {\n");
		for(int i = 0; i < rowCount(); i++) {
			if (first) {
				first = false;
			} else {
				System.out.print("\t},\n");
			}
			System.out.print("\t\"row\": {\n");
			System.out.print("\t\t\"hashPref\": " + table.get(i).hashPrefix() + ",\n");
			System.out.print("\t\t\"bucket\": {\n");
			System.out.print("\t\t\t\"hashLength\": " + table.get(i).getBucket().getHashPrefix() + ",\n");
			System.out.print("\t\t\t\"pokes\": [\n");
			for(int j = 0; j < table.get(i).getBucket().pokeCount(); j++) {
				if (second) {
					second = false;
				} else {
					System.out.print("\t\t\t\t},\n");
				}
				System.out.print("\t\t\t\t\"poke\": {\n");
				System.out.print("\t\t\t\t\t\"hash\": " + string_hash(table.get(i).getBucket().pokeAtIndex(j).pokeKey()) + ",\n");
				System.out.print("\t\t\t\t\t\"pokeKey\": " + table.get(i).getBucket().pokeAtIndex(j).pokeKey() + ",\n");
				System.out.print("\t\t\t\t\t\"pokeName\": " + table.get(i).getBucket().pokeAtIndex(j).pokeName() + ",\n");
				System.out.print("\t\t\t\t\t\"pokePower\": " + table.get(i).getBucket().pokeAtIndex(j).pokePower() + ",\n");
				System.out.print("\t\t\t\t\t\"pokeType\": " + table.get(i).getBucket().pokeAtIndex(j).pokeType() + "\n");
				
			}
			if (table.get(i).getBucket().pokeCount() > 0) System.out.print("\t\t\t\t}\n");
			System.out.print("\t\t\t]\n");
			System.out.print("\t\t}\n");
			second = true;
		}
		System.out.print("\t}\n");
		System.out.print("}\n");
	}

	// GUI-Based Methods
	// These methods are required by GUI to work properly.
	
	public int prefixBitCount()
	{
		return this.global_depth;
	}
	
	public int rowCount()
	{
		return (int)Math.pow(2, this.global_depth);		
	}
	
	public CengHashRow rowAtIndex(int index)
	{
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
