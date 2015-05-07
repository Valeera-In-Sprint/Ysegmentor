#include "utils.h"


void yutils::strip_string(string & str)
{
	int beg_pos = 0;
	int end_pos = int(str.size());
	for(int i = 0; i < int(str.size()); ++i){
		if(str[i] != '\r' && str[i] != '\n' && str[i] != ' ' && str[i] != '\t'){
			beg_pos = i;
			break;
		}
	}
	for(int i = int(str.size())-1; i >= 0; --i){
		if(str[i] != '\r' && str[i] != '\n' && str[i] != ' ' && str[i] != '\t'){
			end_pos = i+1;
			break;
		}
	}
	if(end_pos > beg_pos)
		str = str.substr(beg_pos, end_pos-beg_pos);
	return;
}



void yutils::split_by_tag(const string & str, vector<string> & vec, const char tag)
{
	vec.clear();
	int beg_pos = 0;
	//int end_pos = -1;
	for(int i = 0; i < int(str.size()); ++i){
		if(str[i] == tag){
			vec.push_back(str.substr(beg_pos, i-beg_pos));
			beg_pos = i+1;
		}
	}
	if(beg_pos < int(str.size())){
		vec.push_back(str.substr(beg_pos, str.size()-beg_pos));
	}
	return;
}

void yutils::split_by_tag(const string & str, vector<string> & vec, const string & tag)
{
	vec.clear();
	size_t found = str.find(tag);
	int beg_pos = 0;
	while(found != string::npos){
		vec.push_back(str.substr(beg_pos, found-beg_pos));
		beg_pos = int(found + tag.size());
		found = str.find(tag, beg_pos);
	}
	if(beg_pos < int(str.size())){
		vec.push_back(str.substr(found, int(str.size())-beg_pos));
	}
	return;
}

void yutils::join(const vector<string> & vec, string & str, const string & tag)
{
	stringstream ss;
	for(size_t i = 0; i < vec.size(); ++i){
		ss << vec[i];
		if(i != vec.size()-1)
			ss << tag;
	}
	str = ss.str();
	return;
}

void yutils::join(const vector<int> & vec, string & str, const string & tag)
{
	stringstream ss;
	for(size_t i = 0; i < vec.size(); ++i){
		ss << vec[i];
		if(i != vec.size()-1)
			ss << tag;
	}
	str = ss.str();
	return;
}

void yutils::replace(string & str, const string & src, const string & tgt)
{
	string::size_type pos = str.find(src);
	while(pos != string::npos){
		str.replace(pos, src.size(), tgt);
		pos += tgt.size();
		pos = str.find(src, pos);
	}
	return;
}

int yutils::to_int(const string & s)
{
	return atoi(s.c_str());
}

string yutils::to_string(int i)
{
	stringstream ss;
	ss << i;
	return ss.str();
}

string yutils::to_string(size_t i)
{
	stringstream ss;
	ss << i;
	return ss.str();
}

string yutils::to_string(double i)
{
	stringstream ss;
	ss << i;
	return ss.str();
}

double yutils::random(double max, double min)
{
	return rand()/(double)RAND_MAX*(max-min)+min;
}
