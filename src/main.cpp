#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <map>
#include <sstream>
#include <set>
#include "datatype.h"
#include "bucket.h"

using namespace std;


ifstream inFile ;
ofstream outFile ;
string out;
int NumNodes, NumTerminals, NumNets, NumPins, Smax = 1;
double r = 0.4 , W = 0 , Asize = 0,Bsize=0 , tempsize = 0;
int maxpin;
int ori_cut,iter_cut,final_cut;
void parser(vector<Cell>& cell,vector<Net>& net,char* argv1,char* argv2);
void cal_max_pin(vector<Cell>& cell);
void ini_cut(vector<Cell>& cell,vector<Net>& net);
void ini_gain(vector<Cell>& cell,vector<Net>& net,int f);
void form_bucket_list(vector<Cell>& cell,vector<Net>& net,vector<bucket>& bucket_list,int f);
void change(Node* to_changed,vector<bucket>& bucket_list,vector<Cell>& cell,vector<Net>& net,int index);
void change_bucket(Node* be_changed,int gain,vector<bucket>& bucket_list,int index);
int iterative_change(vector<bucket>& bucket_list,vector<Cell>& cell,vector<Net>& net,bool& stop);
Node* locate(vector<bucket>& bucket_list,vector<Cell>& cell);
int cal_cut(vector<Cell>& cell,vector<Net>& net);
void unlock_all(vector<Cell>& cell);
void med_cut(vector<Cell>& cell,vector<Net>& net);
void output(vector<Cell>& cell,vector<Net>& net,double timee);
int main(int argc, char **argv){
    clock_t start_overall,end_overall;
    start_overall=clock();
    vector<Cell> cell;
    vector<Net> net;
    int f=0;
    cout<<"------------------------start program------------------------\n\n";

    //parser
    parser(cell,net,argv[1],argv[2]);

    //cut
    ini_cut(cell,net);
    //initialize gain
    ini_gain(cell,net,f);

    //calculate max pin
    cal_max_pin(cell);

    //create bucketlist
    vector<bucket> bucket_list;
    form_bucket_list(cell,net,bucket_list,f);

    //start changing position
    cout<<"6. iteratively change\n";
    bool stop=false;
    int gain=0;
    gain=iterative_change(bucket_list,cell,net,stop);
    iter_cut=cal_cut(cell,net);
    f=1;
    while(stop==false && gain!=0){
        unlock_all(cell);
        med_cut(cell,net);
        ini_gain(cell,net,f);
        form_bucket_list(cell,net,bucket_list,f);
        gain=iterative_change(bucket_list,cell,net,stop);
        iter_cut=cal_cut(cell,net);
    }
    final_cut=iter_cut;
    end_overall=clock();
    output(cell,net,(double)(end_overall-start_overall)/CLOCKS_PER_SEC);

    cout<<"\n\nThis program takes "<<(double)(end_overall-start_overall)/CLOCKS_PER_SEC<<"s in total\n";
    cout<<"------------------------end  program-------------------------\n\n";
    return 0;
    
}





void output(vector<Cell>& cell,vector<Net>& net,double timee){
    cout<<"7. dump output file\n";
    outFile.open(out) ; // argv[1] = "xxx.nodes",ios::in
    if(!outFile)
    {
        cerr << "output File could not be opened !!" << endl;
        exit(1) ;
    }
    outFile<<"Cut_size: "<<final_cut<<endl;
    outFile<<"G1_size: "<<(long int)Asize<<endl;
    outFile<<"G2_size: "<<(long int)Bsize<<endl;
    outFile<<"Time: "<<timee<<" s"<<endl;
    outFile<<"G1:"<<endl;
    bool first=false;
    for(int i=1;i<cell.size();i++){
        if(cell[i].side==false){
            if(first==false){
                first=true;
                outFile<<cell[i].name;
            }
            else{
                outFile<<" "<<cell[i].name;
            }
        }
    }
    outFile<<";\n";
    outFile<<"G2:"<<endl;
    first=false;
    for(int i=1;i<cell.size();i++){
        if(cell[i].side==true){
            if(first==false){
                first=true;
                outFile<<cell[i].name;
            }
            else{
                outFile<<" "<<cell[i].name;
            }
        }
    }
    outFile<<";\n";
    outFile<<"Cut_set:"<<endl;
    first=false;
    for(int i=0;i<net.size();i++){
        if(net[i].A_size!=0 && net[i].B_size!=0){
            if(first==false){
                first=true;
                outFile<<net[i].name;
            }
            else{
                outFile<<" "<<net[i].name;
            }
        }
    }
    outFile<<";\n";
    cout<<"   -"<<endl;
}




