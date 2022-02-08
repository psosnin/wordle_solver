from lib.pywordle import *
import numpy as np

with open('words.txt') as f:
    words = f.read().splitlines()
    f.close()

searchForWorst(8)