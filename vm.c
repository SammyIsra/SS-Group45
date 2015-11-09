/*
	Meenakshi Karthikeya
	Victoria Rivas
	Sammy Israwi
	09/20/2015
	Assignment 1
*/

#include <stdio.h>

/*
	Constants
*/
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

/*
	Instruction Structure
*/
typedef struct instruction{
	int op;   // opcode 
	int  l;   // L            
	int  m;   // M         
} instruction;

/*
	Global Variables
*/
int halt = 1;								// Halt(0) or not(1)
int sp = 0;									// stack pointer
int bp = 1;									// base pointer
int pc = 0;									// program counter
int stack[MAX_STACK_HEIGHT] = { 0, 0, 0 }; 	// intialize the Stack
instruction code[MAX_CODE_LENGTH];			// code storage
instruction ir;								// instruction register
int ar[MAX_STACK_HEIGHT];					// stores the activation record history
int cur_ar = 0;								// keep track of which activation record we are at
FILE *ofp;									// make the output file accessible everywhere

/*
	Function Prototypes
*/
int base(int l, int base);
char *decode(int opNum);
instruction fetch();
void execute();
void printInstruction(int line, instruction ins, char *append, int screen);
void printProgram(int lines, int screen);
void printRegisters(int screen);
void printStack(int screen);


/*
	--MAIN FUNCTION                                   
*/
int main(int argc, char *argv[]){

	// Print to screen (used for debugging)
	int screen = 0; 

	FILE *ifp = fopen("mcode.txt", "r");	// Open the input file
	ofp = fopen("stacktrace.txt", "w");		// Open the output file (ofp is a global variable)
	int line = 0;							// Used for storing the instructions in code[]

	// Error opening the file
	if(ifp == NULL) { 
		printf("'mcode.txt' could not be opened!\n");
		return 0;
	}

	// While theres data in input file, store it in code[] and increment the line number
	// fscanf return -1 when we reach EOF
	// We read in line by line because we know that the format is consistent
	while(fscanf(ifp, "%d %d %d", &ir.op, &ir.l, &ir.m) != -1){
		code[line] = ir;	// save the instruction into the code storage
		line++;				// increment the index for the code storage
	}

	// Print all the lines from the program code 
	printProgram(line, 0);
	// Print the heading for the stack trace along with the initial register values
	fprintf(ofp, "\n\t\t\t\tpc\tbp\tsp\tstack\n");
	fprintf(ofp, "Initial values\t\t\t");
	printRegisters(0);
	fprintf(ofp, "\n");

	// Same thing as above, but print to screen
	if(screen){
		printProgram(line, 1);
		printf("\n\t\t\t\tpc\tbp\tsp\tstack\n");
		printf("Initial values\t\t\t");
		printRegisters(1);
		printf("\n");
	}

	// If the base pointer is not 0 
	// AND no halt signal has been detected,
	// THEN continue execution of the program
	while(bp != 0 && halt != 0) {
		ir = fetch();								// Fetch instruction into instruction register
		printInstruction(pc-1, ir, "\t", screen);	// Print it
		execute();									// Execute the instruction
		printRegisters(screen);						// Print the registers
		printStack(screen);			// Print the stack trace
	}

    if(screen)
        printf("\n");

    // After a hard day of decoding and interpreting, close up the program
	return 0;
}

/*
	FIND BASE L LEVELS DOWN	
	From instructions	   
*/
int base(int l, int base){ // l stand for L in the instruction format

	int b1; // find base L levels down
	b1 = base;

	while(l > 0){
		b1 = stack[b1 + 1];
		l--;
	}

	return b1;
}

/*
	--DECODE OPCODE                             
	Takes in an opcode in number format
	Returns a string of the corresponding opcode
*/
char *decode(int opNum){

	char *op = "";	//Translated instruction from opcode to mnemonic
	
	switch(opNum){
		case 1:
			op = "lit";
			break;
		case 2:
			op = "opr";
			break;
		case 3:
			op = "lod";
			break;
		case 4:
			op = "sto";
			break;
		case 5:
			op = "cal";
			break;
		case 6:
			op = "inc";
			break;
		case 07:
			op = "jmp";
			break;
		case 8:
			op = "jpc";
			break;
		case 9:
			op = "sio";
			break;
		case 10:
			op = "sio";
			break;
		case 11:
			op = "sio";
			break;
	}

	//Return the translated opcode
	return op;
}

