#ifndef __route_h__
#define __route_h__

#include "lib_io.h"
/* #include "ns.h" */
#include <vector>
#include <iostream>					// for cout etc.
#include <algorithm>				// for sort algorithm
#include <time.h>					// for random seed
#include <math.h>					// for abs()
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <queue>
#include <stack>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <limits>
#include <cstddef>
#include <string.h>
#include <iostream>
#include <vector>
#include "lib_io.h"


using namespace std;

# define MAX 1000
# define INF 2047483600

/* #define GA_ELITRATE		0.10f		// elitism rate */
/* #define GA_MUTATIONRATE	0.25f		// mutation rate */
/* #define GA_MUTATIONRATE	0.8f		// mutation rate */
#define GA_MUTATION		RAND_MAX * GA_MUTATIONRATE
/* #define GA_POPSIZE		50		// ga population size */
#define GA_MAXITER		4000		// maximum iterations

#define MAX_NODE_NUM 1005
#define ARC_CAP_MAX 100000
#define INF 2047483600
#define Candidate_num_little 30
#define Candidate_num_medium 50
#define Candidate_num_big 200
#define Hot_list_little 5
#define Hot_list_medium 10
#define Hot_list_big 20

#define NO_SOLUTION 0
#define QIWEI_FIND 1
#define Bukejie 2


#define Arc_Basic 0
#define Arc_Low 1
#define Arc_Up 2

struct NSarc;
struct NSnode;



extern float GA_ELITRATE;				// elitism rate
extern float GA_MUTATIONRATE;
extern int GA_POPSIZE;
extern float HanMingDis;
extern int PopInitRate;

extern int TIME_LIMIT;


struct ga_struct;
typedef vector<ga_struct> ga_vector;// for brevity

typedef struct edge Edge;
typedef struct mcmf MCMF;


struct inspire_interface;
struct graph_init;
template<class T>
T ABS( const T x )
{
    return( x >= T( 0 ) ? x : - x );
}

template<class T>
void Swap( T &v1, T &v2 )
{
    T temp = v1;
    v1 = v2;
    v2 = temp;
}

template <typename T>
class Get_Type_Max
{
public:
    Get_Type_Max() {}
    operator T()
    {
        return( std::numeric_limits<T>::max() );
    }
};

/* **********************************    MCMF Fun    **************************************** */
struct NSarc
{
    NSnode *from;
    NSnode *to;
    int cap;
    int flow;
    int cost;
    char flag;
};


struct Candidiate
{
    NSarc *arc;
    int absRC;
};

struct graph_init
{
    int G_n;
    int G_m;
    vector<int>  CAP;
    vector<int>  Deficit;  //node鐨勬敹鏀?
    vector<int> From;    //璧峰鐐?
    vector<int> TO;    // 缁堟鐐?
    vector<int>  COST;
    int server_cost;
    graph_init() {}
    void operator=(graph_init *a)
    {
        G_n=a->G_n;
        G_m=a->G_m;
        for(int i=0; i<a->G_m; i++)
        {
            CAP.push_back(a->CAP[i]);
            From.push_back(a->From[i]);
            TO.push_back(a->TO[i]);
            COST.push_back(a->COST[i]);
        }
        for(int i=0; i<a->G_n; i++)
        {
            Deficit.push_back(a->Deficit[i]);
        }
        server_cost = a->server_cost;
    }
};


struct inspire_interface
{
    vector<int> server_list;	//鍚彂寮忕粰瀹氱殑server list
    int link_cost;				//MCFS绠楀畬鐨勭珛閾捐矾涓婄殑cost
    vector<int> server_used;	//鐢ㄥ埌鐨剆erver
    vector<int> server_balance;	//鐢ㄥ埌鐨剆erver涓婄殑balance
};

struct edge{
    int from,to,cap,flow,cost;
};
struct NSnode
{
    NSnode *pred;
    NSnode *thread;
    int depth;
    NSarc *Entering_Arc;
    int balance;
    int potential;
};


struct mcmf{
    int n,m,s,t; //n:v_num   m:e_num
    int server_cost;
    vector<Edge> edges;
    vector<int> G[MAX];
    int inq[MAX];
    int d[MAX];
    int p[MAX];
    int a[MAX];
    int cus[MAX];

    void init(int n){
        this->n = n;
        for(int i = 0;i < n;i++) G[i].clear();
        edges.clear();
        memset(this->inq,0,sizeof(this->inq));
        memset(this->d,0,sizeof(this->d));
        memset(this->p,0,sizeof(this->p));
        memset(this->a,0,sizeof(this->a));
        memset(this->cus,0,sizeof(this->cus));
    }

