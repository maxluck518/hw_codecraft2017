#ifndef SIMPLEX_H_INCLUDED
#define SIMPLEX_H_INCLUDED

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
#include "lib/lib_io.h"
#include <iostream>
#include <fstream>
using namespace std;

#define MAX_COST int( 1e7 )
#define MAX_CAP int( 1e7 )
#define NODE_NULL int( 1e7 +100)
#define ARC_NULL int( 1e7 +100)


#define Candidate_num_little 30
#define Candidate_num_medium 50
#define Candidate_num_big 200
#define Hot_list_little 5
#define Hot_list_medium 10
#define Hot_list_big 20
/****************************************************************************************/
struct server_in
{
    int server_node_id=0;
    int server_cap=0;
    int server_cost=0;
    server_in(int a,int b,int c)
    {
        server_node_id=a;
        server_cap=b;
        server_cost=c;
    }
    server_in()
    {
        server_node_id=0;
        server_cap=0;
        server_cost=0;
    }
};




struct server_type
{
    int level;
    int cap;
    int cost;
};

struct server_node;
struct client_node;
struct graph_init
{
    int G_n;
    int G_m;
    vector<int>  CAP;
    vector<int>  Deficit;  //node的收支
    vector<int>	set_cost;//node的set值
    vector<int> From;    //起始点
    vector<int> TO;    // 终止点
    vector<int>  COST;
    vector<int> client_num;
    vector<int> client_demand;
    vector<server_type> server_level;
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
        int client_num_size=client_num.size();
        for(int i=0; i<client_num_size; i++)
        {
            client_num.push_back(a->client_num[i]);
            client_demand.push_back(a->client_demand[i]);
        }

        server_cost = a->server_cost;
    }
    void show()
    {
        cout<<"G_n="<<G_n<<endl;
        cout<<"G_m="<<G_m<<endl;
        cout<<"server cost="<<server_cost<<endl;
        cout<<"arc info********************************************:"<<endl;
        int CAP_size=CAP.size();
        for(int i=0; i<CAP_size; i++)
        {
            cout<<"i="<<i<<"   ";
            cout<<From[i]<<"->"<<TO[i]<<" CAP="<<CAP[i]<<" cost="<<COST[i]<<endl;
        }

        cout<<"node info*******************************************:"<<endl;
        int Deficit_size=Deficit.size();
        for(int i=0; i<Deficit_size; i++)	//
        {
            cout<<"node "<<i<<" demand="<<Deficit[i]<<endl;
        }
        cout<<"client info*******************************************:"<<endl;
        int client_num_size=client_num.size();
        for(int i=0; i<client_num_size; i++)	//
        {
            cout<<"client "<<i<<" = "<<client_num[i]<<endl;
        }
    }

    void show_set_cost()
    {
        cout<<"set cost info*******************************************:"<<endl;
        int set_cost_size=set_cost.size();
        for(int i=0; i<set_cost_size; i++)	//
        {
            cout<<"node "<<i<<" = "<<set_cost[i]<<endl;
        }

    }
};

/* struct inspire_interface */
/* { */
/* vector<int> server_list;	//启发式给定的server list */
/* vector<int> client_list;	//client的编号 */
/* int link_cost;				//MCFS算完的立链路上的cost */
/* int status;					//本次计算完，solver的状态 */
/* vector<int> server_used;	//用到的server */
/* vector<int> server_balance;	//用到的server上的balance */
/* vector<int> client_used;	//对应的client上获得的流量 */
/* }; */

/****************************************************************************************/
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



enum TLU_TYPE
{
    T_ARC=0,
    L_ARC=1,
    U_ARC
};

enum STC_TYPE
{
    S_node=0,
    C_node=1,
    Trans_node
};

enum NS_STATUS
{
    NO_SOLUTION=-1,
    QIWEI_FIND,
    Bukejie
};

