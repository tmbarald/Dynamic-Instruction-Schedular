#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cmath>
#include <iomanip>
#include "sim_proc.h"
// define globals

#define IF	"IF"
#define ID	"ID"
#define IS	"IS"
#define EX	"EX"
#define WB	"WB"

char* tf;
unsigned int N = 0;
unsigned int S = 0;
unsigned int DN = 0;
int BLOCKSIZE = 0;
int SIZE = 0;
int ASSOC = 0;
int SIZE2=0;
int ASSOC2 = 0;
int cycles = 0;
int PC = 0;
using namespace std;

void readvals(char** args){
	S = atoi(args[1]);
	N = atoi(args[2]);
	DN = 2*N;
	tf = args[8];

}

void printresults(){

	float IPC = (float)PC/(float)cycles;
	cout << "CONFIGURATION\n";
	cout << " superscalar bandwidth (N) = " << N << "\n";
	cout << " dispatch queue size (2*N) = " << DN << "\n";
	cout << " schedule queue size (S)   = " << S << "\n";
	cout << "RESULTS\n";
	cout << " number of instructions = " << PC << "\n";
	cout << " number of cycles       = " << cycles << "\n";
	cout << " IPC                    = " << setprecision(3) << IPC << "\n";

}

void fetch(ifstream& infile, Dispatch_List& dl, Fake_ROB& frob){
	
	for(unsigned int i = 0; i < dl.dispatch_list.size(); i++){

		if(dl.dispatch_list[i].state == IF){
			dl.dispatch_list[i].state = ID;
			//cout << dl.dispatch_list[i].tag << " on cycle " << cycles << "\n";
			dl.dispatch_list[i].if_end = cycles;	
			dl.dispatch_list[i].id_start = cycles;
			int tag = dl.dispatch_list[i].tag;
			for(unsigned int j = 0; j < frob.frob.size(); j++){
				if(tag == frob.frob[j].tag){
					frob.frob[j].state = ID;
					frob.frob[j].if_end = cycles;
					frob.frob[j].id_start = cycles;
				}
			}
		}
	}



	if(infile.peek() == EOF){

		return;
	}
	  string tag, opcode, dest, r1, r2, mem;
	unsigned int fetch_bandwidth = 0;		
	if(dl.dispatch_list.size() < DN){
		while(infile >> tag >> opcode >> dest >> r1 >> r2 >> mem){

		
		signed int ir1 = atoi(r1.c_str());
		signed int ir2 = atoi(r2.c_str());
		unsigned int iopcode = atoi(opcode.c_str());
		signed int idest = atoi(dest.c_str());
		
		//cout << opcode << " " << dest << " " << ir1 << " " << ir2 << "\n";
		Instruction il = Instruction(PC, iopcode, idest, ir1, ir2);
                il.if_start = cycles;
                PC++;
                dl.dispatch_list.push_back(il);
                frob.frob.push_back(il);
	//	cout << PC << "\n";
//		frob.printvals(frob.frob.size()-1);
		fetch_bandwidth++;
//		cout << "read line "<<tag << " and the size of the list is " << dl.dispatch_list.size() << "\n";
			if(dl.dispatch_list.size() >= DN || fetch_bandwidth >= N)
				return;
			
		
		}

	}
}

