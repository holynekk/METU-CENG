import java.awt.EventQueue;

public class CengPokeKeeper {
	
	private static GUI window;
	private static Boolean guiEnabled;

	private static Integer hashMod;
	private static Integer bucketSize;
	private static String inputFileName;
	
	public static String getInputFileName() 
	{
		return inputFileName;
	}
	public static Integer getHashMod()
	{
		return hashMod;
	}
	public static Integer getBucketSize()
	{
		return bucketSize;
	}

	private static CengHashTable hashTable;
	private static CengBucketList bucketList;
	
	public static void main(String[] args) throws Exception 
	{		
		if(args.length != 4)
		{
			throw new Exception("Usage : java CengPokeKeeper -hashMod- -bucketSize- -inputFileName- -guiEnabled (True or False)- \nExample usage: java CengPokeKeeper 8 2 pokemons.txt True ");
		}
		
		hashMod = Integer.parseInt(args[0]);
		bucketSize = Integer.parseInt(args[1]);
		inputFileName = args[2];
		guiEnabled = Boolean.parseBoolean(args[3]);
		
		if(guiEnabled) 
		{
			setGUI();
		}
				
		bucketList = new CengBucketList();
		hashTable = new CengHashTable();
		
		CengPokeParser.startParsingCommandLine();
	}

	public static CengHashTable getHashTable()
	{
		return CengPokeKeeper.hashTable;
	}
	
	public static CengBucketList getBucketList()
	{
		return CengPokeKeeper.bucketList;
	}

	public static void deletePoke(Integer pokeKey)
	{
		hashTable.deletePoke(pokeKey);
		bucketList.deletePoke(pokeKey);

		updateWindow();
	}

	public static void addPoke(CengPoke poke)
	{
		hashTable.addPoke(poke);
		bucketList.addPoke(poke);
		
		updateWindow();
	}
	
	public static void searchPoke(Integer pokeKey)
	{		
		hashTable.searchPoke(pokeKey);
		bucketList.searchPoke(pokeKey);
		
		updateWindow();
	}
	
	public static void printEverything()
	{		
		hashTable.print();
		bucketList.print();
	}
	
	// GUI Methods

	private static void updateWindow()
	{
		if(guiEnabled)
		{
			if(window == null)
			{
				System.out.println("Err: Window is not initialized yet.");
				
				return;
			}
			
			window.modelNeedsUpdate();
		}
	}
		
	private static void setGUI() {
		EventQueue.invokeLater(new Runnable()
		{
			public void run() 
			{
				try 
				{
					window = new GUI();
					window.show();
				} 
				catch (Exception e) 
				{
					e.printStackTrace();
				}
			}
		});
	}
}
