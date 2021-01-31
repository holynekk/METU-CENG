#ifndef SOCIALNETWORK_H
#define SOCIALNETWORK_H


#include "LinkedList.h"
#include "Profile.h"
#include "Post.h"

class SocialNetwork {
public:
    SocialNetwork();

    void addProfile(const std::string &firstname, const std::string &lastname, const std::string &email);

    void addPost(const std::string &message, const std::string &email);

    void deleteProfile(const std::string &email);

    void makeFriends(const std::string &email1, const std::string &email2);

    void likePost(int postId, const std::string &email);

    void unlikePost(int postId, const std::string &email);

    LinkedList<Profile *> getMutualFriends(const std::string &email1, const std::string &email2);

    LinkedList<Post *> getListOfMostRecentPosts(const std::string &email, int k=0);

    void print() const;

private: // YOU MAY ADD YOUR OWN UTILITY MEMBER FUNCTIONS HERE.

private:
    LinkedList<Profile> profiles;
    LinkedList<Post> posts;
};


#endif //SOCIALNETWORK_H
