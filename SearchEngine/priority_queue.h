#include <math.h>
#include <iostream>
#include <queue>
#include <vector>
#include <string>
using std::cout;
using std::endl;
using std::priority_queue;
using std::string;
using std::vector;

#ifndef _Candidate_H
#define _Candidate_H
struct CandidateResult
{
    friend std::ostream &operator<<(std::ostream &os, const CandidateResult &rhs);
public:
    CandidateResult(string word, int freq, int dist)
        : _word(word), _freq(freq), _dist(dist)
    {
    }
public:
    string _word;
    int _freq;
    int _dist;
};

#endif
