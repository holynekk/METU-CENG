#ifndef LINKEDLIST_H
#define LINKEDLIST_H


#include <iostream>

#include "Node.h"

template<class T>
class LinkedList {
public: // DO NOT CHANGE THIS PART.
    LinkedList();
    LinkedList(const T arr[], int arrSize);
    LinkedList(const LinkedList<T> &obj);

    ~LinkedList();

    Node<T> *getFirstNode() const;
    Node<T> *getLastNode() const;
    Node<T> *getNode(const T &data) const;

    int getNumberOfNodes() const;
    bool isEmpty() const;
    bool contains(Node<T> *node) const;

    void insertAtTheHead(const T &data);
    void insertAtTheTail(const T &data);
    void insertSorted(const T &data);

    void removeNode(Node<T> *node);
    void removeNode(const T &data);
    void removeAllNodes();

    void print() const;

    T *toArray() const;

    LinkedList<T> &operator=(const LinkedList<T> &rhs);

private: // YOU MAY ADD YOUR OWN UTILITY MEMBER FUNCTIONS HERE.

private: // DO NOT CHANGE THIS PART.
    Node<T> *head;
    Node<T> *tail;
};

template<class T>
LinkedList<T>::LinkedList() {
    /* TODO */
    head = nullptr;
    tail = nullptr;
}

template<class T>
LinkedList<T>::LinkedList(const T arr[], int arrSize) {
    /* TODO */
    head = nullptr;
    tail = nullptr;
    for(int i = 0; i < arrSize; i++){
        insertAtTheTail(arr[i]);
    }
}

template<class T>
LinkedList<T>::LinkedList(const LinkedList<T> &obj) {
    /* TODO */
    head = nullptr;
    tail = nullptr;
    Node<T> * tmp = obj.getFirstNode();
    while(tmp){
        insertAtTheTail(tmp->data);
        tmp = tmp->next;
    }
}

template<class T>
LinkedList<T>::~LinkedList() {
    /* TODO */
    removeAllNodes();
    delete tail;
    delete head;
}

template<class T>
Node<T> *LinkedList<T>::getFirstNode() const {
    /* TODO */
    return head;
}

template<class T>
Node<T> *LinkedList<T>::getLastNode() const {
    /* TODO */
    return tail;
}

template<class T>
Node<T> *LinkedList<T>::getNode(const T &data) const {
    /* TODO */
    Node<T> * tmp = head;
    while(tmp){
        if(tmp->data == data){
            break;
        }else{
            tmp = tmp->next;
        }
    }
    return tmp;
}

template<class T>
int LinkedList<T>::getNumberOfNodes() const {
    /* TODO */
    Node<T> * tmp = head;
    int ans = 0;
    while(tmp){
        ans++;
        tmp = tmp->next;
    }
    return ans;
}

template<class T>
bool LinkedList<T>::isEmpty() const {
    /* TODO */
    return !head;
}

template<class T>
bool LinkedList<T>::contains(Node<T> *node) const {
    /* TODO */
    Node<T> * tmp = head;
    while(tmp){
        if(tmp == node){
            tmp = nullptr;
            delete tmp;
            return true;
        }else{
            tmp = tmp->next;
        }
    }tmp = nullptr;
    delete tmp;
    return false;
}

template<class T>
void LinkedList<T>::insertAtTheHead(const T &data) {
    /* TODO */
    Node<T> * nwnode = new Node<T>;
    nwnode->data = data;
    if(head == nullptr){
        head = nwnode;
        tail = nwnode;
    }else{
        head->prev = nwnode;
        nwnode->next = head;
        head = nwnode;
    }nwnode = nullptr;
    delete nwnode;
}

template<class T>
void LinkedList<T>::insertAtTheTail(const T &data) {
    /* TODO */
    Node<T> * nwnode = new Node<T>();
    nwnode->data = data;
    if(tail == nullptr){
        head = nwnode;
        tail = nwnode;
    }else{
        tail->next = nwnode;
        nwnode->prev = tail;
        tail = nwnode;
    }nwnode = nullptr;
    delete nwnode;
}