    void clearup(){
        vector<Edge>().swap(this->edges);
        for(int i = 0;i<MAX;i++)
            vector<int>().swap(this->G[i]);

    }

    void AddEdge(int from,int to,int cap,int cost){
        edges.push_back((Edge){from,to,cap,0,cost});
        edges.push_back((Edge){to,from,0,0,-cost});
        m = edges.size();
        G[from].push_back(m-2);
        G[to].push_back(m-1);
    }

    void AddSuperS(int to){
        edges.push_back((Edge){this->s,to,INF,0,0});
        edges.push_back((Edge){to,this->s,-INF,0,0});
        m = edges.size();
        G[this->s].push_back(m-2);
        G[to].push_back(m-1);
    }

    void AddSuperT(int from,int cap){
        edges.push_back((Edge){from,this->t,cap,0,0});
        edges.push_back((Edge){this->t,from,-INF,0,0});
        m = edges.size();
        G[from].push_back(m-2);
        G[this->t].push_back(m-1);
    }

    void SaveCustomer(int customer,int netnode){
        /* save the num. of the customer */
        this->cus[netnode] = customer;
    }

    bool FlowCheck(int flow){
        vector<int> ::iterator it;
        int max_flow = 0;
        for(it = G[this->t].begin();it != G[this->t].end();it ++){
           max_flow += edges[(*it)^1].cap;
        }
        return (flow == max_flow);
    }

    vector<int> ServerCheck(){
        vector<int> server;
        server.clear();
        vector<int> ::iterator it;
        for(it = G[this->s].begin();it != G[this->s].end();it ++){
            if(edges[(*it)].flow > 0){
                server.push_back(edges[(*it)].to);
            }

        }
        return server;
    }

    void edge_check(Edge * e){
        int cost = abs(e->cost);
        if(e->flow > 0)
            if(e->flow < e->cap)
                e->cost = cost;
            else
                e->cost = INF;
        else if(e->flow < 0)
            if(abs(e->flow) < e->cap)
                e->cost = cost;
            else
                e->cost = INF;
        else
            if(e->from == this->t || e->to == this->s)
                e->cost = INF;
            else
                e->cost = cost;
    }

    void test(){
        vector<Edge> ::iterator it;

        for(it = this->edges.begin();it != edges.end();it ++){
            cout<<"M  tail="<<(*it).to<<" head="<<(*it).from<<" flow="<<(*it).cap<<" cost="<<(*it).cost<<endl;
            cout<<endl;
        }
    }
    //other functions
    //...
};

struct MCFS
{
    /*******************************basic of the graph**********************************************/
    int n;
    int node_max_num;
    int m;
    int arc_max_num;
    int status;

    /******************************* arc **********************************************/
    NSarc *arc_array_begin;
    NSarc *Fake_Arc;
    NSarc *arc_array_end;
    NSarc *Fake_Arc_End;

    /******************************* node **********************************************/
    NSnode *node_array_begin;
    NSnode *Fake_Root;
    NSnode *node_array_end;

    /******************************* candidate list *************************************/
    Candidiate *candy;
    int Candy_Group_Num;
    int Tmp_Candy_Size;
    int Now_Candy;
    int Candy_Size;
    int Hotlist_Size;
    int MAX_COST;

    MCFS( int nmx = 0, int mmx = 0 )
    {
        node_max_num = nmx;
        arc_max_num = mmx;
        n = m = 0;
        node_array_begin = NULL;
        arc_array_begin = NULL;
        candy = NULL;
        status = NO_SOLUTION;
        MAX_COST = int( 1e7 );

        if( node_max_num && arc_max_num )
            MAlloc();
        else
            node_max_num = arc_max_num = 0;
    }

    void MAlloc( void );
    void DAlloc( void );

    NSnode* Father( NSnode *n, NSarc *a );

    void Refresh_Tree( NSarc *h, NSarc *k, NSnode *h1, NSnode *h2, NSnode *k1, NSnode *k2 );

    void Potential( NSnode *r, int delta );

    int Cost_Reduce( NSarc *a );

    void InitNet(graph_init *graph);
    void ReadData(char * topo[MAX_EDGE_NUM],int line_num,graph_init *graph);
    void Add_Super_Arc(vector<int> server_list,graph_init* graph);


    void MllocCandy(void);
    void InitBase(void);
    void Init_Candy( void );

    NSarc* Candy_Pivot( void );
    void Simplex( void );

