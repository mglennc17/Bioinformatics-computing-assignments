#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
using namespace std;
using namespace chrono;

string readFileName = "readFile.txt";			// read and write file names can be changed here
string writeFileName = "writeFile.txt";

int getMax(vector<int> L) {
    int M = 0;
    for (int i = 0; i < L.size(); i++) if (L[i] > M) M = L[i];
    return M;
}

bool valX(vector<int> &L, vector<int> X, int Xi) {	// check is given Xi is valid for PDP
    for (int i = 0; i < X.size(); i++) {
	int dist = X[i] - Xi;				
	if (dist < 0) dist *= -1;
	bool contains = false;
	for (int j = 0; j < L.size(); j++) {
	    if (L[j] == dist) { 			// check if dists between Xi and X values are in L
		L.erase(L.begin() + j);
	        contains = true;
		break;
	    }
	}
	if (contains == false) return false;		// if not, Xi is not valid
    }
    return true;
}

vector<vector<int>> XList;
int M;

void PDP(vector<int> L, vector<int> X) {
    if (L.size() == 0) {				// If L is empty, PDP is complete
	sort(X.begin(), X.end());
	for (int i = 0; i < XList.size(); i++) {	// check if X is a duplicate
	    if (XList[i] == X) return;
	}
	XList.push_back(X);				// Add to list of possible Xs
	return;
    }
    int y = getMax(L);
    vector<int> LCopy = L;

    if (valX(LCopy, X, M - y)) {	
	X.push_back(M - y);
	PDP(LCopy,X);			// call recursively for M - y case
	X.pop_back();			// Fix X for 0 + y case
    }

    if (valX(L, X, y)) {	    	
	X.push_back(y);
	PDP(L,X);			// call recursively for 0 + y case
    }

}

vector<int> L;

void readL() {				// read L values from file, converting to ints
    ifstream readFile;
    readFile.open(readFileName);
    if (!readFile.is_open()) {
	cout << "file not found\n";
	exit(0);
    }
    string line;
    string num;
    int numInt;
    while (getline(readFile, line)) {
	for (auto i : line) {
	    if (isdigit(i)) num += i;
	    else if (num.size() >= 1) {
	        numInt = stoi(num, NULL, 10);
	        L.push_back(numInt);
	        num = "";
	    }
	}
    }
}

void fileOutput() {			// output all values to X to file
    ofstream writeFile;
    writeFile.open(writeFileName);
    if (!writeFile.is_open()) {
	cout << "failed to open file for writing\n";
	exit(0);
    }
    
    writeFile << "possible values for X:\n\n";

    for (int i = 0; i < XList.size(); i++) {
	writeFile << "\tX = { ";
	for (int j = 0; j < XList[i].size() - 1; j++) {
	    writeFile << XList[i][j] << ", ";
	}
	writeFile << XList[i][XList[i].size() - 1] << " }\n";
    }
}

int main() {  
    auto t1 = high_resolution_clock::now();	
    readL();	
    M = getMax(L);				// init: get M and remove it from L
    for (int i = 0; i < L.size(); i++) {
	if (L[i] == M) {
	    L.erase(L.begin() + i);
	    break;
	}
    }
    PDP(L, {0, M});				// init: call PDP with X set to {0, M}

    if (XList.size() < 1) {
	cout << "No solution found\n";
	return 0;
    }

    fileOutput();
    auto t2 = high_resolution_clock::now();
    duration<double, milli> ms_double = t2 - t1;
    cout << "prog time:\t" << ms_double.count() << "ms" << endl;
}
