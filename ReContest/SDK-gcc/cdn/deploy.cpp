#include "deploy.h"
#include <stdio.h>

/*
 * tuihuo   :   inline
 *
 * tuihuo_test2     :    outline
 */

#define YICHUAN_INIT 1 //1 with tuihuo ,2 without tuihuo
#define YICHUAN_INIT_NUM 4 //choose num put in yichuan,max is 4
#define MAGIC_LEVEL 4	//2有时比3好 	1是最好的！！ 不能动，就选1！！

#define TIME_LIMIT_INLINE 10
#define TIME_LIMIT_TUIHUO 12
#define TIME_LIMIT_YICHUAN 2

#define ADD_UP_RATE 0.5

double T =  1;       // 初始温度
double countl=0;

int GA_TARSIZE = 0;
int is_first_calcu = 1;

int confirm_key_server(graph_init *clean_graph,Info *fusai,vector<QIFA> &key_server,int level)
{
    int cost = 0;
    vector<QIFA>().swap(key_server);
    vector<int> server_id = fusai->search_for_key_server(level);
    vector<int> min_server = ZLSolver(clean_graph,fusai,server_id,cost);


    precision result = BACK_Solver(clean_graph,min_server,fusai);
    int size = fusai->cdn_server.size();
    for(int i = 0; i < size; i++)
    {
        int id = fusai->cdn_server[i].node_num;
        int type = fusai->choose_server(fusai->cdn_server[i].supply);
        QIFA s = {id,type};
        key_server.push_back(s);
    }
    cout << key_server.size() << endl;
    cout << "ns link_cost : " << result.link_cost << endl;
    cout << "ns none_zero_total_cost : " << result.none_zero_total_cost<< endl;

    return result.none_zero_total_cost;
}

int inline_tuihuo(char * topo[MAX_EDGE_NUM], int line_num,vector<QIFA> &inline_server)
{
    time_t begin=time(NULL);
    time_t end;
    graph_init* clean_graph=new graph_init;
    Fast_ReadData(topo,line_num,clean_graph);
    Info *fusai = new Info;
    fusai->init(topo,line_num);

    int min_cost = INF;
    int cost;
    vector<int> server;
    vector<int> min_server;
    vector<int> server_id = fusai->search_for_key_server(4);
    server = ZLSolver(clean_graph,fusai,server_id,cost);
    for(int i = 0; i<5; i++)  // inline : loop num
    {
        cost = tuihuo(clean_graph,fusai,server,begin); // type 1

        if(min_cost > cost)
        {
            min_cost = cost;
            vector<int>().swap(min_server);
            min_server = server;  // type 1
        }
        server = min_server;
        end = time(NULL);
        if(end - begin > TIME_LIMIT_INLINE)
            break;
    }
    cout << "*****************************" << endl;
    cout << "The min cost is :       ";
    cout << min_cost << endl;
    cout << "*****************************" << endl;

    // for output
    precision result2 = BACK_Solver(clean_graph,min_server,fusai);
    int sz = fusai->cdn_server.size();
    for(int i = 0;i<sz;i++){
        int id = fusai->cdn_server[i].node_num;
        int supply = fusai->cdn_server[i].supply;
        QIFA qf = {id,fusai->choose_server(supply)};
        inline_server.push_back(qf);
    }
    cost = result2.none_zero_total_cost;
    
    delete fusai;
    delete clean_graph;
    cout << "inline_tuihuo over!" << endl;
    cout << endl;
    return cost;
}
void outline_tuihuo(graph_init *clean_graph,Info *fusai,vector<QIFA> &min_server,int &min_cost){
    //tuihuo 
    int cost = INF;
    vector<QIFA> key_server = min_server;
    time_t begin,end;
    begin = time(NULL);
    for(int i = 0; i<10; i++) 
    {
        tuihuo_test2(clean_graph,fusai,key_server,cost,begin);
        if(cost < min_cost)
        {
            min_cost = cost;
            vector<QIFA>().swap(min_server);
            min_server = key_server;
        }
        else
        {
            cost = min_cost;
            vector<QIFA>().swap(key_server);
            key_server = min_server;
        }

        end = time(NULL);
        if(end - begin > TIME_LIMIT_TUIHUO)
            break;
    }
}

void loop_test(vector<QIFA> server){
    int sz = server.size();
    cout << "min_server test:" << endl;
    for(int i = 0;i<sz;i++){
        cout<< "id : " << server[i].server_id<< "    " << "type:   " << server[i].server_type << endl;
    }
}

void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    /***************************FAST****************************/
    int min_cost;
    vector<QIFA> inline_server;
    srand(unsigned(time(NULL)));

    graph_init* clean_graph=new graph_init;
    Fast_ReadData(topo,line_num,clean_graph);
    Info *fusai = new Info;
    fusai->init(topo,line_num);
    char * topo_file = new char[MAX_EDGE_NUM]();

    /* *********************       new_tuihuo      ********************* */
    vector<QIFA> key_server;
    vector<QIFA> min_server;
    
    // tuihuo 
    // min_cost = inline_tuihuo(topo,line_num,inline_server);
    // min_server = inline_server;
    // outline_tuihuo(clean_graph,fusai,min_server,min_cost);
    // cout << "test" << "   " << min_cost << endl;

    //yichuan
    ga_pop_init key_citizen;
    ga_pop_init key_citizen2;
    for(int i = 0;i<YICHUAN_INIT_NUM;i++){
        key_citizen.cost[i] = inline_tuihuo(topo,line_num,inline_server);
        key_citizen.pop_init[i] = inline_server;
        inline_server.clear();
        cout << "inline init_cost " << i << "   " << key_citizen.cost[i] << endl;
    }

    key_server = min_server;   


    // init key citizen
    GA_TARSIZE = fusai->node_num;
    min_server = Ga_For_MCF(clean_graph,fusai,key_citizen);

    // min_server:last minest QIFA
    // 

    int cost; 
    MCMF * cdn = new MCMF;
    MCFS * final = new MCFS;
    cdn->topo_init(topo,line_num);
    qifa_sim_out(clean_graph,fusai,min_server,final);
	cout<<	"MCFS-LINK result="<<final->link_result<<endl;
	cout<<  "MCFS->final result="<<final->back_real_result<<endl;
    cout << "link_cost final:  " << fusai->link_result << endl;

    cdn->key_edge_init(final);
    cost = FinalMincost(cdn->s,cdn->t,cdn,topo_file);


    cout << "*****************************" << endl;
    cout << cost << endl;
    cout << "*****************************" << endl;



    delete final;
    /* *********************       new_tuihuo      ********************* */
    write_result(topo_file, filename);

    delete clean_graph;
    delete fusai;
}

bool Feasible_judge(graph_init* clean_graph,vector<int> server_id)
{
    MCFS* cdn=new MCFS;
    int max_server_cap=clean_graph->server_level.back().cap;
    server_in tmp;
    vector<server_in> server;
    tmp.server_cap=max_server_cap;
    tmp.server_cost=0;
    int server_id_size=server_id.size();
    for(int i=0; i<server_id_size; i++)
    {
        tmp.server_node_id=server_id[i];
        server.push_back(tmp);
    }

    cdn->fast_init_MCFS_graph(clean_graph,server);
    cdn->determine_inital_feasible_tree();
    cdn->candy_malloc();
    cdn->candy_init();
    cdn->simplex();
    cdn->get_result();

    if(cdn->status==QIWEI_FIND)
    {
        // cout<<"server info*************the rude way:"<<endl;
        int total_cost=0;
        int server_id_size=server_id.size();
        for(int i=0; i<server_id_size; i++)
        {
            int server_num=server_id[i];
            int server_supply=cdn->G_node[server_num].supply_flow;
            server_type tmp;
            if(cdn->judge_equal_level(server_supply,tmp))
            {
                // cout<<"node="<<server_num<<" supply="<<server_supply<<" new cap="<<tmp.cap<<" new cost="<<tmp.cost<<" new level="<<tmp.level<<endl;
                total_cost+=tmp.cost;
                total_cost+=cdn->G_node[server_num].set_cost;
            }
        }


        // cout<<"link_result="<<cdn->link_result<<endl;
        // cout<<"total_cost="<<total_cost+cdn->link_result<<endl;
        // cout<<"************************************************"<<endl;


        delete cdn;
        return true;
    }
    else
    {
        delete cdn;
        return false;
    }

}

