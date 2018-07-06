#include "train.h"
using namespace std;

int main(int argc, char const *argv[])
{
	int iteration=atoi(argv[1]);
	const char* model_init_name=argv[2];
	const char* seq_file_name=argv[3];
	const char* model_name=argv[4];

	//Step 0: load initial HMM model and the seqence file
	HMM* model=new HMM;
	loadHMM(model,model_init_name);
	short seq[10000][50];
	read_seq_file(seq,seq_file_name);

	//Train
	for (int i = 0; i < iteration; ++i)
		train_one_iteration(model,seq);

   	FILE *fp = fopen( model_name, "w" );
	dumpHMM(fp,model);
}