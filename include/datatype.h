#ifndef _DATATYPE_H_
#define _DATATYPE_H_

#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <map>
#include <sstream>
#include <set>
#include "bucket.h"
using namespace std;
class Net;
class Cell;
class Net
{
public:
    string name;
    int index;
    bool cut;
    int linkcell;
    vector< Cell* > CC ; // CC stands for connect cell
    int A_size;
    int B_size;
    Net();
    ~Net();
    friend class Cell;
    //friend class Node;
};

class Cell
{
public:
    string name;
    int index;
    int size;
    bool side; //false for A true for B
    int gain;
    /*int from;
    int to;*/
    Node* tonode;
    bool lock;
    vector< Net* > CN ; // CN stands for connect net

    Cell();
    
    ~Cell();
    friend class Net;
    //friend class Node;
};




#endif