    long Get_Arc_Cost( void )
    {
        if( status == QIWEI_FIND )
        {
            long arc_cost = 0;
            NSarc *arc;
            for( arc = arc_array_begin ; arc != arc_array_end ; arc++ )
            {
                if( ( arc->flag == Arc_Basic ) || ( arc->flag == Arc_Up ) )
                    arc_cost += arc->cost * arc->flow;
            }
            for( arc = Fake_Arc ; arc != Fake_Arc_End ; arc++ )
            {
                if( ( arc->flag == Arc_Basic ) || ( arc->flag == Arc_Up ) )
                    arc_cost += arc->cost * arc->flow;
            }
            return arc_cost ;
        }

        else
        {
            cout<<"qiwei do not solved yet"<<endl;
            return( Get_Type_Max<long>() );
        }
    }

    void qiwei_solve(inspire_interface* inter)
    {
        Simplex();
        if( status == QIWEI_FIND )
        {
            //return MCFGetFO();
            inter->link_cost=Get_Arc_Cost();
            qiwei_server_used(inter);
        }
        else
        {
            /* cout<<"sorry , qiwei can not solve"<<endl; */
            inter->link_cost=INF;
        }
    }

    void qiwei_server_used(inspire_interface* inter)
    {
        int server_num=inter->server_list.capacity();
        NSarc* arc=arc_array_begin+m-server_num;
        for(arc=arc_array_begin+m-server_num; arc<arc_array_end; arc++)
        {
            int flow=arc->flow;
            int to=(arc->to)-node_array_begin;
            if(flow!=0)
            {
                inter->server_used.push_back(to);
                inter->server_balance.push_back(flow);
            }
        }
    }

};


/* **********************************    The end     **************************************** */

/* **********************************    SPFA QUEUE     **************************************** */
typedef struct QElemType
{
    int NetId;
    int dist;
}QElemType;

typedef struct QNode
{
    QElemType data;
    struct QNode *next;
}QNode, *QueuePtr;

// for LLL SLF
typedef struct LinkQueue
{
    QueuePtr front;
    QueuePtr rear;
    long long sum;
    int item_num;
    void init_Queue()
    {
        this->front = this->rear = (QNode *)malloc(sizeof(QNode));
        this->front->next = NULL;
        this->rear->next = NULL;
        this->sum = 0;
        this->item_num = 0;
    }

    // (from rear)
    void en_Queue_Rear(QElemType e)
    {
        QueuePtr temp = (QueuePtr)malloc(sizeof(QNode));
        if(temp)
        {
            temp->data = e;
            temp->next = NULL;
            this->rear->next = temp;
            this->rear = temp;
            this->rear->next = NULL;
            this->sum += e.dist;
            this->item_num ++;
        }
    }

    //  (from top)
    void en_Queue_Top(QElemType e)
    {
        QueuePtr temp = (QueuePtr)malloc(sizeof(QNode));
        if(temp)
        {
            temp->data = e;
            if(this->front == this->rear) {
                this->front->next = temp;
                this->rear = temp;
            }
            else
                temp->next = this->front->next;
            this->front->next = temp;
            this->sum += e.dist;
            this->item_num ++;
        }
    }

    //  (have Top Node)
    void de_Queue()
    {
        if(this->front == this->rear)
            return;
        QueuePtr temp = this->front->next;
        if(this->front->next == this->rear)
            this->rear = this->front;
        this->front->next = temp->next;
        this->sum -= temp->data.dist;
        this->item_num --;
        free(temp);
    }

    void de_Queue_where(int aver,QElemType &out)
    {
        QueuePtr tos;
        QueuePtr pre;
        tos = this->front->next;
        pre = this->front;
        while(pre != this->rear){
            if(tos->data.dist <= aver){
                break;
            }
            pre = tos;
            tos = tos->next;
        }
        if(tos == NULL){
            return;
        }
        else{
            out.NetId= tos->data.NetId;
            out.dist= tos->data.dist;
            pre->next = tos->next;
            this->item_num --;
            this->sum -= out.dist;
            if(tos == this->rear){
                this->rear = pre;
            }

            tos->next = NULL;
            pre = NULL;
            free(tos);
        }
    }

    int is_Empty()
    {
        if(this->rear== this->front)
            return 1;
        return 0;
    }

    int getlength_Queue()
    {
        QueuePtr temp = this->front;
        int i = 0;
        while(temp != this->rear)
        {
            ++i;
            temp = temp->next;
        }
        return i;
    }

    void clear()
    {
        QueuePtr temp = this->front->next;
        while(temp)
        {
            QueuePtr tp = temp;
            temp = temp->next;
            free(tp);
        }
        temp = this->front;
        this->front = this->rear = NULL;
        free(temp);
    }

    void print_Queue()
    {
        if(this->rear== this->front)  {
            cout << "The Queue is empty !!!!" << endl;
            return;
        }
        QueuePtr temp = this->front->next;
        while(temp != this->rear)
        {
            cout << temp->data.NetId << '\t';
            cout << "******************" << '\t';
            cout << temp->data.dist << endl;
            temp = temp->next;
        }
        cout << temp->data.NetId << '\t';
        cout << "******************" << '\t';
        cout << temp->data.dist << endl;
        printf("\n");
        cout << "the item_num is : " << this->item_num << endl;
    }

    QElemType GetTop()
    {
        QElemType e;
        e.NetId = -1;
        e.dist = -1;
        if(this->front == this->rear)
            return e;
        e = this->front->next->data;
        return e;
    }
    void Qswap(LinkQueue &P){
        QueuePtr tmp = NULL;
        long long sum_tmp;

        tmp = this->front;
        this->front = P.front;
        P.front = tmp;

        tmp = this->rear;
        this->rear = P.rear;
        P.rear = tmp;

        sum_tmp = this->sum;
        this->sum = P.sum;
        P.sum = sum_tmp;
    }
}LinkQueue;

