#include "sim.h"

void MCFS::fast_init_MCFS_graph(const graph_init* raw_graph,vector<server_in> server)
{
    /**************************************init the node**********************************/
    node_max=raw_graph->G_n;//ԭ���Ļ����ϼ�һ��������,��������λ��
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


    for(int i=0; i<node_max; i++) //��ͨ�����
    {
        tmp_node.balance=raw_graph->Deficit[i];//��ͨ���balance
        tmp_node.init_demand=tmp_node.balance;
        tmp_node.node_num=i;//���
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
    arc_max=raw_graph->G_m;//����ԭ���ı���Ŀ
    super_arc=arc_max;//�����߿�ʼ��λ��Ҳ���Ǳߵ���������0��ʼ
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
			//����ת��Ϊ���ۣ��Ӳ���ɱ�Ҳ��������@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@���㣡����������������������
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
    node_max=raw_graph->G_n;//ԭ���Ļ����ϼ�һ��������,��������λ��
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


    for(int i=0; i<node_max; i++) //��ͨ�����
    {
        tmp_node.balance=raw_graph->Deficit[i];//��ͨ���balance
        tmp_node.init_demand=tmp_node.balance;
        tmp_node.node_num=i;//���
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
    arc_max=raw_graph->G_m;//����ԭ���ı���Ŀ
    super_arc=arc_max;//�����߿�ʼ��λ��Ҳ���Ǳߵ���������0��ʼ
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
			//����ת��Ϊ���ۣ��Ӳ���ɱ�Ҳ��������@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@���㣡����������������������
            //int per_cost=((server_cost)/(server[i].server_cap));
            G_arc[init_super_num+server_num].cost=0;
        }
    }


    /**************************************init the solver**********************************/
    status=NO_SOLUTION;
}




void MCFS::determine_inital_feasible_tree(void)
{
    for(int i=0; i<arc_max; i++) //�����еıߵ�flow��flag
    {
        G_arc[i].flow=0;
        G_arc[i].TLU=L_ARC;
    }
    //���SN��ÿ����������
    arc tmp_arc;
    arc_total=arc_max+node_max;//�ܵı������ǳ�ʼͼ�ı���+�����Ŀ

    for(int i=0; i<node_max; i++) //���һ�������㣬�����Լ�ָ���Լ�
    {
        if(G_node[i].balance>0)
        {
            tmp_arc.from=super_node;//������㵽������
            tmp_arc.to=G_node[i].node_num;//Ҳ������i��Ϊnode�ı��
            tmp_arc.flow=G_node[i].balance;
        }
        else
        {
            tmp_arc.to=super_node;//�ӳ����㵽�����
            tmp_arc.from=G_node[i].node_num;
            tmp_arc.flow=-G_node[i].balance;
        }
        tmp_arc.cost=MAX_COST;
        tmp_arc.TLU=T_ARC;//�����߶���T��
        tmp_arc.cap=Get_Type_Max<int>();
        G_arc.push_back(tmp_arc);
    }


    //����ÿ�����ǰ������̣���ȣ�potential����ȣ�  �⼸����Ϣ
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
    //SN�ĳ�ʼ������
    G_node[node_max].node_num=node_max;//
    G_node[node_max].balance=0;//
    G_node[node_max].pred=NODE_NULL;//root�㣬û��ǰ��
    G_node[node_max].thread=0;//�����0
    G_node[node_max].potential=MAX_COST;//�Լ����Լ��ĵ�λ�����Ŀ��������ֵ
    G_node[node_max].depth=0;//���ڵ�����Ϊ0
    G_node[node_max].super_in_arc_num=ARC_NULL;//�����㵽�Լ��ı�û��

}

