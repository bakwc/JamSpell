import re
from collections import Counter
import kenlm

def words(text): return re.findall(r'\w+', text.lower())

WORDS = Counter()
TOTAL_WORDS = 0
LANG_MODEL = None

def init(filename = 'big.txt', modelName = 'big.arpa'):
    global WORDS
    global TOTAL_WORDS
    global LANG_MODEL
    WORDS = Counter(words(open(filename).read()))
    TOTAL_WORDS = sum(WORDS.values())
    LANG_MODEL = kenlm.Model(modelName)

# def P(word, sentence, pos):
#     subsent = sentence[max(0,pos-2):pos] + [word] + sentence[pos+1:pos+3]
#     subsent = ' '.join(subsent)
#     return LANG_MODEL.score(subsent, bos = False, eos = False)

def P(word, sentence, pos):
    word, level = word
    subsent = sentence[:pos] + [word] + sentence[pos+1:]
    subsent = ' '.join(subsent) + ' .'
    score = LANG_MODEL.score(subsent, bos = True, eos = True)
    return getScoreByLevel(score, level, len(word))

WEIGHTS = {
    0: 1.0,
    1: 1.1,
    2: 50.0,
}

WEIGHTS_BASE = {
    0: 1.0,
    1: 10.0,
    2: 50.0,
}

def getScoreByLevel(score, level, wordLen):
    #print score, level
    #return score - level * 5.0
    #if wordLen <= 2:

    return score * WEIGHTS[level]

    return score * WEIGHTS_BASE[level]

def correction(sentence, pos):
    "Most probable spelling correction for word."
    word = sentence[pos]
    cands = candidates(word)
    if not cands:
        cands = candidates(word, False)
    if not cands:
        return word
    cands = sorted(cands, key=lambda w: P(w, sentence, pos), reverse=True)
    cands = [c[0] for c in cands]
    if cands[0] == word:
        return word
    return cands

def candidates(word, nearest=True):
    res = {}
    cands = ((0, [word]), (1, edits1(word))) if nearest else ((2, edits2(word)),)
    for lvl, wrds in cands:
        for w in wrds:
            if w in WORDS:
                res.setdefault(w, lvl)
    return res.items()

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
