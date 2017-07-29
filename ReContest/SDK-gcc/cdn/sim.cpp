#include "sim.h"

void MCFS::fast_init_MCFS_graph(const graph_init* raw_graph,vector<server_in> server)
{
    /**************************************init the node**********************************/
    node_max=raw_graph->G_n;//原来的基础上加一个超级点,放在最后的位置
    super_node=node_max;
    node_total=node_max+1;
    node tmp_node;

    total_demand=-raw_graph->Deficit[raw_graph->G_n-1];

	int server_level_size=raw_graph->server_level.size();
    for(int i=0; i<server_level_size; i++)
    {
        server_type tmp;
        tmp.cap=raw_graph->server_level[i].cap;
        tmp.cost=raw_graph->server_level[i].cost;
        tmp.level=raw_graph->server_level[i].level;
        server_level.push_back(tmp);
    }


    for(int i=0; i<node_max; i++) //普通点入队
    {
        tmp_node.balance=raw_graph->Deficit[i];//普通点的balance
        tmp_node.init_demand=tmp_node.balance;
        tmp_node.node_num=i;//编号
        tmp_node.set_cost=raw_graph->set_cost[i];
        G_node.push_back(tmp_node);
    }
    int client_total=(raw_graph->client_num).size();
    for(int i=0; i<client_total; i++)
    {
        client_node tmp_client;
        tmp_client.node_num=raw_graph->client_num[i];
        tmp_client.demand=raw_graph->client_demand[i];
        tmp_client.lack=0;
        client.push_back(tmp_client);
    }
    /**************************************init the arc**********************************/
    arc_max=raw_graph->G_m;//这是原来的边数目
    super_arc=arc_max;//超级边开始的位置也就是边的总数，从0开始
    arc tmp_arc;
    for(int i=0; i<arc_max; i++)
    {
        tmp_arc.from=raw_graph->From[i];
        tmp_arc.to=raw_graph->TO[i];
        tmp_arc.cap=raw_graph->CAP[i];
        tmp_arc.cost=raw_graph->COST[i];
        G_arc.push_back(tmp_arc);
    }


    int init_super_num=(raw_graph->G_m-(raw_graph->G_n-1));
	int server_size=server.size();
    for(int i=0; i<server_size; i++)
    {
        int server_num=server[i].server_node_id;
        if(server_num >= 0 && server_num < raw_graph->G_n-1)
        {
            //graph->CAP[init_super_num+server_num]=server[i].server_cap;
            G_arc[init_super_num+server_num].cap=server[i].server_cap;
			int total_cost=server[i].server_cost;
			int server_cost=total_cost;
			int set_cost=G_node[server_num].set_cost;
			//这里转换为单价，加布点成本也是在这里@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@布点！！！！！！！！！！！！
			total_cost+=set_cost;
            //int per_cost=((server_cost)/(server[i].server_cap));
            int per_cost=((total_cost)/(server[i].server_cap));
            G_arc[init_super_num+server_num].cost=per_cost;
            //G_arc[init_super_num+server_num].cost=0;				////////////////////////////////all  good way
        }
    }


    /**************************************init the solver**********************************/
    status=NO_SOLUTION;
}

