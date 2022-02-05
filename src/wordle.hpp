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
        vector<int> wordsWithPos(char letter, int position);
        vector<int> wordsWithFreq(char letter, int frequency);
        void reset();
        uint64_t stringToInt(string word);
        string intToString(uint64_t iword);
        vector<int> remainingWordList();
        vector<uint64_t> getWordList();
        int makeGuess(int guess_index);
        void setTarget(int index);
        vector<vector<int>> generateCrossTable();

    private:
        void generateLetterMaps();
        vector<int> setDifference(vector<int> v1, vector<int> v2);
        vector<int> setUnion(vector<int> v1, vector<int> v2);
        vector<int> setIntersection(vector<int> v1, vector<int> v2);
        vector<vector<int>> position_map;
        vector<vector<int>> frequency_map;
        vector<map<char, vector<int>>> word_target_maps;
        vector<int> possible_words;
        vector<int> possible_words_reset;
        vector<uint64_t> word_list;
        int target_index;
};