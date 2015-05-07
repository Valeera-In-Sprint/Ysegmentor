#include <assert.h>
#include <stdlib.h>
#include <float.h>
#include<pthread.h>
#include "Segmentor.h"

using namespace std;

Segmentor g_seg;
void* run(void *arg)
{
    int i = *(int *) arg;
    g_seg._segs[i].clear();
    string res;
    for (unsigned j = 0; j < g_seg._sentences[i].size(); j++) {
		//cout << g_seg._sent_str[j] << endl;
		g_seg.LmSeg(g_seg._sentences[i][j], res, g_seg._sent_str[i][j]);
		g_seg._segs[i].push_back(res);
    }
    return NULL;
}

void Segment(const string & corpus, const string & outfile)
{
	ifstream infile(corpus.c_str());
	ofstream fout(outfile.c_str());
	string line;
	vector<unsigned int> digits;
	int i,j, cnt;
	int threadIdx[MAX_THREAD];
    pthread_t thread[MAX_THREAD];
	while(true){
		i = 0;
		while(true){
			g_seg._sentences[i].clear();g_seg._sent_str[i].clear();
			cnt = 0;
			while(getline(infile, line)){
				cnt++;
				digits.clear();
				for(size_t k = 0; k < line.size(); k+=3){
					digits.push_back(g_seg.char2ins(line[k], line[k+1], line[k+2]));
				}
				g_seg._sentences[i].push_back(digits);
				g_seg._sent_str[i].push_back(line);
                if (cnt >= MAX_BLOCK_SIZE) break;
			}
			threadIdx[i] = i;
			i++;
            if (i == MAX_THREAD || cnt < MAX_BLOCK_SIZE) break;
		}
		//int res = 0;
        for (j = 0; j < i; j++) {
            pthread_create(&(thread[j]), NULL, run, (void*)(threadIdx + j));
        }
        for (j = 0; j < i; j++) {
            pthread_join(thread[j], NULL);
            for (unsigned k = 0; k < g_seg._segs[j].size(); k++) fout << g_seg._segs[j][k] << endl;
        }    
        if (cnt == 0) break;
	}
	infile.close();
	fout.close();
	return;
}

int main(int argc, char * argv[])
{
	if(argc < 4){
		cerr << "paramaters: 1. dictfile 2. lmfile 3. input file 4. output file" << endl;
		return -1;
	}
	g_seg.Init(argv[1], argv[2]);
	//Segmentor s();
	Segment(argv[3], argv[4]);
	
	return 0;
}
