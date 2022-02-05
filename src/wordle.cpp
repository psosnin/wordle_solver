#include "wordle.hpp"
#include <set>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <thread>
using namespace std;

//initialises the starting game state
WordleSolver::WordleSolver() {
    //read wordlist from file
    word_list = vector<uint64_t>();

    ifstream file_in("src/word_ints.txt");
    if (!file_in) {
        cout << "file not found" << endl;
    }

    string line;
    while (getline(file_in, line)) {
        word_list.push_back(stoul(line));
    }

    generateLetterMaps();

    int sum = 0;
    for (auto v : position_map) {
        sum += v.size();
    }
    assert(sum == 5*word_list.size());
    
    //initialise possible words set
    possible_words_reset = vector<int>();
    for (int j  = 0; j < word_list.size(); j++) {possible_words_reset.push_back(j);} 
    possible_words = vector<int>(possible_words_reset);
    //set target word
    target_index = 0;
}

void WordleSolver::generateLetterMaps() {
    //initialise vector of which stores the set of words with the given number of given letters (only up to 3 repeated letters)
    position_map = vector<vector<int>>(5*26,vector<int>()); //index is position*26 + char - 97
    frequency_map = vector<vector<int>>(4*26,vector<int>()); //index is frequency*26 + char - 97
    word_target_maps = vector<map<char, vector<int>>> (word_list.size(), map<char, vector<int>>());
    for (int j = 0; j < word_list.size(); j++) {
        vector<int> tmp = vector<int>(26, 0);
        map<char, vector<int>> tmp_map = map<char, vector<int>>();
        uint64_t word = word_list[j];
        for (int k = 0; k < 5; k++) {
            uint64_t letter = ((word >> 8*k) & 0xFF) - 97;
            tmp_map[char(word >> 8*k)].push_back(k);
            position_map[k*26 + letter].push_back(j);
            tmp[letter]++;
        }
        word_target_maps[j] = tmp_map;
        for (int i = 0; i < tmp.size(); i++) {
            frequency_map[tmp[i]*26 + i].push_back(j);
        }
    }
}

void WordleSolver::reset() {
    possible_words = vector<int>(possible_words_reset);
}

int WordleSolver::remainingWords() {
    return possible_words.size();
}

//int is index of word...
void WordleSolver::setTarget(int index) {
    assert(index >= 0 && index <= word_list.size());
    target_index = index;
}

void WordleSolver::sampleWords(int n) {
    int count = 0;
    for (int i : possible_words) {
        count++;
        if (count > n) {
            break;
        } else {
            cout << intToString(word_list[i]) << endl;
        }
    }
}

vector<int> WordleSolver::wordsWithPos(char letter, int position) {
    assert(position < 5 && position >= 0);
    assert(letter -97 >= 0 && letter - 97 < 26);
    return position_map[position*26 + letter - 97];
}

vector<int> WordleSolver::wordsWithFreq(char letter, int frequency) {
    assert(letter -97 >= 0 && letter - 97 < 26);
    assert(frequency < 4 && frequency >= 0);
    return frequency_map[frequency*26 + letter - 97];
}

//takes index of guess in word_list
int WordleSolver::makeGuess(int guess_index) {
    assert(guess_index >= 0 && guess_index <= word_list.size());
    if (guess_index == target_index) {
        //found target, only possible 1 possible word = target
        possible_words = {target_index};
    }
    map<char, vector<int>> guess_map = word_target_maps[guess_index];
    map<char, vector<int>> target_map = word_target_maps[target_index];
    for (const auto& [k, v] : guess_map) {
        if (target_map.find(k) == target_map.end()) {
            //remove all words containing k
            possible_words = setIntersection(possible_words, frequency_map[k - 97]);
        } else {
            if (guess_map[k].size() > target_map[k].size()) {
                //only keep words containing target_map.size() k's
                possible_words = setIntersection(possible_words, frequency_map[target_map[k].size()*26 + k - 97]);
            } else {
                //remove words with less than guess_map.size() k's
                for (int i = 0; i < guess_map[k].size(); i++) {
                    possible_words = setDifference(possible_words, frequency_map[i*26 + k - 97]);
                }
            }
            //only keep words with greens
            for (int i : setIntersection(guess_map[k], target_map[k])) {
                possible_words = setIntersection(possible_words, position_map[26*i + k - 97]);
            }
            //remove other positions that aren't green
            for (int i : setDifference(guess_map[k], target_map[k])) {
                possible_words = setDifference(possible_words, position_map[26*i + k - 97]);
            }
        }
    }
    return possible_words.size();
}

vector<int> WordleSolver::setDifference(vector<int> v1, vector<int> v2) {
    vector<int> ans = vector<int> (v1.size());
    vector<int>::iterator it;
    it = set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), ans.begin());
    ans.resize(it-ans.begin());
    return ans;
}

vector<int> WordleSolver::setUnion(vector<int> v1, vector<int> v2) {
    vector<int> ans = vector<int> (v1.size() + v2.size());
    vector<int>::iterator it;
    it = set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), ans.begin());
    ans.resize(it-ans.begin());
    return ans;
}
vector<int> WordleSolver::setIntersection(vector<int> v1, vector<int> v2) {
    vector<int> ans = vector<int> (max(v1.size(), v2.size()));
    vector<int>::iterator it;
    it = set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), ans.begin());
    ans.resize(it-ans.begin());
    return ans;
}

vector<int> WordleSolver::remainingWordList() {
    return possible_words;
}

vector<uint64_t> WordleSolver::getWordList() {
    return word_list;
}

uint64_t WordleSolver::stringToInt(string word) {
    uint64_t bits = 0;
    for (int i = 4; i >=0; i--) {
        bits = (bits << 8) | word[i];
    }
    return bits;
}

string WordleSolver::intToString(uint64_t iword) {
    string word = "";
    for (int i = 0; i < 5; i++) {
        word += char(iword >> 8*i);
    }
    return word;
}

vector<vector<int>> WordleSolver::generateCrossTable() {
    vector<vector<int>> results = vector<vector<int>> (word_list.size(), 
                                                       vector<int>(word_list.size(), 0));
    //results[target, guess]
    for (int i = 0; i < word_list.size(); i++) {
        setTarget(i);
        for (int j = 0; j < word_list.size(); j++) {
            if (results[i][j] == 0) {
                reset();
                results[i][j] = makeGuess(j);
                for (int k : possible_words) {
                    results[k][j] = possible_words.size();
                }
            }
        }
    }
    return results;
}