/* **********************************    The end     **************************************** */
inline void MCFS::Add_Super_Arc(vector<int> server_list,graph_init* graph)
{
    int server_num=server_list.capacity();
    /* cout<<"afaewfasfdwqaefaf server_num="<<server_num<<endl; */
    for(int i = 0; i<server_num; i++)
    {
        graph->From.push_back(graph->G_n);
        graph->TO.push_back(int(server_list[i]+1));
        graph->CAP.push_back(ARC_CAP_MAX);
        graph->COST.push_back(0);//super S 到 server cost =0
        graph->G_m++;
        /* cout<<"i="<<i<<" "<<Index(server_list[i]+1)<<endl; */
    }

}

inline void MCFS::ReadData(char * topo[MAX_EDGE_NUM],int line_num,graph_init *graph)
{
    int pNum[line_num][4];	//
    int tmp = 0;
    int k = 0;
    for(int ii = 0; ii < line_num; ii++)					//
        for(int jj = 0; jj < (int)strlen(topo[ii]); jj++)
            if(topo[ii][jj] == '\r' || topo[ii][jj] == '\n')
            {
                pNum[ii][k++]= tmp;
                tmp = 0;
                k = 0;
                break;
            }
            else if(topo[ii][jj] != ' ')
                tmp = tmp * 10 + (int)(topo[ii][jj] - 48);
            else
            {
                pNum[ii][k++]= tmp;
                tmp = 0;
            }
    ////////////////////////////////////////create the MCFS STRUCTURE//////////
    /* create cdn graph */
    /*
     * STEP 0: get edge_num,cus_num,node_num 0
     * STEP 1: get server cost 2
     * STEP 2: get edge 4～3+edge_num
     * STEP 3: get customer 5+edge_num ~ 4+edge_num+cus_num
     * start[4] = {0,2,4,5+edge_num}
     * end[4] = {0,2,4+edge_num,5+edge_num+cus_num}
     */
    int node_num = pNum[0][0];
    int edge_num = pNum[0][1];
    int cus_num = pNum[0][2];
    int server_cost = pNum[2][0];
    cout<<"********************node_num"<<node_num<<endl;

    int start[4] = {0,2,4,-1};
    int end[4] = {0,2,-1,-1};
    end[2] = 4 + edge_num;	//边情况end
    start[3] = 5 + edge_num;  //消费节点begin
    end[3] = 5 + edge_num + cus_num;//消费节点end

    graph->G_n=node_num+1;//The number of nodes Add one node:Surper S
    graph->G_m=edge_num*2;//The number of arcs twice the num of edge and the num of server
    graph->server_cost = server_cost;
    cout<<"the Index tm=="<<graph->G_m<<endl;


    for( int i = 0 ; i < graph->G_n ; i++ )           // all deficits are 0
    {
        graph->Deficit.push_back(0);                      // unless otherwise stated
    }

    /************************NODE INFO*******************************/
    int sum_supply=0;
    int connect_node_num=0;
    int node_demand=0;
    int i=0;
    for(i = start[3]; i < end[3]; i++) //消费节点情况
    {
        connect_node_num=pNum[i][1];
        node_demand=pNum[i][2];		//demand or supply ?????????????????????????????
        graph->Deficit[connect_node_num] -= (-node_demand);
        sum_supply+=node_demand;
        //cout<<"sum_supply==="<<sum_supply<<endl;
    }

    graph->Deficit[graph->G_n-1]=(-(sum_supply));//最后那个节点作为超级源节点
    //cout<<"i+1="<<i+1<<" tDfct[i+1]"<<tDfct[i+1]<<endl;
    /*************************ARC INFO*******************************/
    int arc_begin=0;
    int arc_end=0;
    int arc_cap=0;
    int arc_cost=0;
    int j=0;


    for(int i = start[2]; i < end[2]; i++)
    {
        arc_begin=pNum[i][0];
        arc_end=pNum[i][1];
        arc_cap=pNum[i][2];
        arc_cost=pNum[i][3];
        graph->From.push_back(arc_begin+1);
        graph->TO.push_back(arc_end+1);
        graph->CAP.push_back(arc_cap);
        graph->COST.push_back(arc_cost);

        //add both the direction arc
        graph->From.push_back(arc_end+1);
        graph->TO.push_back(arc_begin+1);
        graph->CAP.push_back(arc_cap);
        graph->COST.push_back(arc_cost);
    }

}

