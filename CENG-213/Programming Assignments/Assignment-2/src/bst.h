#ifndef BINARY_SEARCH_TREE
#define BINARY_SEARCH_TREE

#include <functional>
#include <cstddef>
#include <ostream>
#include <list>
#include <vector>
#include <stack>
#include <queue>
#include <cmath>
#include <utility>

#include <iostream>   // you may use it for debugging your code


class DefaultBalanceCondition    // default BST, do not change
{
  public:
    bool operator() (int current_height, int ideal_height) const
    {
      return true;
    }
};


template <typename Key, typename Object,
          typename BalanceCondition=DefaultBalanceCondition,
	  typename Comparator=std::less<Key> >
class BinarySearchTree
{
  public:  /* DO NOT CHANGE */
    struct Node   // core structure of BinarySearchTree
    {
       Key key;         // unique key
       Object data;     // data to be stored, characterized by key
       Node * left;     // pointer to left subtree
       Node * right;    // pointer to right subtree
       size_t height;   // height of the node
       size_t subsize;  // size of the subtree node roots including itself

       Node(const Key &, const Object &, 
            Node *, Node *, size_t =0, size_t =1);   // Node constructor
    };

  public:  /* DO NOT CHANGE PROTOTYPES */
    BinarySearchTree();  // zero-parameter constructor
    BinarySearchTree(const std::list<std::pair<Key, Object> > &); // list is always sorted!
    ~BinarySearchTree();  // destructor

  public:  /* DO NOT CHANGE PROTOTYPES */
    void insert(const Key &, const Object &);  // insert new Key-Object
    void remove(const Key &);  // remove Node characterized by Key
    void toCompleteBST();  // convert into a complete BST

  public:  /* DO NOT CHANGE PROTOTYPES */
    Node * find(const Key &) const; // single item
    std::list<Node *> find(const Key &, const Key &) const; // range queries
    int height() const;  // return the height of the tree
    size_t size() const; // return the number of items in the tree
    bool empty() const;  //return whether the tree is empty or not
    Node * getRoot() const; // return a pointer to the root of the tree
    void print(std::ostream &) const;  // print tree structure into an output stream

  private:  /* DO NOT CHANGE DATA TYPES AND IDENTIFIERS */
    Node * root;                     // designated root
    size_t numNodes;                 // size
    Comparator isLessThan;           // operator upon which BST nodes are arranged
    BalanceCondition isBalanced;     // signals whether the BST is balanced or not
  
  private:  /* DO NOT CHANGE PROTOTYPES */
    Node * find(const Key &, Node *) const;
    int height(Node *) const;
    int subsize(Node *) const;
    void print(Node *, std::ostream &) const;

    // Define your const private utility functions below this line

  private:
    void makeEmpty(Node * &);       // utility for destructor
    
    // Define your private utility functions below this line


  private: /* DO NOT CHANGE PROTOTYPES: compiler defaults are blocked */  
    BinarySearchTree(const BinarySearchTree &);
    const BinarySearchTree & operator=(const BinarySearchTree &);

  private:  // static utility functions
    template <typename T> //static utility function
    static const T & max(const T &, const T &);

    // Define more below this line if needed