void MCFS::init_MCFS_graph(const graph_init* raw_graph,vector<server_in> server)
{
    /**************************************init the node**********************************/
    node_max=raw_graph->G_n;//原来的基础上加一个超级点,放在最后的位置
    super_node=node_max;
    node_total=node_max+1;
    node tmp_node;
	int server_level_size=raw_graph->server_level.size();

	total_demand=-raw_graph->Deficit[raw_graph->G_n-1];

    for(int i=0; i<server_level_size; i++)
    {
        server_type tmp;
        tmp.cap=raw_graph->server_level[i].cap;
        tmp.cost=raw_graph->server_level[i].cost;
        tmp.level=raw_graph->server_level[i].level;
        server_level.push_back(tmp);
    }


    for(int i=0; i<node_max; i++) //普通点入队
    {
        tmp_node.balance=raw_graph->Deficit[i];//普通点的balance
        tmp_node.init_demand=tmp_node.balance;
        tmp_node.node_num=i;//编号
        tmp_node.set_cost=raw_graph->set_cost[i];
        G_node.push_back(tmp_node);
    }
    int client_total=(raw_graph->client_num).size();
    for(int i=0; i<client_total; i++)
    {
        client_node tmp_client;
        tmp_client.node_num=raw_graph->client_num[i];
        tmp_client.demand=raw_graph->client_demand[i];
        tmp_client.lack=0;
        client.push_back(tmp_client);
    }
    /**************************************init the arc**********************************/
    arc_max=raw_graph->G_m;//这是原来的边数目
    super_arc=arc_max;//超级边开始的位置也就是边的总数，从0开始
    arc tmp_arc;
    for(int i=0; i<arc_max; i++)
    {
        tmp_arc.from=raw_graph->From[i];
        tmp_arc.to=raw_graph->TO[i];
        tmp_arc.cap=raw_graph->CAP[i];
        tmp_arc.cost=raw_graph->COST[i];
        G_arc.push_back(tmp_arc);
    }


    int init_super_num=(raw_graph->G_m-(raw_graph->G_n-1));
	int server_size=server.size();
    for(int i=0; i<server_size; i++)
    {
        int server_num=server[i].server_node_id;
        if(server_num >= 0 && server_num < raw_graph->G_n-1)
        {
            //graph->CAP[init_super_num+server_num]=server[i].server_cap;
            G_arc[init_super_num+server_num].cap=server[i].server_cap;
			int total_cost=server[i].server_cost;
			int server_cost=total_cost;
			int set_cost=G_node[server_num].set_cost;
			//这里转换为单价，加布点成本也是在这里@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@布点！！！！！！！！！！！！
            //int per_cost=((server_cost)/(server[i].server_cap));
            G_arc[init_super_num+server_num].cost=0;
        }
    }


    /**************************************init the solver**********************************/
    status=NO_SOLUTION;
}




void MCFS::determine_inital_feasible_tree(void)
{
    for(int i=0; i<arc_max; i++) //将现有的边的flow和flag
    {
        G_arc[i].flow=0;
        G_arc[i].TLU=L_ARC;
    }
    //添加SN到每个点的虚拟边
    arc tmp_arc;
    arc_total=arc_max+node_max;//总的边数就是初始图的边数+点的数目

    for(int i=0; i<node_max; i++) //最后一个超级点，不能自己指向自己
    {
        if(G_node[i].balance>0)
        {
            tmp_arc.from=super_node;//从这个点到超级点
            tmp_arc.to=G_node[i].node_num;//也可以用i作为node的编号
            tmp_arc.flow=G_node[i].balance;
        }
        else
        {
            tmp_arc.to=super_node;//从超级点到这个点
            tmp_arc.from=G_node[i].node_num;
            tmp_arc.flow=-G_node[i].balance;
        }
        tmp_arc.cost=MAX_COST;
        tmp_arc.TLU=T_ARC;//超级边都是T边
        tmp_arc.cap=Get_Type_Max<int>();
        G_arc.push_back(tmp_arc);
    }


    //更新每个点的前驱，后继，深度，potential，深度，  这几个信息
    //???????????????????????????????????????????????????????????
    node SN;
    G_node.push_back(SN);
    for(int i=0; i<node_max; i++)
    {
        G_node[i].pred=G_node[i].node_num-1;
        G_node[i].thread=G_node[i].node_num+1;
        G_node[i].super_in_arc_num=super_arc+i;
        G_node[i].depth=1;
        if(G_node[i].balance>0)
        {
            G_node[i].potential=2*MAX_COST;
        }
        else
        {
            G_node[i].potential=0;
        }
    }
    G_node[0].pred=super_node;
    G_node[node_max-1].thread=NODE_NULL;
    //SN的初始化操作
    G_node[node_max].node_num=node_max;//
    G_node[node_max].balance=0;//
    G_node[node_max].pred=NODE_NULL;//root点，没有前驱
    G_node[node_max].thread=0;//后继是0
    G_node[node_max].potential=MAX_COST;//自己到自己的单位流量的开销是最大值
    G_node[node_max].depth=0;//根节点的深度为0
    G_node[node_max].super_in_arc_num=ARC_NULL;//超级点到自己的边没有

}