template<class T>
void LinkedList<T>::insertSorted(const T &data) {
    /* TODO */
    Node<T> * tmp = head;
    Node<T> * nwnode = new Node<T>();
    nwnode->data = data;
    if(!tmp){
        head = nwnode;
        tail = nwnode;
        tmp = nullptr;
        nwnode = nullptr;
        delete tmp;
        delete nwnode;
        return;
    }
    while(tmp){
        if(tmp->data > data){
            if(tmp == head){
                nwnode->next = head;
                head->prev = nwnode;
                head = nwnode;
                tmp = nullptr;
                nwnode = nullptr;
                delete tmp;
                delete nwnode;
                return;
            }else{
                nwnode->next = tmp;
                nwnode->prev = tmp->prev;
                tmp->prev->next = nwnode;
                tmp->prev = nwnode;
                tmp = nullptr;
                nwnode = nullptr;
                delete tmp;
                delete nwnode;
                return;
            }
        }else{
            tmp = tmp->next;
        }
    }insertAtTheTail(data);
    tmp = nullptr;
    delete tmp;
    delete nwnode;
}

template<class T>
void LinkedList<T>::removeNode(Node<T> *node) {
    /* TODO */
    Node<T> * tmp = head;
    while(tmp){
        if(tmp == node){
            if(tmp->prev && tmp->next){
                tmp->prev->next = tmp->next;
                tmp->next->prev = tmp->prev;
                delete tmp;
                return;
            }else if(tmp->next){
                tmp->next->prev = nullptr;
                head = head->next;
                delete tmp;
                return;
            }else if(tmp->prev){
                tmp->prev->next = nullptr;
                tail = tail->prev;
                delete tmp;
                return;
            }else{
                head = nullptr;
                tail = nullptr;
                delete tmp;
                return;
            }
        }else{
            tmp = tmp->next;
        }
    }
}

template<class T>
void LinkedList<T>::removeNode(const T &data) {
    /* TODO */
    Node<T> * tmp = head;
    while(tmp){
        if(tmp->data == data){
            if(tmp->prev && tmp->next){
                tmp->prev->next = tmp->next;
                tmp->next->prev = tmp->prev;
                delete tmp;
                return;
            }else if(tmp->next){
                tmp->next->prev = nullptr;
                head = head->next;
                delete tmp;
                return;
            }else{
                tmp->prev->next = nullptr;
                tail = tail->prev;
                delete tmp;
                return;
            }
        }else{
            tmp = tmp->next;
        }
    }
}

template<class T>
void LinkedList<T>::removeAllNodes() {
    /* TODO */
    if(!head){
        return;
    }
    while(head){
        Node<T> * tmp = head;
        head = head->next;
        delete tmp;
    }
    head = nullptr;
    tail = nullptr;
}

template<class T>
void LinkedList<T>::print() const {
    std::cout << "Printing the linked list ..." << std::endl;

    if (this->isEmpty()) {
        std::cout << "empty" << std::endl;
        return;
    }

    Node<T> *node = this->head;

    while (node) {
        std::cout << *node << std::endl;
        node = node->next;
    }
}

template<class T>
T *LinkedList<T>::toArray() const {
    /* TODO */
    int num = getNumberOfNodes();
    T * arr = new T[num];
    Node<T> * tmp = head;
    for(int i = 0; i < num; i++){
        arr[i] = tmp->data;
        tmp = tmp->next;
    }return arr;
}

template<class T>
LinkedList<T> &LinkedList<T>::operator=(const LinkedList<T> &rhs) {
    /* TODO */
    if (this != &rhs){
        removeAllNodes();
        const Node<T> *m = rhs.getFirstNode();

        while(m){
            insertAtTheTail(m->data);
            m = m->next;
        }
    }return *this;
}


#endif //LINKEDLIST_H
