//---------------------------------------------------------------------------------------------------------------------//
//
// Author: Boyang Li and Sixing Lu
// File: PublicFunctions.cpp   define all I/O and public functions
//
//---------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <map>
#include <stack>
#include <stdlib.h>
#include "PublicFunctions.h"


/****************************** Checking format functions *************************/
bool PublicFunction::Check_unique()
{
	vector<string> total(0);
	unsigned int i;
	unsigned int j;
    
	for(i=2;i<this->Input.size();i++){
		if(this->Input.at(i).compare("")){
		   total.push_back(this->Input.at(i));
		}
	}

	for(i=2;i<this->Output.size();i++){
		if(this->Output.at(i).compare("")){
			total.push_back(this->Output.at(i));
		}
	}

	for(i=2;i<this->Regs.size();i++){
		if(this->Regs.at(i).compare("")){
			total.push_back(this->Regs.at(i));
		}
	}

	for(j=0;j<total.size();j++){
		i =j+1;
		while(i<total.size()){
			if (total.at(i)==total.at(j)){
				cout<<"The node "<<total.at(i)<<" is not unique!"<<endl;
				return 0;
			}
			i =i+1;
		}
	}
    return 1;
}

bool PublicFunction::Check_existance(vector<string> opera)
{
	unsigned int i;
	unsigned int j;
    
	// the variable on the left side of = should either be output or reg
	i=2;
	while( i< max(this->Regs.size(),this->Output.size()) ){
		if(i<this->Regs.size()){
            if( this->Regs.at(i)==opera.at(0) ){
		         break;
		    }
		}
		if(i<this->Output.size()){
            if( this->Output.at(i)==opera.at(0) ){
		         break;
		    }
		}
		i++;
	}
	if( i>= max(this->Regs.size(),this->Output.size()) ){
		cout<<opera.at(0)<<" should be among REG or OUTPUT!"<<endl;
		return false;
	}

	// the variable on the right side of = should be input of reg
	for (j=2;j<opera.size();j=j+2){
		i=2;
		while( i< max(this->Regs.size(),this->Input.size()) ){
			if(i<this->Regs.size()){
				if( this->Regs.at(i)==opera.at(j) ){
					 break;
				}
			}
			if(i<this->Input.size()){
				if( this->Input.at(i)==opera.at(j) ){
					 break;
				}
			}
			i++;
		}
		if( i>= max(this->Regs.size(),this->Input.size()) ){
			cout<<opera.at(j) <<" should be among REG or INPUT!"<<endl;
			return false;
		}

	}

    return true;
}

bool PublicFunction::Check_listlImple(int mul,int add_sub,int logic)
{
// check resource is enough or not, notice, if no +, adder=0 is ok
    int num_mul =0;
	int num_adder_sub =0;
	int num_logic =0;

	for(unsigned int i=0;i<Process.size();i++){
		for(unsigned int j=0;j<Process.at(i).size();j++){
			if( Process.at(i).at(j).at(0)=="while" || Process.at(i).at(j).at(0)=="if" || Process.at(i).at(j).at(0)=="}" || Process.at(i).at(j).at(0)=="{" ){
				continue;
			}
			if( Process.at(i).at(j).size()==3 ){ // it is a reg
				//num_logic++;
				break;
			}
			if(Process.at(i).at(j).at(3) =="*"){
                num_mul++;
			}
			else if(Process.at(i).at(j).at(3) =="+" || Process.at(i).at(j).at(3) =="-"){
				num_adder_sub++;
			}
			else{
                num_logic++;
			}
		}
	}
    // check multiplyer number
	if( num_mul ==0 ){
		if( mul<0 ){
			cout<<"multiplyer number should not be less than 0!"<<endl;
			return 0;
		}
	}
	else{
        if( mul<=0 ){
			cout<<"multiplyer number should be larger than 0!"<<endl;
			return 0;
		}
	}
	// check adder number
    if( num_adder_sub ==0 ){
		if( add_sub<0 ){
			cout<<"adder/sub number should not be less than 0!"<<endl;
			return 0;
		}
	}
	else{
        if( add_sub<=0 ){
			cout<<"adder/sub number should be larger than 0!"<<endl;
			return 0;
		}
	}
	// check other logic
	if( num_logic ==0 ){
		if( logic<0 ){
			cout<<"adder/sub number should not be less than 0!"<<endl;
			return 0;
		}
	}
	else{
        if( logic<=0 ){
			cout<<"adder/sub number should be larger than 0!"<<endl;
			return 0;
		}
	}

	return 1;
}