void Better_solution(MCFS* cdn,vector<server_in> &test_server,vector<int> &server)
{
    int server_size_better=server.size();
    for(int i=0; i<server_size_better; i++)
    {
        int server_num=server[i];
        int server_supply=cdn->G_node[server_num].supply_flow;
        server_type tmp;
        if(cdn->judge_equal_level(server_supply,tmp))
        {
            // cout<<"node="<<server_num<<" supply="<<server_supply<<" new cap="<<tmp.cap<<" new cost="<<tmp.cost<<" new level="<<tmp.level<<endl;
            test_server[i].server_node_id=server_num;
            test_server[i].server_cap=tmp.cap;
            test_server[i].server_cost=tmp.cost;
        }
    }
}




precision BACK_Solver(graph_init* clean_graph, vector<int> &server,Info *fusai)
{

    int demand_total=-clean_graph->Deficit[clean_graph->G_n-1];//必须为负数


    // cout<<"total demand="<<demand_total<<endl;
    precision FALUT;
    FALUT.status = Bukejie;
    FALUT.link_cost = 0;
    FALUT.all_node_total_cost = INF;
    FALUT.fake_back_total_cost = INF;
    FALUT.none_zero_total_cost = INF;

    //服务器数目太少直接退出
    int server_size=server.size();
    int max_server_cap=clean_graph->server_level.back().cap;
    if(max_server_cap*server_size<demand_total)
    {
        // cout<<"too less server"<<endl;
        return FALUT;
    }
    //假设这些点都是最大点，如果不能成功直接返回
    bool can_solve=false;
    can_solve=Feasible_judge(clean_graph,server);

    if(!can_solve)
    {
        // cout<<"even the biggest server can not solve"<<endl;
        return FALUT;
    }

    double avg_flow=demand_total/server.size();
    // cout<<"avg_flow="<<avg_flow<<endl;
    server_type avg_server;
    bool has=false;
    int clean_graph_server_level_size=clean_graph->server_level.size();
    for(int i=0; i<clean_graph_server_level_size; i++)
    {
        if(clean_graph->server_level[i].cap>avg_flow)
        {
            avg_server=clean_graph->server_level[i];
            has=true;
            break;
        }
    }
    if(!has)
    {
        return FALUT;
    }

    int avg_cap=avg_server.cap;
    int total_cost=avg_server.cost;


    int avg_cost=total_cost/avg_cap;//平均价格，事实上应该直接传总价格进去，在里面算的平均价格


    int avg_server_level=avg_server.level;
    // cout<<"avg_cap="<<avg_cap<<endl;
    // cout<<"avg_cost="<<avg_cost<<endl;
    // cout<<"avg_level="<<avg_server_level<<endl;

    vector<server_in> test_server;
    server_in tmp;
    //avg_cap=10000;//待删！！！！！！！！！！！！！！！！！！！！！
    tmp.server_cap=avg_cap;
    tmp.server_cost=total_cost;	//这里还是传总价格进去，在里面会算一次平均价格！！
    int server_size_i=server.size();
    for(int i=0; i<server_size_i; i++)
    {
        tmp.server_node_id=server[i];
        test_server.push_back(tmp);
    }



    MCFS* cdn=new MCFS;
    cdn->fast_init_MCFS_graph(clean_graph,test_server);
    cdn->determine_inital_feasible_tree();
    cdn->candy_malloc();
    cdn->candy_init();
    cdn->simplex();
    cdn->get_result();

    bool avg_test=false;
    if(cdn->status==QIWEI_FIND)
        avg_test=true;
    // cout<<"avg_test="<<avg_test<<endl;
    if(avg_test) //如果一次测试直接成功，稍加优化
    {
        Better_solution(cdn,test_server,server);
        MCFS* final_cdn=new MCFS;
        final_cdn->fast_init_MCFS_graph(clean_graph,test_server);
        final_cdn->determine_inital_feasible_tree();
        final_cdn->candy_malloc();
        final_cdn->candy_init();
        final_cdn->simplex();
        final_cdn->get_result();



        Better_solution(final_cdn,test_server,server);
        int total_cost=0;
        int none_zero_cost=0;
        vector<int>().swap(server);
        int test_server_size=test_server.size();
        for(int i=0; i<test_server_size; i++)
        {
            int server_num=test_server[i].server_node_id;
            int set_cost=final_cdn->G_node[server_num].set_cost;
            int server_supply=final_cdn->G_node[server_num].supply_flow;
            total_cost+=test_server[i].server_cost;
            total_cost+=set_cost;
            if(server_supply>0)
            {
                none_zero_cost+=test_server[i].server_cost;
                none_zero_cost+=set_cost;
                server.push_back(server_num);
            }
            else
            {
                //cout<<"server "<<server_num<<" do not supply and be delete"<<endl;
                //final_cdn->G_node[server_num].show();
            }
        }
        int link_result=final_cdn->link_result;
        total_cost+=link_result;
        int fack_total_result=final_cdn->total_result;
        none_zero_cost+=link_result;

        // cout<<"link_result="<<link_result<<endl;
        // cout<<"total_cost="<<total_cost<<endl;
        // cout<<"fake-back="<<fack_total_result<<endl;
        // cout<<"none-zero-result="<<none_zero_cost<<endl;
        precision result;

        result.status=QIWEI_FIND;
        result.link_cost=link_result;
        result.all_node_total_cost=total_cost;
        result.fake_back_total_cost=fack_total_result;
        result.none_zero_total_cost=none_zero_cost;
        if(none_zero_cost!=final_cdn->back_real_result)
        {
            cout<<"error!!!: none_zero_cost!=final_cdn->back_real_result"<<endl;
        }
        fusai->MCFS_init(final_cdn);




        //cout<<"this back solver result="<<final_cdn->back_real_result<<endl;
        delete final_cdn;
        delete cdn;



        return result;///////////////////////
        //better完直接return
    }
    else  //平均测试失败
    {
        // cout<<"average test false"<<endl;
        //cdn->bad_node_show();
        //cout<<"test change:"<<endl;
        /*
        cout<<"server info:"<<endl;
        for(int i=0;i<server.size();i++){
        	int server_num=server[i];
        	cout<<"id="<<server_num<<" supply="<<cdn->G_node[server_num].supply_flow<<endl;
        }
        */
        int server_size_t=server.size();
        for(int i=0; i<server_size_t; i++)
        {
            int server_num=server[i];
            int server_supply=cdn->G_node[server_num].supply_flow;
            server_type tmp;
            if(cdn->judge_big_level(server_supply,tmp))
            {
                // cout<<"node="<<server_num<<" supply="<<server_supply<<" new cap="<<tmp.cap<<" new cost="<<tmp.cost<<" new level="<<tmp.level<<endl;
                test_server[i].server_node_id=server_num;
                test_server[i].server_cap=tmp.cap;
                test_server[i].server_cost=tmp.cost;
            }
        }
        //现在的test_server就是更新

        //cout<<"check server:"<<endl;
        //cdn->server_show();
    }
    delete cdn;

    //test_server;//这里是现有的一次average没有成功，调整过的server list
    bool fes_find=false;
    int countk=1;
    while(!fes_find)
    {
        countk++;
        MCFS* cdn=new MCFS;
        cdn->fast_init_MCFS_graph(clean_graph,test_server);
        cdn->determine_inital_feasible_tree();
        cdn->candy_malloc();
        cdn->candy_init();
        cdn->simplex();
        cdn->get_result();
        if(cdn->status==QIWEI_FIND)
        {
            //可行的话直接退出
            fes_find=true;
            Better_solution(cdn,test_server,server);
            MCFS* final_cdn=new MCFS;
            final_cdn->fast_init_MCFS_graph(clean_graph,test_server);
            final_cdn->determine_inital_feasible_tree();
            final_cdn->candy_malloc();
            final_cdn->candy_init();
            final_cdn->simplex();
            final_cdn->get_result();

            Better_solution(final_cdn,test_server,server);


            int total_cost=0;
            int none_zero_cost=0;
            vector<int>().swap(server);
            int test_server_size=test_server.size();
            for(int i=0; i<test_server_size; i++)
            {
                int server_num=test_server[i].server_node_id;
                int set_cost=final_cdn->G_node[server_num].set_cost;
                int server_supply=final_cdn->G_node[server_num].supply_flow;
                total_cost+=test_server[i].server_cost;
                total_cost+=set_cost;
                if(server_supply>0)
                {
                    none_zero_cost+=test_server[i].server_cost;
                    none_zero_cost+=set_cost;
                    server.push_back(server_num);
                }
                else
                {
                    //cout<<"server "<<server_num<<" do not supply and be delete"<<endl;
                    //final_cdn->G_node[server_num].show();
                }
            }
            int link_result=final_cdn->link_result;
            total_cost+=link_result;
            int fack_total_result=final_cdn->total_result;
            none_zero_cost+=link_result;

            // cout<<"link_result="<<link_result<<endl;
            // cout<<"total_cost="<<total_cost<<endl;
            // cout<<"fake-back="<<fack_total_result<<endl;
            // cout<<"none-zero-result="<<none_zero_cost<<endl;
            precision result;

            result.status=QIWEI_FIND;
            result.link_cost=link_result;
            result.all_node_total_cost=total_cost;
            result.fake_back_total_cost=fack_total_result;
            result.none_zero_total_cost=none_zero_cost;

            fusai->MCFS_init(final_cdn);
            delete final_cdn;
            delete cdn;




            return result;///////////////////////
            //better完直接return
        }
        else
        {
            //不可行的话继续调整server_level
            // cout<<"times="<<countk<<"   adjuest agein"<<endl;
            int server_size_P=server.size();
            for(int i=0; i<server_size_P; i++)
            {
                int server_num=server[i];
                int server_supply=cdn->G_node[server_num].supply_flow;
                server_type tmp;
                if(cdn->judge_big_level(server_supply,tmp))
                {
                    // cout<<"node="<<server_num<<" supply="<<server_supply<<" new cap="<<tmp.cap<<" new cost="<<tmp.cost<<" new level="<<tmp.level<<endl;
                    test_server[i].server_node_id=server_num;
                    test_server[i].server_cap=tmp.cap;
                    test_server[i].server_cost=tmp.cost;
                }
            }
        }
        delete cdn;
    }

}




