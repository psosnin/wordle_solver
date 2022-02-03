#pragma once
#include <set>
#include <string>
#include <map>
#include <vector>
using namespace std;

class WordleSolver
{
    public: 
        WordleSolver();
        int remainingWords();
        void sampleWords(int n = 5);
        void reset();
        void setTargetString(string target);
        void setTargetInt(uint64_t word);
        int makeGuess(uint64_t guess);
        int makeGuessString(string guess);
        void addGreen(int position, char letter);
        void addYellowSingle(int position, char letter);
        void addYellowMany(set<int> position, char letter);
        void addGrey(int position, char letter);
        void addGreySingle(int position, char letter);
        void addGreyMany(set<int> position, char letter);
        void testAll(int start = 0, int end = 100);
        vector<int> testWord(string test);
        vector<int> testTarget(string test);
        void secondGuess(string first_guess);
        uint64_t stringToInt(string word);
        string intToString(uint64_t iword);
        vector<int> remainingWordList();

    private:
        vector<int> setDifference(vector<int> v1, vector<int> v2);
        vector<int> setUnion(vector<int> v1, vector<int> v2);
        vector<int> setIntersection(vector<int> v1, vector<int> v2);
        vector<vector<int>> letter_map;
        vector<int> possible_words;
        vector<int> possible_words_saved;
        vector<uint64_t> word_list;
        uint64_t target;
        map<char, int> target_map;
    };

void testAllMultithread(int n_threads, int n_words);