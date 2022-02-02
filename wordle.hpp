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
        void setTarget(string target);
        int makeGuess(string guess);
        void addGreen(int position, char letter);
        void addYellow(int position, char letter);
        void addGrey(int position, char letter);
        void testAll();
        void testWord(string test);
    private:
        vector<vector<int>> letter_map;
        vector<int> possible_words;
        vector<int> possible_words_saved;
        vector<string> word_list;
        string target;
        map<char, int> target_map;
    };