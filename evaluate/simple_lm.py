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
        self.gram3 = defaultdict(int) # (word1, word2, word3) => count
        self.gram4 = defaultdict(int) # (word1, word2, word3, word4) => count

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
                self.gram2[(sentence[j], sentence[j+1])] += 1
            for j in xrange(len(sentence) - 2):
                self.gram3[(sentence[j], sentence[j+1], sentence[j+2])] += 1
            for j in xrange(len(sentence) - 3):
                self.gram4[(sentence[j], sentence[j+1], sentence[j+2], sentence[j+3])] += 1
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
            assert self.gram1 and self.gram2 and self.gram3 and self.gram4

    def getGram1Prob(self, wordID):
        wordCounts = self.gram1.get(wordID, 0) + 1
        totalWord = len(self.gram1)
        return float(wordCounts) / totalWord

    def getGram2Prob(self, wordID1, wordID2):
        countsWord1 = self.gram1.get(wordID1, 0) + 1
        countsBigram = self.gram2.get((wordID1, wordID2), 0) + 0.0001
        return float(countsBigram) / countsWord1

    def getGram3Prob(self, wordID1, wordID2, wordID3):
        countsGram2 = self.gram2.get((wordID1, wordID2), 0) + 1
        countsGram3 = self.gram3.get((wordID1, wordID2, wordID3), 0) + 0.0001
        return float(countsGram3) / countsGram2

    def getGram4Prob(self, wordID1, wordID2, wordID3, wordID4):
        countsGram3 = self.gram2.get((wordID1, wordID2, wordID3), 0) + 1
        countsGram4 = self.gram3.get((wordID1, wordID2, wordID3, wordID4), 0) + 0.0001
        return float(countsGram4) / countsGram3

    def predict(self, sentence):
        sentence = [self.getWordID(w, False) for w in normalize(sentence).split()]
        result = 0
        for i in xrange(0, len(sentence)):
            result += math.log(self.getGram1Prob(sentence[i]))
        for i in xrange(0, len(sentence) - 1):
            result += math.log(self.getGram2Prob(sentence[i], sentence[i + 1]))
        for i in xrange(0, len(sentence) - 2):
            result += math.log(self.getGram3Prob(sentence[i], sentence[i + 1], sentence[i + 2]))
        for i in xrange(0, len(sentence) - 3):
            result += math.log(self.getGram4Prob(sentence[i], sentence[i + 1], sentence[i + 2], sentence[i + 3]))
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
