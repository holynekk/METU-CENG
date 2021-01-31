#ifndef POST_H
#define POST_H


#include <iostream>

// DO NOT CHANGE THIS FILE.

class Post {
public:
    Post();
    Post(const std::string &message);

    const std::string &getMessage() const;
    int getPostId() const;

    void setMessage(const std::string &message);
    void setPostId(int postId);

    friend std::ostream &operator<<(std::ostream &os, const Post &post);

    bool operator==(const Post &rhs) const;
    bool operator!=(const Post &rhs) const;

    bool operator<(const Post &rhs) const;
    bool operator>(const Post &rhs) const;
    bool operator<=(const Post &rhs) const;
    bool operator>=(const Post &rhs) const;

private:
    std::string message;
    int postId;
};


#endif //POST_H
