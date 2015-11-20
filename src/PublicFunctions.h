//---------------------------------------------------------------------------------------------------------------------//
//
// Author: Boyang Li and Sixing Lu
// File: PublicFunctions.h   declare all I/O and public functions
//
//---------------------------------------------------------------------------------------------------------------------//

#include <string>
#include <vector>

#ifndef PUBLIC_H
#define PUBLIC_H

using namespace std;

class PublicFunction{

public:
	vector<string> Input;  // store the Input name
	vector<string> Output; // store the Output name
	vector<string> Regs; // store the Reg name

	vector<vector<vector<string> > > Process;  //store equation

	/******** Checking format functions ********/
	bool Check_unique();  
	bool Check_existance(vector<string> opera);
	bool Check_listlImple(int mul,int add_sub,int logic); // check the resource for ListL is enough or not
	bool Check_cond(vector<string>);  // check cond in while() is constant but not <
	bool Check_inputNum();
	bool Check_operation();


	/******** I/O functions ********/
	int Tras_argv2int(char *argv);  //transfer char* to int number
	bool Read_cfile(char *fileName); // Read from the behavior netlist
	bool Write_head(char *fileName); // Write ports into verilogfile
	bool find(vector<string> vec,string finding);

};

#endif