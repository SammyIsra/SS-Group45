/*
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
#define DEBUG 0

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
//the lexemelist fp
FILE* ifp;
//output file pointer
FILE* ofp;

char nextIdentifier[12];
int nextToken;
int nextNumber;
int symbolsAmount = 0;	//Amount of symbols in the symbol table
int curLexLevel = -1;
int lineCount = 0;
int branchCon = 0;
int codeIndex = 0; //Index of the code that we are writing to, used for jumps and such

/*
 *  FUNCTION PROTOTYPES
 */
void readTokens(char* input);
void error( int errorNo );
void getNextToken();
symbol identifierToSymbol (char iden[]);
int codeAddrMod (int loc, int m);
int print (int op, int l, int m);
void printMcode();
int varLexLevelAmount();
int isRelationalOp(int token);
void enter(int kind, char* name, int val, int lvl, int addr);
void insertToSymbolTable(int kind);
void printSymbolTable();
void program();
void block();
void statement();
void condition();
void expression();
void term();
void factor();
int deleteOldLevels(int lexLevel);


void readTokens(char* input)
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
			strcpy(msg, "Use = instead of :=");
			break;
		case 2:
			strcpy(msg,"= must be followed by a number");
			break;
		case 3:
			strcpy(msg, "Identifier must be followed by :=");
			break;
		case 4:
			strcpy(msg, "const, var, procedure must be followed by identifier");
			break;
		case 5:
			strcpy(msg, "Semicolon or comma missing");
			break;
		case 6:
			strcpy(msg, "Incorrect symbol after statement part in block");
			break;
		case 7:
			strcpy(msg, "Statement expected");
			break;
		case 8:
			strcpy(msg, "Incorrect symbol after statement part in block");
			break;
		case 9:
			strcpy(msg, "Period expected");
			break;
		case 10:
			strcpy(msg, "Semicolon between statements missing");
			break;
		case 11:
			strcpy(msg, "Undeclared identifier");
			break;
		case 12:
			strcpy(msg, "Assignment to constant or procedure is not allowed");
			break;
		case 13:
			strcpy(msg, "Assignment operator expected");
			break;
		case 14:
			strcpy(msg, "call must be followed by an identifier");
			break;
		case 15:
			strcpy(msg, "Call of a constant or variable is meaningless");
			break;
		case 16:
			strcpy(msg, "then expected");
			break;
		case 17:
			strcpy(msg, "Semicolon or } expected");
			break;
		case 18:
			strcpy(msg, "do expected");
			break;
		case 19:
			strcpy(msg, "Incorrect symbol following statement");
			break;
		case 20:
			strcpy(msg,"Relational operator expected");
			break;
		case 21:
			strcpy(msg,"Expression must not contain a procedure identifier");
			break;
		case 22:
			strcpy(msg, "Right parenthesis missing");
			break;
		case 23:
			strcpy(msg,"The preceding factor cannot begin with this symbol");
			break;
		case 24:
			strcpy(msg,"An expression cannot begin with this symbol");
			break;
		case 25:
			strcpy(msg, "This number is too large");
			break;
		case 26:
		    strcpy(msg, "An identifier can only be declared once per lexicographical level");
		    break;
		case 27:
		    strcpy(msg, "end keyword expected");
		    break;
		case 28:
		    strcpy(msg, "Generated mcode is greater than the imposed limit");
		    break;
		default:
			strcpy(msg,"Error number not recognized");
			break;
	}



	printf("ERROR #%d: %s.\n", errorNo, msg);
	if(DEBUG){
	    printSymbolTable();
	}
	
    exit(EXIT_FAILURE);
}

// This function will grab the next token from the token list
void getNextToken() {

	//If there's no token, nextToken is assigned to -1
	if (fscanf(ifp, "%d", &nextToken) == EOF) {
		nextToken = -1;
	}

	//Grabbing the next identifier
	else if (nextToken == identsym) {
		fscanf(ifp, "%s", nextIdentifier);
	}

	//Grabbing the next number
	else if (nextToken == numbersym) {
		fscanf(ifp, "%d", &nextNumber);
	}
}

