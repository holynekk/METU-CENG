#include <list>
#include <stack>
#include <queue>
#include <vector>

#include "HashTable.h"
#include "Edge.h"
/* Do not add new libraries or files */

using namespace std;

/** An adjacency list representation of a directed weighted graph. */
class Graph {

public:

    // TODO: IMPLEMENT THESE FUNCTIONS.
    /** CONSTRUCTORS, ASSIGNMENT OPERATOR, AND THE DESTRUCTOR */
    Graph();
    Graph(const Graph& rhs);
    Graph& operator=(const Graph& rhs);
    ~Graph();

    // TODO: IMPLEMENT THIS FUNCTION.
    /** Adds the given node to the graph with vid or country as key,
     * and an empty edge list as value */
    void addNode(const Node& node);

    // TODO: IMPLEMENT THIS FUNCTION.
    /** Adds a new edge to the edge list of headNode using tailNode and import data */
    void addConnection(const Node& headNode, const Node& tailNode, int import);

    // TODO: IMPLEMENT THIS FUNCTION.
    /** Given a country, returns all adjacent
     * countries of the given country as a list of Node
     * Throw ItemNotFoundException, if the given node does not exist */
    list<Node> getAdjacentNodes(const Node& node);

    // TODO: IMPLEMENT THIS FUNCTION.
    /** For the given node, returns the sum of imports */
    long getTotalImports(const Node& node);

    // TODO: IMPLEMENT THIS FUNCTION.
    /** Deletes the given country from the graph
     * together with its incident connections(edges) */
    void deleteNode(const Node& node);

    // TODO: IMPLEMENT THIS FUNCTION.
    /** Given two countries , returns the least cost path/route between
     * them using import values as the edge weight */
    list<string> findLeastCostPath(const Node& srcNode, const Node& destNode);

    // TODO: IMPLEMENT THIS FUNCTION.
    /** Detect whether the graph contains any cycle */
    bool isCyclic();

    // TODO: IMPLEMENT THIS FUNCTION.
    /** returns the BFS path from srcNode to destNode as a list of country names */
    list<string> getBFSPath(const Node& srcNode, const Node& destNode);

private:
    /** Adjacency list representation of the graph; 
    You can change the signature of this variable*/
    HashTable<string, list<Edge>> adjList;

    // == DEFINE HELPER METHODS & VARIABLES BELOW ==
    
    string return_min(list<pair<long*,string>> * pq){
        string minn = "";
        long imp = numeric_limits<int>::max();
        list<pair<long*,string>>::iterator erase;
        for(list<pair<long*,string>>::iterator it = pq->begin(); it != pq->end(); it++){
            if(*it->first < imp){
                imp = *it->first;
                minn = it->second;
                erase = it;
            }else;
        }
        pq->erase(erase);
        return minn;
    }

    int ind_finder(string * keys, string str){
        int ind = 0;
        while(true){
            if(keys[ind] == str){
                break;
            }
            ind++;
        }return ind;
    }
    
    bool cyc_helper(int i, bool * visited, bool * recur_stack, string * keys){

        if(!visited[i]){
            visited[i] = true;
            recur_stack[i] = true;
            list<Edge> * nn = &adjList.Get(*(keys+i));
            for(list<Edge>::iterator it = nn->begin(); it != nn->end(); it++){
                int ind = ind_finder(keys, it->getTailNode().getCountry());
                if(!visited[ind] && cyc_helper(ind, visited, recur_stack, keys)){
                    return true;
                }else if(recur_stack[ind]){
                    return true;
                }
            }
        }
        recur_stack[i] = false;
        return false;
    }
};
// End of header file