precision BACK_Solver_Out(graph_init* clean_graph, vector<int> &server,Info *fusai,MCMF *out)
{
    int demand_total=-clean_graph->Deficit[clean_graph->G_n-1];//必须为负数
    // cout<<"total demand="<<demand_total<<endl;
    precision FALUT;

    //服务器数目太少直接退出
    int server_size=server.size();
    int max_server_cap=clean_graph->server_level.back().cap;
    if(max_server_cap*server_size<demand_total)
    {
        // cout<<"too less server"<<endl;
        return FALUT;
    }
    //假设这些点都是最大点，如果不能成功直接返回
    bool can_solve=false;
    can_solve=Feasible_judge(clean_graph,server);

    if(!can_solve)
    {
        // cout<<"even the biggest server can not solve"<<endl;
        return FALUT;
    }

    double avg_flow=demand_total/server.size();
    // cout<<"avg_flow="<<avg_flow<<endl;
    server_type avg_server;
    bool has=false;
    int clean_graph_server_level=clean_graph->server_level.size();
    for(int i=0; i<clean_graph_server_level; i++)
    {
        if(clean_graph->server_level[i].cap>avg_flow)
        {
            avg_server=clean_graph->server_level[i];
            has=true;
            break;
        }
    }
    if(!has)
    {
        return FALUT;
    }

    int avg_cap=avg_server.cap;
    int total_cost=avg_server.cost;


    int avg_cost=total_cost/avg_cap;//平均价格，事实上应该直接传总价格进去，在里面算的平均价格


    int avg_server_level=avg_server.level;
    // cout<<"avg_cap="<<avg_cap<<endl;
    // cout<<"avg_cost="<<avg_cost<<endl;
    // cout<<"avg_level="<<avg_server_level<<endl;

    vector<server_in> test_server;
    server_in tmp;
    //avg_cap=10000;//待删！！！！！！！！！！！！！！！！！！！！！
    tmp.server_cap=avg_cap;
    tmp.server_cost=total_cost;	//这里还是传总价格进去，在里面会算一次平均价格！！
    int server_size_pi=server.size();
    for(int i=0; i<server_size_pi; i++)
    {
        tmp.server_node_id=server[i];
        test_server.push_back(tmp);
    }



    MCFS* cdn=new MCFS;
    cdn->fast_init_MCFS_graph(clean_graph,test_server);
    cdn->determine_inital_feasible_tree();
    cdn->candy_malloc();
    cdn->candy_init();
    cdn->simplex();
    cdn->get_result();

    bool avg_test=false;
    if(cdn->status==QIWEI_FIND)
        avg_test=true;
    // cout<<"avg_test="<<avg_test<<endl;
    if(avg_test) //如果一次测试直接成功，稍加优化
    {
        Better_solution(cdn,test_server,server);
        MCFS* final_cdn=new MCFS;
        final_cdn->fast_init_MCFS_graph(clean_graph,test_server);
        final_cdn->determine_inital_feasible_tree();
        final_cdn->candy_malloc();
        final_cdn->candy_init();
        final_cdn->simplex();
        final_cdn->get_result();

        Better_solution(final_cdn,test_server,server);


        int total_cost=0;
        int none_zero_cost=0;
        vector<int>().swap(server);
        int test_server_size=test_server.size();
        for(int i=0; i<test_server_size; i++)
        {
            int server_num=test_server[i].server_node_id;
            int set_cost=final_cdn->G_node[server_num].set_cost;
            int server_supply=final_cdn->G_node[server_num].supply_flow;
            total_cost+=test_server[i].server_cost;
            total_cost+=set_cost;
            if(server_supply>0)
            {
                none_zero_cost+=test_server[i].server_cost;
                none_zero_cost+=set_cost;
                server.push_back(server_num);
            }
        }
        int link_result=final_cdn->link_result;
        total_cost+=link_result;
        int fack_total_result=final_cdn->total_result;
        none_zero_cost+=link_result;

        // cout<<"link_result="<<link_result<<endl;
        // cout<<"total_cost="<<total_cost<<endl;
        // cout<<"fake-back="<<fack_total_result<<endl;
        // cout<<"none-zero-result="<<none_zero_cost<<endl;
        precision result;

        result.status=QIWEI_FIND;
        result.link_cost=link_result;
        result.all_node_total_cost=total_cost;
        result.fake_back_total_cost=fack_total_result;
        result.none_zero_total_cost=none_zero_cost;


        out->key_edge_init(final_cdn);
        fusai->MCFS_init(final_cdn);
        delete final_cdn;
        delete cdn;
        return result;///////////////////////
        //better完直接return
        //better完直接return
    }
    else  //平均测试失败
    {
        // cout<<"average test false"<<endl;
        //cdn->bad_node_show();
        // cout<<"test change:"<<endl;
        /*
        // cout<<"server info:"<<endl;
        for(int i=0;i<server.size();i++){
        	int server_num=server[i];
            // cout<<"id="<<server_num<<" supply="<<cdn->G_node[server_num].supply_flow<<endl;
        }
        */
        int server_size_judge=server.size();
        for(int i=0; i<server_size_judge; i++)
        {
            int server_num=server[i];
            int server_supply=cdn->G_node[server_num].supply_flow;
            server_type tmp;
            if(cdn->judge_big_level(server_supply,tmp))
            {
                // cout<<"node="<<server_num<<" supply="<<server_supply<<" new cap="<<tmp.cap<<" new cost="<<tmp.cost<<" new level="<<tmp.level<<endl;
                test_server[i].server_node_id=server_num;
                test_server[i].server_cap=tmp.cap;
                test_server[i].server_cost=tmp.cost;
            }
        }
        //现在的test_server就是更新

        //cout<<"check server:"<<endl;
        //cdn->server_show();
    }
    delete cdn;

    //test_server;//这里是现有的一次average没有成功，调整过的server list
    bool fes_find=false;
    int countk=1;
    while(!fes_find)
    {
        countk++;
        MCFS* cdn=new MCFS;
        cdn->fast_init_MCFS_graph(clean_graph,test_server);
        cdn->determine_inital_feasible_tree();
        cdn->candy_malloc();
        cdn->candy_init();
        cdn->simplex();
        cdn->get_result();
        if(cdn->status==QIWEI_FIND)
        {
            //可行的话直接退出
            fes_find=true;
            Better_solution(cdn,test_server,server);
            MCFS* final_cdn=new MCFS;
            final_cdn->fast_init_MCFS_graph(clean_graph,test_server);
            final_cdn->determine_inital_feasible_tree();
            final_cdn->candy_malloc();
            final_cdn->candy_init();
            final_cdn->simplex();
            final_cdn->get_result();

            Better_solution(final_cdn,test_server,server);


            int total_cost=0;
            int none_zero_cost=0;
            vector<int>().swap(server);
            int test_server_size=test_server.size();
            for(int i=0; i<test_server_size; i++)
            {
                int server_num=test_server[i].server_node_id;
                int set_cost=final_cdn->G_node[server_num].set_cost;
                int server_supply=final_cdn->G_node[server_num].supply_flow;
                total_cost+=test_server[i].server_cost;
                total_cost+=set_cost;
                if(server_supply>0)
                {
                    none_zero_cost+=test_server[i].server_cost;
                    none_zero_cost+=set_cost;
                    server.push_back(server_num);
                }
            }
            int link_result=final_cdn->link_result;
            total_cost+=link_result;
            int fack_total_result=final_cdn->total_result;
            none_zero_cost+=link_result;

            // cout<<"link_result="<<link_result<<endl;
            // cout<<"total_cost="<<total_cost<<endl;
            // cout<<"fake-back="<<fack_total_result<<endl;
            // cout<<"none-zero-result="<<none_zero_cost<<endl;
            precision result;

            result.status=QIWEI_FIND;
            result.link_cost=link_result;
            result.all_node_total_cost=total_cost;
            result.fake_back_total_cost=fack_total_result;
            result.none_zero_total_cost=none_zero_cost;


            out->key_edge_init(final_cdn);
            fusai->MCFS_init(final_cdn);
            delete final_cdn;
            delete cdn;
            return result;///////////////////////
            //better完直接return
        }
        else
        {
            //不可行的话继续调整server_level
            // cout<<"times="<<countk<<"   adjuest agein"<<endl;
            int server_size_again=server.size();
            for(int i=0; i<server_size_again; i++)
            {
                int server_num=server[i];
                int server_supply=cdn->G_node[server_num].supply_flow;
                server_type tmp;
                if(cdn->judge_big_level(server_supply,tmp))
                {
                    // cout<<"node="<<server_num<<" supply="<<server_supply<<" new cap="<<tmp.cap<<" new cost="<<tmp.cost<<" new level="<<tmp.level<<endl;
                    test_server[i].server_node_id=server_num;
                    test_server[i].server_cap=tmp.cap;
                    test_server[i].server_cost=tmp.cost;
                }
            }
        }
        delete cdn;
    }

}   // if chucuo,remember to check






