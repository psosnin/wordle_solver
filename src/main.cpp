#include "wordle.hpp"
#include <iostream>
#include <bitset>
#include <vector>
#include <thread>
using namespace std;

void testAllMultithread(int start, int end) {
    WordleSolver wordle = WordleSolver();
    wordle.testAll(start, end);
}

int main(int argc, char *argv[]) {
    cout << "don't exit this program until it finishes you might crash :)" << endl;
    vector<thread> threads;
    int n_threads = 8;
    int step = 5757 / n_threads + 1;
    for (int i = 0; i < n_threads; i++) {
        threads.push_back(thread(testAllMultithread, step*i, step*(i+1)));
    }
    for (auto &th : threads) {
        th.join();
    }
}