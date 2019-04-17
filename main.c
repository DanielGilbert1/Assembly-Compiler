/* ======== Includes ======== */
#include "assembler.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/* ====== Global Data Structures ====== */
/* Labels */
labelInfo g_labelArr[MAX_LABELS_NUM];
int g_labelNum = 0;
/* Entry Lines */
lineInfo *g_entryLines[MAX_LABELS_NUM]; /**/
int g_entryLabelsNum = 0;
/* Data */
int g_dataArr[MAX_DATA_NUM];
static char encoding_table[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
								'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
								'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
								'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
								'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
								'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
								'w', 'x', 'y', 'z', '0', '1', '2', '3',
								'4', '5', '6', '7', '8', '9', '+', '/' };
/* ====== Methods ====== */

/* Print an error with the line number. */
void printError(int lineNum, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	printf("[Error] At line %d: ", lineNum);
	vprintf(format, args);
	printf("\n");
	va_end(args);
}

/* Prints a number in base64. */
void fprintfBase64(FILE *file, int num)
{
	int part1 = num & SIX_BITS; /*taking the 6 less significant bits*/
	int part2 = num >> BASE64_NEXT;
	size_t table_size = sizeof(encoding_table) / sizeof(encoding_table[0]);
	if (table_size <= part1) {
	    return;
	}
	if (table_size <= part2) {
	    return;
	}
    fprintf(file, "%c%c", encoding_table[part2], encoding_table[part1]);
}

/* Creates a file (for writing) from a given name and ending, and returns a pointer to it. */
FILE *openFile(char *name, char *ending, const char *mode)
{
	FILE *file;
	char *mallocStr = (char *)malloc(strlen(name) + strlen(ending) + 1), *fileName = mallocStr;
	sprintf(fileName, "%s%s", name, ending);

	file = fopen(fileName, mode);
	free(mallocStr);

	return file;
}

/* Creates the .obj file, which contains the assembled lines in base 2. */
void createObjectFile(char *name, int IC, int DC, int *memoryArr)
{
	int i;
	FILE *file;
	file = openFile(name, ".ob", "wb");

	/* Print IC and DC */
	fprintf(file, "%d", IC);
	fprintf(file, "  ");
	fprintf(file, "%d", DC);

	/* Print all of memoryArr */
	for (i = 0; i < IC + DC; i++)
	{
		fprintf(file, "\n");
		fprintfBase64(file, memoryArr[i]);
	}

	fclose(file);
}

/* Creates the .ent file, which contains the addresses for the .entry labels. */
void createEntriesFile(char *name)
{
	int i;
	FILE *file;

	/* Don't create the entries file if there aren't entry lines */
	if (!g_entryLabelsNum)
	{
		return;
	}

	file = openFile(name, ".ent", "w");

	for (i = 0; i < g_entryLabelsNum; i++)
	{
		fprintf(file, "%s\t\t", g_entryLines[i]->lineStr);
		fprintf(file, "%d", getLabel(g_entryLines[i]->lineStr)->address);

		if (i != g_entryLabelsNum - 1)
		{
			fprintf(file, "\n");
		}
	}

	fclose(file);
}

