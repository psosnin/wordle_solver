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

    //cout << "no of words = " << word_list.size() << endl;
    
    //initialise vector of sets of indices
    letter_map = vector<vector<int>>(5*26,vector<int>()); //index is position*26 + char - 97
    for (int j = 0; j < word_list.size(); j++) {
        uint64_t word = word_list[j];
        for (int k = 0; k < 5; k++) {
            uint64_t letter = ((word >> 8*k) & 0xFF) - 97;
            letter_map[k*26 + letter].push_back(j);
        }
    }

    int sum = 0;
    for (auto v : letter_map) {
        sum += v.size();
    }
    assert(sum == 5*word_list.size());
    
    //initialise possible words set
    possible_words_reset = vector<int>();
    for (int j  = 0; j < word_list.size(); j++) {possible_words_reset.push_back(j);} 
    possible_words = vector<int>(possible_words_reset);
    possible_words_last = vector<int>(possible_words_reset);
    //set target word
    setTargetString("start");
}

void WordleSolver::reset() {
    possible_words = vector<int>(possible_words_reset);
}

void WordleSolver::resetLastGuess() {
    possible_words = vector<int>(possible_words_last);
}

int WordleSolver::remainingWords() {
    return possible_words.size();
}

void WordleSolver::setTargetString(string word) {
    target = stringToInt(word);
    target_map = map<char, int>();
    for (char c : word) {
        target_map[c]++;
    }
}

