#ifndef __ns_h__
#define __ns_h__

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

#define MAX_NODE_NUM 1005              //??????????????/?????????????待修改
#define ARC_CAP_MAX 100000           //??????????????/?????????????待修改
#define UNIPI_PRIMAL_ITER_SHOW 0	//
#define UNIPI_PRIMAL_FINAL_SHOW 0



struct arcPType;
struct nodePType;


/*--------------------------------------------------------------------------*/

typedef unsigned int    Index;           ///< index of a node or arc ( >= 0 )
typedef Index          *Index_Set;       ///< set (array) of indices
typedef const Index    cIndex;           ///< a read-only index
typedef cIndex        *cIndex_Set;       ///< read-only index array

/*--------------------------------------------------------------------------*/

typedef int             SIndex;           ///< index of a node or arc
typedef SIndex         *SIndex_Set;       ///< set (array) of indices
typedef const SIndex   cSIndex;           ///< a read-only index
typedef cSIndex       *cSIndex_Set;       ///< read-only index array

/*--------------------------------------------------------------------------*/

typedef double          FNumber;        ///< type of arc flow
typedef FNumber        *FRow;           ///< vector of flows
typedef const FNumber  cFNumber;        ///< a read-only flow
typedef cFNumber      *cFRow;           ///< read-only flow array

/*--------------------------------------------------------------------------*/

typedef double          CNumber;        ///< type of arc flow cost
typedef CNumber        *CRow;           ///< vector of costs
typedef const CNumber  cCNumber;        ///< a read-only cost
typedef cCNumber      *cCRow;           ///< read-only cost array


/*--------------------------------------------------------------------------*/

typedef double          FONumber;
/**< type of the objective function: has to hold sums of products of
   FNumber(s) by CNumber(s) */

typedef const FONumber cFONumber;       ///< a read-only o.f. value

typedef double iteratorType;  // type for the iteration counter and array

//OPTtimers *MCFt;   ///< timer for performances evaluation


/*--------------------------------------------------------------------------*/
/** Very small class to simplify extracting the "+ infinity" value for a
    basic type (FNumber, CNumber, Index); just use Inf<type>(). */


template<class T>
T ABS( const T x )
{
    return( x >= T( 0 ) ? x : - x );
}

/*--------------------------------------------------------------------------*/

template<class T>
void Swap( T &v1, T &v2 )
{
    T temp = v1;
    v1 = v2;
    v2 = temp;
}









template <typename T>
class Inf
{
public:
    Inf() {}
    operator T()
    {
        return( std::numeric_limits<T>::max() );
    }
};

/*--------------------------------------------------------------------------*/
/** Very small class to simplify extracting the "machine epsilon" for a
    basic type (FNumber, CNumber); just use Eps<type>(). */

template <typename T>
class Eps
{
public:
    Eps() {}
    operator T()
    {
        return( std::numeric_limits<T>::epsilon() );
    }
};
/*--------------------------------------------------------------------------*/
/** true if flow x is equal to zero (possibly considering tolerances). */

template<class T>
bool ETZ( T x, const T eps )
{
    if( std::numeric_limits<T>::is_integer )
        return( x == 0 );
    else
        return( (x <= eps ) && ( x >= -eps ) );
}

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/** true if flow x is greater than zero (possibly considering tolerances). */

template<class T>
bool GTZ( T x, const T eps )
{
    if( std::numeric_limits<T>::is_integer )
        return( x > 0 );
    else
        return( x > eps );
}

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/** true if flow x is greater than or equal to zero (possibly considering
    tolerances). */

template<class T>
bool GEZ( T x, const T eps )
{
    if( std::numeric_limits<T>::is_integer )
        return( x >= 0 );
    else
        return( x >= - eps );
}

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/** true if flow x is less than zero (possibly considering tolerances). */

template<class T>
bool LTZ( T x, const T eps )
{
    if( std::numeric_limits<T>::is_integer )
        return( x < 0 );
    else
        return( x < - eps );
}

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/** true if flow x is less than or equal to zero (possibly considering
    tolerances). */

