# HighLevelSynthesizer-to-verilog

--- Implement no scheduling HSLM, please use argument: -ns cfile verilogfile ---  
"cfile" is the C-like statements;   
"verilogfile" is the HLSM verilog file produced by program according to cfile;   

--- Implement ListL scheduling Algorithm, please use argument: -listl cfile verilogfile mul add logic ---   
"cfile" is the C-like statements;  
"verilogfile" is the HLSM verilog file produced by program according to cfile;   
"mul" is the number of multipliers;   
"add" is the number of adders;   
"logic" is the number of other logics;   

--- Implement ListR scheduling Algorithm, please input: -listr cfile verilogfile latency ---      
"cfile" is the C-like statements;   
"verilogfile" is the HLSM verilog file produced by program according to cfile;    
"latency" is the time constrain for scheduling;   
		
provide 6 scheduling files and 8 error files