inline void MCFS::InitNet(graph_init *graph)
{
    int nmx=graph->G_n;
    int mmx=graph->G_m;
    int pn=graph->G_n;
    int pm=graph->G_m;
    int* pU=&(graph->CAP[0]);
    int* pC=&(graph->COST[0]);
    int* pDfct=&(graph->Deficit[0]);
    int* pSn=&(graph->From[0]);
    int* pEn=&(graph->TO[0]);

    delete[] candy;
    candy = NULL;

    if( ( nmx != node_max_num ) || ( mmx != arc_max_num ) ) //图改变之后，看两者大小是否一样
    {
        if( node_max_num && arc_max_num )     //如果内存已经分配了
        {
            DAlloc();
            node_max_num = arc_max_num = 0;
        }

        if( nmx && mmx )
        {
            node_max_num = nmx;
            arc_max_num = mmx;
            MAlloc();
        }
    }
    if( ( ! node_max_num ) || ( ! arc_max_num ) )
        // 如果有一个内存没有分配正确，那就都设为0
        node_max_num = arc_max_num = 0;

    if( node_max_num )    // if the new dimensions of the memory are correct，如果两者内存都分配正确
    {
        n = pn;     //图中存放的点的总数设为pn
        m = pm;     //图中的边数

        node_array_end = node_array_begin + n;
        Fake_Root = node_array_begin + node_max_num;//
        NSnode *Fake_Root;

        for( NSnode *NSnode = node_array_begin ; NSnode != node_array_end ; NSnode++ )
            NSnode->balance = pDfct[ NSnode - node_array_begin ];  // initialize nodes 初始化节点的balance

        arc_array_end = arc_array_begin + m;
        Fake_Arc = arc_array_begin + arc_max_num;
        Fake_Arc_End = Fake_Arc + n;
        /* cout<<"qiwei_stopDummyP= "<<n<<" "<<dummyArcsP<<endl; */
        for( NSarc *arc = arc_array_begin ; arc != arc_array_end ; arc++ )
        {
            //这里是最重要的函数，是为了将图形放到结构体中
            arc->cost = pC[ arc - arc_array_begin ];
            arc->cap = pU[ arc - arc_array_begin ];
            arc->from = node_array_begin + pSn[ arc - arc_array_begin ] - 1;
            arc->to = node_array_begin + pEn[ arc - arc_array_begin ] - 1;
        }

        MllocCandy();

        status = NO_SOLUTION;
    }
    InitBase();
}

inline void MCFS::MllocCandy(void) //对于不同规模的图，使用不同的候选者ule参数
{
    if( m < 10000 )
    {
        Candy_Size = Candidate_num_little;
        Hotlist_Size = Hot_list_little;
    }
    else if( m > 100000 )
    {
        Candy_Size = Candidate_num_big;
        Hotlist_Size = Hot_list_big ;
    }
    else
    {
        Candy_Size = Candidate_num_medium;
        Hotlist_Size = Hot_list_medium;
    }

    candy = new Candidiate[ Hotlist_Size + Candy_Size + 1 ];

}

inline void MCFS::DAlloc(void)
{

    delete[] node_array_begin;
    delete[] arc_array_begin;
    node_array_begin = NULL;
    arc_array_begin = NULL;

    delete[] candy;
    candy = NULL;
}

