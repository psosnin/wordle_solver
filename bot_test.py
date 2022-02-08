from numpy import average
from lib.pywordle import *

with open('words.txt') as f:
    words = f.read().splitlines()
    f.close()
n = 0
total = 0
failures = 0
wordle = WordleSolver()
for target in words:
    n += 1
    wordle.reset()
    wordle.setTarget(words.index(target))
    count = 0
    while wordle.remainingWords() > 1:
        count += 1
        next_guess = words[wordle.suggestGuess()]
        result = wordle.getPattern(next_guess)
        #print("Guessing: {} Result: {}".format(next_guess, result))
        wordle.guessPattern(next_guess, result)
        #print("Remaining words: ", wordle.guessPattern(next_guess, result))
        if count >= 50:
            print("timed out on ", target)
    assert(target == words[wordle.remainingWordList()[0]])
    if count > 6:
        failures += 1
        print("failed {} in {} guesses".format(target, count))
    total += count
    if (n % 50 == 0):
        print("{}/{}: found {} in {} guesses".format(n, len(words), target, count))
        print("current performance = ", total / n)

print("average performance = ", total / n)
print("failures = ", failures)