int MCFS::Father(int node_num,int arc_num) //点和入边(多个入边的情况怎么办？？？？)
{

    if(arc_num==ARC_NULL)
    {
        return NODE_NULL;
    }
    if(G_arc[arc_num].from==node_num)
    {
        return G_arc[arc_num].to;
    }
    else
    {
        return G_arc[arc_num].from;
    }
}

int MCFS::Reduce_Cost(int arc_num)
{
    int from_num=G_arc[arc_num].from;
    int to_num=G_arc[arc_num].to;

    int arc_cost=G_arc[arc_num].cost;

    int from_potential=G_node[from_num].potential;
    int to_potential=G_node[to_num].potential;

    //cout<<"from num="<<from_num<<" to_num="<<to_num<<" from_pot="<<from_potential<<" to_pot"<<to_potential<<" arc_cost="<<arc_cost<<endl;
    int RC=from_potential-to_potential+arc_cost;
    return RC;
}


int MCFS::select_enter_arc(void)
{
    int next=0;
    int i=0;
    int minimeValue;
    if(Hotlist_Size<Tmp_Candy_Size)//选更小的
    {
        minimeValue=Hotlist_Size;
    }
    else
    {
        minimeValue=Tmp_Candy_Size;
    }

    //cout<<"minimeValue=="<<minimeValue<<endl;

    for(int i=2; i<=minimeValue; i++)
    {
        int arc_num=candy[i].arc_num;
        int RC=Reduce_Cost(arc_num);

        if(((RC<0)&&(G_arc[arc_num].TLU==L_ARC))||((RC>0)&&G_arc[arc_num].TLU==U_ARC))
        {
            next++;
            //cout<<"next="<<next<<"ABS_RC="<<candy[next].ABS_RC<<endl;
            candy[next].arc_num=arc_num;
            candy[next].ABS_RC=abs(RC);
        }
    }
    //cout<<endl;

    Tmp_Candy_Size=next;
    int oldGroupPos = Candy_pointer;//将现在的位置保存下来
    //cout<<"oldGroupPos="<<oldGroupPos<<endl;
    do
    {
        int arc_num;
        //arc_max-1
        //for(arc_num=Candy_pointer; arc_num<arc_max-node_max+1; arc_num+=Candy_Group_Num)
        for(arc_num=Candy_pointer; arc_num<arc_max-1; arc_num+=Candy_Group_Num)
        {
            //G_arc[arc_num].show();
            //cout<<"Reduce_Cost="<<RC<<endl;
            //cout<<endl;
            if(G_arc[arc_num].TLU==L_ARC)
            {
                int RC=Reduce_Cost(arc_num);
                if(RC<0)
                {
                    Tmp_Candy_Size++;
                    candy[Tmp_Candy_Size].arc_num=arc_num;
                    candy[Tmp_Candy_Size].ABS_RC=abs(RC);
                    //cout<<"Tmp_Candy_Size="<<Tmp_Candy_Size<<" ABS_RC="<<candy[Tmp_Candy_Size].ABS_RC<<endl;
                }
            }
            else if(G_arc[arc_num].TLU==U_ARC)
            {
                int RC=Reduce_Cost(arc_num);
                if(RC>0)
                {
                    Tmp_Candy_Size++;
                    candy[Tmp_Candy_Size].arc_num=arc_num;
                    candy[Tmp_Candy_Size].ABS_RC=abs(RC);
                    //cout<<"Tmp_Candy_Size="<<Tmp_Candy_Size<<" ABS_RC="<<candy[Tmp_Candy_Size].ABS_RC<<endl;
                }
            }
            //cout<<"arc_num = "<<arc_num<<"  arc_max-1 = "<<arc_max-1<<endl;
        }
        Candy_pointer++;
        if(Candy_pointer==Candy_Group_Num)
            Candy_pointer=0;//遍历的方法，循环遍历
        //cout<<"Candy_pointer="<<Candy_pointer<<endl;
    }
    while( ( Tmp_Candy_Size < Hotlist_Size ) && ( Candy_pointer != oldGroupPos ) );

    //cout<<"选出最ABS_RC最大的那个"<<endl;
    //选出最ABS_RC最大的那个
    if(Tmp_Candy_Size)
    {
        //sort(candy[1],candy[Tmp_Candy_Size],candy_complare);
        candy_sort(1,Tmp_Candy_Size);
        return candy[1].arc_num;
    }
    else
    {
        // cout<<"no available enter arc"<<endl;
        return ARC_NULL;
    }

}


