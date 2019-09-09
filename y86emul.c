
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>



int registers[8]; //Registers

int OF, ZF, SF; //Condition codes

unsigned int pc; //Program counter

int progsize; //Size of the program - In bytes

typedef union{ // two 4 bit values representing instructions. In bytes
  struct{
    unsigned int higher : 4;
    unsigned int lower : 4;
  } bits;
  char byteVal;
} aByte;

aByte * memptr; //Memory of the y86. This is a pointer to the memory of the byte.


typedef union{ //32 bit values represented in 4 bytes
	struct{
		char byte1;
		char byte2;
		char byte3;
		char byte4;
	}byteChars;

	int intVal;
} intChar;



int getHexVal(char c){
	if(c >= '0' && c <= '9'){
		return c-'0';
	}
	if(c >= 'A' && c <= 'F'){
		return 10+c-'A';

	}
	if(c >= 'a' && c <= 'f'){
		return 10+c-'a';

	}
	fprintf(stderr, "Invali hex Ascii");
	exit(1);


}

void pcCheck(unsigned int ip){
	if(pc>=progsize){
		fprintf(stderr, "Error: Status-ADR. Invalid adress given, out of bounds of program size.");
		exit(1);
	}

}


void rrmovl(){


	if(memptr[pc].bits.higher!=0){
		fprintf(stderr, "Error: Status-INS. Invalid bits for rrmovl, higher bits not 0 of first byte.");
		exit(1);
	}


	pcCheck(pc);
	pc+=1;
	registers[memptr[pc].bits.higher]=registers[memptr[pc].bits.lower];
	pcCheck(pc);
	pc+=1;
}

void irmovl(){

	if(memptr[pc].bits.higher!=0){
		fprintf(stderr, "Error: Status-INS. Invalid bits for irmovl, higher bits not 0 of first byte.");
		exit(1);
	}


	pcCheck(pc);
	pc+=1;

	if(memptr[pc].bits.lower!=15){
		fprintf(stderr, "Error: Status-INS. Invalid in irmovl, register value not F");
		exit(1);
	}

	unsigned int rb=memptr[pc].bits.higher;

	intChar imm;

	pcCheck(pc);
	pc+=1;
	imm.byteChars.byte1=memptr[pc].byteVal;
	pcCheck(pc);
	pc+=1;
	imm.byteChars.byte2=memptr[pc].byteVal;
	pcCheck(pc);
	pc+=1;
	imm.byteChars.byte3=memptr[pc].byteVal;
	pcCheck(pc);
	pc+=1;
	imm.byteChars.byte4=memptr[pc].byteVal;

	registers[rb]=imm.intVal;
	pcCheck(pc);
	pc+=1;
}

void rmmovl(){

	if(memptr[pc].bits.higher!=0){
		fprintf(stderr, "Error: Status-INS. Invalid bits for rmmovl, higher bits not 0 of first byte.");
		exit(1);
	}

	pcCheck(pc);
	pc+=1;

	unsigned int ra=memptr[pc].bits.lower;
	unsigned int rb=memptr[pc].bits.higher;

	intChar displacement;

	pcCheck(pc);
	pc+=1;
	displacement.byteChars.byte1=memptr[pc].byteVal;
	pcCheck(pc);
	pc+=1;
	displacement.byteChars.byte2=memptr[pc].byteVal;
	pcCheck(pc);
	pc+=1;
	displacement.byteChars.byte3=memptr[pc].byteVal;
	pcCheck(pc);
	pc+=1;
	displacement.byteChars.byte4=memptr[pc].byteVal;


	displacement.intVal=displacement.intVal + registers[rb];

	if(displacement.intVal < 0 || displacement.intVal+3>=progsize){
		fprintf(stderr, "Error: Status-ADR. error in rmmovl, out of bounds");
		exit(1);
	}


	intChar output;
	output.intVal=registers[ra];


	memptr[displacement.intVal].byteVal=output.byteChars.byte1;
	memptr[displacement.intVal+1].byteVal=output.byteChars.byte2;
	memptr[displacement.intVal+2].byteVal=output.byteChars.byte3;
	memptr[displacement.intVal+3].byteVal=output.byteChars.byte4;

	pcCheck(pc);
	pc+=1;
	
}

