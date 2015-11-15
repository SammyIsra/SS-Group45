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

void error( char * msg ){
	printf("ERROR: %s\n", msg);
    exit(EXIT_FAILURE);
}