template<class T>
bool LEZ( T x, const T eps )
{
    if( std::numeric_limits<T>::is_integer )
        return( x <= 0 );
    else
        return( x <= eps );
}

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/** true if flow x is greater than flow y (possibly considering tolerances).
 */

template<class T>
bool GT( T x, T y, const T eps )
{
    if( std::numeric_limits<T>::is_integer )
        return( x > y );
    else
        return( x > y + eps );
}

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/** true if flow x is less than flow y (possibly considering tolerances). */

template<class T>
bool LT( T x, T y, const T eps )
{
    if( std::numeric_limits<T>::is_integer )
        return( x < y );
    else
        return( x < y - eps );
}



/*--------------------------------------------------------------------------*/
/*--------------------------- CONSTANTS ------------------------------------*/
/*--------------------------------------------------------------------------*/

static const char DELETED  = -2;  // ident for deleted arcs
static const char CLOSED   = -1;  // ident for closed arcs
static const char BASIC    =  0;  // ident for basis arcs
static const char AT_LOWER =  1;  // ident for arcs in L
static const char AT_UPPER =  2;  // ident for arcs in U


/*选择 entering arc的rule*/
enum enumPrcngRl
{
    kDantzig = 0,        ///< Dantzig's rule (most violated constraint)
    kFirstEligibleArc,   ///< First eligible arc in round-robin
    kCandidateListPivot  ///< Candidate List Pivot Rule
};
/*MCF问题的解决状态*/
enum MCFStatus { kUnSolved = -1,  ///< no solution available
                 kOK = 0,         ///< optimal solution found

                 kStopped,        ///< optimization stopped
                 kUnfeasible,     ///< problem is unfeasible
                 kUnbounded,      ///< problem is unbounded
                 kError           ///< error in the solver
               };


static const int PRIMAL_LOW_NUM_CANDIDATE_LIST =  30;
static const int PRIMAL_MEDIUM_NUM_CANDIDATE_LIST =  50;
static const int PRIMAL_HIGH_NUM_CANDIDATE_LIST =  200;
static const int PRIMAL_LOW_HOT_LIST_SIZE =  5;
static const int PRIMAL_MEDIUM_HOT_LIST_SIZE =  10;
static const int PRIMAL_HIGH_HOT_LIST_SIZE =  20;




/*--------------------------------------------------------------------------*/
/*------------------------------ Base structure ----------------------------*/
/*--------------------------------------------------------------------------*/


struct nodePType
{
    nodePType *prevInT;     // previous node in the order of the Post-Visit on T

    nodePType *nextInT;     // next node in the order of the Post-Visit on T

    arcPType *enteringTArc; // entering basic arc of this node

    FNumber balance;        // supply/demand of this node; a node is called a
    // supply node, a demand node, or a transshipment
    // node depending upon whether balance is larger
    // than, smaller than, or equal to zero

    CNumber potential;      // the node potential corresponding with the flow
    // conservation constrait of this node

    int subTreeLevel;        // the depth of the node in T as to T root

};


struct arcPType
{
    nodePType *tail;        // tail node
    nodePType *head;        // head node
    FNumber flow;           // arc flow
    CNumber cost;           // arc linear cost
    char ident;             // tells if arc is deleted, closed, in T, L, or U
    FNumber upper;          // arc upper bound
};


struct primalCandidType    // Primal Candidate List- - - - - - - - - - - - -
{
    arcPType *arc;            // pointer to the violating primal bound arc
    CNumber absRC;           // absolute value of the arc's reduced cost
};

