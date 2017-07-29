#ifndef __fusai_h__
#define __fusai_h__
#include <iostream>
#include <vector>
#include <queue>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
//#include <mcheck.h>
#include "lib/lib_io.h"
#include "sim.h"
using namespace std;

#define MAX_LEVEL 7
#define INIT_RATE 0.8f
#define INF 204748360

typedef struct qifa
{
    int server_id;
    int server_type;
} QIFA;

typedef struct node_level
{
    vector<int> level_for_each_client;              //based on node,stored info for each client
    vector<int> level_for_which_client[MAX_LEVEL];  //based on client,stored info for each level
} NodeLevel;

typedef struct client_level
{
    vector<int> search_by_level[MAX_LEVEL];  //based on client,stored info for each level
} ClientLevel;

typedef struct server_node_service
{
    int client_id;
    int serve_flow;
} ServerNodeService;

typedef struct m_server_node
{
    int flow_out;
    int total_cost;
    ServerNodeService * service;
} ServerNode;

/* typedef struct edge{ */
/* int from,to,cap,flow,cost; */
/* }Edge; */

typedef struct m_server
{
    int server_type;
    int server_cap;
    int server_cost;
} Server;

typedef struct m_node
{
    int node_id;
    int node_server_cost;
    int node_degree;     //in_degree equals out_degree
    double node_rate;    //define the setup order
    NodeLevel * node_level;
} Node;

typedef struct edge
{
    int from,to,cap,flow,cost;
} Edge;

typedef struct client
{
    int client_id;
    int node_id;
    int client_need;
    ClientLevel * client_level;
} Client;