/* Creates the .ext file, which contains the addresses for the extern labels operands. */
void createExternFile(char *name, lineInfo *linesArr, int linesFound)
{
	int i;
	labelInfo *label;
	bool firstPrint = TRUE; /* This bool meant to prevent the creation of the file if there aren't any externs */
	FILE *file = NULL;

	for (i = 0; i < linesFound; i++)
	{
		/* JumpParam */
		if (linesArr[i].isJmp == TRUE)
		{
			label = linesArr[i].jumpLabel;
			if (label && label->isExtern)
			{
				if (firstPrint)
				{
					/* Create the file only if there is at least 1 extern */
					file = openFile(name, ".ext", "w");
				}
				else
				{
					fprintf(file, "\n");
				}

				fprintf(file, "%s\t\t", label->name);
				fprintf(file, "%d", linesArr[i].address + 1);
				firstPrint = FALSE;
			}
		}
		/* Check if the 1st operand is extern label, and print it. */
		if (linesArr[i].cmd && linesArr[i].cmd->numOfParams >= 2 && linesArr[i].op1.type == LABEL)
		{
			label = getLabel(linesArr[i].op1.str);
			if (label && label->isExtern)
			{
				if (firstPrint)
				{
					/* Create the file only if there is at least 1 extern */
					file = openFile(name, ".ext", "w");
				}
				else
				{
					fprintf(file, "\n");
				}

				fprintf(file, "%s\t\t", label->name);
				fprintf(file, "%d", linesArr[i].op1.address);
				firstPrint = FALSE;
			}
		}

		/* Check if the 2nd operand is extern label, and print it. */
		if (linesArr[i].cmd && linesArr[i].cmd->numOfParams >= 1 && linesArr[i].op2.type == LABEL)
		{
			label = getLabel(linesArr[i].op2.str);
			if (label && label->isExtern)
			{
				if (firstPrint)
				{
					/* Create the file only if there is at least 1 extern */
					file = openFile(name, ".ext", "w");
				}
				else
				{
					fprintf(file, "\n");
				}

				fprintf(file, "%s\t\t", label->name);
				fprintf(file, "%d", linesArr[i].op2.address);
				firstPrint = FALSE;
			}
		}
	}

	if (file)
	{
		fclose(file);
	}
}

/* Resets all the globals and free all the malloc blocks. */
void clearData(lineInfo *linesArr, int linesFound, int dataCount)
{
	int i;

	/* --- Reset Globals --- */

	/* Reset global labels */
	for (i = 0; i < g_labelNum; i++)
	{
		g_labelArr[i].address = 0;
		g_labelArr[i].isData = 0;
		g_labelArr[i].isExtern = 0;
	}
	g_labelNum = 0;

	/* Reset global entry lines */
	for (i = 0; i < g_entryLabelsNum; i++)
	{
		g_entryLines[i] = NULL;
	}
	g_entryLabelsNum = 0;

	/* Reset global data */
	for (i = 0; i < dataCount; i++)
	{
		g_dataArr[i] = 0;
	}

	/* Free malloc blocks */
	for (i = 0; i < linesFound; i++)
	{
		free(linesArr[i].originalString);
	}
}

/* Parsing a file, and creating the output files. */
void parseFile(char *fileName)
{
	FILE *file = openFile(fileName, ".as", "r");
	lineInfo linesArr[MAX_LINES_NUM];
	int memoryArr[MAX_DATA_NUM] = { 0 }, IC = 0, DC = 0, numOfErrors = 0, linesFound = 0;

	/* Open File */
	if (file == NULL)
	{
		printf("[Info] Can't open the file \"%s.as\".\n", fileName);
		return;
	}
	printf("[Info] Successfully opened the file \"%s.as\".\n", fileName);

	/* First Read */
	numOfErrors += firstFileRead(file, linesArr, &linesFound, &IC, &DC);
	/* Second Read */
	numOfErrors += secondFileRead(memoryArr, linesArr, linesFound, IC, DC);

	/* Create Output Files */
	if (numOfErrors == 0)
	{
		/* Create all the output files */
		createObjectFile(fileName, IC, DC, memoryArr);
		createExternFile(fileName, linesArr, linesFound);
		createEntriesFile(fileName);
		printf("[Info] Created output files for the file \"%s.as\".\n", fileName);
	}
	else
	{
		/* print the number of errors. */
		printf("[Info] A total of %d error%s found throughout \"%s.as\".\n", numOfErrors, (numOfErrors > 1) ? "s were" : " was", fileName);
	}

	/* Free all malloc pointers, and reset the globals. */
	clearData(linesArr, linesFound, IC + DC);

	/* Close File */
	fclose(file);
}

/* Main method. Calls the "parsefile" method for each file name in argv. */
int main(int argc, char *argv[])
{
	int i;

	if (argc < 2)
	{
		printf("[Info] no file names were observed.\n");
		return 1;
	}

	/* initialize random seed for later use */
	srand((unsigned)time(NULL));

	for (i = 1; i < argc; i++)
	{
		parseFile(argv[i]);
		printf("\n");
	}

	return 0;
}
