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
        void addGreen(int position, char letter);
        void addYellow(int position, char letter);
        void addGrey(int position, char letter);
        void testAll();
        void testWord(string test);
        void secondGuess(string first_guess);
        uint64_t stringToInt(string word);
        string intToString(uint64_t iword);

    private:
        vector<vector<int>> letter_map;
        vector<int> possible_words;
        vector<int> possible_words_saved;
        vector<uint64_t> word_list;
        uint64_t target;
        map<char, int> target_map;
    };