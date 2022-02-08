from numpy import average
from lib.pywordle import *

with open('words.txt') as f:
    words = f.read().splitlines()
    f.close()

wordle = WordleSolver()
print(wordle.remainingWords())
wordle.generateCrossTable()
"""
wordle.setTarget(words.index("fatty"))
count = 0
while wordle.remainingWords() > 1:
    count += 1
    next_guess = words[wordle.suggestGuess()]
    print(next_guess)
    result = wordle.getPattern(next_guess)
    #print("Guessing: {} Result: {}".format(next_guess, result))
    print("Remaining words: ", wordle.guessPattern(next_guess, result))
print("found {} in {} guesses".format("fatty", count))"""
