#include "movie.h"
#include "title_comparator.h"
#include <iostream>


// output is in main_movie.out
int main(void)
{
  Movie b1("56465", "Name Of The Movie", "Pancar",
          "BIZ", 1993, 657);

  std::cout << b1 << std::endl;
  
  b1.setCompany("Someone else");

  std::cout << b1 << std::endl;

  b1.setUnavailable();

  std::cout << b1 << std::endl;

  b1.setDuration(5667);
  b1.setYear(232);

  std::cout << b1 << std::endl;

  Movie b2("6546", "Here is a movie", "Enver",
          "T", 3533, 565);

  std::cout << b2 << std::endl;

 
  
  TitleComparator isLessThan;

  
  std::cout << isLessThan(Movie::SecondaryKey(b1), 
                          Movie::SecondaryKey(b2)) << std::endl;
  std::cout << isLessThan(Movie::SecondaryKey(b2), 
                          Movie::SecondaryKey(b1)) << std::endl;

  Movie b3("6546", "name of the movie", "Pano",
          "Next", 5656, 233);

  std::cout << isLessThan(Movie::SecondaryKey(b1), 
                          Movie::SecondaryKey(b3)) << std::endl;
  std::cout << isLessThan(Movie::SecondaryKey(b3), 
                          Movie::SecondaryKey(b1)) << std::endl;

  return 0;
}