typedef struct info_mesh
{
    int server_type_num;
    int node_num;
    int edge_num;
    int client_num;

    int min_satisfy_num=node_num;


    int real_edge_num;
    int real_node_num;

    int *cus;

    vector<Server> ServerInfo;
    vector<Edge> EdgeInfo;
    vector<int> EdgeCnt[MAX_NODE_NUM];
    vector<Node> NodeInfo;
    vector<Client> ClientInfo;
    // additional info
    int aver_setup_cost;
    int max_setup_cost;
    int min_setup_cost;
    int sum_client_flow;
    int pre_server_num;
    int max_server_cap;

    //MCFS INFO
    vector<server_node> cdn_server;
    vector<client_node> cdn_client;
    vector<bad_node> cdn_lack_node;
    int link_result;
    int total_demand;
    NS_STATUS status;



    /* //spfa INFO */
    /* vector<Edge> edges; */
    /* vector<int> G[MAX_NODE_NUM]; */
    /* int inq[MAX_NODE_NUM]; */
    /* int d[MAX_NODE_NUM]; */
    /* int p[MAX_NODE_NUM]; */
    /* int a[MAX_NODE_NUM]; */

    void topo_init(char * topo[MAX_EDGE_NUM],int line_num)
    {
        int pNum[line_num][4];
        int tmp = 0;
        int k = 0;
        for(int i = 0; i < line_num; i++)
            for(int j = 0; j < (int)strlen(topo[i]); j++)
                if(topo[i][j] == '\r' || topo[i][j] == '\n')
                {
                    pNum[i][k++]= tmp;
                    tmp = 0;
                    k = 0;
                    break;
                }
                else if(topo[i][j] != ' ')
                    tmp = tmp * 10 + (int)(topo[i][j] - 48);
                else
                {
                    pNum[i][k++]= tmp;
                    tmp = 0;
                }

        /* case 0 */
        int net_num = pNum[0][0];
        int edge_num = pNum[0][1];
        int cus_num = pNum[0][2];
        this->node_num = net_num;
        this->edge_num = edge_num;
        this->client_num = cus_num;
        this->real_node_num = net_num + 2;

        /* case 1 */
        int SN = 2;
        do
        {
            Server tmp = {pNum[SN][0],pNum[SN][1],pNum[SN][2]};
            SN ++;
            this->ServerInfo.push_back(tmp);
        }
        while(pNum[SN][0] != 0);
        this->server_type_num = SN - 2;

        /* case 2,3,4 pre */
        int start[5] = {0,2,SN+1,-1,-1};
        int end[5] = {0,SN-1,SN+net_num,-1,-1};
        start[3] = end[2]+2;
        end[3] = start[3] + edge_num - 1;
        start[4] = end[3] + 2;
        end[4] = start[4] + cus_num - 1;
        /* case 2 */
        for(int i = start[2]; i<= end[2]; i++)
        {
            this->NodeInfo.push_back((Node)
            {
                pNum[i][0],pNum[i][1],-1,1,NULL
            });
        }

        /* case 3 */
        for(int i = start[3]; i<= end[3]; i++)
            this->AddEdge(pNum[i][0],pNum[i][1],pNum[i][2],pNum[i][3]);

        /* case 4 */
        this->cus = (int *)malloc(sizeof(int)*this->node_num);
        memset(this->cus,-1,sizeof(int)*this->node_num);
        for(int i = start[4]; i<= end[4]; i++)
        {
            this->ClientInfo.push_back((Client)
            {
                pNum[i][0],pNum[i][1],pNum[i][2],NULL
            });
            this->AddSuperT(pNum[i][1],pNum[i][2]);
            this->cus[pNum[i][1]] = pNum[i][0];
        }

    }
    void level_init()
    {
        vector<Node> :: iterator nd;
        vector<Client> :: iterator cl;
        for(nd = this->NodeInfo.begin(); nd!=this->NodeInfo.end(); nd ++)
        {
            (*nd).node_level = new NodeLevel;
            for(int i = 0; i<this->client_num; i++)
                (*nd).node_level->level_for_each_client.push_back(-1);
        }
        for(cl = this->ClientInfo.begin(); cl!=this->ClientInfo.end(); cl++)
        {
            (*cl).client_level = new ClientLevel;
        }

        vector<int> vld;
        for(int i = 0; i<this->node_num; i++)
            vld.push_back(0);
        queue<int> Q;
        queue<int> Q_next;
        for(int i = 0; i<this->client_num; i++)
        {
            int tid = this->ClientInfo[i].node_id;
            Q.push(tid);
            vld[tid] = 1;
            this->NodeInfo[tid].node_level->level_for_each_client[i] = 0;
            this->NodeInfo[tid].node_level->level_for_which_client[0].push_back(i);
            this->ClientInfo[i].client_level->search_by_level[0].push_back(tid);
            for(int j = 1; j<MAX_LEVEL; j++)
            {
                while(!Q.empty())
                {
                    int cid = Q.front();
                    Q.pop();
                    vector<int> ::iterator it;
                    for(it = this->EdgeCnt[cid].begin(); it!=this->EdgeCnt[cid].end(); it++)
                    {
                        int des_node = this->EdgeInfo[(*it)].to;
                        if(des_node < this->node_num && !vld[des_node])
                        {
                            this->NodeInfo[des_node].node_level->level_for_each_client[i] = j;
                            this->NodeInfo[des_node].node_level->level_for_which_client[j].push_back(i);
                            this->ClientInfo[i].client_level->search_by_level[j].push_back(des_node);
                            Q_next.push(des_node);
                            vld[des_node] = 1;
                        }
                    }
                }
                Q.swap(Q_next);
                /* cout << Q.size()<< endl; */
            }
            /* cout << "The loop num is: "<<i << endl; */
            /* cout << "The node num is: "<<this->ClientInfo[i].node_id << endl; */
            for(int k = 0; k<this->node_num; k++)
                vld[k] = 0;
        }
    }
    void additional_init()
    {
        long long sum_setup = 0;
        int sum_flow = 0;
        vector<Node> ::iterator it1;
        vector<Client> ::iterator it2;
        this->max_setup_cost = 0;
        this->min_setup_cost = INF;
        for(it1 = this->NodeInfo.begin(); it1!=this->NodeInfo.end(); it1++)
        {
            int tmp = (*it1).node_server_cost;
            sum_setup += tmp;
            if(tmp > this->max_setup_cost)
                this->max_setup_cost = tmp;
            if(tmp < this->min_setup_cost)
                this->min_setup_cost = tmp;
        }
        for(it2 = this->ClientInfo.begin(); it2!=this->ClientInfo.end(); it2++)
        {
            sum_flow += (*it2).client_need;
        }
        this->sum_client_flow = sum_flow;
        this->aver_setup_cost = sum_setup/this->node_num;


        vector<Server> ::iterator it3;
        for(it3 = this->ServerInfo.begin(); it3!=this->ServerInfo.end(); it3++)
        {
            int cost = (*it3).server_cap;
            if(this->max_server_cap < cost)
                this->max_server_cap = cost;
        }

        this->pre_server_num = this->sum_client_flow/this->max_server_cap + 1;
        cout << "sum_client_flow" << this->sum_client_flow<< endl;
        cout << "max_server_cap" << max_server_cap<< endl;

        for(int i = 0; i<this->node_num; i++)
        {
            int sz = this->EdgeCnt[i].size();
            this->NodeInfo[i].node_degree  = sz;
        }
    }

    void MCFS_init(MCFS *cdn)
    {
        vector<server_node>().swap(this->cdn_server);
        vector<client_node>().swap(this->cdn_client);
        vector<bad_node>().swap(this->cdn_lack_node);
        this->cdn_server = cdn->server;
        this->cdn_client = cdn->client;
        this->cdn_lack_node = cdn->lack_node;
        this->link_result = cdn->link_result;
        this->status = cdn->status;
        this->total_demand=cdn->total_demand;
    }

    /* void SPFA_init(MCFS * cdn){ */
    /* memset(this->inq,0,sizeof(this->inq)); */
    /* memset(this->d,0,sizeof(this->d)); */
    /* memset(this->p,0,sizeof(this->p)); */
    /* memset(this->a,0,sizeof(this->a)); */

    /* // get the right answer_server */
    /* int size = cdn->G_arc.size(); */
    /* edges.clear(); */
    /* for(int i = 0;i<size;i++){ */
    /* int from = cdn->G_arc[i].from; */
    /* int to = cdn->G_arc[i].to; */
    /* int cap = cdn->G_arc[i].flow; */
    /* int cost = cdn->G_arc[i].cost; */
    /* Edge e = {from,to,cap,0,cost}; */
    /* edges.push_back(e); */
    /* } */
    /* int tsize = this->EdgeCnt[this->node_num + 1].size(); */
    /* int t = this->node_num + 1; */
    /* for(int i = 0;i<tsize;i++){ */
    /* Edge e = this->EdgeInfo[this->EdgeCnt[t][i]^1]; */
    /* edges.push_back(e); */
    /* } */

    /* } */

    void init(char * topo[MAX_EDGE_NUM],int line_num)
    {
        this->topo_init(topo,line_num);
        this->level_init();
        this->additional_init();
    }

    void AddEdge(int from,int to,int cap,int cost)
    {
        EdgeInfo.push_back((Edge)
        {
            from,to,cap,0,cost
        });
        EdgeInfo.push_back((Edge)
        {
            to,from,cap,0,cost
        });
        this->real_edge_num = EdgeInfo.size();
        EdgeCnt[from].push_back(this->real_edge_num-2);
        EdgeCnt[to].push_back(this->real_edge_num-1);
    }

    void AddSuperS(int to)
    {
        EdgeInfo.push_back((Edge)
        {
            this->node_num,to,INF,0,0
        });
        EdgeInfo.push_back((Edge)
        {
            to,this->node_num,-INF,0,0
        });
        this->real_edge_num = EdgeInfo.size();
        EdgeCnt[this->node_num].push_back(this->real_edge_num-2);
        EdgeCnt[to].push_back(this->real_edge_num-1);
    }

    void AddSuperT(int from,int cap)
    {
        EdgeInfo.push_back((Edge)
        {
            from,this->node_num+1,cap,0,0
        });
        EdgeInfo.push_back((Edge)
        {
            this->node_num+1,from,-INF,0,0
        });
        this->real_edge_num = EdgeInfo.size();
        EdgeCnt[from].push_back(this->real_edge_num-2);
        EdgeCnt[this->node_num+1].push_back(this->real_edge_num-1);
    }

    void UpdateNodeRate()   //if big server,+0.1
    {
        int s_size = this->cdn_server.size();
        int item = this->server_type_num - 1;
        for(int i = 0; i<s_size; i++)
        {
            if(this->cdn_server[i].supply > this->ServerInfo[item].server_cap)
            {
                int node = this->cdn_server[i].node_num;
                this->NodeInfo[node].node_rate +=0.1;
            }
            else if(this->cdn_server[i].supply > this->ServerInfo[item-1].server_cap)
            {
                int node = this->cdn_server[i].node_num;
                this->NodeInfo[node].node_rate +=0.05;
            }
        }
    }

    vector<int> search_for_key_server(int level)
    {
        /* *************************       from node aspect       **************************** */
        typedef struct tongji
        {
            int node_id;
            int cnt;
            double rate;
            bool operator<(const tongji b) const
            {
                if(this->rate > b.rate)
                    return true;
                else if(this->rate == b.rate)
                    return this->node_id < b.node_id;
                else
                    return false;
            }
        } tongji;
        int key_server_vld[this->node_num];
        memset(key_server_vld,0,sizeof(key_server_vld));
        vector<tongji> key_serevr;
        vector<int> server_id;

        vector<Client> ::iterator it2;
        vector<int> ::iterator it3;
        for(it2 = this->ClientInfo.begin(); it2!=this->ClientInfo.end(); it2++)
        {
            for(int k = 0; k<level; k++)
            {
                for(it3 = (*it2).client_level->search_by_level[k].begin(); it3!=(*it2).client_level->search_by_level[k].end(); it3++)
                {
                    if(key_server_vld[(*it3)])
                    {
                        key_server_vld[*it3]++;
                    }
                    else
                    {
                        tongji key_server_tmp = {(*it3),1,0};
                        key_server_vld[*it3]++;
                        key_serevr.push_back(key_server_tmp);
                    }
                }
            }
        }

        vector<tongji> ::iterator it4;
        for(it4 = key_serevr.begin(); it4!= key_serevr.end(); it4++)
        {
            (*it4).cnt = key_server_vld[(*it4).node_id];
            double tmp = double(this->aver_setup_cost)/this->NodeInfo[(*it4).node_id].node_server_cost;
            double tmp_node_rate = this->NodeInfo[(*it4).node_id].node_rate;
            (*it4).rate = (*it4).cnt * tmp * tmp_node_rate;
        }

        sort(key_serevr.begin(),key_serevr.end());
        int server_cnt = 0;
        for(it4 = key_serevr.begin(); it4!= key_serevr.end(); it4++)
        {
            if(server_cnt <= this->pre_server_num * INIT_RATE)
            {
                server_id.push_back((*it4).node_id);
            }
            server_cnt ++;
            /* cout << (*it4).node_id << " : "<< (*it4).cnt << " : "<<(*it4).rate<<" : "<<this->NodeInfo[(*it4).node_id].node_server_cost<<endl; */
        }

        return server_id;

        /* *************************       from client aspect       **************************** */
    }

    /* vector<int> check_for_closure(inspire_interface result){ // return which client is not satisfied */

    /* } */

    void modify_server(vector<int> &tmp_server,int level)   // within the range of level,find the minest closure,return final server_id
    {
        int csize = this->cdn_lack_node.size();
        int cnt[this->node_num];
        vector<int> final_server;
        memset(cnt,0,sizeof(cnt));
        int tsize = tmp_server.size();
        for(int i = 0; i<tsize; i++)
        {
            int id = tmp_server[i];
            cnt[id] ++;
        }
        for(int i = 0; i<csize; i++) // how to find the answer
        {
            int client_id = this->cus[this->cdn_lack_node[i].node_num];
            for(int k = 0; k<level; k++)
            {
                vector<int> tmp = this->ClientInfo[client_id].client_level->search_by_level[k];
                int ctsize = tmp.size();
                for(int j = 0; j<ctsize; j++)
                {
                    int id = tmp[j];
                    cnt[id] ++;
                }
            }
        }
        for(int i = 0; i<this->node_num; i++)
        {
            if(cnt[i])
            {
                final_server.push_back(i);
            }
        }
        vector<int>().swap(tmp_server);
        tmp_server = final_server;
    }

    void branch_band()
    {

    }

    int find_closure_server(vector<int> tmp_server,int level)   // within the range of level,find the minest closure,return final server_id
    {
        int csize = this->cdn_lack_node.size();
        int cnt[this->node_num];
        vector<int> final_server;
        memset(cnt,0,sizeof(cnt));
        int tsize = tmp_server.size();
        for(int i = 0; i<tsize; i++)
        {
            int id = tmp_server[i];
            cnt[id] = INF;
        }
        for(int i = 0; i<csize; i++) // how to find the answer
        {
            int client_id = this->cus[this->cdn_lack_node[i].node_num];
            for(int k = 0; k<level; k++)
            {
                vector<int> tmp = this->ClientInfo[client_id].client_level->search_by_level[k];
                int ctsize = tmp.size();
                for(int j = 0; j<ctsize; j++)
                {
                    int id = tmp[j];
                    cnt[id] ++;
                }
            }
        }

        double max = -1;
        int vld = -1;
        for(int i = 0; i<this->node_num; i++)
        {
            if(cnt[i]<INF)
            {
                double qu = cnt[i] * this->NodeInfo[i].node_rate;
                if(qu > max)
                {
                    max = cnt[i];
                    vld = i;
                }
            }
        }

        return vld;
    }
    QIFA find_closure_server_qifa(vector<QIFA> tmp_server,int level)   // within the range of level,find the minest closure,return final server_id
    {
        int csize = this->cdn_lack_node.size();
        int cnt[this->node_num];
        vector<int> final_server;
        memset(cnt,0,sizeof(cnt));
        int tsize = tmp_server.size();
        for(int i = 0; i<tsize; i++)
        {
            int id = tmp_server[i].server_id;
            cnt[id] = INF;
        }
        for(int i = 0; i<csize; i++) // how to find the answer
        {
            int client_id = this->cus[this->cdn_lack_node[i].node_num];
            for(int k = 0; k<level; k++)
            {
                vector<int> tmp = this->ClientInfo[client_id].client_level->search_by_level[k];
                int ctsize = tmp.size();
                for(int j = 0; j<ctsize; j++)
                {
                    int id = tmp[j];
                    cnt[id] ++;
                }
            }
        }

        double max = -1;
        int vld = -1;
        for(int i = 0; i<this->node_num; i++)
        {
            if(cnt[i]<INF)
            {
                double qu = cnt[i] * this->NodeInfo[i].node_rate;
                if(qu > max)
                {
                    max = cnt[i];
                    vld = i;
                }
            }
        }
        int c_size =  this->cdn_lack_node.size();
        int c_cnt = 0;
        for(int i = 0; i<c_size; i++)
        {
            c_cnt += this->cdn_lack_node[i].lack;
        }
        int s_type_out = this->choose_server(c_cnt);
        if(s_type_out == -1)
            s_type_out = this->server_type_num -1;
        QIFA out = {vld,s_type_out};
        return out;
    }

    void level_up(int up_num,int up_level,vector<QIFA> &server)
    {
        typedef struct ModServer
        {
            int server_id;
            double rate;
            bool operator<(const ModServer b) const
            {
                if(this->rate > b.rate)
                    return true;
                else
                    return false;
            }
        } ModServer;

        vector<ModServer> mserver;
        int csize = this->cdn_lack_node.size();
        double cnt[this->node_num];
        int hash[this->node_num];
        int total_lack = 0;

        memset(cnt,0,sizeof(cnt));
        int tsize = server.size();
        for(int i = 0; i<tsize; i++)
        {
            int id = server[i].server_id;
            cnt[id] = INF;
            hash[id] = i;
        }
        for(int i = 0; i<csize; i++)
        {
            total_lack +=this->cdn_lack_node[i].lack;
        }


        //cout << "total_lack :  " << total_lack << endl;

        for(int i = 0; i<csize; i++) // how to find the answer
        {
            int client_id = this->cus[this->cdn_lack_node[i].node_num];
            for(int k = 0; k<4; k++)  //zhengli tiaoshu
            {
                vector<int> tmp = this->ClientInfo[client_id].client_level->search_by_level[k];
                int ctsize = tmp.size();
                for(int j = 0; j<ctsize; j++)
                {
                    int id = tmp[j];
                    cnt[id] += (double)(this->cdn_lack_node[i].lack)/total_lack;
                }
            }
        }
        for(int i =0; i<this->node_num; i++)
        {
            if(cnt[i] >= INF)
            {
                ModServer s = {i,cnt[i] - INF};
                mserver.push_back(s);
            }
        }
        sort(mserver.begin(),mserver.end());
        int msize = mserver.size();
        int k = 0;
        for(int i= 0;i<msize;i++){
            int id = mserver[i].server_id;
            if(server[hash[id]].server_type == this->server_type_num - 1)
            {
                continue;
            }
            k++;
            cout << "test    id :  " << id << "     " << 1000 * mserver[i].rate << endl;;
            int new_level = server[hash[id]].server_type + up_level;
            if(new_level > this->server_type_num -1)
                server[hash[id]].server_type = this->server_type_num -1;
            else
                server[hash[id]].server_type = new_level;
            if(k == up_num)
                break;
        }
    }

    void level_down(int down_num,int down_level,vector<QIFA> &server)
    {
        typedef struct ModServer
        {
            int server_id;
            double rate;
            bool operator<(const ModServer b) const
            {
                if(this->rate > b.rate)
                    return true;
                else
                    return false;
            }
        } ModServer;

        vector<ModServer> mserver;
        int hash[this->node_num];
        double cnt[this->node_num];

        memset(cnt,-1,sizeof(cnt));
        memset(hash,-1,sizeof(hash));
        int cdn_size = cdn_server.size();
        for(int i = 0;i<cdn_size;i++){
           int id = this->cdn_server[i].node_num;
           int supply = this->cdn_server[i].supply;
           int type = this->choose_server(supply);
           cnt[id] = this->ServerInfo[type].server_cap - supply;
        }

        int tsize = server.size();
        for(int i = 0; i<tsize; i++)
        {
            int id = server[i].server_id;
            if(cnt[id] != -1){
                mserver.push_back({id,cnt[id]});
            }
            hash[id] = i;
        }
        sort(mserver.begin(),mserver.end());
        for(int i = 0;i<down_num;i++){
            int id = mserver[0].server_id;
            int s_id = hash[id];
            int new_level = server[s_id].server_type - down_level;
            if(new_level < 0)
                server.erase(server.begin()+s_id);
           else 
               server[s_id].server_type = new_level;
        }
    }

    void modify_finded_server(vector<server_node>server_used,vector<int> &tmp_server,int de_num)
    {

        vector<int>().swap(tmp_server);
        int size = server_used.size();
        cout << size << endl;
        sort(server_used.begin(),server_used.end());
        for(int i = de_num; i<size; i++)
        {
            tmp_server.push_back(server_used[i].node_num);
        }
    }

    int choose_server(int supply)
    {
        int type_id = this->server_type_num - 1;
        for(int i = 0; i<=type_id; i++)
        {
            if(supply <= this->ServerInfo[i].server_cap)
                return i;
        }
        return -1;
    }

    vector<server_in> interface_transfer(vector<server_node> server)
    {
        vector<server_in> interface_server;
        int size = server.size();
        for(int i = 0; i<size; i++)
        {
            struct server_in tmp;
            tmp.server_node_id = server[i].node_num;
            int type_id = choose_server(server[i].supply);
            tmp.server_cap = this->ServerInfo[type_id].server_cap;
            interface_server.push_back(tmp);
        }
        return interface_server;
    }

    int compute_setup_cost(vector<server_node> server)
    {
        int size = server.size();
        int cost = 0;
        for(int i = 0; i<size; i++)
        {
            int type_id = choose_server(server[i].supply);
            if(type_id != -1)
            {
                int setup_cost = this->NodeInfo[server[i].node_num].node_server_cost;
                cost += setup_cost + this->ServerInfo[type_id].server_cost;
            }
            else
                return INF;
        }
        return cost;
    }

    void test()
    {

        /* *************************       test for item     **************************** */
        cout << this->node_num << '\t' << this->edge_num << '\t'<< this->client_num<< endl;
        cout << "aver_setup_cost : " << this->aver_setup_cost<< endl;
        cout << "max_setup_cost : "  << this->max_setup_cost<< endl;
        cout << "min_setup_cost :" << this->min_setup_cost<< endl;
        cout << "sum_client_flow :" << this->sum_client_flow;

        /* *************************       test for edge     **************************** */
        vector<Edge> ::iterator it;
        for(it = this->EdgeInfo.begin(); it != EdgeInfo.end(); it ++)
        {
            cout<<"M  head="<<(*it).from<<" tail="<<(*it).to<<" flow="<<(*it).cap<<" cost="<<(*it).cost<<endl;
            cout<<endl;
        }

        /* *************************       test for node      **************************** */
        vector<Node> ::iterator it2;
        for(it2 = this->NodeInfo.begin(); it2 != this->NodeInfo.end(); it2 ++)
        {
            cout << "The info of node " << (*it2).node_id <<":  "<< endl;
            cout<<"Node_id:  "<<(*it2).node_id << "  Node_cost: "<<(*it2).node_server_cost<<" Mode_degree: " << (*it2).node_rate << endl;
            vector<int> ::iterator it3;
            vector<int> ::iterator it4;
            int i = 0;
            /* for(it3 = (*it2).node_level->level_for_each_client.begin(),i = 0;it3!=(*it2).node_level->level_for_each_client.end();it3++,i++){
                if((*it3) != -1)
                    cout << "client " << i << ": " << (*it3) << endl;
            } */
            cout << "The node level info is: " << endl;
            for(int k = 0; k<MAX_LEVEL; k++)
            {
                cout << "level" << k << ":  "<< endl;
                for(it4 = (*it2).node_level->level_for_which_client[k].begin(),i = 0; it4!=(*it2).node_level->level_for_which_client[k].end(); it4++,i++)
                {
                    cout << (*it4) << "  ";
                }
                cout << endl;
            }
        }
        /* *************************       test for client        **************************** */
        /* vector<Client> ::iterator it5;
        vector<int> ::iterator it6;
        int i = 0;
        for(it5 = this->ClientInfo.begin();it5!=this->ClientInfo.end();it5++){
            cout << "client :" << i ++ << endl;
            for(int k = 0;k<MAX_LEVEL;k++){
                cout << "level :" << k << endl;
                for(it6 = (*it5).client_level->search_by_level[k].begin();it6!=(*it5).client_level->search_by_level[k].end();it6++)
                    cout << (*it6) << "  ";
                cout << endl;
            }
        } */
    }
} Info;


#endif
