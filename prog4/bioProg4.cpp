#include <iostream>
#include <stdio.h>
#include <vector>
#include <string.h>
using namespace std;

const int gapPenalty = -1;	// score penalty for gaps
const int ifMatch = 1;		// score added for a match
const int ifMismatch = -2;	// score penalty for mismatch

#define MAX(a, b) (a > b ? a : b)
#define MATCH(a,b) (a == b ? ifMatch : ifMismatch)

const char queryFileName[] = "F25-Prog4-query.fa";		// name of file storing the query sequence
const char databaseFileName[] = "F25-Prog4-database.fa";	// name of file storing the database sequences
const char outputFileName[] = "F25-Prog4-output.fa";		// name of file the program will ouput results to
const int buffSize = 150;					// buffsize must be large enough to hold sequence data -- lines starting with '>'

char* query;		// stores data about the query sequence -- lines starting with '>'
char* querySeq;		// stores the query sequence itself
char** database;	// stores data about the database sequences -- lines starting with '>'
char** databaseSeq;	// stores the database sequences themselves

int maxSeq;		// longest sequence length in database file, calculated in allocDB
int maxDB;		// longest seqence data in database file -- lines starting with '>', calculated in allocDB
int dbSize;		// number of sequences in the database, calculated in allocDB

void readQuery(const char *fileName) {						// reads the query sequence and sequence data into strings

    FILE *file = fopen(fileName, "r");
    long fileSize; char buf[buffSize + 1];

    if (file == NULL) {
	printf("==========FILE NOT FOUND: \"%S\"==========\n", fileName);
        exit(-1);
    }

    fseek(file,0,SEEK_END);							// seek to the end of the file and use ftell() to get the file size
    fileSize = ftell(file);
    fseek(file,0,0);

    fgets(buf, buffSize, file);							// read the sequence data into "query"
    query = new char[strlen(buf) + 1];
    query[strlen(buf) + 1] = {0};
    strcpy(query, buf);

    querySeq = new char[fileSize - strlen(query) + 1];
    querySeq[fileSize - strlen(query) + 1] = {0};				// sequence length = total file size - sequence data length
    fread(querySeq, fileSize - strlen(query), fileSize - strlen(query), file);	// read the rest of the file into "querySeq"

    fclose(file);

}

void allocDB(const char *fileName) {						// allocate memory for storing database info
    
    char buf[buffSize + 1];	
    dbSize = 0; maxDB = 0; maxSeq = 0;
    int seqIDX = 0;
    int currSeq = 0; int currDB = 0;
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
	printf("==========FILE NOT FOUND: \"%S\"==========\n", fileName);
        exit(-1);
    }
    
    while (fgets(buf, buffSize, file)){						// calculate the max size of sequence, sequence data, and the size of the database
	if (buf[0] == '>') {							
	    currDB += strlen(buf);						
	    maxDB = MAX(maxDB, currDB);
	    currSeq = 0;
	    dbSize++;
	}
	else {
	    currSeq += strlen(buf);
	    maxSeq = MAX(maxSeq, currSeq);
	    currDB = 0;
	}
    }

    database = new char*[dbSize];						// allocate enough strings to store all database sequences
    databaseSeq = new char*[dbSize];

}

void readDatabase(const char *fileName) {					// read database data into allocated database and databaseSeq arrays

    FILE *file = fopen(fileName, "r");
    char buf[buffSize + 1];
    int dbIDX = 0; int seqIDX = 0;

    if (file == NULL) {
	printf("==========FILE NOT FOUND: \"%S\"==========\n", fileName);
        exit(-1);
    }

    while (fgets(buf, buffSize, file)) {
	if (buf[0] == '>') {							// if the line begins with '>', read into "database" and increment database indexes
	    database[dbIDX] = new char[maxDB + 1];
	    database[dbIDX][maxDB + 1] = {0};
	    strcpy(database[dbIDX], buf);
	    seqIDX = dbIDX++;
	}	
	else {									// otherwise read into current seqIDX of "databaseSeq" until line begins with '>'
	    if (databaseSeq[seqIDX] == NULL) databaseSeq[seqIDX] = new char[maxSeq + 1];
	    databaseSeq[seqIDX][maxSeq + 1] = {0};
	    strcat(databaseSeq[seqIDX], buf);
	}
    }

    fclose(file);
    seqIDX++;

}

void printDatabase() {								// print database sequences and corresponding sequence data

    int dbIDX = 0;
    for (int i = 0; i < dbSize; i++) printf("%s\n%s\n", database[i],databaseSeq[i]);

}

void reverseStr(char *str) {

    int l = 0; int r = strlen(str) - 1;
    char temp;
    while (l < r) {
        temp = str[l];
	str[l] = str[r];
	str[r] = temp;

	l++; r--;
    }

}

