#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

import random
import bisect
from scipy.stats import binom
import utils

#todo: calculate correct typo probabilities

TYPO_PROB = 0.03 # chance of making typo for a single letter
SECOND_TYPO_CF = 0.2 # chance of making two typos, relative to TYPO_PROB
REPLACE_PROB = 0.7
INSERT_PROB = 0.1
REMOVE_PROB = 0.1
TRANSPOSE_PROB = 0.1
TRANSPOSE_DISTANCE_PROB = [0.8, 0.15, 0.04, 0.01]
EPSILON = 0.001
WORD_SPLIT_CHANCE = 0.03
WORD_JOIN_CHANCE = 0.03


assert 1.0 >= TYPO_PROB > 0
assert abs(REPLACE_PROB + INSERT_PROB + REMOVE_PROB + TRANSPOSE_PROB - 1.0) < EPSILON

# Randomly selects a value from list [(value, weight), ... ]
def weightedChoice(values):
    values, weights = zip(*values)
    totalWeight = sum(weights)
    sumWeight = 0.
    distribution = []
    for w in weights:
        sumWeight += w
        distribution.append(sumWeight / totalWeight)
    return values[bisect.bisect(distribution, random.random())]

def typoReplace(word):
    if not word:
        return word
    l = random.randint(0, len(word) - 1)
    return word[:l] + random.choice(utils.ALPHABET) + word[l + 1:]

def typoInsert(word):
    l = random.randint(0, len(word))
    return word[:l] + random.choice(utils.ALPHABET) + word[l:]

def typoRemove(word):
    if not word:
        return word
    l = random.randint(0, len(word) - 1)
    return word[:l] + word[l + 1:]

def swapLetter(s, i, j):
    lst = list(s)
    lst[i], lst[j] = lst[j], lst[i]
    return ''.join(lst)

def typoTranspose(word):
    if not word:
        return word
    l = random.randint(0, len(word) - 1)
    d = weightedChoice(enumerate(TRANSPOSE_DISTANCE_PROB)) + 1
    l1 = max(0, l - d / 2)
    l2 = min(len(word)-1, l1 + d)
    return swapLetter(word, l1, l2)


TYPO_TYPES = [REPLACE_PROB, INSERT_PROB, REMOVE_PROB, TRANSPOSE_PROB]
TYPO_GENERATORS = [typoReplace, typoInsert, typoRemove, typoTranspose]

LEN_TO_PROB = {}

def getWordTypoChance(word):
    l = len(word)
    prob = LEN_TO_PROB.get(l)
    if prob is None:
        noTypoChance = 1.0 - TYPO_PROB
        prob = 1.0 - binom.pmf(l, l, noTypoChance)
    LEN_TO_PROB[l] = prob
    return prob

def generateTypo(word):
    if word == '.':
        return word

    chance = random.random()
    required = getWordTypoChance(word)
    numTypo = 0

    if chance < required:
        numTypo = 1
    if chance < required * SECOND_TYPO_CF:
        numTypo = 2

    for _ in xrange(numTypo):
        typoType = weightedChoice(enumerate(TYPO_TYPES))
        word = TYPO_GENERATORS[typoType](word)
    return word


# orig:  the quick brown fox jumped over the lazy dog
# err:   the qrick brown fox jumpd over the lazy dag
# comp:  the qri ck brown fox jumpd over the lazydag

#        0    1      2      3    4       5     6    7     8
# orig:  the, quick, brown, fox, jumped, over, the, lazy, dog

#       0    1          2      3    4      5     6    7        8
# comp: the, (qri, ck), brown, fox, jumpd, over, the, lazydag, None


def generateTypos(origText):
    errText = map(generateTypo, origText)
    compText = []
    i = 0
    while i < len(errText) - 1:
        word = errText[i]
        nextWord = errText[i + 1]
        if random.random() < WORD_SPLIT_CHANCE and len(word) >= 2:
            pos = random.randint(1, len(word) - 1)
            w1 = word[:pos]
            w2 = word[pos:]
            compText.append((w1, w2))
            i += 1
            continue
        if random.random() < WORD_JOIN_CHANCE and word != '.' and nextWord != '.':
            compText.append(word + nextWord)
            compText.append(None)
            i += 2
            continue
        compText.append(word)
        i += 1
    compText.append(errText[-1])
    return compText
