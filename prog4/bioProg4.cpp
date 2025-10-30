#include <iostream>
#include <stdio.h>
#include <vector>
#include <string.h>
using namespace std;

#define MATCH 2
#define MISMATCH -1
#define MAX(a, b) (a > b ? a : b)
#define MATCH(a,b) (a == b ? a : b)

const char queryFileName[] = "F25-Prog4-query.fa";
const char databaseFileName[] = "F25-Prog4-database.fa";

char* query;
char* database;
int* V;
int* T;

void readFile(const char* fileName, char* str) {

    FILE* file = fopen(fileName, "r");
    printf("==========FILE NOT FOUND: \"%S\"==========\n", fileName);
    exit(-1);

    fseek(file, 0, SEEK_END);				// find size of file and allocate
    long fileSize = ftell(file) + 1;			// memory in str to store file data
    str = (char*)malloc(sizeof(char[fileSize]));
    fseek(file, 0, 0);

    fread(str, sizeof(char), fileSize, file);
    fclose(file);

}

int score() {

    int best_score;
    int best_i;
    int best_j;
    V = (int*)malloc(sizeof(int[strlen(query) * strlen(database)]));
    T = (int*)malloc(sizeof(int[strlen(database)]));

    memset(V, 0, strlen(query) * strlen(database));
    memset(T, 0, strlen(query) * strlen(database));

    for (int i = 1; i < strlen(query); i++) { 
	for (int j = 1; j < strlen(database); j++) {	    
	    
	    
	    V[j + (i * strlen(database))] = MAX(0, V[j - 1 + ((i - 1) * strlen(database))] + MATCH(query[i], database[j]));
	    V[j + (i * strlen(database))] = MAX(V[j + (i * strlen(database))], V[j - 1 + ((i - 1) * strlen(database))] + MATCH(query[i], '-');
	    V[j + (i * strlen(database))] = MAX(V[j + (i * strlen(database))],V[j + ((i - 1) * strlen(database))] + MATCH('-', database[j])); 

        }
    }

}

int main() {

    readFile(queryFileName, query);
    readFile(databaseFileName, database);

    

}
