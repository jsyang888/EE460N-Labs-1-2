#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

FILE* infile = NULL;
FILE* outfile = NULL;

int main(int argc, char* argv[]) {
    /* open the source file */
    infile = fopen(argv[1], "r");
    outfile = fopen(argv[2], "w");
		 
    if (!infile) {
       printf("Error: Cannot open file %s\n", argv[1]);
       exit(4);
	}
    if (!outfile) {
       printf("Error: Cannot open file %s\n", argv[2]);
       exit(4);
    }

    /* Do stuff with files */

    fclose(infile);
    fclose(outfile);
}

#define MAX_LINE_LENGTH 255
	enum
	{
	   DONE, OK, EMPTY_LINE
	};

int readAndParse(
    FILE *pInfile,
    char *pLine,
    char **pLabel,
    char **pOpcode,
    char **pArg1,
    char **pArg2,
    char **pArg3,
    char **pArg4
    ) 
    {
	   char * lRet, * lPtr;
	   int i;
	   if(!fgets( pLine, MAX_LINE_LENGTH, pInfile))
		return( DONE );
	   for( i = 0; i < strlen( pLine ); i++ )
		pLine[i] = tolower( pLine[i] );
	   
           /* convert entire line to lowercase */
	   *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

	   /* ignore the comments */
	   lPtr = pLine;

	   while( *lPtr != ';' && *lPtr != '\0' &&
	   *lPtr != '\n' ) 
		lPtr++;

	   *lPtr = '\0';
	   if( !(lPtr = strtok( pLine, "\t\n ," ) ) ) 
		return( EMPTY_LINE );

	   if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
	   {
		*pLabel = lPtr;
		if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	   }
	   
           *pOpcode = lPtr;

	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	   
           *pArg1 = lPtr;
	   
           if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg2 = lPtr;
	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg3 = lPtr;

	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg4 = lPtr;

	   return( OK );
	}

	/* Note: MAX_LINE_LENGTH, OK, EMPTY_LINE, and DONE are defined values */

    // basically splits up the line of the code into tokens, isOpCode can output a -1, which would indicate
    // not a real instruction. Or if first char is a dot, indicates pseudocode.

// Symbol Table Stuff
typedef struct {
    char name[21]; // each name has a max of 20 characters + null terminator
    int address; // for what mem address it's at
    int define_check; // if it's been defined yet 
} Symbol;

#define TABLE_SIZE 1024

Symbol table[TABLE_SIZE]; // initial size, maybe add feature to increase size if running out? idk ignore if we pass all tests
int symbol_count = 0;

int insert_symbol(const char* name, int address) { 
    // check if name is too long first
    char checked_name[21];
    if (strlen(name) > 20) {
        //error, truncate it
        strncpy (checked_name, name, 20);
    }
    else {
        strcpy (checked_name, name);
    }
    
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (strcmp(table[i].name, checked_name) == 0) {
            return -1; //that means this symbol already exists in the table, can't have dupes
        } 
    }
    if (symbol_count >= TABLE_SIZE) {
        return -1; //table is full
    }
    strcpy(table[symbol_count].name, checked_name);
    table[symbol_count].address = address;
    table[symbol_count].define_check = 1;
    symbol_count++; // assigning all the fields
}

int find_symbol(const char* name) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (strcmp(name, table[i].name) == 0) {
            return table[i].address;
        }
    }
    return -1;

}