inline void MCFS::MAlloc( void )
{
    node_array_begin = new NSnode[ node_max_num + 1 ];
    arc_array_begin = new NSarc[ arc_max_num + node_max_num ];
    Fake_Arc = arc_array_begin + arc_max_num;
}

inline NSnode* MCFS::Father( NSnode *n, NSarc *a )
{
    if( a == NULL )
        return NULL;

    if( a->from == n )
        return( a->to );
    else
        return( a->from );
}



inline void MCFS::Refresh_Tree( NSarc *h, NSarc *k, NSnode *h1, NSnode *h2, NSnode *k1, NSnode *k2 )
{
    int delta = (k1->depth) + 1 - (k2->depth);
    NSnode *root = k2;
    NSnode *dad;
    NSnode *previousNode = k1;
    NSnode *lastNode;
    NSarc *arc1 = k;
    NSarc *arc2;
    bool fine = false;
    while( fine == false )
    {
        if( root == h2 )
            fine = true;
        dad = Father( root, root->Entering_Arc );
        int level = root->depth;
        NSnode *node = root;
        while ( ( node->thread ) && ( ( node->thread )->depth > level ) )
        {
            node = node->thread;
            node->depth = node->depth + delta;
        }

        root->depth = root->depth + delta;

        if( root->pred )
            ( root->pred )->thread = node->thread;
        if( node->thread )
            ( node->thread )->pred = root->pred;
        lastNode=node;

        NSnode *nextNode = previousNode->thread;
        root->pred = previousNode;
        previousNode->thread = root;
        lastNode->thread = nextNode;
        if( nextNode )
            nextNode->pred = lastNode;

        previousNode = lastNode;
        delta = delta + 2;
        arc2 = root->Entering_Arc;
        root->Entering_Arc = arc1;
        arc1 = arc2;
        root = dad;
    }
}


inline void MCFS::Potential( NSnode *r, int delta )
{
    int level = r->depth;
    NSnode *n = r;
	n->potential = n->potential + delta;
	n = n->thread;
    while ( ( n ) && ( n->depth > level ) )
    {
        n->potential = n->potential + delta;
        n = n->thread;
    }
}


inline int MCFS::Cost_Reduce( NSarc *tmp )
{
    int reduce = (tmp->from)->potential - (tmp->to)->potential;
    reduce = reduce + tmp->cost;
    return( reduce );
}


inline void MCFS::InitBase(void)
{
    NSarc *arc;
    NSnode *NSnode;
    for(arc = arc_array_begin; arc != arc_array_end; arc++)
    {
        arc->flow=0;
        arc->flag=Arc_Low;
    }
    for(arc=Fake_Arc; arc!=Fake_Arc_End; arc++)
    {
        NSnode = node_array_begin + ( arc - Fake_Arc );
        if( NSnode->balance > 0 )
        {
            arc->from = Fake_Root;
            arc->to = NSnode;
            arc->flow = NSnode->balance;
        }
        else
        {
            arc->from = NSnode;
            arc->to = Fake_Root;
            arc->flow = -NSnode->balance;
        }
        arc->cost = MAX_COST;
        arc->flag = Arc_Basic;
        arc->cap = Get_Type_Max<int>();
    }
    Fake_Root->balance = 0;
    Fake_Root->pred = NULL;
    Fake_Root->thread = node_array_begin;
    Fake_Root->Entering_Arc = NULL;
    Fake_Root->potential = MAX_COST;
    Fake_Root->depth = 0;
    for( NSnode = node_array_begin ; NSnode != node_array_end ; NSnode++)
    {
        NSnode->pred = NSnode - 1;
        NSnode->thread = NSnode + 1;
        NSnode->Entering_Arc = Fake_Arc + (NSnode - node_array_begin);
        if( NSnode->balance > 0 )
            NSnode->potential = 2 * MAX_COST;
        else
            NSnode->potential = 0;

        NSnode->depth = 1;
    }

    node_array_begin->pred = Fake_Root;
    ( node_array_begin + n - 1 )->thread = NULL;
}


inline void MCFS::Init_Candy( void )
{
    Candy_Group_Num = ( ( m - 1 ) / Candy_Size ) + 1;
    Now_Candy = 0;
    Tmp_Candy_Size = 0;
}
/* **********************************    generic     **************************************** */

struct ga_struct
{
	string  server;			// 1:biuld server here ;  0:not
	int serverNum;
	vector<int> server_id;
	int fitness;					// its fitness

    void clearup(){
        this->server.erase();
        this->serverNum = 0;
        this->fitness = 0;
        this->server_id.erase(this->server_id.begin());
        vector<int>().swap(this->server_id);
    }