bool PublicFunction::Check_cond(vector<string> vec)
{
	if(vec.at(3)!=")"){
		cout<<"The condtion in "<<vec.at(0)<<" shoud be a variable but equation!"<<endl;
		return false;
	}
    return true;
}

bool PublicFunction::Check_inputNum()
{
	for(unsigned int i=0;i< this->Process.size();i++){
		for(unsigned int j=0;j<this->Process.at(i).size();j++){

			if( Process.at(i).at(j).at(0)=="while" || Process.at(i).at(j).at(0)=="if" || Process.at(i).at(j).at(0)=="}" || Process.at(i).at(j).at(0)=="{" ){
				continue;
			}

			if( Process.at(i).at(j).size() ==3 ){  // reg
				if(Process.at(i).at(j).at(1)!="="){
					cout<<"Input number error!"<<endl;
					return 0;
				}
			}
			else if( Process.at(i).at(j).at(3)=="?"){  // a=b?c:d
				if( Process.at(i).at(j).size()!=7 ){
					cout<<"Input number error!"<<endl;
					return 0;
				}
			}
			else{  // + - * / >> <<
				if( Process.at(i).at(j).size()!=5 ){
					cout<<"Input number error!"<<endl;
					return 0;
				}
			}

		}
	}
	return 1;
}

bool PublicFunction::Check_operation()
{   
	for(unsigned int i=0;i< this->Process.size();i++){
		for(unsigned int j=0;j<this->Process.at(i).size();j++){

			if( Process.at(i).at(j).at(0)=="while" || Process.at(i).at(j).at(0)=="if" || Process.at(i).at(j).at(0)=="}" || Process.at(i).at(j).at(0)=="{" ){
				continue;
			}

			if(Process.at(i).at(j).at(1)!="="){
				cout<<"lack of \"=\" in the equation!"<<endl;
				return 0;
			}

			/*if(Process.at(i).at(j).size()<3){
				cout<<"error equation number!"<<endl;
				return 0;
			}*/

			if(Process.at(i).at(j).size()>3){  // not reg
				string compa =Process.at(i).at(j).at(3);
				if(compa=="?"){
					if(Process.at(i).at(j).at(5)!=":"){
						cout<<"should use \":\" instead of "<<Process.at(i).at(j).at(5)<<endl;
						return 0;
					}
				}

				if(compa!="+" && compa!="-" && compa!="*" && compa!="/" && compa!=">" && compa!="<" && compa!=">>" && compa!="<<" && compa!="?" && compa!="=="){
					cout<<compa<<" is not a legal operation!"<<endl;
					return 0;
				}
			}
		}
	}
	
	return 1;
}

