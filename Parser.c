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
#define MAX_CODE_LENGTH 500

//Token table
typedef enum {
    nulsym = 1, identsym, numbersym, plussym, minussym,
    multsym, slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym, elsesym
} token_type;

typedef struct symbol{
	int kind; 		//const = 1, var = 2, proc = 3
	char name[12]; 	//name up to 11 chars
	int val; 		//value
	int level;		//L level
	int addr;		//M address
} symbol;

typedef struct instruction{
    int op;
    int l;
    int m;
} instruction;

symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
instruction code[MAX_CODE_LENGTH];
char nextIdentifier[12];
int nextToken;
int nextNumber;
int symbolsAmount = 0;	//Amount of symbols in the symbol table
int curLexLevel = -1;
int branchCon = 0;
int lineCount;
int codeIndex = 0; //Index of the code that we are writing to, used for jumps and such

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
		case 26:
		    msg = "An identifier can only be decladed once per lexicographical level";
		    break;
		case 27:
		    msg = "end keyword expected"
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

//This funciton takes in an identifier and returns the corresponding symbol from
// the symbol table. 
symbol identifierToSymbol (char iden[]) {
	int i;
	
	//Downcounting for-loop 
	for (i = symbolsAmount - 1; i >= 0; i--) {
		if (strcmp(symbol_table[i].name, nextIdentifier) == 0) {
			if (symbol_table[i].level <= curLexLevel)
				return symbol_table[i];
		}
	}
	//If symbol not found, error found.
	error(11);
}

void print (int op, int l, int m) {
    
    if(codeIndex > MAX_CODE_LENGTH){
        error(28); "Generated mcode is greater than the imposed limit, %d", MAX_CODE_LENGTH);
    }
    
    code[codeIndex].op = op;
    code[codeIndex].l = l;
    code[codeIndex].m = m;
    codeIndex++;
    
    /*  Old code
	if (branchCon != 0)
		return;
	fprintf(ofp,"%d %d %d \n", op, l, m);
	*/
}

// Returns the amount of variables in the current lexicographical level
int varLexLevelAmount(){
    
    int i;
    int num = 0;
    
    for(i = 0 ; i < symbolsAmount ; i++){
        
        if(symbol_table[i].level == curLexLevel && symbol_table[i].kind == 2)
            num++;
    }
    
    return num;
}

// Returns true of the token received is a relational operator sans 'odd'
//  Relational operators:
//      | = | <> | < | <= | > | >= | 
int isRelationalOp(int token){
    
    //Return 1 if it is a relational operator. Otherwise, return 0;
    return (token == eqsym || token == neqsym || token == lessym 
        || token == leqsym || token == gtrsym || token == geqsym);
}

// Insert an indentifier to the Symbol Table
// Validation is done inside the function
// Increase of symbol counter is also done inside the function
void insertToSymbolTable(int kind){
	
	//Try to find if the symbol is already declared
	int i;
	for(i = 0 ; i < symbolsAmount ; i++){
		
		// IF the name of the identifier is already on the symbol table 
		// AND IF they are both on the same lexicographical level
		// THEN error out
        if(strcmp(symbol_table[i].name, nextIdentifier) == 0 
            && symbol_table[i].level == curLexLevel){
			
			//The declared symbol had already been declared in that level
			error(26);
		}
	}
	
	//Add the symbol to the symbol table
	strcpy(symbol_table[symbolsAmount].name, nextIdentifier);
	symbol_table[symbolsAmount].kind = kind;
	symbol_table[symbolsAmount].level = curLexLevel;
	
	if(kind == 1){
	    //Add a constant to the symbol table
	    symbol_table[symbolsAmount].val = nextNumber;
	    symbol_table[symbolsAmount].addr = 0;
	    
	} else if(kind == 2){
	    //Add a variable to the symbol table
	    symbol_table[symbolsAmount].addr = 4 + varLexLevelAmount();
	    
	} else if(kind == 3){
	    //Add a procedure to the symbol table
	    symbol_table[symbolsAmount].addr = lineCount + 1;
	}
    
    symbolsAmount++;
}

