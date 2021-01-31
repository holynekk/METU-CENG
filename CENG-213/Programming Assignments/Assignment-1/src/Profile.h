#ifndef PROFILE_H
#define PROFILE_H


#include <iostream>

#include "LinkedList.h"
#include "Post.h"

// DO NOT CHANGE THIS FILE.

class Profile {
public:
    Profile();
    Profile(const std::string &firstname, const std::string &lastname, const std::string &email);

    const std::string &getFirstname() const;
    const std::string &getLastname() const;
    const std::string &getEmail() const;

    LinkedList<Profile *> &getFriends();
    LinkedList<Post *> &getLikes();
    LinkedList<Post *> &getPosts();

    void setFirstname(const std::string &firstname);
    void setLastname(const std::string &lastname);
    void setEmail(const std::string &email);

    void addFriend(Profile *profile);
    void addLike(Post *post);
    void addPost(Post *post);

    void dropFriend(Profile *profile);
    void dropLike(Post *post);
    void dropPost(Post *post);

    friend std::ostream &operator<<(std::ostream &os, const Profile &profile);

    bool operator==(const Profile &rhs) const;
    bool operator!=(const Profile &rhs) const;

    bool operator<(const Profile &rhs) const;
    bool operator>(const Profile &rhs) const;
    bool operator<=(const Profile &rhs) const;
    bool operator>=(const Profile &rhs) const;

private:
    std::string firstname;
    std::string lastname;
    std::string email;

    LinkedList<Profile *> friends;
    LinkedList<Post *> likes;
    LinkedList<Post *> posts;
};


#endif //PROFILE_H
