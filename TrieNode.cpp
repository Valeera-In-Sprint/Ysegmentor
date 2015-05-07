#include "TrieNode.h"

TrieNode::TrieNode():_is_word(0)
{}

TrieNode::TrieNode(unsigned int ch, unsigned int is_word):_ch(ch),_is_word(is_word)
{}

TrieNode::~TrieNode()
{
	for(map<unsigned int, TrieNode *>::iterator iter = _index.begin(); iter != _index.end(); ++iter){
		delete iter->second;
	}
}

TrieNode* TrieNode::Next(unsigned int ch)
{
	map<unsigned int, TrieNode*>::iterator iter = _index.find(ch);
	if(iter != _index.end())
		return iter->second;
	else
		return NULL;
}