    // CUSTOM CUSTOM CUSTOM CUSTOM CUSTOM CUSTOM CUSTOM CUSTOM CUSTOM CUSTOM CUSTOM CUSTOM
    private:
      Node * conhelper(const std::list<std::pair<Key,Object> > & datalist, int a, int b){
        std::list<std::pair<int, float> >::const_iterator itr = datalist.begin();
        if(a > b){
          return NULL;
        }
        int mid = comp_mid(b-a+1)+a;
        int tmp = mid;
        while(tmp--) itr++;
        Node * tr = new Node(itr->first, itr->second, conhelper(datalist, a, mid-1), conhelper(datalist, mid+1, b));
        
        if(tr->left){
          tr->subsize += tr->left->subsize;
          tr->height = tr->left->height+1;
        }
        if(tr->right){
          tr->subsize += tr->right->subsize;
        }
        return tr;
      }
    private:
      int comp_mid(int len) {
        if ( len <= 4 )
          return len / 2;
        int levels = 1;
        int total = 1;
        while ( total < len ) {
          levels *= 2;
          total += levels;
        }
        int excess = len - (total - levels);
        int minMid = (total - levels + 1) / 2;
        if ( excess <= levels / 2 ) {
          return minMid + (excess - 1); 
        } else {
          int midExcess = levels/2; 
          return minMid + (midExcess - 1);
        }
      }
    private:
      void toList(std::vector<Node *> & bumbe, Node * nodnod){
        if (!nodnod){
          return;
        }
        toList(bumbe, nodnod->left);
        bumbe.push_back(nodnod);
        nodnod->height = 0;
        nodnod->subsize = 1;
        toList(bumbe, nodnod->right);
      }
    private:
      Node * toCBT_helper(std::vector<Node *> &nodelist, int a, int b){
        if(a>b){
          return NULL;
        }int mid = comp_mid(b-a+1)+a;
        nodelist[mid]->left = toCBT_helper(nodelist, a, mid-1);;
        nodelist[mid]->right = toCBT_helper(nodelist, mid+1, b);

        if(nodelist[mid]->left){
          nodelist[mid]->subsize += nodelist[mid]->left->subsize;
          nodelist[mid]->height = nodelist[mid]->left->height+1;
        }if(nodelist[mid]->right){
          nodelist[mid]->subsize += nodelist[mid]->right->subsize;
        }

        return nodelist[mid];
      }
    private:
      int hfix(Node * cur){
        if(cur->left && cur->right){
          return max(cur->left->height, cur->right->height)+1;
        }else if(cur->left){
          return cur->left->height + 1;
        }else if(cur->right){
          return cur->right->height + 1;
        }return 0;
      }
    
    private:
      Node * cbcb(Node * parent){
        std::vector<Node *> bumbe;
        toList(bumbe, parent);
        int num = bumbe.size();
        int mid = comp_mid(num);
        parent = bumbe[mid];
        parent->left = toCBT_helper(bumbe, 0, mid-1);
        parent->right = toCBT_helper(bumbe, mid+1, num-1);
        if(parent->left){
            parent->height = parent->left->height+1;
            parent->subsize += parent->left->subsize;
        }if(parent->right){
            parent->subsize += parent->right->subsize;
        }
        return parent;
      }
    private:
      Node * finsert(Node * parent, Node * t){
        if(isLessThan(parent->key, t->key)){
          if(!parent->right){
            parent->right = t;
            parent->subsize++;
            parent->height = hfix(parent);
            if(!isBalanced(parent->height, floor(log2(parent->subsize)))){
              parent = cbcb(parent);
            }
            return parent;
          }
          parent->right = finsert(parent->right, t);
          parent->subsize++;
          parent->height = hfix(parent);
        }else{
          if(!parent->left){
            parent->left = t;
            parent->subsize++;
            parent->height = hfix(parent);
            if(!isBalanced(parent->height, floor(log2(parent->subsize)))){
              parent = cbcb(parent);
            }
            return parent;
          }
          parent->left = finsert(parent->left, t);
          parent->subsize++;
          parent->height = hfix(parent);
        }
        if(!isBalanced(parent->height, floor(log2(parent->subsize)))){
          parent = cbcb(parent);
        }

        return parent;
      }


    private:
      void fran(Node * cur, std::list<Node *> &lst, Key lower, Key upper) const{
        if(!cur){
          return;
        }if(!isLessThan(cur->key, lower) && !isLessThan(lower, cur->key)){
          lst.push_back(cur);
          fran(cur->right, lst, lower, upper);
        }else if(!isLessThan(cur->key, upper) && !isLessThan(upper, cur->key)){
          fran(cur->left, lst, lower, upper);
          lst.push_back(cur);
        }else if(isLessThan(cur->key, upper) &&  !isLessThan(cur->key, lower)){
          fran(cur->left, lst, lower, upper);
          lst.push_back(cur);
          fran(cur->right, lst, lower, upper);
        }else if(isLessThan(cur->key, upper) || !isLessThan(cur->key, lower) && !isLessThan(lower, cur->key)/*cur->key == upper*/){
          fran(cur->right, lst, lower, upper);
        }else if(!isLessThan(cur->key, lower) || !isLessThan(cur->key, upper) && !isLessThan(upper, cur->key)/*cur->key == lower*/){
          fran(cur->left, lst, lower, upper);
        }
        
      }

