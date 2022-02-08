from lib.pywordle import *
with open('words.txt') as f:
    words = f.read().splitlines()
    f.close()

def tests():
    wordle = WordleSolver()
    wordle.reset()
    wordle.setTarget("aback")
    assert(wordle.makeGuess("fuzzy", False) == 1352)
    wordle.reset()
    wordle.setTarget("acute")
    assert(wordle.makeGuess("vivid", False) == 1328)

# def testHardMode(attempt):
#     wordle = WordleSolver()
#     wordle.setTarget(attempt[-1])
#     for guess in attempt[0:-1]:
#         if (words.index(guess) not in wordle.remainingWordList()):
#             print("Illegal guess:", guess)
#             return False
#         else:
#             wordle.makeGuess(guess, False)
#     return True

tests()

# attempt1 = ['fuzzy', 'vivid', 'mamma', 'check', 'retro', 'error', 'goner', 'boxer', 'wooer', 'power', "lower"]
# attempt2 = ['wreak', 'affix', 'cocoa', 'jazzy', 'mammy', 'nanny', 'happy', 'tatty', 'sassy', 'gaudy', 'badly']
# attempt3 = ['graze', 'every', 'upper', 'refer', 'inner', 'odder', 'joker', 'boxer', 'homer', 'wooer', 'cower', 'lower', 'sower', 'tower']
# attempt4 = ['leave', 'cocoa', 'kayak', 'jazzy', 'mammy', 'baggy', 'nanny', 'tatty', 'happy', 'daddy', 'dairy', 'daisy']
# attempt5 = ['abbot', 'mummy', 'check', 'eerie', 'dried', 'inner', 'fixer', 'wiser', 'piper', 'giver', 'liver', 'river']
# attempt6 = ['blare', 'affix', 'mamma', 'jazzy', 'gawky', 'daddy', 'nanny', 'happy', 'tatty', 'sassy', 'saucy', 'savoy', 'savvy']
# attempts = [attempt1, attempt2, attempt3, attempt4, attempt5, attempt6]
# for atmp in attempts:
#     print(testHardMode(atmp), len(atmp))

wordle = WordleSolver()
wordle.setTarget("tower")
print(wordle.makeGuess("graze", False))
print(wordle.makeGuess("every", False))
print(wordle.makeGuess("upper", False))
print(wordle.makeGuess("refer", False))


#'graze', 'every', 'upper', 'refer', 'inner', 'odder', 'joker', 'boxer', 'homer', 'wooer', 'cower', 'lower', 'sower', 'tower'