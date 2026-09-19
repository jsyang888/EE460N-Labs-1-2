/*
    Name 1: Justin S Yang
    Name 2: Manu Vajha
    UTEID 1: jsy558
    UTEID 2: mcv925
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
  int address; /* this is a byte address */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
  int k; 

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word >> 1;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *files[], int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(files[i]);
  }
  CURRENT_LATCHES.Z = 1;  
  NEXT_LATCHES = CURRENT_LATCHES;
    
  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(&argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);
  
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/

void CCsetter(int DR);

void process_instruction(){
  /*  function: process_instruction
   *  
   *    Process one instruction at a time  
   *       -Fetch one instruction
   *       -Decode 
   *       -Execute
   *       -Update NEXT_LATCHES
   */

    int current_instruction = 0;
    current_instruction = Low16bits(MEMORY[CURRENT_LATCHES.PC/2][0] | MEMORY[CURRENT_LATCHES.PC/2][1] << 8);
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2; //increment PC now

    int opcode = (current_instruction & 0xF000) >> 12; // after keeping only instruction bits, shift into 0-3
    if ((opcode == 10) || (opcode == 11)) { //not opcodes in LC3b
        // invalid opcode
        exit(-1); 
    }
    for (int i = 0; i < LC_3b_REGS; i++) {
        NEXT_LATCHES.REGS[i] = CURRENT_LATCHES.REGS[i]; // set it all now, can overwrite later if have to
    }
    NEXT_LATCHES.N = CURRENT_LATCHES.N;
    NEXT_LATCHES.Z = CURRENT_LATCHES.Z;
    NEXT_LATCHES.P = CURRENT_LATCHES.P; // set it all now, overwrite if have to

    //branch instructions
    if (opcode == 0) {

        int CCbits = (current_instruction & 0x0E00) >> 9; //00001110 0000 0000 
        int PCoffset9 = (current_instruction &0x1FF); // last 9 bits 
        if (PCoffset9 & 0x100) {  // convert to negative if bit 8 is 1
            PCoffset9 -= 0x200;  
        }

        int takeBranch = (((CCbits & 4) && CURRENT_LATCHES.N) || ((CCbits & 2) && CURRENT_LATCHES.Z) || ((CCbits & 1) && CURRENT_LATCHES.P));
        if (takeBranch) {
        NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + PCoffset9 * 2);
        }
    }
    //add instructions
    else if (opcode == 1) {
        int DR = (current_instruction & 0x0E00) >> 9;
        int SR1 = (current_instruction & 0x01C0) >> 6;
        int steer = (current_instruction & 0x20);
        if (steer == 0) {
            //means that this takes SR2
            int SR2 = (current_instruction & 0x07);
            NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] + CURRENT_LATCHES.REGS[SR2]);
        }
        else {
            int imm5 = (current_instruction & 0x1F);
            if (imm5 & 0x10) {
              imm5 -= 0x20;
            }
            NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] + imm5);
        }
        CCsetter(DR);
    }
    //and instructions
    else if (opcode == 5) {
        int DR = (current_instruction & 0x0E00) >> 9;
        int SR1 = (current_instruction & 0x01C0) >> 6;
        int steer = (current_instruction & 0x20);
        if (steer == 0) {
            //means that this takes SR2
            int SR2 = (current_instruction & 0x07);
            NEXT_LATCHES.REGS[DR] = CURRENT_LATCHES.REGS[SR1] & CURRENT_LATCHES.REGS[SR2];
        }
        else {
            int imm5 = (current_instruction & 0x1F);
            if (imm5 & 0x10) {
              imm5 -= 0x20;
            }
            NEXT_LATCHES.REGS[DR] = CURRENT_LATCHES.REGS[SR1] & imm5;
        }
        CCsetter(DR);
    }
    //jmp instructions
    else if (opcode == 12) {
        int BaseR = (current_instruction & 0x1C0) >> 6;
        NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[BaseR]; // jump to baseR value 
    }
    //jsr / r instructions 
    // NOT SURE ABOUT THIS ONE CHECK LAB2 DOC
    else if (opcode == 4) {
        if (current_instruction & 0x800) {
            // bit 12 is 1 so JSR mode
            int PCoffset11 = current_instruction & 0x07FF; // mask for 11 bits
            if (PCoffset11 & 0x0400) { // first bit is one, make negative
                PCoffset11 -= 0x800;
            }
            NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC; // save next PC value to R7 of next state
            NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + (PCoffset11 * 2));
        }
        else {
            // jsrr in this case
            int BaseR = (current_instruction & 0x1C0) >> 6;
            NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC; // save next PC value to R7 of next state
            NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[BaseR]; // jump to baseR value 
        }
    }
    // ldb instructions
    else if (opcode == 2) {
        int DR = (current_instruction & 0x0E00) >> 9;
        int BaseR = (current_instruction & 0x01C0) >> 6;
        int boffset6 = (current_instruction & 0x3F);
        if (boffset6 & 0x20) {
            //negative
            boffset6 -= 0x40;
        }
        int address = Low16bits(CURRENT_LATCHES.REGS[BaseR] + boffset6);
        int byte = address & 1;
        int temp = MEMORY[address/2][byte];
        if (temp & 0x80) {
            // bit 7 is high, sign extend
            temp |= 0xFF00;
        } 
        NEXT_LATCHES.REGS[DR] = temp;
        CCsetter(DR);
    }
    // ldw instructions
    else if (opcode == 6) {
        int DR = (current_instruction & 0x0E00) >> 9;
        int BaseR = (current_instruction & 0x01C0) >> 6;
        int boffset6 = (current_instruction & 0x3F);
        if (boffset6 & 0x20) {
            //negative
            boffset6 -= 0x40;
        }
        int address = Low16bits(CURRENT_LATCHES.REGS[BaseR] + 2 * boffset6);
        int temp = (MEMORY[address/2][1] << 8) | (MEMORY[address/2][0]);
        NEXT_LATCHES.REGS[DR] = temp;
        CCsetter(DR);
    }
    // lea instructions DO NOT SET CC
    else if (opcode == 14) {
        int DR = (current_instruction & 0x0E00) >> 9;
        int PCoffset9 = (current_instruction &0x1FF); // last 9 bits 
        if (PCoffset9 & 0x100) {  // convert to negative if bit 8 is 1
            PCoffset9 -= 0x200;  
        }
        NEXT_LATCHES.REGS[DR] = Low16bits(NEXT_LATCHES.PC + (PCoffset9 * 2)); // set DR to address of offset
    }
    // not instructions / xor
    else if (opcode == 9) {
        // NOT part
        if ((current_instruction & 0x3F) == 0x3F) {
            int DR = (current_instruction & 0x0E00) >> 9;
            int SR = (current_instruction & 0x01C0) >> 6;
            NEXT_LATCHES.REGS[DR] = Low16bits(~CURRENT_LATCHES.REGS[SR]); // not the value in the SR into DR
            CCsetter(DR);
        }
        else if ((current_instruction & 0x20) == 0) {
            int DR = (current_instruction & 0x0E00) >> 9;
            int SR1 = (current_instruction & 0x01C0) >> 6;
            int SR2 = (current_instruction & 0x0007);
            NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] ^ CURRENT_LATCHES.REGS[SR2]) ; // XOR the two regs, put into DR
            CCsetter(DR);
        }
        else if (current_instruction & 0x20) {
            // steering bit is 1
            int DR = (current_instruction & 0x0E00) >> 9;
            int SR1 = (current_instruction & 0x01C0) >> 6;
            int imm5 = (current_instruction & 0x001F);
            if (imm5 & 0x10) {
              imm5 -= 0x20;
            }
            NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] ^ imm5) ; // XOR the SR1 with imm5, put in DR
            CCsetter(DR);
        }
        
    }
    // rti SKIP DO NOT NEED TO IMPLEMENT
    // shf instructions
    else if (opcode == 13) {
        int DR = (current_instruction & 0x0E00) >> 9;
        int SR = (current_instruction & 0x01C0) >> 6;
        int steer = (current_instruction & 0x30) >> 4;
        int amt4 = current_instruction & 0xF;
        if (steer == 0) {
            //LSHF
            NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR] << amt4); 
            CCsetter(DR);
        }
        else if (steer == 1) {
            //RSHFL
            NEXT_LATCHES.REGS[DR] = (CURRENT_LATCHES.REGS[SR] & 0xFFFF) >> amt4;
            CCsetter(DR);
        }
        else if (steer == 3 ) {
            //RSHFA
            int temp = CURRENT_LATCHES.REGS[SR] >> amt4;
            // now need to take the SR[15] and sign extend
            if (CURRENT_LATCHES.REGS[SR] & 0x8000) {
                // if bit 15 is 1:
                for (int i = 0; i < amt4; i++) {
                    temp |= 1 << (15-i); // i think this shifts 1s to the front for however many times
                    // it's shifted over
                }
            }
            // if bit 15 is zero, nothing to do
            NEXT_LATCHES.REGS[DR] = temp;
            CCsetter(DR);
        }
    } 
    // stb instructions
    else if (opcode == 3) {
        int SR = (current_instruction & 0x0E00) >> 9;
        int BaseR = (current_instruction & 0x01C0) >> 6;
        int boffset6 = (current_instruction & 0x3F);
        if (boffset6 & 0x20) {
            //negative
            boffset6 -= 0x40;
        }
        int address = Low16bits(CURRENT_LATCHES.REGS[BaseR] + boffset6);
        int byte = address & 1; // computes whether it's in index 0 or 1
        MEMORY[address/2][byte] = CURRENT_LATCHES.REGS[SR] & 0x00FF; // only store the lower 8 bits
    }
    // stw instructions
    else if (opcode == 7) {
        int SR = (current_instruction & 0x0E00) >> 9;
        int BaseR = (current_instruction & 0x01C0) >> 6;
        int boffset6 = (current_instruction & 0x3F);
        if (boffset6 & 0x20) {
            //negative
            boffset6 -= 0x40;
        }
        int address = Low16bits(CURRENT_LATCHES.REGS[BaseR] + 2 * boffset6);
        MEMORY[address/2][0] = CURRENT_LATCHES.REGS[SR] & 0xFF; // only store the lower 8 bits
        MEMORY[address/2][1] = (CURRENT_LATCHES.REGS[SR] >> 8) & 0xFF; // store msb at higher address, little endian
    }
    // trap instructions
    else if (opcode == 15) {
        // set PC to 0
        NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC; // store return address into R7
        NEXT_LATCHES.PC = 0; // set PC = 0
    }
}



// helper function to set CC bits based on value of the DR
void CCsetter(int DR) {
  int temp = NEXT_LATCHES.REGS[DR];
  if (temp & 0x8000) { // negative, so we need to sub it into being negative
    temp -= 0x10000; // make negative
  }
    if (temp > 0) {
        // positive
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 1;
    }
    else if (temp == 0) {
        // zero
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 1;
        NEXT_LATCHES.P = 0;
    }
    else {
        // negative
        NEXT_LATCHES.N = 1;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 0;
    }
}
