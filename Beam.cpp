#include "Beam.h"

Beam::Beam(){}

Beam::~Beam()
{
	for(size_t i = 0; i < _states.size(); ++i){
		delete _states[i];
	}
}

bool Beam::Push(State *st)
{
	map<vector<unsigned int>, int>::iterator iter = _signiture.find(st->_sig);
	if(iter == _signiture.end()){
		_states.push_back(st);
		_signiture.insert(pair<vector<unsigned int>, int>(st->_sig, _states.size()-1));
		return true;
	}
	else{
		if(st->_score > _states[iter->second]->_score){
			delete _states[iter->second];
			_states[iter->second] = st;
			return true;
		}
		else
			return false;
	}
}