void mrmovl(){

	if(memptr[pc].bits.higher!=0){
		fprintf(stderr, "Error: Status-INS- Higher bits of first byte in mrmovl not 0.");
		exit(1);
	}
	pcCheck(pc);
	pc+=1;

	unsigned int ra=memptr[pc].bits.lower;
	unsigned int rb=memptr[pc].bits.higher;

	intChar displacement;

	pcCheck(pc);
	pc+=1;
	displacement.byteChars.byte1=memptr[pc].byteVal;
	pcCheck(pc);
	pc+=1;
	displacement.byteChars.byte2=memptr[pc].byteVal;
	pcCheck(pc);
	pc+=1;
	displacement.byteChars.byte3=memptr[pc].byteVal;
	pcCheck(pc);
	pc+=1;
	displacement.byteChars.byte4=memptr[pc].byteVal;

	displacement.intVal=displacement.intVal+registers[rb];

	if(displacement.intVal < 0 || displacement.intVal+3>=progsize){
		fprintf(stderr, "Error: Status-ADR. error in rmmovl, out of bounds");
		exit(1);
	}

	intChar output;


	output.byteChars.byte1=memptr[displacement.intVal].byteVal;
	output.byteChars.byte2=memptr[displacement.intVal+1].byteVal;
	output.byteChars.byte3=memptr[displacement.intVal+2].byteVal;
	output.byteChars.byte4=memptr[displacement.intVal+3].byteVal;


	registers[ra]=output.intVal;

	pcCheck(pc);
	pc+=1;

}


void addl(){
	int valRa=registers[memptr[pc].bits.lower];
	int valRb=registers[memptr[pc].bits.higher];

	if(valRb>0 && valRa>0 && valRa+valRb<0){
		OF=1;
	}

	else if(valRb<0 && valRa<0 && valRa+valRb>0){
		OF=1;
	}
	else{
		OF=0;
	}
	int value=valRb+valRa;
	registers[memptr[pc].bits.higher]=value;

	if (value==0){
		ZF=1;
	}
	else{
		ZF=0;
	}

	if(value<0){
		SF=1;
	}
	else{
		SF=0;
	}
}

void subl(){
	int valRa=registers[memptr[pc].bits.lower];
	int valRb=registers[memptr[pc].bits.higher];

	if(valRb<0 && valRa>0 && valRa-valRb<0){
		OF=1;
	}
	else if(valRa<0 && valRb>0 && valRa-valRb>0){
		OF=1;
	}
	else{
		OF=0;
	}
	int value=valRb-valRa;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ;

	registers[memptr[pc].bits.higher]=value;

	if(value==0){
		ZF=1;
	}
	else{
		ZF=0;
	}

	if(value<0){
		SF=1;
	}
	else{
		SF=0;
	}

}

void andl(){
	int value=registers[memptr[pc].bits.lower] & registers[memptr[pc].bits.higher];

	OF=0;

	if(value==0){
		ZF=1;
	}
	else{
		ZF=0;
	}

	if(value<0){
		SF=1;
	}
	else{
		SF=0;
	}
}

void xorl(){
	int value=registers[memptr[pc].bits.lower] ^ registers[memptr[pc].bits.higher];

	OF=0;

	if(value==0){
		ZF=1;
	}
	else{
		ZF=0;
	}

	if(value<0){
		SF=1;
	}
	else{
		SF=0;
	}
}


void mull(){
	int valRa=registers[memptr[pc].bits.lower];
	int valRb=registers[memptr[pc].bits.higher];

	if(valRa>0 && valRb>0 && valRb*valRa<0){
		OF=1;
	}
	else if(valRa<0 && valRb<0 && valRb*valRa<0){
		OF=1;
	}
	else if(valRa>0 && valRb<0 && valRb*valRa>0){
		OF=1;
	}
	else if(valRa<0 && valRb>0 && valRb*valRa>0){
		OF=1;
	}
	else{
		OF=0;
	}

	int value=valRb*valRa;
	registers[memptr[pc].bits.higher]=value;

	if(value==0){
		ZF=1;
	}
	else{
		ZF=0;
	}

	if(value<0){
		SF=1;
	}
	else{
		SF=0;
	}
}

void cmpl(){

	int valRa=registers[memptr[pc].bits.lower];
	int valRb=registers[memptr[pc].bits.higher];

	OF=0;

	int value=valRb - valRa;

	if(value==0){
		ZF=1;
	}
	else{
		ZF=0;
	}

	if(value<0){
		SF=1;
	}
	else{
		SF=0;
	}

}

