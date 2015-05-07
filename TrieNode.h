#ifndef Y_TRIENODE_H
#define Y_TRIENODE_H
#include "utils.h"

class TrieNode
{
public:

	TrieNode();
	TrieNode(unsigned int ch, unsigned int is_word);
	~TrieNode();
	TrieNode* Next(unsigned int ch);

	unsigned int _ch; // index, here we only consider very character has 3 bytes
	unsigned int _is_word; // if this node is the end of a word
	                       // Y.H for language model storage: 1 means 1-gram. 2 means 2-gram, 3 for 3-gram, so on so forth.
	float prob;
	float backoff;
	//unsigned int _wid;
	map<unsigned int, TrieNode *> _index;
};


#endif