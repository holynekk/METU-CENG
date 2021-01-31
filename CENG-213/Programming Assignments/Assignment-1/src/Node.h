#ifndef NODE_H
#define NODE_H


// DO NOT CHANGE THIS FILE.

#include <ostream>

template<class T>
class Node {
public:
    T data;
    Node<T> *prev;
    Node<T> *next;

    Node();
    Node(T data, Node<T> *prev, Node<T> *next);

    friend std::ostream &operator<<(std::ostream &os, const Node<T> &obj) {
        os << obj.data;
        return os;
    }
};

template<class T>
Node<T>::Node() {
    this->prev = nullptr;
    this->next = nullptr;
}

template<class T>
Node<T>::Node(T data, Node<T> *prev, Node<T> *next) {
    this->data = data;
    this->prev = prev;
    this->next = next;
}


#endif //NODE_H
