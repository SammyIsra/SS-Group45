/*
Victoria Rivas
COP3402 - System Software
Assignment 2
Clean Input
*/

#include <stdio.h>
#include <stdlib.h>

int main()
{
    /*local variables*/
    FILE *ipf, *opf;
    char c, next;
    int com_flag = 0;

    ipf = fopen("input.txt", "r");
    opf = fopen("cleaninput.txt", "w");

    /*If the pointer to the input file returns null,
    end the program*/
    if(ipf == NULL)
    {
        printf("File not found");
        return 0;
    }

    while(1)
    {
        /*Read the next character*/
        c = fgetc(ipf);

        /*If we are at the end of the file, then break out of the loop*/
        if (c == EOF)
            break;

        /*Check if at the beginning of a comment*/
        else if(c == '/')
        {
            /*If the next character is a star, then replace it
            with a space in the output file. Otherwise,
            print the character*/
            next = fgetc(ipf);
            if(next == '*')
            {
                fputc(' ',opf);
                com_flag = 1;
            }
            else
            {
                fputc(c,opf);
                fputc(next, opf);
            }
        }

        /*Check if at the end of a comment*/
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

        /*Print the character if not within a comment*/
        else if(!com_flag)
            fputc(c,opf);

    }

    fclose(ipf);
    fclose(opf);

    return 0;
}
