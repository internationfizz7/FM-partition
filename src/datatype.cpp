#include "datatype.h"



Net::Net()
{
    name="";
    index=0;
    cut=false;
    linkcell=0;
    A_size=0;
    B_size=0;
}
Net::~Net()
{}
Cell::Cell()
{
    name = "";
    index = 0;
    size = 0;
    lock=false;
    side=true;
    gain=0;
    /*from=0;
    to=0;*/
    tonode = nullptr;
}
Cell::~Cell()
{}