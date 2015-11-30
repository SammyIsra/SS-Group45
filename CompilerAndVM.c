/* Assignment 4 - Integrated compiler and VM
    Sammy Israwi
    Meenakshi Karthikeya
    Victoria Rivas
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Scanner constants
#define IDENTIFIER_MAX_LEN 11
#define NUMBER_MAX_LEN 5
#define TOKEN_MAX_LEN 32

//Parser constants
#define MAX_SYMBOL_TABLE_SIZE 100
#define MAX_CODE_LENGTH 500
#define DEBUG 0

//VM constants
#define MAX_STACK_HEIGHT 2000
#define MAX_LEXI_LEVELS 3

/*
	Instruction Structure
*/
typedef struct instruction{
	int op;   // opcode
	int  l;   // L
	int  m;   // M
} instruction;

typedef struct symbol{
	int kind; 		//const = 1, var = 2, proc = 3
	char name[12]; 	//name up to 11 chars
	int val; 		//value
	int level;		//L level
	int addr;		//M address
} symbol;

//Scanner functions
void cleanCode(char* inputFile, char* outputFile);
void error(int line, char *msg);
void error_noline(char *msg);
int tokenTranslate(char* temp);
void lexemTable(char* inputFile, char* outputFile);
char* appendC(char *temp, int tempsize, char c);
int tokenTranslate(char* temp);
int is_Special(char c);
void lexemeList(char *inputFile, char* outputFile);
void tokensList(char* inputFile, char* outputFile);

//Parser functions
void readTokens(char* input);
void parserError( int errorNo );
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

//VM functions
int base(int l, int base);
char *decode(int opNum);
instruction fetch();
void execute();
void printInstruction(FILE *printFile, int line, instruction ins, char *append, int screen);
void printProgram(FILE *printFile, int lines, int screen);
void printRegisters(int screen);
void printStack(int screen);

//Token Table
typedef enum {
    nulsym = 1, identsym, numbersym, plussym, minussym, multsym,  slashsym, oddsym,
    eqsym, neqsym, lessym, leqsym, gtrsym, geqsym, lparentsym, rparentsym, commasym,
    semicolonsym, periodsym, becomessym, beginsym, endsym, ifsym, thensym, whilesym,
    dosym, callsym, constsym, varsym, procsym, writesym, readsym, elsesym
} token_type;

