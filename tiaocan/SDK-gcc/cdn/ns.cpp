#include "ns.h"
#include "deploy.h"

void MCFS::Add_Super_Arc(vector<int> server_list,graph_init* graph)
{
    int server_num=server_list.capacity();
    /* cout<<"afaewfasfdwqaefaf server_num="<<server_num<<endl; */
    for(int i = 0; i<server_num; i++)
    {
        graph->tStartn.push_back(graph->tn);
        graph->tEndn.push_back(Index(server_list[i]+1));
        graph->tU.push_back(ARC_CAP_MAX);
        graph->tC.push_back(0);//super S 到 server cost =0
        graph->tm++;
        /* cout<<"i="<<i<<" "<<Index(server_list[i]+1)<<endl; */
    }

}



FONumber MCFS::GetFO( void )
{
    FONumber fo = 0;
    arcPType *arco;
    for( arco = arcsP ; arco != stopArcsP ; arco++ )
    {
        if( ( arco->ident == BASIC ) || ( arco->ident == AT_UPPER ) )
            fo += arco->cost * arco->flow;
    }
    for( arco = dummyArcsP ; arco != stopDummyP ; arco++ )
    {
        if( ( arco->ident == BASIC ) || ( arco->ident == AT_UPPER ) )
            fo += arco->cost * arco->flow;
    }
    return( fo );
}

void MCFS::infoPArc( arcPType *arc, int ind, int tab )
{
    for( int t = 0 ; t < tab ; t++ )
        cout << "\t";
    cout << "Arco ";
    PrintPArc( arc );
    cout << ": x = " << arc->flow;
    cout << " u = " << arc->upper;
    cout << " c = " << arc->cost;
    cout << endl;
}


void MCFS::PrintPArc( arcPType *arc )
{
    if( arc )
    {
        cout << "(";
        PrintPNode( arc->tail );
        cout << ", ";
        PrintPNode( arc->head );
        cout << ")";
    }
    else
        cout << "..";
}

void MCFS::PrintPNode( nodePType *nodo )
{
    if( nodo )
        if( nodo != dummyRootP )
        {
            cout << ( nodo - nodesP + 1 );
            //cout << ( nodo - nodesP + 1 )<<" pre="<<nodo->prevInT<<" next"<<nodo->nextInT<<" depth"<<nodo->subTreeLevel;
        }
        else
            cout << "r";
    else
        cout << "..";
}
void MCFS::infoPNode( nodePType *node, int tab )
{
    for( int t = 0 ; t < tab ; t++ )
        cout << "\t";
    cout << "Nodo ";
    PrintPNode( node );
    cout << ": b = " << node->balance << " y = " << node->potential << endl;
    cout << ": TArc=";
    PrintPArc( node->enteringTArc );
    cout << endl;
}





void MCFS::ShowSituation( int tab )
{
    arcPType *arc;
    nodePType *node;
    int i = 0;
    cout << endl;
    cout << "this is arc:" << endl;
    for( arc = arcsP ; arc != stopArcsP ; arc++ )
    {
        infoPArc( arc, i, tab );
        i++;
    }
    cout << "this is dummy:" << endl;

    i = 0;
    for( arc = dummyArcsP ; arc != stopDummyP ; arc++ )
    {
        infoPArc( arc, i, tab );
        i++;
    }
    cout << "this is dummy node:" << endl;
    infoPNode( dummyRootP, tab );
    for( node = nodesP ; node != stopNodesP ; node++ )
        infoPNode( node, tab );
    cout << endl;
}


