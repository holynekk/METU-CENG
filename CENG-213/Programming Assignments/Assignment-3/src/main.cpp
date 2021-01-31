#include "Graph.h"
#include <iostream>
/*You can add additional libraries here to read and process the dataset*/
#include <fstream> // file read/write

using namespace std;

// tester
int main()
{

    // YOU CAN WRITE YOUR TEST CASES BELOW
    //Testcase9
    
    Graph G;

    Node a1 = Node(1, "a", "Africa", 1209);
    Node b2 = Node(2, "b", "South America", 8042);
    Node c3 = Node(4, "c", "Europe", 28942);
    Node d4 = Node(5, "d", "South America", 7053);
    Node e5 = Node(6, "e", " Asia", 272);
    Node f6 = Node(7, "f", "Lux.Europe", 27129);
    Node g7 = Node(12, "g", "South America", 4589);
    Node h8 = Node(14, "h", "South America", 2089);
        
    G.addNode(a1);
    G.addNode(b2);
    G.addNode(c3);
    G.addNode(d4);
    G.addNode(e5);
    G.addNode(f6);
    G.addNode(g7);
    G.addNode(h8);
        

    G.addConnection(a1, b2, 1000);
    G.addConnection(a1, d4, 1000);
    G.addConnection(a1, e5, 1000);
    G.addConnection(a1, f6, 1000);

    G.addConnection(b2, a1, 1000);
    G.addConnection(b2, c3, 1000);
    G.addConnection(b2, e5, 1000);
    G.addConnection(b2, g7, 1000);

    G.addConnection(c3, b2, 1000);
    G.addConnection(c3, d4, 1000);
    G.addConnection(c3, g7, 1000);
    G.addConnection(c3, h8, 1000);

    G.addConnection(d4, a1, 1000);
    G.addConnection(d4, c3, 1000);
    G.addConnection(d4, f6, 1000);
    G.addConnection(d4, h8, 1000);

    G.addConnection(e5, a1, 1000);
    G.addConnection(e5, b2, 1000);
    G.addConnection(e5, f6, 1000);
    G.addConnection(e5, h8, 1000);

    G.addConnection(f6, a1, 1000);
    G.addConnection(f6, d4, 1000);
    G.addConnection(f6, e5, 1000);
    G.addConnection(f6, g7, 1000);

    G.addConnection(g7, b2, 1000);
    G.addConnection(g7, c3, 1000);
    G.addConnection(g7, f6, 1000);
    G.addConnection(g7, h8, 1000);

    G.addConnection(h8, a1, 1000);
    G.addConnection(h8, c3, 1000);
    G.addConnection(h8, d4, 1000);
    G.addConnection(h8, g7, 1000);



    Graph H;

    Node _1a = Node(1, "a'", "Africa", 1209);
    Node _2b = Node(2, "b'", "South America", 8042);
    Node _3c = Node(4, "c'", "Europe", 28942);
    Node _4d = Node(5, "d'", "South America", 7053);
    Node _5e = Node(6, "e'", " Asia", 272);
    Node _6f = Node(7, "f'", "Lux.Europe", 27129);
    Node _7g = Node(12, "g'", "South America", 4589);
    Node _8h = Node(14, "h'", "South America", 2089);
        
    H.addNode(_1a);
    H.addNode(_2b);
    H.addNode(_3c);
    H.addNode(_4d);
    H.addNode(_5e);
    H.addNode(_6f);
    H.addNode(_7g);
    H.addNode(_8h);
        

    H.addConnection(_1a, _2b, 1000);
    H.addConnection(_1a, _3c, 1000);
    H.addConnection(_1a, _7g, 1000);
    H.addConnection(_1a, _8h, 1000);

    H.addConnection(_2b, _1a, 1000);
    H.addConnection(_2b, _3c, 1000);
    H.addConnection(_2b, _6f, 1000);
    H.addConnection(_2b, _8h, 1000);

    H.addConnection(_3c, _1a, 1000);
    H.addConnection(_3c, _2b, 1000);
    H.addConnection(_3c, _4d, 1000);
    H.addConnection(_3c, _5e, 1000);

    H.addConnection(_4d, _3c, 1000);
    H.addConnection(_4d, _5e, 1000);
    H.addConnection(_4d, _6f, 1000);
    H.addConnection(_4d, _8h, 1000);

    H.addConnection(_5e, _3c, 1000);
    H.addConnection(_5e, _4d, 1000);
    H.addConnection(_5e, _6f, 1000);
    H.addConnection(_5e, _7g, 1000);

    H.addConnection(_6f, _2b, 1000);
    H.addConnection(_6f, _4d, 1000);
    H.addConnection(_6f, _5e, 1000);
    H.addConnection(_6f, _7g, 1000);

    H.addConnection(_7g, _1a, 1000);
    H.addConnection(_7g, _5e, 1000);
    H.addConnection(_7g, _6f, 1000);
    H.addConnection(_7g, _8h, 1000);

    H.addConnection(_8h, _1a, 1000);
    H.addConnection(_8h, _2b, 1000);
    H.addConnection(_8h, _4d, 1000);
    H.addConnection(_8h, _7g, 1000);



    
    list<Node> fora = G.getAdjacentNodes(a1);
    cout << "a: ";
    for(list<Node>::iterator it = fora.begin(); it != fora.end(); it++){
        cout << it->getCountry() << "-";
    }cout << '\n';
    fora = G.getAdjacentNodes(b2);
    cout << "b: ";
    for(list<Node>::iterator it = fora.begin(); it != fora.end(); it++){
        cout << it->getCountry() << "-";
    }cout << '\n';
    fora = G.getAdjacentNodes(c3);
    cout << "c: ";
    for(list<Node>::iterator it = fora.begin(); it != fora.end(); it++){
        cout << it->getCountry() << "-";
    }cout << '\n';
    fora = G.getAdjacentNodes(d4);
    cout << "d: ";
    for(list<Node>::iterator it = fora.begin(); it != fora.end(); it++){
        cout << it->getCountry() << "-";
    }cout << '\n';
    fora = G.getAdjacentNodes(e5);
    cout << "e: ";
    for(list<Node>::iterator it = fora.begin(); it != fora.end(); it++){
        cout << it->getCountry() << "-";
    }cout << '\n';
    fora = G.getAdjacentNodes(f6);
    cout << "f: ";
    for(list<Node>::iterator it = fora.begin(); it != fora.end(); it++){
        cout << it->getCountry() << "-";
    }cout << '\n';
    fora = G.getAdjacentNodes(g7);
    cout << "g: ";
    for(list<Node>::iterator it = fora.begin(); it != fora.end(); it++){
        cout << it->getCountry() << "-";
    }cout << '\n';
    fora = G.getAdjacentNodes(h8);
    cout << "h: ";
    for(list<Node>::iterator it = fora.begin(); it != fora.end(); it++){
        cout << it->getCountry() << "-";
    }cout << '\n';


    cout << "--------------------------------------" << endl;

    fora = H.getAdjacentNodes(_1a);
    cout << "a': ";
    for(list<Node>::iterator it = fora.begin(); it != fora.end(); it++){
        cout << it->getCountry() << "-";
    }cout << '\n';
    fora = H.getAdjacentNodes(_2b);
    cout << "b': ";
    for(list<Node>::iterator it = fora.begin(); it != fora.end(); it++){
        cout << it->getCountry() << "-";
    }cout << '\n';
    fora = H.getAdjacentNodes(_3c);
    cout << "c': ";
    for(list<Node>::iterator it = fora.begin(); it != fora.end(); it++){
        cout << it->getCountry() << "-";
    }cout << '\n';
    fora = H.getAdjacentNodes(_4d);
    cout << "d': ";
    for(list<Node>::iterator it = fora.begin(); it != fora.end(); it++){
        cout << it->getCountry() << "-";
    }cout << '\n';
    fora = H.getAdjacentNodes(_5e);
    cout << "e': ";
    for(list<Node>::iterator it = fora.begin(); it != fora.end(); it++){
        cout << it->getCountry() << "-";
    }cout << '\n';
    fora = H.getAdjacentNodes(_6f);
    cout << "f': ";
    for(list<Node>::iterator it = fora.begin(); it != fora.end(); it++){
        cout << it->getCountry() << "-";
    }cout << '\n';
    fora = H.getAdjacentNodes(_7g);
    cout << "g': ";
    for(list<Node>::iterator it = fora.begin(); it != fora.end(); it++){
        cout << it->getCountry() << "-";
    }cout << '\n';
    fora = H.getAdjacentNodes(_8h);
    cout << "h': ";
    for(list<Node>::iterator it = fora.begin(); it != fora.end(); it++){
        cout << it->getCountry() << "-";
    }cout << '\n';
    

    return 0;
}