struct graph_init
{
    Index tn;
    Index tm;
    vector<FNumber>  tU;  // arc upper capacities
    vector<FNumber>  tDfct;  // node deficits     node的收支
    vector<Index> tStartn;    // arc start nodes
    vector<Index> tEndn;    // arc end nodes
    vector<CNumber>  tC;  // arc costs
    int server_cost;
    graph_init() {}
    void operator=(graph_init *a)
    {
        tn=a->tn;
        tm=a->tm;
        for(int i=0; i<a->tm; i++)
        {
            tU.push_back(a->tU[i]);
            tStartn.push_back(a->tStartn[i]);
            tEndn.push_back(a->tEndn[i]);
            tC.push_back(a->tC[i]);
        }
        for(int i=0; i<a->tn; i++)
        {
            tDfct.push_back(a->tDfct[i]);
        }
        server_cost = a->server_cost;
    }
};


struct inspire_interface
{
    vector<int> server_list;	//启发式给定的server list
    int link_cost;	//MCFS算完的立链路上的cost
    vector<int> server_used;	//用到的server
    vector<int> server_balance;	//用到的server上的balance
};

struct MCFS
{


    /* That's not important */
    FNumber EpsFlw;   ///< precision for comparing arc flows / capacities
    FNumber EpsDfct;  ///< precision for comparing node deficits
    CNumber EpsCst;   ///< precision for comparing arc costs

    double MaxTime;   ///< max time (in seconds) in which MCF Solver can find
    ///< an optimal solution (0 = no limits)
    int MaxIter;      ///< max number of iterations in which MCF Solver can find
    ///< an optimal solution (0 = no limits)

    Index n;      ///< total number of nodes
    Index nmax;   ///< maximum number of nodes

    Index m;      ///< total number of arcs
    Index mmax;   ///< maximum number of arcs

    int status;   ///< return status, see the comments to MCFGetStatus() above.
///< Note that the variable is defined int to allow derived
///< classes to return their own specialized status codes
    //bool Senstv;  ///< true <=> the latest optimal solution should be exploited


    /*****************************************************************************/

    iteratorType iterator_qiwei;         // the current number of iterations
    bool usePrimalSimplex;         // TRUE if the Primal Network Simplex is used

    char pricingRule;              // which pricing rule is used

    nodePType *nodesP;             // vector of nodes: points to the n + 1 node
// structs (including the dummy root node)
// where the first node is indexed by zero
// and the last node is the dummy root node

    nodePType *dummyRootP;         // the dummy root node

    nodePType *stopNodesP;         // first infeasible node address = nodes + n

    arcPType *arcsP;               // vector of arcs; this variable points to
// the m arc structs.

    arcPType *dummyArcsP;          // vector of artificial dummy arcs: points
// to the artificial dummy arc variables and
// contains n arc structs. The artificial
// arcs are used to build artificial feasible
// starting bases. For each node i, there is
// exactly one dummy arc defined to connect
// the node i with the dummy root node.

    arcPType *stopArcsP;           // first infeasible arc address = arcs + m

    arcPType *stopDummyP;          // first infeasible dummy arc address
// = arcs + m + n

    arcPType *arcToStartP;         // Dantzig Rule and First Eligible Arc Rule
// start their search from this arc

//iteratorType iterator;         // the current number of iterations

    primalCandidType *candP;       // every element points to an element of the
// arcs vector which contains an arc violating
// dual bound


    Index numGroup;                // number of the candidate lists

    Index tempCandidateListSize;   // hot list dimension (it is variable)

    Index groupPos;                // contains the actual candidate list

    Index numCandidateList;        // number of candidate lists

    Index hotListSize;             // number of candidate lists and hot list dimension

    Index forcedNumCandidateList;  // value used to force the number of candidate list

    Index forcedHotListSize;       // value used to force the number of candidate list
// and hot list dimension

    bool newSession;               // true if algorithm is just started

    CNumber MAX_ART_COST;          // large cost for artificial arcs

    FNumber *modifiedBalance;      // vector of balance used by the PostVisit

    FONumber EpsOpt;               // the precision of the objective function value
// for the quadratic case of the Primal Simplex

    int recomputeFOLimits;         // after how many iterations the quadratic Primal
// Simplex computes "manually" the o.f. value
    /*****************************************************************************/

