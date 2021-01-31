#ifndef _movie_h__
#define _movie_h__

#include <string>
#include <ostream>


class Movie //do not change this file
{
  public:
    Movie(const std::string &, const std::string &,
         const std::string &, const std::string &,
         unsigned short, unsigned short =0, bool =true);

    const Movie & operator=(const Movie &);

    const std::string & getID( ) const; //main index
    const std::string & getTitle( ) const; //part of second index
    const std::string & getDirector( ) const; //part of second index
    
    unsigned short getYear( ) const;

    void setCompany(const std::string &);
    void setYear(unsigned short);
    void setDuration(unsigned short);

    void setAvailable( );
    void setUnavailable( );

    class SecondaryKey;

    friend std::ostream & operator<<(std::ostream &, const Movie &);

  private:
    const std::string id;
    const std::string title;
    const std::string director;
    std::string company;
    unsigned short year;
    unsigned short duration;
    bool status;
};


class Movie::SecondaryKey
{
  public:
    SecondaryKey(const Movie &);
    SecondaryKey(const std::string &, const std::string &);

    const std::string & getTitle( ) const;
    const std::string & getDirector( ) const;

  private:
    const std::string title;
    const std::string director;

  private: //cannot be reassigned, no implementation provided
    const SecondaryKey & operator=(const SecondaryKey &);

  friend std::ostream & operator<<(std::ostream &, const Movie::SecondaryKey &);
};

#endif