void med_cut(vector<Cell>& cell,vector<Net>& net){
    for(int i=0;i<net.size();i++){
        net[i].A_size=0;
        net[i].B_size=0;
    }
    for(int i=0;i<net.size();i++){
        for(int j=0;j<net[i].CC.size();j++){
            if(net[i].CC[j]->side==false){
                net[i].A_size++;
            }
            else{
                net[i].B_size++;
            }
            
        }
        
    }
    
    
}




void unlock_all(vector<Cell>& cell){
    for(int i=0;i<cell.size();i++){
        cell[i].lock=false;
    }
}


int iterative_change(vector<bucket>& bucket_list,vector<Cell>& cell,vector<Net>& net,bool& stop){
    int tt=0;
    int gain=0;
    bool temp=false;
    while(temp==false){
        Node* to_changed=new Node();
        
        //locate
        to_changed=locate(bucket_list,cell);
        /*if(to_changed!=nullptr){
            cout<<"\nto changed: "<<to_changed->cell_id<<endl;
        }*/
        if(to_changed==nullptr){
            //cout<<"acc"<<endl;
            if(tt==0){
                stop=true;
            }
            cout<<"    gain "<<gain<<endl;
            break;
        }
        if(cell[to_changed->cell_id].gain<0 ){
            //cout<<"abb"<<endl;
            if(tt==0){
                stop=true;
            }
            cout<<"    gain "<<gain<<endl;
            break;
        }
        else{
            //cout<<"aee"<<endl;
            tt++;
            gain+=cell[to_changed->cell_id].gain;
            change(to_changed,bucket_list,cell,net,cell[to_changed->cell_id].gain+maxpin);
            //cout<<"gain "<<gain<<endl;
            //temp=true;
            
        }
    }

    return gain;
}


int cal_cut(vector<Cell>& cell,vector<Net>& net){
    iter_cut=0;
    for(int i=0;i<net.size();i++){
        
            
            if(net[i].A_size!=0 && net[i].B_size!=0){
                iter_cut++;
            }
        
    }
    
    
    cout<<"   -iterative cut: "<<iter_cut<<"\n";
    return iter_cut;
}

Node* locate(vector<bucket>& bucket_list,vector<Cell>& cell){
    Node* to_changed=new Node();
    for(int max=bucket_list.size()-1;max>=maxpin;max--){
        //cout<<max<<endl;
        if(bucket_list[max].to_node!=nullptr){
            to_changed=bucket_list[max].to_node;
        }
        else{
            continue;
        }
        bool b=false;
        double t_size;
        if(cell[to_changed->cell_id].side==false)
            t_size=Asize-cell[to_changed->cell_id].size;
        else
            t_size=Asize+cell[to_changed->cell_id].size;
        //cout<<t_size<<" "<<r<<" "<<W<<" "<<Smax<<endl;
        while((t_size)>(r*W+Smax) || (t_size)<(r*W-Smax)){
            //cout<<"又跳過一個襙你媽\n";
            if(to_changed->next!=nullptr){
                to_changed=to_changed->next;
                if(cell[to_changed->cell_id].side==false)
                    t_size=Asize-cell[to_changed->cell_id].size;
                else
                    t_size=Asize+cell[to_changed->cell_id].size;
            }
            else{
                b=true;
                break;
            }
        }
        if(b==true){
            continue;
        }
        return to_changed;
    } 
    return nullptr;
}

