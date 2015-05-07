#include "State.h"

State::State():_prestate(NULL),_score(0.0)
{
	_edge._start = 0;
	_edge._end = 0;
}

State::State(const Edge & eg)
{
	_edge._start = eg._start;
	_edge._end = eg._end;
}

State::~State()
{}
	
void State::UpdateBordary(const vector<unsigned int> & orig, unsigned int wid, vector<unsigned int> & res)
{

	int start = 1;
	if((int)orig.size() < ORDER)
		start = 0;
	for(int i = start; i < (int)orig.size(); ++i){
		res.push_back(orig[i]);
	}
	res.push_back(wid);
}


State* State::Advance(const Edge & eg, unsigned int wid)
{
	State * st = new State(eg);
	UpdateBordary(_sig, wid, st->_sig);
	//UpdateScore(_score, st->_score);
	st->_prestate = this;
	return st;
}


ostream & operator << (ostream & ss, State * st)
{
	ss << "Edge:" << st->_edge._start << "-" << st->_edge._end << ", score:" << st->_score;
	ss << ",sig:";
	for(size_t i = 0 ; i < st->_sig.size(); ++i)
		ss << st->_sig[i] << " ";
	ss << endl;
	return ss;
}
