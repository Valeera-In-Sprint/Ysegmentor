#ifndef Y_EDGE_H
#define Y_EDGE_H

#include "utils.h"

class Edge
{
public:
	Edge(){}
	~Edge(){}
	Edge(int s, int e):_start(s), _end(e)
	{}

	int _start;//Y.H open interval
	int _end;
};


#endif