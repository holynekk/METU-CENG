import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Scanner;

public class CengPokeParser {

	public static ArrayList<CengPoke> parsePokeFile(String filename)
	{
		ArrayList<CengPoke> pokeList = new ArrayList<CengPoke>();

		// You need to parse the input file in order to use GUI tables.
		// TODO: Parse the input file, and convert them into CengPokes
		

		try {
			FileReader input = new FileReader(filename);
			BufferedReader bufRead = new BufferedReader(input);
			String myLine = null;

			while ( (myLine = bufRead.readLine()) != null) {    
				String[] array1 = myLine.split("\\t");
				CengPoke newPoke = new CengPoke(Integer.parseInt(array1[1]), array1[2], array1[3], array1[4]);
				pokeList.add(newPoke);
			}

		} catch (IOException ex) {
			// There will be no exception bro..
		}


		return pokeList;
	}
	
	public static void startParsingCommandLine() throws IOException
	{
		// TODO: Start listening and parsing command line -System.in-.
		// There are 5 commands:
		// 1) quit : End the app. Print nothing, call nothing.
		// 2) add : Parse and create the poke, and call CengPokeKeeper.addPoke(newlyCreatedPoke).
		// 3) search : Parse the pokeKey, and call CengPokeKeeper.searchPoke(parsedKey).
		// 4) delete: Parse the pokeKey, and call CengPokeKeeper.removePoke(parsedKey).
		// 5) print : Print the whole hash table with the corresponding buckets, call CengPokeKeeper.printEverything().

		Scanner in = new Scanner(System.in);
		String myLine = null;
		
		while ((myLine = in.nextLine()) != null) {
			if (myLine.startsWith("add") || myLine.startsWith("Add")|| myLine.startsWith("aDd")|| myLine.startsWith("adD")|| myLine.startsWith("ADd")|| myLine.startsWith("aDD")|| myLine.startsWith("ADD")) {
				String[] array1 = myLine.split("\\t");
				CengPoke newPoke = new CengPoke(Integer.parseInt(array1[1]), array1[2], array1[3], array1[4]);
				CengPokeKeeper.addPoke(newPoke);
			} else if (myLine.startsWith("delete")) {
				String[] array1 = myLine.split("\\t");

			} else if (myLine.startsWith("search")) {
				String[] array1 = myLine.split("\\t");

			} else if (myLine.startsWith("print")) {
				
			} else if (myLine.startsWith("quit")) {
				return;
			} else;
			
		}
		return;
		// Commands (quit, add, search, print) are case-insensitive.
	}
}