bool fast_use_sim(graph_init* clean_graph,Info * fusai,MCFS * cdn,vector<int> server_id)
{
    //int demand_total=-clean_graph->Deficit[clean_graph->G_n-1];//必须为负数

    vector<server_in> server;
    int size = server_id.size();

    for(int i = 0; i < size; i++)
    {
        int id = server_id[i];
        int s_cap = fusai->ServerInfo[fusai->server_type_num-1].server_cap;
        int s_cost=0;
        struct server_in server_item = {id,s_cap,s_cost};
        server.push_back(server_item);
    }

    cdn->fast_init_MCFS_graph(clean_graph,server);
    cdn->determine_inital_feasible_tree();
    cdn->candy_malloc();
    cdn->candy_init();



    cdn->simplex();
    cdn->get_result();
    fusai->MCFS_init(cdn);
    countl++;

    if(cdn->status == 0)
    {
        fusai->UpdateNodeRate();
        return true;
    }
    else
        return false;
}
bool qifa_sim(graph_init* clean_graph,Info * fusai,vector<QIFA> &server_qifa)
{

    vector<server_in> server;
    MCFS *cdn = new MCFS;
    int size = server_qifa.size();
    //cout << "debug :  " << size << endl;

    for(int i = 0; i < size; i++)
    {
        int id = server_qifa[i].server_id;
        int type = server_qifa[i].server_type;
        int s_cap = fusai->ServerInfo[type].server_cap;
        int s_cost=0;
        struct server_in server_item = {id,s_cap,s_cost};
        server.push_back(server_item);
    }

    cdn->init_MCFS_graph(clean_graph,server);
    cdn->determine_inital_feasible_tree();
    cdn->candy_malloc();
    cdn->candy_init();

    cdn->simplex();
    cdn->get_result();
    fusai->MCFS_init(cdn);


    vector<QIFA>().swap(server_qifa);
    int sz = fusai->cdn_server.size();
    for(int i = 0;i<sz;i++){
        QIFA qifa;
        qifa.server_id = fusai->cdn_server[i].node_num;
        int supply = fusai->cdn_server[i].supply;
        qifa.server_type = fusai->choose_server(supply);
        server_qifa.push_back(qifa);
    }


    if(cdn->status == 0)
    {
        fusai->UpdateNodeRate();
        delete cdn;
        return true;
    }
    else
    {
        delete cdn;
        return false;
    }
}



bool qifa_sim_out(graph_init* clean_graph,Info * fusai,vector<QIFA> server_qifa,MCFS *cdn)
{

    vector<server_in> server;
    int size = server_qifa.size();

    for(int i = 0; i < size; i++)
    {
        int id = server_qifa[i].server_id;
        int type = server_qifa[i].server_type;
        int s_cap = fusai->ServerInfo[type].server_cap;
        int s_cost = fusai->ServerInfo[type].server_cost;	//kenengdaTODODODODODODOODODOODODODO
        s_cost = 0;
        struct server_in server_item = {id,s_cap,s_cost};
        server.push_back(server_item);
    }

    cdn->init_MCFS_graph(clean_graph,server);
    cdn->determine_inital_feasible_tree();
    cdn->candy_malloc();
    cdn->candy_init();


    cdn->simplex();
    cdn->get_result();
    fusai->MCFS_init(cdn);

    //cout << "qiwei total cost :   " << cdn->back_real_result << endl;
    //cout<< " qiwei real link cost::"<<cdn->link_result <<endl;

    if(cdn->status == 0)
    {
        fusai->UpdateNodeRate();
        return true;
    }
    else
    {
        return false;
    }
}

void Fast_ReadData(char * topo[MAX_EDGE_NUM],int line_num,graph_init *graph)
{
    //记得，点读进来之后，从0开始，也就是raw data
    //所以使用的时候点的编号需要是i+1

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


    int SN = 2;
    do
    {
        server_type tmp;
        tmp.cap=pNum[SN][1];
        tmp.cost=pNum[SN][2];
        tmp.level=pNum[SN][0];
        graph->server_level.push_back(tmp);
        SN ++;
    }
    while(pNum[SN][0] != 0);


    int start[5] = {0,2,SN+1,-1,-1};
    int end[5] = {0,SN-1,SN+node_num,-1,-1};
    start[3] = end[2]+2;//arc的情况
    end[3] = start[3] + edge_num - 1;
    start[4] = end[3] + 2;//消费节点begin
    end[4] = start[4] + cus_num - 1;//消费节点end

    for(int i = start[2]; i<= end[2]; i++)
    {
        int set_cost=pNum[i][1];
        graph->set_cost.push_back(set_cost);
        //graph->set_cost.push_back((Node){pNum[i][0],pNum[i][1],-1,1,NULL});
    }




    int arc_begin_num=start[3];
    int arc_end_num=end[3];
    int clinet_begin_num=start[4];
    int client_end_num=end[4];



    /*
    int start[4] = {0,2,4,-1};
    int end[4] = {0,2,-1,-1};
    end[2] = 4 + edge_num;	//边情况end
    start[3] = 5 + edge_num;  //消费节点begin
    end[3] = 5 + edge_num + cus_num;//消费节点end
    */

    graph->G_n=node_num+1;//still need the super supply
    graph->G_m=edge_num*2;//The number of arcs twice the num of edge and the num of server
    graph->server_cost = server_cost;


    for( int i = 0 ; i < graph->G_n ; i++ )           // all deficits are 0
    {
        graph->Deficit.push_back(0);                      // unless otherwise stated
    }

    /************************NODE INFO*******************************/
    int sum_supply=0;
    int connect_node_num=0;
    int node_demand=0;
    int i=0;
    for(i = clinet_begin_num; i <= client_end_num; i++) //消费节点情况
    {
        connect_node_num=pNum[i][1];
        node_demand=pNum[i][2];		//demand
        graph->client_num.push_back(connect_node_num);
        graph->client_demand.push_back(node_demand);

        graph->Deficit[connect_node_num] -= (-node_demand);//点的收支情况入队，点的编号
        sum_supply+=node_demand;
        //cout<<"sum_supply==="<<sum_supply<<endl;
    }

    graph->Deficit[graph->G_n-1]=(-(sum_supply));

    //cout<<"i+1="<<i+1<<" tDfct[i+1]"<<tDfct[i+1]<<endl;
    /*************************ARC INFO*******************************/
    int arc_begin=0;
    int arc_end=0;
    int arc_cap=0;
    int arc_cost=0;


    for(int i = arc_begin_num; i <= arc_end_num; i++)
    {
        arc_begin=pNum[i][0];
        arc_end=pNum[i][1];
        arc_cap=pNum[i][2];
        arc_cost=pNum[i][3];
        graph->From.push_back(arc_begin);
        graph->TO.push_back(arc_end);
        graph->CAP.push_back(arc_cap);
        graph->COST.push_back(arc_cost);

        //add both the direction arc
        graph->From.push_back(arc_end);
        graph->TO.push_back(arc_begin);
        graph->CAP.push_back(arc_cap);
        graph->COST.push_back(arc_cost);
    }


    /*********************************************************************/

    for(int i=0; i<node_num; i++)
    {
        graph->From.push_back(graph->G_n-1);
        graph->TO.push_back(i);
        graph->CAP.push_back(0);//TODO！！！！！！！！！！！！！！
        //这里待定，不知道选择什么样的服务器，所以这里的边的价格为0，后面可能要和启发式互动
        graph->COST.push_back(MAX_COST);//TO Do
    }
    graph->G_m=edge_num*2+graph->G_n-1;//边的总数是纯图的边的数目的两倍+所有的点的数目-1(去掉超级源)

    /*
    int init_super_num=(edge_num*2);
    for(int i=0; i<server.size(); i++)
    {
        int server_num=server[i].server_node_id;
        if(server_num >= 0 && server_num < node_num)
            graph->CAP[init_super_num+server_num]=server[i].server_cap;
    }
    */
}

