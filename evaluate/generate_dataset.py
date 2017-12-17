#!/usr/bin/env python
# -*- coding: utf-8 -*-

import codecs
import random

SOURCE_FILES = [
    'eng_wikipedia_2016_300K-sentences.txt',
    'eng_news_2015_300K-sentences.txt',
]
RANDOM_SEED = 42
TRAIN_TEST_SPLIT = 0.85
TRAIN_FILE = 'train.txt'
TEST_FILE = 'test.txt'

def loadSentences(fname):
    sentences = []
    with codecs.open(fname, 'r', 'utf-8') as f:
        for line in f:
            sentences.append(line.split('\t')[1].strip().lower())
    return sentences

def saveSentences(sentences, fname):
    with codecs.open(fname, 'w', 'utf-8') as f:
        for s in sentences:
            f.write(s)
            f.write('\n')

random.seed(RANDOM_SEED)
sentences = []
for sourceFile in SOURCE_FILES:
    print '[info] loading', sourceFile
    sentences += loadSentences(sourceFile)

print '[info] loaded %d sentences' % len(sentences)
print '[info] removing duplicates'

sentences = list(set(sentences))

print '[info] %d left' % len(sentences)
print '[info] shuffling'

random.shuffle(sentences)


total = len(sentences)
trainHalf = int(total * TRAIN_TEST_SPLIT)
trainSentences = sentences[:trainHalf]
testSentences = sentences[trainHalf:]

print '[info] saving train set'
saveSentences(trainSentences, TRAIN_FILE)

print '[info] saving test set'
saveSentences(testSentences, TEST_FILE)

print '[info] done'

