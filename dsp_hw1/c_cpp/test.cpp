#include "hmm.h"
#include <math.h>
using namespace std;

void predictOneSeq(HMM* hmms,short*& seq, FILE*& fp);
void predictWithModel(HMM*& model,short*& seq, FILE*& fp);

int main(int argc, char const *argv[])
{
	//./test modellist.txt testing_data.txt result.txt 
	HMM hmms[5];
	load_models( argv[1], hmms, 5);
	short seq[2500][50];
	const char* seq_file_name=argv[2];
	read_seq_file(seq,seq_file_name);
   	FILE *fp = fopen( argv[3], "w" );
	for (int i = 0; i < 2500; ++i)
		predictOneSeq(hmms,seq[i],fp);
}