/*
	--INSTRUCTION FETCH                                    
	Returns an instruction (struct) containing the instruction pointed at by the PC (Program Counter)                     
*/
instruction fetch(){

	// FETCH the instruction from code[]
	// And increment the Program Counter
	instruction ins = code[pc++];

	// Return the instruction
	return ins;
}

/*
	--EXECUTE                                            
	Executes the instruction for the opcode 
	Note: For the sake of not repeating myself every time, 
		remember that every time we perform an operation,
		we store the result at top of stack.
*/
void execute(){

 	// Select instruction based on opcode
	switch(ir.op){

		case 1: // LIT
			sp++;
			stack[sp] = ir.m;
			break;
		case 2: // OPR (operation, arithmetic)

			// OPR can be...
			switch(ir.m){

				case 0: // RET (return)
					sp = bp - 1;
					pc = stack[sp + 4];
					bp = stack[sp + 3];
					cur_ar--;
					break;
				case 1: // NEG (negation)
						//Multiply the top of the stack by -1
					stack[sp] = -stack[sp];
					break;
				case 2: // ADD (addition)
						//Add top two items on stack
					sp--;
					stack[sp] = stack[sp] + stack[sp + 1];
					break;
				case 3: // SUB (subtraction)
						//Substract top two items on stack
					sp--;
					stack[sp] = stack[sp] - stack[sp + 1];
					break;
				case 4: // MUL (multiplication)
						//Multiply top two items on stack
					sp--;
					stack[sp] = stack[sp] * stack[sp + 1];
					break;
				case 5: // DIV (division)
						//Divide top two items on stack
					sp--;
					stack[sp] = stack[sp] / stack[sp + 1];
					break;
				case 6: // ODD (check if odd)
						// Check if top item in stack is odd.
						// Store 1 if true, 0 if false on top of stack
					stack[sp] = stack[sp] % 2;
					break;
				case 7: // MOD (modulo)
						// Store tos % tos+1 on top of stack
					sp--;
					stack[sp] = stack[sp] % stack[sp + 1];
					break;
				case 8: // EQL (equality test)
						// Check equality of the top two items of stack.
						// Store 1 if they are equal, 0 if they are not
					sp--;
					stack[sp] = stack[sp] == stack[sp + 1];
					break;
				case 9: // NEQ (inequality test)
						// Same as above, but for inequality
					sp--;
					stack[sp] = stack[sp] != stack[sp + 1];
					break;
				case 10: // LSS (less than)
						 // Check for less than
					sp--;
					stack[sp] = stack[sp] < stack[sp + 1];
					break;
				case 11: //  LEQ (less than or equal to)
						 // Check for less than or equal to
					sp--;
					stack[sp] = stack[sp] <= stack[sp + 1];
					break;
				case 12: // GTR (greater than)
						 // Check for greater than
					sp--;
					stack[sp] = stack[sp] > stack[sp + 1];
					break;
				case 13: // GEQ (greater than or equal to)
						 // Check for greater than or equal to
					sp--;
					stack[sp] = stack[sp] >= stack[sp + 1];
					break;
			}
			break;

		// LOAD and STORE operations
		case 3: // LOD (load)

			if(ir.l <= MAX_LEXI_LEVELS){
				sp++;
				stack[sp] = stack[base(ir.l, bp) + ir.m];
			}
			else 
				printf("Lexicographical level exceeds MAX_LEXI_LEVELS of %d\n", MAX_LEXI_LEVELS); 
			
			break;
		case 4: // STO (store)

			if(ir.l <= MAX_LEXI_LEVELS){
				stack[base(ir.l, bp) + ir.m] = stack[sp];
				sp--;
			}else 
				printf("Lexicographical level exceeds MAX_LEXI_LEVELS of %d\n", MAX_LEXI_LEVELS); 
			break;

		case 5:// CAL (call procedure M at level L)

			if(ir.l <= MAX_LEXI_LEVELS){
				stack[sp + 1] = 0;				// space to return value (FV)
				stack[sp + 2] = base(ir.l, bp); // static link (SL)
				stack[sp + 3] = bp;				// dynamic link (DL)
				stack[sp + 4] = pc;				// return address (RA)
				bp = sp + 1;					// new Base Pointer based on current Stack Pointer
				pc = ir.m;						// new program counter indicated by instruction
				ar[cur_ar] = bp;				// store the location of the ar
				if(cur_ar < MAX_STACK_HEIGHT)
					cur_ar++;					// increment the ar (for printing later)
			}else 
				printf("Lexicographical level exceeds MAX_LEXI_LEVELS (%d)\n", MAX_LEXI_LEVELS); 
			break;

		case 6: // INC (increment sp by ir.m)
			sp += ir.m;
			break;
		case 7: // JMP (jump to M)
			pc = ir.m;
			break;
		case 8: // JPC (jump conditional to ir.m)
			if(stack[sp] == 0)
				pc = ir.m;
			sp--;
			break;
		case 9: // SIO 1 (print and pop the top item on the stack)
			printf("%d\n", stack[sp]);
			sp--;
			break;
		case 10: // SIO 2 (load user inputed item on top of stack)
			sp++;
			printf("Enter an integer: ");
			scanf("%d", &stack[sp]);
			break;
		case 11: // SIO 3 (halt the program)
				 // Set variables to 0, set halt to 0
			sp = 0;
			bp = 0;
			pc = 0;
			halt = 0;
			break;
	}
}