/*
	VM Global Variables
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

//Parser Global Variables
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
instruction code[MAX_CODE_LENGTH];
char nextIdentifier[12];
int nextToken;
int nextNumber;
int symbolsAmount = 0;	//Amount of symbols in the symbol table
int curLexLevel = -1;
int lineCount = 0;
int branchCon = 0;
int codeIndex = 0; //Index of the code that we are writing to, used for jumps and such

FILE* ifp;
FILE* ofp;
FILE *acode;


/*
	Parser Error function
		Takes in a number
		Prints out the error message associated with the number received
		Exits out of the program
*/
void parserError( int errorNo )
{

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

/*
    For when there is a mess up in the scanner.
        Prints the error, and
        exits the program
*/
void error(int line, char *msg){
    printf("Line %d - ERROR: %s\n", line, msg);
    exit(EXIT_FAILURE);
}

void error_noline(char *msg){
    printf("ERROR: %s\n", msg);
    exit(EXIT_FAILURE);
}

/*
    Clean Code
        Will take in the raw input file with the name 'inputFile'
        Will return a version of the file stripped of all comments
*/
void cleanCode(char* inputFile, char* outputFile){

    FILE *ipf, *opf;
    char c, next;
    int com_flag = 0;

    //Input file
    ipf = fopen(inputFile, "r");
    //Output file
    opf = fopen(outputFile, "w");

    //If the pointer to the input file returns null, end the program
    if(ipf == NULL)
        error(0, "File not found");

    while(1)
    {
        //Read the next character
        c = fgetc(ipf);

        //If we are at the end of the file, then break out of the loop
        if (c == EOF)
            break;
        //Check if at the beginning of a comment
        else if(c == '/')
        {
            //If the next character is a star, then replace it with a space in the output file.
            //Otherwise, print the character
            next = fgetc(ipf);
            if(next == '*')
            {
                fputc(' ',opf);
                com_flag = 1;
            }
            else
            {
                fputc(c, opf);
                fputc(next, opf);
            }
        }
        //Check if at the end of a comment
        else if(c == '*')
        {
            next = fgetc(ipf);
            if(next == '/')
            {
                fputc(' ', opf);
                com_flag = 0;
            }
            else
            {
                fputc(c,opf);
                fputc(next, opf);
            }

        }

        //Print the character if not within a comment
        else if(!com_flag)
            fputc(c,opf);

    }

    //Close files
    fclose(ipf);
    fclose(opf);

    return;
}

/*
    Lexem Table
        Takes in the input file AFTER being stripped of all comments
        Putputs a file that consists of the lexem table of all symbols
*/
void lexemTable(char* inputFile, char* outputFile){

    FILE *input, *output;

    //Input file
    input = fopen(inputFile, "r");
    //Output file
    output = fopen(outputFile, "w");

    int lineCount = 0;

    // Error opening the file
    if(input == NULL)
        error(lineCount, "Could not open the file.\n");

    char temp[TOKEN_MAX_LEN] = "";   // we will read the token into this
    char c = '0';                   // current char
    char next = '0';                // look ahead char
    int doubleSymbol = 0;              // flag used for if we hit a double symbol (>=, :=, etc)
    int tokenType;                    // for converting token to internal representation

    //Print table headers
    fprintf(output, "lexeme\ttoken type\n");

    //While c is not the End Of File
    while(c != EOF){

        c = fgetc(input);

        if(c == '\n'){
            lineCount++;
        }

        if(isalpha(c)){     //If the char is a letter

            //This gets the full token
            while(isalnum(c)){   //if the char is a letter or number
                appendC(temp, sizeof(temp), c);
                c = fgetc(input);
            }

            //Apparently theres a function that puts a character BACK to a stream...
            //Who knew...
            ungetc(c, input);

            //Get the symbol number from the token table that we got from prof
            tokenType = tokenTranslate(temp);

            if(tokenType != -1)    //If the token is a reserved word or a symbol
                fprintf(output, "%s\t%d\n", temp, tokenType);
            else if(strlen(temp) <= IDENTIFIER_MAX_LEN) //If the token is an identifier and does not exceed the length
                fprintf(output, "%s\t%d\n", temp, identsym);
            else    //Not a reserved word, and name is too damn long
                error(lineCount, "Name of identifier is too long.\n");

            //Reset the size of temp buffer to 0
            temp[0] = '\0';

            //End of case if first char of token is a letter
        }else if(isdigit(c)){    //If the char is a digit

            //Get the full number
            while(isdigit(c)){
                appendC(temp, sizeof(temp), c);
                c = fgetc(input);
            }

            //Temp should NOT be tjos long
            if(isalpha(c))
                error(lineCount, "Variables should not start with numbers.\n");

            //Remember ungetc?
            ungetc(c, input);

            if(strlen(temp) <= NUMBER_MAX_LEN)
                fprintf(output, "%s\t%d\n", temp, numbersym);
            else
                error(lineCount, "Number is too long.\n");

            temp[0] = '\0';

            //End of case if it starts with a number

        }else if(is_Special(c)){ //Had to make a special function for this because of out specific alphabet

            //The first couple of cases check if we have any of the double symbols
            // Theres probably an easy way to do this, but I can't think of it right now

            //First of all, we have to check if we have a ':='
            if(c == ':'){

                next = fgetc(input);

                if(next == '='){
                    fprintf(output, ":=\t%d\n", becomessym);
                    doubleSymbol = 1;
                }else{
                    ungetc(next, input);
                    error(lineCount, "Invalid symbol.\n");
                }
            }

            //Check for '<='
            if(c == '<'){

                next = fgetc(input);

                if(next == '='){
                    fprintf(output, "<=\t%d\n", leqsym);
                    doubleSymbol = 1;
                }else{
                    ungetc(next, input);
                }
            }

            //Check for '>='
            if(c == '>'){

                next = fgetc(input);

                if(next == '='){
                    fprintf(output, ">=\t%d\n", geqsym);
                    doubleSymbol = 1;
                }else{
                    ungetc(next, input);
                }
            }


            //Check for '<>'
            if(c == '<'){

                next = fgetc(input);

                if(next == '>'){
                    fprintf(output, "<>\t%d\n", neqsym);
                    doubleSymbol = 1;
                }else{
                    ungetc(next, input);
                }
            }

            //Now we check for all other symbols
            if(doubleSymbol == 0){
                temp[0] = c;
                temp[1] = '\0';

                //Translate to whatever we have on our table
                tokenType = tokenTranslate(temp);

                if(tokenType != -1)
                    fprintf(output, "%s\t%d\n", temp, tokenType);

                temp[0] = '\0';
            }

            //Resetting everything for good measure.
            doubleSymbol = 0;
            temp[0] = '\0';

        }else if(isprint(c) && !isspace(c) && !is_Special(c)){
            //This is the "you seriously screwed up" part
            //This section of code will only be reached if we have a character that
            //  *Is a printable character, AND
            //  *is NOT a space character, AND
            //  *is NOT one of our special characters, AND
            //  *is NOT a digit, AND
            //  *is NOT a alpha character
            error(lineCount, "Invalid symbol.\n");
        }
    }

    fclose(input);
    fclose(output);

}

char* appendC(char *temp, int tempsize, char c){

    int len = strlen(temp);

    if((len+1) < tempsize){
        temp[len++] = c;     //Character to append
        temp[len] = '\0';    //End of String
        return temp;
    }else{
        return NULL;
    }
}

/*
    Translate token to number from enum
        Takes in a string representing the token
        returns a number from the symbol table
        Is a very ugly function
*/
int tokenTranslate(char* temp){

    int retu;   //Returnable value

    //At night, I pray for pretty code. Instead, I get this.
    if(strcmp(temp, "+") == 0)
        retu = plussym;
    else if(strcmp(temp, "-") == 0)
        retu = minussym;
    else if(strcmp(temp, "*") == 0)
        retu = multsym;
    else if(strcmp(temp, "/") == 0)
        retu = slashsym;
    else if(strcmp(temp, "odd") == 0)
        retu = oddsym;
    else if(strcmp(temp, "=") == 0)
        retu = eqsym;
    else if(strcmp(temp, "<>") == 0)
        retu = neqsym;
    else if(strcmp(temp, "<") == 0)
        retu = lessym;
    else if(strcmp(temp, "<=") == 0)
        retu = leqsym;
    else if(strcmp(temp, ">") == 0)
        retu = gtrsym;
    else if(strcmp(temp, ">=") == 0)
        retu = geqsym;
    else if(strcmp(temp, "(") == 0)
        retu = lparentsym;
    else if(strcmp(temp, ")") == 0)
        retu = rparentsym;
    else if(strcmp(temp, ",") == 0)
        retu = commasym;
    else if(strcmp(temp, ";") == 0)
        retu = semicolonsym;
    else if(strcmp(temp, ".") == 0)
        retu = periodsym;
    else if(strcmp(temp, ":=") == 0)
        retu = becomessym;
    else if(strcmp(temp, "begin") == 0)
        retu = beginsym;
    else if(strcmp(temp, "end") == 0)
        retu = endsym;
    else if(strcmp(temp, "if") == 0)
        retu = ifsym;
    else if(strcmp(temp, "then") == 0)
        retu = thensym;
    else if(strcmp(temp, "while") == 0)
        retu = whilesym;
    else if(strcmp(temp, "do") == 0)
        retu = dosym;
    else if(strcmp(temp, "call") == 0)
        retu = callsym;
    else if(strcmp(temp, "const") == 0)
        retu = constsym;
    else if(strcmp(temp, "var") == 0)
        retu = varsym;
    else if(strcmp(temp, "procedure") == 0)
        retu = procsym;
    else if(strcmp(temp, "write") == 0)
        retu = writesym;
    else if(strcmp(temp, "read") == 0)
        retu = readsym;
    else if(strcmp(temp, "else") == 0)
        retu = elsesym;
    else
        retu =  -1;  //Symbol is not recognized

    return retu;
}

/*
    Is Special?
        Returns 1 if we the character is an allowed special character
        0 if it's not
*/
int is_Special(char c){

    if( c == '+' || c == '-' || c == '*' ||
        c == '/' || c == '(' || c == ')' ||
        c == '=' || c == ',' || c == '.' ||
        c == '<' || c == '>' || c == ';' ||
        c == ':')
        return 1;

    return 0;
}

/*
    Tokens List
        Takes in the lexemtable from last section
        Prints out the lexem list from that
*/
void tokensList(char* inputFile, char* outputFile){

    FILE *input = fopen(inputFile, "r");
    FILE *output = fopen(outputFile, "w");

    char trash[128] = "";
    int code, some;
    char hold[TOKEN_MAX_LEN] = "";

    if(input == NULL)
        error_noline("Could not open the Lexem Table file");

    //Remove the headers from the stream
    fscanf(input, "%s", trash); //Header 1
    fscanf(input, "%s", trash); //Header 2
    fscanf(input, "%s", trash); //Random '0'?

    while((some = fscanf(input, "%s", hold)) > 0){

        //Get the token number
        fscanf(input, "%d", &code);

        //Print said token
        fprintf(output, "%d ", code);

        if(code == 2 || code == 3)
            fprintf(output, "%s ", hold);
    }

    //Close the files
    fclose(input);
    fclose(output);
}

void readTokens(char* input)
{
	FILE *ipf = fopen(input, "r");
	char c = '0';

	if(ipf == NULL)
		printf("File could not be found");

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
	parserError(11);
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
        parserError(28);
    }

    code[codeIndex].op = op;
    code[codeIndex].l = l;
    code[codeIndex].m = m;
    return codeIndex++;

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
			parserError(26);
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
			parserError(26);
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
			parserError(21);
		}
		getNextToken();
	} else if (nextToken == numbersym) {
		print(1,0, nextNumber);
		getNextToken();
	} else if (nextToken == lparentsym) {
		getNextToken();
		expression();
		if (nextToken != rparentsym)
			parserError(22);
		getNextToken();
	} else {
		parserError(23);
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
			parserError(20);
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
        parserError(9);
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
            parserError(12);
        }

        getNextToken();

        if(nextToken != becomessym){
            parserError(3);
        }

        getNextToken();

        expression();

        symbol currentSym = identifierToSymbol(temp);

        print(4, (curLexLevel - currentSym.level), currentSym.addr );

    } else if (nextToken == callsym){   // CALL

        getNextToken();

        if(nextToken != identsym){
            parserError(14);
        }

        symbol proc = identifierToSymbol(nextIdentifier);

        if(proc.kind != 3){
            parserError(15);
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
            parserError(27);
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
            parserError(16);

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
            parserError(18);

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
                parserError(4);

            getNextToken();

            if(nextToken != eqsym)
                parserError(3);

            getNextToken();

            if(nextToken != numbersym)
                parserError(2);

            getNextToken();

            // Add constant to the symbol table
            enter(1, nextIdentifier, nextNumber, curLexLevel, -1);

            // last getNextToken() will store the comma in nextToken
        } while(nextToken == commasym);

        if (nextToken != semicolonsym)
            parserError(5);

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
                parserError(4);

            numVars++;

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
            parserError(5);

        getNextToken();
    }

    // Procedure
    while(nextToken == procsym) {

        if(DEBUG){
            printf("Procsym\n");
        }

        //Next token should be an identifier
        getNextToken();

        if(nextToken != identsym)
            parserError(4);


        //INSERT BOGUS ADDRESS. WILL BE MODIFIED LATER.
        enter(3, nextIdentifier, 0, curLexLevel, codeIndex);

        getNextToken();

        if(nextToken != semicolonsym)
            parserError(5);

        getNextToken();

        //Something needs to be returned from block()
        //  That something will be the address of the procedure we stored
        //  a few lines up.
        block();

        if(nextToken != semicolonsym)
            parserError(5);

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
					strcpy(symbol_table[i].name, "");
					numSymbols++;
				}

				else //if the symbol is a procedure or variable
				{
					symbol_table[i].kind = 0;
					symbol_table[i].addr = 0;
					symbol_table[i].level = 0;
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
	Takes in a line number, instruction, and a string to append to the end of the instruction (for seperators)
	Prints the line number, followed by the instruction, and the string appended to the end
*/
void printInstruction(FILE* printFile, int line, instruction ins, char *append, int screen){

	char* decodedOP = decode(ins.op);

	// Print the instruction
	fprintf(printFile, "%2d\t%s\t%d\t%d", line, decodedOP, ins.l, ins.m);

	// Print the string to be appended (separator)
	fprintf(printFile, "%s", append);

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
void printProgram(FILE *printFile, int lines, int screen){

	int j;

	// Print the code heading
	fprintf(printFile, "Line\tOP\tL\tM\n");

	// Print the code stored in code[]
	for(j = 0; j < lines; j++)
		printInstruction(printFile, j, code[j], "\n", screen);
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

int main(int argc, char *argv[])
{
    //Begin scanner
    //First thing to do is to clean the input
    cleanCode("input.txt", "cleaninput.txt");

    //Second step is to make the lexem table from the clean input
    lexemTable("cleaninput.txt", "lexemetable.txt");

    tokensList("lexemetable.txt", "tokenlist.txt");

    //Begin parser
    ifp = fopen("tokenlist.txt", "r");
    program();

    ofp = fopen("mcode.txt","w");
    printMcode();
    fclose(ifp);
    fclose(ofp);

    //Begin VM
    // Print to screen (used for debugging)
	int screen = 0;

	ifp = fopen("mcode.txt", "r");	// Open the input file
	ofp = fopen("stacktrace.txt", "w");		// Open the output file for stack trace (ofp is a global variable)
	acode = fopen("acode.txt", "w");  //Open output file for disassembled code
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
	printProgram(acode, line, 0);
	// Print the heading for the stack trace along with the initial register values
	fprintf(ofp, "\t\t\t\tpc\tbp\tsp\tstack\n");
	fprintf(ofp, "Initial values\t\t\t");
	printRegisters(0);
	fprintf(ofp, "\n");

	// Same thing as above, but print to screen
	if(screen){
		printProgram(acode,line, 1);
		printf("\t\t\t\tpc\tbp\tsp\tstack\n");
		printf("Initial values\t\t\t");
		printRegisters(1);
		printf("\n");
	}

	// If the base pointer is not 0
	// AND no halt signal has been detected,
	// THEN continue execution of the program
	while(bp != 0 && halt != 0) {
		ir = fetch();								// Fetch instruction into instruction register
		printInstruction(ofp,pc-1, ir, "\t", screen);	// Print it
		execute();									// Execute the instruction
		printRegisters(screen);						// Print the registers
		printStack(screen);			// Print the stack trace
	}

    if(screen)
        printf("\n");

    return 0;
}