//This function takes in an identifier and returns the corresponding symbol from
// the symbol table.
symbol identifierToSymbol (char iden[]) {
	int i;

	//Downcounting for-loop
	for (i = symbolsAmount - 1; i >= 0; i--) {
		if (strcmp(symbol_table[i].name, iden) == 0) {
			if (symbol_table[i].level <= curLexLevel)
				return symbol_table[i];
		}
	}
	//If symbol not found, error found.
	error(11);
}

void printSymbolTable(){

    int i;

    printf("**Symbol table**\n");
    printf("kind\tname\tvalue\tlevel\taddress\n");
    for(i = 0; i < symbolsAmount; i++){
        printf("%d\t%s\t%d\t%d\t%d\n", symbol_table[i].kind, symbol_table[i].name, symbol_table[i].val, symbol_table[i].level, symbol_table[i].addr);
    }
}

int print (int op, int l, int m) {

    if(DEBUG){
        printf("mcode %d: %d %d %d\n", codeIndex, op, l, m);
    }

    if(codeIndex > MAX_CODE_LENGTH){
        error(28);
    }

    code[codeIndex].op = op;
    code[codeIndex].l = l;
    code[codeIndex].m = m;
    return codeIndex++;

    /*  Old code
	if (branchCon != 0)
		return;
	fprintf(ofp,"%d %d %d \n", op, l, m);
	*/
}

int codeAddrMod (int loc, int m){

    //Modify the address at location loc
    code[loc].m = m;

    if(DEBUG){
        printf("mcode %d: %d %d %d\n", loc, code[loc].op, code[loc].l, code[loc].m);
    }
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

/*
    Inserts a symbol to the symbol table
    if kind == 1, the symbol is a constant
    if kind == 2, the symbol is a variable
    if kind == 3, the symbol is a procedure
    val is the value of the symbol (variables and constants only)
    lvl is the lexicographical level
    addr is the location in the mcode
*/
void enter(int kind, char* name, int val, int level, int addr){

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

    //Copy the kind
    symbol_table[symbolsAmount].kind = kind;

    //Copy the name
    strcpy(symbol_table[symbolsAmount].name, name);

    //Copy the value (Only relevant if a constant)
    symbol_table[symbolsAmount].val = val;

    //Copy lexicographical level
    symbol_table[symbolsAmount].level = level;

    //Copy the address
    symbol_table[symbolsAmount].addr = addr;

    //Increase symbols counter
    symbolsAmount++;
}

//      DEPRECATED
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

	if(kind == 1)
	{
	    //Add a constant to the symbol table
	    symbol_table[symbolsAmount].val = nextNumber;
	    symbol_table[symbolsAmount].addr = 0;

	}

	else if(kind == 2)
	{
	    //Add a variable to the symbol table
	    symbol_table[symbolsAmount].addr = 4 + varLexLevelAmount();

	}

	else if(kind == 3)
	{
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
		} else if (cur.kind == 2) {
			print(3, (curLexLevel - cur.level), cur.addr);
		} else {
			error(21);
		}
		getNextToken();
	} else if (nextToken == numbersym) {
		print(1,0, nextNumber);
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
	} else {
		expression();
		if (isRelationalOp(nextToken) == 0)
			error(20);
		int mValue = nextToken - 1;
		getNextToken();
		expression();
		print(2, 0,mValue);
	}
}

void program(){

    getNextToken();

    block();

    if(nextToken != periodsym){
        error(9);
    }

    // SIO 0 3
    print(11, 0, 3);
}