    void print_ga(){
        cout << "the num of server is :  " << this->serverNum << endl;
        vector<int>:: iterator it;
        int i = 0;
        for(it = this->server_id.begin();it!=this->server_id.end();it++){
            cout <<"server["<< i << "]"<< '=' << (*it) << ';' <<'\n';
            i++;
        }
        cout << endl;
    }
};

/* **********************************    The end     **************************************** */


/* **********************************    main        **************************************** */

void deploy_server(char * graph[MAX_EDGE_NUM], int edge_num, char * filename);
void topo_init(char * topo[MAX_EDGE_NUM],int line_num,MCMF * cdn,MCFS * qiwei);

bool BellmanFord(int s,int t,int &flow,int &cost,MCMF *cdn);
void CompTrace(MCMF *cdn,char *trace);

int FinalMincost(int s,int t,MCMF *cdn,char *output);  // compute the path

bool Ga_For_MCF(MCFS * f_cdn,graph_init * clean_graph,char * output);
inspire_interface  MFCSolver(MCFS *cdn,vector<int> server,graph_init* clean_graph);

/* **********************************    The end     **************************************** */
inline NSarc* MCFS::Candy_Pivot( void )
{
    int next = 0;
    int i;
    int minimeValue;
    if( Hotlist_Size < Tmp_Candy_Size )
        minimeValue = Hotlist_Size;
    else
        minimeValue = Tmp_Candy_Size;

    for( i = 2 ; i <= minimeValue ; i++ )
    {
        NSarc *arc = candy[i].arc;
        int red_cost = Cost_Reduce( arc );

        if((( red_cost < 0) && ( arc->flag == Arc_Low ) ) ||
                ((red_cost>0) && ( arc->flag == Arc_Up ) ) )
        {
            next++;
            candy[ next ].arc = arc;
            candy[ next ].absRC = ABS( red_cost );
        }
    }

    Tmp_Candy_Size = next;
    int oldGroupPos = Now_Candy;
    do
    {
        NSarc *arc;
        for( arc = arc_array_begin + Now_Candy ; arc < arc_array_end ; arc += Candy_Group_Num )
        {
            if( arc->flag == Arc_Low )
            {
                int red_cost = Cost_Reduce( arc );
                if( red_cost < 0 )
                {
                    Tmp_Candy_Size++;
                    candy[ Tmp_Candy_Size ].arc = arc;
                    candy[ Tmp_Candy_Size ].absRC = ABS( red_cost );
                }
            }
            else if( arc->flag == Arc_Up )
            {
                int red_cost = Cost_Reduce( arc );
                if(red_cost>0)
                {
                    Tmp_Candy_Size++;
                    candy[ Tmp_Candy_Size ].arc = arc;
                    candy[ Tmp_Candy_Size ].absRC = ABS( red_cost );
                }
            }
        }

        Now_Candy++;
        if( Now_Candy == Candy_Group_Num )
            Now_Candy = 0;

    }
    while( ( Tmp_Candy_Size < Hotlist_Size ) && ( Now_Candy != oldGroupPos ) );

    if( Tmp_Candy_Size )
    {

        int tmp=candy[ 1 ].absRC;
        NSarc *tmp_arc=candy[1].arc;
        for(int i=1; i<Tmp_Candy_Size+1; i++)
        {
            if(candy[i].absRC>tmp)
            {
                tmp=candy[i].absRC;
                tmp_arc=candy[i].arc;
            }
        }
        return tmp_arc;
    }
    else
        return( NULL );
}


