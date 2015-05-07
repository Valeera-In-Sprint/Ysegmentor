#ifndef Y_STATE_H
#define Y_STATE_H

#include "Edge.h"
#include "Lm.h"
class State
{

public:
	State();
	~State();
	State(const Edge & eg);
	State* Advance(const Edge & eg, unsigned int wid);

	void UpdateBordary(const vector<unsigned int> & orig, unsigned int wid, vector<unsigned int> & res);
	friend ostream & operator << (ostream & ss, State * st);
	State * _prestate;
	//Y.H incoming edges
	Edge _edge;
	float _score;
	vector<unsigned int> _sig;
};

#endif
