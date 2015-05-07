#include "Lm.h"

int ORDER;

Lm::Lm()
{}

Lm::~Lm()
{
	delete _root;
}

bool Lm::StoreNgram(const string & token, float prob, float backoff)
{
	vector<string> words;
	yutils::split_by_tag(token, words);
	TrieNode * tnode = _root;
	for(size_t i = 0; i < words.size(); ++i){
		unsigned int tindex;
		map<string, unsigned int>::iterator iter = _dict.find(words[i]);
		if(iter == _dict.end()){
			tindex = (unsigned)_dict.size()+1; // start index at 1, 0 is reserve for OOV
			_dict.insert(pair<string, unsigned int>(words[i], tindex));
		}
		else
			tindex = iter->second;
		TrieNode * next = tnode->Next(tindex);
		if(next != NULL)
			tnode = next;
		else{
			TrieNode * tmp = new TrieNode(tindex, 0);
			tnode->_index.insert(pair<unsigned int, TrieNode *>(tindex, tmp));
			tnode = tmp;
		}
	}
	tnode->_is_word = words.size();
	tnode->prob = prob;
	tnode->backoff = backoff;
	return _root == tnode;
}

bool Lm::ReadNgrams(ifstream & in, const long & size)
{
	long index = 0;
	string line;
	while(getline(in, line) && index < size){
		string::size_type pos1 = line.find("\t",0);
		if(pos1 == string::npos)
			continue;
		float prob = atof(line.substr(0, pos1).c_str());
		float backoff = 0.0;
		string::size_type pos2 = line.find("\t", pos1+1);
		if(pos2 != string::npos){
			backoff = atof(line.substr(pos2+1, line.size()-pos2-1).c_str());
			StoreNgram(line.substr(pos1+1, pos2-pos1-1), prob, backoff);
		}
		else{
			StoreNgram(line.substr(pos1+1, line.size()-pos1-1), prob, backoff);
		}
		++index;
	}
	return true;
}

Lm::Lm(const std::string &lmfile)
{
	_root = new TrieNode();
	
	TrieNode * tmp = new TrieNode(0, 0);
	tmp->prob = PR_OOV;
	tmp->backoff = ALPHA_OOV;
	_root->_index.insert(pair<unsigned int, TrieNode *>(0, tmp));


	string line;
	ifstream in(lmfile.c_str());
	if(!in.good()){
		cerr << "lm file error" << endl;
		return;
	}
	while(getline(in, line)){
		if(line.find("\\data\\") != string::npos){
			getline(in, line);
			while(line.find("=") != string::npos){
				string::size_type pos = line.find("=");
				_ngram_size.push_back(atof(line.substr(pos+1, line.size()-pos-1).c_str()));
				getline(in, line);
			}
		}
		ORDER = (int)_ngram_size.size();
		if(line.find("-grams:") != string::npos){
			string::size_type pos = line.find("-grams:");
			int order = atoi(line.substr(1, pos-1).c_str());
			ReadNgrams(in, _ngram_size[order-1]);
		}
	}
	in.close();
	cout << "Reading Lm complete, vocabulary size:" << _dict.size() << endl;
}

bool Lm::FindNgram(const vector<unsigned> & digits, float & prob, float & backoff)
{
	TrieNode * tnode = _root;
	for(size_t i = 0; i < digits.size(); ++i){
		TrieNode * next = tnode->Next(digits[i]);
		if(next == NULL){
			//cerr << digits[i] << " not in trie" << endl;
			return false;
		}
		tnode = next;
	}
	prob = tnode->prob; backoff = tnode->backoff;
	return true;
}

//bool Lm::FindNgram(const vector<string> & strs, float & prob, float & backoff)
//{
//	TrieNode * tnode = _root;
//	for(size_t i = 0; i < strs.size(); ++i){
//		map<string, unsigned int>::iterator iter = _dict.find(strs[i]);
//		if(iter == _dict.end()){
//			cerr << "out of vocabulary" << endl;
//			return false;
//		}
//		TrieNode * next = tnode->Next(iter->second);
//		if(next == NULL){
//			cerr << strs[i] << " not in trie" << endl;
//			return false;
//		}
//		tnode = next;
//	}
//	prob = tnode->prob; backoff = tnode->backoff;
//	return true;
//}

bool Lm::Match(const vector<unsigned> & strs, int span, float & prob)
{
	float backoff;
	vector<unsigned> tmpstrs;
	for(size_t i = strs.size()-span; i < strs.size(); ++i){
		tmpstrs.push_back(strs[i]);
	}
	if(FindNgram(tmpstrs, prob, backoff))
		return true;
	else
		return false;
}

bool Lm::Backoff(const vector<unsigned> & strs, int span, float & prob, float & unigram_prob)
{
	float backoff = 0.0;
	vector<unsigned> tmpstrs;
	for(size_t i = strs.size()-span; i < strs.size()-1; ++i){
		tmpstrs.push_back(strs[i]);
	}
	if(FindNgram(tmpstrs, prob, backoff)){
		prob = backoff + unigram_prob;
		return true;
	}
	else
		return false;
}

void Lm::Str2digits(const vector<string> & strs, vector<unsigned> & digits)
{
	digits.clear();
	for(size_t i = 0; i < strs.size(); ++i){
		map<string, unsigned int>::iterator iter = _dict.find(strs[i]);
		if(iter != _dict.end())
			digits.push_back(iter->second);
		else
			digits.push_back(0);
	}
	return;
}

float Lm::Query(const vector<string> & strs)
{
	vector<unsigned> digits;
	Str2digits(strs, digits);
	return Query(digits);
}

float Lm::Query(const vector<unsigned> & digits)
{
	float prob, backoff, unigram_prob;
	int i = (int)digits.size();
	vector<unsigned> unigram;
	unigram.push_back(digits[digits.size()-1]);
	if(!FindNgram(unigram, unigram_prob, backoff)){
		cerr << "unigram OOV" << endl;
		return PR_OOV;
	}
	while(i > 0){
		if(Match(digits, i, prob)){ //match exact ngram or n-1 gram or ... 1-gram
			return prob;
		}
		else if(Backoff(digits, i, prob, unigram_prob)){ //ngram not match, resort to match Wn and n-1 gram for backoff
			return prob;
		}
		else{
			--i;
		}
	}
	return PR_OOV;
}

unsigned int Lm::GetDigit(const string & str)
{
	map<string, unsigned>::iterator iter = _dict.find(str);
	if(iter != _dict.end())
		return iter->second;
	else
		return OOV_WID;
}