void change(Node* to_changed,vector<bucket>& bucket_list,vector<Cell>& cell,vector<Net>& net,int index){
    //get cell information
    //cell[i].gain+maxpin為bucketlist所在位置
    if(to_changed->next!=nullptr){
        //cout<<"xx\n";
        
        if(to_changed->prev==nullptr){
            bucket_list[index].to_node=to_changed->next;
            bucket_list[index].to_node->prev=nullptr;
            to_changed->next=nullptr;
            to_changed->prev=nullptr;
        }
        else{
            Node* t_p=to_changed->prev;
            t_p->next=to_changed->next;
            to_changed->next->prev=t_p;
            to_changed->next=nullptr;
            to_changed->prev=nullptr;
        }
    }
    else{
        //cout<<"yy\n";
        
        if(to_changed->prev==nullptr){
            bucket_list[index].to_node=nullptr;
            to_changed->next=nullptr;
            to_changed->prev=nullptr;
        }
        else{
            Node* t_p=to_changed->prev;
            t_p->next=nullptr;
            to_changed->next=nullptr;
            to_changed->prev=nullptr;
        }
    }
    Cell* base=&cell[to_changed->cell_id];
    bool origin_side=base->side;
    base->side=(!base->side);
    base->lock=true;
    if(origin_side==false){
            Asize-=base->size;
            Bsize+=base->size;
        }
        else{
            Bsize-=base->size;
            Asize+=base->size;
        }
    for(int i=0;i<base->CN.size();i++){
        Net* get_net=base->CN[i];
        //A_size與B_size皆已算好
        bool side=origin_side;
        int from,to;
        if(side==false){
            from=get_net->A_size;
            to=get_net->B_size;
        }
        else{
            from=get_net->B_size;
            to=get_net->A_size;
        }
        //before
        if(to==0){
            //cout<<"to==0\n";
            for(int j=0;j<get_net->CC.size();j++){
                if(get_net->CC[j]->side==side && get_net->CC[j]!=base && get_net->CC[j]->lock==false){
                    int index=get_net->CC[j]->gain+maxpin;
                    //cout<<" "<<get_net->CC[j]->index<<" "<<index<<endl;
                    Node* be_changed=get_net->CC[j]->tonode;
                    get_net->CC[j]->gain++;
                    change_bucket(be_changed,1,bucket_list,index);
                }
            }
        }
        else if(to==1){
            //cout<<"to==1\n";
            for(int j=0;j<get_net->CC.size();j++){
                if(get_net->CC[j]->side!=side && get_net->CC[j]!=base && get_net->CC[j]->lock==false){
                    int index=get_net->CC[j]->gain+maxpin;
                    //cout<<" "<<get_net->CC[j]->index<<" "<<index<<endl;
                    Node* be_changed=get_net->CC[j]->tonode;
                    get_net->CC[j]->gain--;
                    change_bucket(be_changed,-1,bucket_list,index);
                }
            }
        }
        //reflect the move
        from--;
        to++;
        if(side==false){
            get_net->A_size--;
            get_net->B_size++;
        }
        else{
            get_net->B_size--;
            get_net->A_size++;
        }
        side=(!side);
        //after
        if(from==0){
            //cout<<"from==0\n";
            for(int j=0;j<get_net->CC.size();j++){
                if(get_net->CC[j]->side==side && get_net->CC[j]!=base && get_net->CC[j]->lock==false){
                    int index=get_net->CC[j]->gain+maxpin;
                    //cout<<" "<<get_net->CC[j]->index<<" "<<index<<endl;
                    Node* be_changed=get_net->CC[j]->tonode;
                    get_net->CC[j]->gain--;
                    change_bucket(be_changed,-1,bucket_list,index);
                }
            }
        }
        else if(from==1){
            //cout<<"from==1\n";
            for(int j=0;j<get_net->CC.size();j++){
                if(get_net->CC[j]->side!=side && get_net->CC[j]!=base && get_net->CC[j]->lock==false){
                    int index=get_net->CC[j]->gain+maxpin;
                    //cout<<" "<<get_net->CC[j]->index<<" "<<index<<endl;
                    Node* be_changed=get_net->CC[j]->tonode;
                    get_net->CC[j]->gain++;
                    
                    change_bucket(be_changed,1,bucket_list,index);
                }
            }
        }
    }

}