/*
	--PRINT INSTRUCTION                        
	Takes in a line number, instruction, and a string to append to te end of the instruction (for seperators)              
	Prints the line number, followed by the instruction, and the string appended to the end                                
*/
void printInstruction(int line, instruction ins, char *append, int screen){

	char* decodedOP = decode(ins.op);

	// Print the instruction
	fprintf(ofp, "%2d\t%s\t%d\t%d", line, decodedOP, ins.l, ins.m);
	
	// Print the string to be appended (separator)
	fprintf(ofp, "%s", append);

	if(screen){
		printf("%2d\t%s\t%d\t%d", line, decodedOP, ins.l, ins.m);
		printf("%s", append);
	}
}

/*
	--PRINT PROGRAM                             
	Takes in the number of lines of code to to print                                  
	Prints the specified lines stored in the array code[]                          
*/
void printProgram(int lines, int screen){

	int j;

	// Print the code heading
	fprintf(ofp, "Line\tOP\tL\tM\n");

	// Print the code stored in code[]
	for(j = 0; j < lines; j++)
		printInstruction(j, code[j], "\n", screen);
}

/*
	--PRINT REGISTERS                           
	Prints the PC, BP and SP registers separated by tabs
	Will print to screen too if flag (screen) indicates so
*/
void printRegisters(int screen){

	//print to file
	fprintf(ofp, "%d\t%d\t%d\t", pc, bp, sp);

	if(screen) // print to screen
		printf("%d\t%d\t%d\t", pc, bp, sp);
}

/*
	--PRINT STACK                               
	Prints data stored in the stack and a separator between activation records followed by a new line
	Will print to screen too if flag (screen) indicates so
*/
void printStack(int screen){

	int i, j = 0;

	for(i = 1; i <= sp; i++){

		// Print the pipe '|' separator in between each activation record
		if(i == ar[j]){

			fprintf(ofp, "| ");

			if(screen)
				printf("| ");

			j++;	// Incrase counter for activation record
		}

		// Print value from stack location i
		fprintf(ofp, "%d ", stack[i]);

		if(screen)
			printf("%d ", stack[i]);
	}

	// Print end of line
	fprintf(ofp, "\n");

	if(screen)
		printf("\n");
}

