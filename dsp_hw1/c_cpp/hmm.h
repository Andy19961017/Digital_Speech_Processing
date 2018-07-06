#ifndef HMM_HEADER_
#define HMM_HEADER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>
using namespace std;

#ifndef MAX_STATE
#	define MAX_STATE	10
#endif

#ifndef MAX_OBSERV
#	define MAX_OBSERV	26
#endif

#ifndef MAX_SEQ
#	define	MAX_SEQ		200
#endif

#ifndef MAX_LINE
#	define MAX_LINE 	256
#endif

typedef struct{
   char *model_name;
   int state_num;					//number of state
   int observ_num;					//number of observation
   double initial[MAX_STATE];			//initial prob.   pi
   double transition[MAX_STATE][MAX_STATE];	//transition prob.   a
   double observation[MAX_OBSERV][MAX_STATE];	//observation prob.    b
} HMM;

static FILE *open_or_die( const char *filename, const char *ht )
{
   FILE *fp = fopen( filename, ht );
   if( fp == NULL ){
      perror( filename);
      exit(1);
   }

   return fp;
}

static void loadHMM( HMM *hmm, const char *filename )
{
   int i, j;
   FILE *fp = open_or_die( filename, "r");

   hmm->model_name = (char *)malloc( sizeof(char) * (strlen( filename)+1));
   strcpy( hmm->model_name, filename );

   char token[MAX_LINE] = "";
   while( fscanf( fp, "%s", token ) > 0 )
   {
      if( token[0] == '\0' || token[0] == '\n' ) continue;

      if( strcmp( token, "initial:" ) == 0 ){
         fscanf(fp, "%d", &hmm->state_num );

         for( i = 0 ; i < hmm->state_num ; i++ )
            fscanf(fp, "%lf", &( hmm->initial[i] ) );
      }
      else if( strcmp( token, "transition:" ) == 0 ){
         fscanf(fp, "%d", &hmm->state_num );

         for( i = 0 ; i < hmm->state_num ; i++ )
            for( j = 0 ; j < hmm->state_num ; j++ )
               fscanf(fp, "%lf", &( hmm->transition[i][j] ));
      }
      else if( strcmp( token, "observation:" ) == 0 ){
         fscanf(fp, "%d", &hmm->observ_num );

         for( i = 0 ; i < hmm->observ_num ; i++ )
            for( j = 0 ; j < hmm->state_num ; j++ )
               fscanf(fp, "%lf", &( hmm->observation[i][j]) );
      }
   }
   fclose (fp);
}

static void dumpHMM( FILE *fp, HMM *hmm )
{
   int i, j;

   //fprintf( fp, "model name: %s\n", hmm->model_name );
   fprintf( fp, "initial: %d\n", hmm->state_num );
   for( i = 0 ; i < hmm->state_num - 1; i++ )
      fprintf( fp, "%.5lf ", hmm->initial[i]);
   fprintf(fp, "%.5lf\n", hmm->initial[ hmm->state_num - 1 ] );

   fprintf( fp, "\ntransition: %d\n", hmm->state_num );
   for( i = 0 ; i < hmm->state_num ; i++ ){
      for( j = 0 ; j < hmm->state_num - 1 ; j++ )
         fprintf( fp, "%.5lf ", hmm->transition[i][j] );
      fprintf(fp,"%.5lf\n", hmm->transition[i][hmm->state_num - 1]);
   }

   fprintf( fp, "\nobservation: %d\n", hmm->observ_num );
   for( i = 0 ; i < hmm->observ_num ; i++ ){
      for( j = 0 ; j < hmm->state_num - 1 ; j++ )
         fprintf( fp, "%.5lf ", hmm->observation[i][j] );
      fprintf(fp,"%.5lf\n", hmm->observation[i][hmm->state_num - 1]);
   }
}

