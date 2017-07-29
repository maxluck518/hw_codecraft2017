// Genetic Algorithm Hello World
// http://www.generation5.org/content/2003/gahelloworld.asp


#pragma warning(disable:4786)		// disable debug warning

#include <iostream>					// for cout etc.

#include <fstream>
#include <vector>					// for vector class
#include <string>					// for string class
#include <algorithm>				// for sort algorithm
#include <time.h>					// for random seed
#include <math.h>					// for abs()

//#define GA_POPSIZE		50		// ga population size
#define GA_ELITRATE		0.10f		// elitism rate
#define GA_MUTATIONRATE	0.25f		// mutation rate
#define GA_MUTATION		RAND_MAX * GA_MUTATIONRATE
#define GA_POPSIZE		50		// ga population size
#define GA_MAXITER		1000		// maximum iterations

using namespace std;				// polluting global namespace, but hey...

int  GA_TARSIZE = 1000;
struct ga_struct 
{
	string  server;			// 1:biuld server here ;  0:not
	int serverNum;
	vector<int> server_id;
	int fitness;					// its fitness
};

struct graph 
{
	int node;
	int arcs;
	int conNum;
	int serverCost;
	vector<int> consumerID;
	vector<int> consumerNeed;
	vector<int> startNode;
	vector<int> endNode;
	vector<int> Upcapacity;	 
	vector<int> Costs;
};



void readdata(const char *filename,graph &Ggraph){
  ifstream inputdata;
  int databuffer;  
  int i;
  
  inputdata.open(filename);
  if(!inputdata){
  	cerr << "Can't open input file " << filename << endl;
  	exit(1);
 	}
  
  inputdata >> Ggraph.node;
  inputdata >> Ggraph.arcs;
  inputdata >> Ggraph.conNum;
  inputdata >> Ggraph.serverCost;
  Ggraph.startNode.resize(Ggraph.arcs);
  Ggraph.endNode.resize(Ggraph.arcs);
  Ggraph.Upcapacity.resize(Ggraph.arcs);
  Ggraph.Costs.resize(Ggraph.arcs);
  Ggraph.consumerID.resize(Ggraph.conNum);
  Ggraph.consumerNeed.resize(Ggraph.conNum);

  for(i = 0; i < Ggraph.arcs; i++){
  	inputdata >> databuffer;
  	Ggraph.startNode[i] = databuffer - 1;
  	
  	inputdata >> databuffer;
  	Ggraph.endNode[i] = databuffer - 1;
  	
  	inputdata >> databuffer;
  	Ggraph.Upcapacity[i] = databuffer;

  	inputdata >> databuffer;
  	Ggraph.Costs[i] = databuffer;
  }
  
  for(i=0; i<Ggraph.conNum; i++){
  	inputdata >> databuffer;
  	inputdata >> databuffer;
  	Ggraph.consumerID[i] = databuffer - 1;
  	inputdata >> databuffer;
  	Ggraph.consumerNeed[i] = databuffer;
  }

  GA_TARSIZE = Ggraph.node;
  inputdata.close();
  
}


typedef vector<ga_struct> ga_vector;// for brevity

void init_population(ga_vector &population, ga_vector &buffer ) 
{
	int tsize = GA_TARSIZE;

	for (int i=0; i<GA_POPSIZE; i++) {
		ga_struct citizen;
		citizen.serverNum = 0;
		citizen.fitness = 0;
		citizen.server_id.clear();
	
		citizen.server.erase();
		for (int j=0; j<tsize; j++){
			//citizen.server[j] = ((1+rand()%100) > 85);
			citizen.server += ((1+rand()%100) > 80)?'1':'0';
			if(citizen.server[j]=='1'){
				citizen.serverNum++;
				citizen.server_id.push_back(j);
			}
		}

		population.push_back(citizen);
	}
	buffer.resize(GA_POPSIZE);
}

void ResetGraph(ga_struct citizen, graph &Tgraph){
	Tgraph.node++;
	Tgraph.arcs += citizen.serverNum;
	for (int i = 0; i < citizen.serverNum; ++i){
		Tgraph.startNode.push_back(Tgraph.node-1);
		Tgraph.endNode.push_back(citizen.server_id[i]);
		Tgraph.Upcapacity.push_back(1000);
		Tgraph.Costs.push_back(Tgraph.serverCost);
	}
}
void deply(graph Tgraph){
	cout << Tgraph.node<<endl;
}

int  MFCSolver(ga_struct citizen, graph Tgraph){
	ResetGraph(citizen,Tgraph);
	//deply(Tgraph);
	return citizen.serverNum;
}


inline void calc_fitness(ga_vector &population,graph Ggraph)
{
	for (int i=0; i<GA_POPSIZE; i++) 
		population[i].fitness = MFCSolver(population[i],Ggraph);
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
	if(member.server[ipos]=='1') member.server[ipos] = '0';
	else member.server[ipos] = '1';
}

void mate(ga_vector &population, ga_vector &buffer){
	int esize = GA_POPSIZE * GA_ELITRATE;
	int tsize = GA_TARSIZE, spos, i1, i2;
	// cout << "elitism" << endl;
	elitism(population, buffer, esize);
	// cout << "end   elitism" << endl;

	// Mate the rest
	for (int i=esize; i<GA_POPSIZE; i++) {
		i1 = rand() % (GA_POPSIZE / 2);
		i2 = rand() % (GA_POPSIZE / 2);
		spos = rand() % tsize;
		buffer[i].server = population[i1].server.substr(0, spos) + 
			            population[i2].server.substr(spos, tsize - spos);
	       
		
		// cout << "befor mutata" << endl;
		if (rand() < GA_MUTATION) mutate(buffer[i]);
		// cout << "after mutata" << endl;
		buffer[i].server_id.clear();
		buffer[i].serverNum = 0;

		for (int j = 0; j < tsize; ++j)
			if(buffer[i].server[j]=='1'){
				buffer[i].serverNum++;
				buffer[i].server_id.push_back(j);
			}
		 
	}
}


inline void print_best(ga_vector &gav)
{ cout << "Best: " << gav[0].server << " (" << gav[0].fitness << ")" << endl; }

inline void swap(ga_vector *&population, ga_vector *&buffer){ 
	ga_vector *temp = population; population = buffer; buffer = temp; 
}


void Ga_For_MCF(graph Ggraph){
	srand(unsigned(time(NULL)));
	ga_vector pop_alpha, pop_beta;
	ga_vector *population, *buffer;
	init_population(pop_alpha, pop_beta);
	population = &pop_alpha;
	buffer = &pop_beta;
	for (int i=0; i<GA_MAXITER; i++) {
		calc_fitness(*population,Ggraph);		// calculate fitness
		sort_by_fitness(*population);				// sort them
		mate(*population, *buffer);					// mate the population together
		swap(population, buffer);					// swap buffers
	}
	print_best(*population);
}

