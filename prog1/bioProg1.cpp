#include "prog1.h"
using namespace std;

int main() {
    
    printf("getting exon regions...");
    get_exon_indexes();
    printf("\t\tSUCCESS\n");

    printf("copying chr1...");
    read_into_string();
    printf("\t\t\tSUCCESS\n");

    printf("masking intron regions...");
    mask_introns();
    printf("\tSUCCESS\n");

    printf("reading input file...");
    read_input_genes();
    printf("\t\tSUCCESS\n");

    printf("reading annotated chr1...");
    read_annot();
    printf("\tSUCCESS\n");

    printf("outputting results to file...");
    output_to_file();
    printf("\tSUCCESS\n");

}