    private:
      Node * left_finder(Node * cur){
        while(cur->left){
          cur = cur->left;
        }
        return cur;
      }
    private:
      Node * findpre(Node * cur, Node * last){
        while(cur->left != last){
          if(cur->right){
            cur->height = max(cur->left->height-1, cur->right->height)+1;
          }else cur->height--;
          cur->subsize--;
          cur = cur->left;
        }
        if(cur->right){
          cur->height = max(cur->left->height, cur->right->height+1);
        }else cur->height--;
        cur->subsize--;
        return cur;
      }
    
    private:
      Node * rm_helper(Node * parent, Node * rmnode){
        // Deletion part
        if(parent == rmnode){
          // Double childeren
          if(parent->left && parent->right){
            Node * successor = left_finder(parent->right);

            if(parent->right == successor){
              successor->left = parent->left;
              successor->height = hfix(successor);
              if(successor->right){
                successor->subsize = 1+ successor->left->subsize + successor->right->subsize;
              }else{
                successor->subsize = 1+ successor->left->subsize;
              }
              
              delete rmnode;
              if(!isBalanced(successor->height, floor(log2(successor->subsize)))){
                successor = cbcb(successor);
              }
              return successor;

            }else{
              Node * pre = findpre(parent->right, successor);

              if(successor->right){

                pre->left = successor->right;
                successor->right = parent->right;
                successor->left = parent->left;
                delete rmnode;

              }else{

                pre->left = NULL;
                successor->right = parent->right;
                successor->left = parent->left;
                delete rmnode;

              }
              successor->height = hfix(successor);
              successor->subsize = 1 + successor->right->subsize + successor->left->subsize;
              return successor;
            }
          }

          //Single childeren
          else if(parent->left){
            Node * tmp = rmnode->left;
            delete rmnode;
            return tmp;
          }
          else if(parent->right){
            Node * tmp = rmnode->right;
            delete rmnode;
            return tmp;
          }

          // No child
          else{
            delete rmnode;
            return NULL;
          }
        }
        
        
        // Recursion part
        if(isLessThan(parent->key, rmnode->key)){
          parent->right = rm_helper(parent->right, rmnode);
          parent->subsize--;
          parent->height = hfix(parent);
        }else{
          parent->left = rm_helper(parent->left, rmnode);
          parent->subsize--;
          parent->height = hfix(parent);
        }
        if(!isBalanced(parent->height, floor(log2(parent->subsize)))){
          parent = cbcb(parent);
        }

        return parent;
      }
    // CUSTOM CUSTOM CUSTOM CUSTOM CUSTOM CUSTOM CUSTOM CUSTOM CUSTOM CUSTOM CUSTOM CUSTOM

};