static int load_models( const char *listname, HMM *hmm, const int max_num )
{
   FILE *fp = open_or_die( listname, "r" );

   int count = 0;
   char filename[MAX_LINE] = "";
   while( fscanf(fp, "%s", filename) == 1 ){
      loadHMM( &hmm[count], filename );
      count ++;

      if( count >= max_num ){
         return count;
      }
   }
   fclose(fp);

   return count;
}

static void dump_models( HMM *hmm, const int num )
{
   int i = 0;
   for( ; i < num ; i++ ){ 
      //		FILE *fp = open_or_die( hmm[i].model_name, "w" );
      dumpHMM( stderr, &hmm[i] );
   }
}

void read_seq_file(short seq[][50], const char *filename )
{
   fstream f;
   f.open(filename, ios::in);
   if(!f)
      cout<<"cannot open: "<<filename<<endl;
   else{
      short i=0;
      while (true){
         string s;
         getline(f,s);
         if (s.length()==0) return;
         for (unsigned j = 0; j < 50; ++j){
            if (s[j]=='A') seq[i][j]=0;
            else if (s[j]=='B') seq[i][j]=1;
            else if (s[j]=='C') seq[i][j]=2;
            else if (s[j]=='D') seq[i][j]=3;
            else if (s[j]=='E') seq[i][j]=4;
            else if (s[j]=='F') seq[i][j]=5;
         }
         i++;
      }
   }
}

void claculateModelProb(const HMM* const & model, double (&delta)[50][6], double & P, const short* const & seq)
{
   for (int i = 0; i < 6; ++i)
      delta[0][i]=model->initial[i]*model->observation[seq[0]][i];
   for (int t = 1; t < 50; ++t)
      for (int j = 0; j < 6; ++j){
         double x[6];
         for (int i = 0; i < 6; ++i)
            x[i]=delta[t-1][i]*model->transition[i][j];
         for (int i = 1; i < 6; ++i)
            if (x[i]>x[0]) x[0]=x[i];
         delta[t][j]=x[0]*model->observation[seq[t]][j];
      }
   for (int i = 1; i < 6; ++i)
      if (delta[49][i]>delta[49][0]) delta[49][0]=delta[49][i];
   P=delta[49][0];
}

void predictOneSeq(const HMM* const & hmms, const short* const & seq, FILE* const fp)
{
   double P[5];
   double delta[5][50][6];
   for (int i = 0; i < 5; ++i)
      claculateModelProb((hmms+i), delta[i], P[i], seq);
   short maxProbState=0;
   double maxProb=P[0];
   for (int i = 1; i < 5; ++i){
      if (P[i]>maxProb){
         maxProb=P[i];
         maxProbState=i;
      }
   }
   fprintf( fp, "model_0%i.txt %e\n", maxProbState+1, maxProb);
}

void observeModel(HMM* model)
{
   double sum[6];
   for (int i = 0; i < 6; ++i)
      sum[i]=0;
   cout << "Update model to:" << endl;
   cout << "initial:" << endl;
   for (int i = 0; i < 6; ++i){
      sum[0]+=model->initial[i];
      cout << model->initial[i] << ' ';
   }
   cout << "sum: " << sum[0];
   sum[0]=0;
   cout << "\n\ntransition:" << endl;
   for (int i = 0; i < 6; ++i){
      for (int j = 0; j < 6; ++j){
         sum[i]+=model->transition[i][j];
         cout << model->transition[i][j] << ' ';
      }
      cout << "sum: " << sum[i] << endl;
   }
   for (int i = 0; i < 6; ++i)
      sum[i]=0;
   cout << "\nobservation:" << endl;
   for (int i = 0; i < 6; ++i){
      for (int j = 0; j < 6; ++j){
         sum[j]+=model->observation[i][j];
         cout << model->observation[i][j] << ' ';
      }
      cout << endl;
   }
   for (int i = 0; i < 6; ++i)
      cout << sum[i] << ' ';
   cout << "(sum)" << endl << endl;
}

#endif