void MCFS::add_potential(int root,int delta)
{
    int level=G_node[root].depth;
    //cout<<"level="<<level<<endl;
    int node_num=root;
    do
    {
        //cout<<"potentialing888888888888"<<endl;
        //G_node[node_num].show();
        G_node[node_num].potential+=delta;
        node_num=G_node[node_num].thread;
    }
    while((node_num!=NODE_NULL)&&((G_node[node_num].depth)>level));
}


int MCFS::cut_and_update_subtree(int root,int depth_delta)
{
    //这个函数的作用是将root子树上所有的点的depth更新，同时断开root和其前驱的联系，并将root子树的最右下的点放到root的前驱的后面
    int level=G_node[root].depth;
    int node =root;
    //cout<<endl;
    //cout<<"cut_and_update_subtree:"<<endl;
    //cout<<"root0==";
    //G_node[root].show();
    while((G_node[node].thread!=NODE_NULL)&&(G_node[G_node[node].thread].depth>level))
    {
        //将ROOT下面所有的点的深度改变
        //cout<<"node1:";
        //G_node[node].show();
        node=G_node[node].thread;
        G_node[node].depth+=depth_delta;
    }
    G_node[root].depth+=depth_delta;//root上depth的更新
    //cout<<"root2==";
    //G_node[root].show();
    //cout<<"node2==";
    //G_node[node].show();
    //更新root点的左边节点的后继，同时断开root和其左边节点的联系:单方向断开，root的前驱无法找到root，但是root可以通过前驱找到原来的root的前驱
    if(G_node[root].pred!=NODE_NULL)
    {
        int root_pred=G_node[root].pred;
        G_node[root_pred].thread=G_node[node].thread;
        //cout<<"root3==";
        //G_node[root].show();
        //cout<<"node3==";
        //G_node[node].show();
    }
    if(G_node[node].thread!=NODE_NULL)
    {
        int node_thread=G_node[node].thread;
        G_node[node_thread].pred=G_node[root].pred;
        //cout<<"root4==";
        //G_node[root].show();
        //cout<<"node4==";
        //G_node[node].show();
    }
    //cout<<endl;
    return node;
}


void MCFS::paste_subtree(int root,int last_node,int previous_node)
{
    /*将子树(root和last_node是这个子树的第一个点和最后一个点)*/
    /*
    cout<<endl;
    cout<<"paste_subtree"<<endl;
    cout<<"root=";
    G_node[root].show();
    cout<<"last_node=";
    G_node[last_node].show();
    cout<<"previous_node=";
    G_node[previous_node].show();
    cout<<endl;
    */
    int next_node=G_node[previous_node].thread;
    G_node[root].pred=previous_node;//其实这步好像不用，因为上一步中root的前驱其实没有断开
    G_node[previous_node].thread=root;//？？？那last_node和previous——node的关系呢？
    G_node[last_node].thread=next_node;
    if(next_node!=NODE_NULL)
    {
        G_node[next_node].pred=last_node;
    }
    //为什么将last_node 从子树上切开，放到previous下面，然后再切开，又放回去？？？？
}

