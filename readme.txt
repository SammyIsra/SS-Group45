PL/0 Compiler

Parts included:
    Building instructons
        Easy way
        Manual way
    Output files explanation
    Explanation of files included
    Names of the developers
    
Building instrucitons

    Keep in mind that the project assumes that there is a PL/0 source code in the source folder. The file must be called 'input.txt'.
    We created a script to make it easier to compile and run the project.

Easy way
    The script included does the exact same thing a user would do when manually building and running the project: it compiles and then runs all parts of the project.
    To run the script, simply type and run the following command on a UNIX based machine:
        ./build.sh
    The process will create executables for all parts of the project and the output they produce.
    
Manual way
    To build and run the project manually, type the following commands
        gcc scanner.c -o scan.exe
        gcc Parser.c -o parse.exe
        gcc vm.c -o vm.exe
        ./scan.exe
        ./parse.exe
        ./vm.exe
    The first three commands will build the project and create executables for each part.
    The last three commands will run the parts in the correct order.

Output files explanation
    cleaninput.txt
        The input file without any comments.
    lexemetable.txt
        The table of all tokens
    tokenlist.txt
        The list of all tokens, identifiers, and numbers in the order they appear in. Used by the parser.
    mcode.txt
        The list of instruction that will be read by the Virtual Machine. 'Machine Code' to be read by the machine.
    stacktrace.txt
        The output of the Virtual Machine after executing the mcode (machine code). 
        The file details the status of the stack at every step of the execution, and the instructions that are being read.
        
Explanation of files included
    vm.c
        The virtual machine interpreter for PM/0, performs ALU operations and generates a stacktrace.
    scanner.c
        Implements the lexical analyzer (scanner) for PL/0. 
        Generates the source code file without comments,t he lexeme table, and the token list.
    parser.c
        Implements the parser (syntax analyzer) and PM/0 code generator for PL/0.
        Reads the output of the PL/0 scanner and generates a symbol table and machine code suitable for execution in the virtual machine
    'demo' folder
        All files in the folder called 'demo' are the input and output files of a successful run. 
        
Developers
    Sammy Israwi
    Meenakshi Karthikeya
    Victoria Rivas
    