void execute(Dispatch_List& dl, Issue_List& il, Execute_List& el, Fake_ROB& frob, Register_File& rf){

	
	for(unsigned int i = 0; i < el.execute_list.size(); i++){


		if(cycles >= el.execute_list[i].ex_end){

			int tag = el.execute_list[i].tag;
		//	 cout << " here 3 \n";

			for(unsigned int j = 0; j < frob.frob.size(); j++){
				if(tag == frob.frob[j].tag){
					frob.frob[j].wb_start = cycles;
					frob.frob[j].state = WB;
				//	index.push_back(i);
					// cout << " here 4 \n";		
					if(el.execute_list[i].dest_reg > -1 ){
						rf.ready[el.execute_list[i].dest_reg] = true;
						for(unsigned int p = 0; p < il.issue_list.size(); p++){
							bool finder = false;
	//						if(il.issue_list[p].tag == 47 && el.execute_list[i].tag == 46){
	//								cout << cycles << " this program sucks \n";
	//						}
							if(el.execute_list[i].dest_reg == il.issue_list[p].reg_src1 && el.execute_list[i].tag < il.issue_list[p].tag){
								for(unsigned int test = 0; test < el.execute_list.size(); test++){
                                                                        if(el.execute_list[i].dest_reg == el.execute_list[test].dest_reg && el.execute_list[i].tag < el.execute_list[test].tag){
                                                                        if(el.execute_list[test].tag < il.issue_list[p].tag){
                                                                                finder = true;
                                                                                break;
                                                                        }
                                                                        }
                                                                }
								if(!finder){
								il.issue_list[p].r1Ready = true;
								il.issue_list[p].r1Un = true;
	//							if(il.issue_list[p].tag == 47){
          //                                                              cout << "at cycle " << cycles << " tag : " << il.issue_list[p].tag << " was activated \n";
            //                                                    }
								}
							}
							if(finder)
								break;
	
							if(el.execute_list[i].dest_reg ==  il.issue_list[p].dest_reg && el.execute_list[i].tag < il.issue_list[p].tag){
								break;
							}
					
	
						}
						for(unsigned int m = 0; m < il.issue_list.size(); m++){
							bool finder = false;
							if(el.execute_list[i].dest_reg == il.issue_list[m].reg_src2 && el.execute_list[i].tag < il.issue_list[m].tag){
                                                                for(unsigned int test = 0; test < el.execute_list.size(); test++){
									if(el.execute_list[i].dest_reg == el.execute_list[test].dest_reg && el.execute_list[i].tag < el.execute_list[test].tag){
									if(el.execute_list[test].tag < il.issue_list[m].tag){
										finder = true;
										break;
									}
									}	
								}
								
								if(!finder){
								il.issue_list[m].r2Ready = true;
								il.issue_list[m].r2Un = true;
								if(il.issue_list[m].tag == 1577){
									cout << "at cycle " << cycles << " tag : " << il.issue_list[m].tag << " was activated \n";
								}
								}
                                                        }
                                                        if((el.execute_list[i].dest_reg ==  il.issue_list[m].dest_reg) && (el.execute_list[i].tag < il.issue_list[m].tag)){
                                                                break;
                                                        }
							if(finder)
								break;
				
						}
						bool reg1InIs = false;

						for(unsigned int no = 0; no < il.issue_list.size(); no++){
							if(el.execute_list[i].dest_reg == il.issue_list[no].dest_reg){
								reg1InIs = true;
							}
	
						}

						if(!reg1InIs){
						for(unsigned int o = 0; o < dl.dispatch_list.size(); o++){
							bool finder = false;
							if(el.execute_list[i].dest_reg == dl.dispatch_list[o].reg_src1 && el.execute_list[i].tag < dl.dispatch_list[o].tag && dl.dispatch_list[o].state == ID){
								for(unsigned int test = 0; test < el.execute_list.size(); test++){
									if(el.execute_list[i].dest_reg == el.execute_list[test].dest_reg && el.execute_list[i].tag < el.execute_list[test].tag){
										if(el.execute_list[test].tag < dl.dispatch_list[o].tag){
											finder = true;
											break;
										}
									}	
								}
								for(unsigned int pop = 0; pop < il.issue_list.size(); pop++){
									if(el.execute_list[i].dest_reg == il.issue_list[pop].dest_reg && el.execute_list[i].tag < il.issue_list[pop].tag){
	
										if(il.issue_list[pop].tag < dl.dispatch_list[o].tag){
											finder = true;
											break;
										}
										

									}	
								}
								if(!finder){
									dl.dispatch_list[o].r1Ready = true;
									dl.dispatch_list[o].r1Un = true;
								}
							}
						
							if((el.execute_list[i].dest_reg == dl.dispatch_list[o].dest_reg) && el.execute_list[i].tag < dl.dispatch_list[o].tag){
								break;
							}
							if(finder)
								break;	
						}
						}
						bool reg2InIs = false;
						for(unsigned int lol = 0; lol < il.issue_list.size(); lol++){
							if(el.execute_list[i].dest_reg == il.issue_list[lol].dest_reg){
								reg2InIs = true;
							}


						}

						if(!reg2InIs){
						for(unsigned int n = 0; n < dl.dispatch_list.size(); n++){
						
							bool finder = false;
							if(el.execute_list[i].dest_reg == dl.dispatch_list[n].reg_src2 && el.execute_list[i].tag < dl.dispatch_list[n].tag && dl.dispatch_list[n].state == ID){
								for(unsigned int test = 0; test < el.execute_list.size(); test++){
									if(el.execute_list[i].dest_reg == el.execute_list[test].dest_reg && el.execute_list[i].tag < el.execute_list[test].tag){
										if(el.execute_list[test].tag < dl.dispatch_list[n].tag){
											finder = true;
											break;
										}
									}
								}
								for(unsigned int t = 0; t < il.issue_list.size(); t++){
					
									if(il.issue_list[t].dest_reg == el.execute_list[i].dest_reg && el.execute_list[i].tag < el.execute_list[t].tag){
										if(il.issue_list[t].tag < dl.dispatch_list[n].tag){
											finder = true;
											break;
										}
									}

								}
								if(!finder){
									dl.dispatch_list[n].r2Ready = true;
									dl.dispatch_list[n].r2Un = true;
								}
							}
	
							if((el.execute_list[i].dest_reg == dl.dispatch_list[n].dest_reg) && el.execute_list[i].tag < dl.dispatch_list[n].tag){
								break;
							}
							if(finder)
								break;

						}
						}
					}
					 				
					el.execute_list.erase(el.execute_list.begin() + i);
					i--;
					// cout << " here 5 \n";

				}

			}
			

		}
		

	}


}

