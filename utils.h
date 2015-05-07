#ifndef Y_UTILS
#define Y_UTILS
#define LOG_FILE_NAME "log.txt"
#define VERBOSE_LIMIT 4

#define LOG1(x) Log::WriteLog(x,1)
#define LOG2(x) Log::WriteLog(x,2)
#define LOG3(x) Log::WriteLog(x,3)

#define LOGE1(x) Log::WriteLogerr(x,1)
#define LOGE2(x) Log::WriteLogerr(x,2)
#define LOGE3(x) Log::WriteLogerr(x,3)

/*static union{
    double d;
    struct{
        int j,i;
        } n;
} d2i;*/
#define M_LN2 0.693147180559945309417232121458176568
#define EXP_A (1048576/M_LN2)
#define EXP_C 60801
#define FAST_EXP(y) (real)(d2i.n.i = EXP_A*(y)+(1072693248-EXP_C),d2i.d)

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <sstream>
#include <iomanip> // for setprecision
#include <ctime>
#include <time.h>
#include <algorithm>
#include <cmath>
#include <queue>
#include <assert.h>
using namespace std;



namespace yutils{


	//Y.H remove blank from both head and tail of the string
	void strip_string(string & str);

	//Y.H split string by a specific tag
	void split_by_tag(const string & str, vector<string> & vec, const char tag=32);
	void split_by_tag(const string & str, vector<string> & vec, const string & tag);

	//Y.H join a vector of string or int into a string by a specific tag
	void join(const vector<string> & vec, string & str, const string & tag);
	void join(const vector<int> & vec, string & str, const string & tag);

	//Y.H replace specific tags in a string
	void replace(string & str, const string & src, const string & tgt);

	int to_int(const string & s);
	string to_string(int i);
	string to_string(double i);
	string to_string(size_t i);
	double random(double max, double min);

	class Log
	{
	public:
		Log(){}
		~Log(){}
		static void WriteLog(const string & str, int verbose)
		{
			if(verbose < VERBOSE_LIMIT){
				cout << str << endl;
			}
		}
		static void WriteLogerr(const string & str, int verbose)
		{
			if(verbose < VERBOSE_LIMIT){
				cerr << str << endl;
			}
		}

	};
}

#endif
