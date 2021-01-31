#ifndef _moviestore_h__
#define _moviestore_h__

#include <string>
#include <iostream>

#include "bst.h"
#include "movie.h"
#include "director_comparator.h"
#include "title_comparator.h"


// balance condition used in MovieStore
class MovieBalanceCondition
{
  public:
    bool operator() (int current_height, int ideal_height) const
    {
      if (current_height >= 2 * ideal_height + 1)
      {
        return false;
      }

      return true;
    }
};


class MovieStore  // do not change this file, you cannot add any other functions here
{
  public:
    MovieStore();
    void insert(const Movie &);
    void remove(const std::string &);
    void remove(const std::string &, const std::string &);
    void removeAllMoviesWithTitle(const std::string &);
    void makeAvailable(const std::string &);
    void makeUnavailable(const std::string &, const std::string &);
    void updateCompany(const std::string &, const std::string &);

  public:
    void printPrimarySorted( ) const;
    void printSecondarySorted( ) const;
    void printTernarySorted( ) const;

  public:
    void printMoviesWithID(const std::string &,
                           const std::string &,
                           unsigned short =0) const;
    void printMoviesOfDirector(const std::string &,
                               const std::string & ="a",
                               const std::string & ="{") const;
  public:
    void printTrees( ) const
    {
      primaryIndex.print(std::cout);
      secondaryIndex.print(std::cout);
      ternaryIndex.print(std::cout);
    }

  protected:
    typedef Movie::SecondaryKey SKey;
    typedef BinarySearchTree<std::string, Movie, MovieBalanceCondition> MSTP;
    typedef BinarySearchTree<SKey, const Movie *, 
                             MovieBalanceCondition, DirectorComparator> MSTS;
    typedef BinarySearchTree<SKey, Movie *,
                             MovieBalanceCondition, TitleComparator> MSTT;

  protected:
    void printPrimarySorted(MSTP::Node *) const;
    void printSecondarySorted(MSTS::Node *) const;
    void printTernarySorted(MSTT::Node *) const;

  protected:
    MSTP primaryIndex;
    MSTS secondaryIndex;
    MSTT ternaryIndex;
};


#endif