vector<int> ZLSolver(graph_init* clean_graph,Info * fusai,vector<int> &server_id,int &cost)
{
    bool status = false;
    int cnt = 0;

    while(!status)
    {
        cnt ++;
        MCFS *cdn = new MCFS;
        status = fast_use_sim(clean_graph,fusai,cdn,server_id);
        int setup_cost = fusai->compute_setup_cost(cdn->server);
        int tmp_cost = setup_cost + cdn->link_result;

        // cout << "The server_num is :  " << cdn->server.size()<< endl;
        // cout << "The cost is : " << tmp_cost << endl;
        if(status)
        {
            vector<int> ttt = server_id;
            vector<int>().swap(server_id);
            int max = -1;
            int n_size = fusai->pre_server_num * INIT_RATE;
            vector<server_node> ::iterator st;
            // cout << "The size of server :  " << cdn->server.size() << endl;
            for(int i = 0; i <= n_size; i++)
            {
                int id = i;
                int cnt = 0;
                for(st = cdn->server.begin(); st!=cdn->server.end(); st++)
                {
                    if((*st).supply > max)
                        id = cnt;
                    cnt++;
                }
                server_id.push_back(cdn->server[id].node_num);
                cdn->server.erase(cdn->server.begin() + id);
                max = 0;
            }
            cost = tmp_cost;
            return ttt;
        }

        if(cnt > fusai->pre_server_num * INIT_RATE * 1.5)
        {
            int ln_size = cdn->lack_node.size();
            for(int i = 0; i<ln_size; i++)
            {
                int id = cdn->lack_node[i].node_num;
                server_id.push_back(id);
                sort(server_id.begin(),server_id.end());
            }
        }
        else
        {
            int add = fusai->find_closure_server(server_id,4);
            server_id.push_back(add);
            sort(server_id.begin(),server_id.end());
            // cout << "The new added serevr is : " << add << endl;
            // cout << "The total serevr_num is : " << cdn->server.size()<< endl;
        }

        // cout << "*****************";
        // cout << "      "<< "The end :  " <<  "      ";
        // cout << "*****************" << endl;
        delete cdn;
    }
}

int compute_df2(Info * fusai,int link_cost)
{
    int size = fusai->cdn_lack_node.size();
    int demand = 0;
    for(int i = 0; i<size; i++)
    {
        demand += fusai->cdn_lack_node[i].lack;
    }
    return link_cost * demand * demand;
}

int tuihuo(graph_init *clean_graph,Info * fusai,vector<int> &server,time_t begin)
{
    int cost = INF;
    vector<int> min_server;
    vector<int> top_server;
    int min_cost = INF;
    struct precision result;
    result = BACK_Solver(clean_graph,server,fusai);
    int standard = compute_df2(fusai,result.link_cost);
    T = 5 * INF/3;
    int INF_cnt = 0;
    for(int i=0; i<L_INIT; i++)
    {
        MCFS * cdn = new MCFS;
        vector<int> last_server = server;
        // modify server
        if(cost == INF)
        {
            int add = fusai->find_closure_server(server,4);
            server.push_back(add);
            INF_cnt ++;
            if(INF_cnt >10)
            {


                //qiwei_change
                //server = min_server;
                int top_server_size=top_server.size();
                if(top_server_size>1)
                {
                    server=top_server;
                }
                else
                {
                    server=min_server;
                }

                int re = rand() % server.size();
                server.erase(server.begin() + re);
                INF_cnt = 0;
            }
        }
        else
        {
            vector<int>().swap(top_server);
            top_server = server;
            int re = rand() % server.size();
            server.erase(server.begin() + re);
            INF_cnt = 0;
        }

        struct precision result = BACK_Solver(clean_graph,server,fusai);



        int new_cost = result.none_zero_total_cost;
        if(new_cost!=INF)
        {
            // cout<<"this result.none_zero_total_cost="<<new_cost<<endl;
        }
        int new_standard = compute_df2(fusai,result.link_cost);
        int df2 = new_cost - cost;
        int df1 = new_standard - standard;
        int df = df1 + df2;
        if(df<0)   //接受准则
        {
            cost = cost + df2;
            standard = standard + df1;
        }
        else if(exp(-df/T)>((double)rand()/RAND_MAX))
        {
            cost = cost + df2;
            standard = standard + df1;
        }
        else
        {
            vector<int>().swap(server);
            server = last_server;
        }
        T=T*AT;
        if(T<E)
        {
            //cout << T << endl;
            break;
        }
        if(cost < min_cost)
        {
            vector<int>().swap(min_server);
            min_server = server;
            min_cost = cost;
        }
        //cout << cost << endl;
        delete cdn;

    }

    vector<int>().swap(server);
    server = min_server;
    return min_cost;
}

int choose_level(int &flag,int &loop_cnt,int min_cost,Info* fusai,vector<QIFA> &server)
{
    // flag0: find_closure
    //全20，最好值是417121 387188 383245 史上最好结果，就它了，不改了！
    int a[8] = {10,10,10,10,10,10,25,10};
    // int a[8] = {1,1,1,1,1,1,2,1};
    // int a[8] = {2,2,2,2,2,2,2,2};
    // int a[8] = {3,3,3,3,3,3,6,3};
    // int a[8] = {5,5,5,5,5,5,10,5};

    int total_demand = fusai->total_demand;
    int now_total_supply=0;
    int cdn_server_size=server.size();
    bool esile=false;


    for(int i=0; i<cdn_server_size; i++)
    {
        int type=server[i].server_type;
        int cap=fusai->ServerInfo[type].server_cap;
        now_total_supply+=cap;
    }

    if(now_total_supply<=total_demand)
    {
        flag=0;
        esile=true;
        //cout << "e si l !" << endl;
    }

    if(loop_cnt > a[flag])
    {
        //cout<<"loop_cnt > a[flag]......"<<endl;
        if(flag == 0)
        {
            if(esile)
            {
                flag=0;
                return 0;

            }
            flag = 2;
            // else
            // {
                // int next_flag = rand()%5;
                // flag=next_flag;
                // flag=2;
                // // flag=0;
            // }
        }
        else if(flag == 1)
        {

            flag = 2;
        }
        else if(flag == 2)
        {
            flag = 6;
        }
        else if(flag == 3)
        {
            flag = 5;
        }
        else if(flag == 4)
        {
            flag = 5;
        }
        else if(flag == 5)
        {
            // flag = 2;
            flag = 0;
        }
        else if (flag == 6)
        {
            flag = 3;
        }
        else if (flag == 7)
        {
            flag = 5;
        }
        loop_cnt = 0;
    }
    else
    {
        int total_demand = fusai->total_demand;
        int now_total_supply=0;
        int cdn_server_size=server.size();

        for(int i=0; i<cdn_server_size; i++)
        {
            int type=server[i].server_type;
            int cap=fusai->ServerInfo[type].server_cap;
            now_total_supply+=cap;
        }

        if(now_total_supply<=total_demand)
        {
            flag=0;
        }
    }
    return 1;
}

