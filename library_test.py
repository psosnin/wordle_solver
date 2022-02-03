from lib.pywordle import *
wordle = WordleSolver()
#print(wordle.remainingWords())
#wordle.reset()
#wordle.setTarget("moist")
#print(wordle.makeGuess("aloes"))
#print(wordle.makeGuess("asess"))
#wordle.sampleWords(1)
print(list(wordle.testWord("aloes")))