FILE *file = fopen(outputFileName, "w");

void SmithWaterman(char* seq, char* DB, char* seqDB) {				// main Smith-Waterman function which calculates optimal alignment and outputs to file

    char *outSQ;
    char *outDB;
    char *cmp;

    int currDir;
    int prev;
    int strSize;

    int best_score = 0;
    int best_i = 0;
    int best_j = 0;

    int sqLen = strlen(seq);
    int dbLen = strlen(seqDB);

    int V[sqLen + 1][dbLen + 1];
    int T[sqLen + 1][dbLen + 1];
    for (int i = 0; i < sqLen + 1; i++) {  								// initially all values in T and V are 0
	fill(V[i], V[i] + dbLen + 1, 0); 
	fill(T[i], T[i] + dbLen + 1, 0); 
    }

    for (int i = 1; i < sqLen; i++) {									// at each [i][j] find the direction with the best score
	for (int j = 1; j < dbLen; j++) {
	    T[i][j] = 1;
	    prev = V[i][j] = MAX(0, V[i - 1][j - 1] + MATCH(seq[i], seqDB[j]));
	    if (prev != (V[i][j] = MAX(V[i][j], V[i - 1][j] + gapPenalty))) { prev = V[i][j]; T[i][j] = 2; }
	    if (prev != (V[i][j] = MAX(V[i][j], V[i][j-1] + gapPenalty))) { T[i][j] = 3; }
	    if (best_score < V[i][j]) {
		best_score = V[i][j];									// if the current score is higher than best_score
		best_i = i;										// update best_score, best_i, and best_j
		best_j = j;
	    }
	}
    }
    
    strSize = best_i + best_j;										// allocate the worst case size for output strings

    outSQ = new char[strSize + 1];
    outDB = new char[strSize + 1];
    cmp = new char[strSize + 1];
 
    int sqIDX = best_i; int dbIDX = best_j; int allIDX = 0;

    while (T[sqIDX][dbIDX] > 0) {									// traceback using by following directions stored in table 'T'
	int curDir = T[sqIDX][dbIDX];
	if (curDir == 1) { outSQ[allIDX] = seq[sqIDX]; outDB[allIDX] = seqDB[dbIDX]; sqIDX--; dbIDX--; }
	else if (curDir == 2) { outSQ[allIDX] = seq[sqIDX]; outDB[allIDX] = '-'; sqIDX--; }
	else if (curDir == 3) { outSQ[allIDX] = '-'; outDB[allIDX] = seqDB[dbIDX]; dbIDX--; }
	allIDX++;
    }

    
    outSQ[allIDX] = '\0';
    outDB[allIDX] = '\0';
    cmp[allIDX] = '\0';

    reverseStr(outSQ);											// traceback gets strings in reverse order, so they must be reversed to get the original strings
    reverseStr(outDB);

    for (int i = 0; i < strlen(outSQ); i++) if (outSQ[i] == outDB[i]) cmp[i] = '|'; else cmp[i] = ' ';	// detect matches for the comparison string
    
    fprintf(file,"%s",DB);										// output results to file
    fprintf(file,"Optimum Smith-Waterman score = %d\n", best_score);
    fprintf(file,"Query\t: best alignment start_index = %d, end_index = %d\n", sqIDX, best_i);
    fprintf(file,"DB seq\t: best alignment start_index = %d, end_index = %d\n", dbIDX, best_j);
    fprintf(file,"\tq_string\t: \t%s\n", outSQ);
    fprintf(file,"\t\t\t\t%s\n", cmp);
    fprintf(file,"\tdb_string\t:\t%s\n\n\n", outDB);
    
    //printf("\n%s",DB);
    //printf("Optimum Smith-Waterman score = %d\n", best_score);
    //printf("Query\t: best alignment start_index = %d, end_index = %d\n", sqIDX, best_i);
    //printf("DB seq\t: best alignment start_index = %d, end_index = %d\n", dbIDX, best_j);
    //printf("\tq_string\t: \t%s\n", outSQ);
    //printf("\t\t\t\t%s\n", cmp);
    //printf("\tdb_string\t:\t%s\n\n", outDB);
    
    delete[] outSQ;
    delete[] outDB;
    delete[] cmp;

}

int main() {

    readQuery(queryFileName);
    allocDB(databaseFileName);
    readDatabase(databaseFileName);
    for(int i = 0; i < dbSize; i++) SmithWaterman(querySeq, database[i], databaseSeq[i]);
 
    delete[] database;
    delete[] databaseSeq;
    delete[] query;
    delete[] querySeq;

    return 0;

}
