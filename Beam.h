#ifndef Y_BEAM_H
#define Y_BEAM_H

#include "State.h"
class Beam
{

public:
	Beam();
	~Beam();
	bool Push(State* st);

	vector<State*> _states;
	map<vector<unsigned int>, int> _signiture;
};


#endif