void MCFS::update_tree(int enter_arc,int leave_arc,int delta,bool leave_arc_reduce_flow)
{
    int arc_num;
    int k1;
    int k2;
    int root;//这是最后从两边向上找到root
    arc tmp_arc=G_arc[enter_arc];

    if(tmp_arc.TLU==U_ARC)
    {
        k1=tmp_arc.to;
        k2=tmp_arc.from;
    }
    else
    {
        k1=tmp_arc.from;
        k2=tmp_arc.to;
    }
    if(delta!=0)
    {
        //首先利用delta更新entering arc
        if(G_arc[enter_arc].from==k1)	//如果同向，增加delta的流量
        {
            G_arc[enter_arc].flow+=delta;
        }
        else	//反向，减少delta的流量
        {
            G_arc[enter_arc].flow-=delta;
        }
        root=k1;
        while(k1!=k2)
        {
            if(G_node[k1].depth > G_node[k2].depth)
            {
                arc_num=G_node[k1].super_in_arc_num;
                if(G_arc[arc_num].from!=k1)
                {
                    G_arc[arc_num].flow+=delta;
                }
                else
                {
                    G_arc[arc_num].flow-=delta;
                }
                k1=Father(k1,arc_num);
            }
            else
            {
                arc_num=G_node[k2].super_in_arc_num;
                if(G_arc[arc_num].from==k2)	//这里和上面相反，因为边和cycle的方向相反
                {
                    G_arc[arc_num].flow+=delta;
                }
                else
                {
                    G_arc[arc_num].flow-=delta;
                }
                k2=Father(k2,arc_num);
            }
        }
        root=k1;
//        cout<<"root="<<root<<endl;
    }
    if(enter_arc!=leave_arc) //利用K1和K2更新T上的potential
    {
        /*
        首先，leaving arc的leave，将T裂为两棵子树：T1和T2
        同时，这条边成为了L 或者 U (看这条边到了上界还是下界)
        加入enter tree ，删掉leave tree使得这颗生成树变为另一颗
        T1是包含根节点的子树，p,q也一定分别在这两棵子树中
        由于T1中root的存在，所以T中不需要update potential
        对于T2：
        如果k在T1中，T2中所有的点增加 - （C pi (k,l)）
        如果L在T1中，T2中所有的点增加 + （C pi (k,l)）
        */
        bool leave_arc_bring_flow_to_T2;
        int leave_arc_from=G_arc[leave_arc].from;
        int leave_arc_to=G_arc[leave_arc].to;
        int leave_arc_from_depth=G_node[leave_arc_from].depth;
        int leave_arc_to_depth=G_node[leave_arc_to].depth;
        if(leave_arc_from_depth>leave_arc_to_depth)
        {
            //如果Leaving arc 的 from depth > to depth
            if(leave_arc_reduce_flow)
            {
                leave_arc_bring_flow_to_T2=true;
            }
            else
            {
                leave_arc_bring_flow_to_T2=false;
            }
        }
        else
        {
            if(leave_arc_reduce_flow)
            {
                leave_arc_bring_flow_to_T2=false;
            }
            else
            {
                leave_arc_bring_flow_to_T2=true;
            }
        }

    }

}

