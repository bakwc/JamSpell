import re
from collections import Counter

def words(text): return re.findall(r'\w+', text.lower())

WORDS = Counter()
TOTAL_WORDS = 0

def init(filename = 'big.txt'):
    global WORDS
    global TOTAL_WORDS
    WORDS = Counter(words(open(filename).read()))
    TOTAL_WORDS=sum(WORDS.values())

def P(word, N=None):
    "Probability of `word`."
    N = N or TOTAL_WORDS
    return WORDS[word] / N

def correction(word):
    if known([word]):
        return word
    cands = known(edits1(word)) or known(edits2(word))
    if not cands:
        return word
    cands = sorted(cands, key=P, reverse=True)
    if cands[0] == word:
        return word
    return sorted(cands, key=P, reverse=True)

def known(words):
    "The subset of `words` that appear in the dictionary of WORDS."
    return set(w for w in words if w in WORDS)

def edits1(word):
    "All edits that are one edit away from `word`."
    letters    = 'abcdefghijklmnopqrstuvwxyz'
    splits     = [(word[:i], word[i:])    for i in range(len(word) + 1)]
    deletes    = [L + R[1:]               for L, R in splits if R]
    transposes = [L + R[1] + R[0] + R[2:] for L, R in splits if len(R)>1]
    replaces   = [L + c + R[1:]           for L, R in splits if R for c in letters]
    inserts    = [L + c + R               for L, R in splits for c in letters]
    return set(deletes + transposes + replaces + inserts)

def edits2(word):
    "All edits that are two edits away from `word`."
    return (e2 for e1 in edits1(word) for e2 in edits1(e1))
