#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <utility>
#include <cstring>
using namespace std;

struct gene {
    string chromosome_name;
    int start_index;
    int end_index;
    string ID;
    char strand;
    
    gene() {}
    gene(string chromosome_name_init, int start_index_init, int end_index_init, string ID_init, char strand_init) {
	chromosome_name = chromosome_name_init;
	start_index = start_index_init;
	end_index = end_index_init;
	ID = ID_init;
	strand = strand_init;
    }
};

//////////////////////////////FILE NAMES////////////////////////////////////

const string exon_file_name = "HG38-RefSeq-exon-annot-chr1";	//file containing exon data
const string chr1_file_name = "HG38-chr1.fa";			//file containing chr1 sequence
const string input_file_name = "input-genes";			//file containing all input gene IDs
const string annot_file_name = "HG38-RefSeq-annot-chr1";	//file containing annotated chr1 data
const string output_file_name = "output.fa";			//file program will output result to

////////////////////////////GLOBAL VARIABLES////////////////////////////////

vector<pair<int,int>> exon_list;				//stores all exon regions by their starting and ending index
string seq;							//stores entire nucleotide sequence
vector<string> input_genes; 					//stores all input gene IDs
vector<gene> output_genes;					//store data of all output genes

////////////////////////////////////////////////////////////////////////////

gene read_gene(string line) {
    string chromosome_name; string gene_ID; string strand; string discard;
    string gene_start_index; string gene_end_index;
    stringstream linestream(line);
    getline(linestream, chromosome_name, '\t');
    string gene_start_index_str;
    string gene_end_index_str;
    linestream >> gene_start_index >> gene_end_index >> gene_ID >> discard >> strand;
    return gene(chromosome_name, stoi(gene_start_index, NULL, 10), stoi(gene_end_index, NULL, 10), gene_ID, strand[0]);
}

void read_into_string() {
    ifstream read_file;
    read_file.open(chr1_file_name);
    if (!read_file.is_open()) {
	printf("\n\tCHROMOSOME SEQUENCE FILE \"%s\" NOT FOUND\n", chr1_file_name);
	exit(-1);
    }
    const int MAX_line_length = 51;
    char* line = new char[51];
    read_file.getline(line, MAX_line_length);  //discard first line
    while (read_file.getline(line, MAX_line_length) && strlen(line) > 0) {
        seq += line;
    }
    read_file.close();
}

pair<int,int> read_bounds(string s) {
    stringstream linestream(s);
    string discard;
    string start; string end;
    getline(linestream, discard, '\t');
    linestream >> start >> end;
    return pair(stoi(start, NULL, 10), stoi(end, NULL, 10));
}

void get_exon_indexes() {
    ifstream read_file;
    read_file.open(exon_file_name);
    if (!read_file.is_open()) {
	printf("\n\tEXON FILE \"%s\" NOT FOUND\n", exon_file_name);
	exit(-1);
    }
    string line;
    while (getline(read_file, line)) {
	exon_list.push_back(read_bounds(line));
    }
    read_file.close();
}

void mask_introns() {
    sort(exon_list.begin(), exon_list.end());
    int seq_index = 0;
    for (int i = 0; i < exon_list.size(); i++) {
	if (exon_list[i].first < seq_index) seq_index = exon_list[i].first;	
	fill (seq.begin() + seq_index, seq.begin() + exon_list[i].first, '=');    
	seq_index = exon_list[i].second;
    }
    fill (seq.begin() + seq_index, seq.end(), '=');
}

void read_input_genes() {
    string line;
    ifstream read_file;
    read_file.open(input_file_name);
    if (!read_file.is_open()) {
	printf("\n\t INPUT GENE FILE \"%s\" NOT FOUND\n", input_file_name);
	exit(-1);
    }

    while (getline(read_file, line)) {
	input_genes.push_back(line);
    }
}

void read_annot() {
    ifstream read_file;
    read_file.open(annot_file_name);
    if (!read_file.is_open()) {
	printf("\n\tANNOTATED GENE FILE \"%s\" NOT FOUND\n", annot_file_name);
	exit(-1);
    }
    string line;
    while (getline(read_file, line)) {
	gene curr = read_gene(line);
	for (int i = 0; i < input_genes.size(); i++) {
	    if (input_genes[i] == curr.ID) {
		output_genes.push_back(curr);
	    }
	}
    }
    read_file.close();
}

char complement(char base) {
    switch (base) { 
	case 'A':
	      return 'T';
	case 'T':
	      return 'A';
	case 'C':
	      return 'G';
	case 'G':
	      return 'C';
	case 'a':
	      return 't';
	case 't':
	      return 'a';
	case 'g':
	      return 'c';
	case 'c':
	      return 'g';
    }
    printf("\tINVALID BASE: \'%c\' IN INPUT FILE\n", base);
    exit (-1);
    return ' ';
}

void reverse_complement(string &s) {
    reverse(s.begin(), s.end());
    for (int i = 0; i < s.size(); i++) {
	while (s[i] == '=' && i < s.size() - 1) i++;
        s[i] = complement(s[i]);
    }
}

void output_to_file() {
    ofstream output_file;
    output_file.open(output_file_name);
    for (int i = 0; i < output_genes.size(); i++) {
	string output_seq;
	int current_index;
	output_file << '>' << output_genes[i].chromosome_name << ',' << output_genes[i].start_index << ',';
        output_file << output_genes[i].end_index << ',' << output_genes[i].ID << ',' << output_genes[i].strand << '\n';
	output_seq = string(seq, output_genes[i].start_index, output_genes[i].end_index - output_genes[i].start_index);
	if (output_genes[i].strand == '-') reverse_complement(output_seq);
	output_file << output_seq << '\n';
    }
    output_file.close();
}