int MCFS::Father(int node_num,int arc_num) //������(�����ߵ������ô�죿������)
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
    if(Hotlist_Size<Tmp_Candy_Size)//ѡ��С��
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
    int oldGroupPos = Candy_pointer;//�����ڵ�λ�ñ�������
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
            Candy_pointer=0;//�����ķ�����ѭ������
        //cout<<"Candy_pointer="<<Candy_pointer<<endl;
    }
    while( ( Tmp_Candy_Size < Hotlist_Size ) && ( Candy_pointer != oldGroupPos ) );

    //cout<<"ѡ����ABS_RC�����Ǹ�"<<endl;
    //ѡ����ABS_RC�����Ǹ�
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
    //��������������ǽ�root���������еĵ��depth���£�ͬʱ�Ͽ�root����ǰ������ϵ������root�����������µĵ�ŵ�root��ǰ���ĺ���
    int level=G_node[root].depth;
    int node =root;
    //cout<<endl;
    //cout<<"cut_and_update_subtree:"<<endl;
    //cout<<"root0==";
    //G_node[root].show();
    while((G_node[node].thread!=NODE_NULL)&&(G_node[G_node[node].thread].depth>level))
    {
        //��ROOT�������еĵ����ȸı�
        //cout<<"node1:";
        //G_node[node].show();
        node=G_node[node].thread;
        G_node[node].depth+=depth_delta;
    }
    G_node[root].depth+=depth_delta;//root��depth�ĸ���
    //cout<<"root2==";
    //G_node[root].show();
    //cout<<"node2==";
    //G_node[node].show();
    //����root�����߽ڵ�ĺ�̣�ͬʱ�Ͽ�root������߽ڵ����ϵ:������Ͽ���root��ǰ���޷��ҵ�root������root����ͨ��ǰ���ҵ�ԭ����root��ǰ��
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
    /*������(root��last_node����������ĵ�һ��������һ����)*/
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
    G_node[root].pred=previous_node;//��ʵ�ⲽ�����ã���Ϊ��һ����root��ǰ����ʵû�жϿ�
    G_node[previous_node].thread=root;//��������last_node��previous����node�Ĺ�ϵ�أ�
    G_node[last_node].thread=next_node;
    if(next_node!=NODE_NULL)
    {
        G_node[next_node].pred=last_node;
    }
    //Ϊʲô��last_node ���������п����ŵ�previous���棬Ȼ�����п����ַŻ�ȥ��������
}