    void MemAlloc( void );
    void MemDeAlloc( void );



    /*构造函数*/
    MCFS( cIndex nmx = 0, cIndex mmx = 0 )
    {
        nmax = nmx;
        mmax = mmx;
        n = m = 0;

        status = kUnSolved;

        EpsFlw = Eps<FNumber>() * 100;
        EpsCst = Eps<CNumber>() * 100;
        EpsDfct = EpsFlw * ( nmax ? nmax : 100 );

        MaxTime = 0;
        MaxIter = 0;


        newSession = true;
        if( nmax && mmax )
            MemAlloc();
        else
            nmax = mmax = 0;


        pricingRule = kCandidateListPivot;
        forcedNumCandidateList = 0;
        forcedHotListSize = 0;
        usePrimalSimplex = true;
        nodesP = NULL;
        arcsP = NULL;
        candP = NULL;

        if( std::numeric_limits<CNumber>::is_integer )
            MAX_ART_COST = CNumber( 1e7 );
        else
            MAX_ART_COST = CNumber( 1e10 );


    }






    template<class N, class A>
    N* Father( N *n, A *a );
    template<class N>
    N* CutAndUpdateSubtree( N *root, int delta );

    template<class N>
    void PasteSubtree( N *root, N *lastNode, N *previousNode );

    template<class N, class A>
    void UpdateT( A *h, A *k, N *h1, N *h2, N *k1, N *k2 );

    template<class N, class RCT>
    void AddPotential( N *r, RCT delta );

    template <class A>
    CNumber ReductCost( A *a );


    void InitNet(graph_init *graph);
    //read data 需要修改，这里传的是实参！！！！！要拷贝一次的！！！
    void ReadData(char * topo[MAX_EDGE_NUM],int line_num,graph_init *graph);
    void Add_Super_Arc(vector<int> server_list,graph_init* graph);



    void PrintPArc( arcPType *arc );
    void PrintPNode( nodePType *nodo );
    void infoPArc( arcPType *arc, int ind, int tab );
    void infoPNode( nodePType *node, int tab );
    void ShowSituation( int tab );
    void MemAllocCandidateList(void);
    void InitBase(void);
    void InitializePrimalCandidateList( void );
    arcPType* RuleDantzig( void );
    arcPType* PRuleFirstEligibleArc( void );
    void SortPrimalCandidateList( Index min, Index max );
    arcPType* RulePrimalCandidateListPivot( void );
    void PrimalSimplex( void );
    FONumber GetFO( void );
    FONumber MCFGetFO( void )
    {
        if( status == kOK )
            return( (FONumber) GetFO() );
        else if( status == kUnbounded )
        {
            cout<<"kUnbounded:::qiwei do not solve yet"<<endl;
            return( - Inf<FONumber>() );
        }
        else
        {
            cout<<"qiwei do not solved yet"<<endl;
            return( Inf<FONumber>() );
        }
    }

    void qiwei_solve(inspire_interface* inter)
    {
        PrimalSimplex();
        if( status == kOK )
        {
            //return MCFGetFO();
            inter->link_cost=MCFGetFO();
			qiwei_server_used(inter);
		}
        else
        {
            /* cout<<"sorry , qiwei can not solve"<<endl; */
            inter->link_cost=0;
        }
    }

    void qiwei_server_used(inspire_interface* inter)
    {
        int server_num=inter->server_list.capacity();
        arcPType* arc=arcsP+m-server_num;
        for(arc=arcsP+m-server_num; arc<stopArcsP; arc++)
        {   
            int flow=arc->flow;
            //cout<<"   tail ";
            //PrintPNode(arc->tail);
            //cout<<" head: ";
            int head=(arc->head)-nodesP;
            //PrintPNode(arc->head);
            //cout<<" result=="<<flow<<endl;
       
            if(flow!=0)
            {
                inter->server_used.push_back(head);
                inter->server_balance.push_back(flow);
            }
        }
        //cout<<"qiwei666"<<endl;
    }

};


#endif

















