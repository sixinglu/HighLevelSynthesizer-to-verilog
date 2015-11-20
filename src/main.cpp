//---------------------------------------------------------------------------------------------------------------------//
//
// Author: Boyang Li and Sixing Lu
// Date:   11-30-2013
//
//---------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <string>
#include <sstream>
#include "PublicFunctions.h"
#include "listl.h"
#include "listr.h"
#include "notspecfic.h"
//#include "help.h"

using namespace std;

// checking commandling arguments required for Assignment 4
int main(int argc, char** argv) {


	if( argc != 2 && argc != 4 && argc != 5 && argc != 7 ) {
		cerr << endl << "Usage: " << argv[0] << " -ns cfile verilogfile" << endl;
        cerr << endl << "Usage: " << argv[0] << " -listl cfile verilogfile mul add logic" << endl;
		cerr << endl << "Usage: " << argv[0] << " -listr cfile verilogfile latency" << endl;
		cerr << endl << "Usage: " << argv[0] << " -help" << endl<< endl;
		return -1;
	}


	if( string(argv[1]) == "-ns" && argc != 4) {
		cerr << endl << "Usage: " << argv[0] << " -cp techlib netlist-ns cfile verilogfil" << endl << endl;
		return -1;
	}
	if( string(argv[1]) == "-listl" && argc != 7) {
		cerr << endl << "Usage: " << argv[0] << " -listl cfile verilogfile mul add logic" << endl << endl;
		return -1;
	}
	if( string(argv[1]) == "-listr" && argc != 5) {
		cerr << endl << "Usage: " << argv[0] << " -listr cfile verilogfile latency" << endl << endl;
		return -1;
	}
	if( string(argv[1]) == "-help" && argc != 2) {
		cerr << endl << "Usage: " << argv[0] << " -help" << endl << endl;
		return -1;
	}

	/*--------------------------------------------------------------------------------------------*/

	/************ Write help document ***********/
	if (string(argv[1]) == "-help"){
		cout<<"Implement no scheduling HSLM, please input: -ns cfile verilogfile"<<endl;
		cout<<"--cfile is the C-like statements"<<endl;
		cout<<"--verilogfile is the HLSM verilog file produced by program according to cfile"<<endl;
		cout<<endl;
		cout<<"Implement ListL scheduling Algorithm, please input: -listl cfile verilogfile mul add logic"<<endl;
		cout<<"--cfile is the C-like statements"<<endl;
		cout<<"--verilogfile is the HLSM verilog file produced by program according to cfile"<<endl;
		cout<<"--mul is the number of multipliers"<<endl;
		cout<<"--add is the number of adders"<<endl;
		cout<<"--logic is the number of other logics"<<endl;
		cout<<endl;
		cout<<"Implement ListR scheduling Algorithm, please input: -listr cfile verilogfile latency"<<endl;
        cout<<"--cfile is the C-like statements"<<endl;
		cout<<"--verilogfile is the HLSM verilog file produced by program according to cfile"<<endl;
		cout<<"--latency is the time constrain for scheduling"<<endl;
		
		return 0;
	}
     
	/*********** Read from the cfile ************/
	PublicFunction pubfun;
	if( !pubfun.Read_cfile(argv[2]) ){
		return -1;
	}

    /******* Write the head of Verilog file *******/
	if( !pubfun.Write_head(argv[3]) ){
		return -1;
	}

	/*********** Write ns HLSM ************/
	if ( string(argv[1]) == "-ns" ) {		
		NotSpec notspecify;
		if( !notspecify.Notspec_output(argv[3],pubfun) ){
			return -1;
		}
                else{
                    cout<<"not scheduling done!"<<endl;
                } 	
	}

	/*********** Write ListL Algorithm HLSM ************/
	else if ( string(argv[1]) == "-listl" ){
		// transfer argument and check if resource is enough
		int mul =pubfun.Tras_argv2int(argv[4]);
		int add_sub =pubfun.Tras_argv2int(argv[5]);
		int logic =pubfun.Tras_argv2int(argv[6]);		
		if( !pubfun.Check_listlImple(mul,add_sub,logic) ){ 
		   return -1;
	    }

        ListL listl(mul,add_sub,logic); // build the class
		if( !listl.ListlAlgorithm(argv[3],pubfun) ){
			return -1;
		}
	}

	/*********** Write ListR Algorithm HLSM ************/
	else if( string(argv[1]) == "-listr" ){
        // transfer argument and check if resource is enough
		int late;
		late =pubfun.Tras_argv2int(argv[4]);
		if( late< 1 ){ //latency <1
			cerr<<argv[4]<<" is less than 1, so the List_R algorithm is impossible to be implemented!"<<endl;
			return -1;
		}

        ListR listr(late);	
		if( !listr.ListrAlgorithm(argv[3],pubfun) ){
			return -1;
		}
	}

	/*********** Wrong input argument ************/
	else{
		cerr << endl << "Usage: " << "-ns -listl -listr -help"<<endl;
		return -1;
	}
	
	return 0;
}