void operation1(){
	int op=memptr[pc].bits.higher;
	pcCheck(pc);
	pc+=1;

	switch (op){
		case 0:
		addl();
		break;

		case 1:
		subl();
		break;

		case 2:
		andl();
		break;

		case 3:
		xorl();
		break;

		case 4:
		mull();
		break;

		case 5:
		cmpl();
		break;

		default:

		fprintf(stderr, "Error: Status- INS. Imporper op code in operation 1.");
		exit(1);
	}

	pcCheck(pc);
	pc+=1;
}



void jmp(){
	intChar dst;

	dst.byteChars.byte1=memptr[pc].byteVal;
	pcCheck(pc);
	pc+=1;
	dst.byteChars.byte2=memptr[pc].byteVal;
	pcCheck(pc);
	pc+=1;
	dst.byteChars.byte3=memptr[pc].byteVal;
	pcCheck(pc);
	pc+=1;
	dst.byteChars.byte4=memptr[pc].byteVal;

	if (dst.intVal >=progsize || dst.intVal<0){
		fprintf(stderr, "Error:Satus-ADR. Error in jmp statement");
		exit(1);
	}

	pc=dst.intVal;

}

void jxx(){
	int op=memptr[pc].bits.higher;
	pcCheck(pc);
	pc+=1;

	switch(op){
		case 0:
		jmp();
		break;

		case 1:  //jle
		if((SF^OF)==1 || ZF == 1){
			jmp();
		}
		else{
			pcCheck(pc);
			pc+=1;
		}
		break;

		case 2: //jl
		if((SF^OF)==1){
			jmp();
		}
        else{
        	pcCheck(pc);
        	pc+=1;
        }
        break;

        case 3: //je
        if(ZF==1){
        	jmp();
        }
        else{
        	pcCheck(pc);
        	pc+=1;
        }
        break;

        case 4: //jne
		if(ZF==0){
			jmp();
		}
        else{
        	pcCheck(pc);
        	pc+=1;
        }
        break;

        case 5: //jge
        if((SF^OF)==0){
        	jmp();
        }
        else{
        	pcCheck(pc);
        	pc+=1;
        }
        break;

        case 6: //jg
        if((SF^OF)==0 && ZF==0){
        	jmp();
        }
        else{
        	pcCheck(pc);
        	pc+=1;
        }
        break;

        default:
        fprintf(stderr,"ERROR:Status- INS. Improper op code for jump statement");
        exit(1);
		
	}
}
void call(){

	if(memptr[pc].bits.higher!=0){
		fprintf(stderr, "Error:Status-INS. higher bits of first byte not 0");
		exit(1);
	}


	pcCheck(pc);
	pc+=1;



	intChar destination;

	destination.byteChars.byte1=memptr[pc].byteVal;
	pcCheck(pc);
	pc+=1;
	destination.byteChars.byte2=memptr[pc].byteVal;
	pcCheck(pc);
	pc+=1;
	destination.byteChars.byte3=memptr[pc].byteVal;
	pcCheck(pc);
	pc+=1;
	destination.byteChars.byte4=memptr[pc].byteVal;
	pcCheck(pc);
	pc+=1;


	intChar retAdd;
	retAdd.intVal=pc;


	memptr[registers[4]-4].byteVal=retAdd.byteChars.byte1;
	memptr[registers[4]-3].byteVal=retAdd.byteChars.byte2;
	memptr[registers[4]-2].byteVal=retAdd.byteChars.byte3;
	memptr[registers[4]-1].byteVal=retAdd.byteChars.byte4;

	registers[4]-=4;


	pc=destination.intVal;
	
	
}

void ret(){  //pops and copies return address to the pc

	if(memptr[pc].bits.higher!=0){
		fprintf(stderr, "Error:Status-INS. higher bits of first byte not 0");
		exit(1);
	}

	intChar retAdd;


	retAdd.byteChars.byte4=memptr[registers[4]+3].byteVal;
	retAdd.byteChars.byte3=memptr[registers[4]+2].byteVal;
	retAdd.byteChars.byte2=memptr[registers[4]+1].byteVal;
	retAdd.byteChars.byte1=memptr[registers[4]].byteVal;

	registers[4]+=4;

	pc=retAdd.intVal;
	

}