void issue(Issue_List& il, Execute_List& el, Fake_ROB& frob, Register_File& rf){

	unsigned int bandwidth = 0;



	bool wasUsed = true;


	while(bandwidth < N && il.issue_list.size() > 0 && wasUsed){
		for(unsigned int i = 0; i < il.issue_list.size(); i++){
			int dest = il.issue_list[i].dest_reg;
			int r1 = il.issue_list[i].reg_src1;
			int r2 = il.issue_list[i].reg_src2;
			int tag = il.issue_list[i].tag;
			wasUsed = false;
			if(r1 < 0 || il.issue_list[i].r1Un){
				il.issue_list[i].r1Ready = true;


			}
			else{

			
				if(rf.ready[r1]){
					il.issue_list[i].r1Ready =true;
				}		
				

				 unsigned int index = 0;
				if(!frob.frob.empty()){
                                for(index = index; index < frob.frob.size(); index++){
                                        if(frob.frob[index].tag == tag){
                                                break;
                                        }

                                }
				}
			

                                index--;
				int s_index = (signed)index;
				if(!frob.frob.empty() && s_index > -1){
                                for(s_index = s_index; s_index >= 0; s_index--){
                                        if(frob.frob[s_index].dest_reg == r1){
                                                if(frob.frob[s_index].state == WB){
                                                        il.issue_list[i].r1Ready = true;
                                                }
                                                else{
                                                        il.issue_list[i].r1Ready = false;
                                                }
                                                break;
                                        }

                                }
				}

     

			}
			if(r2 < 0 || il.issue_list[i].r2Un){
				il.issue_list[i].r2Ready = true;
			}
			else{
     
				if(rf.ready[r2]){
					il.issue_list[i].r2Ready = true;
				}

				unsigned int index = 0;
				if(!frob.frob.empty()){
				for(index = index; index < frob.frob.size(); index++){
					if(frob.frob[index].tag == tag){
						break;
					}	

				}
				}
				index--;
				int s_index = (signed)index;
				if(!frob.frob.empty() && s_index > -1){
				for(s_index = s_index; s_index >= 0; s_index--){
					if(frob.frob[s_index].dest_reg == r2){
						if(frob.frob[s_index].state == WB){
							il.issue_list[i].r2Ready = true;
						}
						else{
							il.issue_list[i].r2Ready = false;
						}
						break;
					}

				}}
		}
                 if(il.issue_list[i].r1Ready && il.issue_list[i].r2Ready){

				for(unsigned int j = 0; j < frob.frob.size(); j++){	
					if(tag == frob.frob[j].tag){

						frob.frob[j].state = EX;
						frob.frob[j].is_end = cycles;
						frob.frob[j].ex_start = cycles;
						if(frob.frob[j].opcode == 0)
							frob.frob[j].ex_end = cycles+1;
						else if(frob.frob[j].opcode == 1)
							frob.frob[j].ex_end = cycles+2;
						else
							frob.frob[j].ex_end = cycles+5;

						
						bandwidth++;
						el.execute_list.push_back(frob.frob[j]);
						il.issue_list.erase(il.issue_list.begin() + i);
						i--;
					
						wasUsed=true;
						if(dest>-1)
							rf.ready[dest] = false;
					 
						if(bandwidth >= N)
							return;	
					}

				}	
						
	
			}
		}
	}
	
}
void dispatch(Issue_List& il, Dispatch_List& dl, Fake_ROB& frob){

	int i = 0;
	unsigned int bandwidth = 0;

	Dispatch_List temp_list;

	for(unsigned int k = 0; k < dl.dispatch_list.size(); k++){

		if(dl.dispatch_list[k].state == ID){
			temp_list.dispatch_list.push_back(dl.dispatch_list[k]);

		}

	}


	
	while((il.issue_list.size() < S) && (!temp_list.dispatch_list.empty()) && (bandwidth < N)){
		
		if(temp_list.dispatch_list[i].state == ID){
			int tag = temp_list.dispatch_list[i].tag;
			for(unsigned int j = 0; j < frob.frob.size(); j++){
				if(tag == frob.frob[j].tag){
			//		cout << " here in dispatch right \n";
					frob.frob[j].id_end = cycles;
					frob.frob[j].is_start = cycles;
					frob.frob[j].state = IS;
					if(temp_list.dispatch_list[i].r2Un == true){
						frob.frob[j].r2Un = true;
					}
					if(temp_list.dispatch_list[i].r1Un == true){
						frob.frob[j].r1Un = true;
					}
					il.issue_list.push_back(frob.frob[j]);
					dl.dispatch_list.erase(dl.dispatch_list.begin());
//					index.erase(index.begin());
//					indexfixer++;
					temp_list.dispatch_list.erase(temp_list.dispatch_list.begin());
					bandwidth++;
					if(bandwidth >= N){
						return;	
					}
					break;
				}
			}	
		}								
	//	cout << "stuck here \n";	
	}

	
	
	


}
void fake_retire(Fake_ROB& frob){

	if(!frob.frob.empty()){

	while(frob.frob[0].state == WB){
	
		cout << frob.frob[0].tag << " fu{" << frob.frob[0].opcode << "} ";
	
		cout << "src{" << frob.frob[0].reg_src1 << "," << frob.frob[0].reg_src2 << "} ";
	
		cout << "dst{" << frob.frob[0].dest_reg << "} ";
	
		cout << "IF{" << frob.frob[0].if_start << "," << frob.frob[0].if_end-frob.frob[0].if_start << "} ";
	
		cout << "ID{" << frob.frob[0].id_start << "," << frob.frob[0].id_end-frob.frob[0].id_start << "} ";
	
		cout << "IS{" << frob.frob[0].is_start << "," << frob.frob[0].is_end-frob.frob[0].is_start << "} ";
	
	
		cout << "EX{" << frob.frob[0].ex_start << "," << frob.frob[0].ex_end-frob.frob[0].ex_start << "} ";
	
		cout << "WB{" << frob.frob[0].wb_start << "," << "1" << "} ";
	
		cout <<"\n";
		frob.frob.erase(frob.frob.begin());

	}
	}



}