void factor () {
	if (nextToken == identsym) {
		symbol cur = identifierToSymbol(nextIdentifier);
		if (cur.kind == 1) {
			print(1, 0, cur.val);
			lineCount++;
		} else if (cur.kind == 2) {
			print(3, (curLexLevel - cur.level), cur.addr);
			lineCount++;
		} else {
			error(21);
		}
		getNextToken();
	} else if (nextToken == lparentsym) {
		getNextToken();
		expression();
		if (nextToken != rparentsym)
			error(22);
		getNextToken();
	} else {
		error(23);
	}
}

void condition () {
	if (nextToken == oddsym) {
		getNextToken();
		expression();
		print(2, 0, 6);
		lineCount++;
	} else {
		expression();
		if (isRelationalOp(nextToken) == 0)
			error(20);
		int mValue = nextToken - 1;
		getNextToken();
		expression();
		print(2, 0,mValue);
		lineCount++;
	}
}

void expression(){
    
    if (nextToken == plussym || nextToken == minussym){
        if (nextToken == minussym){
            //When we have a negative number
            print(2, 0, 1); //OPR 0 1
            lines++;
        } else {
            getNextToken();
        }
    }
    
    term();
    
    while(nextToken == plussym || nextToken == minussym){
        
        /* Note:
            The reason why getNextToken() and term() are not called twice
            inside the if-else statement is because we need to remember what 
            was the token we had, - or +, before it changes on the next 
            getNextToken()
        */
        
        if(nextToken == plussym){
            //We add an addition to the mcode
            getNextToken();
            term();
            print(2, 0, 2); //OPR 0 2
        } else {
            //We add a substraction to the mcode
            getNextToken();
            term();
            print(2, 0, 3); //OPR 0 3
        }
        
        //Increase line counter
        lineCount++;
    }
}

void term(){
    
    factor();
    
    while (token == multsym || token == slashsym){
        
        /* Note:
            The reason why getNextToken() and factor() are not called twice
            inside the if-else statement is because we need to remember what 
            was the token we had, * or /, before it changes on the next 
            getNextToken()
        */
        if (token == multsym){
            //We add a multiplier to the mcode
            getNextToken();
            factor();
            print(2, 0, 4); //OPR 0 4
        } else {
            //We add a division to the mcode
            getNextToken();
            factor();
            print(2, 0, 5); //OPR 0 5
        }
        
        //Increase line counter
        lineCount++;
    }
}

