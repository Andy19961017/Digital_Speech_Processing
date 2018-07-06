#include "hmm.h"

void train_one_iteration(HMM* model, short seq[][50])
{
	//initial
	double pi[6];
	double a_n[6][6], a_d[6][6];
	double b_n[6][6], b_d[6][6];
	for (int i = 0; i < 6; ++i){
		pi[i]=0;
		for (int j = 0; j < 6; ++j){
			a_n[i][j]=a_d[i][j]=b_n[i][j]=b_d[i][j]=0;
		}
	}

	//for each data
	for (int d = 0; d < 10000; ++d){

		//Step 1: Calculate alpha(50*6)
		double alpha[50][6];
		for (int i = 0; i < 6; ++i)
			alpha[0][i]=model->initial[i]*model->observation[seq[d][0]][i];
		for (int t = 0; t < 49; ++t){
			for (int j = 0; j < 6; ++j){
				double sum=0;
				for (int i = 0; i < 6; ++i)
					sum+=alpha[t][i]*model->transition[i][j];
				alpha[t+1][j]=sum*model->observation[seq[d][t+1]][j];
			}
		}

		//Step 2: Calculate beta(50*6)
		double beta[50][6];
		for (int i = 0; i < 6; ++i)
			beta[49][i]=1;
		for (int t = 48; t >= 0; --t){
			for (int i = 0; i < 6; ++i){
				beta[t][i]=0;
				for (int j = 0; j < 6; ++j)
					beta[t][i]+=model->transition[i][j]*
					model->observation[seq[d][t+1]][j]*beta[t+1][j];
			}
		}

		//Step 3: Calculate gama(50*6)
		double gama[50][6];
		double* sum1=new double[50];
		for (int t = 0; t < 50; ++t){
			sum1[t]=0;
			for (int i = 0; i < 6; ++i){
				sum1[t]+=alpha[t][i]*beta[t][i];
			}
		}
		for (int t = 0; t < 50; ++t)
			for (int i = 0; i < 6; ++i)
				gama[t][i]=alpha[t][i]*beta[t][i]/sum1[t];

		//Step 4: Calculate eps(49*6*6)
		double eps[49][6][6];
		double sum2[49];
		for (int t = 0; t < 49; ++t)
			sum2[t]=0;
		for (int t = 0; t < 49; ++t)
			for (int i = 0; i < 6; ++i)
				for (int j = 0; j < 6; ++j)
					sum2[t]+=alpha[t][i]*model->transition[i][j]
					*model->observation[seq[d][t+1]][j]*beta[t+1][j];
		for (int t = 0; t < 49; ++t)
			for (int i = 0; i < 6; ++i)
				for (int j = 0; j < 6; ++j)
					eps[t][i][j]=alpha[t][i]*model->transition[i][j]
					*model->observation[seq[d][t+1]][j]*beta[t+1][j]/sum2[t];

		//calculation of parameters
		for (int i = 0; i < 6; ++i)
			pi[i]+=gama[0][i];
		for (int i = 0; i < 6; ++i){
			for (int j = 0; j < 6; ++j){
				for (int t = 0; t < 49; ++t){
					a_n[i][j]+=eps[t][i][j];
					a_d[i][j]+=gama[t][i];
				}
			}
		}
		for (int j = 0; j < 6; ++j){
			for (int k = 0; k < 6; ++k){
				for (int t = 0; t < 50; ++t){
					if (seq[d][t]==k) b_n[j][k]+=gama[t][j];
					b_d[j][k]+=gama[t][j];
				}
			}
		}
	}//end of one data

	//Update model
	for (int i = 0; i < 6; ++i){
		model->initial[i]=pi[i]/10000;
		for (int j = 0; j < 6; ++j){
			model->transition[i][j]=a_n[i][j]/a_d[i][j];
			model->observation[j][i]=b_n[i][j]/b_d[i][j];
		}
	}
}