void modify_server(int flag,vector<QIFA> &server,Info *fusai,queue<int> &q,int cost)
{
    vector<int> ::iterator it;
    if(flag == 0)
    {
        double rate = double(rand())/RAND_MAX;
        //cout <<"rate: "<< rate << endl;
        if(cost == INF)
        {
            if(rate > ADD_UP_RATE)
            {
                QIFA s= fusai->find_closure_server_qifa(server,4);
                server.push_back(s);
            }
            else
            {
                int up_num = 3;
                int up_level = 1;
                fusai->level_up(up_num,up_level,server);
            }
        }
        else
        {
            if(rate > 0.2)
            {
                int re = rand() % server.size();
                server.erase(server.begin() + re);
            }
            else
            {
                int down_num = 4;
                int down_level = 1;
                int re = rand() % server.size();
                server.erase(server.begin() + re);
                // fusai->level_down(down_num,down_level,server);
            }
        }
    }
    else if (flag == 1)
    {
        //删一个，然后其他点增档
        vector<QIFA> ::iterator it;
        for(it = server.begin(); it!=server.end(); it++)
        {
            if ((*it).server_type + MAGIC_LEVEL <= fusai->server_type_num-1)
            {
                (*it).server_type += MAGIC_LEVEL;
            }
            else
            {
                (*it).server_type = fusai->server_type_num-1;
            }
        }
        int r = rand() % server.size();
        server.erase(server.begin()+r);
        // *(server.begin()+r).server_type = -1;
    }
    else if (flag == 2)
    {
        //随机挑一个升档
        int r = rand() % server.size();
        if ((*(server.begin()+r)).server_type + MAGIC_LEVEL <= fusai->server_type_num-1)
        {
            (*(server.begin()+r)).server_type += MAGIC_LEVEL;
        }
        else
        {
            (*(server.begin()+r)).server_type = fusai->server_type_num-1;
        }
    }
    else if(flag==3) //随机挑一个关掉，随机挑一个升挡
    {
        int server_total_num=server.size();
        int closed_num = rand()%server_total_num;
        //cout <<"closed num: "<< closed_num << endl;
        server.erase(server.begin()+closed_num);
        int up_num = rand()%(server_total_num-1);
        int now_type=server[up_num].server_type;

        if(now_type+MAGIC_LEVEL > fusai->server_type_num-1)
        {
            server[up_num].server_type=fusai->server_type_num-1;
        }
        else
        {
            server[up_num].server_type+=MAGIC_LEVEL;
        }
    }
    else if(flag==4)  //随机挑两个关掉，随机挑两个升挡
    {

        int server_total_num=server.size();
        int closed_num1 = rand()%server_total_num;
        server.erase(server.begin() + closed_num1);
        int closed_num2 = rand()%server_total_num-1;
        server.erase(server.begin() + closed_num2);

        int up_num1 = rand()%(server_total_num-2);
        int up_num2 = rand()%(server_total_num-2);

        int now_type1=server[up_num1].server_type;
        if(now_type1+MAGIC_LEVEL > fusai->server_type_num-1)
        {
            server[up_num1].server_type=fusai->server_type_num-1;
        }
        else
        {
            server[up_num1].server_type+=MAGIC_LEVEL;
        }

        int now_type2=server[up_num2].server_type;
        if(now_type2+MAGIC_LEVEL > fusai->server_type_num-1)
        {
            server[up_num2].server_type=fusai->server_type_num-1;
        }
        else
        {
            server[up_num2].server_type+=MAGIC_LEVEL;
        }

    }
    else if(flag == 5)
    {
        //随机找两个server互换
        int size = server.size();
        int re = rand() % size;
        int id1 = server[re].server_id;
        int edge_id = rand() % fusai->EdgeCnt[id1].size();
        int id2 = fusai->EdgeInfo[edge_id].to;
        int i = 0;
        for(i = 0; i<size; i++)
        {
            if(id2 == server[i].server_id)
            {
                int tmp = server[i].server_type;
                server[i].server_type = server[re].server_type;
                server[re].server_type = tmp;
            }
        }
        if(i == size)
        {
            int type = server[re].server_type;
            server.erase(server.begin() + re);
            QIFA qf = {id2,type};
            server.push_back(qf);
        }
    }
    else if (flag == 6)
    {
        int r = rand() % server.size();
        if ((*(server.begin()+r)).server_type - MAGIC_LEVEL >= 0)
        {
            (*(server.begin()+r)).server_type -= MAGIC_LEVEL;
        }
        else
        {
            (*(server.begin()+r)).server_type = 0;
        }
        int r2 = rand() % server.size();
        if ((*(server.begin()+r2)).server_type - MAGIC_LEVEL >= 0)
        {
            (*(server.begin()+r2)).server_type -= MAGIC_LEVEL;
        }
        else
        {
            (*(server.begin()+r2)).server_type = 0;
        }

    }
    else if (flag == 7)
    {
        int re = rand() % server.size();
        int flag1 = 0;
        for (int i=0; i<server.size(); i++)
        {
            if (server[i].server_id == re)
            {
                flag1 = 1;
                break;
            }
        }
        // for(it = server.begin(); it!=server.end(); it++)
        // {
        //     if (((*it).server_id) == re)
        //     {
        //         flag = 1;
        //         break;
        //     }
        // }
        if (flag1 == 0)
        {
            QIFA qf = {re,rand()%fusai->server_type_num};
            server.push_back(qf);


            int r = rand() % server.size();
            if ((*(server.begin()+r)).server_type - MAGIC_LEVEL >= 0)
            {
                (*(server.begin()+r)).server_type -= MAGIC_LEVEL;
            }
            else
            {
                (*(server.begin()+r)).server_type = 0;
            }
        }
    }

}

void tongji_flag(int flag,int df2,int cnt[8])
{
    if(flag == 0 && df2 < 0) cnt[0]++;
    if(flag == 1 && df2 < 0) cnt[1]++;
    if(flag == 2 && df2 < 0) cnt[2]++;
    if(flag == 3 && df2 < 0) cnt[3]++;
    if(flag == 4 && df2 < 0) cnt[4]++;
    if(flag == 5 && df2 < 0) cnt[5]++;
    if(flag == 6 && df2 < 0) cnt[6]++;
    if(flag == 7 && df2 < 0) cnt[7]++;
}


int tuihuo_test2(graph_init *clean_graph,Info * fusai,vector<QIFA> &server,int &min_cost,time_t begin)
{
    int cost = min_cost;
    int new_cost = INF;
    vector<QIFA> min_server = server;
    vector<QIFA> last_server = server;
    vector<QIFA> top_server = server;
    int last_cost = INF;
    int standard = compute_df2(fusai,fusai->link_result);
//[> ******************       modify method        ******************* <]
    int cost_record = 0;
    int loop_cnt = 0;
    int flag = 3;
    queue<int> q;
    int flag_cnt[8] = {0,0,0,0,0,0,0,0};
    int INF_cnt = 0;
    int in_step = 0;
    T = 100;
//[> ******************        modify end          ******************* <]
    for(int i=0; i<L; i++)
    {
        // out loop
        T=T*AT;
        if(T<E)
        {
            cout << T << endl;
            break;
        }
        if(cost < min_cost)
        {
            vector<QIFA>().swap(min_server);
            min_server = server;
            min_cost = cost;
        }


        //记录上次循环的信息
        last_cost = cost;
        vector<QIFA>().swap(last_server);
        last_server = server;

        cost = min_cost; //关键
        server = min_server;
        top_server = min_server;
        //end

        time_t end = time(NULL);
        if(end -begin >TIME_LIMIT_TUIHUO)
            break;

        //in loop
        while(in_step < 100){
            in_step ++;
            loop_cnt++;
            last_server = server;

            //Generate
            int full = choose_level(flag,loop_cnt,min_cost,fusai,server);
            if(INF_cnt>5)
            {
                server = last_server;
                INF_cnt = 0;
                modify_server(flag,server,fusai,q,last_cost);
            }
            else if(!full) //e si l ,if flag =0,must add!
            {
                modify_server(flag,server,fusai,q,INF);
            }
            else
                modify_server(flag,server,fusai,q,new_cost);
            //end

            //evaluate
            qifa_sim(clean_graph,fusai,server);
            // fusai->UpdateNodeRate();
            if(fusai->status == 0)
                new_cost = fusai->link_result + fusai->compute_setup_cost(fusai->cdn_server);
            else
                new_cost = INF;
            int new_standard = compute_df2(fusai,fusai->link_result);
            int df2 = new_cost - cost;
            int df1 = new_standard - standard;
            //end

            tongji_flag(flag,df2,flag_cnt);
            if(df2<0)
            {
                cost = cost + df2;
                standard = new_standard;
                vector<QIFA>().swap(top_server);
                top_server = server;
            }
            else
            {
                vector<QIFA>().swap(server);
                server = top_server;
            }

            if(new_cost == INF)
            {
                INF_cnt ++;
                INF_cnt = 0 ;
            }
            else
            {
                INF_cnt = 0;
            }
        }
        int df = cost - min_cost;
        if(exp(-df/T)>((double)rand()/RAND_MAX) && df!=0)
        {
            vector<QIFA>().swap(server);
            server = top_server;
        }
        in_step = 0;
        //end
    }

    vector<QIFA>().swap(server);
    server = min_server;
    return min_cost;
}