bool AdvanceCycle(ifstream& infile, Fake_ROB& frob){


	if((infile.peek() == EOF) && frob.frob.empty()){
		return false;
	}
	else{
		//cout << cycles << "\n";
		cycles++;
		return true;
	}


}
int main(int argc, char** argv){



	Dispatch_List dl;
	Issue_List il;
	Execute_List el;
	Fake_ROB frob;
	Register_File rf;
	if(argc == 9){
		readvals(argv);
	}
	else{
		cout<<"not enough inputs";
	}


	ifstream infile(tf);
	do{
		fake_retire(frob);
	//	cout << "cycle is " << cycles << "here" << "and frob size is  " << frob.frob.size() << "\n";
		execute(dl, il, el, frob, rf);
	//	cout << "cycle is " << cycles << "here and execute lis size is " << el.execute_list.size() << "\n";
		issue(il, el, frob, rf);
		
	//	cout << "cycle is " << cycles << "here and issue lis size is " << il.issue_list.size() << "\n";


		dispatch(il, dl, frob);
		
	//	cout << "cycle is " << cycles << "here and dispatch list size is " << dl.dispatch_list.size() << "\n";;


		fetch(infile, dl, frob);
//	 cout << "cycle is " << cycles << "here and dispatch list size is " << dl.dispatch_list.size() << "\n";;

//		cout << "cycle is " << cycles << " here\n";
	

	}while(AdvanceCycle(infile, frob));
		

	printresults();	
	return 0;
}