void MCFS::ReadData(char * topo[MAX_EDGE_NUM],int line_num,graph_init *graph)
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
    // cout<<"********************node_num"<<node_num<<endl;

    int start[4] = {0,2,4,-1};
    int end[4] = {0,2,-1,-1};
    end[2] = 4 + edge_num;	//边情况end
    start[3] = 5 + edge_num;  //消费节点begin
    end[3] = 5 + edge_num + cus_num;//消费节点end

    graph->tn=node_num+1;//The number of nodes Add one node:Surper S
    graph->tm=edge_num*2;//The number of arcs twice the num of edge and the num of server
    graph->server_cost = server_cost;
    // cout<<"the Index tm=="<<graph->tm<<endl;


    for( Index i = 0 ; i < graph->tn ; i++ )           // all deficits are 0
    {
        graph->tDfct.push_back(0);                      // unless otherwise stated
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
        graph->tDfct[connect_node_num] -= (-node_demand);
        sum_supply+=node_demand;
        //cout<<"sum_supply==="<<sum_supply<<endl;
    }

    graph->tDfct[graph->tn-1]=(-(sum_supply));//最后那个节点作为超级源节点
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
        graph->tStartn.push_back(arc_begin+1);
        graph->tEndn.push_back(arc_end+1);
        graph->tU.push_back(arc_cap);
        graph->tC.push_back(arc_cost);

        //add both the direction arc
        graph->tStartn.push_back(arc_end+1);
        graph->tEndn.push_back(arc_begin+1);
        graph->tU.push_back(arc_cap);
        graph->tC.push_back(arc_cost);
    }

}

void MCFS::InitNet(graph_init *graph)
{

    cIndex nmx=graph->tn;
    cIndex mmx=graph->tm;
    cIndex pn=graph->tn;
    cIndex pm=graph->tm;
    cFRow pU=&(graph->tU[0]);
    cCRow pC=&(graph->tC[0]);
    cFRow pDfct=&(graph->tDfct[0]);
    cIndex_Set pSn=&(graph->tStartn[0]);
    cIndex_Set pEn=&(graph->tEndn[0]);

    delete[] candP;
    candP = NULL;

    if( ( nmx != nmax ) || ( mmx != mmax ) ) //图改变之后，看两者大小是否一样
    {
        // if the size of the allocated memory changes

        if( nmax && mmax )     // if the memory was already allocated 如果内存已经分配了
        {
            MemDeAlloc();         // deallocate the Primal
            nmax = mmax = 0;
        }

        if( nmx && mmx )     // if the new dimensions of the memory are correct
        {
            nmax = nmx;
            mmax = mmx;
            MemAlloc();
        }
    }
    if( ( ! nmax ) || ( ! mmax ) )
        // if one of the two dimension of the memory isn't correct 如果有一个内存没有分配正确，那就都设为0
        nmax = mmax = 0;

    if( nmax )    // if the new dimensions of the memory are correct，如果两者内存都分配正确
    {
        n = pn;     //图中存放的点的总数设为pn
        m = pm;     //图中的边数

        stopNodesP = nodesP + n;   // first infeasible node address = nodes + n
        dummyRootP = nodesP + nmax;
        nodePType *dummyRootP;         // the dummy root node 中间过程的root,也就是w

        for( nodePType *node = nodesP ; node != stopNodesP ; node++ )
            node->balance = pDfct[ node - nodesP ];  // initialize nodes 初始化节点的balance

        stopArcsP = arcsP + m;     // first infeasible arc address = arcs + m
        dummyArcsP = arcsP + mmax; //tmp spanning tree structure，对于每个点i,有一个dummy arc 连接i和dummy root
        stopDummyP = dummyArcsP + n; //第一个不可行解 = arcs + m + n
        /* cout<<"qiwei_stopDummyP= "<<n<<" "<<dummyArcsP<<endl; */
        for( arcPType *arc = arcsP ; arc != stopArcsP ; arc++ )
        {
            //这里是最重要的函数，是为了将图形放到结构体中
            // initialize real arcs
            arc->cost = pC[ arc - arcsP ];
            arc->upper = pU[ arc - arcsP ];
            arc->tail = nodesP + pSn[ arc - arcsP ] - 1;
            arc->head = nodesP + pEn[ arc - arcsP ] - 1;
        }

        if( pricingRule == kCandidateListPivot )
            MemAllocCandidateList();

        status = kUnSolved;
    }
    InitBase();
}



void MCFS::MemAllocCandidateList(void) //对于不同规模的图，使用不同的候选者ule参数
{
    if( m < 10000 )
    {
        numCandidateList = PRIMAL_LOW_NUM_CANDIDATE_LIST;
        hotListSize = PRIMAL_LOW_HOT_LIST_SIZE;
    }
    else if( m > 100000 )
    {
        numCandidateList = PRIMAL_HIGH_NUM_CANDIDATE_LIST;
        hotListSize = PRIMAL_HIGH_HOT_LIST_SIZE ;
    }
    else
    {
        numCandidateList = PRIMAL_MEDIUM_NUM_CANDIDATE_LIST;
        hotListSize = PRIMAL_MEDIUM_HOT_LIST_SIZE;
    }
    if( forcedNumCandidateList > 0 )    //手动选择候选者list的大小
        numCandidateList = forcedNumCandidateList;

    if( forcedHotListSize > 0 )    //手动选择第二层循环的hotlist的大小
        hotListSize = forcedHotListSize;

    candP = new primalCandidType[ hotListSize + numCandidateList + 1 ];


}