struct node
{
    int pred;
    int depth;
    int thread;
    int potential;
    int node_num;
    int balance;//代表这个点的收支状况
    int super_in_arc_num;//代表超级点到这个点的边的编号
    int init_demand;
    int path_vector_index;//代表在path vector中的位
    int lack_flow=0;

    int supply_flow=0;
    int set_cost=0;
    int server_level=-1;//默认不建站，值为-1
    int server_cost=0;
    int server_cap=0;

    void show_node_num()
    {
        cout<<"node:"<<node_num<<endl;
    }
    void show()
    {
        cout<<"node:"<<node_num<<" demand="<<init_demand<<" balance="<<balance<<" potential:"<<potential<<" pred:"<<pred<<" thread:"<<thread<<" depth:"<<depth<<endl;
        //cout<<" super_in_arc_num="<<super_in_arc_num<<endl;
    }
    void show_set()
    {
        cout<<"node:"<<node_num<<" set_cost="<<set_cost<<endl;
    }
};

//xij 就是arc(i,j)上的cost
//其中cost=0 或者 cap的边称为restrict arc
struct arc
{
    int from;
    int to;
    int cap;//边的容量
    int flow;
    int cost;//边上的cost
    TLU_TYPE TLU;//判断此边是T L U
    //int arc_reduced_cost=0;
    void show()
    {


        cout<<(from)<<"->"<<(to)<<" flow="<<flow<<" cap="<<cap<<" cost="<<cost<<" type=";
        if(TLU==L_ARC)
        {
            cout<<"L";
        }
        else if(TLU==U_ARC)
        {
            cout<<"U";
        }
        else
        {
            cout<<"T";
        }
        //cout<<"                                     RC="<<arc_reduced_cost<<endl;
        cout<<endl;
    }
};


struct candidiate //the candidate list
{
    int arc_num;
    int ABS_RC;

    void show()
    {
        cout<<"arc_num="<<arc_num<<" ABS_RC="<<ABS_RC;
    }


};


struct server_node
{
    int node_num;
    int supply;
    bool operator<(const struct server_node b) const
    {
        if(this->supply < b.supply)
            return true;
    }
};

struct client_node
{
    int node_num;
    int demand;
    int lack;
};

struct bad_node
{
    int node_num;
    int lack;
};



struct MCFS
{
    int node_max;//初始图的点的数目
    int arc_max;//初始图的边的数目
    int super_node;//超级节点的开始位置
    int super_arc;//超级边的起始位置
    int arc_total;//总共的边的数目=初始边的数目+点的数目
    int node_total;//总共的点的数目=初始点的数目+1=超级点的位置+1

	int total_demand=0;

    vector<node> G_node;
    vector<arc> G_arc;
    NS_STATUS status;
    int link_result;
    int total_result;
	int back_real_result;

    vector<server_node> server;
    vector<client_node> client;
    vector<bad_node> lack_node;
    vector<int> U_none_zero_arc;
    vector<int> T_none_zero_arc;
    vector<server_type> server_level;
    //候选者列表
    vector<candidiate> candy;
    int Candy_Group_Num;
    int Tmp_Candy_Size;
    int Candy_pointer;//指明当前使用的候选者列表
    int Candy_Size;//外层的大小
    int Hotlist_Size;//内层的大小

    void candy_malloc(void)
    {
        if(arc_max<10000)
        {
            Candy_Size=Candidate_num_little;
            Hotlist_Size = Hot_list_little;
        }
        else if(arc_max>100000)
        {
            Candy_Size = Candidate_num_big;
            Hotlist_Size = Hot_list_big ;
        }
        else
        {
            Candy_Size = Candidate_num_medium;
            Hotlist_Size = Hot_list_medium;
        }
        candidiate tmp_candy;
        tmp_candy.arc_num=ARC_NULL;
        tmp_candy.ABS_RC=0;
        for(int i=0; i<Candy_Size+Hotlist_Size+1; i++) //初始化，分配Candy_Size+Hotlist_Size+1个candy
        {
            candy.push_back(tmp_candy);
        }
    }

