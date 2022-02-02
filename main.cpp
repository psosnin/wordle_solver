#include "wordle.hpp"
#include <iostream>
#include <bitset>
using namespace std;

int main(int argc, char *argv[]) {
    WordleSolver wordle = WordleSolver();
    wordle.reset();
    //wordle.testAll();
    wordle.setTargetInt(wordle.stringToInt("moist"));
    wordle.makeGuess(wordle.stringToInt("aloes"));
    wordle.makeGuess(wordle.stringToInt("asses"));
    //wordle.sampleWords();
    cout << wordle.remainingWords() << endl;
}