void expression(){

    if (nextToken == plussym || nextToken == minussym){
        if (nextToken == minussym){
            //When we have a negative number
            print(2, 0, 1); //OPR 0 1
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
    }
}

void term(){

    factor();

    while (nextToken == multsym || nextToken == slashsym){

        /* Note:
            The reason why getNextToken() and factor() are not called twice
            inside the if-else statement is because we need to remember what
            was the token we had, * or /, before it changes on the next
            getNextToken()
        */
        if (nextToken == multsym){
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
    }
}

void statement(){

    if (nextToken == identsym){         // IDENTIFIER

        char temp[12];
        strcpy(temp, nextIdentifier);
        
        if(DEBUG){
            printf("Identifier is: %s\n", temp);
        }

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

        print(4, (curLexLevel - currentSym.level), currentSym.addr );

    } else if (nextToken == callsym){   // CALL

        getNextToken();

        if(nextToken != identsym){
            error(14);
        }

        symbol proc = identifierToSymbol(nextIdentifier);

        if(proc.kind != 3){
            error(15);
        }

        print(5, (curLexLevel-proc.level), proc.addr);    // CAL L M

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

    }  else if(nextToken == ifsym){

        if(DEBUG)
            printf("STATEMENT: ifsym\n");

        /*
                Steps
            Check for condition
            If contition is true, do not jump
            Else, jump to "else" (Keep track of it's location, use dummy instruction)
            Go through statement inside body of "if"
            jumo to end of else (Keep track if location to jump to and from, use dummy variable)
            check for else
            *First jump arrives here*
            Go through statement inside "else"
            *second jump arrives here*
            end of "if-else"

        */

        getNextToken();

        condition();

        if(nextToken != thensym)
            error(16);

        if(DEBUG){
            printf("Then:\n");
        }
        
        //Dummy line
        int firstJump = print(8, 0, 0);

        getNextToken();

        //Inside the if block
        statement();
        
        int elseJump = print(7, 0, 0);

        if(DEBUG){
            printf("End of Then\n");
        }
        
        //Modify the conditional jump to go here
        codeAddrMod(firstJump, codeIndex);

        //If we have an else block
        if(nextToken == elsesym)
        {

            if(DEBUG){
                printf("Else:\n");
            }

        	getNextToken();

        	//Statement inside "else" block
        	statement();

        	if(DEBUG){
        	    printf("End of Else\n");
            }
        } 
        
        // Modify the unconditional jump to go here 
        codeAddrMod(elseJump, codeIndex);
        
    }
    else if(nextToken == whilesym){

        if(DEBUG)
            printf("STATEMENT: whilesym\n");

        // Record the line before the conditional
        int line1 = codeIndex;
        getNextToken();

        condition();

        // Record the line after the conditional
        int line2 = codeIndex;

        //Dummy line
        print(8, 0, -1);

        if(nextToken != dosym)
            error(18);

        getNextToken();

        //Inside the while body
        statement();

        //record current line
        int lastLine = codeIndex;

        //Get to dummy line
        //codeIndex = firstLine;

        //Overwrite dummy line
        print(7, 0, line1);

        codeAddrMod(line2, codeIndex);

        //Get back to actual line
        //codeIndex = lastLine;

        //print(7, 0, realFirstLine);

    }else if (nextToken == readsym) {  // READ

        getNextToken();
        symbol cur = identifierToSymbol(nextIdentifier);

        print(9,0,1);
        print(4, (curLexLevel - cur.level), cur.addr);
        getNextToken();

    } else if (nextToken == writesym) {  // WRITE

        getNextToken();
        symbol cur = identifierToSymbol(nextIdentifier);

        if (cur.kind == 1)
        	print(1, 0, cur.val);

        if (cur.kind == 2)
        	print(3, (curLexLevel - cur.level), cur.addr);

        print(9,0,0);
        getNextToken();

    }
}


void block() {

    curLexLevel++;
    int space = 4;
    //Record position of dummy instruction, Step 1
	int jmpaddr = print(7, 0, 0);

	if(DEBUG){
	    printf("BLOCK\n");
	}

    // Constant?
    if(nextToken == constsym) {
        do {

            if(DEBUG){
        	    printf("Const\n");
        	}

        	getNextToken();

            if(nextToken != identsym)
                error(4);

            getNextToken();

            if(nextToken != eqsym)
                error(3);

            getNextToken();

            if(nextToken != numbersym)
                error(2);

            getNextToken();

            // Add constant to the symbol table
            enter(1, nextIdentifier, nextNumber, curLexLevel, -1);

            // last getNextToken() will store the comma in nextToken
        } while(nextToken == commasym);

        if (nextToken != semicolonsym)
            error(5);

        getNextToken();

        if(DEBUG){
            printf("end of Const\n");
        }
    }
    if(nextToken == varsym) {

        if(DEBUG){
            printf("Varsym\n");
        }

        //Number of variables
        int numVars = 0;

        do {
            getNextToken();

            if(nextToken != identsym)
                error(4);

            numVars++;
            // space++;

            getNextToken();

            // Add variable to the symbol table
            enter(2, nextIdentifier, 0, curLexLevel, 4+numVars-1);

            // last getNextToken() will store the comma in nextToken
            if(DEBUG){
                printf("numVars %d\n", numVars);
            }

        } while(nextToken == commasym);

        space += numVars;

        if (nextToken != semicolonsym)
            error(5);

       	//print(6, 0, numVars); // INC, 0, M for allocating variable space
        getNextToken();
    }

    // Procedure?
    while(nextToken == procsym) {

        if(DEBUG){
            printf("Procsym\n");
        }

        //Next token should be an identifier
        getNextToken();

        if(nextToken != identsym)
            error(4);


        //INSERT BOGUS ADDRESS. WILL BE MODIFIED LATER.
        enter(3, nextIdentifier, 0, curLexLevel, codeIndex);

        getNextToken();

        // Add procedure to the symbol table | DEPRECATED
        //insertToSymbolTable(3);

        //getNextToken(); // was causing errors (semicolon read by previous getNextToken()?)

        if(nextToken != semicolonsym)
            error(5);

        getNextToken();

        //Something needs to be returned from block()
        //  That something will be the address of the procedure we stored
        //  a few lines up.
        block();

        if(nextToken != semicolonsym)
            error(5);

        getNextToken();

        if(DEBUG){
            printf("end of Procsym\n");
        }
    }

    if(DEBUG){
        printf("Level: %d\n", curLexLevel);
        printSymbolTable();
    }


    //Supposed to be
    /*
    code[jmpaddr].addr = NEXT_CODE_ADDR
    gen(INC, 0, space)
    STATEMENT()
    gen(RTN, 0, 0)
    level--
    */
    codeAddrMod(jmpaddr, codeIndex);
    print(6, 0, space);
    statement();
    
    //We do not need to return from the main block.
    //  Therefore, only print the return statement if lex level > 1
    if(curLexLevel >= 1)
        print(2, 0, 0);
        
    curLexLevel--;
    symbolsAmount = symbolsAmount - deleteOldLevels(curLexLevel);

    //At this point, we should delete old levels off the table

    /*  DEPRECATED. SAME AS THE THING ABOVE
    int lastLine = codeIndex;
    codeIndex = firstLine;
    print(7, 0, lastLine+1);
    codeIndex = lastLine+1;
    print(6, 0, space); // Step 2
    statement();
    print(2, 0, 0); // Step 4
    curLexLevel--;
    */
}

int deleteOldLevels(int lexLevel)
{
	int numSymbols = 0; //number of symbols in the table that have been deleted
	int i;

	//loop through the number of symbols in the table
	for(i = 0; i < symbolsAmount; i++)
	{
		if(symbol_table[i].level > lexLevel)//if the level of the current symbol is greater than the current level
		{
				if(symbol_table[i].kind == 1)//if the symbol is a constant
				{
					symbol_table[i].kind = 0;
					symbol_table[i].level = 0;
					symbol_table[i].val = 0;
					//symbol_table[i].name[0] = 0;
					strcpy(symbol_table[i].name, "");
					numSymbols++;
				}

				else //if the symbol is a procedure or variable
				{
					symbol_table[i].kind = 0;
					symbol_table[i].addr = 0;
					symbol_table[i].level = 0;
					//symbol_table[i].name[0] = 0;
					strcpy(symbol_table[i].name, "");
					numSymbols++;
				}
		}
	}

	return numSymbols;
}


void printMcode(){

    int i;
    for (i = 0; i < codeIndex; i++) {
    	fprintf(ofp, "%d %d %d\n", code[i].op,code[i].l, code[i].m);
    }

}


void main()
{
    ifp = fopen("tokenlist.txt", "r");
    program();

    //Does OFP need to be a global variable?
    ofp = fopen("mcode.txt","w");
    printMcode();
    fclose(ifp);
    fclose(ofp);
}