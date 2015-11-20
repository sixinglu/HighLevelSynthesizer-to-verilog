//---------------------------------------------------------------------------------------------------------------------//
//
// Author: Boyang Li and Sixing Lu
// File: listr.h   declare schedule algorithm List_R
//
//---------------------------------------------------------------------------------------------------------------------//

#include <string>
#include <vector>
#include "PublicFunctions.h"
#include <stack>



#ifndef LISTR_H
#define LISTR_H

#define MAXinput 3
#define VEC_capital 10
#define Hanging -1

using namespace std;

// class used to define edge in the graph
class edgeR
{
	private:
		string value;
		int order;//ID number

	public:
	   int node_in;  // the input node of this edge
       int node_out; // the output node of this edge

       edgeR   *edge_InNext;  //edges go into the same node will link together
	   edgeR   *edge_OutNext;
	   edgeR   *edge_prev;  //used to trace the edges
       edgeR   *edge_next;

	   // constructor and destructor functions
	   edgeR(const string &value, int order);

	   // get private elements
	   string GetEdge_value(){return value;}
	   int    GetEdge_order(){return order;}

};

// class used to define vertex in the graph
class vertexR
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
	   int ALAP;
	   int dependency;
	public: 
	   vector<string> statement;//Added statement
	   int row_num;
	   vector<vertexR*> UpTree;
	   vector<vertexR*> DownTree;
	   string Done;
	 

	   vertexR *node_prev; // used to trace the nodes
       vertexR *node_next;

	   vertexR *up;
	   vertexR *down;

        edgeR  *edge_in; //find the edge ID number.
	    edgeR  *edge_out;

	
	   // constructor and destructor functions
	   vertexR(const string &com, int order, int input, vector<string> statement,int row_num);

	   // get private elements
	   int    GetVex_height(){return height;}
	   int    GetVex_order(){return order;}
	   int    GetVex_InputNum(){return InputNum;}
	   string GetVex_hardware(){return hardware;}
	   string GetVex_comp(){return comp;}
	   int	  GetVex_cylces(){return cylces;}
	   int	  GetVex_Starttimer(){return Starttimer;}
	   int	  GetVex_Endtimer(){return Endtimer;}
	   int	  GetVex_ALAP(){return ALAP;}
	   int	  GetVex_Dependency(){return dependency;}
	

	   // write into the private elements
	   void		WriteVex_height(int height);
	   void		WriteVex_hardware(string hardware);
	   void		WriteVex_cylces(int cylces);
	   void	    WriteVex_Starttimer(int Starttimer);
	   void	    WriteVex_Endtimer(int Endtimer);
	   void 	WriteVex_ALAP(int ALAP);
	   void 	WriteVex_InputNum(int NewInputNum){ InputNum= NewInputNum;}
	   void 	WriteVex_Dependency(int A){ dependency = A;}
	  

};

class ListR{
private:
	int latency;

	
	vertexR* Vex_head;
	vertexR* Vex_tail;

	edgeR *edge_head;
	edgeR *edge_tail;

public:
    int flag_empty_graph;

    int multi_max;
	int add_sub_max;
	int logic_max;

	int multi_num;
	int add_sub_num;
	int logic_num;

	vertexR* uproot;
	vertexR* downroot;
	int finish;

    ListR(int latency);

	int Get_latency(){return latency;}

	bool ListrAlgorithm(char* filename,PublicFunction pubfun);

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

	vertexR* CreateGragh( vector<vector<string> > ScheObj,PublicFunction pubfun);

	void CreateUpTree();//ALAP    
	void Sort_UpTree();//Sort out the height of the tree from down to Up, Call Sort_UpTree_Caculator
	void Sort_UpTree_Caculator(vertexR* node);//Recursive call visit_UpTree (Preorder)
	void visit_UpTree(vertexR* node);//
	void Schedule_Add(int timer);
	void Schedule_Logic(int timer);
	void Schedule_Multi(int timer);
	void Reg_Release(int timer);
	void Add_Release(int timer);
	void Logic_Release(int timer);
	void Multi_Release(int timer);
	void Schedule_Reg(int timer);

	int Find_Maxheight(vector<vertexR*> HeightSort);//return the index
	int Find_Slack_0(vector<vertexR*> HeightSort,int timer);
	int Find_MaxFinishtime();

	
	//Detect what kind of hw each operation should use,and assign cycles

    bool detect_hardware();	//Caculate the time
	int detect_finish();//find out the finish time
	bool Sear_list(vector<string> netlist, string element);



	   // get private elements
	   vertexR*		GetVex_head(){return Vex_head;}     
       vertexR*		GetVex_tail(){return Vex_tail;}
	   
	   edgeR *       GetEdge_head(){return edge_head;}
	   edgeR *       GetEdge_tail(){return edge_tail;}

	   // functions when building the graph
	   bool Delete_edge();

	   // function when release memory
	   void Rel_Memory();

	   vertexR*	Find_AdjVertex(vertexR* index_vertex);// Base on vertex
	   vertexR*	Find_AdjVertex_Base_edgeup(edgeR* index_edge);//up tree uses
	   vertexR*	Find_AdjVertex_Base_edgedown(edgeR* index_edge);//down tree uses old 
	   vertexR*  Vertex_Finder(int ID);// Base on ID

	
       // testing functions

	   void printout_VerList();
	   void printout_Hardware();
	   void printout_EdgeList();
	   void printout_OutLink();
       void printout_Inlink();	 
	   void printout_ALAP();	 
	   void printout_max_hardware();	



};


#endif