inline void MCFS::Simplex( void )
{

    status = NO_SOLUTION;
    NSarc *enteringArc;
    NSarc *leavingArc;
    Init_Candy();

    while( status == NO_SOLUTION )      //还没找到可行解
    {
        enteringArc = Candy_Pivot();	//从候选者列表中找出entering arc
        if( enteringArc )
        {
            NSarc *arc;
            NSnode *K;
            NSnode *L;
            int t;
            int theta;	//用来保存entering上的流量
            if( enteringArc->flag == Arc_Up )	//如果enteringarc是UP，cycle的方向和entering arc相同，且theta=x
            {
                K = enteringArc->to;
                L = enteringArc->from;
                theta = enteringArc->flow;
            }
            else	//否则方向相反，theta=U-X
            {
                K = enteringArc->from;
                L = enteringArc->to;
                theta = enteringArc->cap - enteringArc->flow;
            }

            NSnode *memK1 = K;
            NSnode *memK2 = L;
            leavingArc = NULL;



            //向下是为了寻找leaving ARC
            bool leavingReducesFlow = Cost_Reduce( enteringArc )>0;
            //计算theta,找到eaving arc 和 cycle的root
            bool leave;

            while( K != L )	//确定pivot cycle
            {
                if( K->depth > L->depth )
                {
                    arc = K->Entering_Arc;
                    if( arc->from != K )
                    {
                        t = arc->cap - arc->flow;
                        leave = false;
                    }
                    else
                    {
                        t = arc->flow;
                        leave = true;
                    }

                    if( t < theta )
                    {
                        theta = t;
                        leavingArc = arc;
                        leavingReducesFlow = leave;
                    }

                    K = Father( K, arc );
                }
                else
                {
                    arc = L->Entering_Arc;
                    if( arc->from == L )
                    {
                        t = arc->cap - arc->flow;
                        leave = false;
                    }
                    else
                    {
                        t = arc->flow;
                        leave = true;
                    }

                    if( t <= theta )
                    {
                        theta = t;
                        leavingArc = arc;
                        leavingReducesFlow = leave;
                    }

                    L = Father(L, arc);
                }
            }



            if( leavingArc == NULL )
                leavingArc = enteringArc;


            /***********************************update the flow********************************************/
            // Update flow with "theta" and UPdate tree
            K = memK1;
            L = memK2;

            if(theta!=0)
            {
                if( enteringArc->from == K )
                    enteringArc->flow = enteringArc->flow + theta;
                else
                    enteringArc->flow = enteringArc->flow - theta;

                while( K != L )
                {
                    if( K->depth > L->depth )
                    {
                        arc = K->Entering_Arc;
                        if( arc->from != K )
                            arc->flow = arc->flow + theta;
                        else
                            arc->flow = arc->flow - theta;

                        K = Father(K, K->Entering_Arc);
                    }
                    else
                    {
                        arc = L->Entering_Arc;
                        if( arc->from == L )
                            arc->flow = arc->flow + theta;
                        else
                            arc->flow = arc->flow - theta;

                        L = Father(L, L->Entering_Arc);
                    }
                }
            }



            /***********************************update the tree********************************************/

            if( enteringArc != leavingArc )
            {
                bool leavingBringFlowInT2 = ( leavingReducesFlow ==( ( leavingArc->from )->depth > ( leavingArc->to )->depth ) );
                if( leavingBringFlowInT2 == ( memK1 == enteringArc->from ) )
                {
                    L = enteringArc->from;
                    K = enteringArc->to;
                }
                else
                {
                    L = enteringArc->to;
                    K = enteringArc->from;
                }
            }

            if( leavingReducesFlow )
                leavingArc->flag = Arc_Low;
            else
                leavingArc->flag = Arc_Up;

            if( leavingArc != enteringArc )
            {
                enteringArc->flag = Arc_Basic;
                NSnode *h1;
                NSnode *h2;
                if( ( leavingArc->from )->depth < ( leavingArc->to )->depth )
                {
                    h1 = leavingArc->from;
                    h2 = leavingArc->to;
                }
                else
                {
                    h1 = leavingArc->to;
                    h2 = leavingArc->from;
                }

                Refresh_Tree(leavingArc, enteringArc, h1, h2, K, L);
                L = enteringArc->to;
                int delta = Cost_Reduce(enteringArc);
                if( ( enteringArc->from )->depth > ( enteringArc->to )->depth )
                {
                    delta = -delta;
                    L = enteringArc->from;
                }

                Potential( L, delta );
            }

        }
        else
        {
            status = QIWEI_FIND;
            for( NSarc *arc = Fake_Arc ; arc != Fake_Arc_End ; arc++ )
                if(arc->flow>0)
                    status = Bukejie;

        }
    }
}

/* *************************************************************************** */
/*
 * generic algorithm defination
 */

inline void calc_fitness(ga_vector &population,graph_init *clean_graph);
bool init_population(ga_vector &population, ga_vector &buffer,graph_init *clean_graph);
inline bool fitness_sort(ga_struct x, ga_struct y);
inline void sort_by_fitness(ga_vector &population);
void elitism(ga_vector &population, ga_vector &buffer, int esize );
inline void mutate(ga_struct &member);
void mate(ga_vector &population, ga_vector &buffer);
inline void print_best(ga_vector &gav);
inline void swap(ga_vector *&population, ga_vector *&buffer);


bool hanMing(ga_vector population, ga_struct citizen);
int hanMingDis(ga_struct popu_citizen,ga_struct citizen);
/* *************************************************************************** */


#endif