void statement(){
    
    if (nextToken == identsym){         // IDENTIFIER
        
        char temp[12];
        strcpy(temp, nextIdentifier);
        
        if(identifierToSymbol(nextIdentifier).kind != 2){
            error(12);
        }
        
        getNextToken();
        
        if(nextToken != becomessym){
            error(3);
        } 
        
        getNextToken();
        expression();
        
        symbol currentSym = identifierToSymbol(temp);
        
        // STO L M
        print(4, (curLexLevel - currentSym.level), currentSym.addr );   
        lineCount++;
        
    } else if (nextToken == callsym){   // CALL
        
        getNextToken();
        
        if(nextToken != identsym){
            error(14);
        }
        
        if(identifierToSymbol(nextIdentifier).kind != 3){
            error(15);
        }
        
        symbol current = identifierToSymbol(nextIdentifier);
        print(5, (curLexLevel-current.level), current.addr);    // CAL L M
        
        lineCount++;
        
        getNextToken();
        
    } else if (nextToken == beginsym){  // BEGIN
        
        getNextToken();
        statement();
        
        while(nextToken == semicolonsym){
            getNextToken();
            statement();
        }
        
        if(nextToken != endsym){
            error(27);
        }
        
        getNextToken();
        
    } else if (nextToken == ifsym){     // IF
        
        int firstJump;
        int secondJump;
        int currentLine;
        int lastLine;
        
        getNextToken();
        
        condition();
        
        if(nextToken != thensym){
            error(16);
        }
        
        //Record the code index, make a dummy line
        firstJump = codeIndex;
        print(0, 0, 0);
        
        getNextToken();
        statement();
        
        if(nextToken == elsesym){
            
            getNextToken();
            
            //Get location of dummy line 
            // (Location of the second jump)
            //second_Jump will land at the line at end of execution
            secondJump = codeIndex;
            //Dummy line
            print(0, 0, 0);
            
            //Record the current line
            currentLine = codeIndex;
            
            codeIndex = firstJump;

            print(8, 0, currentLine);
        
            statement();
            
            currentLine = codeIndex;
            
            codeIndex = secondJump;
            
            print(7, 0, currentLine);
            
            codeIndex = currentLine;
        
            
        } else {
            
            //Record where the last line is
            lastLine = codeIndex;
            
            //Make the code inex the place where the first jump departs from
            codeIndex = firstJump;
            
            //Rewrite the dummy line from the IF statement
            print(8, 0, lastLine);
            
            //codeIndex is now back at the rightful place
            codeIndex = lastLine;
        }
        
    } else if (nextToken == whilesym){  // WHILE
        
        if (nextToken != dosym)
        	error(18);
        
        int firstLine1;
        int firstLine2;
        int lastLine;
        
        getNextToken();
        
        firstLine1 = codeIndex;
        condition();
        firstLine2 = codeIndex;
        
        // Print the dummy line
        print(0, 0, 0);
        
        
        if(nextToken != dosym){
            error(18);
        }
        
        getNextToken();
        
        //Inside the while body
        statement();
        
        //Record the current line
        lastLine = codeIndex;
        
        //Change codeIndex to rewrite the dummy line
        codeIndex = firstLine2;
        
        //Rewrite dummy line
        print(8, 0, lastLine+1);
        
        //Go back to last line
        codeIndex = lastLine;
        
        print(7, 0, firstLine1);
        
    } else if (nextToken == readsym) {  // READ
        
        getNextToken();
        symbol cur = identifierToSymbol(nextIdentifier);
        
        print(9,0,1);
        print(4, (curLexLevel - cur.level), cur.addr)
        lineCount+=2;
        getNextToken();
        
    } else if (nextToken == writesym) {  // WRITE
        
        getNextToken();
        symbol cur = identifierToSymbol(nextIdentifier);
        
        if (cur.kind == 1)
        	print(1, 0, cur.val)
        	
        if (cur.kind == 2)
        	print(3, (curLexLevel - cur.level), cur.addr)
      
        print(9,0,0)
        lineCount+=2;
        getNextToken();
        
    }
}
void block(int table) {
	curLexLevel++;
	if (nextToken == constsym) {
		do {
			getNextToken();
			if (nextToken != identsym)
				error(4);
			getNextToken();
			if (nextToken == becomessym)
				error(1);
			if (nextToken != eqsym) 
				error (2);
			getNextToken();
			if (nextToken != numbersym)
				error(3);
			if (table == 0)
				insertToSymbolTable(1);
			getNextToken();
		} while (nextToken == commasym);
		
		if (nextToken != semicolonsym)
			error(5);
		getNextToken();
	}
	if (nextToken == varsym) {
		do {
			getNextToken();
			if (nextToken != identsym)
				error(4);
			if (table == 0)
				insertToSymbolTable(2);
			getNextToken();
		} while (nextToken == commasym);
		
		if (nextToken != semicolonsym)
			error(5);
		getNextToken();
	}
	while (nextToken == procsym) {
		getNextToken();
		if (nextToken != identsym)
			error(4);
		if (table == 0)
			insertToSymbolTable(3);
		getNextToken();
		if (nextToken != semicolonsym)
			error(6);
			
		
	}
}