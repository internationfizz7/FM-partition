#ifndef _BK_H_
#define _BK_H_

#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <map>
#include <sstream>
#include <set>
using namespace std;

class bucket;
class Node;
class bucket{
public:
    int value;
    Node* to_node;
    
    bucket();
    ~bucket();
    friend class Node;
};

class Node{
public:
    int cell_id;
    Node* next;
    Node* prev;
    //Cell* to_cell;

    Node();
    Node(int );
    ~Node();
    friend class bucket;
}; 

#endif