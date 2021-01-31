#include "moviestore.h"


//Expected output is in "main_moviestore.out"

void printAllMoviesSorted(const MovieStore & store)
{
  std::cout << "All movies (id sorted):" << std::endl;
  store.printPrimarySorted();
  std::cout << "---------------------------------" << std::endl;

  std::cout << "All movies (director-title sorted):" << std::endl;
  store.printSecondarySorted();
  std::cout << "---------------------------------" << std::endl;

  std::cout << "All movies (title-director sorted):" << std::endl;
  store.printTernarySorted();
  std::cout << "---------------------------------" << std::endl;
}


int main( )
{
  MovieStore myStore;

  myStore.insert(Movie("0120737", "The Lord of the Rings: The Fellowship of the Ring", "Peter Jackson",
                      "X Movies", 2001, 150));
  myStore.insert(Movie("1170358", "the hobbit: The Desolation of Smaug", "peter JACKSON",
                      "Y Movies", 2013, 160));
  myStore.insert(Movie("0103873", "Dead Alive", "Peter JACKSON",           
                      "X Movies", 1994, 104));
  myStore.insert(Movie("0360717", "King Kong", "pEteR JackSon",
                      "Y Movies", 2005, 102));
  myStore.insert(Movie("0074751", "King Kong", "John Guillermin",
                      "X Classics", 1976, 136));
  myStore.insert(Movie("7905466", "They Shall Not Grow Old", "Peter Jackson",
                      "X Movies", 2018, 245));
  myStore.insert(Movie("0172495", "Gladiator", "Ridley Scott",
                      "Z Movies", 2000, 136));
  myStore.insert(Movie("0104346", "Gladiator", "Rowdy Herrington",
                      "O\'Reilly Movies", 1992, 104));


  printAllMoviesSorted(myStore);
  
  
  /*
  std::cout << "All movies by PETER JACKSON with titles in [\"c\", \"p\"]" << std::endl;
  myStore.printMoviesOfDirector("PETER JACKSON", "c", "p");
  std::cout << "---------------------------------" << std::endl;

  std::cout << "All movies since 2003:" << std::endl;
  myStore.printMoviesWithID("0", ":", 2003);
  std::cout << "---------------------------------" << std::endl;

  std::cout << "All movies by PETER JACKSON" << std::endl;
  myStore.printMoviesOfDirector("PeteR JACKson");
  std::cout << "---------------------------------" << std::endl;
  
 
  std::cout << "Movies with title \"Gladiator\" are now unavailable" << std::endl;
  myStore.makeUnavailable("Gladiator", "Ridley Scott");
  myStore.makeUnavailable("Gladiator", "Rowdy Herrington");
  printAllMoviesSorted(myStore);

  std::cout << "Rowdy Herrington\'s movie is now available" << std::endl;
  myStore.makeAvailable("0104346");
  printAllMoviesSorted(myStore);
  

  std::cout << "Peter Jackson now works with \"Company X\"" << std::endl;
  myStore.updateCompany("peTER jACksOn", "Company X");
  printAllMoviesSorted(myStore);
 
  std::cout << "Removing movie with id 7905466" << std::endl;
  myStore.remove("7905466");
  printAllMoviesSorted(myStore);
  */

  std::cout << "Removing Peter Jackson's movie \"the hobbit: The Desolation of Smaug\"" << std::endl;
  myStore.remove("tHe hoBBit: The Desolation of smaug", "pETEr jaCKSon");
  printAllMoviesSorted(myStore);
  /*
  std::cout << "Removing all movies with title \"Gladiator\"" << std::endl;
  myStore.removeAllMoviesWithTitle("gladiator");
  printAllMoviesSorted(myStore);
  
  std::cout << "Removing all movies with title \"King Kong\"" << std::endl;
  myStore.removeAllMoviesWithTitle("king kong");
  printAllMoviesSorted(myStore);
  */
  return 0;
}
