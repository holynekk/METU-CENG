#include "Graph.h"

/*Do not add new libraries or files*/


Graph::Graph() {
    // TODO: IMPLEMENT THIS FUNCTION.
    
    // Should I do anything here?
}

Graph::Graph(const Graph& rhs) {
    // TODO: IMPLEMENT THIS FUNCTION.
    adjList = rhs.adjList;
}

Graph& Graph::operator=(const Graph& rhs) {
    // TODO: IMPLEMENT THIS FUNCTION.
    adjList = rhs.adjList;
    return *this;
}

Graph::~Graph() {
// TODO: IMPLEMENT THIS FUNCTION.

}


void Graph::addNode(const Node &node) {
    // TODO: IMPLEMENT THIS FUNCTION.
    list<Edge> edg_list;
    adjList.Insert(node.getCountry(), edg_list);
}

void Graph::addConnection(const Node& headNode, const Node& tailNode, int import) {
    // TODO: IMPLEMENT THIS FUNCTION.
    Edge nedg(tailNode, import);
    list<Edge> * ls = &adjList.Get(headNode.getCountry());
    ls->push_back(nedg);
}

list<Node> Graph::getAdjacentNodes(const Node& node) {
    // TODO: IMPLEMENT THIS FUNCTION.
    list<Node> ret;
    list<Edge> * nn = &adjList.Get(node.getCountry());
    for(list<Edge>::iterator kk = nn->begin(); kk != nn->end(); kk++){
        ret.push_back(kk->getTailNode());
    }
    return ret;
}

long Graph::getTotalImports(const Node& node) {
    // TODO: IMPLEMENT THIS FUNCTION.
    long ans = 0;
    list<Edge> * nn = &adjList.Get(node.getCountry());
    for(list<Edge>::iterator kk = nn->begin(); kk != nn->end(); kk++){
        ans += kk->getImport();
    }
    return ans;
}

void Graph::deleteNode(const Node& node) {
    // TODO: IMPLEMENT THIS FUNCTION.
    string * str_ar = new string[adjList.Size()];
    adjList.getKeys(str_ar);
    for(int i = 0; i < adjList.Size(); i++){
        list<Edge> * nn = &adjList.Get(*(str_ar+i));
        if((*nn).size()){
            for(list<Edge>::iterator kk = nn->begin(); kk != nn->end(); kk++){
                if(kk->getTailNode().getCountry() == node.getCountry()){
                    (*nn).erase(kk);
                    break;
                }
            }
        }
    }adjList.Delete(node.getCountry());
    delete [] str_ar;
}

list<string> Graph::findLeastCostPath(const Node& srcNode, const Node& destNode) {
    // TODO: IMPLEMENT THIS FUNCTION.
    string * str_ar = new string[adjList.Size()];
    adjList.getKeys(str_ar);
    int num = adjList.Size();

    long * distance = new long[num];
    string * previous = new string[num];
    bool * visited = new bool[num];

    list<pair<long*,string>> pq;

    for(int i = 0; i < num; i++){
        distance[i] = numeric_limits<int>::max();
        visited[i] = false;
    }
    distance[ind_finder(str_ar, srcNode.getCountry())] = 0;

    int a = ind_finder(str_ar, srcNode.getCountry());

    visited[a] = true;
    pq.push_back({distance+a, srcNode.getCountry()});
    while(!pq.empty()){
        string str = return_min(&pq);
        list<Edge> * nn = &adjList.Get(str);
        for(list<Edge>::iterator it = nn->begin(); it != nn->end(); it++){
            int ind = ind_finder(str_ar, it->getTailNode().getCountry());
            long newlength = distance[ind_finder(str_ar, str)] + it->getImport();
            long oldlength = distance[ind];
            
            if(!visited[ind]){
                pq.push_back({distance+ind, it->getTailNode().getCountry()});
            }
            visited[ind] = true;
            if(newlength < oldlength){
                distance[ind] = newlength;
                previous[ind] = str;
            }
        }
    }
    list<string> ans;
    const Node * tmp = &destNode;
    string eski = destNode.getCountry();
    ans.push_back(eski);
    
    while(eski != srcNode.getCountry()){
        int ind = ind_finder(str_ar, eski);
        eski = previous[ind];
        ans.push_front(eski);
    }
    delete [] previous;
    delete [] str_ar;
    delete [] distance;
    delete [] visited;
    return ans;
}


bool Graph::isCyclic() {
    // TODO: IMPLEMENT THIS FUNCTION.
    int num = adjList.Size();
    string * str_ar = new string[num];
    adjList.getKeys(str_ar);
    bool * visited = new bool[num];
    bool * recur_stack = new bool[num];
    for(int i = 0; i < num; i++){
        visited[i] = false;
        recur_stack[i] = false;
    }
    for(int i = 0; i < num; i++){
        if(cyc_helper(i, visited, recur_stack, str_ar)){
            delete [] str_ar;
            delete [] visited;
            delete [] recur_stack;
            return true;
        }
    }
    delete [] str_ar;
    delete [] visited;
    delete [] recur_stack;
    return false;
}


list<string> Graph::getBFSPath(const Node& srcNode, const Node& destNode) {
    // TODO: IMPLEMENT THIS FUNCTION.
    queue<int> qu;
    list<string> res;
    if(srcNode.getCountry() == destNode.getCountry()){
        res.push_back(srcNode.getCountry());
        return res;
    }
    int num = adjList.Size();
    string * str_ar = new string[num];
    adjList.getKeys(str_ar);

    bool * visited = new bool[num];
    for(int i = 0; i < num; i++){
        visited[i] = false;
    }
    int sr = ind_finder(str_ar, srcNode.getCountry());
    qu.push(sr);
    visited[sr] = true;

    while(qu.size()){
        int deq = qu.front();
        res.push_back(str_ar[deq]);
        qu.pop();
        list<Edge> * nn = &adjList.Get(str_ar[deq]);
        for(list<Edge>::iterator it = nn->begin(); it != nn->end(); it++){
            string sttrr = it->getTailNode().getCountry();
            sr = ind_finder(str_ar, sttrr);
            if(!visited[sr]){ 
                visited[sr] = true;
                if(sttrr == destNode.getCountry()){
                    while(qu.size()){
                        deq = qu.front();
                        res.push_back(str_ar[deq]);
                        qu.pop();
                    }
                    res.push_back(sttrr);
                    break;
                }else{
                    qu.push(ind_finder(str_ar, sttrr));
                }
            }
        }
    }
    delete [] str_ar;
    delete [] visited;
    return res;
}
