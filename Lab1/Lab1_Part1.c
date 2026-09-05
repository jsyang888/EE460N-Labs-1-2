#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

FILE* infile = NULL;
FILE* outfile = NULL;


int readAndParse(
    FILE *pInfile,
    char *pLine,
    char **pLabel,
    char **pOpcode,
    char **pArg1,
    char **pArg2,
    char **pArg3,
    char **pArg4
);

int toNum(char *pStr); //Converts string to number
int isOpcode(char *opcode); //Determines if string is opcode; returns 0 if opcode, -1 if not opcode
int insert_symbol(const char *name, int address); //Inserts symbol into symbol table; returns -1 if table already contains symbol or if table is full
int find_symbol(const char *name); //Searches for symbol in symbol table; returns address of symbol, -1 if not found
void write_symbol_table(FILE *out); // test function so we can see the symbol table generated.
//function prototype definitions

#define MAX_LINE_LENGTH 255
	enum
	{
	   DONE, OK, EMPTY_LINE
	};

// Symbol Table Stuff
typedef struct {
    char name[21]; // each name has a max of 20 characters + null terminator
    int address; // for what mem address it's at
    int define_check; // if it's been defined yet 
} Symbol;

#define TABLE_SIZE 1024

Symbol table[TABLE_SIZE]; // initial size, maybe add feature to increase size if running out? idk ignore if we pass all tests
int symbol_count = 0;

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

    char line[MAX_LINE_LENGTH];

    char *label;
    char *opcode;
    char *arg1;
    char *arg2;
    char *arg3;
    char *arg4;

    int status;
    int locationCounter = 0;
    int originFound = 0; //check if there's an origin before code
    int endFound = 0;

    /* Do stuff with files */

    // first pass with assigning symbols
    while ((status = readAndParse(infile, line, &label, &opcode, &arg1, &arg2, &arg3, &arg4)) != DONE) {
        if (status == EMPTY_LINE) {
            continue; // ignore the blanks
        }
        if (strcmp(opcode, ".orig") == 0) {
            int origin;
            if (!toNum(arg1)) // invalid number
            {
                exit(1);
            }

			origin = toNum(arg1);

            if (origin < 0 || origin > 0xFFFF) // out of range
            {
                exit(1);
            }

            if (origin % 2 != 0) // not even
            {
                exit(1);
            }

            locationCounter = origin;
            originFound = 1;
            
            continue;
        }

        if (!originFound) {
            exit(1); // no origin found
        }

        if (strcmp(opcode, ".end") == 0) {
            endFound = 1;
            continue;
        }

        // now, check for a label
        if (label[0] != '\0') {
            if (insert_symbol(label, locationCounter) == -1) {
                fprintf(stderr, "Error when adding to symbol table:%s %d", label, locationCounter);
                return(1); // error when adding to symbol table
            }

        }
        if (opcode[0] == '\0') {
            continue; // possible for a line to just have a label, but nothing else so just continue
        }

        if (isOpcode(opcode) != -1) {
            if (locationCounter > 0xFFFE) {
                return(1);
            }
            locationCounter += 2;
        }

        if (strcmp(opcode, ".fill") == 0) {
            //fill line command
            int value;

            if (!toNum(arg1)) {
                fprintf(stderr, "Not a valid fill value: .fill %s", arg1);
                return(1); // not a valid fill value
            }

			value = toNum(arg1);

            if (value < -32768 || value > 0xFFFF) {
                fprintf(stderr, "Value is out of bounds: %d\n", value);
                return (1); // out of bounds
            }
            if (locationCounter > 0xFFFE) {
                fprintf(stderr, "Location is out of bounds: %d\n", locationCounter);
                return (1); // idk how but the user manages to put the fill command out of memory
            }
            locationCounter +=2; // increment
        }
    }    
    printf("About to write %d symbols\n", symbol_count);
    write_symbol_table(outfile);
	/* Done doing stuff with files */

    fclose(infile);
    fclose(outfile);
}


int readAndParse(
    FILE *pInfile, // pointer for in file
    char *pLine, // pointer for what line we are on
    char **pLabel, //pointer for the label, if it exists
    char **pOpcode, //pointer for the op code
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



int toNum(char * pStr)
{
   char * t_ptr;
   char * orig_pStr;
   int t_length,k;
   int lNum, lNeg = 0;
   long int lNumLong;

   orig_pStr = pStr;
   if( *pStr == '#' )				/* decimal */
   { 
     pStr++;
     if( *pStr == '-' )				/* dec is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isdigit(*t_ptr))
       {
	 printf("Error: invalid decimal operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNum = atoi(pStr);
     if (lNeg)
       lNum = -lNum;
 
     return lNum;
   }
   else if( *pStr == 'x' )	/* hex     */
   {
     pStr++;
     if( *pStr == '-' )				/* hex is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isxdigit(*t_ptr))
       {
	 printf("Error: invalid hex operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
     lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
     if( lNeg )
       lNum = -lNum;
     return lNum;
   }
   else
   {
	printf( "Error: invalid operand, %s\n", orig_pStr);
	exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
   }
}

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
    
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(table[i].name, checked_name) == 0) {
            fprintf(stderr, "%s already exists in table\n", checked_name);
            return -1; //that means this symbol already exists in the table, can't have dupes
        } 
    }
    if (symbol_count >= TABLE_SIZE) {
        fprintf(stderr, "symbol table is full!\n");
        return -1; //table is full
    }
    strcpy(table[symbol_count].name, checked_name);
    table[symbol_count].address = address;
    table[symbol_count].define_check = 1;
    symbol_count++; // assigning all the fields
    return 0;
}

int find_symbol(const char* name) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (strcmp(name, table[i].name) == 0) {
            return table[i].address;
        }
    }
    fprintf(stderr, "symbol not found!\n");
    return -1;

}

int assign_symbols(char **pLabel, int address) {
    if (*pLabel == NULL) return -1;
    if (insert_symbol(*pLabel, address) == -1) {
        fprintf(stderr, "Error while insertion!\n");
        return -1; // threw some sort of error while inserting
        
    }
    return 0;
}

int isOpcode(char *opcode) {
    fprintf(stderr, "%s\n", opcode);
    const char *opcodes[] = {
        "add", "and", "br", "brn", "brz", "brp",
        "brnz", "brnp", "brzp", "brnzp",
        "halt", "jmp", "jsr", "jsrr",
        "ldb", "ldw", "lea", "nop", "not", "ret",
        "lshf", "rshfl", "rshfa", "rti",
        "stb", "stw", "trap", "xor"
    };

    for (int i = 0; i < sizeof(opcodes) / sizeof(char*); i++) {
        if (strcmp(opcodes[i], opcode) == 0) {
            fprintf(stderr, "success!\n");
            return 0;
        }
    }
    return -1;
}

// temporary testing function for the symbol table, running old LC3 labs here.
void write_symbol_table(FILE *out)
{
    fprintf(out, "Label                 Address\n");
    fprintf(out, "--------------------  -------\n");

    for (int i = 0; i < symbol_count; i++)
    {
        fprintf(
            out,
            "%-20s  x%04X\n",
            table[i].name,
            table[i].address & 0xFFFF
        );
    }
}