void MCFS::MemDeAlloc(void)
{

    delete[] nodesP;
    delete[] arcsP;
    nodesP = NULL;
    arcsP = NULL;

    //delete the candiate list
    delete[] candP;
    candP = NULL;
}

void MCFS::MemAlloc( void )
{
    nodesP = new nodePType[ nmax + 1 ];   // array of nodes
    arcsP = new arcPType[ mmax + nmax ];  // array of arcs
    dummyArcsP = arcsP + mmax;            // artificial arcs are in the last
    // nmax positions of the array arcs[]
}



template<class N, class A>
N* MCFS::Father( N *n, A *a )
{
    if( a == NULL )
        return NULL;

    if( a->tail == n )
        return( a->head );
    else
        return( a->tail );
}


template<class N>
N* MCFS::CutAndUpdateSubtree( N *root, int delta )
{
    int level = root->subTreeLevel;
    N *node = root;// The root of this subtree is passed by parameters, the last node is searched.
    while ( ( node->nextInT ) && ( ( node->nextInT )->subTreeLevel > level ) )
    {
        node = node->nextInT;
        // The "subTreeLevel" of every nodes of subtree is updated
        node->subTreeLevel = node->subTreeLevel + delta;
    }

    root->subTreeLevel = root->subTreeLevel + delta;
    /* The 2 neighbouring nodes of the subtree (the node at the left of the root
       and the node at the right of the last node) is joined. */

    if( root->prevInT )
        ( root->prevInT )->nextInT = node->nextInT;
    if( node->nextInT )
        ( node->nextInT )->prevInT = root->prevInT;

    return( node );  // the method returns the last node of the subtree
}

template<class N>
void MCFS::PasteSubtree( N *root, N *lastNode, N *previousNode )
{
    /* The method inserts subtree ("root" and "lastNode" are the extremity of the
       subtree) after "previousNode". The method starts to identify the next node
       of "previousNode" ("nextNode"), so it joins "root" with "previousNode" and
       "lastNode" with "nextNode" (if exists). */

    N *nextNode = previousNode->nextInT;
    root->prevInT = previousNode;
    previousNode->nextInT = root;
    lastNode->nextInT = nextNode;
    if( nextNode )
        nextNode->prevInT = lastNode;
}


template<class N, class A>
void MCFS::UpdateT( A *h, A *k, N *h1, N *h2, N *k1, N *k2 )
{
    /* In subtree T2 there is a path from node h2 (deepest node of the leaving
       arc h and root of T2) to node k2 (deepest node of the leaving arc h and
       coming root of T2). With the update of T, this path will be overturned:
       node k2 will become the root of T2...
       The subtree T2 must be reordered and the field "subTreeLevel", which
       represents the depth in T of every node, of every T2's nodes is changed.
       Variable delta represents the increase of "subTreeLevel" value for node
       k2 and its descendants: probably this value is a negative value. */

    int delta = (k1->subTreeLevel) + 1 - (k2->subTreeLevel);
    N *root = k2;
    N *dad;

    /*To reorder T2, the method analyses every nodes of the path h2->k2, starting
      from k2. For every node, it moves the node's subtree from its original
      position to a new appropriate position. In particular k2 and its subtree
      (k2 is the new root of T2, so the first nodes of the new T2) will be moved
      next to node k1 (new father of k2), the next subtree will be moved beside
      the last node of k2's subtree....
      "previousNode" represents the node where the new subtree will be moved
      beside in this iterative action. At the start "previousNode" is the node
      k1 (T2 will be at the right of k1). */

    N *previousNode = k1;
    N *lastNode;
    /* "arc1" is the entering arc in T (passed by parameters).
       For every analysed node of path h2->k2, the method changes
       "enteringTArc" but it must remember the old arc, which will be the
       "enteringTArc" of the next analysed node.
       At the start "arc1" is k (the new "enteringTArc" of k2). */

    A *arc1 = k;
    A *arc2;
    bool fine = false;
    while( fine == false )
    {
        // If analysed node is h2, this is the last iteration
        if( root == h2 )
            fine = true;

        dad = Father( root, root->enteringTArc );
        // Cut the root's subtree from T and update the "subLevelTree" of its nodes
        lastNode = CutAndUpdateSubtree( root, delta );
        // Paste the root's subtree in the right position;
        PasteSubtree( root, lastNode, previousNode );
        // In the next iteration the subtree will be moved beside the last node of
        // the actual analysed subtree.
        previousNode = lastNode;
        /* The increase of the subtree in the next iteration is different from
           the actual increase: in particual the increase increases itself (+2
           at every iteration). */
        delta = delta + 2;
        /* A this point "enteringTArc" of actual root is stored in "arc2" and
           changed; then "arc1" and "root" are changed. */
        arc2 = root->enteringTArc;
        root->enteringTArc = arc1;
        arc1 = arc2;
        root = dad;
    }
}