bool BellmanFord(int s,int t,int &flow,int &cost,MCMF *cdn)
{
    for(int i = 0; i < cdn->n; i++) cdn->d[i]=INF;
    memset(cdn->inq,0,sizeof(cdn->inq));
    cdn->d[s] = 0;
    cdn->inq[s] = 1;
    cdn->p[s] = 0;
    cdn->a[s] = INF;

    /* *******************     The min        ******************* */
    LinkQueue Q;
    Q.init_Queue();
    QElemType source = {cdn->s,0};
    Q.en_Queue_Rear(source);
    while(!Q.is_Empty())
    {
        QElemType to = Q.GetTop();

        Q.de_Queue();
        int u = to.NetId;

        cdn->inq[u] = 0;
        int Gsize = (int)cdn->G[u].size();
        for(int i = 0; i < Gsize; i++)
        {
            Edge& e = cdn->edges[cdn->G[u][i]];
            if(e.cap > e.flow && cdn->d[e.to] > cdn->d[u] + e.cost)
            {
                cdn->a[e.to] = min(cdn->a[u],e.cap - e.flow);
                cdn->d[e.to] = cdn->d[u] + e.cost;
                cdn->p[e.to] = cdn->G[u][i];
                if(!cdn->inq[e.to] && cdn->d[e.to] <= cdn->d[t])
                {
                    QElemType q_in;
                    QElemType q_front;
                    q_in.NetId = e.to;
                    q_in.dist = cdn->d[e.to];
                    if(!Q.is_Empty())
                    {
                        q_front = Q.GetTop();
                        if(abs(q_in.dist) > abs(q_front.dist))
                        {
                            Q.en_Queue_Rear(q_in);
                        }
                        else
                        {
                            Q.en_Queue_Top(q_in);
                        }
                    }
                    else
                    {
                        Q.en_Queue_Rear(q_in);
                    }
                    cdn->inq[e.to] = 1;
                }
            }
            if(cdn->d[t] == 0)  break;
        }

        if(cdn->d[t] == 0)  break;
    }

    /* *******************     The end    ******************* */

    if(cdn->d[t] == INF) return false;
    int u = t;
    while(u != s)
    {

        cdn->edges[cdn->p[u]].flow += cdn->a[t];
        cdn->edges[cdn->p[u]^1].flow -= cdn->a[t];
        cdn->edge_check(&cdn->edges[cdn->p[u]]);
        cdn->edge_check(&cdn->edges[cdn->p[u]^1]);
        u = cdn->edges[cdn->p[u]].from;

        /* update the new cost 0 for an fixed server */
        if(cdn->edges[cdn->p[u]].from == s)
        {
            /* update flow and cost in this turn */
            flow += cdn->a[t];
            cost += cdn->d[t] * cdn->a[t];
            /* ***************************************** */
        }
        /* ***************************************** */
    }

    return true;

}

void CompTrace(MCMF *cdn,char *trace)   // for the output of trace
{
    int s = cdn->s;
    int t = cdn->t;
    int u = t;
    stack<int> tmp_trace;
    tmp_trace.push(u);
    while(u != s)
    {
        u = cdn->edges[cdn->p[u]].from;
        tmp_trace.push(u);
    }
    char *str = new char[MAX_NODE_NUM]();
    int record = -1;
    int first = -1;
    while(!tmp_trace.empty())
    {
        u = tmp_trace.top();
        tmp_trace.pop();
        if(u != s && u != t)
        {
            sprintf(str,"%d",u);
            if(record == -1)
            {
                sprintf(trace,"%s%s",trace,str);
                first = u;
            }
            else
            {
                sprintf(trace,"%s%s%s",trace," ",str);
            }
            record = u;
        }
    }
    /* save the customer num. and added flow in this turn. */
    sprintf(str,"%d",cdn->cus[record]);
    sprintf(trace,"%s%s%s",trace," ",str);

    sprintf(str,"%d",cdn->a[t]);
    sprintf(trace,"%s%s%s",trace," ",str);
    int size = cdn->server.size();
    int id = -1;
    for(int i =0; i<size; i++)
    {
        if(first == cdn->server[i].node_num)
        {
            id = cdn->choose_server(cdn->server[i].supply);
        }
    }
    sprintf(str,"%d",id);
    sprintf(trace,"%s%s%s",trace," ",str);

    /* *************************************************** */
    delete[] str;
}

int FinalMincost(int s,int t,MCMF *cdn,char *output)
{
    int flow = 0,cost = 0;
    int n = 0;
    char * ttmp = new char[MAX_EDGE_NUM]();
    char * trace = new char[MAX_NODE_NUM]();
    char * str = new char[MAX_NODE_NUM]();
    while(BellmanFord(s,t,flow,cost,cdn))
    {
        CompTrace(cdn,trace);
        n++;
        sprintf(ttmp,"%s%s%s",ttmp,"\n",trace);
        memset(trace,0,sizeof(*trace));
    }
    /* compute server cost first */
    cout << "spfa link_cost :  " << cost << endl;
    cost += cdn->compute_setup_cost();
    cout << "spfa setup_cost :  " << cdn->compute_setup_cost() << endl;
    /* **************************** */
    /* check the correctness of the flow */
    if(!cdn->FlowCheck(flow))
        cost = INF;
    // cout << "jhdhj"<<cost << endl;
    // cout << n << endl;

    /* ******************************* */
    sprintf(str,"%d",n);
    sprintf(output,"%s%s%s%s",str,"\n",ttmp,"\0");
    delete[] ttmp;
    delete[] trace;
    delete[] str;

    return cost;
}

/* ********************************************************************************** */
/*
 * genetic algorithm defination
 */

ga_struct init_key_serevr(graph_init *clean_graph,Info *fusai,int level){
    ga_struct citizen;
    vector<QIFA> ::iterator it;
    citizen.clearup();

    int cost = confirm_key_server(clean_graph,fusai,citizen.server_id,level);
    citizen.fitness = cost;
    citizen.serverNum = citizen.server_id.size();

    for(int k =0;k<GA_TARSIZE;k++)
        citizen.server += '0'-1;
    for(it = citizen.server_id.begin();it != citizen.server_id.end();it++)
        citizen.server[(*it).server_id] = (*it).server_type + '0';

    cout << "***********************       result 666       ****************************"  << endl;
    cout << "The init_cost is :  " << cost << endl;

    // cout << "*******************" << endl;
    // cout << citizen.server << endl;
    // cout << "*******************" << endl;

    cout << "***********************       result 999       ****************************"  << endl;

    return citizen;
}

ga_struct init_from_tuihuo(graph_init *clean_graph,Info *fusai,vector<QIFA> &key_server,int &cost){
    ga_struct citizen;
    vector<QIFA>::iterator it;
    vector<QIFA> min_server = key_server;
    int min_cost = cost;
    cout << "TMD :" << min_cost << endl;
    outline_tuihuo(clean_graph,fusai,min_server,min_cost);

    citizen.server_id = min_server;
    citizen.serverNum = min_server.size();
    citizen.fitness = min_cost;
    for(int k =0;k<GA_TARSIZE;k++)
        citizen.server += '0'-1;
    for(it = citizen.server_id.begin();it != citizen.server_id.end();it++)
        citizen.server[(*it).server_id] = (*it).server_type + '0';

    key_server = min_server;
    cost = min_cost;

    return citizen;
}

void init_key_citizen(graph_init *clean_graph,Info *fusai,ga_vector &population,ga_pop_init key_citizen){
    if(YICHUAN_INIT == 2){
        population.push_back(init_key_serevr(clean_graph,fusai,2));
        population.push_back(init_key_serevr(clean_graph,fusai,3));
        population.push_back(init_key_serevr(clean_graph,fusai,5));
        population.push_back(init_key_serevr(clean_graph,fusai,4));
    }
    else if(YICHUAN_INIT == 1)
    {

        vector<QIFA> key_server[4];
        ga_struct citizen[4];
        int cost[4];
        for(int i =0;i<YICHUAN_INIT_NUM;i++){
            key_server[i] = key_citizen.pop_init[i];
            cost[i] = key_citizen.cost[i];
            citizen[i] = init_from_tuihuo(clean_graph,fusai,key_server[i],cost[i]);
            cout << cost[i] << "    hahaha   " << endl;
            population.push_back(citizen[i]);
        }
        // population.push_back(init_key_serevr(clean_graph,fusai,5));
        // population.push_back(init_key_serevr(clean_graph,fusai,4));
    }
}

