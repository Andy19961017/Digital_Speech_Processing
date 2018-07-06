#include "Ngram.h"
#include <sstream>
#include <iostream>
#include <map>
#include <string>
#include <iterator>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace std;

static Vocab voc;
Ngram lm( voc, 2 );
vector<string> split(const string &source);
static map<string, vector<string> > dic;
static map<string, vector<string> >::iterator iter;
double getProb(const char *w1, const char *w2);
void viterbi_init(const vector<string> &words);
void viterbi_iterate(const vector<string> &words);
vector<string> viterbi_back();



int main(int argc, char* argv[]){
    {	
        File lmFile( argv[6], "r" );
        lm.read(lmFile);
        lmFile.close();
	}
	{
		ifstream mapping_f(argv[4]);
		string str;
		while(getline(mapping_f,str)){
			vector<string> temp = split(str);
			dic[temp[0]] = temp;
 		}
	}
	ifstream test_f(argv[2]);
	string str;
	while(getline(test_f,str,'\n')){
		bool flag = true;
		vector<string> sent= split(str);
		for(int i=0; i<sent.size(); ++i){
			const vector<string> words = dic.find(sent[i])->second;
			if (flag){
				viterbi_init(words);	
				flag = false;	
			}
			else{
				viterbi_iterate(words);
			}
		}
		vector<string> word_list = viterbi_back();
		cout << "<s>" << " ";
		for(int i =0; i < word_list.size(); ++i){
			cout << word_list[i] << " ";
		}
		cout << "</s>" << endl;
	}
	return  0;
}


vector<string> split(const string &source)
{
    stringstream ss(source);
    vector<string> vec((istream_iterator<string>(ss)),istream_iterator<string>());
    return vec;
}

double getProb(const string &word1, const string &word2)
{	
	const char* w1 = word1.c_str();
	const char* w2 = word2.c_str();
    VocabIndex w_id1 = voc.getIndex(w1);
    VocabIndex w_id2 = voc.getIndex(w2);
    if(w_id1 == Vocab_None)  //OOV
        w_id1 = voc.getIndex(Vocab_Unknown);
    if(w_id2 == Vocab_None)  //OOV
        w_id2 = voc.getIndex(Vocab_Unknown);
    VocabIndex context[] = { w_id1, Vocab_None };

    return lm.wordProb( w_id2, context);
}

map<string, double> delta[2];
vector<map<string, string> > mem;

void viterbi_init(const vector<string> &words){
	delta[0].clear();
	delta[1].clear();
	mem.clear();
	for(int i=1; i<words.size();++i){
		delta[0][words[i]] = 0.0;
		delta[1][words[i]] = 0.0;
	}
};

void viterbi_iterate(const vector<string> &words){
	delta[1].clear();
	mem.push_back(map<string,string>());
	for(int i = 1; i<words.size();++i){
		string word = words[i];
		double max_prob = -1.0/0.0;
		string max_prob_word = words[1];
		
		for(map<string, double>::iterator it = delta[0].begin();it!= delta[0].end();++it){
			string prev_word = it->first;
			double prev_prob = it->second;
			LogP p = getProb(prev_word,word);
			double prob = prev_prob + p;
			if(prob > max_prob){
				max_prob = prob;
				max_prob_word = prev_word;
			}
		}
		delta[1][word] = max_prob;
		mem.back()[word] = max_prob_word;
	}
	swap(delta[0],delta[1]);
};

vector<string> viterbi_back(){
	double max_prob = -1.0/0.0;
	string word;
	for(map<string, double>::iterator it = delta[0].begin(); it!=delta[0].end();++it){
		string wd = it->first;
		double prob = it->second;
		if(prob>max_prob){
			word = wd, max_prob = prob;
		}
	}
	vector<string> word_list;
	word_list.push_back(word);
	while(!mem.empty()){
		word = mem.back()[word];
		mem.pop_back();
		word_list.push_back(word);
	}
	reverse(word_list.begin(),word_list.end());
	return word_list;
};