    void candy_init(void)
    {
        /* cout<<"arc_max="<<arc_max-1<<endl; */
        //cout<<"arc_max="<<arc_max-node_max+1<<endl;
        //TODO TODO
        Candy_Group_Num=((arc_max-1)/Candy_Size)+1;//将总的边数分在候选者列表中
        //Candy_Group_Num=((arc_max-node_max+1)/Candy_Size)+1;//将总的边数分在候选者列表中
        Candy_pointer=0;//当前candy指针指向第一个
        Tmp_Candy_Size=0;//??????????????????????????????????
    }

    void candy_sort(int can_min,int can_max)
    {
        int left=can_min;
        int right=can_max;
        int cut=candy[(left+right)/2].ABS_RC;
        do
        {
            while(candy[left].ABS_RC>cut)
                left++;
            while(candy[right].ABS_RC<cut)
                right--;
            if(left<right)
            {
                candidiate tmp;
                tmp=candy[left];
                candy[left]=candy[right];
                candy[right]=tmp;
            }
            if(left<=right)
            {
                left++;
                right--;
            }
        }
        while(left<=right);
        if(can_min<right)
        {
            candy_sort(can_min,right);
        }

        if((left<can_max)&&(left<=Hotlist_Size))
        {
            candy_sort(left,can_max);
        }
    }

    void init_MCFS_graph(graph_init* raw_graph);

    void fast_init_MCFS_graph(const graph_init* raw_graph,vector<server_in> server);
    void init_MCFS_graph(const graph_init* raw_graph,vector<server_in> server);
    void determine_inital_feasible_tree(void);

    int Reduce_Cost(int arc_num);
    int Father(int node_num,int arc_num);
    int select_enter_arc(void);
    int cut_and_update_subtree(int root,int depth_delta);
    void paste_subtree(int root,int last_node,int previous_node);
    void add_potential(int root,int delta);

    void client_flag();
    void server_flag();
    void Trans_flag();

    void enter_simplex(int enter_arc);

    void update_tree(int enter_arc,int leave_arc,int delta,bool leave_arc_reduce_flow);

    void simplex(void);

    bool node_is_client(int node_num);

