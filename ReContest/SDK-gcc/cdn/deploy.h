#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "lib/lib_io.h"
#include "sim.h"
#include "fusai.h"
#include <set>
#include <stack>

/* #define E 0.000000001  // 迭代误差 */
#define E 0.1  // 迭代误差
#define L_INIT 70 //20000   // 迭代次数inline
#define L 200 //20000   // 迭代次数outline
#define AT 0.9  //0.999   // 降温系数 不要动，这里的0.9就是很好的值
// #define Z 30   //初始化种群的个数
// #define D 10    //汉明距离
#define HanMingDis      0.9f
#define PopInitRate     90
#define GA_ELITRATE		0.3f		// elitism rate
#define GA_MUTATIONRATE	0.8f		// mutation rate
#define GA_MUTATION		RAND_MAX * GA_MUTATIONRATE
#define GA_POPSIZE		10		// ga population size
#define GA_MAXITER		4		// maximum iterations

extern int GA_TARSIZE;
extern int is_first_calcu;
using namespace std;



int inline_tuihuo(char * topo[MAX_EDGE_NUM], int line_num,vector<QIFA> &inline_server);
void outline_tuihuo(graph_init *clean_graph,Info *fusai,vector<QIFA> &min_server,int &min_cost);
int confirm_key_server(graph_init *clean_graph,Info *fusai,vector<QIFA> &key_server,int level);

int tuihuo(graph_init *clean_graph,Info * fusai,vector<int> &server,time_t begin);
int tuihuo2(graph_init *clean_graph,Info * fusai,vector<QIFA> &server,int &min_cost,time_t begin);
int tuihuo_test2(graph_init *clean_graph,Info * fusai,vector<QIFA> &server,int &min_cost,time_t begin);

/* **********************************    main    **************************************** */
typedef struct mcmf MCMF;
struct server_in;
struct precision;
void deploy_server(char * graph[MAX_EDGE_NUM], int edge_num, char * filename);
vector<int> ZLSolver(graph_init* clean_graph,Info * fusai,vector<int> &server_id,int &cost);
int FZLSolver(graph_init* clean_graph,Info * fusai,MCFS * cdn,vector<int> server_id);
bool fast_use_sim(graph_init* clean_graph,Info * fusai,MCFS * cdn,vector<int> server_id);

bool qifa_sim(graph_init* clean_graph,Info * fusai,vector<QIFA> &server_qifa);
bool qifa_sim_out(graph_init* clean_graph,Info * fusai,vector<QIFA> server_qifa,MCFS *cdn);
void Fast_ReadData(char * topo[MAX_EDGE_NUM],int line_num,graph_init *graph);


void change_text(graph_init* clean_graph);

precision BACK_Solver_Out(graph_init* clean_graph, vector<int> &server,Info *fusai,MCMF *out);
precision BACK_Solver(graph_init* clean_graph, vector<int> &server,Info *fusai);





struct precision
{
	NS_STATUS status=Bukejie;
    int link_cost=0;
    int none_zero_total_cost=0;
    int fake_back_total_cost=0;
    int all_node_total_cost=0;

};




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
struct mcmf{
    int n,m,s,t; //n:v_num   m:e_num
    int server_type_num;
    vector<server_node> server;
    vector<Server> ServerInfo;
    vector<Node> NodeInfo;
    vector<Edge> edges;
    vector<int> G[MAX_NODE_NUM];
    int inq[MAX_NODE_NUM];
    int d[MAX_NODE_NUM];
    int p[MAX_NODE_NUM];
    int a[MAX_NODE_NUM];
    int * cus;

    void topo_init(char * topo[MAX_EDGE_NUM],int line_num){
        int pNum[line_num][4];
        int tmp = 0;
        int k = 0;
        for(int i = 0;i < line_num;i++)
            for(int j = 0;j < (int)strlen(topo[i]);j++)
                if(topo[i][j] == '\r' || topo[i][j] == '\n'){
                    pNum[i][k++]= tmp;
                    tmp = 0;
                    k = 0;
                    break;
                }
                else if(topo[i][j] != ' ')
                    tmp = tmp * 10 + (int)(topo[i][j] - 48);
                else{
                    pNum[i][k++]= tmp;
                    tmp = 0;
                }

        /* case 0 */
        int net_num = pNum[0][0];
        int edge_num = pNum[0][1];
        int cus_num = pNum[0][2];
        this->init(net_num + 2);
        this->s = net_num;
        this->t = net_num +1;

        /* case 1 */
        int SN = 2;
        do{
            Server tmp = {pNum[SN][0],pNum[SN][1],pNum[SN][2]};
            SN ++;
            this->ServerInfo.push_back(tmp);
        }while(pNum[SN][0] != 0);
        this->server_type_num = SN - 2;

        /* case 2,3,4 pre */
        int start[5] = {0,2,SN+1,-1,-1};
        int end[5] = {0,SN-1,SN+net_num,-1,-1};
        start[3] = end[2]+2;
        end[3] = start[3] + edge_num - 1;
        start[4] = end[3] + 2;
        end[4] = start[4] + cus_num - 1;
        /* case 2 */
        for(int i = start[2];i<= end[2];i++){
            this->NodeInfo.push_back((Node){pNum[i][0],pNum[i][1],-1,1,NULL});
        }

        /* case 3 */
        /* for(int i = start[3];i<= end[3];i++) */
        /* this->AddEdge(pNum[i][0],pNum[i][1],pNum[i][2],pNum[i][3]); */
        /* int size = cdn->G_arc.size(); */
        /* for(int i = 0;i<size;i++){ */
            /* if(cdn->G_arc[i].flow != 0){ */
                /* int from = cdn->G_arc[i].from; */
                /* int to = cdn->G_arc[i].to; */
                /* int cap = cdn->G_arc[i].flow; */
                /* int cost = cdn->G_arc[i].cost; */
                /* if(from == this->s){ */
                    /* this->AddSuperS(to,cap); */
                    /* struct server_node s = {to,cap}; */
                    /* this->server.push_back(s); */
                /* } */
                /* else */
                    /* this->AddEdge(from,to,cap,cost); */
            /* } */

        /* } */

        /* case 4 */
        this->cus = (int *)malloc(sizeof(int)*net_num);
        memset(this->cus,-1,sizeof(int)*net_num);
        for(int i = start[4];i <= end[4];i++){
            this->AddSuperT(pNum[i][1],pNum[i][2]);
            this->SaveCustomer(pNum[i][0],pNum[i][1]);
        }
    }

