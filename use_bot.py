from lib.pywordle import *
import numpy as np

with open('words.txt') as f:
    words = f.read().splitlines()
    f.close()

wordle = WordleSolver()
print("Suggested first guess = ", words[wordle.suggestGuess()])
while True:
    guess = input("please input your guess: ")
    result = input("please input the result of your guess, e.g. 'y__g_': ")
    print("There are {} possible words remaining after your guess".format(wordle.guessPattern(guess, result)))
    print("Some possible targets are: ")
    print("\t", end = "")
    possible_words = wordle.remainingWordList()
    if (len(possible_words) == 1):
        break
    for i in range(min(6, len(possible_words))):
        print(words[possible_words[i]], end = ", ")
    print()
    print("The guess that leaves the fewest possible targets after your next guess is:")
    print("\t" + words[wordle.suggestGuess()])

print("The only remaining target is: ")
print("\t" + words[wordle.remainingWordList()[0]])

"""
average performance =  3.5533477321814253
failures =  64
"""