    void get_result()
    {
        link_result=0;
        server.clear();
        lack_node.clear();
        U_none_zero_arc.clear();
        T_none_zero_arc.clear();
        if(status==QIWEI_FIND)
        {

            for(int i=0; i<(super_arc-node_max+1); i++)
            {
            	if(G_arc[i].flow!=0)
                {
					//G_arc[i].show();
                    link_result+=(G_arc[i].cost)*(G_arc[i].flow);
                }
            }
            total_result=link_result;
            back_real_result=link_result;

            for(int i=super_arc-node_max+1; i<super_arc; i++)	//这里急需修改和确认！！！！！！！！！！！！！！！！！！！！！
            {
            	if(G_arc[i].flow!=0)
                {
                	//G_arc[i].show();
                    int to=G_arc[i].to;
                    int flow=G_arc[i].flow;
                    server_node tmp_server;
                    tmp_server.node_num=to;
                    tmp_server.supply=flow;
                    server.push_back(tmp_server);
                    G_node[to].supply_flow=flow;

					server_type tmp;
					judge_equal_level(flow,tmp);
					int level=tmp.level;
					int cost=tmp.cost;
					int cap=tmp.cap;
					int node_set=G_node[to].set_cost;
					int total_cost=cost+node_set;
					G_node[to].server_level=level;
					G_node[to].server_cost=cost;
					G_node[to].server_cap=cap;

					G_arc[i].cap=cap;
					int per_cost=total_cost/cap;
					G_arc[i].cost=per_cost;

					back_real_result+=node_set+cost;

                }
            }
            /* cout<<"solve："<<endl; */
            /* cout<<"link cost="<<link_result<<endl; */
            //return link_result;
        }
        else
        {
            //for(int i=0; i<(super_arc-node_max); i++)
            for(int i=0; i<(super_arc-node_max+1); i++)
            {
                if(G_arc[i].flow!=0)
                {
                    link_result+=(G_arc[i].cost)*(G_arc[i].flow);
                }
            }
            for(int i=super_arc-node_max+1; i<super_arc; i++)
            {
                if(G_arc[i].flow!=0)
                {
                    //G_arc[i].show();
                    int to=G_arc[i].to;
                    int flow=G_arc[i].flow;
                    server_node tmp_server;
                    tmp_server.node_num=to;
                    tmp_server.supply=flow;
                    server.push_back(tmp_server);
                    G_node[to].supply_flow=flow;

					server_type tmp;
					judge_equal_level(flow,tmp);
					int level=tmp.level;
					int cost=tmp.cost;
					int cap=tmp.cap;
					int node_set=G_node[to].set_cost;
					int total_cost=cost+node_set;
					G_node[to].server_level=level;
					G_node[to].server_cost=cost;
					G_node[to].server_cap=cap;

					G_arc[i].cap=cap;
					int per_cost=total_cost/cap;
					G_arc[i].cost=per_cost;

					back_real_result+=node_set+cost;
                }
            }

            //获取 bad_node and lack flow
            /* cout<<endl; */
            /* cout<<endl; */

            for(int i=super_arc; i<arc_total; i++)
            {
                //G_arc[i].show();
                if(G_arc[i].flow!=0)
                {
                    //int from=G_arc[i].from;
                    int to=G_arc[i].to;
                    if(to!=super_node)
                    {
                        int lack_flow=G_arc[i].flow;
                        /* cout<<"bad node is : "<<to<<" and lack flow = "<<lack_flow<<endl; */
                        bad_node tmp_bad_node;
                        tmp_bad_node.node_num=to;
                        tmp_bad_node.lack=lack_flow;
                        lack_node.push_back(tmp_bad_node);
                        G_node[to].lack_flow=lack_flow;
                    }
                }
            }
            /* cout<<"can not solve:"<<endl; */

            /* cout<<"link cost="<<link_result<<endl; */
            //return link_result;
        }
    }


    bool judge_big_level(int supply,server_type &tmp)
    {
        if(supply>server_level.back().cap)
        {
            return false;
        }
        if(supply==server_level.back().cap)
        {
        	tmp=server_level.back();
            return true;
        }
        int server_level_size=server_level.size();
        for(int i=0; i<server_level_size; i++)
        {
            if(server_level[i].cap>supply)
            {
                tmp=server_level[i];
                break;
            }
        }
        return true;
    }


    bool judge_equal_level(int supply,server_type &tmp)
    {
        if(supply>server_level.back().cap)
        {
            return false;
        }
        int server_level_size=server_level.size();
        for(int i=0; i<server_level_size; i++)
        {
            if(server_level[i].cap>=supply)
            {
                tmp=server_level[i];
                return true;
                break;
            }
        }
        return true;
    }



    void get_path();


    MCFS() {}
    ~MCFS()
    {
        G_node.clear();
        G_arc.clear();
        candy.clear();
        server.clear();
        client.clear();
        server_level.clear();
    }

    void server_show()
    {
        cout<<"MCFS server info*******************************************:"<<endl;
        int server_size=server.size();
        for(int i=0; i<server_size; i++)
        {
            cout<<"server "<<i<<" :"<<server[i].node_num<<" and supply = "<<server[i].supply<<endl;
            //G_node[server[i].node_num].show();
        }
    }


