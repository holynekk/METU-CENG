#include "bst.h"
#include <iostream>


// Output is in main_tree.out


class BalanceCondition
{
  public:
    bool operator() (int current_height, int ideal_height) const
    {
      if (current_height > 2 * ideal_height)
        return false;
      return true;
    }
};

class DelicateBalance
{
  public:
    bool operator() (int current_height, int ideal_height) const
    {
      return false;
    }
};


class AdditionalBalance
{
  public:
    bool operator() (int current_height, int ideal_height) const
    {
      if (current_height > ideal_height)
      {
         return false;
      }
      return true;
    }
};


int main(void)
{
  
  BinarySearchTree<int, float> empty_bst;

  empty_bst.print(std::cout);

  std::list<std::pair<int, float> > datalist;

  for (int i = 0; i < 7; ++i)
  {
    datalist.push_back(std::make_pair(i+1, i+1));
  }
  
  std::list<std::pair<int, float> >::const_iterator itr = datalist.begin();

  std::cout << "Populating sorted data list contains these key-object values:" << std::endl;
  for ( ; itr != datalist.end(); ++itr)
  {
    std::cout << itr->first << " " << itr->second << std::endl;
  }
  std::cout << std::endl;
  
  BinarySearchTree<int, float, DelicateBalance> list_bst(datalist);

  std::cout << "Constructed tree is as follows for DelicateBalance condition: " << std::endl;
  list_bst.print(std::cout);
  std::cout << std::endl;
  
  std::cout << "Inserting 9" << std::endl;
  list_bst.insert(9, 9);
  list_bst.print(std::cout);
  std::cout << std::endl;

  std::cout << "Inserting 8" << std::endl;
  list_bst.insert(8, 8);
  list_bst.print(std::cout);
  std::cout << std::endl;

  std::cout << "Inserting 0" << std::endl;
  list_bst.insert(0, 0);
  list_bst.print(std::cout);
  std::cout << std::endl;

  std::cout << "Inserting 11" << std::endl;
  list_bst.insert(11, 11);
  list_bst.print(std::cout);
  std::cout << std::endl;

  std::cout << "Inserting 10" << std::endl;
  list_bst.insert(10, 10);
  list_bst.print(std::cout);
  std::cout << std::endl;
  
  std::cout << "Removing 15" << std::endl;
  list_bst.remove(15);
  list_bst.print(std::cout);
  std::cout << std::endl;

  std::cout << "Removing 9" << std::endl;
  list_bst.remove(9);
  list_bst.print(std::cout);
  std::cout << std::endl;
  
  std::cout << "Removing 6" << std::endl;
  list_bst.remove(6);
  list_bst.print(std::cout);
  std::cout << std::endl;

  std::cout << "Removing 2" << std::endl;
  list_bst.remove(2);
  list_bst.print(std::cout);
  std::cout << std::endl;

  std::cout << "Removing 3" << std::endl;
  list_bst.remove(3);
  list_bst.print(std::cout);
  std::cout << std::endl;

  std::cout << "Removing 11" << std::endl;
  list_bst.remove(11);
  list_bst.print(std::cout);
  std::cout << std::endl;

  std::cout << "Removing 0" << std::endl;
  list_bst.remove(0);
  list_bst.print(std::cout);
  std::cout << std::endl;

  std::cout << "Removing 1" << std::endl;
  list_bst.remove(1);
  list_bst.print(std::cout);
  std::cout << std::endl;

  std::cout << std::endl;

  BinarySearchTree<int, int, BalanceCondition> linear_bst;
  std::cout << "An empty BST with BalanceCondition is constructed" << std::endl;

  for (int i = 0; i < 4; ++i)
  {
    std::cout << "Inserting " << (i+1) << std::endl;
    linear_bst.insert(i+1, i+1);
    linear_bst.print(std::cout);
    std::cout << std::endl;
  }  

  for (int i = 0; i < 4; ++i)
  {
    std::cout << "Inserting " << (-i-1) << std::endl;
    linear_bst.insert(-i-1, -i-1);
    linear_bst.print(std::cout);
    std::cout << std::endl;

    std::cout << "Inserting " << 40*(i+1) << std::endl;
    linear_bst.insert(40*(i+1), 40*(i+1));
    linear_bst.print(std::cout);
    std::cout << std::endl;
  }  

  std::cout << "Removing 6" << std::endl;
  linear_bst.remove(6);
  linear_bst.print(std::cout);
  std::cout << std::endl;

  std::cout << "Search elements within [6, 8]..." << std::endl;
  std::list<BinarySearchTree<int, int, BalanceCondition>::Node *> results = linear_bst.find(6, 8);

  std::cout << "Results: " <<std::endl;
  for (std::list<BinarySearchTree<int, int, BalanceCondition>::Node *>::const_iterator itr = results.begin();
      itr != results.end(); ++itr)
  {
    std::cout << (*itr)->key << " " << (*itr)->data << std::endl;
  }
  std::cout << std::endl;
  
  std::cout << "Search elements within [-2, 40]..." << std::endl;
  results = linear_bst.find(-2, 40);
  
  for (std::list<BinarySearchTree<int, int, BalanceCondition>::Node *>::const_iterator itr = results.begin();
      itr != results.end(); ++itr)
  {
    std::cout << (*itr)->key << " " << (*itr)->data << std::endl;
  }
  

  return 0;
}
