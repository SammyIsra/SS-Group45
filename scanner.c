/*
    Meenakshi Karthikeya
    Victoria Rivas
    Sammy Israwi
    COP3402 - System Software
    Assignment 2
*/

#include <stdio.h>
#include <stdlib.h>

//Function prototypes
void cleanCode(char* inputFile, char* outputFile);

//Token Table
typedef enum {
    nulsym = 1, identsym, numbersym, plussym, minussym, multsym,  slashsym, oddsym,
    eqsym, neqsym, lessym, leqsym, gtrsym, geqsym, lparentsym, rparentsym, commasym,
    semicolonsym, periodsym, becomessym, beginsym, endsym, ifsym, thensym, whilesym,
    dosym, callsym, constsym, varsym, procsym, writesym, readsym, elsesym
} token_type;

//Reserved Words
const char* words[13] = {
                            "begin", "end", "if", "then", "while", "do", "call", 
                            "const", "var", "procedure", "write", "read", "else"
                        };






int main()
{
    
    //First thing to do is to clean the input
    cleanCode("input.txt", "cleaninput.txt");

    return 0;
}

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
    {
        printf("File not found");
        return;
    }

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
