#ifndef Y_LM_H
#define Y_LM_H

#define OOV_WID 0
#define PR_OOV log10(0.00000001)
#define ALPHA_OOV log10(0.95)
//#define ORDER 3
#include "TrieNode.h"

extern int ORDER;


class Lm
{
public:
	Lm();
	Lm(const string & lmfile);
	~Lm();
	float Query(const vector<string> & strs);
	float Query(const vector<unsigned> & digits);
	bool ReadNgrams(ifstream & in, const long & size);
	void Str2digits(const vector<string> & strs, vector<unsigned> & digits);
	unsigned int GetDigit(const string & str);

	TrieNode * _root;
	map<string, unsigned int> _dict; //Y.H this works fine with for 64bit, the size if 4294967295.
	vector<long> _ngram_size;
	//
private:
	bool StoreNgram(const string & token, float prob, float backoff);
	//bool FindNgram(const vector<string> & strs, float & prob, float & backoff);
	bool FindNgram(const vector<unsigned> & digits, float & prob, float & backoff);
	bool Match(const vector<unsigned> & strs, int span, float & prob);
	bool Backoff(const vector<unsigned> & strs, int span, float & prob, float & unigram_prob);
};


#endif
