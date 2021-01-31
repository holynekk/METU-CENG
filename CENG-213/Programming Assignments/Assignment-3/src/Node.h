#ifndef NODE_H
#define NODE_H

#include <string>
/* Do not add new libraries or files */

class Node {

public:
    // Constructors
    Node() : country(""), continent("") {}

    Node(int vid_, const std::string &country_, const std::string &continent_, long gdp_) :
        vid(vid_), country(country_), continent(continent_), gdp(gdp_) {}

    // Getters & setters
    int getVid() const {
        return vid;
    }

    void setVid(int vid) {
        Node::vid = vid;
    }

    const std::string &getCountry() const {
        return country;
    }

    void setCountry(const std::string &country_) {
        country = country_;
    }

    const std::string &getContinent() const {
        return continent;
    }

    void setContinent(const std::string &continent_) {
        continent = continent_;
    }

    long getGdp() const {
        return gdp;
    }

    void setGdp(long gdp_) {
        gdp = gdp_;
    }


private:
    int vid; // country id
    std::string country; // country name
    std::string continent; // continent of the country
    long gdp; // gross domestic product per capita
};

#endif //NODE_H
