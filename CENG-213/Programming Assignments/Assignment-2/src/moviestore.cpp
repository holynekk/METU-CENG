#include "moviestore.h"


MovieStore::MovieStore( ) //implemented, do not change
{
}


// IMPLEMENT
void
MovieStore::insert(const Movie & movie)
{
  primaryIndex.insert(movie.getID(), movie);

  const Movie *const p1 = &(primaryIndex.find(movie.getID()))->data;
  secondaryIndex.insert({movie.getTitle(), movie.getDirector()}, p1);
  
  Movie *const p2 = &(primaryIndex.find(movie.getID()))->data;
  ternaryIndex.insert({movie.getTitle(), movie.getDirector()}, p2);
  
}


// IMPLEMENT
void
MovieStore::remove(const std::string & id)
{
  std::list<MSTP::Node *> range = primaryIndex.find(id,"{");
  std::list<MSTP::Node *>::const_iterator itr = range.begin();
  
  while(itr != range.end()){
    if((*itr)->data.getID() == id){
      Movie * p = &((*itr)->data);
      std::string title = p->getTitle();
      std::string director = p->getDirector();
      ternaryIndex.remove({title, director});
      secondaryIndex.remove({title, director});
      primaryIndex.remove(id);
      break;
    }
    itr++;
  }
}


// IMPLEMENT
void
MovieStore::remove(const std::string & title,
                  const std::string & director)
{
  MSTT::Node * p = ternaryIndex.find({title, director});
  std::string idd = p->data->getID();
  ternaryIndex.remove({title, director});
  secondaryIndex.remove({title, director});
  primaryIndex.remove(idd);
}


// IMPLEMENT
void
MovieStore::removeAllMoviesWithTitle(const std::string & title)
{
  std::list<MSTT::Node *> range = ternaryIndex.find({title, "a"}, {title, "z"});
  std::list<MSTT::Node *>::const_iterator itr = range.begin();
  
  while(itr != range.end()){
    std::string idd = (*itr)->data->getID();
    const Movie *const p = (*itr)->data;
    ternaryIndex.remove({title, (*itr)->data->getDirector()});
    
    secondaryIndex.remove({title, p->getDirector()});
    primaryIndex.remove(idd);
    itr++;
  }
  
}


// IMPLEMENT
void
MovieStore::makeAvailable(const std::string & id)
{
  primaryIndex.find(id)->data.setAvailable();
}


// IMPLEMENT
void
MovieStore::makeUnavailable(const std::string & title,
                           const std::string & director)
{
  std::list<MSTT::Node *> range = ternaryIndex.find({title, "a"}, {title, "z"});
  std::list<MSTT::Node *>::const_iterator itr = range.begin();
  DirectorComparator bubu;
  while(itr != range.end()){
    if(!bubu({(*itr)->data->getTitle(), (*itr)->data->getDirector()},{title, director}) && !bubu({title, director}, {(*itr)->data->getTitle(), (*itr)->data->getDirector()})){
      (*itr)->data->setUnavailable();
    }
    itr++;
  }
}


// IMPLEMENT
void
MovieStore::updateCompany(const std::string & director, 
                           const std::string & Company)
{
  std::list<MSTT::Node *> range = ternaryIndex.find({"a", director}, {"zzzzzzzz", director});
  std::list<MSTT::Node *>::const_iterator itr = range.begin();
  DirectorComparator nn;

  while(itr != range.end()){
    if(!nn( {"{", (*itr)->data->getDirector()}, {"{", director} ) && !nn( {"{", director}, {"{", (*itr)->data->getDirector()} )){
      (*itr)->data->setCompany(Company);
    }
    itr++;
  }
}


// IMPLEMENT
void
MovieStore::printMoviesWithID(const std::string & id1,
                              const std::string & id2,
                              unsigned short since) const
{
  std::list<MSTP::Node *> range = primaryIndex.find(id1, id2);
  std::list<MSTP::Node *>::const_iterator itr = range.begin();

  std::list<MSTP::Node *> nn;
  while(itr != range.end()){
    if((*itr)->data.getYear() >= since){
      nn.push_back(*itr);
    }
    itr++;
  }
  itr = nn.begin();
  
  MovieStore::MSTP ntre;

  while(itr != nn.end()){
    ntre.insert((*itr)->data.getID(), (*itr)->data);
    itr++;
  }
  printPrimarySorted(ntre.getRoot());
  ntre.empty();
  
}


// IMPLEMENT
void
MovieStore::printMoviesOfDirector(const std::string & director,
                                  const std::string & first,
                                  const std::string & last) const
{
  std::list<MSTT::Node *> range = ternaryIndex.find({first, "a"}, {last, "z"});
  std::list<MSTT::Node *>::const_iterator itr = range.begin();
  std::list<MSTT::Node *> nn;

  while(itr != range.end()){
    std::string str = (*itr)->data->getDirector();
    int str1 = str.size();
    int str2 = director.size();

    if(str1 == str2 && std::tolower(str[0]) == std::tolower(director[0]) && std::tolower(str.back()) == std::tolower(director.back())){
      nn.push_back((*itr));
    }
    itr++;
  }

  MovieStore::MSTS ntre;
  itr = nn.begin();
  
  while(itr != nn.end()){
    ntre.insert({(*itr)->data->getDirector(), (*itr)->data->getTitle()}, (*itr)->data);
    itr++;
  }
  printSecondarySorted(ntre.getRoot());
  ntre.empty();
}



void  // implemented, do not change
MovieStore::printPrimarySorted( ) const
{
  printPrimarySorted(primaryIndex.getRoot());
}


void  // implemented, do not change
MovieStore::printPrimarySorted(MSTP::Node * t) const
{
  if (t == NULL)
  {
    return;
  }

  printPrimarySorted(t->left);
  std::cout << t->data << std::endl;
  printPrimarySorted(t->right);
}


void  // implemented, do not change
MovieStore::printSecondarySorted( ) const
{
  printSecondarySorted(secondaryIndex.getRoot());
}


void  // implemented, do not change
MovieStore::printSecondarySorted(MSTS::Node * t) const
{
  if (t == NULL)
  {
    return;
  }

  printSecondarySorted(t->left);
  std::cout << *(t->data) << std::endl;
  printSecondarySorted(t->right);
}


void  // implemented, do not change
MovieStore::printTernarySorted( ) const
{
  printTernarySorted(ternaryIndex.getRoot());
}


void  // implemented, do not change
MovieStore::printTernarySorted(MSTT::Node * t) const
{
  if (t == NULL)
  {
    return;
  }

  printTernarySorted(t->left);
  std::cout << *(t->data) << std::endl;
  printTernarySorted(t->right);
}