void change_bucket(Node* be_changed,int gain,vector<bucket>& bucket_list,int index){
    if(be_changed->prev==nullptr){
        bucket_list[index].to_node=be_changed->next;
        if(bucket_list[index].to_node!=nullptr){
            bucket_list[index].to_node->prev=nullptr;
        }
        be_changed->next=nullptr;
        be_changed->prev=nullptr;
        if(gain==1){
            index++;
            if(bucket_list[index].to_node!=nullptr){
                Node* temp=bucket_list[index].to_node;
                bucket_list[index].to_node=be_changed;
                be_changed->prev=nullptr;
                bucket_list[index].to_node->next=temp;
                temp->prev=bucket_list[index].to_node;
            }
            else{
                bucket_list[index].to_node=be_changed;
                be_changed->prev=nullptr;
                be_changed->next=nullptr;
            }
        }
        else if(gain==-1){
            index--;
            if(bucket_list[index].to_node!=nullptr){
                Node* temp=bucket_list[index].to_node;
                bucket_list[index].to_node=be_changed;
                be_changed->prev=nullptr;
                bucket_list[index].to_node->next=temp;
                temp->prev=bucket_list[index].to_node;
            }
            else{
                bucket_list[index].to_node=be_changed;
                be_changed->prev=nullptr;
                be_changed->next=nullptr;
            }
        }
        else{
            cout<<"gain error!\n";
        }
    }
    else{
        be_changed->prev->next=be_changed->next;
        if(be_changed->next!=nullptr){
            be_changed->next->prev=be_changed->prev;
        }
        be_changed->next=nullptr;
        be_changed->prev=nullptr;
        if(gain==1){
            index++;
            if(bucket_list[index].to_node!=nullptr){
                Node* temp=bucket_list[index].to_node;
                bucket_list[index].to_node=be_changed;
                be_changed->prev=nullptr;
                bucket_list[index].to_node->next=temp;
                temp->prev=bucket_list[index].to_node;
            }
            else{
                bucket_list[index].to_node=be_changed;
                be_changed->prev=nullptr;
                be_changed->next=nullptr;
            }
        }
        else if(gain==-1){
            index--;
            if(bucket_list[index].to_node!=nullptr){
                Node* temp=bucket_list[index].to_node;
                bucket_list[index].to_node=be_changed;
                be_changed->prev=nullptr;
                bucket_list[index].to_node->next=temp;
                temp->prev=bucket_list[index].to_node;
            }
            else{
                bucket_list[index].to_node=be_changed;
                be_changed->prev=nullptr;
                be_changed->next=nullptr;
            }
        }
        else{
            cout<<"gain error!\n";
        }
    }
}



void cal_max_pin(vector<Cell>& cell){
    cout<<"4. calculate max pin\n";
    maxpin=0;
    for(int i=1;i<cell.size();i++){
        if(cell[i].CN.size()>maxpin){
            maxpin=cell[i].CN.size();
        }
    }
    cout<<"   -maxpin: "<<maxpin<<"\n";
}

void form_bucket_list(vector<Cell>& cell,vector<Net>& net,vector<bucket>& bucket_list,int f){
    if(f==0)
        cout<<"5. form bucket list\n";
    bucket_list.resize(2*maxpin+1);
    for(int i=0;i<bucket_list.size();i++){
        bucket_list[i].value=i-maxpin;
        bucket_list[i].to_node=nullptr;
    }
    //cout<<bucket_list.size()<<"\n";
    for(int i=1;i<cell.size();i++){
        int index=cell[i].gain+maxpin;
        Node* node = new Node(cell[i].index);
        cell[i].tonode=node;
        if(bucket_list[index].to_node==nullptr){
            //cout << node->cell_id<<" ";
            bucket_list[index].to_node=node;
        }
        else{
            //cout<<node<<" ";
            Node* temp=bucket_list[index].to_node;
            bucket_list[index].to_node=node;
            node->prev=nullptr;
            bucket_list[index].to_node->next=temp;
            temp->prev=bucket_list[index].to_node;
        }
        
    }
    if(f==0)
        cout<<"   -"<<endl;
    //test
    /*for(int i=bucket_list.size()-1;i>=0;i--){
        cout<<i<<endl;
        if(bucket_list[i].to_node!=nullptr){
            cout<<" "<<bucket_list[i].to_node->cell_id<<endl;
        }
    }*/
 
}



void ini_gain(vector<Cell>& cell,vector<Net>& net,int f){
    if(f==0)
        cout<<"3. initialize gain\n";
    


    for(int i=1;i<cell.size();i++){
        cell[i].gain=0;
        bool c_side=cell[i].side;
        int from;
        int to;
        for(int j=0;j<cell[i].CN.size();j++){
            if(c_side==false){
                from=cell[i].CN[j]->A_size;
                to=cell[i].CN[j]->B_size;
            }
            else{
                from=cell[i].CN[j]->B_size;
                to=cell[i].CN[j]->A_size;
            }
            if(from==1){
                cell[i].gain++;
            }
            if(to==0){
                cell[i].gain--;
            }
        }
    }
    if(f==0)
        cout<<"   -\n";
}

