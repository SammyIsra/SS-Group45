/*
 * Parser
 *
 *	Created on: Nov 9, 2015
 *  Author: 
 *		Victoria Rivas
 *		Sammy Israwi
 *		Meenakshi Karthikeya
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SYMBOL_TABLE_SIZE 100

//Token table
typedef enum{
	nulsym = 1, indentsym, numbersym, plussym, minssym,
	multsym, slashsym, oddsym, eqsym, neqsym, lessym, semicolonsym,
	periodsym, becomessym, beginsym, endsym, ifsym, thensym, whilesym,
	dosym, callsym, constsym, varsym, procsym, writesym, readsym, elsesym
} token_type;

typedef struct symbol{
	int kind; 		//const = 1, var = 2, proc = 3
	char name[12]; 	//name up to 11 chars
	int val; 		//value
	int level;		//L level
	int addr;		//M address
} symbol;

symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
int nextToken;
char nextIdentifier[12];
int nextNumber;

readTokens(char* input);

int main()
{
	readTokens("tokenlist.txt");

}

readTokens(char* input)
{
	FILE *ipf = fopen(input, "r");
	char c = '0';

	if(ipf == NULL)
		printf("File could not be found");

}

/*
	Error function
		Takes in a number
		Prints out the error message associated with the number received
		Exits out of the program
*/
void error( int errorNo ){
	
	char msg[128] = "";
	
	switch (errorNo){
		case 1: 
			msg = "Use = instead of :=";
			break;
		case 2: 
			msg = "= must be followed by a number";
			break;
		case 3: 
			msg = "Identifier must be followed by :=";
			break;
		case 4: 
			msg = "const, var, procedure must be followed by identifier";
			break;
		case 5: 
			msg = "Semicolon or comma missing";
			break;
		case 6: 
			msg = "Incorrect symbol after statement part in block";
			break;
		case 7: 
			msg = "Statement expected";
			break;
		case 8: 
			msg = "Incorrect symbol after statement part in block";
			break;
		case 9: 
			msg = "Period expected";
			break;
		case 10: 
			msg = "Semicolon between statements missing";
			break;
		case 11: 
			msg = "Undeclared identifier";
			break;
		case 12: 
			msg = "Assignment to constant or procedure is not allowed";
			break;
		case 13: 
			msg = "Assignment operator expected";
			break;
		case 14: 
			msg = "call must be followed by an identifier";
			break;
		case 15: 
			msg = "Call of a constant or variable is meaningless";
			break;
		case 16: 
			msg = "then expected";
			break;
		case 17: 
			msg = "Semicolon or } expected";
			break;
		case 18: 
			msg = "do expected";
			break;
		case 19: 
			msg = "Incorrect symbol following statement";
			break;
		case 20: 
			msg = "Relational operator expected";
			break;
		case 21: 
			msg = "Expression must not contain a procedure identifier";
			break;
		case 22: 
			msg = "Right parenthesis missing";
			break;
		case 23: 
			msg = "The preceding factor cannot begin witht this symbol";
			break;
		case 24: 
			msg = "An expression cannot begin with this symbol";
			break;
		case 25: 
			msg = "This number is too large";
			break;
		default:
			msg = "Error number not recognized";
			break;
	}
	
	printf("ERROR #%d: %s.\n", errorNo, msg);
    exit(EXIT_FAILURE);
}
    
// This function will grab the next token from the token list
void getNextToken() {
	
	//Grabbing the next identifier
	if (nextToken == indentsym) {
		fscanf(ifp, "%s", nextIdentifier)
	}
	
	//Grabbing the next number
	if (nextToken == indentsym) {
		fscanf(ifp, "%s", nextNumber)
	}
	
	//If there's no token, nextToken is assigned to -1
	if (fscanf(ifp, "%d", &nextToken) == EOF) {
		nextToken = -1;
	}
}

