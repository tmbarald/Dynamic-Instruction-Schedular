#ifndef SIM_PROC_H
#define SIM_PROC_H
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <deque>
#define IF      "IF"
#define ID      "ID"
#define IS      "IS"
#define EX      "EX"
#define WB      "WB"

extern int cycles;

class Instruction{

	public:
	int tag;
	int opcode;
	int dest_reg;
	int reg_src1;
	int reg_src2;
	int memaddress;
	int if_start;
	int if_end;
	int id_start;
	int id_end;
	int is_start;
	int is_end;
	int ex_start;
	int ex_end;
	int wb_start;
	int wb_end;
	std::string state;
	bool r1Ready;
	bool r2Ready;
	bool r1Un;
	bool r2Un;

	public:
	Instruction(int tg, int opc, int dr, int r1, int r2){
		tag = tg;
		opcode = opc;
		dest_reg = dr;
		reg_src1 = r1;
		reg_src2 = r2;
		memaddress = 0; //ignore
		state = "IF";
		r1Ready = r2Ready = false;	
		r1Un = r2Un = false;
	}
	

};

class Fake_ROB{

	public:
	std::deque<Instruction> frob;

	public:
	void printvals(int index){
		std::cout << "tag = " << frob[index].tag << " opcode = " << frob[index].opcode << " source 1 = " << frob[index].reg_src1 << " source 2 = " << frob[index].reg_src2 << " dest = " << frob[index].dest_reg <<" On cycle " << cycles << "\n";
	}
};

class Register_File{

	public:
	std::vector<int> reg_name;
	std::vector<bool> ready;


	public:
	Register_File(){
		for(int i = 0; i < 128; i++){
			reg_name.push_back(i);
			ready.push_back(true);
		}
	}
	
};

class Dispatch_List{

	public:
	std::vector<Instruction> dispatch_list;

};

class Issue_List{

	public:
		std::vector<Instruction> issue_list;


};

class Execute_List{

	public:
		std::vector<Instruction> execute_list;

};

#endif