void MCFS::update_tree(int enter_arc,int leave_arc,int delta,bool leave_arc_reduce_flow)
{
    int arc_num;
    int k1;
    int k2;
    int root;//�����������������ҵ�root
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
        //��������delta����entering arc
        if(G_arc[enter_arc].from==k1)	//���ͬ������delta������
        {
            G_arc[enter_arc].flow+=delta;
        }
        else	//���򣬼���delta������
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
                if(G_arc[arc_num].from==k2)	//����������෴����Ϊ�ߺ�cycle�ķ����෴
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
    if(enter_arc!=leave_arc) //����K1��K2����T�ϵ�potential
    {
        /*
        ���ȣ�leaving arc��leave����T��Ϊ����������T1��T2
        ͬʱ�������߳�Ϊ��L ���� U (�������ߵ����Ͻ绹���½�)
        ����enter tree ��ɾ��leave treeʹ�������������Ϊ��һ��
        T1�ǰ������ڵ��������p,qҲһ���ֱ���������������
        ����T1��root�Ĵ��ڣ�����T�в���Ҫupdate potential
        ����T2��
        ���k��T1�У�T2�����еĵ����� - ��C pi (k,l)��
        ���L��T1�У�T2�����еĵ����� + ��C pi (k,l)��
        */
        bool leave_arc_bring_flow_to_T2;
        int leave_arc_from=G_arc[leave_arc].from;
        int leave_arc_to=G_arc[leave_arc].to;
        int leave_arc_from_depth=G_node[leave_arc_from].depth;
        int leave_arc_to_depth=G_node[leave_arc_to].depth;
        if(leave_arc_from_depth>leave_arc_to_depth)
        {
            //���Leaving arc �� from depth > to depth
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

    /*������˼����ͨ�������������ҵ���һ����ͬ�����ȣ��Ӷ�ȷ��pivot cycle
    cycle�ķ�������ȷ������enter_arc(k,l), ���(K,L)��L�ߣ���ô��W������ͬ�������෴
    delta��cycle w���ܹ����ӵ�������������������ͬ�������ӵ���u-x,������x,�������blocking arc
    �㷨���ǽ��depth����Ϣ�����ٸ�����
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
                //t�ǵ�ǰ����·�ϵĿ��Ա仯��ֵ
                //delta���ܹ������������Ҳ������Щt�е���Сֵ
                //�����������delta
                //cout<<"t="<<t<<" delta="<<delta<<endl;
                if(t<delta)
                {
                    delta=t;
                    leave_arc=arc_num;
                    leave_arc_reduce_flow=leave;//�����leave arc�Ƿ��ܴ�������
                }
                k1=Father(k1,arc_num);
            }
            else
            {
                arc_num=G_node[k2].super_in_arc_num;
                if(G_arc[arc_num].from==k2)	//����������෴����Ϊ�ߺ�cycle�ķ����෴
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
                if(t<=delta)	//��ȵ����Ҳ�Ž�ȥ�������strong feasible arc�ĸ��ÿ��ѡ����������߷��򣬴Ӷ��㿪ʼ�����һ��blocking arc
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
            //��������delta����entering arc
            if(G_arc[enter_arc].from==k1)	//���ͬ������delta������
            {
                G_arc[enter_arc].flow+=delta;
            }
            else	//���򣬼���delta������
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
                    if(G_arc[arc_num].from==k2)	//����������෴����Ϊ�ߺ�cycle�ķ����෴
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
                //���Leaving arc �� from depth > to depth
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

        if(leave_arc_reduce_flow) //���leaving arc �����������ĵݼ�
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

            //�����Ǹ���������������potential �� depth
            /*****************************************************************************
            h2:leave arc ������Ǹ��㣬ͬʱҲ��T2��root
            k2:leave arc ����ĵ� �� coming root of T2
            t2����һ����node h2 ��K2��path�������߻ᱻ�ı䣬 k2����T2�ĸ��ڵ㡣������

            T2�ᱻreorder��depthҲ�ᱻ���ģ������������������е���ȣ�T2��ÿ�������ȶ��仯��

            ����depth_delta������k2�ϵĵ��depth������/�½���ͬʱ��Ϊk2��T2�����Ƚڵ㣬����T2�����е�node����ȶ�Ҫͬʱ�仯

            *****************************************************************************/
            int depth_delta=(G_node[k1].depth)-(G_node[k2].depth)+1;
            int root = k2 ;//T2��root
            int dad;//what is this???

            /*****************************************************************************
            reorder  T2,����h2->k2��ÿ��path����K2��ʼ��
            ����ÿ���ڵ㣬��ÿ���ڵ��������ԭλ���ƶ����µ�λ�á�
            �ر�ã�����T2���ԣ���K2��T2��root,Ҳ���µ�T2��root����K2��K2�����T2�ᱻ����Ų��T1����(Ҳ����ΪK2���˸��µ�)
            ��һ�������ᱻ�ӵ�k2�����������һ������棨�������������ǲ���˵��ÿ�������μӵ�ǰ���Ǹ���ĺ��棿��
            prviousnode �������µ���������ε������ƶ�������Աߵĵ㡣
            �ʼ��ʱ��previousnode�ǵ�k1��Ҳ����˵T2����k1���ұ�
            k1��K2��entering arc�����㣬�����Ǵ�k1->k2
            h1��h2��leaving arc�����㣬
            *****************************************************************************/
            int previous_node =k1;
            int last_node;
            /*****************************************************************************
            arc1��T�е�entering arc��
            ����h2->k2�����еĵ㣬��Ҫ�ı� entering arc
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
                //�����ǰ�����ĵ���h2����������һ�ε���
                if(root==h2)
                    fine=true;
                dad=Father(root,G_node[root].super_in_arc_num);
                //cout<<"it is dad:"<<endl;
                //G_node[dad].show();
                //��root��������T�϶Ͽ������Ҹ�����������ϵ����е��depth��������������������һ���ڵ�ı��
                last_node=cut_and_update_subtree(root,depth_delta);
                //cout<<"it is last node:"<<endl;
                //G_node[last_node].show();
                //��root�������ŵ���ȷ��λ���ϣ���������ԭ����������
                paste_subtree(root,last_node,previous_node);
                //cout<<"it is last node -1 42----:"<<endl;
                //G_node[last_node-1].show();
                //��һ�Σ�subtree���ᱻŲ���ո��ǿ��������һ������Աߣ�
                previous_node=last_node;
                //???��ʵ�ʵ�ncrease��ͬ
                depth_delta+=2;//��ȼ�2,������ֽ��ͼ����Ϊroot����ĵ���ʵ�Ƿŵ���root������
                arc2=G_node[root].super_in_arc_num;
                G_node[root].super_in_arc_num=arc1;
                arc1=arc2;
                root=dad;//�ҵ�root�����һ����

            }//end of while ���������������������ŵ���ȷ��λ����ȥ

            //�������
            /*****************************************************************************/
            k2=G_arc[enter_arc].to;
            //����T2�ϵ�potential
            int T2_delta=Reduce_Cost(enter_arc);
            //����Ϊ�˴�enter arc�ĸ���ı߿�ʼ����
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
    else  //enter arc==node null�����
    {
        status=QIWEI_FIND;//�ҵ��˿��н�
        // cout<<"status=QIWEI_FIND"<<endl;
        //���������ʣ���super_arc����super_arc+1
        //���������������������ô���ǲ��ɽ�
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
            // cout<<"û�ҵ��������ң�enter����num=";
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