/*--------------------------------------------------------------------------*/

template<class N, class RCT>
void MCFS::AddPotential( N *r, RCT delta )
{
    int level = r->subTreeLevel;
    N *n = r;

    do
    {
        n->potential = n->potential + delta;
        n = n->nextInT;
    }
    while ( ( n ) && ( n->subTreeLevel > level ) );
}


template <class A>
CNumber MCFS::ReductCost( A *a )
{
    CNumber redc = (a->tail)->potential - (a->head)->potential;
    redc = redc + a->cost;
    return( redc );
}

void MCFS::InitBase(void)
{
    arcPType *arc;
    nodePType *node;
    for(arc = arcsP; arc != stopArcsP; arc++) // initialize dummy arcs  初始化真实边
    {
        arc->flow=0;
        arc->ident=AT_LOWER;
    }
    for(arc=dummyArcsP; arc!=stopDummyP; arc++)  //初始化虚拟边，也就是超级汇点
    {
        node = nodesP + ( arc - dummyArcsP );
        if( node->balance > 0 )    // sink nodes 对于汇点，指向虚拟点
        {
            arc->tail = dummyRootP;
            arc->head = node;
            arc->flow = node->balance;
        }
        else     // source nodes or transit nodes 对于非汇点，从该点指向
        {
            arc->tail = node;
            arc->head = dummyRootP;
            arc->flow = -node->balance;
        }
        arc->cost = MAX_ART_COST;   //每个边的价格设置为最大消费
        arc->ident = BASIC; //标记此边为基本边
        arc->upper = Inf<FNumber>();    //将上界设置为最大值  ???????????????????????
        //arc->upper = Max;    //将上界设置为最大值

    }
    dummyRootP->balance = 0;
    dummyRootP->prevInT = NULL; //初始化之前的点为空？？？？？？
    dummyRootP->nextInT = nodesP;   //一个放了所有点的队列，包括超级汇
    dummyRootP->enteringTArc = NULL;    //进入这个节点的basic 边
    dummyRootP->potential = MAX_ART_COST;   //可能的剩余cost
    dummyRootP->subTreeLevel = 0;   //二级树level为0
    for( node = nodesP ; node != stopNodesP ; node++)    // initialize nodes
    {
        node->prevInT = node - 1;
        node->nextInT = node + 1;
        node->enteringTArc = dummyArcsP + (node - nodesP);
        if( node->balance > 0 )  // sink nodes
            node->potential = 2 * MAX_ART_COST;
        else                     // source nodes or transit node
            node->potential = 0;

        node->subTreeLevel = 1;
    }

    nodesP->prevInT = dummyRootP;
    ( nodesP + n - 1 )->nextInT = NULL;
}




void MCFS::InitializePrimalCandidateList( void )
{
    numGroup = ( ( m - 1 ) / numCandidateList ) + 1;
    groupPos = 0;
    tempCandidateListSize = 0;
}