/************************************* I/O functions ****************************************/
bool PublicFunction::Read_cfile(char *fileName) // Read from the behavior netlist
{
    ifstream fp;
	fp.open(fileName,ios::in);
    if (!fp.is_open()){
      cout<<"cannot open the cfile! \n"<<endl;
      return 0;
    }

	//bool flag_brace =0;
	int left_brace =0;
    int right_brace =0;  // used to check brace number
	int count_while =0;
	int count_if =0;
	vector<vector<string> > Operation;
    /***************** Read a line per time **************/
	string temp;
	vector<string> loop;  // log while/if temporary
    vector<string> condition;
    while(getline(fp,temp)){
		if (temp.empty()) {  // if the line is empty, ignore this line
		 continue; 
		} 
		/*std::size_t found;
		found =temp.find("//");
		if(found!=string::npos){
			continue;
		}*/
		istringstream getword(temp); 
	    string token;
		vector<string> vec(0);
		while(!getword.eof()){
			if( (getword >>token).fail())
			   break;
			vec.push_back(token);
		}
		//string::size_type position; // used to find substring in the string
		//position = temp.find("//"); 
		for(unsigned int lu=0;lu<vec.size();lu++){
			unsigned int log_size =vec.size();
			if(vec.at(lu)=="//"){
				for(unsigned int boy=0;boy<log_size-lu;boy++){
					vec.pop_back();
				}
				break;
			}
		}

		if(vec.size()==0){
		   continue;
		}

		/***************** Read the classification **************/
		if( vec.size()>1 ){
			if (!vec.at(1).compare(":")){

				if (!vec.at(0).compare("INPUTS")){  // create edges and input vertexs
					if(Input.size()==0){
						Input =vec;
					}
					else{
						cout<<"There are more than one INPUTS: line!"<<endl;
						return 0;
					}					
				}
					
				else if(!vec.at(0).compare("OUTPUTS")){
					if (this->Output.size()==0){
						this->Output =vec;
					}
					else{
						cout<<"There are more than one OUTPUTS: line!"<<endl;
						return 0;
					}					
				}

				else if(!vec.at(0).compare("REGS")){  //internal registers
					if (this->Regs.size()==0){
						this->Regs =vec;	
					}
					else{
						cout<<"There are more than one REGS: line!"<<endl;
						return 0;
					}							
				}	

				else{ 
					cout<<"The types of ports have to be INPUTS,OUTPUTS,REGS!"<<endl;
					return 0;
				}
				continue;
			}
			
		}
        
        /************************  read into operation line  *************************/
		//else{ 
		string::size_type position1,position2,position3,position4;
		position1 = temp.find("while"); 
        position2 = temp.find("if"); 
		position3 = temp.find("}"); 
		position4 = temp.find("{");

        if( !Check_unique() ){   // check uniqueness 
			return 0;
		}

		if(vec.size()<3){
			if( position1== temp.npos && position2== temp.npos && position3== temp.npos && position4== temp.npos){
				cout<<"the number of equation is incorrect!"<<endl;
				return 0;
			}
		}

		if(left_brace==right_brace && position3!=temp.npos && position4==temp.npos ){
			cout<<"\"{\" should be in front of \"}\"!"<<endl;
			return 0;
		}

		/////////// check while/if format //////////////////////
		unsigned int pos_while =0;
		unsigned int pos_if =0;
		for(unsigned int i=0;i<vec.size();i++){
			if(vec.at(i)=="while"){
               pos_while = i;
			   break;
			}
			else if(vec.at(i)=="if"){
			   pos_if =i;
			   break;
			}
		}
		// check while loop  while(a
		if(position1!=temp.npos){
			if( pos_while+3>vec.size()-1){
				cout<<"while lacks condition or brace!"<<endl;
                return 0;
			}
			if( vec.at(pos_while+1)!="(" || vec.at(pos_while+3)!=")" ){
				cout<<"while's expression is incorrect!"<<endl;
				return 0;
			}
        
		}
		// check if loop  if(a
		if( position2!=temp.npos ){
			if( pos_if+3>vec.size()-1 ){
				cout<<"if lacks condition or brace!"<<endl;
                return 0;
			}
			if( vec.at(pos_if+1)!="(" || vec.at(pos_if+3)!=")" ){
				cout<<"if's expression is incorrect!"<<endl;
				return 0;
			}
		}
        /////////////////////////////////////////

		for(unsigned int i=0;i<vec.size();i++){
			if(vec.at(i)=="{"){
               left_brace ++;
			}
			else if(vec.at(i)=="}"){
			   right_brace ++;
			}
		}

		if(position1!= temp.npos){
			//flag_brace =1;
			count_while ++;
		}
		if(position2!= temp.npos){
			count_if ++;
		}

		// seperate some special cases
		if( (vec.at(0)=="while" || vec.at(0)=="if") && position4 != temp.npos && vec.size()>5){ // while/if(con){c=a+b or while/if(con){c=a+b}
			vector<string> predo;
			for(int i=0;i<5;i++){
				predo.push_back(vec.at(i));
			}
			if(!this->Check_cond(predo)){ // check whether cond is not a equation
				return 0;
			}
			if(Operation.size()!=0){
				this->Process.push_back(Operation);
				Operation.clear();   // over one process
			}
            Operation.push_back(predo);
			loop.push_back(vec[0]); //put while/if into loop
			condition.push_back(vec[2]); // while ( condition )

			vec.erase(vec.begin(),vec.begin()+4);
		}
		else if(vec.at(0)=="{" && vec.size()==1){ // single {
			continue;
		}
		else if(vec.at(0)=="{" && vec.size()>1){  //  {c=a+b or {c=a+b} or {while
			//vector<string> brace;    // "{" not be put into process, then there will not be one line only has "{"
			//brace.push_back("{");
			//Operation.push_back(brace);
			vec.erase(vec.begin());
			//Operation.push_back(vec);  // will deal with vec later
		}

		/////////////// deal with vec ///////////////
	    if ( this->find(vec,"while")|| this->find(vec,"if") ){  // while/if(con){
			if(!this->Check_cond(vec)){ // check whether cond is not a equation
				return 0;
			}
			if(Operation.size()!=0){
				this->Process.push_back(Operation);
				Operation.clear(); // over one process
			}
			Operation.push_back(vec);    // push while line into Operation for later use
			loop.push_back(vec[0]); //put while/if into loop
			condition.push_back(vec[2]); // while ( condition )
		}
		else if( this->find(vec,"}") ){  // if this line has "}" 
			if( vec.at(0)=="}" && vec.size()==1){  // this line only has }
				if(loop.size()!=0){
				   vec.push_back(loop.back()); // while/if information  will stored with "}"
				}
				if(condition.size()!=0){
				   vec.push_back(condition.back()); 
				}
				Operation.push_back(vec);
				this->Process.push_back(Operation);
				Operation.clear();  //over one process
			}
			else if(vec.at(0)=="}" && vec.size()!=1){ // }c=a+b
                vector<string> brace;
				brace.push_back("}");
				if(loop.size()!=0){
				    brace.push_back(loop.back());
				}
				if(condition.size()!=0){
				    brace.push_back(condition.back());
				}
                Operation.push_back(brace);

				this->Process.push_back(Operation);
				Operation.clear();    //over one process

				vec.erase(vec.begin()); // get rid of }
				if( !Check_existance(vec) ){  // check existance
                 return 0;
		        }
                Operation.push_back(vec);
			}
			else{ // c=a+b}   we should seperate them
				vector<string> brace;
				brace.push_back("}");
				if(loop.size()!=0){
				   brace.push_back(loop.back());
				}
				if(condition.size()!=0){
				   brace.push_back(condition.back());
				}

				vec.pop_back(); // get rid of }
				if( !Check_existance(vec) ){  // check existance
                 return 0;
		        }
                Operation.push_back(vec);
                Operation.push_back(brace);

                this->Process.push_back(Operation);
				Operation.clear();  //over one process
			}
			if(loop.size()!=0){
			   loop.pop_back();
			}
			if(condition.size()!=0){
			   condition.pop_back();
			}
		}
		else{  //calculation statement
			  if( !Check_existance(vec) ){  // check existance
                 return 0;
		      }
			  Operation.push_back(vec);  // if this line is a statement line
		}
		//}
	}
	if(Operation.size()!=0){
		this->Process.push_back(Operation);
		//Operation.clear(); // over one process
	}

	// check inpput number
	if(!this->Check_inputNum()){
		cout<<"Input number of operation is incorrect!"<<endl;
		return 0;
	}

	// check the operation
	if(!this->Check_operation()){
        return 0;
    }

	// check brace number
	if(left_brace!=right_brace){
		cout<<"the number of braces is incorrect!"<<endl;
		return 0;
	}

	if((count_while+count_if)*2!=(left_brace+right_brace)){
		cout<<"the number if brace is wrong!"<<endl;
		return 0;
	}
	/*if(flag_brace ==1){
		if(left_brace==0 || right_brace==0){
			cout<<"There is no brace!"<<endl;
			return 0;
		}
	}*/

	fp.close();
	return 1;
}

bool PublicFunction::find(vector<string> vec,string finding)
{
	for(unsigned int i=0;i<vec.size();i++){
		if(vec.at(i)==finding){
			return true;
		}
	}
	return false;
}

bool PublicFunction::Write_head(char *fileName) // Write ports into verilogfile
{
    ofstream fp;
	fp.open(fileName,ios::out);
    if (fp==NULL){
	    cout<<"Cannot write ports into verilogfile!"<<endl;
        return 0;
    }
 
	fp<<"// Project Name: Assignmet4: high-lever state machine "<<endl;
	fp<<"// Authors: Boyang Li, Sixing Lu"<<endl<<endl;
	fp<<"`timescale 1ns / 1ps"<<endl<<endl;

	fp.close();
	return 1;

}

int PublicFunction:: Tras_argv2int(char *argv)
{
	//istringstream trans(temp);
	int num =atoi(argv);
/*  int num=argv[strlen(argv)-1]; // units digit
    int count=1;
	for(int i=strlen(argv)-2;i>=0;i--){
       num =num+argv[i]*count*10;
	   count++;
	}*/

	return num;
}
