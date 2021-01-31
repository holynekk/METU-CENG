#ifndef _title_h__
#define _title_h__

#include "movie.h"
#include <cstring>


class TitleComparator
{
  public:
    // IMPLEMENT BELOW (inline)
    bool operator( ) (const Movie::SecondaryKey & key1, 
                      const Movie::SecondaryKey & key2) const
    { 
      std::string string1 = key1.getTitle();
      std::string string2 = key2.getTitle();
      int size1 = string1.size();
      int size2 = string2.size();
      int loop = (size1>size2) ? size2 : size1;

      for(int i = 0; i < loop; i++){
        if(std::tolower(string1[i]) > std::tolower(string2[i])){
          return false;
        }else if(std::tolower(string1[i]) < std::tolower(string2[i])){
          return true;
        }
      }
      if(size1 < size2){
        return true;
      }
      if(size1 == size2){
        string1 = key1.getDirector();
        string2 = key2.getDirector();
        size1 = string1.size();
        size2 = string2.size();
        loop = (size1>size2) ? size2 : size1;
        for(int i = 0; i < loop; i++){
          if(std::tolower(string1[i]) > std::tolower(string2[i])){
            return false;
          }if(std::tolower(string1[i]) < std::tolower(string2[i])){
            return true;
          }
        }
        if(size1 < size2) return true;
      }
      return false;
    }
};

#endif
