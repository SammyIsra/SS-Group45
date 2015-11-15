/*
    Meenakshi Karthikeya
    Victoria Rivas
    Sammy Israwi
    COP3402 - System Software
    Assignment 2: PL/0 Scanner
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Function prototypes
void cleanCode(char* inputFile, char* outputFile);
void error(int line, char *msg);
int tokenTranslate(char* temp);
void lexemTable(char* inputFile, char* outputFile);
char* appendC(char *temp, int tempsize, char c);
int tokenTranslate(char* temp);
int is_Special(char c);
void lexemeList(char *inputFile, char* outputFile);
void tokensList(char* inputFile, char* outputFile);

//Token Table
typedef enum {
    nulsym = 1, identsym, numbersym, plussym, minussym, multsym,  slashsym, oddsym,
    eqsym, neqsym, lessym, leqsym, gtrsym, geqsym, lparentsym, rparentsym, commasym,
    semicolonsym, periodsym, becomessym, beginsym, endsym, ifsym, thensym, whilesym,
    dosym, callsym, constsym, varsym, procsym, writesym, readsym, elsesym
} token_type;


#define IDENTIFIER_MAX_LEN 11
#define NUMBER_MAX_LEN 5
#define TOKEN_MAX_LEN 32


int main(){
    
    //First thing to do is to clean the input
    cleanCode("input.txt", "cleaninput.txt");

    //Second step is to make the lexem table from the clean input
    lexemTable("cleaninput.txt", "lexemetable.txt");

    tokensList("lexemetable.txt", "tokenlist.txt");

    return 0;
}

/*
    For when there is a mess up.
        Prints the error, and
        exits the program
*/
void error(int line, char *msg){
    printf("Line %d - ERROR: %s\n", line, msg);
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
        error(lineCount, "Could not open the Lexem Table file");

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