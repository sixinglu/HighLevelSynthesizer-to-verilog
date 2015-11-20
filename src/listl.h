//---------------------------------------------------------------------------------------------------------------------//
//
// Author: Boyang Li and Sixing Lu
// File: listl.h   declare schedule algorithm List_L
//
//---------------------------------------------------------------------------------------------------------------------//

#include <string>
#include <vector>
#include <stack>
#include "PublicFunctions.h"

#ifndef LISTL_H
#define LISTL_H

using namespace std;

#define MAXinput 3
#define VEC_capital 10
#define Hanging -1



// class used to define edge in the graph
class edge
{
	private:
		string value;
		int order;//ID number

	public:
	   int node_in;  // the input node of this edge
       int node_out; // the output node of this edge

       edge   *edge_InNext;  //edges go into the same node will link together
	   edge   *edge_OutNext;
	   edge   *edge_prev;  //used to trace the edges
       edge   *edge_next;

	   // constructor and destructor functions
	   edge(const string &value, int order);

	   // get private elements
	   string GetEdge_value(){return value;}
	   int    GetEdge_order(){return order;}


};

// class used to define vertex in the graph
class vertex
{
	private:
	   string comp; // the name of component 
	   int order;   //ID trace find delay
	   int InputNum;
	   string hardware;//Which hardware to use: ADD_SUB, LOGIC, MULTIPLE

	   int cylces;//Cycles of the instrucition
	   int height; //height of the vertex
	   int Starttimer;
	   int Endtimer;
	   int dependency;

	public: 
	    int row_num;
	   vector<string> statement;//Added statement
	   vector<vertex*> UpTree;
	   vector<vertex*> DownTree;
	   string Done;
	   /*
	   none
	   exe
	   done
	   */

	   vertex *node_prev; // used to trace the nodes
       vertex *node_next;

	   vertex *up;
	   vertex *down;

       edge   *edge_in; //find the edge ID number.
	   edge   *edge_out;

	
	   // constructor and destructor functions
	   vertex(const string &com, int order, int input, vector<string> statement,int row_num);

	   // get private elements
	   int    GetVex_height(){return height;}
	   int    GetVex_order(){return order;}
	   int    GetVex_InputNum(){return InputNum;}
	   string GetVex_hardware(){return hardware;}
	   string GetVex_comp(){return comp;}
	   int	  GetVex_cylces(){return cylces;}
	   int	  GetVex_Starttimer(){return Starttimer;}
	   int	  GetVex_Endtimer(){return Endtimer;}
	

	   // write into the private elements
	   void		WriteVex_height(int height);
	   void		WriteVex_hardware(string hardware);
	   void		WriteVex_cylces(int cylces);
	   void	    WriteVex_Starttimer(int Starttimer);
	   void	    WriteVex_Endtimer(int Endtimer);
	   void 	WriteVex_InputNum(int NewInputNum){ InputNum= NewInputNum;}
	   void 	WriteVex_Dependency(int A){ dependency = A;}
	  
	   
};


class ListL{
private:
	int multi_max;
	int add_sub_max;
	int logic_max;

	vertex* Vex_head;
	vertex* Vex_tail;

	edge *edge_head;
	edge *edge_tail;
public:	
	 int flag_empty_graph;
	int multi_num;
	int add_sub_num;
	int logic_num;

	vertex* uproot;
	vertex* downroot;
	int finish;

	ListL(int mul,int add_sub,int logic);  //constructor
	void Clearvertex();
	// get private member of class
	int Get_mul(){return multi_num;}
	int Get_add_sub(){return add_sub_num;}
	int Get_logic(){return logic_num;}

	int Get_mul_max(){return multi_max;}
	int Get_add_sub_max(){return add_sub_max;}
	int Get_logic_max(){return logic_max;}

	void Write_mul(int mul){ multi_num=mul;}
	void Write_add_sub(int add_sub){ add_sub_num=add_sub;}
	void Write_logic(int logic){ logic_num=logic;}

	void Write_mul_max(int mul){ multi_max=mul;}
	void Write_add_sub_max(int add_sub){ add_sub_max=add_sub;}
	void Write_logic_max(int logic){ logic_max=logic;}

	// List_L Algorith
	bool ListlAlgorithm(char* filename,PublicFunction pubfun);
	vertex* CreateGragh( vector<vector<string> > ScheObj,PublicFunction pubfun);

	void CreateUpTree();//ALAP    
	void Sort_UpTree();//Sort out the height of the tree from down to Up, Call Sort_UpTree_Caculator
	void Sort_UpTree_Caculator(vertex* node);//Recursive call visit_UpTree (Preorder)
	void visit_UpTree(vertex* node);//

	void Schedule_Add(int timer);
	void Schedule_Logic(int timer);
	void Schedule_Multi(int timer);
	void Schedule_Reg(int timer);
	void Reg_Release(int timer);
	void Add_Release(int timer);
	void Logic_Release(int timer);
	void Multi_Release(int timer);

	int Find_Maxheight(vector<vertex*> HeightSort);//return the index
	int Find_MaxFinishtime();

	void CreateDownTree();//ASAP
	void Sort_DownTree();//ASAP
	void Sort_DownTree_Caculator(vertex* node);
	void visit_DownTree(vertex* node);
	
	//Detect what kind of hw each operation should use,and assign cycles

    bool detect_hardware();	//Caculate the time
	int detect_finish();//find out the finish time
	
	bool Sear_list(vector<string> netlist, string element);



	   // get private elements
	   vertex*		GetVex_head(){return Vex_head;}     
       vertex*		GetVex_tail(){return Vex_tail;}
	   
	   edge *       GetEdge_head(){return edge_head;}
	   edge *       GetEdge_tail(){return edge_tail;}

	   // functions when building the graph
	   bool Delete_edge();

	   // function when release memory
	   void Rel_Memory();

	   vertex*	Find_AdjVertex(vertex* index_vertex);// Base on vertex
	   vertex*	Find_AdjVertex_Base_edgeup(edge* index_edge);//up tree uses
	   vertex*	Find_AdjVertex_Base_edgedown(edge* index_edge);//down tree uses old 
	   vertex*  Vertex_Finder(int ID);// Base on ID

	


       // testing functions

	   void printout_VerList();
	   void printout_VerList_Backward();
	   void printout_Hardware();
	   void printout_EdgeList();
	   void printout_OutLink();
       void printout_Inlink();	 
	   void printout_max_hardware();	




};



#endif