void pushl(){

	if(memptr[pc].bits.higher!=0){
		fprintf(stderr, "Error:Status-INS. higher bits of first byte not 0");
		exit(1);
	}

	pcCheck(pc);
	pc+=1;

	if(memptr[pc].bits.higher!=15){
		fprintf(stderr, "Error:Status-INS. Register filler not 15 in pushl");
		exit(1);
	}

	intChar retAdd;
	retAdd.intVal=registers[memptr[pc].bits.lower];


	memptr[registers[4]-4].byteVal=retAdd.byteChars.byte1;
	memptr[registers[4]-3].byteVal=retAdd.byteChars.byte2;
	memptr[registers[4]-2].byteVal=retAdd.byteChars.byte3;
	memptr[registers[4]-1].byteVal=retAdd.byteChars.byte4;

	registers[4]-=4;

	pc+=1;
	pcCheck(pc);
	
}

void popl(){

	pc+=1;
	pcCheck(pc);

	intChar retAdd;

	retAdd.byteChars.byte1=memptr[registers[4]].byteVal;
	retAdd.byteChars.byte2=memptr[registers[4]+1].byteVal;
	retAdd.byteChars.byte3=memptr[registers[4]+2].byteVal;
	retAdd.byteChars.byte4=memptr[registers[4]+3].byteVal;

	registers[4]+=4;

	registers[memptr[pc].bits.lower]=retAdd.intVal;
	pc+=1;
	pcCheck(pc);


}

void readX(){
	int operation=memptr[pc].bits.higher;


	pc+=1;
	pcCheck(pc);

	int destreg=registers[memptr[pc].bits.lower];

	intChar final;

	pc+=1;
	pcCheck(pc);
	final.byteChars.byte1=memptr[pc].byteVal;
	pc+=1;
	pcCheck(pc);
	final.byteChars.byte2=memptr[pc].byteVal;
	pc+=1;
	pcCheck(pc);
	final.byteChars.byte3=memptr[pc].byteVal;
	pc+=1;
	pcCheck(pc);
	final.byteChars.byte4=memptr[pc].byteVal;
	pc+=1;
	pcCheck(pc);

	destreg+=final.intVal;


	if (operation==0){
		memptr[destreg].byteVal=getchar();
		if(memptr[destreg].byteVal == -1){
			ZF=1;
		}
		else{
			ZF=0;
		}

	}
	else if (operation==1){
		if(EOF==scanf("%d", &final.intVal)){
			ZF=1;
		}
		else{
			ZF=0;
		}
		memptr[destreg+3].byteVal=final.byteChars.byte4;
		memptr[destreg+2].byteVal=final.byteChars.byte3;
		memptr[destreg+1].byteVal=final.byteChars.byte2;
		memptr[destreg].byteVal=final.byteChars.byte1;
	}
	else{
		fprintf(stderr, "ERROR: Status-INS. Improper function op in readx");
	}
}

void writeX(){
	int operation=memptr[pc].bits.higher;

	pc+=1;
	pcCheck(pc);


	int dest=registers[memptr[pc].bits.lower];

	intChar final;

	pc+=1;
	pcCheck(pc);
	final.byteChars.byte1=memptr[pc].byteVal;
	pc+=1;
	pcCheck(pc);
	final.byteChars.byte2=memptr[pc].byteVal;
	pc+=1;
	pcCheck(pc);
	final.byteChars.byte3=memptr[pc].byteVal;
	pc+=1;
	pcCheck(pc);
	final.byteChars.byte4=memptr[pc].byteVal;
	pc+=1;
	pcCheck(pc);

	dest+=final.intVal;

	if(operation==0){
		printf("%c",memptr[dest].byteVal );
	}
	else if(operation==1){
		final.byteChars.byte4=memptr[dest+3].byteVal;
		final.byteChars.byte3=memptr[dest+2].byteVal;
		final.byteChars.byte2=memptr[dest+1].byteVal;
		final.byteChars.byte1=memptr[dest].byteVal;
		printf("%i", final.intVal);

	}
	else{
		fprintf(stderr, "ERROR: Status-INS. Improper function op in writeX");

	}
}

void movsbl(){
	pc+=1;
	pcCheck(pc);


	int initReg=memptr[pc].bits.lower;
	int destReg=memptr[pc].bits.higher;

	intChar final;

	pc+=1;
	pcCheck(pc);
	final.byteChars.byte1=memptr[pc].byteVal;
	pc+=1;
	pcCheck(pc);
	final.byteChars.byte2=memptr[pc].byteVal;
	pc+=1;
	pcCheck(pc);
	final.byteChars.byte3=memptr[pc].byteVal;
	pc+=1;
	pcCheck(pc);
	final.byteChars.byte4=memptr[pc].byteVal;
	pc+=1;
	pcCheck(pc);

	destReg+=final.intVal;

	if(memptr[destReg].bits.lower>=8){
		final.intVal=-1;
		final.byteChars.byte1=memptr[destReg].byteVal;
	}
	else{
		final.intVal=0;
		final.byteChars.byte1=memptr[destReg].byteVal;
	}
	registers[initReg]=final.intVal;
	
}

