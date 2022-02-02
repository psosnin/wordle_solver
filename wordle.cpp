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
    word_list = vector<string>();

    ifstream file_in("fives.txt");
    if (!file_in) {
        cout << "file not found" << endl;
    }

    string line;
    while (getline(file_in, line)) {
        word_list.push_back(line);
    }

    //cout << "no of words = " << word_list.size() << endl;
    
    //initialise vector of sets of indices
    letter_map = vector<vector<int>>(5*26,vector<int>()); //index is position*26 + char - 97
    for (int j = 0; j < word_list.size(); j++) {
        string word = word_list[j];
        for (int k = 0; k < 5; k++) {
            int letter = int(word[k]) - 97;
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
    setTarget("start");
}

void WordleSolver::reset() {
    possible_words = vector<int>(possible_words_saved);
}

int WordleSolver::remainingWords() {
    return possible_words.size();
}

void WordleSolver::setTarget(string word) {
    target = word;
    target_map = map<char, int>();
    for (char c : word) {
        target_map[c]++;
    }
}

void WordleSolver::sampleWords(int n) {
    int count = 0;
    for (int i : possible_words) {
        count++;
        if (count > n) {
            break;
        } else {
            cout << word_list[i] << endl;
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

int WordleSolver::makeGuess(string guess) {
    for (int i = 0; i < 5; i++) {
        if (guess[i] == target[i]) {
            addGreen(i, guess[i]);
        } else if (target_map.find(guess[i]) != target_map.end()) {
            addYellow(i, guess[i]);
        } else {
            addGrey(i, guess[i]);
        }
    }
    return possible_words.size();
}

void WordleSolver::testAll() {
    ofstream myfile("output.txt");
    for (int j = 0; j < word_list.size(); j++) {
        setTarget(word_list[j]);
        myfile << word_list[j] << ", ";
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