    void show()
    {
        cout<<"MCFS info********************************************:"<<endl;
        cout<<"node max="<<node_max<<endl;
        cout<<"arc_max="<<arc_max<<endl;
        cout<<"super_node="<<super_node<<endl;
        cout<<"super_arc="<<super_arc<<endl;
        cout<<"arc_total="<<arc_total<<endl;
        cout<<"node_total="<<node_total<<endl;
        cout<<"G_arc.size="<<G_arc.size()<<endl;
        cout<<"G_node.size="<<G_node.size()<<endl;
        cout<<"status="<<status<<endl;

        cout<<"MCFS arc info********************************************:"<<endl;
        int G_arc_size=G_arc.size();
        for(int i=0; i<G_arc_size; i++)
        {
            if(i==(arc_max-node_max+1))
            {
                cout<<"super server:"<<endl;
            }
            if(i==super_arc)
            {
                cout<<"dummy arc:"<<endl;
            }
            //cout<<"i="<<i<<" ";
            G_arc[i].show();
        }


        cout<<"MCFS node info*******************************************:"<<endl;
        int G_node_size=G_node.size();
        for(int i=0; i<G_node_size; i++)	//
        {
            if(i==super_node)
            {
                cout<<"this is dummy node:"<<endl;
            }
            //cout<<"i="<<i<<" ";
            G_node[i].show();
            int S_arc_num=G_node[i].super_in_arc_num;
            if(S_arc_num!=ARC_NULL)
                G_arc[S_arc_num].show();
        }
        cout<<"MCFS server info*******************************************:"<<endl;
        int server_size=server.size();
        for(int i=0; i<server_size; i++)
        {
            cout<<"server "<<i<<" :"<<server[i].node_num<<" and supply = "<<server[i].supply<<endl;
            G_node[server[i].node_num].show();
        }
        cout<<"MCFS client info*******************************************:"<<endl;
        int client_size=client.size();
        for(int i=0; i<client_size; i++)
        {
            cout<<"client "<<i<<" :"<<client[i].node_num<<" and demand = "<<client[i].demand<<endl;
            G_node[client[i].node_num].show();
        }
        cout<<"MCFS lack info*******************************************:"<<endl;
        int lack_node_size=lack_node.size();
        for(int i=0; i<lack_node_size; i++)
        {
            cout<<"lack node:"<<lack_node[i].node_num<<" and lack flow = "<<lack_node[i].lack<<endl;
            G_node[lack_node[i].node_num].show();
        }

    }

    void set_cost_show()
    {
        cout<<"MCFS set cost info*******************************************:"<<endl;
        int G_arc_size=G_arc.size();
        for(int i=0; i<G_arc_size; i++)
        {
            cout<<"node:"<<G_node[i].node_num<<" and set cost = "<<G_node[i].set_cost<<endl;
        }
    }


    void bad_node_show()
    {
        cout<<"MCFS lack info*******************************************:"<<endl;
        int lack_node_size=lack_node.size();
        for(int i=0; i<lack_node_size; i++)
        {
            cout<<"lack node:"<<lack_node[i].node_num<<" and lack flow = "<<lack_node[i].lack<<endl;
            G_node[lack_node[i].node_num].show();
        }
    }

    void server_level_show()
    {
        cout<<"MCFS server_level info*******************************************:"<<endl;
        int server_level_size=server_level.size();
        for(int i=0; i<server_level_size; i++)
        {
            cout<<server_level[i].level<<" "<<server_level[i].cap<<" "<<server_level[i].cost<<endl;
        }

    }

    void candy_show()
    {
        cout<<"MCFS candy info********************************************:"<<endl;
        cout<<" Candy_Group_Num="<<Candy_Group_Num<<endl;
        cout<<" Tmp_Candy_Size="<<Tmp_Candy_Size<<endl;
        cout<<" Candy_pointer="<<Candy_pointer<<endl;//指明当前使用的候选者列表
        cout<<" Candy_Size="<<Candy_Size<<endl;//外层的大小
        cout<<" Hotlist_Size="<<Hotlist_Size<<endl;//内层的大小
        for(int i=1; i<=Tmp_Candy_Size; i++)
        {
            cout<<"i="<<i<<"  ";
            candy[i].show();
            if(candy[i].arc_num!=ARC_NULL)
            {
                cout<<"                    ";
                G_arc[candy[i].arc_num].show();
            }
            cout<<endl;
        }
    }


};


#endif // SIMPLEX_H_INCLUDED
