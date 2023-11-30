#include "bucket.h"
bucket::bucket()
{
    value=0;
    to_node=nullptr;
}
bucket::~bucket()
{}
Node::Node()
{
    cell_id=0;
    next=nullptr;
    prev=nullptr;
    //to_cell=nullptr;
}
Node::Node(int c)
{
    cell_id=c;
    next=nullptr;
    prev=nullptr;
}
Node::~Node()
{}