bool init_population(graph_init *clean_graph,ga_vector &population, ga_vector &buffer,Info *fusai,ga_pop_init key_citizen)
{ 
    int tsize = GA_TARSIZE; 

    init_key_citizen(clean_graph,fusai,population,key_citizen);

    cout << "YICHUAN" << endl;
    for(int i = 0;i<4;i++){
        population[i].print_ga();
        cout << "fitness: " << population[i].fitness << endl;
    }
    
    for (int i=4; i<GA_POPSIZE; i++) { 
        ga_struct citizen; 
        citizen.serverNum = 0; 
        citizen.fitness = INF; 
        citizen.server_id.clear(); 

        citizen.server.erase(); 
        for (int j=0; j<tsize; j++){ 
            int init = ((1+rand()%100) > PopInitRate)?'1':'0';
            if(init =='1'){ 
                int type = rand()%fusai->server_type_num;
                citizen.serverNum++; 
                QIFA q = {j,type};
                citizen.server_id.push_back(q); 
                citizen.server += '0' + type;
            } 
            else
                citizen.server += '0'-1;
        } 

        population.push_back(citizen); 
    } 
    buffer.resize(GA_POPSIZE); 
    return true;
} 

//如果相同字符个数大于长度的一半，则不采用，return false
bool hanMing(ga_vector population, ga_struct citizen)
{
    int tsize = GA_TARSIZE;
    int pop_size = population.size();
    for (int i=0;i<pop_size;i++){
        if (hanMingDis(population[i],citizen) > tsize * HanMingDis)
            return false;
    }
    return true;
}

//返回字符相同的个数
int hanMingDis(ga_struct popu_citizen,ga_struct citizen)
{
    int tsize = GA_TARSIZE;
    int count = 0;
    for (int i=0;i<tsize;i++){
        if (popu_citizen.server[i] == citizen.server[i])
            count += 1;
    }
    return count;
}

inline void calc_fitness(graph_init *clean_graph,ga_vector &population,Info *fusai)
{

    int esize = 0;
    int cost = INF;
    // [> *******    elite population need not to calculate conce more     ******* <]
    if(is_first_calcu){
        esize = 0;
        is_first_calcu = 0;
    }
    else
        esize = GA_POPSIZE * GA_ELITRATE;

    // [> ************************************************************************ <]

    for (int i=esize; i<GA_POPSIZE; i++) {
        qifa_sim(clean_graph,fusai,population[i].server_id);
        if(fusai->status == 0)
            cost = fusai->link_result + fusai->compute_setup_cost(fusai->cdn_server);
        else
            cost = INF;

        if(cost == INF)
            population[i].fitness = INF;
        else{
            population[i].fitness = cost;
            population[i].serverNum = population[i].server_id.size();
            // cout <<"link.cost is :             "<< result.link_cost << endl;
            // cout <<"size is :                  "<< result.server_used.size() << endl;
            // cout <<"server_cost is :           "<<  clean_graph->server_cost << endl;
            // cout << "The cost is :             "<<population[i].fitness << endl;
            vector<QIFA> ::iterator it;
            for(int k =0;k<GA_TARSIZE;k++)
                population[i].server[k] = '0'-1;
            for(it = population[i].server_id.begin();it != population[i].server_id.end();it++)
                population[i].server[(*it).server_id] = '0' + (*it).server_type;
        }

    }
}

inline bool fitness_sort(ga_struct x, ga_struct y) {
    return (x.fitness < y.fitness);
}

inline void sort_by_fitness(ga_vector &population){
    sort(population.begin(), population.end(), fitness_sort);
}

void elitism(ga_vector &population, ga_vector &buffer, int esize ){
    for (int i=0; i<esize; i++) {
        buffer[i].serverNum = population[i].serverNum;
        buffer[i].server = population[i].server;
        buffer[i].fitness = population[i].fitness;
        buffer[i].server_id = population[i].server_id;
    }
}

inline void mutate(ga_struct &member){
    int tsize = GA_TARSIZE;
    int ipos = rand() % tsize;
    if(member.server[ipos]!='/') member.server[ipos] = '0' - 2;
    else member.server[ipos] = '/';
}

int SetParent(ga_vector &population){
    int TotalFitness = 0;
    int EachFitnes[GA_POPSIZE/2];
    int cnt;
    for(int i = 0;i < GA_POPSIZE/2;i++){
        EachFitnes[i] = INF/population[i].fitness;
        TotalFitness += EachFitnes[i];
    }
    int dRange = rand() % TotalFitness;
    int dCursor = 0;
    for(cnt = 0;cnt < GA_POPSIZE/2;cnt++){
        dCursor += EachFitnes[cnt];
        if(dCursor > dRange)
            break;
    }
    return cnt;
}

void mate(ga_vector &population, ga_vector &buffer){
    int esize = GA_POPSIZE * GA_ELITRATE;
    int tsize = GA_TARSIZE, spos, i1, i2;
    elitism(population, buffer, esize);

    for (int i=esize; i<GA_POPSIZE; i++) {
        // [> ***********************    seclection   *********************** <]
        i1 = rand() % (esize);
        i2 = rand() % (esize);
        // i1 = SetParent(population);
        // i2 = SetParent(population);
        // cout << i1 << endl;
        // cout << i2 << endl;
        // [> ***********************       end       *********************** <]

        // [> ***********************    crossover    *********************** <]
        spos = rand() % tsize;
        // spos2 = rand() % (tsize-spos1-1);
        buffer[i].server.erase();
        // buffer[i].server = population[i1].server.substr(0, spos1) +
                        // population[i2].server.substr(spos1, spos1 +spos2) +
                        // population[i1].server.substr(spos1+spos2,tsize);
        buffer[i].server = population[i1].server.substr(0, spos) +
                        population[i2].server.substr(spos, tsize - spos);
            
        // buffer[i].server.resize(tsize);
        // for(int k = 0;k<buffer[i].server.size();k++){
            // buffer[i].server[k] = (population[i1].server[k] + population[i2].server[k])/2;
        // }

        // [> ***********************       end       *********************** <]

        if (rand() < GA_MUTATION) mutate(buffer[i]);
        buffer[i].server_id.clear();
        buffer[i].serverNum = 0;

        for (int j = 0; j < tsize; ++j)
            if(buffer[i].server[j]!='/'){
                buffer[i].serverNum++;
                int type = buffer[i].server[j] - '0';
                QIFA q = {j,type};
                buffer[i].server_id.push_back(q);
            }

    }
}

// inline void print_best(ga_vector &gav)
// { cout << "Best: " << gav[0].server << " (" << gav[0].fitness << ")" << endl; }
inline void print_best(ga_vector &gav)
{
    cout << "Best: " << gav[0].serverNum << " (" << gav[0].fitness << ")" << endl; 
    int sz = gav[0].server_id.size();
    for(int i = 0;i<sz;i++){
        cout << gav[0].server_id[i].server_id << "  ";
    }
    cout << endl;
}
// inline void print_best(ga_vector &gav)
// { cout << "Best: " << gav[0].serverNum << " (" << gav[0].fitness << ")" << endl; }

inline void swap(ga_vector *&population, ga_vector *&buffer){
    ga_vector *temp = population; population = buffer; buffer = temp;
}


/* ********************************************************************************** */

/* ***************************   main solver     ************************************ */


vector<QIFA> Ga_For_MCF(graph_init *clean_graph,Info *fusai,ga_pop_init key_citizen){
    time_t begin,end;
    begin = time(NULL);
	ga_vector pop_alpha, pop_beta;
	ga_vector *population, *buffer;
     
    init_population(clean_graph,pop_alpha, pop_beta,fusai,key_citizen);
    population = &pop_alpha;
    buffer = &pop_beta;
    for (int i=0; i<GA_MAXITER; i++) {
        calc_fitness(clean_graph,*population,fusai);		// calculate fitness
        sort_by_fitness(*population);				// sort them
        mate(*population, *buffer);					// mate the population together
        swap(population, buffer);					// swap buffers
        end = time(NULL);
        if(end - begin > TIME_LIMIT_YICHUAN){
            cout << "The time of this turn is :  "<< end - begin << endl;
            cout << "The loop num. is :   " << i << endl;
            print_best(*population);
            break;
        }
        cout << "The time of this turn is :  "<< end - begin << endl;
        cout << "The loop num. is :   " << i << endl;
        print_best(*population);
    }

   /* **************************    print   ************************** */
    cout << "/* **************************    min    ************************** */" << endl;
    print_best(*population);
   /* **************************    end     ************************** */
    is_first_calcu = 1;
    vector<QIFA> shuchu = (*population)[0].server_id;
    return shuchu;
}

/* ********************************************************************************** */