    void key_edge_init(MCFS *cdn){
        int size = cdn->G_arc.size();
        for(int i = 0;i<size;i++){
            if(cdn->G_arc[i].flow != 0){
                int from = cdn->G_arc[i].from;
                int to = cdn->G_arc[i].to;
                int cap = cdn->G_arc[i].flow;
                int cost = cdn->G_arc[i].cost;
                if(from == this->s){
                    this->AddSuperS(to,cap);
                    struct server_node s = {to,cap};
                    this->server.push_back(s);
                }
                else
                    this->AddEdge(from,to,cap,cost);
            }

        }
    }

    void init(int n){
        this->n = n;
        for(int i = 0;i < n;i++) G[i].clear();
        edges.clear();
        memset(this->inq,0,sizeof(this->inq));
        memset(this->d,0,sizeof(this->d));
        memset(this->p,0,sizeof(this->p));
        memset(this->a,0,sizeof(this->a));
    }

    void clearup(){
        vector<Edge>().swap(this->edges);
        for(int i = 0;i<MAX_NODE_NUM;i++)
            vector<int>().swap(this->G[i]);

    }

    void AddEdge(int from,int to,int cap,int cost){
        edges.push_back((Edge){from,to,cap,0,cost});
        edges.push_back((Edge){to,from,0,0,-cost});
        m = edges.size();
        G[from].push_back(m-2);
        G[to].push_back(m-1);
    }

    void AddSuperS(int to,int cap){
        edges.push_back((Edge){this->s,to,cap,0,0});
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

    int choose_server(int supply){
        int type_id = this->server_type_num - 1;
        for(int i = 0;i<=type_id;i++){
            if(supply <= this->ServerInfo[i].server_cap)
                return i;
        }
        return -1;
    }

    int compute_setup_cost(){
       int size = this->server.size();
       int cost = 0;
       for(int i = 0;i<size;i++){
           int type_id = choose_server(this->server[i].supply);
           if(type_id != -1){
               int setup_cost = this->NodeInfo[this->server[i].node_num].node_server_cost;
               cost += setup_cost + this->ServerInfo[type_id].server_cost;
           }
           else
               return INF;
       }
       return cost;
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

int FinalMincost(int s,int t,MCMF *cdn,char *output);



/* **********************************    generic     **************************************** */


struct ga_struct
{
	string  server;			// 1:biuld server here ;  0:not
	int serverNum;
	vector<QIFA> server_id;
	int fitness;					// its fitness

    void clearup(){
        this->server.erase();
        this->serverNum = 0;
        this->fitness = 0;
        vector<QIFA>().swap(this->server_id);
    }

    void print_ga(){
        cout << "the num of server is :  " << this->serverNum << endl;
        vector<QIFA>:: iterator it;
        int i = 0;
        /* for(it = this->server_id.begin();it!=this->server_id.end();it++){ */
            /* cout <<"server["<< i << "].server_id"<< '=' << (*it).server_id << ';' <<'\n'; */
            /* cout <<"server["<< i << "].server_type"<< '=' << (*it).server_type<< ';' <<'\n'; */
            /* i++; */
        /* } */
        int sz = server_id.size();
        for(int i = 0;i<sz;i++){
            cout << server_id[i].server_id << "  ";
        }
        cout << endl;
        cout << server << endl;
        cout << endl;
    }
};

typedef struct ga_pop_init{
    vector<QIFA >pop_init[4];
    int cost[4];
}ga_pop_init;

typedef vector<ga_struct> ga_vector;// for brevity

struct inspire_interface
{
    vector<int> server_list;	
    int cost;			
    vector<int> server_used;
};

/* **********************************    The end     **************************************** */

bool hanMing(ga_vector population, ga_struct citizen);
int hanMingDis(ga_struct popu_citizen,ga_struct citizen);

/* ****************************       source        ****************************** */
/* ga_struct init_key_serevr(char * topo[MAX_EDGE_NUM], int line_num,Info *fusai,int level); */
/* bool init_population(char * topo[MAX_EDGE_NUM], int line_num,ga_vector &population, ga_vector &buffer,Info *fusai); */
/* inline void calc_fitness(char * topo[MAX_EDGE_NUM], int line_num,ga_vector &population,Info *fusai); */
/* vector<int> Ga_For_MCF(char * topo[MAX_EDGE_NUM], int line_num,Info *fusai); */

/* ****************************       modified      ****************************** */
ga_struct init_key_serevr(graph_init *clean_graph,Info *fusai,int level);
ga_struct init_from_tuihuo(graph_init *clean_graph,Info *fusai,vector<QIFA> &key_server,int &cost);
bool init_population(graph_init *clean_graph,ga_vector &population, ga_vector &buffer,Info *fusai,ga_pop_init key_citizen);
inline void calc_fitness(graph_init *clean_graph,ga_vector &population,Info *fusai);
vector<QIFA> Ga_For_MCF(graph_init *clean_graph,Info *fusai,ga_pop_init key_citizen);
/* **********************************    The end     **************************************** */

#endif
