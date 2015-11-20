//---------------------------------------------------------------------------------------------------------------------//
//
// Author: Boyang Li and Sixing Lu
// File: notspecfic.cpp   define process not specificed scheduling althorithm functions
//
//---------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <stdlib.h>
#include "notspecfic.h"


bool NotSpec::Notspec_output(char* filename,PublicFunction pubfun)
{
 	ofstream fp;
	fp.open(filename,std::ios::app);
	if (fp==NULL){
	    cout<<"Cannot write ns_HLSM into verilogfile!"<<endl;
        return 0;
    }

	/****** Calculate the exact number state register needs ******/
    int state_num =0;  // max number needs for state
	double middle;
	double rest;
	for(unsigned int i=0;i<pubfun.Process.size();i++){
		state_num =state_num + pubfun.Process[i].size();
	}

	 middle =log((double)(state_num+2))/log((double)2);

     rest =middle-(int)middle;

	 if(rest==0){
        state_num =(unsigned int)middle;
	 }
	 else{
        state_num =floor(middle)+1;
	 }

	/****************************** port *********************************/
	fp<<"module HLSM( Clk,Rst,Start,";
	for(unsigned int i=2;i<pubfun.Input.size();i++){
		fp<<pubfun.Input.at(i)<<",";
	}
	for(unsigned int i=2;i<pubfun.Output.size();i++){
		fp<<pubfun.Output.at(i)<<",";
	}
	fp<<"Done );"<<endl<<endl;

	fp<<"  input Clk, Rst, Start;"<<endl;
	fp<<"  input [31:0] ";
	for(unsigned int i=2;i<pubfun.Input.size()-1;i++){
		fp<<pubfun.Input.at(i)<<",";
	}
	fp<<pubfun.Input.at(pubfun.Input.size()-1)<<";"<<endl;

	fp<<"  output reg [31:0] ";
    for(unsigned int i=2;i<pubfun.Output.size()-1;i++){
		fp<<pubfun.Output.at(i)<<",";
	}
	fp<<pubfun.Output.at(pubfun.Output.size()-1)<<";"<<endl;
	fp<<"  output reg Done;"<<endl<<endl;

	fp<<"  reg ["<<state_num-1<<":0] state;"<<endl;
    fp<<"  reg [31:0] ";
    for(unsigned int i=2;i<pubfun.Regs.size()-1;i++){
		fp<<pubfun.Regs.at(i)<<",";
	}
	fp<<pubfun.Regs.at(pubfun.Regs.size()-1)<<";"<<endl<<endl;
    /***************************** state *****************************/
    fp<<"  parameter S_wait = 0;"<<endl;
	int count_all=1;
	int count_while =1;
    int count_if =1;
	vector<int> count_bracewhile;
    vector<int> count_braceif;
	int count_state;
	for(unsigned int i=0;i<pubfun.Process.size();i++){
		count_state =1;
		for(unsigned int j=0;j<pubfun.Process.at(i).size();j++){
			if(pubfun.Process.at(i).at(j).at(0)=="while"){
				fp<<"  parameter S_while"<<count_while<<" ="<<count_all<<";"<<endl;
				count_bracewhile.push_back(count_while);
				count_while ++;
			}
			else if(pubfun.Process.at(i).at(j).at(0)=="if"){
                fp<<"  parameter S_if"<<count_if<<" ="<<count_all<<";"<<endl;
				count_braceif.push_back(count_if);
				count_if ++;
			}
			else if(pubfun.Process.at(i).at(j).at(0)=="}"){
				if(pubfun.Process.at(i).at(j).at(1)=="while"){
					fp<<"  parameter S_while_dummy"<<count_bracewhile.back()<<" ="<<count_all<<";"<<endl;
					count_bracewhile.pop_back();
				}
				else{  //Process.at(i).at(j).at(1)=="if"
					fp<<"  parameter S_if_dummy"<<count_braceif.back()<<" ="<<count_all<<";"<<endl;
					count_braceif.pop_back();
				}
			}
			else if(pubfun.Process.at(i).at(j).at(0)=="{"){ // will never happen actually
				continue;
			}
			else{
				fp<<"  parameter S_P"<<i<<"_"<<count_state<<" ="<<count_all<<";"<<endl;
				count_state ++;
			}
			count_all ++;
		}
	}
    fp<<"  parameter S_final ="<<count_all<<";"<<endl;
    fp<<endl;

    /***************************** entity *****************************/	
    fp<<"  always@(posedge Clk) begin"<<endl;

    /*************** reset ****************/
    fp<<"    if (Rst == 1) begin"<<endl;
	fp<<"      state <= S_wait;"<<endl;
    fp<<"      Done <= 0;"<<endl;
    for(unsigned int i=2;i<pubfun.Output.size();i++){
		fp<<"      "<<pubfun.Output.at(i)<<" <= 0;"<<endl;
	}
	for(unsigned int i=2;i<pubfun.Regs.size();i++){
		fp<<"      "<<pubfun.Regs.at(i)<<" <= 0;"<<endl;
	}
	fp<<"    end"<<endl<<"    else begin"<<endl;

	/************* state_wait *************/
    fp<<endl;
	fp<<"      case(state)"<<endl;
	fp<<"        S_wait: begin"<<endl;
	fp<<"          Done <= 0;"<<endl;
    for(unsigned int i=2;i<pubfun.Output.size();i++){
		fp<<"          "<<pubfun.Output.at(i)<<" <= 0;"<<endl;
	}
	for(unsigned int i=2;i<pubfun.Regs.size();i++){
		fp<<"          "<<pubfun.Regs.at(i)<<" <= 0;"<<endl;
	}
    //fp<<endl;
	fp<<"          if(Start==1) begin"<<endl;
    fp<<"            state <= 1;"<<endl;
    fp<<"          end"<<endl;
	fp<<"          else begin"<<endl;
	fp<<"            state <= S_wait;"<<endl;
	fp<<"          end"<<endl;
	fp<<"        end"<<endl<<endl;  // S_wait begin's end

    /************* other states *************/
    int counter_while =1;
    int counter_if =1;
	vector<int> counter_bracewhile;
	vector<int> counter_braceif;
	int counter_state;
    for(unsigned int i=0;i<pubfun.Process.size();i++){
		counter_state =1;
		for(unsigned int j=0;j<pubfun.Process.at(i).size();j++){
			/***************** while state *****************/
			if(pubfun.Process.at(i).at(j).at(0)=="while"){ // while could be the end of a process, so check process or check the next statement is another while/if or normal statement
				fp<<"        S_while"<<counter_while<<": begin"<<endl;
                // condition not satisfied
				fp<<"          if("<< pubfun.Process.at(i).at(j).at(2)<<"==0 ) begin"<<endl; 
				fp<<"            state <= S_while_dummy"<<counter_while<<";"<<endl;  //cannot jump to next process directly, because next one may still a loop inside this while
				fp<<"          end"<<endl;		
				// condition satisfied, go to next statement
				fp<<"          else begin"<<endl;

				if( j==pubfun.Process.at(i).size()-1 ){ // end of process
                    if( pubfun.Process.at(i+1).at(0).at(0)=="while" ){  // if the next process's first is a while
						fp<<"          state <= S_while"<<counter_while+1<<";"<<endl;
					}
					else if( pubfun.Process.at(i+1).at(0).at(0)=="if" ){
                        fp<<"          state <= S_if"<<counter_if<<";"<<endl;
					}
					else if( pubfun.Process.at(i+1).at(0).at(0)=="}" ){
						fp<<"          state <= S_while_dummy"<<counter_bracewhile.back()<<";"<<endl;
					}
					else{
						fp<<"          state <= S_P"<<i<<"_"<<counter_state<<";"<<endl;
					}			
				 }
				else if( pubfun.Process.at(i).at(j+1).at(0)=="while" ){
					fp<<"          state <= S_while"<<counter_while+1<<";"<<endl;
				}
				else if( pubfun.Process.at(i).at(j+1).at(0)=="if" ){
					fp<<"          state <= S_if"<<counter_if<<";"<<endl;
				}
				else if( pubfun.Process.at(i).at(j+1).at(0)=="}" ){ //must be a while }, which means execute nothing in while
					fp<<"          state <= S_while_dummy"<<counter_while<<";"<<endl;
				}
				else{ // normal calculation
					fp<<"            state <= S_P"<<i<<"_"<<counter_state<<";"<<endl;
				}
				fp<<"          end"<<endl;
                fp<<"        end"<<endl<<endl;

				counter_bracewhile.push_back(counter_while);
				counter_while ++;
			}
            /***************** if state *****************/
			else if( pubfun.Process.at(i).at(j).at(0)=="if" ){ // if could be the end of a process
				fp<<"        S_if"<<counter_if<<": begin"<<endl;
                // condition not satisfied
				fp<<"          if("<< pubfun.Process.at(i).at(j).at(2)<<"==0 ) begin"<<endl; 
                fp<<"            state <= S_if_dummy"<<counter_if<<";"<<endl;
				fp<<"          end"<<endl;		
				// condition satisfied, go to next statement
				fp<<"          else begin"<<endl;
				if( j==pubfun.Process.at(i).size()-1 ){ // end of process
                    if( pubfun.Process.at(i+1).at(0).at(0)=="while" ){  // if the next process's first is a while
						fp<<"          state <= S_while"<<counter_while<<";"<<endl;
					}
					else if( pubfun.Process.at(i+1).at(0).at(0)=="if" ){
                        fp<<"          state <= S_if"<<counter_if+1<<";"<<endl;
					}
					else if( pubfun.Process.at(i+1).at(0).at(0)=="}" ){
						fp<<"          state <= S_while_dummy"<<counter_bracewhile.back()<<";"<<endl;
					}
					else{
						fp<<"          state <= S_P"<<i<<"_"<<counter_state<<";"<<endl;
					}			
				 }
				else if( pubfun.Process.at(i).at(j+1).at(0)=="while" ){
					fp<<"          state <= S_while"<<counter_while<<";"<<endl;
				}
				else if( pubfun.Process.at(i).at(j+1).at(0)=="if" ){
					fp<<"          state <= S_if"<<counter_if+1<<";"<<endl;
				}
				else if( pubfun.Process.at(i).at(j+1).at(0)=="}" ){ //must be a if }, which means execute nothing in if
					fp<<"          state <= S_if_dummy"<<counter_if<<";"<<endl;
				}
				else{ // normal calculation
					fp<<"            state <= S_P"<<i<<"_"<<counter_state<<";"<<endl;
				}
				fp<<"          end"<<endl;
                fp<<"        end"<<endl<<endl;

				counter_braceif.push_back(counter_if);
				counter_if ++;
			}
			/***************** dummy state *****************/
			else if(pubfun.Process.at(i).at(j).at(0)=="}"){  // } must be the end of a process, may be the last process
				if(pubfun.Process.at(i).at(j).at(1)=="while"){  // should check condition again, condition infor already stored in this line
					fp<<"        S_while_dummy"<<counter_bracewhile.back()<<": begin"<<endl;
					// condition satisfied, go back to while
					fp<<"          if("<< pubfun.Process.at(i).at(j).at(2)<<"!=0 ) begin"<<endl; 
					fp<<"            state <= S_while"<<counter_bracewhile.back()<<";"<<endl;    // jump back to its while
					fp<<"          end"<<endl;	
                    // condition not satisfied, go to next process, if there is no next statement, exit
					if( i == pubfun.Process.size()-1 ){ // this is the last process. 
						fp<<"          else begin"<<endl;
						fp<<"             state <= S_final;"<<endl;
						fp<<"          end"<<endl;
						fp<<"        end"<<endl<<endl;
						fp<<"        S_final: begin"<<endl;
						fp<<"           Done <= 1;"<<endl;
						fp<<"          state <= S_wait;"<<endl;
						fp<<"        end"<<endl<<endl;
						break;  // end the whole program, exit
					}
					fp<<"          else begin"<<endl;
					if( pubfun.Process.at(i+1).at(0).at(0)=="while" ){  // if the next process's first is a while
						fp<<"            state <= S_while"<<counter_while<<";"<<endl;
					}
					else if( pubfun.Process.at(i+1).at(0).at(0)=="if" ){
                        fp<<"            state <= S_if"<<counter_if<<";"<<endl;
					}
					else if( pubfun.Process.at(i+1).at(0).at(0)=="}" ){
						if( pubfun.Process.at(i+1).at(0).at(1)=="while" ){ // next } is a while's }
							fp<<"            state <= S_while_dummy"<<counter_bracewhile[counter_bracewhile.size()-2]<<";"<<endl;  
						}
						else{ // it is a if's }
							fp<<"            state <= S_if_dummy"<<counter_braceif.back()<<";"<<endl;// has not pop this }, so the next } should be the last but one in counter_braceif
						}
					}
					else{
						//fp<<"            state <= S_P"<<i<<"_"<<counter_state<<";"<<endl;
						fp<<"          state <= S_P"<<i+1<<"_"<<"1"<<";"<<endl;
					}
						fp<<"        end"<<endl<<endl;
					counter_bracewhile.pop_back();
				}
				else{  //pubfun.Process.at(i).at(j).at(1)=="if"
					fp<<"        S_if_dummy"<<counter_braceif.back()<<": begin"<<endl; // if need not to go back, directly go to next statement
                    if( i == pubfun.Process.size()-1 ){ // this is the last process. 
						fp<<"           state <= S_final;"<<endl;
						fp<<"        end"<<endl<<endl;
						fp<<"        S_final: begin"<<endl;
						fp<<"           Done <= 1;"<<endl;
						fp<<"          state <= S_wait;"<<endl;
						fp<<"        end"<<endl<<endl;
						break;  // end the whole program, exit
					}
					//fp<<"          else begin"<<endl;
					if( pubfun.Process.at(i+1).at(0).at(0)=="while" ){  // if the next process's first is a while
						fp<<"          state <= S_while"<<counter_while<<";"<<endl;
					}
					else if( pubfun.Process.at(i+1).at(0).at(0)=="if" ){
                        fp<<"          state <= S_if"<<counter_if<<";"<<endl;
					}
					else if( pubfun.Process.at(i+1).at(0).at(0)=="}" ){
						if( pubfun.Process.at(i+1).at(0).at(1)=="while" ){ // next } is a while's }
							fp<<"          state <= S_while_dummy"<<counter_bracewhile.back()<<";"<<endl;  
						}
						else{ // it is a if's }
							fp<<"          state <= S_if_dummy"<<counter_braceif[counter_braceif.size()-2]<<";"<<endl;// has not pop this }, so the next } should be the last but one in counter_braceif
						}
					}
					else{
						//fp<<"          state <= S_P"<<i<<"_"<<counter_state<<";"<<endl;
						fp<<"          state <= S_P"<<i+1<<"_"<<"1"<<";"<<endl;
					}
					//fp<<"          end"<<endl;
					counter_braceif.pop_back();
				}
				fp<<"        end"<<endl<<endl;
				
			}
			/***************** calculation state *****************/
			else{  // could be the end of process, could be end of program, could be in the middle
				fp<<"        S_P"<<i<<"_"<<counter_state<<": begin"<<endl;
				fp<<"          ";
				for(unsigned int k=0;k<pubfun.Process.at(i).at(j).size();k++){//Printout this equation
					if(k==1)
					{
				    	fp<<" <=";
					}
					else
					{
						fp<<pubfun.Process.at(i).at(j).at(k); //Printout individual token
					}
					
				}
				fp<<";"<<endl;
				// decide the next state
				if( i==pubfun.Process.size()-1 && j == pubfun.Process.at(i).size()-1 ){ // end of program, exit
					fp<<"           state <= S_final;"<<endl;
					fp<<"        end"<<endl<<endl;
					fp<<"        S_final: begin"<<endl;
					fp<<"           Done <= 1;"<<endl;
					fp<<"          state <= S_wait;"<<endl;
					fp<<"        end"<<endl<<endl;
					break;
				}
				else if( j==pubfun.Process.at(i).size()-1 ){ // end of process
                    if( pubfun.Process.at(i+1).at(0).at(0)=="while" ){  // if the next process's first is a while
						fp<<"          state <= S_while"<<counter_while<<";"<<endl;
					}
					else if( pubfun.Process.at(i+1).at(0).at(0)=="if" ){
                        fp<<"          state <= S_if"<<counter_if<<";"<<endl;
					}
					else if( pubfun.Process.at(i+1).at(0).at(0)=="}" ){
						fp<<"          state <= S_while_dummy"<<counter_bracewhile.back()<<";"<<endl;
					}
					else{
						fp<<"          state <= S_P"<<i<<"_"<<counter_state<<";"<<endl;
					}			
				}
				else{ // in the middle, the next statement could be calculation or } 
					if( pubfun.Process.at(i).at(j+1).at(0)=="}" ){
						if( pubfun.Process.at(i).at(j+1).at(1)=="while" ){ //next } is a while }
                            fp<<"          state <= S_while_dummy"<<counter_bracewhile.back()<<";"<<endl;
						}
						else{  // next } is a if }
                            fp<<"          state <= S_if_dummy"<<counter_braceif.back()<<";"<<endl;
						}
					}
					else {// next statement is calculation
                        fp<<"          state <= S_P"<<i<<"_"<<counter_state+1<<";"<<endl;
					}
				}

				fp<<"        end"<<endl<<endl;
				counter_state ++;
			}
		}
	}
  
	fp<<endl;
	fp<<"      endcase"<<endl;
	fp<<"    end"<<endl;
	fp<<"  end"<<endl;
	fp<<"endmodule"<<endl;
						
	fp.close();
    return true;
}
