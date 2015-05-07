#ifndef Y_SEGMENTOR_H
#define Y_SEGMENTOR_H

#define MAX_THREAD 32
#define MAX_BLOCK_SIZE 65536

#include "TrieNode.h"
#include "Beam.h"
#include "Lm.h"

class Segmentor
{
public:
	Segmentor();
	bool Init(const string & dict_file, const string & lm_file);
	~Segmentor();


	//unsigned int str2ins(const string & str);
	
	bool MaximumMatch(vector<unsigned> & sent,  vector<pair<int, int> > & res);
	int AmbiguousSpan(vector<unsigned> & sent, int beg, int last);
	void FormatSent(const string & line, vector<unsigned> & digits);
	string ints2str(const string & sent, unsigned i, unsigned j);

	bool LmSeg(vector<unsigned> & sent, string & res, const string & sent_str);
	unsigned int char2ins(unsigned char w1, unsigned char w2, unsigned char w3);
	
	TrieNode * _root;
	Lm * _lm;
	vector< vector<vector<unsigned int> > > _sentences;
	vector<vector<string> > _sent_str;
	vector<vector<string> > _segs;
private:
	void MatchAll(vector<unsigned> & sent, int beg, int end, vector<Beam> & lattice, const string & sent_str);

	TrieNode * FindWord(const string & word);
	int FindMaxSpan(vector<unsigned> & sent, int beg);
	void PrintLattice(vector<Beam> & lattice);

	//void* segment(void *arg);
	//void SegEx(const string & corpus, const string & outfile);
	//pthread_t tid;
};


#endif
