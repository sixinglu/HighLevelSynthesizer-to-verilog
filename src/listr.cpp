//---------------------------------------------------------------------------------------------------------------------//
//
// Author: Boyang Li and Sixing Lu
// File: listr.cpp   define schedule algorithm List_R's functions
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
#include <algorithm>
#include <math.h>
#include "listr.h"



ListR::ListR(int latency)
{
	this->latency =latency;
	this->Vex_head = NULL;
	this->Vex_tail = NULL;
	this->edge_head = NULL;
	this->edge_tail = NULL;
	this->downroot = NULL;
	this->uproot = NULL;
	this->finish=0;//Finish signal for the processor
	 this->multi_num =1;
	this->add_sub_num =1;
	this->logic_num =1;

	this->multi_max =1;//max_avaliable
	this->add_sub_max =1;//max_avaliable
	this->logic_max =1;//max_avaliable

	this->flag_empty_graph =0;

}


// constructor of class vertex
vertexR::vertexR(const string &com,int order, int input,vector<string> statement,int row_num)
{
	this->row_num =row_num;
	this->comp =com;
	this->hardware ="none";// hardware 
	this->Done ="none";
    this->order =order;
	this->InputNum =input;
	this->edge_in =NULL;   // there is no edge/vertex into nop_start
	this->node_prev =NULL;
	this->edge_out =NULL;  // will give value later
	this->node_next =NULL;
	this->up=NULL;//down tree pointer back
	this->down=NULL;//up tree pointer down
	this->height =0;//height initalize to -1	
	this->cylces =Hanging;//cylces initalize to -1	
	this->Endtimer=0;
	this->Starttimer=0;
	this->ALAP=0;
	this->statement.assign(statement.begin(),statement.end());
	this->dependency = 0;
}

// constructor of class edge
edgeR::edgeR(const string &value, int order)
{
    this->value =value;
	this->order =order;
	this->edge_InNext =NULL;
	this->edge_OutNext =NULL;
	this->edge_prev =NULL;  
    this->edge_next =NULL;
	this->node_in = Hanging;  // Hanging(-1) means not connect to any vertex
    this->node_out = Hanging;

}

// write into the private elements
void  vertexR::WriteVex_height(int height)
{
	this->height=height;
}

// write into the private elements
void  vertexR::WriteVex_hardware(string hardware)
{
	this->hardware=hardware;
}

// write into the private elements
void  vertexR::WriteVex_cylces(int cylces)
{
	this->cylces=cylces;
}
void  vertexR::WriteVex_Starttimer(int Starttimer)
{

		this->Starttimer=Starttimer;
}
void  vertexR::WriteVex_Endtimer(int Endtimer)
{
	this->Endtimer=Endtimer;

}
void  vertexR::WriteVex_ALAP(int ALAP)
{

	this->ALAP=ALAP;

}
bool  ListR::ListrAlgorithm(char* filename,PublicFunction pubfun)
{
	
    int counter_while =1;
	double rest=-1;
    int counter_if =1;
	vector<int> counter_bracewhile;
	vector<int> counter_braceif;
	int counter_state;
	//string temp;
	//unsigned int i=0;
	//unsigned int x=0;
	vertexR* HeadNode =NULL;
	int Finishtime=-1;
	int overallfinish=0;
	//Initialized the NodeMatrix


	ofstream fp;
	fp.open(filename,std::ios::app);
	if (fp==NULL){
	    cout<<"Cannot write ListlAlgorithm into verilogfile!"<<endl;
        return 0;
    }

	for(unsigned int i=0;i<pubfun.Process.size();i++){
		HeadNode=CreateGragh(pubfun.Process.at(i),pubfun);	
		
		if(this->flag_empty_graph==1){  //it is an empty graph
			Clearvertex();
			continue;
		}

		if(!detect_hardware()){
			printout_Hardware();
			return 0;
		}
		
		CreateUpTree();
		Sort_UpTree();
	
	//    printout_ALAP();
	//	printout_max_hardware();
	//	printout_Hardware();
	//	printout_VerList();
		for(int timer=1;this->finish==0;timer++)//time stamp
		{	
			//ALAP will need to be recacullated

			Reg_Release(timer);
			Add_Release(timer);
	        Logic_Release(timer);
	        Multi_Release(timer);
			Schedule_Multi(timer);
			Schedule_Add(timer);
			Schedule_Logic(timer);
			Schedule_Reg(timer);
			this->finish=detect_finish();//Keep check if the process all finished or not
	//     	printout_VerList();
		}
	//	printout_max_hardware();

		Finishtime = Find_MaxFinishtime();
		overallfinish += Finishtime;

		if(Finishtime>this->Get_latency())
		{
		cout<<"This task will not be able to finish within the time constrain!"<<endl;
		return 0;
		}
		Clearvertex();
		this->finish=0;
	}

	



	/****** Calculate the exact number state register needs ******/
    int state_num =0;  // max number needs for state
	double middle;

	for( unsigned int i=0;i<pubfun.Process.size();i++ ){
		for( unsigned int j=0;j<pubfun.Process.at(i).size();j++ ){
			if ( pubfun.Process.at(i).at(j).at(0) =="while" || pubfun.Process.at(i).at(j).at(0)=="if" ||  pubfun.Process.at(i).at(j).at(0)=="}" ){
				state_num++;
			}
		}
	}	
		
	
	/*Go through all the state and go though all the if & while */
	 middle =log((double)(overallfinish+state_num+2))/log((double)2);

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
				count_all ++;
			}
			else if(pubfun.Process.at(i).at(j).at(0)=="if"){
                fp<<"  parameter S_if"<<count_if<<" ="<<count_all<<";"<<endl;
				count_braceif.push_back(count_if);
				count_if ++;
				count_all ++;
			}
			else if(pubfun.Process.at(i).at(j).at(0)=="}"){
				if(pubfun.Process.at(i).at(j).at(1)=="while"){
					fp<<"  parameter S_while_dummy"<<count_bracewhile.back()<<" ="<<count_all<<";"<<endl;
					count_all ++;
					count_bracewhile.pop_back();
				}
				else{  //Process.at(i).at(j).at(1)=="if"
						fp<<"  parameter S_if_dummy"<<count_braceif.back()<<" ="<<count_all<<";"<<endl;
						count_all ++;
						count_braceif.pop_back();
					}
			}
			else if(pubfun.Process.at(i).at(j).at(0)=="{"){ // will never happen actually
				continue;
			}
			else{
				//    int end_state=1;
					int timer=1;
				
					HeadNode=CreateGragh(pubfun.Process.at(i),pubfun);

					if(this->flag_empty_graph==1){  //it is an empty graph
						Clearvertex();
						continue;
					}

					detect_hardware();	
					CreateUpTree();
					Sort_UpTree();
				//	printout_VerList();
					for(int timer=1;this->finish==0;timer++)//time stamp
					{	
						Reg_Release(timer);
						Add_Release(timer);
						Logic_Release(timer);
						Multi_Release(timer);
						Schedule_Multi(timer);
						Schedule_Add(timer);
						Schedule_Logic(timer);
						Schedule_Reg(timer);
				//		Reg_Release(timer);
						this->finish=detect_finish();//Keep check if the process all finished or not
						
					}
			
					Finishtime = Find_MaxFinishtime();
	
					Clearvertex();
					this->finish=0;
				//    cout<<"Pass it second!!!!!"<<endl<<endl<<endl<<endl;
					while(Finishtime+1!=timer)
				  {
					if(count_state<Finishtime+1)//add wait_state
					{
						fp<<"  parameter S_P"<<i<<"_"<<count_state<<" ="<<count_all<<";"<<endl;
						count_state ++;			
						count_all ++;
					}
					timer++;
			   	}
				
			}

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
	
