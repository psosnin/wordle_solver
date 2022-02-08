#include "wordle.hpp"
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <iterator>
#include <limits>
#include <numeric>
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
    ct_flag = false;
    prev_guesses = vector<uint64_t>();
}

void WordleSolver::generateLetterMaps() {
    position_map = vector<vector<int>>(5*26,vector<int>()); //index is position*26 + char - 97
    frequency_map = vector<vector<int>>(5*26,vector<int>()); //index is frequency*26 + char - 97
    word_target_maps = vector<unordered_map<char, vector<int>>> (word_list.size(), unordered_map<char, vector<int>>());
    for (int j = 0; j < word_list.size(); j++) {
        vector<int> tmp = vector<int>(26, 0);
        unordered_map<char, vector<int>> tmp_map = unordered_map<char, vector<int>>();
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
    prev_guesses = vector<uint64_t>();
}

int WordleSolver::remainingWords() {
    return possible_words.size();
}

//int is index of word...
void WordleSolver::setTarget(int index) {
    assert(index >= 0 && index <= word_list.size());
    target_index = index;
    reset();
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

//removes the words that don't match the given pattern from the guess
int WordleSolver::guessPattern(string guess, string pattern) {
    //pattern = y for yellow, g for green and grey otherwise
    assert(guess.size() == 5 && pattern.size() == 5);
    prev_guesses.push_back(stringToInt(guess));
    unordered_map<char, vector<int>> guess_map = unordered_map<char, vector<int>>();
    unordered_map<char, vector<int>> greens = unordered_map<char, vector<int>>();
    unordered_map<char, vector<int>> yellows = unordered_map<char, vector<int>>();
    unordered_map<char, vector<int>> greys = unordered_map<char, vector<int>>();
    for (int i = 0; i < 5; i++) {
        guess_map[guess[i]].push_back(i);
        if (pattern[i] == 'g') {
            greens[guess[i]].push_back(i);
        } else if (pattern[i] == 'y') {
            yellows[guess[i]].push_back(i);
        } else {
            greys[guess[i]].push_back(i);
        }
    }
    for (const auto& [k,v] : guess_map) {
        if (greens[k].size() + yellows[k].size() == 0) {
            possible_words = setIntersection(possible_words, frequency_map[k - 97]);
        } else {
            //keep only words with more than or equal to yellows + greens occurances of letter
            for (int i = 0; i < greens[k].size() + yellows[k].size(); i++) {
                possible_words = setDifference(possible_words, frequency_map[26*i + k - 97]);
            }
        }

        //keep only words with greens:
        for (int g : greens[k]) {
            possible_words = setIntersection(possible_words, position_map[26*g + k - 97]);
        }
        //remove words with yellows in that position:
        for (int y : yellows[k]) {
            possible_words = setDifference(possible_words, position_map[26*y + k - 97]);
        }
    }

    for (const auto& [k,v] : greys) {
        for (int p : v) {
            possible_words = setDifference(possible_words, position_map[26*p + k - 97]);
        }
    } 
    return possible_words.size();
}

string WordleSolver::getPattern(string guess) {
    unordered_map<char, vector<int>> target_map = word_target_maps[target_index];
    unordered_map<char, vector<int>> guess_map;
    for (int i = 0; i < 5; i++) {
        guess_map[guess[i]].push_back(i);
    }
    char pattern[] = "_____";
    for (const auto& [k, v] : guess_map) {
        if (target_map.count(k) == 0) {
            for (int g : v) {
                pattern[g] = '_';
            }
        } else {
            int count = 0;
            //only keep words with greens
            for (int i : setIntersection(guess_map[k], target_map[k])) {
                pattern[i] = 'g';
                count++;
            }
            //remove other positions that aren't green
            for (int i : setDifference(guess_map[k], target_map[k])) {
                if (count >= target_map[k].size()) break;
                pattern[i] = 'y';
                count++;
            }
        }
    }
    string ans = "";
    for (int i = 0; i < 5; i++) {
        ans += pattern[i];
    }
    return ans;
}

//given the remaining possible words, suggest the guess that cuts down the most words on average
int WordleSolver::suggestGuess() {
    vector<uint64_t> prev_guesses_saved = prev_guesses;
    int target_index_saved = target_index;
    vector<int> possible_words_saved = possible_words;
    bool full = possible_words.size() == word_list.size();
    vector<vector<int>> results = full ? generateCrossTable() : generateCrossTableReduced();
    vector<int> total_per_guess = vector<int>(results[0].size(), 0);
    for (int j = 0; j < results.size(); j++) {
        for (int i = 0; i < results[0].size(); i++) {
            total_per_guess[i] += results[j][i];
        }
    }
    vector<int>::iterator min = min_element(total_per_guess.begin(), total_per_guess.end());
    int total_vec_index = distance(total_per_guess.begin(), min);
    int index = full ? total_vec_index : possible_words[total_vec_index];
    while (find(prev_guesses_saved.begin(), prev_guesses_saved.end(), word_list[index]) != prev_guesses_saved.end()) {
        total_per_guess[total_vec_index] = numeric_limits<int>::max();
        min = min_element(total_per_guess.begin(), total_per_guess.end());
        total_vec_index = distance(total_per_guess.begin(), min);
        index = total_vec_index;
    }
    prev_guesses = prev_guesses_saved;
    possible_words = possible_words_saved;
    target_index = target_index_saved;
    assert(index < word_list.size());
    return index;
}

//updates the possible words remaining after making the guess on the given target
int WordleSolver::makeGuess(int guess_index) {
    assert(guess_index >= 0 && guess_index <= word_list.size());
    if (guess_index == target_index) {
        //found target, only possible 1 possible word = target
        possible_words = {target_index};
    }
    unordered_map<char, vector<int>> guess_map = word_target_maps[guess_index];
    unordered_map<char, vector<int>> target_map = word_target_maps[target_index];
    for (const auto& [k, v] : guess_map) {
        if (target_map.count(k) == 0) {
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

uint64_t WordleSolver::getWord(int word_index) {
    assert(word_index >= 0 && word_index < word_list.size());
    return word_list[word_index];
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

//generate the number of possible words remaining when every guess is made on every target
vector<vector<int>> WordleSolver::generateCrossTable() {
    if (ct_flag) {
        return complete_cross_table;
    }
    complete_cross_table = vector<vector<int>> (word_list.size(), 
                                                       vector<int>(word_list.size(), 0));
    vector<int> saved_possible_words = possible_words;
    //results[target, guess]
    reset();
    for (int i = 0; i < word_list.size(); i++) {
        setTarget(i);
        for (int j = 0; j < word_list.size(); j++) {
            if (complete_cross_table[i][j] == 0) {
                reset();
                complete_cross_table[i][j] = makeGuess(j);
                for (int k : possible_words) {
                    complete_cross_table[k][j] = possible_words.size();
                }
            }
        }
    }
    ct_flag = true;
    possible_words = saved_possible_words;
    return complete_cross_table;
}


//generate the number of possible words remaining when every guess is made on remaining possible word
vector<vector<int>> WordleSolver::generateCrossTableReduced() {
    vector<vector<int>> results = vector<vector<int>> (possible_words.size(), 
                                                       vector<int>(word_list.size(), 0));
    vector<int> saved_possible_words = possible_words;
    //results[target, guess]
    for (int i = 0; i < possible_words.size(); i++) {
        setTarget(i);
        for (int j = 0; j < word_list.size(); j++) {
            possible_words = saved_possible_words;
            results[i][j] = makeGuess(j);
        }
    }
    possible_words = saved_possible_words;
    return results;
}

//generate the number of words remaining when every first and second guess combination is made on the target
vector<vector<int>> WordleSolver::generateTargetTable(int target_index, int start, int end) {
    assert(target_index >= 0 && target_index <= word_list.size());
    assert(0 <= start && start <= word_list.size());
    end = min(int(word_list.size()), end);
    assert(start < end);
    vector<vector<int>> results = vector<vector<int>> (end - start, 
                                                       vector<int>(word_list.size(), 0));
    //results[guess1, guess2]
    setTarget(target_index);
    vector<int> saved;
    for (int i = start; i < end; i++) {
        reset();
        if (i % 50 == 0 && start == 0) cout << i << "/" << end << " first guesses" << endl;
        makeGuess(i);
        saved = possible_words;
        for (int j = 0; j < word_list.size(); j++) {
            if (results[i-start][j] == 0) {
                possible_words = saved;
                int remaining = makeGuess(j);
                results[i-start][j] = remaining;
                if (start <= j && end > j) {
                    results[j-start][i] = remaining;
                }
            }
        }
    }
    reset();
    return results;
}