void ini_cut(vector<Cell>& cell,vector<Net>& net){
    cout<<"2. initial cut\n";
    int half=NumNodes/2;
    ori_cut=0;
    double target=r*W;
    Bsize=W;
    Asize=0;
    for(int i=0;i<cell.size();i++){
        if(Asize+cell[i].size<target){
            cell[i].side=false;
            Asize+=cell[i].size;
            Bsize-=cell[i].size;
        }
    }

    for(int i=0;i<net.size();i++){
        for(int j=0;j<net[i].CC.size();j++){
            if(net[i].CC[j]->side==false){
                net[i].A_size++;
            }
            else{
                net[i].B_size++;
            }
            
        }
        if(net[i].A_size!=0 && net[i].B_size!=0){
            ori_cut++;
        }
    }
    
    
    cout<<"   -original cut: "<<ori_cut<<"\n";
}





void parser(vector<Cell>& cell,vector<Net>& net,char* argv1,char* argv2){
    cout<<"1. start parser\n";
    string str, tempstr, terminal, name ;
    string a1=argv1;
    std::size_t found = a1.find(".");
    out=a1.substr(0,found);
    out+=".out";
    //cout<<out<<endl;
    int counter = 1 , index ;
    int link, reallink = 0 ,Pmax = 0, cutsize = 0 , gain = 0 , realcutsize = 0;
    int Fn = 0 , Tn = 0 , A = 0 , B = 0  ;
    double w,h;
    set<string> cellset;
    map<string,int> mapcell;

    inFile.open(argv1) ; // argv[1] = "xxx.nodes",ios::in
    if(!inFile)
    {
        cerr << ".nodes File could not be opened !!" << endl;
        exit(1) ;
    }
    for( int i = 0 ; i < 4 ; ++i )
    {
        getline(inFile , tempstr);
    }
    inFile >> str >> str >> NumNodes;
    inFile >> str >> str >> NumTerminals;
    NumNodes = NumNodes - NumTerminals; // real num of cells we need
    cell.resize(NumNodes+1);
    while(!inFile.eof())
    {
        if(counter < NumNodes+1)
        {
            inFile >> name >> w >> h ;
        }
        else
        {
            inFile >> name >> w >> h >> terminal;
        }
        if(terminal !="")
        {
            break;
        }
        else
        {
            cell[counter].name = name;
            cell[counter].size = w*h;
            cell[counter].index = counter ; // 0 for special use !
            mapcell[name] = counter;
            if(cell[counter].size > Smax)
            {
                Smax = cell[counter].size;
            }
            W += cell[counter].size ;
        }
        counter++ ;
    }
    inFile.close() ;
    counter=0;
    inFile.open(argv2) ; //argv[2] = "xxx.nets",ios::in
    if(!inFile)
    {
        cerr << ".nets File could not be opened !!" << endl;
        exit(1) ;
    }
    for( int i = 0 ; i < 4 ; ++i )
    {
        getline(inFile , tempstr);
    }
    inFile >> str >> str >> NumNets;
    inFile >> str >> str >> NumPins;
    net.resize(NumNets);
    for(int i = 0 ; i < net.size() ; ++i)
    { 
        inFile >> str >> str >> link >> name;
        net[i].name = name ;
        net[i].cut = 0 ;
        net[i].index = i ;
        net[i].CC.resize(link) ;
        for(int j = 0 ; j < link ; ++j)
        {
            inFile >> name >> str >> str >> str >> str ;
            if( mapcell [name] != 0 ) // not a terminal node
            {   
                //if( !cellset.count(name)   ) // cell name non-repeating
                {
                    cellset.insert(name);
                    net[i].CC[reallink] = &cell[mapcell[name]];
                    reallink++;
                }
            }
            cellset.clear();
        }
        net[i].linkcell = reallink ;
        net[i].CC.resize(reallink) ;
        reallink = 0 ;
    }
    // CN connected 
    for(int i = 0 ; i < net.size() ; ++i)
    {
        for(int j = 0 ; j < net[i].linkcell ; ++j)
        {
            cell[ net[i].CC[j]->index ].CN.push_back(&net[i]) ;
        }
    }
    inFile.close() ;
    cout<<"   -\n";
}