//	Big loop

	int timer=1;
	int end_state=1;
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
                    if( pubfun.Process.at(i+1).at(0).at(0)=="while" ){  // if the next process's fiRst is a while
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
			else if( pubfun.Process.at(i).at(j).at(0)=="if" ){ // if never could be the end of a process
				fp<<"        S_if"<<counter_if<<": begin"<<endl;
                // condition not satisfied
				fp<<"          if("<< pubfun.Process.at(i).at(j).at(2)<<"==0 ) begin"<<endl; 
                fp<<"            state <= S_if_dummy"<<counter_if<<";"<<endl;
				fp<<"          end"<<endl;		
				// condition satisfied, go to next statement
				fp<<"          else begin"<<endl;		
				if( j==pubfun.Process.at(i).size()-1 ){ // end of process
                    if( pubfun.Process.at(i+1).at(0).at(0)=="while" ){  // if the next process's fiRst is a while
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
					if( pubfun.Process.at(i+1).at(0).at(0)=="while" ){  // if the next process's fiRst is a while
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
					fp<<"          end"<<endl;
					counter_bracewhile.pop_back();
				}
				else{  //pubfun.Process.at(i).at(j).at(1)=="if"
					fp<<"        S_if_dummy"<<counter_braceif.back()<<": begin"<<endl; // if need not to go back, directly go to next statement
                    if( i == pubfun.Process.size()-1 ){ // this is the last process. 
						fp<<"           state <= S_final;"<<endl;
						fp<<"        end"<<endl;
						fp<<endl;
						fp<<"        S_final: begin"<<endl;
						fp<<"           Done <= 1;"<<endl;
						fp<<"          state <= S_wait;"<<endl;
						fp<<"        end"<<endl<<endl;
						break;  // end the whole program, exit
					}
					if( pubfun.Process.at(i+1).at(0).at(0)=="while" ){  // if the next process's fiRst is a while
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
					counter_braceif.pop_back();
				}
				fp<<"        end"<<endl<<endl;
			}

			else{  // could be the end of process, could be end of program, could be in the middle
		
				// Each process
				end_state=1;
				timer=1;
				HeadNode=CreateGragh(pubfun.Process.at(i),pubfun);	

				if(this->flag_empty_graph!=1){  //it isnot an empty graph
					
					detect_hardware();	
					CreateUpTree();
					Sort_UpTree();
					
			
					for(int intertimer=1;this->finish==0;intertimer++)//time stamp
					{	

						Reg_Release(intertimer);
						Add_Release(intertimer);
						Logic_Release(intertimer);
						Multi_Release(intertimer);
						Schedule_Multi(intertimer);
						Schedule_Add(intertimer);
						Schedule_Logic(intertimer);
						Schedule_Reg(intertimer);
						this->finish=detect_finish();//Keep check if the process all finished or not
					}
					//			printout_VerList();
					//printout_VerList();	
					Finishtime = Find_MaxFinishtime();	
					
					if(Finishtime==0)
					{
						Finishtime=1;
					}
					//go through all the timer
				
					while(Finishtime+1 != timer)
					{
							if(Finishtime>=end_state)
							{
								fp<<"        S_P"<<i<<"_"<<counter_state<<": begin"<<endl;
								
					
								vertexR* finishindex=this->GetVex_head();
				
								while(finishindex!=NULL)
								{
									if(finishindex->GetVex_Starttimer()==timer)
									{
										fp<<"          ";
										for(unsigned int z=0;z<finishindex->statement.size();z++)
										{
											if(z==1)
											{
												fp<<" <=";	
											}
											else
											{
												fp<<" "<<finishindex->statement.at(z);//<<endl;//Print out the whole statement	
											}		
										}
										j++;	
										fp<<";"<<endl;
								
									}
									finishindex =finishindex->node_next;
								}
							
								if(Finishtime>end_state)
								{

									fp<<"           state <= S_P"<<i<<"_"<<end_state+1<<";"<<endl;
							
									fp<<"        end"<<endl<<endl;//Different state
								
								}
							 }	
							timer++;
							end_state++;
							counter_state++;
					  }
						
					  Clearvertex();
					  j--;
				 }

                // decide where to jump
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
					if( pubfun.Process.at(i+1).at(0).at(0)=="while" ){  // if the next process's fiRst is a while
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
					 // break;
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
				
				this->finish=0;
			}
		}
	}//End of process

	
	fp<<endl;
	fp<<"      endcase"<<endl;
	fp<<"    end"<<endl;
	fp<<"  end"<<endl;
	fp<<"endmodule"<<endl;
	
	cout<<"listr HLSM Finished£¡"<<endl;
						
	fp.close();
	return true;
}

void ListR::Clearvertex()
{
	edgeR *tempedge =GetEdge_head();
	edgeR *oldedge=tempedge;

	vertexR* temp= GetVex_head();
	vertexR* old=temp;

	while(temp!=NULL)
	{	
		old=temp->node_next;
        delete(temp);
		temp=old;

		
	}

	while(tempedge!=NULL)
	{	
		oldedge=oldedge->edge_next;
		delete(tempedge);
		tempedge=oldedge;
		
	}
	
	this->edge_head =NULL;
	this->edge_tail =NULL;
	this->Vex_head =NULL;
	this->Vex_tail =NULL;
	this->downroot = NULL;
	this->uproot = NULL;

	this->finish=0;//Finish signal for the processor
	 this->multi_num =1;
	this->add_sub_num =1;
	this->logic_num =1;

	this->multi_max =1;//max_avaliable
	this->add_sub_max =1;//max_avaliable
	this->logic_max =1;//max_avaliable

	delete(this->uproot);
	delete(this->downroot);
	this->uproot=NULL;
	this->downroot=NULL;

	this->flag_empty_graph =0;
}


vertexR* ListR::CreateGragh(vector<vector<string> > ScheObj,PublicFunction pubfun)
{
	vertexR *node=NULL;
	vertexR *curr_vex=NULL; //node is used for creating new vertex, curr_vex is used for indicate which vertex
	//vertex * OPER_temp=NULL;
	//vertex * REG_temp=NULL;
	edgeR   *side=NULL;
	edgeR *curr_edge=NULL; //side is used for creating new edge, curr_edge is used for indicate which edge
	int index_vertex =1;
	int index_edge =1;
	vector<string> empty;
	empty.push_back("");

	node =new vertexR("nop_start",0,0,empty,-1); //nop_start is node_order =0, InputNum =0
	this->Vex_head =node; //connect the head of graph
	curr_vex =node;

	node =new vertexR("nop_end",-2,pubfun.Output.size(),empty,-1);  //nop_end is node_order =MAX_order,InputNum= output node Num
	this->Vex_tail =node; //connect to the end of the graph
	node->node_prev =curr_vex; // connect to previous list
	curr_vex->node_next =node;
	curr_vex =node;		

	 for(unsigned int j=0;j<ScheObj.size();j++)//Hight(Y) ->Equation
		{	
			 bool flag_circle =0;
			 edgeR * log_sear_outedge=NULL;
			 vertexR * OPER_temp=NULL;
	         vertexR * REG_temp=NULL; // initial

		     if( ScheObj.at(j).at(0)=="while" || ScheObj.at(j).at(0)=="if" || ScheObj.at(j).at(0)=="}"){
				 continue;
			 }
			 ////////////////// if this line is for declare input/output/wire, create the nop node/////////////
			  // if the operation is a "=" 
			 if (ScheObj.at(j).size()==3){
				 // do nothing, need not build operation vertex
			 }
			//create a operation vertex if that operation is not "="
			 else{
				 
				 // build opertaion vertex
				 node =new vertexR(ScheObj.at(j).at(3),index_vertex,(ScheObj.at(j).size()-1)/2,ScheObj.at(j),j);  
				 index_vertex++;
				 node->node_prev =curr_vex; // connect to list
				 curr_vex->node_next =node;
				 curr_vex =node;
			 }
			
            OPER_temp =node;  //store the operation, because we will build another node
           ///////////////// check if the outcome is OUTPUTS ///////////////////
		    if (Sear_list(pubfun.Output,ScheObj.at(j).at(0))){
				if(ScheObj.at(j).size()==3){ //z=zin, it will not have operation vertex
					//creat output vertex, since it is regs
					node =new vertexR("REG", index_vertex,1,ScheObj.at(j),j);
					REG_temp =node;
					index_vertex++;
					node->node_prev =curr_vex; // connect to vertex list
					curr_vex->node_next =node;
					curr_vex =node;

					// create the edge go out from this reg, go into nop_end
					side =new edgeR(ScheObj.at(j).at(0),index_edge);
					index_edge++;
					side->node_out =node->GetVex_order();
					side->node_in =this->GetVex_tail()->GetVex_order();   // this final edge is input of nop_end
					if (this->edge_head ==NULL){
						this->edge_head =side;
						side->edge_prev =this->edge_head;
					}
					else{
						curr_edge->edge_next =side;
						side->edge_prev =curr_edge;
					}
					curr_edge =side;
					this->edge_tail =side;
					log_sear_outedge =side;  // fora later search

				}

				// create edge go out from operation vertex
				else{  // x=a+b but not x=a
					side =new edgeR(ScheObj.at(j).at(0),index_edge); // delaytime of edge is 0
					index_edge++;
					side->node_out =OPER_temp->GetVex_order();
					//side->node_in =node->GetVex_order();
					side->node_in =this->GetVex_tail()->GetVex_order();  // go into nop_end
					if (this->edge_head ==NULL){
						this->edge_head =side;
						side->edge_prev =this->edge_head;
					}
					else{
						curr_edge->edge_next =side;
						side->edge_prev =curr_edge;
					}
					curr_edge =side;
					this->edge_tail =side;
					log_sear_outedge =side;  // fora later search
				}				
			}
			
            //////////////////////// check if the outcome is Regs, but behavior is wire /////////////////////
			if (Sear_list(pubfun.Regs,ScheObj.at(j).at(0))){
		       //creat a vertex, since it is regs
				if(ScheObj.at(j).size()==3){
					node =new vertexR("REG", index_vertex,1,ScheObj.at(j),j);
					REG_temp =node;
					index_vertex++;
					node->node_prev =curr_vex; // connect to vertex list
					curr_vex->node_next =node;
					curr_vex =node;

					//creat the edge go out of reg vertex
					side =new edgeR(ScheObj.at(j).at(0),index_edge); // delaytime of edge is 0
					index_edge++;
					side->node_out =node->GetVex_order();
					if (this->edge_head ==NULL){
						this->edge_head =side;
						side->edge_prev =this->edge_head;
					}
					else{
						curr_edge->edge_next =side;
						side->edge_prev =curr_edge;
					}
					curr_edge =side;
					this->edge_tail =side;
					log_sear_outedge =side;  // fora later search
				}
				else{  // x=a+b but not x=a, the latter one will not have operation vertex
					side =new edgeR(ScheObj.at(j).at(0),index_edge); // delaytime of edge is 0
					index_edge++;
					side->node_out =OPER_temp->GetVex_order();
					//side->node_in =node->GetVex_order();
					if (this->edge_head ==NULL){
						this->edge_head =side;
						side->edge_prev =this->edge_head;
					}
					else{
						curr_edge->edge_next =side;
						side->edge_prev =curr_edge;
					}
					curr_edge =side;
					this->edge_tail =side;
					log_sear_outedge =side;  // fora later search
			   }
				
			}

			/////////////////////// check if input element is INPUTS ///////////////////            
			unsigned int i =2;
            vector<edgeR*> log_del;
			edgeR * log_sear_edge =NULL;
			bool flag_second =0;
			while (i<ScheObj.at(j).size()){
			   if (Sear_list(pubfun.Input,ScheObj.at(j).at(i))){
					// create the input vertex
				    node =new vertexR("INPUTS",index_vertex,1,empty,-1);
					index_vertex++;
					node->node_prev =curr_vex; // connect to vertex list
					curr_vex->node_next =node;
					curr_vex =node;

					// create the edge go into the input vertex, go out from nop_start
					side =new edgeR(ScheObj.at(j).at(i),index_edge);
					side->node_in =node->GetVex_order();
					side->node_out =this->GetVex_head()->GetVex_order();
					index_edge++;
					if (this->edge_head ==NULL){
						this->edge_head =side;
						side->edge_prev =this->edge_head;
					}
					else{
						curr_edge->edge_next =side;
						side->edge_prev =curr_edge;
					}
					curr_edge =side;
					this->edge_tail =side;

					
					// create the edge go out from input vertex, go into operation vertex or reg vertex
					side =new edgeR(ScheObj.at(j).at(i),index_edge); // delaytime of edge is 0
					index_edge++;
			
					if (this->edge_head ==NULL){
						this->edge_head =side;
						side->edge_prev =this->edge_head;
					}
					else{
						curr_edge->edge_next =side;
						side->edge_prev =curr_edge;
					}
					curr_edge =side;
					this->edge_tail =side;

					if(ScheObj.at(j).size()!=3){   // connect to node
						side->node_in =OPER_temp->GetVex_order();
					}
					else{
						side->node_in =REG_temp->GetVex_order();
					}
					
				    side->node_out =node->GetVex_order();
			    }	
			    else if ( Sear_list(pubfun.Output,ScheObj.at(j).at(i))){  // if it is Output, error
				    cout<<ScheObj.at(j).at(i)<<" should not be an output!"<<endl;
				    return 0;
			    }

				else if(ScheObj.at(j).at(i)==ScheObj.at(j).at(0)){  // t1 =t1+c
					if(flag_second==1){
						i=i+2;
						continue;
					}

					flag_second =1;
					//create the edge go into operation vertex/reg
					side =new edgeR(ScheObj.at(j).at(i),index_edge); // delaytime of edge is 0
					log_sear_edge =side;
					index_edge++;
					if (this->edge_head ==NULL){
						this->edge_head =side;
						side->edge_prev =this->edge_head;
					}
					else{
						curr_edge->edge_next =side;
						side->edge_prev =curr_edge;
					}
					curr_edge =side;
					this->edge_tail =side;

					if (ScheObj.at(j).size()!=3){					
						side->node_in =OPER_temp->GetVex_order(); // record, these edges are the input of operation vertex
					}
					else{
						side->node_in =REG_temp->GetVex_order();
				 	}

					// if there is depency, connect to the pervious one
					vector<edgeR*> log_muldummy;
					edgeR *depend =this->GetEdge_head();
                    edgeR *lastline =NULL;
					vertexR *lastnode =NULL;
					while(depend!=NULL){
						if(side->GetEdge_value()==depend->GetEdge_value() && side->node_in!=depend->node_out && side!=depend){
							if(depend->node_in==Hanging){
								if(lastline==NULL){
                                    lastline =depend;
									lastnode =this->Vertex_Finder(lastline->node_out);
								}
								else{
									if(this->Vertex_Finder(depend->node_out)->row_num > lastnode->row_num){
										lastline =depend;
										lastnode =this->Vertex_Finder(lastline->node_out);
									}
								}
								log_del.push_back(depend);  //log it for later deletion
							}
							else{  // find the edge into a node with same name
								if(lastline==NULL){
									lastline =depend;
									lastnode =this->Vertex_Finder(lastline->node_in);
								}
								else{
									if(this->Vertex_Finder(depend->node_in)->row_num > lastnode->row_num){
										lastline =depend;
										lastnode =this->Vertex_Finder(lastline->node_in);
									}
								}	
								log_muldummy.push_back(depend);
							}
						}
						depend =depend->edge_next;
					 }


					if(lastline!=NULL){
						if(lastline->node_in==Hanging){
							side->node_out =lastline->node_out;
						}
						else{ 
							flag_circle =1;
							for(unsigned int kk=0;kk<log_muldummy.size();kk++){
								node =new vertexR("DUMMY",index_vertex,1,empty,-1);
								index_vertex++;
								node->node_prev =curr_vex; // connect to vertex list
								curr_vex->node_next =node;
								curr_vex =node;
								// create a edge go from dummy node, go into opea/reg node
								side =new edgeR("DUMMY",index_edge); // delaytime of edge is 0
								index_edge++;
								if (this->edge_head ==NULL){
									this->edge_head =side;
				 					side->edge_prev =this->edge_head;
								}
								else{
									curr_edge->edge_next =side;
									side->edge_prev =curr_edge;
								}
								curr_edge =side;
								this->edge_tail =side;
								// connect this edge with node
								side->node_out =node->GetVex_order();
								if(ScheObj.at(j).size()!=3){
									side->node_in =OPER_temp->GetVex_order();
								}
								else{
									side->node_in =REG_temp->GetVex_order();
								}
								// create a edge go from prev-node, go into dummy node
								side =new edgeR("DUMMY",index_edge); // delaytime of edge is 0
								index_edge++;
								if (this->edge_head ==NULL){
									this->edge_head =side;
				 					side->edge_prev =this->edge_head;
								}
								else{
									curr_edge->edge_next =side;
									side->edge_prev =curr_edge;
								}
								curr_edge =side;
								this->edge_tail =side;
								// connect this side to node
								side->node_out =this->Vertex_Finder(log_muldummy.at(kk)->node_in)->GetVex_order();
								side->node_in =node->GetVex_order();
							}
					     }	
				     }
				}

				else {   // this input is not Input/Output
					//create the edge go into operation vertex/reg
					side =new edgeR(ScheObj.at(j).at(i),index_edge); // delaytime of edge is 0
					index_edge++;
					if (this->edge_head ==NULL){
						this->edge_head =side;
						side->edge_prev =this->edge_head;
					}
					else{
						curr_edge->edge_next =side;
						side->edge_prev =curr_edge;
					}
					curr_edge =side;
					this->edge_tail =side;

					if (ScheObj.at(j).size()!=3){					
						side->node_in =OPER_temp->GetVex_order(); // record, these edges are the input of operation vertex
					}
					else{
						side->node_in =REG_temp->GetVex_order();
					}
					
					// if there is depency, connect to the pervious one, but should be the lastline
					edgeR *depend_out2 =this->GetEdge_head();
					edgeR *lastline_normal =NULL;
					while(depend_out2!=NULL){
						if(side->GetEdge_value()==depend_out2->GetEdge_value() && depend_out2->node_in==Hanging){
							if(lastline_normal==NULL){
								lastline_normal =depend_out2;
							}
							else{
								if(this->Vertex_Finder(depend_out2->node_out)->row_num > this->Vertex_Finder(lastline_normal->node_out)->row_num){
									lastline_normal =depend_out2;
								}
							}
						}
						depend_out2 =depend_out2->edge_next;
					 }
					 // put node information into edge
					if(lastline_normal!=NULL){
					   side->node_out =lastline_normal->node_out;
					}
				 }

				 i =i+2;
			}
			////////////////////////////////////////////////////////////////////////////
			if(log_del.size()!=0){ // delete the connected edge
				for(unsigned int k=0;k<log_del.size();k++){
					if(log_del.at(k)==this->GetEdge_head()){
						this->edge_head =log_del[k]->edge_next;
						this->edge_head->edge_prev =NULL;
					}
					else if(log_del.at(k)==this->GetEdge_tail()){
						this->edge_tail =log_del[k]->edge_prev;
						this->edge_tail->edge_next =NULL;
					}
					else{
						edgeR* del_next =log_del[k]->edge_next;
						edgeR* del_prev =log_del[k]->edge_prev;
						del_prev->edge_next =del_next;
						del_next->edge_prev =del_prev;
					}
					delete log_del[k];
				}
                log_del.clear();
			}

			/////////////////////  deal with g =h+a, h=b  ////////////////////////////
			if(flag_circle ==1){  // already create dummy
				continue;
			}
			vector<edgeR*> log_muldummy_normal;
			edgeR *comm =this->GetEdge_head();
			edgeR *lastline_comm =NULL;
			while(comm!=NULL){
				if( ScheObj.at(j).at(0)==comm->GetEdge_value() && comm->node_in!=Hanging && comm->node_in!=-2){
					if(log_sear_edge!=NULL){
						if(comm->GetEdge_order()!=log_sear_edge->GetEdge_order()){
							if(lastline_comm==NULL){
						        lastline_comm =comm;
					        }
					        else{
								if(this->Vertex_Finder(comm->node_in)->row_num > this->Vertex_Finder(lastline_comm->node_in)->row_num){
								   lastline_comm =comm;
								}
					        }
							log_muldummy_normal.push_back(comm);
						}
					}
					else{
						if(lastline_comm==NULL){
						        lastline_comm =comm;
					    }
					    else{
							if(this->Vertex_Finder(comm->node_in)->row_num > this->Vertex_Finder(lastline_comm->node_in)->row_num){
								lastline_comm =comm;
							}
					    }
						log_muldummy_normal.push_back(comm);
					}
				}
				comm =comm->edge_next;
			}
			//if(lastline_comm!=NULL){
			for(unsigned int pp=0;pp<log_muldummy_normal.size();pp++){
				// create dummy node
				node =new vertexR("DUMMY",index_vertex,1,empty,-1);
				index_vertex++;
				node->node_prev =curr_vex; // connect to vertex list
				curr_vex->node_next =node;
				curr_vex =node;
				// create a edge go from dummy node, go into opea/reg node
				side =new edgeR("DUMMY",index_edge); // delaytime of edge is 0
				index_edge++;
				if (this->edge_head ==NULL){
					this->edge_head =side;
				 	side->edge_prev =this->edge_head;
				}
				else{
					curr_edge->edge_next =side;
					side->edge_prev =curr_edge;
				}
				curr_edge =side;
				this->edge_tail =side;
				// connect this edge with node
				side->node_out =node->GetVex_order();
				if(ScheObj.at(j).size()!=3){
					side->node_in =OPER_temp->GetVex_order();
				}
				else{
					side->node_in =REG_temp->GetVex_order();
				}
				// create a edge go from prev-node, go into dummy node
                side =new edgeR("DUMMY",index_edge); // delaytime of edge is 0
				index_edge++;
				if (this->edge_head ==NULL){
					this->edge_head =side;
				 	side->edge_prev =this->edge_head;
				}
				else{
					curr_edge->edge_next =side;
					side->edge_prev =curr_edge;
				}
				curr_edge =side;
				this->edge_tail =side;
				// connect this side to node
				side->node_out =this->Vertex_Finder(log_muldummy_normal.at(pp)->node_in)->GetVex_order();
				side->node_in =node->GetVex_order();
			}

			//////////////////// deal with overlap ////////////////////////  connect the multiple, could be the last one, but lazy to do
			edgeR *overlap =this->GetEdge_head();
			vector<edgeR *> comm_overlap;
			while(overlap!=NULL){
				if( ScheObj.at(j).at(0)==overlap->GetEdge_value() && (overlap->node_in==Hanging || overlap->node_in==-2)){
					if(log_sear_outedge!=NULL){
						if(overlap->GetEdge_order()!=log_sear_outedge->GetEdge_order()){
							comm_overlap.push_back(overlap);
						}
					}
					else{
						comm_overlap.push_back(overlap);
					}
				}
				overlap =overlap->edge_next;
			}
			// create the dummy node
			for(unsigned int uu=0;uu<comm_overlap.size();uu++){
				// create dummy node
				node =new vertexR("DUMMY",index_vertex,1,empty,-1);
				index_vertex++;
				node->node_prev =curr_vex; // connect to vertex list
				curr_vex->node_next =node;
				curr_vex =node;
				// create a edge go from dummy node, go into opea/reg node
				side =new edgeR("DUMMY",index_edge); // delaytime of edge is 0
				index_edge++;
				if (this->edge_head ==NULL){
					this->edge_head =side;
				 	side->edge_prev =this->edge_head;
				}
				else{
					curr_edge->edge_next =side;
					side->edge_prev =curr_edge;
				}
				curr_edge =side;
				this->edge_tail =side;
				// connect this edge with node
				side->node_out =node->GetVex_order();
				if(ScheObj.at(j).size()!=3){
					side->node_in =OPER_temp->GetVex_order();
				}
				else{
					side->node_in =REG_temp->GetVex_order();
				}
				// create a edge go from prev-node, go into dummy node
                side =new edgeR("DUMMY",index_edge); // delaytime of edge is 0
				index_edge++;
				if (this->edge_head ==NULL){
					this->edge_head =side;
				 	side->edge_prev =this->edge_head;
				}
				else{
					curr_edge->edge_next =side;
					side->edge_prev =curr_edge;
				}
				curr_edge =side;
				this->edge_tail =side;
				// connect this side to node
				side->node_out =this->Vertex_Finder(comm_overlap.at(uu)->node_out)->GetVex_order();
				side->node_in =node->GetVex_order();
			}

	}		

	Delete_edge();

    /////////////////////// build the links among edges and vertex //////////////////////////
    vertexR *traver_vertex =this->GetVex_head();
	edgeR *traver_edge =this->GetEdge_head();
	edgeR *link_edge_in =NULL;
	edgeR *link_edge_out =NULL;

	while(traver_vertex!=NULL){
        traver_edge =this->GetEdge_head();
		while(traver_edge!=NULL){
			if(traver_edge->node_in == traver_vertex->GetVex_order()){
				if (traver_vertex->edge_in == NULL){
					traver_vertex->edge_in =traver_edge;					
				}
				else{
					link_edge_in->edge_InNext =traver_edge;
				}
				link_edge_in =traver_edge;
			}

			else if(traver_edge->node_out == traver_vertex->GetVex_order()){
				if (traver_vertex->edge_out == NULL){
					traver_vertex->edge_out =traver_edge;					
				}
				else{
					link_edge_out->edge_OutNext =traver_edge;
				}
				link_edge_out =traver_edge;
			}
			traver_edge =traver_edge->edge_next;
		}
		traver_vertex =traver_vertex->node_next;
	}

	vertexR* trave_empty =this->GetVex_head();
	int count_empty =0;
	while(trave_empty!=NULL){
        count_empty++;
		trave_empty =trave_empty->node_next;
	}
	if(count_empty==2){  // it is an empty graph
		this->flag_empty_graph =1;
	}

	//printout_EdgeList();

	return Vex_head;						
}

// check whether the element in equation in INPUT/OUTPUT/WIRES/REGS
bool ListR::Sear_list(vector<string> netlist, string element)  
{
     unsigned  int i=0;
	if (netlist.size()==0){
		return 0;
	}

	for(i=0;i<netlist.size();i++){
		if(!element.compare(netlist.at(i))){
			return 1;
		}
	}

    return 0;
}


// used to combine edges when building the graph
bool ListR::Delete_edge()
{
		    
	if(this->GetEdge_head()==NULL){
		return 0;
	}

    edgeR* index =this->GetEdge_head();
	edgeR* deletedge =NULL;
	edgeR* temp_prev=NULL,*temp_next=NULL;
	edgeR* temp_location=NULL;
	while(index!=NULL){		
	    if (index->node_in==Hanging){
			deletedge =index;
			if (deletedge==this->GetEdge_head()){
				this->edge_head =this->edge_head->edge_next;
				temp_location =this->edge_head;
			}
			else if(deletedge==this->GetEdge_tail()){
				this->edge_tail =this->edge_tail->edge_prev;
				this->edge_tail->edge_next =NULL;
				temp_location =NULL;
			}
			else{       
				temp_prev =deletedge->edge_prev;
				temp_next =deletedge->edge_next;
				temp_prev->edge_next =temp_next;
				temp_next->edge_prev =temp_prev;
				temp_location =temp_next;
			}
			delete deletedge;
            deletedge =NULL;
			index =temp_location;
		}
		else{
			index =index->edge_next;
		}
	}

	return 1;

}

//Search the vertex by using the number of the vertex
vertexR* ListR::Vertex_Finder(int ID)
 {
    vertexR* temp_vertex=this->GetVex_head();

	// modify by lu 2013-11-25
	//while((temp_vertex!= NULL)&&(temp_vertex->GetVex_order() != ID) )//to find the vertex by using the vertex ID
	while(temp_vertex!= NULL)
	{   if(temp_vertex->GetVex_order() == ID){
		   break;
	    }
		temp_vertex=temp_vertex->node_next;
	}
	if(temp_vertex==NULL)
	{
		return NULL;
	}
	else
	{
		return temp_vertex;
	}
 }

//Find the adjcent Vertex base on edge
vertexR* ListR::Find_AdjVertex_Base_edgeup(edgeR* index_edge)//Up tree use
{
	//From down to up
	int  vertex_id=-10;//Initial
	vertexR* temp_vertex=this->GetVex_head();

	if(index_edge == NULL)//We explored all the adj. vertex
		{
			return NULL;
		}
	else
		{
			vertex_id = index_edge->node_out;//?



			temp_vertex = this->uproot;
			
			if(temp_vertex->GetVex_order()==vertex_id)
			{
				return temp_vertex;
			}

			temp_vertex =this->GetVex_head();
			//while (temp_vertex!=NULL || temp_vertex->GetVex_order() != vertex_id )//to find the vertex by using the vertex ID
			while (temp_vertex!=NULL)
			{	
				if(temp_vertex->GetVex_order() == vertex_id){
					break;
				}
				temp_vertex=temp_vertex->node_next;// ch
			}
			return temp_vertex;
		}
}


//Detect what hardware to use and assign the cycles
bool ListR::detect_hardware()
{
	//cout<< endl;
	vertexR* index=this->GetVex_head();
	while(index!=NULL){

		if(index->GetVex_comp()=="*")
		{
		
			index->WriteVex_hardware("multi");
	        index->WriteVex_cylces(2);
		 }
		 else if(index->GetVex_comp()=="-"||index->GetVex_comp()=="+")
		 {

			index->WriteVex_hardware("add_sub");
	        index->WriteVex_cylces(1);
		 }
		 else if(index->GetVex_comp()==">"
			 ||index->GetVex_comp()=="<"||index->GetVex_comp()=="=="||index->GetVex_comp()=="?"
			 ||index->GetVex_comp()=="<<"||index->GetVex_comp()==">>")
		 {
			index->WriteVex_hardware("logic");
	        index->WriteVex_cylces(1);
		 }
		 else if(index->GetVex_comp()=="REG")
		 {
			
			index->WriteVex_hardware("REG");	
			index->WriteVex_cylces(1);
		 
		 }
		 else if(index->GetVex_comp()=="INPUTS"||index->GetVex_comp()=="nop_start"||index->GetVex_comp()=="nop_end"||
			 index->GetVex_comp()=="uproot"||index->GetVex_comp()=="upend"||index->GetVex_comp()=="DUMMY")
		 {
			
			index->WriteVex_hardware("none");
	        index->WriteVex_cylces(0);
		 
		 }
		 else if(index->GetVex_comp()=="OUTPUTS")
		 {
			
			index->WriteVex_hardware("none");
	        index->WriteVex_cylces(0);
		 
		 }
		 else 
		 {
		 
			cout<<"NO SUCH OPERATION! Error!"<<endl;
			index->WriteVex_hardware("ERROR!");
			return 0;
		 
		 }

		index =index->node_next;
	}

	//printout_Hardware();
		return 1;
	
}
//Caculate the height of each vertex
int ListR::detect_finish()
{
	vertexR* index_vertex=this->GetVex_head();//up
	while(index_vertex!= NULL)
	{
		//printout_VerList();
		if(index_vertex->GetVex_cylces()==1 ||index_vertex->GetVex_cylces()==2 )//Check all the components that with a height
		{
			if(index_vertex->Done =="none" || index_vertex->Done =="exe")//Any one is not finished return 0
			{
				return 0;
			}
		
		}
		index_vertex=index_vertex->node_next;//Traverse all the tree	
	}	
	
	return 1;
		



}

vertexR*	ListR::Find_AdjVertex_Base_edgedown(edgeR* index_edge)//down tree uses old
{
	//From up to down

	int  vertex_id=-10;//Initial
	vertexR* temp_vertex=this->GetVex_head();

	if(index_edge == NULL)//We explored all the adj. vertex
		{
			return NULL;
		}
	else
		{
			
			   vertex_id = index_edge->node_in;//Done tree use
			
				// modify by lu 2013-11-25
				//while (temp_vertex!=NULL || temp_vertex->GetVex_order() != vertex_id )//to find the vertex by using the vertex ID
				while(temp_vertex!=NULL)
				{   if(temp_vertex->GetVex_order() == vertex_id){
					   break;
					}
					temp_vertex=temp_vertex->node_next;
				}
				return temp_vertex;
			

			
		}




}


//ALAP(Down to up) LISTL

void ListR::CreateUpTree()//ALAP
{
	vertexR* index_vertex;
//	vertexR* indicator=NULL;
	vertexR* temp=NULL;
	vector<string> empty;
	edgeR* edgein_counter;//=index_vertex->edge_in;//get edges linkto the pre
	edgeR* indexedge=NULL;
	int inputsnumber=0;
	int edgeID=-100;

	/********** test ***********/
/*	cout<<"before:"<<endl;
	printout_OutLink();
	printout_Inlink(); 
	printout_EdgeList();*/
	////////////////////////////
	
	/*********** Create new end_nop for the partial graph **********/
	vertexR* newuproot=new vertexR("uproot",-5,inputsnumber,empty,-1);//up
	vertexR* temphead =this->GetVex_head();
	while(temphead!= NULL){

		//Create the new edges
		if(temphead->GetVex_comp()!="nop_end" && temphead->GetVex_comp()!="nop_start"){
		   if(temphead->edge_out==NULL){		
			  // for the new edge
			  edgeR* side =new edgeR(temphead->GetVex_comp(),edgeID); 
	          edgeID++;
              
			  temphead->edge_out=side;
              side->node_out=temphead->GetVex_order();

			  // put edge into edge_list
			  this->edge_tail->edge_next =side;
			  this->edge_tail =side;

			  if(this->GetVex_tail()->edge_in==NULL){  // no valid nop_end

				  side->node_in=-5;
				  
				  //For the new root
  				  if(newuproot->edge_in==NULL){	
					  newuproot->edge_in=side;
					  indexedge=side;
				   }
				  else{
					  indexedge->edge_InNext=side;//Chaining
					  indexedge=side;
				  }	 
					
				  inputsnumber++; 
			  }
			  else{  // connect to valid nop_end
				  side->node_in =this->GetVex_tail()->GetVex_order();

				  edgeR* find_edgeend =this->GetVex_tail()->edge_in;
				  edgeR* find_prev=this->GetVex_tail()->edge_in;
				  while(find_edgeend!=NULL){
					  find_prev =find_edgeend;
					  find_edgeend =find_edgeend->edge_InNext;
				  }
				  find_prev->edge_InNext =side;
				  this->GetVex_tail()->WriteVex_InputNum(this->GetVex_tail()->GetVex_InputNum()+1);
			  }
		   }
		}
		 temphead=temphead->node_next;//Traverse all the tree  	     
	}				
	newuproot->WriteVex_InputNum(inputsnumber);
	newuproot->WriteVex_cylces(0);

	// make connection in the vertex list
    if(this->GetVex_tail()->edge_in==NULL){  // has update
		vertexR* old_end =this->GetVex_head()->node_next;   // old nop_end
		vertexR* old_end_next =old_end->node_next;

		this->GetVex_head()->node_next =newuproot;
        newuproot->node_prev =this->GetVex_head();
		newuproot->node_next =old_end_next;
		old_end_next->node_prev =newuproot;
        
		this->Vex_tail=newuproot;
		delete old_end;
	}
    
	this->uproot =this->Vex_tail;

	/********** test ***********/
/*	cout<<"after:"<<endl;
	printout_OutLink();
	printout_Inlink(); */
    ////////////////////////////

	/*********** Create new start_nop for the partial graph **********/
	vertexR* newupend=new vertexR("upend",-6,inputsnumber,empty,-1);//up
	newupend->WriteVex_cylces(0);
	edgeR* tempdege =this->GetEdge_head();
    vertexR* new_nop_start=NULL;  // it is what you want
	while(tempdege!= NULL){  // travse the who graph, finding the node without edge_in
   
			if( tempdege->node_out==Hanging ){	
				if(this->GetVex_head()->edge_out==NULL ){  // invalid old nop_start
				    tempdege->node_out=-6;
				    //For the new nop_start
  					if(newupend->edge_out==NULL){	
				        newupend->edge_out=tempdege;
					    indexedge=tempdege;
					}
					else{
						indexedge->edge_OutNext=tempdege;//Chaining
						indexedge=tempdege;
					}
				 }
				else{
					tempdege->node_out=this->GetVex_head()->GetVex_order();

					edgeR* find_edgestart =this->GetVex_head()->edge_out;
					edgeR* find_prevs=this->GetVex_head()->edge_out;
					while(find_edgestart!=NULL){
						find_prevs =find_edgestart;
						find_edgestart =find_edgestart->edge_OutNext;
					}
					  find_prevs->edge_OutNext =tempdege;
				}
			}

			tempdege=tempdege->edge_next;//Traverse all the tree
		    new_nop_start =newupend;
	}				
	//newupend->WriteVex_InputNum(inputsnumber);  // still 0, uncessary to rewrite it
		
	// make connection in the vertex list
	if(this->GetVex_head()->edge_out==NULL){ // has update new nop_start
		new_nop_start->node_next =this->GetVex_head()->node_next;
		this->GetVex_head()->node_next->node_prev =new_nop_start;
		this->Vex_head =new_nop_start;
	}
    
	this->downroot =this->Vex_head;


	index_vertex =this->GetVex_head(); 	
	while(index_vertex!= NULL)
	{

		edgein_counter=index_vertex->edge_in;//Edge number goes into the vertex, trace down to up
	
		while(edgein_counter!=NULL)
			{				
				//temp is uplevel
				//index_vertex is lower level
				temp=Find_AdjVertex_Base_edgeup(edgein_counter);
				temp->down=index_vertex;//Up higher level pointer to the down level (index_vertex has not update yet)
				index_vertex->UpTree.push_back(temp);//Push into the up level vetor
			/*
				if(edgein_counter->node_in!=edgein_counter->node_out )
				{
					index_vertex->UpTree.push_back(temp);//Push into the up level vetor
				}
				else
				{
					cout<<edgein_counter->node_in<<"      "<<edgein_counter->node_out<<endl;
				
				}*/
		
					edgein_counter=edgein_counter->edge_InNext;//Find the other branch that into this vertex
			
			
			}	
	
		index_vertex=index_vertex->node_next;//Traverse all the tree
	}

}

void ListR::Sort_UpTree()
{
	//vertexR* index_vertex=this->uproot;
	vertexR* index_vertex=this->uproot;
	Sort_UpTree_Caculator(index_vertex);
}

void ListR::Sort_UpTree_Caculator(vertexR* node)
{	
//	int temp=0;
	if( node->UpTree.size() == 0) 
		return;
	else{
		unsigned int i=0;

		this->visit_UpTree(node);
		while(node->UpTree.size()>i)
		{				
			Sort_UpTree_Caculator(node->UpTree.at(i));
			i++;
		}
	//	cout<<endl;
	}

}

void ListR::visit_UpTree(vertexR* index_vertex)
{
	vertexR* NextVertex=NULL;
	int temp=-1;
	int tempcycle=0;
	int latency=this->Get_latency();
	//edgeR* index_edge=index_vertex->edge_out;//get edges
	edgeR* temp_edge_out=NULL;


		temp_edge_out=index_vertex->edge_out;
		
		NextVertex=Find_AdjVertex_Base_edgedown(temp_edge_out);
	
		

		
		while(temp_edge_out!= NULL)//Go through all the output edges per vertex
		{
				if( NextVertex != NULL )
				{
					//Keep the next one.

					if(index_vertex->GetVex_cylces()!=0 || index_vertex->GetVex_comp()=="DUMMY")
					{
						if(NextVertex->GetVex_height()>=index_vertex->GetVex_height())//Compare with all out_edge's height, pick the heightest one
						{
							if(index_vertex->GetVex_comp()=="DUMMY")
							{
								temp=NextVertex->GetVex_height();
								index_vertex->WriteVex_height(temp);//Update
								tempcycle=index_vertex->GetVex_cylces();
							}
							else
							{
								temp=NextVertex->GetVex_height();//Get the Next heightt
								tempcycle=index_vertex->GetVex_cylces();
								
								temp=temp+tempcycle;
								index_vertex->WriteVex_height(temp);//Update
								
							
							}


							if(tempcycle==2)
							{
								//index_vertex->WriteVex_ALAP(latency-temp-1);//Caculate the ALAP
								index_vertex->WriteVex_ALAP(latency-temp);
							}
							else if(tempcycle==1)
							{
								index_vertex->WriteVex_ALAP(latency-temp);//Caculate the ALAP
							}
							else
							{
								index_vertex->WriteVex_ALAP(0);
							
							}
						

						}
					}
					else//0
					{
						index_vertex->WriteVex_height(0);
					}
				}
				else
				{
						index_vertex->WriteVex_height(0);
				}
			
			temp_edge_out=temp_edge_out->edge_OutNext;

			NextVertex=Find_AdjVertex_Base_edgedown(temp_edge_out);
			

			
		}

}

void ListR::Schedule_Add(int timer)
{
	int maxindex=-1;
	unsigned int count=0;
	unsigned int sizecouter=0;
	//bool check=false;
	int source=this->Get_add_sub();
	vertexR* index_vertex=this->GetVex_head();
	vector<vertexR*> HeightSort;//Sort out the Descencing order.

	while(index_vertex!=NULL)//Go through all the vertex
	{
		if(index_vertex->GetVex_hardware()=="add_sub" && index_vertex->Done !="done" )//need to schedule Add
		{

				//Release resources
				if(index_vertex->Done =="exe")
				{
					index_vertex->Done="done";
					index_vertex->WriteVex_Endtimer(timer);
					if(source<this->Get_add_sub_max())
					{
						source++;	
						this->Write_add_sub(source);
					}
				}



	for(count=0;count<index_vertex->UpTree.size();count++)
			{
				//done or INPUTS
				if(index_vertex->UpTree.at(count)->Done =="done"|| index_vertex->UpTree.at(count)->GetVex_comp() == "INPUTS"
					|| index_vertex->UpTree.at(count)->GetVex_comp() == "nop_start" || index_vertex->UpTree.at(count)->GetVex_comp() == "upend")//Not finish and not inputs then,can not execute
				{
					sizecouter++;
				}

				if(index_vertex->UpTree.at(count)->GetVex_comp() =="DUMMY" && index_vertex->UpTree.at(count)->UpTree.at(0)->Done=="done")
				{
					sizecouter++;
				}

			}



				if(sizecouter==index_vertex->UpTree.size())
				{
					if(index_vertex->Done=="none")
					{
						HeightSort.push_back(index_vertex);
					
					}
					
					
				}
				sizecouter=0;
	
			}

			index_vertex =index_vertex->node_next;
	}
	//After I have the height sorted vector,check the dependency then, I can schedule based on source and height.
	

			int tempsource=0;
			int slackindex=-2;
			if(HeightSort.size()!=0)
			{
				slackindex=Find_Slack_0(HeightSort,timer);
				while(slackindex != -1)
				{		
					index_vertex=HeightSort.at(slackindex);
					index_vertex->WriteVex_Starttimer(timer);
					index_vertex->Done="exe";

					if(source!=0)
					{
						source--;	
						this->Write_add_sub(source);
						HeightSort.erase(HeightSort.begin()+slackindex);//Delete the sorted Max height	
						slackindex=Find_Slack_0(HeightSort,timer);
					}
					else
					{
						tempsource=this->Get_add_sub_max();
						tempsource++;
						this->Write_add_sub_max(tempsource);
						HeightSort.erase(HeightSort.begin()+slackindex);//Delete the sorted Max height		
						slackindex=Find_Slack_0(HeightSort,timer);
					}	
				}
			}



				while(source!=0 && HeightSort.size()!=0 )
				{		
					maxindex=Find_Maxheight(HeightSort);
					index_vertex=HeightSort.at(maxindex);
					index_vertex->WriteVex_Starttimer(timer);
					index_vertex->Done="exe";
					source--;	
					this->Write_add_sub(source);
					HeightSort.erase(HeightSort.begin()+maxindex);//Delete the sorted Max height		
				}		

		
				//Check for slack


		//	cout<<endl<<"The Add source avaialble is "<< source <<" max add"<< this->Get_add_sub_max() <<" timer is: "<<timer<<endl;
	
}

void ListR::Schedule_Logic(int timer)
{
	int maxindex=-1;
	unsigned int sizecouter=0;
	unsigned int count=0;
	//bool check=false;
	int source=this->Get_logic();
	vertexR* index_vertex=this->GetVex_head();
	vector<vertexR*> HeightSort;//Sort out the Descencing order.
	
	while(index_vertex!=NULL)//Go through all the vertex
	{		
		if(index_vertex->GetVex_hardware()=="logic" && index_vertex->Done !="done")
		{		
					//Release resources
				if(index_vertex->Done =="exe")
				{	
					index_vertex->WriteVex_Endtimer(timer);
					index_vertex->Done="done";
					if(source<this->Get_logic_max())
					{
						source++;	
						this->Write_logic(source);
					}
				}

			//Check dependency by going to the up tree.
		for(count=0;count<index_vertex->UpTree.size();count++)
			{
				//done or INPUTS
				if(index_vertex->UpTree.at(count)->Done =="done"|| index_vertex->UpTree.at(count)->GetVex_comp() == "INPUTS"
					|| index_vertex->UpTree.at(count)->GetVex_comp() == "nop_start" || index_vertex->UpTree.at(count)->GetVex_comp() == "upend")//Not finish and not inputs then,can not execute
				{
					sizecouter++;
				}

				if(index_vertex->UpTree.at(count)->GetVex_comp() =="DUMMY" && index_vertex->UpTree.at(count)->UpTree.at(0)->Done=="done")
				{
					sizecouter++;

				}

			}


				if(sizecouter==index_vertex->UpTree.size())
				{
					if(index_vertex->Done=="none")
					{
						HeightSort.push_back(index_vertex);
					
					}
					
				
				}
				sizecouter=0;
	
			}

			index_vertex =index_vertex->node_next;
	}

			int tempsource=0;
			int slackindex=-2;
			if(HeightSort.size()!=0)
			{
				slackindex=Find_Slack_0(HeightSort,timer);
				while(slackindex != -1)
				{		
					index_vertex=HeightSort.at(slackindex);
					index_vertex->WriteVex_Starttimer(timer);
					index_vertex->Done="exe";

					if(source!=0)
					{
						source--;	
						this->Write_logic(source);
						HeightSort.erase(HeightSort.begin()+slackindex);//Delete the sorted Max height	
						slackindex=Find_Slack_0(HeightSort,timer);
					}
					else//if source ==0
					{
						tempsource=this->Get_logic_max();
						tempsource++;
						this->Write_logic_max(tempsource);
						HeightSort.erase(HeightSort.begin()+slackindex);//Delete the sorted Max height		
						slackindex=Find_Slack_0(HeightSort,timer);
					}	
				}
			}


			while(source!=0 && HeightSort.size()!=0 )
			{		
				maxindex=Find_Maxheight(HeightSort);
				index_vertex=HeightSort.at(maxindex);
				index_vertex->WriteVex_Starttimer(timer);
				index_vertex->Done="exe";
				source--;	
				this->Write_logic(source);
				HeightSort.erase(HeightSort.begin()+maxindex);//Delete the Max height		
			}	


				
	//	cout<<endl<<"The Logic source avaialble is "<<source<<" timer is: "<<timer<<endl;
}

void ListR::Schedule_Multi(int timer)
{
	int maxindex=-1;
	unsigned int count=0;
	unsigned int sizecouter=0;
	//bool check=false;
	int source=this->Get_mul();
	vertexR* index_vertex=this->GetVex_head();
	vector<vertexR*> HeightSort;//Sort out the Descencing order.
	while(index_vertex!=NULL)//Go through all the vertex
	{
		if(index_vertex->GetVex_hardware()=="multi" && index_vertex->Done !="done")
		{	

			if((index_vertex->Done =="exe")&&(timer-index_vertex->GetVex_Starttimer()==2))//check for 2 cycle
				{			
					index_vertex->Done="done";
					index_vertex->WriteVex_Endtimer(timer);
					if(source < this->Get_mul_max())
					{	
						source++;
						this->Write_mul(source);//Release the source
					}
				}


				//Check dependency by going to the up tree.
		for(count=0;count<index_vertex->UpTree.size();count++)
			{
				//done or INPUTS
				if(index_vertex->UpTree.at(count)->Done =="done"|| index_vertex->UpTree.at(count)->GetVex_comp() == "INPUTS"
					|| index_vertex->UpTree.at(count)->GetVex_comp() == "nop_start" || index_vertex->UpTree.at(count)->GetVex_comp() == "upend")//Not finish and not inputs then,can not execute
				{
					sizecouter++;
				}

				if(index_vertex->UpTree.at(count)->GetVex_comp() =="DUMMY" && index_vertex->UpTree.at(count)->UpTree.at(0)->Done=="done")
				{
					sizecouter++;

				}

			}


				if(sizecouter==index_vertex->UpTree.size())
				{
					if(index_vertex->Done=="none")
					{
						HeightSort.push_back(index_vertex);
					
					}
					
				}

				sizecouter=0;

			
			}

		index_vertex =index_vertex->node_next;
	}

			int tempsource=0;
			int slackindex=-2;
			if(HeightSort.size()!=0)
			{
				slackindex=Find_Slack_0(HeightSort,timer);
				while(slackindex != -1)
				{		
					index_vertex=HeightSort.at(slackindex);
					
					if(index_vertex->Done =="none")
					{
						
						index_vertex->WriteVex_Starttimer(timer);
						index_vertex->Done="exe";
					

						if(source!=0)
						{
							source--;	
							this->Write_mul(source);	
							HeightSort.erase(HeightSort.begin()+slackindex);//Delete the sorted Max height	
							slackindex = Find_Slack_0(HeightSort,timer);
						}
						else
						{
							tempsource=this->Get_mul_max();
							tempsource++;
							this->Write_mul_max(tempsource);
							HeightSort.erase(HeightSort.begin()+slackindex);//Delete the sorted Max height		
							slackindex=Find_Slack_0(HeightSort,timer);
						}
					}
				}
			}




			//Actuall schedule
			while(source!=0 && HeightSort.size()!=0 )
			{		
				maxindex=Find_Maxheight(HeightSort);
				index_vertex=HeightSort.at(maxindex);

					if(index_vertex->Done =="none")
					{
						index_vertex->WriteVex_Starttimer(timer);
						index_vertex->Done="exe";
						source--;	
						this->Write_mul(source);
						HeightSort.erase(HeightSort.begin()+maxindex);//Delete the Max height		
					
					}	
			}
			
				
		
}

void ListR::Schedule_Reg(int timer)
{
	unsigned int sizecouter=0;
	unsigned int count=0;
//	bool check=true;
	//int source=this->Get_add_sub();
	vertexR* index_vertex=this->GetVex_head();

	while(index_vertex!=NULL)//Go through all the vertex
	{
		if(index_vertex->GetVex_comp()=="REG" && index_vertex->Done !="done")//need to schedule
		{
			
			//Check dependency by going to the up tree.
			for(count=0;count<index_vertex->UpTree.size();count++)
			{

				//done or INPUTS
				if(index_vertex->UpTree.at(count)->Done =="done"|| index_vertex->UpTree.at(count)->GetVex_comp() == "INPUTS"
					|| index_vertex->UpTree.at(count)->GetVex_comp() == "nop_start" || index_vertex->UpTree.at(count)->GetVex_comp() == "upend")//Not finish and not inputs then,can not execute
				{
					sizecouter++;
				}

				if(index_vertex->UpTree.at(count)->GetVex_comp() =="DUMMY" && index_vertex->UpTree.at(count)->UpTree.at(0)->Done=="done")
				{
					
					sizecouter++;

				}


			}

				//If no dependency then, check something else

				if(sizecouter==index_vertex->UpTree.size())
				{
							if(index_vertex->Done =="none")//done
							{
								index_vertex->WriteVex_Starttimer(timer);
								index_vertex->Done="exe";
							}
							else //exe
							{
								index_vertex->Done="done";
								index_vertex->WriteVex_Endtimer(timer);
							}

				}
			}
			sizecouter = 0;
			index_vertex =index_vertex->node_next;
	}
		
}

void ListR::Reg_Release(int timer)

{
	vertexR* index_vertex=this->GetVex_head();

	while(index_vertex!=NULL)//Go through all the vertex
	{
		if(index_vertex->GetVex_comp()=="REG" )//need to schedule
		{
				if(index_vertex->Done =="exe")
				{
					index_vertex->Done="done";
					index_vertex->WriteVex_Endtimer(timer);
				}
		
		}

			index_vertex =index_vertex->node_next;
	}
			

}
void ListR::Add_Release(int timer)
{
	//int maxindex=-1;
	//int count=0;
	//bool check=true;
	int source=this->Get_add_sub();
	vertexR* index_vertex=this->GetVex_head();
	vector<vertexR*> HeightSort;//Sort out the Descencing order.

	while(index_vertex!=NULL)//Go through all the vertex
	{
		if(index_vertex->GetVex_hardware()=="add_sub" && index_vertex->Done !="done" )//need to schedule Add
		{

				//Release resources
				if(index_vertex->Done =="exe")
				{
					index_vertex->Done="done";
					index_vertex->WriteVex_Endtimer(timer);
					if(source<this->Get_add_sub_max())
					{
						source++;	
						this->Write_add_sub(source);
					}
				}
			}

			index_vertex =index_vertex->node_next;
	}



}
void ListR::Logic_Release(int timer)
{
	//int maxindex=-1;
	//int count=0;
	//bool check=true;
	int source=this->Get_logic();
	vertexR* index_vertex=this->GetVex_head();
	vector<vertexR*> HeightSort;//Sort out the Descencing order.
	
	while(index_vertex!=NULL)//Go through all the vertex
	{		
		if(index_vertex->GetVex_hardware()=="logic" && index_vertex->Done !="done")
		{		
					//Release resources
				if(index_vertex->Done =="exe")
				{	
					index_vertex->WriteVex_Endtimer(timer);
					index_vertex->Done="done";
					if(source<this->Get_logic_max())
					{
						source++;	
						this->Write_logic(source);
					}
				}
		}

			index_vertex =index_vertex->node_next;
	}




}
void ListR::Multi_Release(int timer)
{
//	int maxindex=-1;
//	int count=0;
//	bool check=true;
	int source=this->Get_mul();
	vertexR* index_vertex=this->GetVex_head();
	vector<vertexR*> HeightSort;//Sort out the Descencing order.

	while(index_vertex!=NULL)//Go through all the vertex
	{
		if(index_vertex->GetVex_hardware()=="multi" && index_vertex->Done !="done")
		{	

				//Release resources

			if(index_vertex->Done =="exe" &&(timer-index_vertex->GetVex_Starttimer()==2))//check for 2 cycles
				{			
					index_vertex->Done="done";
					index_vertex->WriteVex_Endtimer(timer);
					if(source < this->Get_mul_max())
					{	
						source++;
						this->Write_mul(source);//Release the source
					}
				}
			}

			index_vertex =index_vertex->node_next;
	}


}

int ListR::Find_Maxheight(vector<vertexR*> HeightSort)
{
	int tempcount=0;
	int tempmaxheight=0;

	for(unsigned int size=0;size<HeightSort.size();size++)
	{
		if(tempmaxheight < HeightSort.at(size)->GetVex_height())//Find the max height
		{
			tempcount=size;//find the count
			tempmaxheight=HeightSort.at(size)->GetVex_height();//Keep the hight
		}
	
	}
	return tempcount;
}

int ListR::Find_Slack_0(vector<vertexR*> HeightSort,int timer)
{
	int tempcount=0;
	//int tempALAP=0;
	int buffer=0;

	for(unsigned int size=0;size<HeightSort.size();size++)
	{	
		buffer=HeightSort.at(size)->GetVex_ALAP()- timer+1;
		//buffer=HeightSort.at(size)->GetVex_ALAP()- timer;
		if(buffer == 0 )//
		{
			tempcount=size;//find the count
			return tempcount;
		}
	}
	return -1;
}

int ListR::Find_MaxFinishtime()
{
	int maxtime=0;
	vertexR* index=this->GetVex_head();

	while(index!=NULL)
	{

		if(maxtime < index->GetVex_Endtimer())//Find the max height
		{
			maxtime=index->GetVex_Endtimer();
			//maxtime=index->GetVex_Starttimer();
		}
		index =index->node_next;
	}
		
	
  return maxtime-1;
//	return maxtime;


}

/*Test functions*/
void ListR::printout_Hardware()
{

	cout<< endl;
	vertexR* index=this->GetVex_head();
	while(index!=NULL){
		cout<<" vertex_order:"<<index->GetVex_order()<<" components:"<<index->GetVex_comp()
		<<" hardware: "<<index->GetVex_hardware()<<" "<< " cycles: "<<index->GetVex_cylces()<<endl;
		index =index->node_next;
	}


}

void ListR::printout_max_hardware()
{
	cout<<" max multi unit: "<<this->Get_mul_max()<<" max adder unit: "<<this->Get_add_sub_max()<<" max logic unit: "<<this->Get_logic_max()<<endl;
	
}	
void ListR::printout_ALAP()
{
	cout<< endl;
	vertexR* index=this->GetVex_head();
	while(index!=NULL){
	cout<<" vertex_order:"<<index->GetVex_order()<<" components:"<<index->GetVex_comp()
		<<" hardware: "<<index->GetVex_hardware()<<" "<< " height: "<<index->GetVex_height()
    	<< " ALAP: "<<index->GetVex_ALAP()<<endl;
		index =index->node_next;
	}



}
	
//printout_VerList() this is a test function,by printing out all the vertex
void ListR::printout_VerList()
{
	cout<< endl;
	vertexR* index=this->GetVex_head();
	while(index!=NULL){
		cout<<" vertex_order:"<<index->GetVex_order()<<" component: "<<index->GetVex_comp()//<<" "<<" name: "<<index->
			<<" "<<" height: "<<index->GetVex_height()<<" "<<" finish: "<<index->Done<<" "
			<<" Start timer: "<<index->GetVex_Starttimer()
			<<" End timer: "<<index->GetVex_Endtimer()<< endl;
		index =index->node_next;
	}

}


//printout_EdgeList() this is a test function,by printing out all the edges
void ListR::printout_EdgeList()
{
	cout<<endl;
	edgeR* index= this->GetEdge_head();
	while(index!=NULL){

		cout<<"edge_name:"<< index->GetEdge_value()<<" edge_order:"<<index->GetEdge_order()<<" node_in:"<<index->node_in<<" node_out:"<<index->node_out<<endl;
		index =index->edge_next;
	}

}

//printout_OutLink() this is a test function,by printing out all the edges that go out the vertex
void ListR::printout_OutLink()
{
	cout<<endl;
	cout<<"the edges go out from vertex:"<<endl;
    vertexR* index_vertex=this->GetVex_head();
	edgeR* index_edge=NULL;
	while(index_vertex!=NULL){
		cout<<"vertex "<<index_vertex->GetVex_order()<<": ";
        index_edge=index_vertex->edge_out;
		while(index_edge!=NULL){
			cout<<index_edge->GetEdge_order()<<" ";
			index_edge =index_edge->edge_OutNext;
		}
		index_vertex =index_vertex->node_next;
        cout<<endl;
	}

}
//printout_Inlink() this is a test function,by printing out all the edges that go to the vertex
void ListR::printout_Inlink()
{
	cout<<endl;
	cout<<"the edges go into vertex:"<<endl;
	vertexR* index_vertex=this->GetVex_head();
	edgeR* index_edge=NULL;
	while(index_vertex!=NULL){
		cout<<"vertex "<<index_vertex->GetVex_order()<<": ";
        index_edge=index_vertex->edge_in;
		while(index_edge!=NULL){
			cout<<index_edge->GetEdge_order()<<" ";
			index_edge =index_edge->edge_InNext;
		}
		index_vertex =index_vertex->node_next;
        cout<<endl;
	}
   

}
//release the Memory
void ListR::Rel_Memory()
{
   
	// delete vertex
	if (this->GetVex_head()==NULL)
	{
		return;
	}
	vertexR* indexV=this->GetVex_head();
	vertexR* dele_vertex =NULL;
	while(indexV!=NULL){
		dele_vertex =indexV;
		indexV =indexV->node_next;

        delete dele_vertex;
		dele_vertex =NULL;
	}

	// delete edge
	if (this->GetEdge_head()==NULL)
	{
		return;
	}
	edgeR* indexE=this->GetEdge_head();
	edgeR* dele_edge =NULL;
	while(indexE!=NULL){
		dele_edge =indexE;
		indexE =indexE->edge_next;

        delete dele_edge;
		dele_edge =NULL;
	}

	

}
