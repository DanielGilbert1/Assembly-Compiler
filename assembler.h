#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <string.h>
#include <math.h>
/* ======== Macros ======== */
/* Utilities */
#define FOREVER				for(;;)
#define BYTE_SIZE			8
#define FALSE				0
#define TRUE				1

/* Given Constants */
#define MAX_DATA_NUM		1024
#define FIRST_ADDRESS		100 
#define MAX_LINE_LENGTH		80 /*not including \n*/
#define MAX_LABEL_LENGTH	31
#define MEMORY_WORD_LENGTH	12
#define MAX_REGISTER_DIGIT	7

/* Defining Constants */
#define MAX_LINES_NUM		700
#define MAX_LABELS_NUM		MAX_LINES_NUM 
#define BASE64_NEXT			6
#define SIX_BITS 			0x3f

/* ======== Data Structures ======== */
typedef unsigned int bool; /*TRUE or FALSE*/

/* === First Read  === */

/* Labels Management */
typedef struct
{
	int address;					/* The address it contains */
	char name[MAX_LABEL_LENGTH];	/* The name of the label */
	bool isExtern;					/* External flag */
	bool isData;					/* Flag: .data or .string) */
} labelInfo;

/* Directive And Commands */
typedef struct
{
	char *name;
	void(*parseFunc)();
} directive;

typedef struct
{
	char *name;
	unsigned int opcode : 4;
	int numOfParams;
} command;

/* Operands */
typedef enum { NUMBER = 1, LABEL = 3, REGISTER = 5, INVALID = -1 } opType;

typedef struct
{
	int value;				/* Value */
	char *str;				/* String */
	opType type;			/* Type */
	int address;			/* The address of the operand in the memory */
} operandInfo;

/* Line */
typedef struct
{
	int lineNum;				/* line number in the file */
	int address;				/* The address of the first word in the line */
	char *originalString;		/* The original pointer, allocated by malloc */
	char *lineStr;				/* The text it contains (changed while using parseLine) */
	bool isError;				/* Represent whether there is an error or not */
	labelInfo *label;			/* A poniter to the lines label in labelArr */

	char *commandStr;			/* The string of the command or directive */

	/* Command line */
	const command *cmd;			/* A pointer to the command in g_cmdArr */
	operandInfo op1;			/* The 1st operand */
	operandInfo op2;			/* The 2nd operand */
	bool isJmp;					/*command is a jmp, rts or bne*/
	labelInfo *jumpLabel;			/* A poniter to the lines label in labelArr */
	char jumpLabelName[MAX_LABEL_LENGTH]; /* the name of label in labelArr */
} lineInfo;

/* === Second Read  === */
/*external, relative and absolute numeration*/
typedef enum { ABSOLUTE = 0, EXTERNAL = 1, RELOCATABLE = 2 } eraType; 

/* Memory Word */

typedef struct /* 12 bits */
{
	unsigned int era : 2;

	union /* 10 bits */
	{
		/* Commands (only 10 bits) */
		struct
		{
			unsigned int dest : 3;		/* Destination op addressing method ID */
			unsigned int opcode : 4;	/* Command ID */
			unsigned int src : 3;		/* Source op addressing method ID */
		} cmdBits;

		/* Registers (only 10 bits) */
		struct
		{
			unsigned int destBits : 5;
			unsigned int srcBits : 5;
		} regBits;

		/* Other operands */
		int value : 10; /* (10 bits) */

	} valueBits; /* End of 10 bits union */

} memoryWord;


/* ======== Methods Declaration ======== */
/* utility.c methods */
int getCmdId(char *cmdName);
labelInfo *getLabel(char *labelName);
void trimLeftStr(char **ptStr);
void trimStr(char **ptStr);
char *getFirstTok(char *str, char **endOfTok);
bool isOneWord(char *str);
bool isWhiteSpaces(char *str);
bool isLegalLabel(char *label, int lineNum, bool printErrors);
bool isExistingLabel(char *label);
bool isExistingEntryLabel(char *labelName);
bool isRegister(char *str, int *value);
bool isCommentOrEmpty(lineInfo *line);
char *getFirstOperand(char *line, char **endOfOp, bool *foundComma);
bool isDirective(char *cmd);
bool isLegalStringParam(char **strParam, int lineNum);
bool isLegalNum(char *numStr, int numOfBits, int lineNum, int *value);

/* firstRead.c methods */
int firstFileRead(FILE *file, lineInfo *linesArr, int *linesFound, int *IC, int *DC);

/* secondRead.c methods */
int secondFileRead(int *memoryArr, lineInfo *linesArr, int lineNum, int IC, int DC);

/* main.c methods */
void printError(int lineNum, const char *format, ...);

#endif
