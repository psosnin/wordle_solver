#include "wordle.hpp"
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
    WordleSolver wordle = WordleSolver();
    wordle.testAll();
}