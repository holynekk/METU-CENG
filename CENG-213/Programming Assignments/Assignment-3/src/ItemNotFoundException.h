#ifndef ITEMNOTFOUNDEXCEPTION_H
#define ITEMNOTFOUNDEXCEPTION_H

class ItemNotFoundException : public std::exception{
public:
    const char * what() const throw() {
        return "Item Not Found in the Hash Table!";
    }
};


#endif //ITEMNOTFOUNDEXCEPTION_H