#endif


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
BinarySearchTree<K,O,B,C>::Node::
Node(const K & _k, const O & _d, Node * _l, Node * _r, size_t _h, size_t _s)
  : key(_k), data(_d), left(_l), right(_r), height(_h), subsize(_s)
{
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
BinarySearchTree<K,O,B,C>::BinarySearchTree()
  : root(NULL), numNodes(0)  //comparator(C() ??)
{
}



// IMPLEMENT
template <typename K, typename O, typename B, typename C>
BinarySearchTree<K,O,B,C>::BinarySearchTree(const std::list<std::pair<K,O> > & datalist)
   : root(NULL), numNodes(0)  // change it as you'd like
{
  int num = datalist.size();
  int mid = comp_mid(num), tmp = mid;
  std::list<std::pair<int, float> >::const_iterator itr = datalist.begin();
  while(tmp--) itr++;

  Node * r = new Node(itr->first, itr->second, conhelper(datalist, 0, mid-1), conhelper(datalist, mid+1, num-1));
  root = r;
  numNodes = num;
  root->height = root->left->height+1;
  root->subsize += root->left->subsize + root->right->subsize;
}


// IMPLEMENT
template <typename K, typename O, typename B, typename C>
void
BinarySearchTree<K,O,B,C>::toCompleteBST()
{
  std::vector<Node *> bumbe;
  toList(bumbe, root);
  int num = bumbe.size();
  int mid = comp_mid(num);
  root = bumbe[mid];
  root->left = toCBT_helper(bumbe, 0, mid-1);
  root->right = toCBT_helper(bumbe, mid+1, num-1);
  if(root->left){
      root->height = root->left->height+1;
      root->subsize += root->left->subsize;
  }if(root->right){
      root->subsize += root->right->subsize;
  }
}


// IMPLEMENT
template <typename K, typename O, typename B, typename C>
void
BinarySearchTree<K,O,B,C>::insert(const K & k, const O & x)
{
  Node * p = find(k);
  if(p){
    p->data = x;
    return;
  }
  Node * nwnode = new Node(k, x, NULL, NULL);
  if(!root){
    root = nwnode;
  }else{
    root = finsert(root, nwnode);
  }
  numNodes++;
}


// IMPLEMENT
template <typename K, typename O, typename B, typename C>
void
BinarySearchTree<K,O,B,C>::remove(const K & k)
{
  Node * p = find(k);
  if(!p){
    return;
  }
  root = rm_helper(root, p);
  if(!isBalanced(root->height, floor(log2(root->subsize)))){
    root = cbcb(root);
  }
  numNodes--;
}



// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
typename BinarySearchTree<K,O,B,C>::Node *
BinarySearchTree<K,O,B,C>::find(const K & key) const
{
  return find(key, root);
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
typename BinarySearchTree<K,O,B,C>::Node *
BinarySearchTree<K,O,B,C>::find(const K & key, Node * t) const
{
  if (t == NULL)
  {
    return NULL;
  }
  else if (isLessThan(key, t->key))
  {
    return find(key, t->left);
  }
  else if (isLessThan(t->key, key))
  {
    return find(key, t->right);
  }
  else //found
  {
    return t;
  }
}


// IMPLEMENT
template <typename K, typename O, typename B, typename C>
std::list<typename BinarySearchTree<K,O,B,C>::Node *>
BinarySearchTree<K,O,B,C>::find(const K & lower, const K & upper) const
{
  std::list<Node *> ranges;

  // implement

  fran(root, ranges, lower, upper);


  return ranges;
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
typename BinarySearchTree<K,O,B,C>::Node *
BinarySearchTree<K,O,B,C>::getRoot() const
{
  return root;
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
int
BinarySearchTree<K,O,B,C>::height() const
{
  return height(root);
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
int
BinarySearchTree<K,O,B,C>::height(Node * t) const
{
  return (t == NULL) ? -1 : t->height;
}


// private utility
template <typename K, typename O, typename B, typename C>
int
BinarySearchTree<K,O,B,C>::subsize(Node * t) const
{
  return (t == NULL) ? 0 : t->subsize;
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
size_t
BinarySearchTree<K,O,B,C>::size() const
{
  return numNodes;
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
bool
BinarySearchTree<K,O,B,C>::empty() const
{
  return numNodes == 0;
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
void
BinarySearchTree<K,O,B,C>::makeEmpty(Node * & t)
{
  if (t != NULL)
  {
    makeEmpty(t->left);
    makeEmpty(t->right);
    delete t;

    --numNodes;
  }
  
  t = NULL;
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
BinarySearchTree<K,O,B,C>::~BinarySearchTree()
{
  makeEmpty(root);
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
void
BinarySearchTree<K,O,B,C>::print(std::ostream & out) const
{
  print(root, out);
  out << '\n';
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
void
BinarySearchTree<K,O,B,C>::print(Node * t, std::ostream & out) const
{
  if (t != NULL && t->left != NULL) 
  {
    out << '[';
    print( t->left, out );
  }
  else if (t != NULL && t->left == NULL && t->right != NULL)
  {
    out << "[";
  }

  if (t != NULL)
  {
    if (t->left == NULL && t->right == NULL)
    {
      out << '(' << (t->key) << ')';
    }
    else if (t->left != NULL || t->right != NULL)
    {
      out << '{' << (t->key) << ",H" << t->height << ",S" << t->subsize << '}';
    }
  }
  
  if (t != NULL && t->right != NULL)
  {
    print( t->right, out );
    out << ']';
  }
  else if (t != NULL && t->left != NULL && t->right == NULL)
  {
    out << "]";
  }
}


// do not change
template <typename K, typename O, typename B, typename C>
template <typename T>
const T &
BinarySearchTree<K,O,B,C>::max(const T & el1, const T & el2)
{
  return el1 > el2 ? el1 : el2;
}