arcPType* MCFS::RuleDantzig( void )
{
    arcPType *arc = arcToStartP;
    arcPType *enteringArc = NULL;

    CNumber RC;
    CNumber maxValue = 0;


    do
    {
        if( arc->ident > BASIC )
        {
            RC = ReductCost( arc );

            if( ( LTZ( RC, EpsCst ) && ( arc->ident == AT_LOWER ) ) ||
                    ( GTZ( RC, EpsCst ) && ( arc->ident == AT_UPPER ) ) )
            {

                if( RC < 0 )
                    RC = -RC;

                if( RC > maxValue )
                {
                    maxValue = RC;
                    enteringArc = arc;
                }
            }
        }


        arc++;
        if( arc == stopArcsP )
            arc = arcsP;

    }
    while( arc != arcToStartP );
    return( enteringArc );
}

/*--------------------------------------------------------------------------*/

arcPType* MCFS::PRuleFirstEligibleArc( void )
{
    arcPType *arc = arcToStartP;
    arcPType *enteringArc = NULL;

    CNumber RC;

    do
    {

        if( arc->ident > BASIC )
        {
            RC = ReductCost( arc );
            if( ( LTZ( RC, EpsCst ) && ( arc->ident == AT_LOWER ) ) ||
                    ( GTZ( RC, EpsCst ) && ( arc->ident == AT_UPPER ) ) )
                enteringArc = arc;
        }

        arc++;
        if( arc == stopArcsP )
            arc = dummyArcsP;
        if( arc == stopDummyP )
            arc = arcsP;

    }
    while( ( enteringArc == NULL ) && ( arc != arcToStartP ) );

    return( enteringArc );
}
/*--------------------------------------------------------------------------*/
void MCFS::SortPrimalCandidateList( Index min, Index max )
{
    Index left = min;
    Index right = max;

    CNumber cut = candP[ ( left + right ) / 2 ].absRC;

    do
    {
        while( candP[ left ].absRC > cut)
            left++;
        while( cut > candP[ right ].absRC)
            right--;

        if( left < right )
            Swap( candP[ left ], candP[ right ] );

        if(left <= right)
        {
            left++;
            right--;
        }
    }
    while( left <= right );

    if( min < right )
        SortPrimalCandidateList( min, right );
    if( ( left < max ) && ( left <= hotListSize ) )
        SortPrimalCandidateList( left, max );
}

/*--------------------------------------------------------------------------*/

arcPType* MCFS::RulePrimalCandidateListPivot( void )
{
    Index next = 0;
    Index i;
    Index minimeValue;
    if( hotListSize < tempCandidateListSize )
        minimeValue = hotListSize;
    else
        minimeValue = tempCandidateListSize;

    // Check if the left arcs in the list continue to violate the dual condition
    for( i = 2 ; i <= minimeValue ; i++ )
    {
        arcPType *arc = candP[i].arc;
        CNumber red_cost = ReductCost( arc );

        if( ( LTZ( red_cost, EpsCst ) && ( arc->ident == AT_LOWER ) ) ||
                ( GTZ( red_cost, EpsCst ) && ( arc->ident == AT_UPPER ) ) )
        {
            next++;
            candP[ next ].arc = arc;
            candP[ next ].absRC = ABS( red_cost );
        }
    }

    tempCandidateListSize = next;
    Index oldGroupPos = groupPos;
    // Search other arcs to fill the list
    do
    {
        arcPType *arc;
        for( arc = arcsP + groupPos ; arc < stopArcsP ; arc += numGroup )
        {
            if( arc->ident == AT_LOWER )
            {
                CNumber red_cost = ReductCost( arc );
                if( LTZ( red_cost, EpsCst ) )
                {
                    tempCandidateListSize++;
                    candP[ tempCandidateListSize ].arc = arc;
                    candP[ tempCandidateListSize ].absRC = ABS( red_cost );
                }
            }
            else if( arc->ident == AT_UPPER )
            {
                CNumber red_cost = ReductCost( arc );
                if( GTZ( red_cost, EpsCst ) )
                {
                    tempCandidateListSize++;
                    candP[ tempCandidateListSize ].arc = arc;
                    candP[ tempCandidateListSize ].absRC = ABS( red_cost );
                }
            }
        }

        groupPos++;
        if( groupPos == numGroup )
            groupPos = 0;

    }
    while( ( tempCandidateListSize < hotListSize ) && ( groupPos != oldGroupPos ) );

    if( tempCandidateListSize )
    {
        SortPrimalCandidateList( 1, tempCandidateListSize );
        return( candP[ 1 ].arc );
    }
    else
        return( NULL );
}
/*--------------------------------------------------------------------------*/