void MCFS::enter_simplex(int enter_arc)
{

    /*基本的思想是通过两个点向上找到第一个共同的祖先，从而确定pivot cycle
    cycle的方向这样确定：设enter_arc(k,l), 如果(K,L)是L边，那么和W方向相同，否则相反
    delta是cycle w上能够增加的流量的最大流量，如果同向，能增加的是u-x,否则是x,这个边是blocking arc
    算法就是结合depth的信息，快速更新树
    */
    int leave_arc=ARC_NULL;
    if(enter_arc!=ARC_NULL)
    {
        int arc_num;
        int k1;
        int k2;
        int t;
        int delta;
        arc tmp_arc=G_arc[enter_arc];
        //cout<<"G_arc[enter_arc].TLU="<<G_arc[enter_arc].TLU<<endl;
        //cout<<"tmp_arc.TLU="<<tmp_arc.TLU<<endl;

        if(tmp_arc.TLU==U_ARC)
        {
            k1=tmp_arc.to;
            k2=tmp_arc.from;
            delta=tmp_arc.flow;
        }
        else
        {
            //cout<<"it is L"<<endl;
            //cout<<"G_arc[enter_arc].flow="<<G_arc[enter_arc].flow<<endl;
            k1=tmp_arc.from;
            k2=tmp_arc.to;
            delta=tmp_arc.cap-tmp_arc.flow;
            //cout<<"delta="<<delta<<endl;
        }
        int mem_k1=k1;
        int mem_k2=k2;
        bool leave_arc_reduce_flow=(Reduce_Cost(enter_arc)>0);
        bool leave;
        //cout<<"k1::::";
        //G_node[k1].show();
        //cout<<"k2::::";
        //G_node[k2].show();
        while(k1!=k2)
        {
            //cout<<"k1!=k2"<<endl;
            if(G_node[k1].depth > G_node[k2].depth)
            {
                arc_num=G_node[k1].super_in_arc_num;
                if(G_arc[arc_num].from!=k1)
                {
                    t=G_arc[arc_num].cap-G_arc[arc_num].flow;
                    leave=false;
                }
                else
                {
                    t=G_arc[arc_num].flow;
                    leave=true;
                }
                //t是当前的链路上的可以变化的值
                //delta是能够增广的流量，也就是这些t中的最小值
                //所以下面更新delta
                //cout<<"t="<<t<<" delta="<<delta<<endl;
                if(t<delta)
                {
                    delta=t;
                    leave_arc=arc_num;
                    leave_arc_reduce_flow=leave;//这代表leave arc是否能带出流量
                }
                k1=Father(k1,arc_num);
            }
            else
            {
                arc_num=G_node[k2].super_in_arc_num;
                if(G_arc[arc_num].from==k2)	//这里和上面相反，因为边和cycle的方向相反
                {
                    t=G_arc[arc_num].cap-G_arc[arc_num].flow;
                    leave=false;
                }
                else
                {
                    t=G_arc[arc_num].flow;
                    leave=true;
                }
                //cout<<"t="<<t<<" delta="<<delta<<endl;
                if(t<=delta)	//想等的情况也放进去，这就是strong feasible arc的概念：每次选择沿着这个边方向，从顶点开始的最后一个blocking arc
                {
                    delta=t;
                    leave_arc=arc_num;
                    leave_arc_reduce_flow=leave;
                }
                k2=Father(k2,arc_num);
            }
            //cout<<"k1::::";
            //G_node[k1].show();
            //cout<<"k2::::";
            //G_node[k2].show();
            //cout<<"arc="<<arc_num<<" leave arc="<<leave_arc<<endl;
            //cout<<"here is the tmp arc*****************************************************************"<<endl;
            //G_arc[arc_num].show();
        }

        //while end : find the apex point and the delta (the minimal t in the cycle)
        if(leave_arc==ARC_NULL)
        {
            // cout<<"leave arc is null, the leave arc is enter arc"<<endl;
            leave_arc=enter_arc;
            //return enter_arc;
        }
        /*
                cout<<"the enter arc:";
                if(enter_arc!=ARC_NULL)
                {
                    G_arc[enter_arc].show();
                }
                else
                {
                    cout<<"NULL"<<endl;
                }
                cout<<"leave arc is :";
                G_arc[leave_arc].show();
        */





        /**********************update the flow**********************************************/

        k1=mem_k1;
        k2=mem_k2;
        //int root=0;
        if(delta!=0)
        {
            //cout<<"delta!=0"<<endl;
            //首先利用delta更新entering arc
            if(G_arc[enter_arc].from==k1)	//如果同向，增加delta的流量
            {
                G_arc[enter_arc].flow+=delta;
            }
            else	//反向，减少delta的流量
            {
                G_arc[enter_arc].flow-=delta;
            }
            while(k1!=k2)
            {
                if(G_node[k1].depth > G_node[k2].depth)
                {
                    arc_num=G_node[k1].super_in_arc_num;
                    if(G_arc[arc_num].from!=k1)
                    {
                        G_arc[arc_num].flow+=delta;
                    }
                    else
                    {
                        G_arc[arc_num].flow-=delta;
                    }
                    k1=Father(k1,arc_num);
                }
                else
                {
                    arc_num=G_node[k2].super_in_arc_num;
                    if(G_arc[arc_num].from==k2)	//这里和上面相反，因为边和cycle的方向相反
                    {
                        G_arc[arc_num].flow+=delta;
                    }
                    else
                    {
                        G_arc[arc_num].flow-=delta;
                    }
                    k2=Father(k2,arc_num);
                }
            }
            //root=k1;
            //cout<<"root="<<root<<endl;
        }
        if(enter_arc!=leave_arc)
        {
            //cout<<"enter_arc!=leave_arc"<<endl;
            bool leave_arc_bring_flow_to_T2;
            int leave_arc_from=G_arc[leave_arc].from;
            int leave_arc_to=G_arc[leave_arc].to;
            int leave_arc_from_depth=G_node[leave_arc_from].depth;
            int leave_arc_to_depth=G_node[leave_arc_to].depth;
            if(leave_arc_from_depth>leave_arc_to_depth)
            {
                //如果Leaving arc 的 from depth > to depth
                if(leave_arc_reduce_flow)
                {
                    leave_arc_bring_flow_to_T2=true;
                }
                else
                {
                    leave_arc_bring_flow_to_T2=false;
                }
            }
            else
            {
                if(leave_arc_reduce_flow)
                {
                    leave_arc_bring_flow_to_T2=false;
                }
                else
                {
                    leave_arc_bring_flow_to_T2=true;
                }
            }

            if(leave_arc_bring_flow_to_T2==(mem_k1==G_arc[enter_arc].from))
            {
                k2=G_arc[enter_arc].from;
                k1=G_arc[enter_arc].to;
            }
            else
            {
                k2=G_arc[enter_arc].to;
                k1=G_arc[enter_arc].from;
            }
        }
//        cout<<"don        :"<<endl;
//        cout<<"k1:::";
//        G_node[k1].show();
//        cout<<"k2:::";
//        G_node[k2].show();
//        cout<<endl;

        if(leave_arc_reduce_flow) //如果leaving arc 带来了流量的递减
        {
            G_arc[leave_arc].TLU=L_ARC;
            //cout<<"G_arc[leave_arc].TLU=L_ARC;"<<endl;
        }
        else
        {
            G_arc[leave_arc].TLU=U_ARC;
            //cout<<"G_arc[leave_arc].TLU=U_ARC;"<<endl;
        }

        if(leave_arc!=enter_arc)
        {
            //cout<<"yezai??"<<endl;
            G_arc[enter_arc].TLU=T_ARC;
            int h1;
            int h2;
            int leave_from_num=G_arc[leave_arc].from;
            int leave_to_num=G_arc[leave_arc].to;
            if(G_node[leave_from_num].depth < G_node[leave_to_num].depth)
            {
                h1=leave_from_num;
                h2=leave_to_num;
            }
            else
            {
                h2=leave_from_num;
                h1=leave_to_num;
            }
            //cout<<"h1=";
            //G_node[h1].show();
            //cout<<"h2=";
            //G_node[h2].show();

            //下面是更新整个树，包括potential 和 depth
            /*****************************************************************************
            h2:leave arc 最深的那个点，同时也是T2的root
            k2:leave arc 最深的点 ， coming root of T2
            t2中有一个从node h2 到K2的path，这条边会被改变， k2会变成T2的根节点。？？？

            T2会被reorder，depth也会被更改，这代表着这个点在树中的深度，T2中每个点的深度都变化了

            变量depth_delta代表着k2上的点的depth的增加/下降，同时因为k2是T2的祖先节点，所以T2上所有的node的深度都要同时变化

            *****************************************************************************/
            int depth_delta=(G_node[k1].depth)-(G_node[k2].depth)+1;
            int root = k2 ;//T2的root
            int dad;//what is this???

            /*****************************************************************************
            reorder  T2,分析h2->k2的每个path，从K2开始：
            对于每个节点，将每个节点的子树从原位置移动到新的位置。
            特别得，对于T2而言，（K2是T2的root,也是新的T2的root），K2和K2下面的T2会被整个挪到T1下面(也就是为K2找了个新爹)
            下一个子树会被加到k2的子树的最后一个点后面（？？？？？？是不是说，每个点依次加到前面那个点的后面？）
            prviousnode 代表着新的子树在这次迭代会移动到点的旁边的点。
            最开始的时候，previousnode是点k1，也就是说T2会在k1的右边
            k1和K2是entering arc的两点，方向是从k1->k2
            h1和h2是leaving arc的两点，
            *****************************************************************************/
            int previous_node =k1;
            int last_node;
            /*****************************************************************************
            arc1是T中的entering arc，
            对于h2->k2中所有的点，需要改变 entering arc
            *****************************************************************************/
            int arc1=enter_arc;
            int arc2;
            bool fine=false;
            //cout<<"h2=";
            //G_node[h2].show();
            while(fine==false)
            {
                //cout<<"fine=false"<<endl;
                //cout<<"root=";
                //G_node[root].show();
                //如果当前分析的点是h2，这就是最后一次迭代
                if(root==h2)
                    fine=true;
                dad=Father(root,G_node[root].super_in_arc_num);
                //cout<<"it is dad:"<<endl;
                //G_node[dad].show();
                //将root的子树从T上断开，并且更新这个子树上的所有点的depth，并返回这个子树的最后一个节点的编号
                last_node=cut_and_update_subtree(root,depth_delta);
                //cout<<"it is last node:"<<endl;
                //G_node[last_node].show();
                //将root的子树放到正确的位置上，将子树和原树连接起来
                paste_subtree(root,last_node,previous_node);
                //cout<<"it is last node -1 42----:"<<endl;
                //G_node[last_node-1].show();
                //下一次，subtree将会被挪到刚刚那棵树的最后一个点的旁边，
                previous_node=last_node;
                //???和实际的ncrease不同
                depth_delta+=2;//深度加2,合理，见纸上图，因为root上面的点其实是放到了root的下面
                arc2=G_node[root].super_in_arc_num;
                G_node[root].super_in_arc_num=arc1;
                arc1=arc2;
                root=dad;//找到root上面的一个点

            }//end of while 更新整个树，并将子树放到正确的位置上去

            //更新完成
            /*****************************************************************************/
            k2=G_arc[enter_arc].to;
            //更新T2上的potential
            int T2_delta=Reduce_Cost(enter_arc);
            //这是为了从enter arc的更深的边开始更新
            if((G_node[G_arc[enter_arc].from].depth)>(G_node[G_arc[enter_arc].to].depth))
            {
                T2_delta=-T2_delta;
                k2=G_arc[enter_arc].from;
            }
            //cout<<"T2_delta=="<<T2_delta<<endl;
            add_potential(k2,T2_delta);
            //cout<<"after potential^^^^^^^^^^^^^^^^^^^^^^update the tree ^^^^^^^^^^^"<<endl;
            //int enter_from=G_arc[enter_arc].from;
            //int enter_to=G_arc[enter_arc].to;
            //G_arc[enter_arc].show();
            //G_node[enter_from].show();
            //G_node[enter_to].show();
            //show();
        }
    }
    else  //enter arc==node null的情况
    {
        status=QIWEI_FIND;//找到了可行解
        // cout<<"status=QIWEI_FIND"<<endl;
        //这里有疑问，是super_arc还是super_arc+1
        //如果超级边上有流量，那么就是不可解
        for(int i=super_arc; i<arc_total; i++)
        {
            if(G_arc[i].flow>0)
            {
                status=Bukejie;
            }
        }
    }
}






void MCFS::simplex(void)
{
    int max_inter=1;
    //show();
    while(status==NO_SOLUTION)
    {
        //cout<<"get enter arc"<<endl;
        int enter_arc=select_enter_arc();
        //cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ candy 2 time= "<<max_inter<<endl;
        enter_simplex(enter_arc);


        if(status==Bukejie)
        {
            // cout<<"********Bukejie"<<endl;
            //get_result();
            break;
        }
        if(status==NO_SOLUTION)
        {
            // cout<<"没找到，继续找，enter——num=";
            // if(enter_arc!=ARC_NULL)
            // {
            // cout<<enter_arc<<endl;
            // }
        }
        if(max_inter>10000)
        {
            cout<<"max inter times"<<max_inter<<endl;
            break;
        }
        // cout<<"max inter="<<max_inter<<endl;
        max_inter++;
    }
    if(status==QIWEI_FIND)
    {
        // cout<<"find it"<<endl;
        //get_result();//important!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
}


