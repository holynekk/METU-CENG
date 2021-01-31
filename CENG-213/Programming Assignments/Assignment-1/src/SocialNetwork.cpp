#include "SocialNetwork.h"

#include <iostream>

SocialNetwork::SocialNetwork() {

}

void SocialNetwork::addProfile(const std::string &firstname, const std::string &lastname, const std::string &email) {
    /* TODO */
    Profile nwp(firstname, lastname, email);
    profiles.insertSorted(nwp);
}

void SocialNetwork::addPost(const std::string &message, const std::string &email) {
    /* TODO */  
    Post * nwp = new Post(message);
    posts.insertAtTheTail(*nwp);
    delete nwp;
    Post * p = &posts.getLastNode()->data;
    Node<Profile> * pt = profiles.getFirstNode();
    while(pt){
        if(pt->data.getEmail() == email){
            pt->data.getPosts().insertAtTheTail(p);
            return;
        }else{
            pt = pt->next;
        }
    }
}

void SocialNetwork::deleteProfile(const std::string &email) {
    /* TODO */
    Node<Profile> * deluser = profiles.getFirstNode();
    while(deluser->data.getEmail() != email){
        deluser = deluser->next;
    }
    Node<Profile> * pt = profiles.getFirstNode();
    while(pt){
        if(pt == deluser){
            pt = pt->next;
            continue;
        }else{
            Node<Profile *> * fr = pt->data.getFriends().getFirstNode();
            while(fr){
                if(fr->data->getEmail() == email){
                    pt->data.getFriends().removeNode(fr);
                    break;
                }else{
                    fr = fr->next;
                }
            }
            Node<Post *> * li = pt->data.getLikes().getFirstNode();
            Node<Post *> * uspo = deluser->data.getPosts().getFirstNode();
            while(uspo){
                while(li){
                    if(li->data->getPostId() == uspo->data->getPostId()){
                        pt->data.getLikes().removeNode(li);
                        break;
                    }else{
                        li = li->next;
                    }
                }
                li = pt->data.getLikes().getFirstNode();
                uspo = uspo->next;
            }
            /*
            Node<Post> * allposts = posts.getFirstNode();
            uspo = deluser->data.getPosts().getFirstNode();
            while(uspo){
                while(allposts){
                    if(uspo->data->getPostId() == allposts->data.getPostId()){
                        Node<Post> * tmp = allposts;
                        allposts = allposts->next;
                        posts.removeNode(tmp);
                        break;
                    }else{
                        allposts = allposts->next;
                    }
                }uspo = uspo->next;
            }*/
        }
        pt = pt->next;
    }
    Node<Post *> * uspo = deluser->data.getPosts().getFirstNode();
    Node<Post> * allposts = posts.getFirstNode();
    while(uspo){
        while(allposts){
            if(uspo->data->getPostId() == allposts->data.getPostId()){
                Node<Post> * tmp = allposts;
                allposts = allposts->next;
                posts.removeNode(tmp);
                break;
            }else{
                allposts = allposts->next;
            }
        }uspo = uspo->next;
    }
    profiles.removeNode(deluser); 
}

void SocialNetwork::makeFriends(const std::string &email1, const std::string &email2) {
    /* TODO */
    Node<Profile> * p1 = profiles.getFirstNode();
    Node<Profile> * p2 = profiles.getFirstNode();
    while(p1->data.getEmail() != email1 || p2->data.getEmail() != email2){
        if(p1->data.getEmail() == email1 && p2->data.getEmail() == email2){
            break;
        }else if(p1->data.getEmail() == email1){
            p2 = p2->next;
        }else{
            p1 = p1->next;
        }
    }p1->data.getFriends().insertAtTheTail(&p2->data);
    p2->data.getFriends().insertAtTheTail(&p1->data);
}

void SocialNetwork::likePost(int postId, const std::string &email) {
    /* TODO */
    Node<Profile> * pr = profiles.getFirstNode();
    Node<Post> * po = posts.getFirstNode();
    while(po->data.getPostId() != postId){
        po = po->next;
    }while(pr->data.getEmail() != email){
        pr = pr->next;
    }pr->data.getLikes().insertAtTheTail(&po->data);
}

void SocialNetwork::unlikePost(int postId, const std::string &email) {
    /* TODO */
    Node<Profile> * pr = profiles.getFirstNode();
    while(pr->data.getEmail() != email){
        pr = pr->next;
    }
    Node<Post *> * po = pr->data.getLikes().getFirstNode();
    while(po && po->data->getPostId() != postId){
        po = po->next;
    }pr->data.getLikes().removeNode(po);
}

LinkedList<Profile *> SocialNetwork::getMutualFriends(const std::string &email1, const std::string &email2) {
    /* TODO */
    LinkedList<Profile *> mut;
    Node<Profile> * p1 = profiles.getFirstNode();
    Node<Profile> * p2 = profiles.getFirstNode();
    while(p1){
        if(p1->data.getEmail() != email1){
            p1 = p1->next;
        }else break;
    }while(p2){
        if(p2->data.getEmail() != email2){
            p2 = p2->next;
        }else break;
    }
    
    Node<Profile *> * f1 = p1->data.getFriends().getFirstNode();
    Node<Profile *> * f2 = p2->data.getFriends().getFirstNode();

    while(f1){
        while(f2){
            if(f1->data == f2->data){
                mut.insertAtTheTail(f1->data);
                break;
            }else{
                f2 = f2->next;
            }
        }
        f2 = p2->data.getFriends().getFirstNode();
        f1 = f1->next;
    }
    return mut;
}

LinkedList<Post *> SocialNetwork::getListOfMostRecentPosts(const std::string &email, int k) {
    /* TODO */
    LinkedList<Post *> mlist;
    Node<Profile> * p = profiles.getFirstNode();
    while(p){
        if(p->data.getEmail() != email){
            p = p->next;
        }else break;
    }
    Node<Post *> * pp = p->data.getPosts().getLastNode();
    while(pp && k){
        mlist.insertAtTheTail(pp->data);
        pp = pp->prev;
        k--;
    }
    return mlist;
}

void SocialNetwork::print() const {
    std::cout << "# Printing the social network ..." << std::endl;

    std::cout << "# Number of profiles is " << profiles.getNumberOfNodes() << ":" << std::endl;
    profiles.print();

    std::cout << "# Number of posts is " << posts.getNumberOfNodes() << ":" << std::endl;
    posts.print();

    std::cout << "# Printing is done." << std::endl;
}