void WordleSolver::setTargetInt(uint64_t word) {
    target = word;
    target_map = map<char, int>();
    for (int i = 0; i < 5; i++) {
        target_map[char(word >> 8*i)]++;
    }
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

void WordleSolver::addGreen(int position, char letter) {
    possible_words = setIntersection(possible_words, letter_map[position*26 + int(letter) - 97]);
}

void WordleSolver::addYellowSingle(int position, char letter) {
    vector<int> yellows;
    for (int i = 0; i < 5; i++) {
        if (position != i) {
            yellows = setUnion(yellows, letter_map[i*26 + int(letter) - 97]);
        } else {
            possible_words = setDifference(possible_words, letter_map[position*26 + int(letter) - 97]);
        }
    }
    possible_words = setIntersection(possible_words, yellows);
}

void WordleSolver::addYellow(vector<int> position, char letter) {
    if (position.size() == 1) {
        addYellowSingle(*(position.begin()), letter);
    } else {
        //remove the positions that actually are yellow:
        for (int i : position) {
            possible_words = setDifference(possible_words, letter_map[i*26 + int(letter) - 97]);
        }
        //find all pairs of sets of ints not in positions
        vector<int> possible_positions = {0,1,2,3,4};
        possible_positions = setDifference(possible_positions, position);
        //now find the union of the intersection of all of these pairs
        vector<int> pairs = vector<int>();
        for (int i : possible_positions) {
            for (int j : possible_positions) {
                if (i != j) {
                    vector<int> tmp = setIntersection(letter_map[i*26 + int(letter) - 97],
                                                      letter_map[j*26 + int(letter) - 97]);
                    pairs = setUnion(tmp, pairs);
                }
            }
        }
        //now take the intersection of all possible pairs with the remaining possible words
        possible_words = setIntersection(possible_words, pairs);
    }
}


void WordleSolver::addGrey(int position, char letter) {
    for (int j = 0; j < 5; j++) {
        possible_words = setDifference(possible_words, letter_map[j*26 + int(letter) - 97]);
    }
}

void WordleSolver::addGreySingle(int position, char letter) {
    possible_words = setDifference(possible_words, letter_map[position*26 + int(letter) - 97]);
}

void WordleSolver::addGreyMany(vector<int> positions, char letter) {
    vector<int> greys = {0,1,2,3,4};
    greys = setDifference(greys, positions);
    for (int i : greys) {
        possible_words = setDifference(possible_words, letter_map[i*26 + int(letter) - 97]);
    }
}

int WordleSolver::makeGuess(uint64_t guess, bool save) {
    if (save) {
        possible_words_last = vector<int>(possible_words);
    }
    map<char, int> tmp_map = map<char, int>(target_map);
    map<char, vector<int>> green_map = map<char, vector<int>>();
    map<char, vector<int>> yellow_map = map<char, vector<int>>();
    //green
    for (int i = 0; i < 5; i++) {
        if (((guess >> 8*i) & 0xFF) == ((target >> 8*i) & 0xFF)) {
            addGreen(i, char(guess >> 8*i));
            tmp_map[char(guess >> 8*i)]--;
            green_map[char(guess >> 8*i)].push_back(i);
        }
    }

    //yellow and grey
    for (int j = 0; j < 5; j++) {
        if (((guess >> 8*j) & 0xFF) == ((target >> 8*j) & 0xFF)) {
            continue;
        } else if (tmp_map.find((guess >> 8*j)) != tmp_map.end() && 
            tmp_map[(guess >> 8*j)] > 0 && ((guess >> 8*j) & 0xFF) != ((target >> 8*j) & 0xFF)) {
            yellow_map[char(guess >> 8*j)].push_back(j);
            tmp_map[char(guess >> 8*j)]--;
        } else if (tmp_map.find((guess >> 8*j)) == tmp_map.end()){
            addGrey(j, char(guess >> 8*j));
        } else if (yellow_map.find((guess >> 8*j)) == yellow_map.end()) {
            addGreyMany(green_map[char(guess >> 8*j)], char(guess >> 8*j));
        } else {
            addGreySingle(j, char(guess >> 8*j));
        }
    }

    for (auto const& [k,v] : yellow_map) {
        addYellow(v, k);
    }

    return possible_words.size();
}

int WordleSolver::makeGuessString(string guess, bool save) {
    return makeGuess(stringToInt(guess), save);
}

void WordleSolver::testAll(int start, int end) {
    //cout << "testing words from " << start << " to " << end << endl;
    string filename = "data/output" + to_string(start) + "_" + to_string(end) + ".txt";
    ofstream myfile(filename);
    reset();
    for (int j = start; j < min(end, int(word_list.size())); j++) {
        setTargetInt(word_list[j]);
        for (int i = 0; i < word_list.size(); i++) {
            reset();
            myfile << makeGuess(word_list[i]) << ", ";
        }
        myfile << endl;
        if (start == 0 && j%20 == 0) {
            cout << "Thread 1 has processed " << j << "/" << end << " words" << endl;
        }
    }
    reset();
}

vector<int> WordleSolver::testWord(string test) {
    vector<int> output = vector<int>(word_list.size(), 0);
    uint64_t guess = stringToInt(test);
    for (int j = 0; j < word_list.size(); j++) {
        reset();
        setTargetInt(word_list[j]);
        output[j] = makeGuess(guess);
    }
    reset();
    return output;
}

vector<int> WordleSolver::testTarget(string test) {
    vector<int> output = vector<int>(word_list.size(), 0);
    setTargetString(test);
    for (int j = 0; j < word_list.size(); j++) {
        reset();
        output[j] = makeGuess(word_list[j]);
    }
    reset();
    return output;
}

vector<int> WordleSolver::remainingWordList() {
    return possible_words;
}

vector<uint64_t> WordleSolver::getWordList() {
    return word_list;
}

void initThread(int start, int end) {
    WordleSolver wordle = WordleSolver();
    wordle.testAll(start, end);
}

void testAllMultithread(int n_threads, int n_words) {
    cout << "don't exit this program until it finishes you might crash :)" << endl;
    vector<thread> threads;
    int step = n_words / n_threads + 1;
    for (int i = 0; i < n_threads; i++) {
        threads.push_back(thread(initThread, step*i, step*(i+1)));
    }
    for (auto &th : threads) {
        th.join();
    }
}

vector<uint64_t> computeNextGuess(vector<uint64_t> guesses, WordleSolver &wordle) {
    wordle.reset();
    uint64_t worst_guess = 0;
    int worst_remaining = -1;
    for (uint64_t guess : guesses) {
        wordle.makeGuess(guess);
    }
    vector<int> valid_indices = wordle.remainingWordList();
    for (int test : valid_indices) {
        wordle.reset();
        for (uint64_t guess : guesses) {
            wordle.makeGuess(guess);
        }
        int remaining = wordle.makeGuess(wordle.getWordList()[test]);
        if (remaining > worst_remaining) {
            worst_guess = wordle.getWordList()[test];
            worst_remaining = remaining;
        }
    }
    guesses.push_back(worst_guess);
    return guesses;
}

void initSearch(int seed) {
    WordleSolver wordle = WordleSolver();
    vector<uint64_t> word_list = wordle.getWordList();
    int max_len = 0;
    int i = seed;
    for (int t = 0; t < word_list.size(); t++) {
        uint64_t target = word_list[t];
        if (seed == 0 && t % 20 == 0) {
            cout << "Thread 1 has processed " << t << " / " << word_list.size() << " targets" << endl;
        }
        i = (i + 1) % 8;
        wordle.setTargetInt(target);
        while (i < word_list.size()) {
            uint64_t first_guess = word_list[i];
            i = i + 50;
            vector<uint64_t> guesses = {first_guess};
            while (guesses.size() < 2 ||
                   guesses[guesses.size() - 1] != 
                   guesses[guesses.size() - 2]) {
                guesses = computeNextGuess(guesses, wordle);
            }
            if (guesses.size() > max_len) {
                if (guesses.size() > 8) {
                    cout << "Thread " << seed << " found: ";
                    for (uint64_t guess : guesses) {
                        cout << wordle.intToString(guess) << ", ";
                    }
                    cout << endl;
                }
                max_len = guesses.size();
            }
        }
    }
}

void searchForWorst(int n_threads) {
    cout << "don't exit this program until it finishes you might crash :)" << endl;
    vector<thread> threads;
    for (int i = 0; i < n_threads; i++) {
        threads.push_back(thread(initSearch, i));
    }
    for (auto &th : threads) {
        th.join();
    }
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