void MCFS::PrimalSimplex( void )
{

    status = kUnSolved;
    if( pricingRule != kCandidateListPivot )
        arcToStartP = arcsP;

    iterator_qiwei = 0;  // setting the initial arc for the Dantzig or First Elibigle Rule   初始边可以用D规则或者F规则先找出来

    arcPType *enteringArc;  //
    arcPType *leavingArc;   //
    if( pricingRule == kCandidateListPivot )    //使用候选人规则找entering arc
        InitializePrimalCandidateList();

    while( status == kUnSolved )      //还没找到可行解
    {
        iterator_qiwei++;
        switch( pricingRule )       //利用不同的规则找边
        {
        case( kDantzig ):
            enteringArc = RuleDantzig();
            break;
        case( kFirstEligibleArc ):
            enteringArc = PRuleFirstEligibleArc();
            break;
        case( kCandidateListPivot ):
            enteringArc = RulePrimalCandidateListPivot();
            break;
        }


        if( pricingRule != kCandidateListPivot )
        {
            // in every iteration the algorithm changes the initial arc for
            // Dantzig and First Eligible Rule.   改变初始边
            arcToStartP++; // Dantzig Rule and First Eligible Arc Rule start their search from this arc
            if( arcToStartP == stopArcsP )   //wraparound fashion 查找法
                arcToStartP = arcsP;
        }


        if( enteringArc )
        {
            arcPType *arc;
            nodePType *k1;
            nodePType *k2;
            /* If the reduced cost of the entering arc is > 0, the Primal Simplex
               pushes flow in the cycle determinated by T and entering arc for decreases
               flow in the entering arc: in the linear case entering arc's flow goes to 0,
               in the quadratic case it decreases while it's possibile.
               If the reduced cost of the entering arc is < 0, the Primal Simplex pushes
               flow in the cycle  determinated by T and entering arc for increases flow
               in the entering arc: in the linear case entering arc's flow goes to upper
               bound, in the quadratic case it increases while it's possibile.  */
            //通过entering arc 找到 leaving arc
            FNumber t;
            FNumber theta;
            if( enteringArc->ident == AT_UPPER )
            {
                /*  Primal Simplex increases or decreases entering arc's flow.
                "theta" is a positive value.
                For this reason the algorithm uses two nodes ("k1" and "k2") to push
                flow ("theta") from "k1" to "k2".
                According to entering arc's reduct cost, the algorithm determinates
                "k1" and "k2" */

                k1 = enteringArc->head;
                k2 = enteringArc->tail;
                theta = enteringArc->flow;
            }
            else
            {
                k1 = enteringArc->tail;
                k2 = enteringArc->head;
                theta = enteringArc->upper - enteringArc->flow;
            }

            nodePType *memK1 = k1;
            nodePType *memK2 = k2;
            leavingArc = NULL;
            bool leavingReducesFlow = GTZ( ReductCost( enteringArc ), EpsCst );
            // Compute "theta", find outgoing arc and "root" of the cycle
            bool leave;
            // Actual "theta" is compared with the bounds of the other cycle's arcs
            while( k1 != k2 )
            {
                if( k1->subTreeLevel > k2->subTreeLevel )
                {
                    arc = k1->enteringTArc;
                    if( arc->tail != k1 )
                    {
                        t = arc->upper - arc->flow;
                        leave = false;
                    }
                    else
                    {
                        t = arc->flow;
                        leave = true;
                    }

                    if( t < theta )
                    {
                        // The algorithm has found a possible leaving arc
                        theta = t;
                        leavingArc = arc;
                        leavingReducesFlow = leave;
                        // If "leavingReducesFlow" == true, if this arc is selected to exit the base,
                        // it decreases its flow
                    }

                    k1 = Father( k1, arc );
                }
                else
                {
                    arc = k2->enteringTArc;
                    if( arc->tail == k2 )
                    {
                        t = arc->upper - arc->flow;
                        leave = false;
                    }
                    else
                    {
                        t = arc->flow;
                        leave = true;
                    }

                    if( t <= theta )
                    {
                        // The algorithm has found a possible leaving arc
                        theta = t;
                        leavingArc = arc;
                        leavingReducesFlow = leave;
                        // If "leavingReducesFlow" == true, if this arc is selected to exit the base,
                        // it decreases its flow
                    }

                    k2 = Father(k2, arc);
                }
            }



            if( leavingArc == NULL )
                leavingArc = enteringArc;

            if( theta >= Inf<FNumber>() )
            {
                status = kUnbounded;
                break;
            }

            // Update flow with "theta"
            k1 = memK1;
            k2 = memK2;

            if( ! ETZ(theta, EpsFlw ) )
            {
                if( enteringArc->tail == k1 )
                    enteringArc->flow = enteringArc->flow + theta;
                else
                    enteringArc->flow = enteringArc->flow - theta;

                while( k1 != k2 )
                {
                    if( k1->subTreeLevel > k2->subTreeLevel )
                    {
                        arc = k1->enteringTArc;
                        if( arc->tail != k1 )
                            arc->flow = arc->flow + theta;
                        else
                            arc->flow = arc->flow - theta;

                        k1 = Father(k1, k1->enteringTArc);
                    }
                    else
                    {
                        arc = k2->enteringTArc;
                        if( arc->tail == k2 )
                            arc->flow = arc->flow + theta;
                        else
                            arc->flow = arc->flow - theta;

                        k2 = Father(k2, k2->enteringTArc);
                    }
                }
            }

            if( enteringArc != leavingArc )    //strongly fesibly 选取
            {
                bool leavingBringFlowInT2 = ( leavingReducesFlow ==
                                              ( ( leavingArc->tail )->subTreeLevel > ( leavingArc->head )->subTreeLevel ) );
                // "leavingBringFlowInT2" == true if leaving arc brings flow to the subtree T2
                if( leavingBringFlowInT2 == ( memK1 == enteringArc->tail ) )
                {
                    k2 = enteringArc->tail;
                    k1 = enteringArc->head;
                }
                else
                {
                    k2 = enteringArc->head;
                    k1 = enteringArc->tail;
                }
            }

            if( leavingReducesFlow )
                leavingArc->ident = AT_LOWER;
            else
                leavingArc->ident = AT_UPPER;

            if( leavingArc != enteringArc )
            {
                enteringArc->ident = BASIC;
                nodePType *h1;
                nodePType *h2;
                // "h1" is the node in the leaving arc with smaller tree's level
                if( ( leavingArc->tail )->subTreeLevel < ( leavingArc->head )->subTreeLevel )
                {
                    h1 = leavingArc->tail;
                    h2 = leavingArc->head;
                }
                else
                {
                    h1 = leavingArc->head;
                    h2 = leavingArc->tail;
                }

                UpdateT(leavingArc, enteringArc, h1, h2, k1, k2);
                // Update potential of the subtree T2
                k2 = enteringArc->head;
                CNumber delta = ReductCost(enteringArc);
                if( ( enteringArc->tail )->subTreeLevel > ( enteringArc->head )->subTreeLevel )
                {
                    delta = -delta;
                    k2 = enteringArc->tail;
                }

                AddPotential( k2, delta );
                // In the linear case Primal Simplex only updates the potential of the nodes of
                // subtree T2
            }

        }
        else
        {
            status = kOK;
            // If one of dummy arcs has flow bigger than 0, the solution is unfeasible.
            for( arcPType *arc = dummyArcsP ; arc != stopDummyP ; arc++ )
                if( GTZ( arc->flow, EpsFlw ) )
                    status = kUnfeasible;
        }


#if( UNIPI_PRIMAL_ITER_SHOW )
        int it = (int) iterator_qiwei;
        if( it % UNIPI_PRIMAL_ITER_SHOW == 0 )
        {
            cout << endl;
            for( int t = 0; t < 3; t++ )
                cout << "\t";
            cout << "PRIMALE MCFSimplex: ARCHI E NODI ALLA " << it << " ITERAZIONE" << endl;
            ShowSituation( 3 );
        }
#endif
    }

#if( UNIPI_PRIMAL_FINAL_SHOW )
    cout << endl;
    for( int t = 0; t < 3; t++ )
        cout << "\t";
    cout << "PRIMALE UniPi: ARCHI E NODI ALLA FINE" << endl;
    ShowSituation( 3 );
#endif

}  // end( PrimalSimplex )

