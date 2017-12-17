#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

from utils import loadText, normalize, generateSentences
from collections import defaultdict
import cPickle, zlib
import math
import sys
import time

class SimpleLangModel(object):
    def __init__(self):
        self.wordToId = {} # word => int id
        self.idToWord = {} # int id => word
        self.lastID = 0
        self.gram1 = defaultdict(int) # word => count
        self.gram2 = defaultdict(int) # (word1, word2) => count

    def train(self, trainFile):
        print '[info] loading text'
        text = loadText(trainFile)
        sentences = generateSentences(text)
        sentences = self.convertToIDs(sentences)

        print '[info] generating N-grams'
        total = len(sentences)
        lastTime = time.time()
        for i in xrange(0, total):
            sentence = sentences[i]
            for w in sentence:
                self.gram1[w] += 1
            for j in xrange(len(sentence) - 1):
                w1 = sentence[j]
                w2 = sentence[j+1]
                self.gram2[(w1, w2)] += 1
            if time.time() - lastTime >= 4.0:
                lastTime = time.time()
                print '[info] processed %.2f%%' % (100.0 * i / total)

        print '[info] finished training'

    def convertToIDs(self, sentences):
        newSentences = []
        for s in sentences:
            newSentence = []
            for w in s:
                newSentence.append(self.getWordID(w))
            newSentences.append(newSentence)
        return newSentences

    def getWordID(self, word, add=True):
        wid = self.wordToId.get(word)
        if wid is None:
            if add:
                self.lastID += 1
                wid = self.lastID
                self.wordToId[word] = wid
                self.idToWord[wid] = word
            else:
                return -1
        return wid

    def save(self, modelFile):
        with open(modelFile, 'wb') as f:
            data = zlib.compress(cPickle.dumps(self.__dict__, -1))
            f.write(data)

    def load(self, modelFile):
        with open(modelFile, 'rb') as f:
            data = cPickle.loads(zlib.decompress(f.read()))
            self.__dict__.clear()
            self.__dict__.update(data)

    def getGram1Prob(self, wordID):
        wordCounts = self.gram1.get(wordID, 0) + 1
        totalWord = len(self.gram1)
        return float(wordCounts) / totalWord

    def getGram2Prob(self, wordID1, wordID2):
        countsWord1 = self.gram1.get(wordID1, 0) + 1
        countsBigram = self.gram2.get((wordID1, wordID2), 0) + 1
        return float(countsBigram) / countsWord1

    def predict(self, sentence):
        sentence = normalize(sentence).split()
        if len(sentence) == 0:
            return 0
        if len(sentence) == 1:
            return math.log(self.getGram1Prob(self.getWordID(sentence[0], False)))
        result = 0
        for i in xrange(0, len(sentence) - 1):
            w1 = self.getWordID(sentence[i], False)
            w2 = self.getWordID(sentence[i+1], False)
            result += math.log(self.getGram2Prob(w1, w2))
        return result

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print 'usage: %s trainFile model.bin' % sys.argv[0]
        sys.exit(42)

    trainFile = sys.argv[1]
    modelFile = sys.argv[2]

    model = SimpleLangModel()
    model.train(trainFile)
    model.save(modelFile)
