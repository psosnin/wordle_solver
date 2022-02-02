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
using namespace std;

//initialises the starting game state
WordleSolver::WordleSolver() {
    //read wordlist from file
    word_list = vector<uint64_t>();

    ifstream file_in("word_ints.txt");
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
    possible_words_saved = vector<int>();
    for (int j  = 0; j < word_list.size(); j++) {possible_words_saved.push_back(j);} 
    possible_words = vector<int>(possible_words_saved);
    //set target word
    setTargetString("start");
}

void WordleSolver::reset() {
    possible_words = vector<int>(possible_words_saved);
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

void WordleSolver::addGreen(int position, char letter) {
    vector<int> v = vector<int>(letter_map[position*26 + int(letter) - 97].size());
    vector<int>::iterator it;
    it = set_intersection(possible_words.begin(), possible_words.end(),
                          letter_map[position*26 + int(letter) - 97].begin(),
                          letter_map[position*26 + int(letter) - 97].end(),
                          v.begin());
    v.resize(it-v.begin());
    possible_words = v;
}

void WordleSolver::addYellow(int position, char letter) {
    vector<int> yellows;
    for (int i = 0; i < 5; i++) {
        if (position != i) {
            vector<int> v = vector<int> (letter_map[i*26 + int(letter) - 97].size() + yellows.size());
            vector<int>::iterator it;
            it = set_union(yellows.begin(), yellows.end(),
                           letter_map[i*26 + int(letter) - 97].begin(),
                           letter_map[i*26 + int(letter) - 97].end(),
                           v.begin());
            v.resize(it-v.begin());
            yellows = v;
        } else {
            vector<int> v = vector<int> (possible_words.size());
            vector<int>::iterator it;
            it = set_difference(possible_words.begin(), possible_words.end(),
                           letter_map[position*26 + int(letter) - 97].begin(),
                           letter_map[position*26 + int(letter) - 97].end(),
                           v.begin());
            v.resize(it-v.begin());
            possible_words = v;
        }
    }
    vector<int> v = vector<int> (possible_words.size());
    vector<int>::iterator it;
    it = set_intersection(possible_words.begin(), possible_words.end(),
                    yellows.begin(), yellows.end(), v.begin());
    v.resize(it-v.begin());
    possible_words = v;
}

void WordleSolver::addGrey(int position, char letter) {
    for (int j = 0; j < 5; j++) {
        vector<int> v = vector<int> (possible_words.size());
        vector<int>::iterator it;
        it = set_difference(possible_words.begin(), possible_words.end(),
                        letter_map[j*26 + int(letter) - 97].begin(),
                        letter_map[j*26 + int(letter) - 97].end(),
                        v.begin());
        v.resize(it-v.begin());
        possible_words = v;
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

int WordleSolver::makeGuess(uint64_t guess) {
    map<char, int> tmp_map = map<char, int>();
    //generate target map
    for (int j = 0; j < 5; j++) {
        tmp_map[char(target >> 8*j)]++;
    }

    //green
    for (int i = 0; i < 5; i++) {
        if (((guess >> 8*i) & 0xFF) == ((target >> 8*i) & 0xFF)) {
            //cout << char(guess >> 8*j) << " " << j << " green" << endl;
            addGreen(i, char(guess >> 8*i));
            tmp_map[char(guess >> 8*i)]--;
        }
    }

    //yellow and grey
    for (int j = 0; j < 5; j++) {
        if (tmp_map.find((guess >> 8*j)) != tmp_map.end() && 
            tmp_map[(guess >> 8*j)] > 0 && ((guess >> 8*j) & 0xFF) != ((target >> 8*j) & 0xFF)) {
            //cout << char(guess >> 8*j) << " " << j << " yellow" << endl;
            addYellow(j, char(guess >> 8*j));
            tmp_map[char(guess >> 8*j)]--;
        } else if (tmp_map.find((guess >> 8*j)) == tmp_map.end()){
            //cout << char(guess >> 8*j) << " " << j << " grey" << endl;
            addGrey(j, char(guess >> 8*j));
        }
    }
    return possible_words.size();
}

void WordleSolver::testAll() {
    ofstream myfile("output_ints.txt");
    for (int j = 0; j < 100; j++) {
        setTargetInt(word_list[j]);
        for (int i = 0; i < word_list.size(); i++) {
            reset();
            myfile << makeGuess(word_list[i]) << ", ";
        }
        myfile << endl;
        if (j%10 == 0) {
            cout << j << endl;
        }
    }
}
/*
void WordleSolver::testWord(string test) {
    vector<int> results = vector<int>(word_list.size());
    for (int j = 0; j < word_list.size(); j++) {
        reset();
        setTarget(word_list[j]);
        results[j] = makeGuess(test);
    }

    ofstream myfile(test+".txt");
    for (int i = 0; i < word_list.size(); i++) {
        myfile << word_list[i] << ", " << results[i] << "," << endl;
    }
}

void WordleSolver::secondGuess(string first_guess) {
    reset();
    cout << "Possible words after first guess: " << possible_words.size() << endl;
    vector<int> words_after_first_guess = vector<int>(possible_words);
    for (int j : words_after_first_guess) {
            setTarget(word_list[j]);
            addGrey(0, 'a');
            addGrey(1, 'l');
            addGrey(2, 'o');
            addGrey(3, 'e');
            addGrey(4, 's');
            makeGuess(first_guess);
            for (int i : possible_words_saved) {
                possible_words = vector<int>(words_after_first_guess);
                makeGuess(word_list[i]);
                cout << "Second guess: " << word_list[i] << " possible words: " << possible_words.size() << endl;
            }
    }
}*/