int main(int argc, char** argv){

	if(strcmp(argv[1],"-h")==0){
		printf("Usage: y86emul <y86 input file>\n");
		exit(0);
	}

	if(argc!=2){
		fprintf(stderr, "Improper number of arguments entered.\n");
	}

	char * fileExt=strstr(argv[1],".y86");
	if(strcmp(fileExt,".y86")!=0){
		fprintf(stderr, "Error in file name, please make sure file is .y86\n");
		exit(1);
	}

	FILE * fyle = fopen(argv[1],"r");
	if(fyle==NULL){
		printf("File not found in directory\n");
		return 1;
	}

	size_t length=0;
	ssize_t reading=0;
	char * currLine=NULL;

	while((reading=getline(&currLine,&length,fyle))!=-1){
		char * token=strtok(currLine,"\t");
		while(token!=NULL){
			if(strcmp(token,".size")==0){
				token=strtok(NULL, "\n");

				int sizeB= ((int) strtol(token,NULL,16));
				progsize=sizeB;
				memptr=malloc(sizeB*sizeof(aByte));

				if (memptr==NULL){
					fprintf(stderr, "Memory space not allocated.\n" );
					exit(0);
				}
			}
			else if(strcmp(token,".long")==0){
				token=strtok(NULL,"\t");

				int address=(int) strtol(token,NULL,16);

				token=strtok(NULL,"\n");
				intChar longVal;

				longVal.intVal=(int) strtol(token,NULL,16);

				memptr[address].byteVal=longVal.byteChars.byte1;
				memptr[address+1].byteVal=longVal.byteChars.byte2;
				memptr[address+2].byteVal=longVal.byteChars.byte3;
				memptr[address+3].byteVal=longVal.byteChars.byte4;

			}
			else if(strcmp(token,".string")==0){
				token=strtok(NULL,"\t");
				int address=(int) strtol(token,NULL,16);
				token=strtok(NULL,"\n");

				char * stringPtr = token+1;

				while(*stringPtr != '\0' && *stringPtr!='\"'){
					memptr[address].byteVal=*stringPtr;
					stringPtr++;
					address++;
				}
			}
			else if(strcmp(token,".byte")==0){
				token=strtok(NULL,"\t");
				int address=(int) strtol(token,NULL,16);
				token=strtok(NULL,"\n");
				int hexVal=(int) strtol(token,NULL,16);
				memptr[address].byteVal=(char) hexVal;

			}
			else if(strcmp(token,".text")==0){
				token=strtok(NULL,"\t");
				int address=(int) strtol(token,NULL,16);
				pc=address;

				token=strtok(NULL,"\n");

				char * txt=token;

				while(*txt!='\0' && isspace(*txt)==0){
					memptr[address].bits.lower=getHexVal(*txt);
					txt++;
					if(*txt!='\0' && isspace(*txt)==0){
						memptr[address].bits.higher=getHexVal(*txt);
						txt++;
					}
					address++;
				}

			}
			else{
				fprintf(stderr, "Invalid directive in y86 file.\n");
			}

			token=strtok(NULL,"\t"); // to stop loop.

		}
	}
	//close the file
	if(fclose(fyle)!=0){
		fprintf(stderr, "Error: File not closed" );
		exit(1);
	}


	int execute;
	while(execute!=-1){
		execute=memptr[pc].bits.lower;

		switch(execute){
			case 0:
			pc+=1;
			pcCheck(pc);
			break;

			case 1:
			execute=-1;
			break;

			case 2:
			rrmovl();
			break;

			case 3:
			irmovl();
			break;

			case 4:
			rmmovl();
			break;

			case 5:
			mrmovl();
			break;

			case 6:
			operation1();
			break;

			case 7:
			jxx();
			break;

			case 8:
			call();
			break;

			case 9:
			ret();
			break;

			case 10:
			pushl();
			break;

			case 11:
			popl();
			break;

			case 12:
			readX();
			break;

			case 13:
			writeX();
			break;

			case 14:
			movsbl();
			break;

			default:
			fprintf(stderr, "Error: Error during excution, invalid directive.");
			exit(1);
		}
	}

	printf("Status: AOK\n");

	free(memptr);

	return 0;
}
