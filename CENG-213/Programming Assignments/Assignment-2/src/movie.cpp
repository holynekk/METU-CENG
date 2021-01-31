#include "movie.h"


//do not change this file, everything is implemented


Movie::Movie(const std::string & _id, const std::string & _title,
           const std::string & _director, const std::string & _company,
           unsigned short _year, unsigned short _duration, bool _status)
  : id(_id), title(_title), director(_director)
{
  company = _company;
  year = _year;
  duration = _duration;
  status = _status;
}

const Movie &
Movie::operator=(const Movie & rhs)
{
  company = rhs.company;
  year = rhs.year;
  duration = rhs.duration;
  status = rhs.status;

  return *this;
}

const std::string & 
Movie::getID( ) const
{
  return id;
}

const std::string & 
Movie::getTitle( ) const
{
  return title;
}

const std::string & 
Movie::getDirector( ) const
{
  return director;
}
     
unsigned short
Movie::getYear( ) const
{
  return year;
}

void 
Movie::setCompany(const std::string & _company)
{
  company = _company;
}

void 
Movie::setYear(unsigned short _year)
{
  year = _year;
}

void 
Movie::setDuration(unsigned short _duration)
{
  duration = _duration;
}

void 
Movie::setAvailable( )
{
  status = true;
}

void
Movie::setUnavailable( )
{
  status = false;
}

std::ostream &
operator<<(std::ostream & out, const Movie & movie)
{
  std::string sep(" | ");

  out << movie.id << sep << movie.title << sep
      << movie.director << sep << movie.company << sep
      << movie.year << sep << movie.duration << sep
      << (movie.status ? 'Y' : 'N');

  return out;
}

Movie::SecondaryKey::SecondaryKey(const Movie & movie)
  : title(movie.title), director(movie.director)
{
}

Movie::SecondaryKey::SecondaryKey(const std::string & _title,
                                  const std::string & _director)
  : title(_title), director(_director)
{
}

const std::string &
Movie::SecondaryKey::getTitle( ) const
{
  return title;
}

const std::string &
Movie::SecondaryKey::getDirector( ) const
{
  return director;
}
