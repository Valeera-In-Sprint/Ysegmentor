#include "Segmentor.h"

//vector<string> buffers[MAX_THREAD];
//vector<string> segs[MAX_THREAD];

Segmentor::Segmentor()
{}


unsigned int Segmentor::char2ins(unsigned char w1, unsigned char w2, unsigned char w3)
{
	unsigned ch = w1;
	ch |= (w2 << 8);
	ch |= (w3 << 16);
	return ch;
}


bool Segmentor::Init(const string & dict_file, const string & lm_file)
{
	_lm = new Lm(lm_file);

	fstream in_dic(dict_file.c_str());
	//Y.H read dictionary: dict should be one word per line
	string word;
	_root = new TrieNode();
	while(in_dic >> word)
	{
		TrieNode * trienode = _root;
		for(size_t i = 0; i < word.size(); i+=3)
		{
			unsigned ch = char2ins((unsigned char)word[i], (unsigned char)word[i+1], (unsigned char)word[i+2]);
			TrieNode * tmpnode = trienode->Next(ch);
			if(tmpnode == NULL){
				tmpnode = new TrieNode(ch, 0);
				trienode->_index.insert(pair<unsigned int, TrieNode*>(ch, tmpnode));
			}
			trienode = tmpnode;
		}
		trienode->_is_word = 1;
	}
	in_dic.close();
	//Y.H read segmentation corpus.
	//    this should be ready careful, since the corpus size may be huge.
	_segs.resize(MAX_THREAD);
	_sentences.resize(MAX_THREAD);
	_sent_str.resize(MAX_THREAD);
	return true;
}



Segmentor::~Segmentor()
{
	delete _root;
}


//Y.H here we decide to construct the lattice by open interval, which means (0,1) is the first word
int Segmentor::FindMaxSpan(vector<unsigned> & sent, int i)
{
	TrieNode* root = _root;
	int last = i;
	while(i < (int)sent.size()){
		TrieNode* tmpnode = root->Next(sent[i]);
		if(tmpnode != NULL){
			++i;
			root = tmpnode;
			if(tmpnode->_is_word == 1)
				last = i;
		}
		else
			break;
	}
	return last;
}

bool Segmentor::MaximumMatch(vector<unsigned> & sent, vector<pair<int, int> > & res)
{
	int i = 0;
	int last, j;
	while(i < (int)sent.size()){
		last = FindMaxSpan(sent, i);
		if(last == i)//not matched;
			++i;
		else{
			j = AmbiguousSpan(sent, i, last);
			if(j > last){
				last = j;
			}
			else{
				res.push_back(pair<int, int>(i, last));
			}
		}
		i = last;
	}
	return true;
}

int Segmentor::AmbiguousSpan(vector<unsigned> & sent, int beg, int last)
{
	for(int i = beg+1; i < last && i < (int)sent.size(); ++i){
		int j = FindMaxSpan(sent, i);
		if(last < j){
			last = j;
		}
	}
	return last;
}

string Segmentor::ints2str(const string & sent, unsigned i, unsigned j)
{
	//cout << sent << "," << i << "-" << j <<  endl;
	string word = sent.substr(i*3, (j-i)*3);
	//for (; i < j; i++) word += sent[i];
	return word;
}


void Segmentor::MatchAll(vector<unsigned> & sents, int beg, int end, vector<Beam> & lattice, const string & sent_str)
{
	for(int k = beg; k < end; ++k){
		int j = k;
		TrieNode * tnode = _root;
		while(j < end){
			unsigned ch = sents[j];
			TrieNode * it = tnode->Next(ch);
			++j; //Y.H make sure this j is open interval !
			if(it == NULL){
				for(size_t a = 0; a < lattice[k]._states.size(); ++a){
					State * st = lattice[k]._states[a]->Advance(Edge(k,k+1), OOV_WID);
					st->_score = lattice[k]._states[a]->_score + PR_OOV;
					if(!lattice[k+1].Push(st))
						delete st;
				}
				break;
			}
			tnode = it;
			if(it->_is_word > 0){
				string word = ints2str(sent_str, k, j);
				unsigned int wid = _lm->GetDigit(word);
				//if(wid == 0){
				//	cerr << "MatchAll: word not found in lm_dict:" << word << endl;
				//}
				for(size_t a = 0; a < lattice[k]._states.size(); ++a){
					State * st = lattice[k]._states[a]->Advance(Edge(k,j), wid);
					st->_score = lattice[k]._states[a]->_score + _lm->Query(st->_sig);
					if(!lattice[j].Push(st))
						delete st;
				}
			}
		}
	}
}


bool Segmentor::LmSeg(vector<unsigned> & sent, string & res, const string & sent_str)
{
	vector<Beam> lattice;
	lattice.resize(sent.size()+1);
	State * init_state =  new State();
	init_state->_sig.push_back(_lm->GetDigit("<s>"));
	lattice[0].Push(init_state);
	int last, j; int i = 0;
	while(i < (int)sent.size()){
		last = FindMaxSpan(sent, i);
		if(last == i){ //not matched
			for(size_t k = 0; k < lattice[i]._states.size(); ++k){
				State * st = lattice[i]._states[k]->Advance(Edge(i,i+1), OOV_WID);
				st->_score = lattice[i]._states[k]->_score + PR_OOV;
				if(!lattice[i+1].Push(st))
					delete st;
			}
			++last;
		}
		else{
			j = AmbiguousSpan(sent, i, last);
			if(j > last){
				last = j;
				MatchAll(sent, i, j, lattice, sent_str);
			}
			else{
				string word = ints2str(sent_str, i, last);
				unsigned int wid = _lm->GetDigit(word);
				for(size_t k = 0; k < lattice[i]._states.size(); ++k){
					State * st = lattice[i]._states[k]->Advance(Edge(i,last), wid);
					st->_score = lattice[i]._states[k]->_score + _lm->Query(st->_sig);
					if(!lattice[last].Push(st))
						delete st;
				}
			}
		}
		i = last;
	}
	//PrintLattice(lattice);
	float best = -100;
	int index = 0;
	for(size_t i = 0; i < lattice[sent.size()]._states.size(); ++i){
		if(best < lattice[sent.size()]._states[i]->_score){
			best = lattice[sent.size()]._states[i]->_score;
			index = i;
		}
		//cout << lattice[sent.size()]._states[i]->_score << endl;
	}
	State * pre = lattice[sent.size()]._states[index];
	vector<string> res_str;
	res = "";
	while(pre->_prestate != NULL){
		res = ints2str(sent_str, pre->_edge._start, pre->_edge._end) + " " + res;
		//res_str.push_back(ints2str(sent_str, pre->_edge._start, pre->_edge._end));
		pre = pre->_prestate;
	}
	return true;
}

void Segmentor::PrintLattice(vector<Beam> & lattice)
{
	for(size_t i = 0; i < lattice.size(); ++i){
		cout << "Beam:" << i << endl;
		for(size_t j = 0; j < lattice[i]._states.size(); ++j){
			cout << lattice[i]._states[j];
		}
		cout << "=========" << endl;
	}
}
