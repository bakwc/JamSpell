import re
from collections import Counter
import simple_lm

def words(text): return re.findall(r'\w+', text.lower())

WORDS = Counter()
TOTAL_WORDS = 0
LANG_MODEL = None

def init(filename = 'big.txt', modelName = 'big.bin'):
    global WORDS
    global TOTAL_WORDS
    global LANG_MODEL
    WORDS = Counter(words(open(filename).read()))
    TOTAL_WORDS = sum(WORDS.values())
    LANG_MODEL = simple_lm.SimpleLangModel()
    LANG_MODEL.load(modelName)

    print LANG_MODEL.predict('he is better than me')
    print LANG_MODEL.predict('he is better then me')


# def P(word, sentence, pos):
#     subsent = sentence[max(0,pos-2):pos] + [word] + sentence[pos+1:pos+3]
#     subsent = ' '.join(subsent)
#     return LANG_MODEL.score(subsent, bos = False, eos = False)

def P(word, sentence, pos):
    subsent = sentence[:pos] + [word] + sentence[pos+1:]
    subsent = ' '.join(subsent) + ' .'
    return LANG_MODEL.predict(subsent)
    #return LANG_MODEL.score(subsent, bos = True, eos = True)

def correction(sentence, pos):
    "Most probable spelling correction for word."
    word = sentence[pos]
    cands = candidates(word)
    cands = sorted(cands, key=lambda w: P(w, sentence, pos), reverse=True)
    if cands[0] == word:
        return word
    return cands

def candidates(word):
    "Generate possible spelling corrections for word."
    return (known([word]) or known(edits1(word)) or known(edits2(word)) or [word])

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
