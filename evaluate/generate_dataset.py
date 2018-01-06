#!/usr/bin/env python
# -*- coding: utf-8 -*-

import codecs
import random
import os
import argparse

RANDOM_SEED = 42
TRAIN_TEST_SPLIT = 0.95

def saveSentences(sentences, fname):
    with codecs.open(fname, 'w', 'utf-8') as f:
        for s in sentences:
            f.write(s)
            f.write('\n')

def dirFilesIterator(dirPath):
    for root, directories, filenames in os.walk(dirPath):
        for filename in filenames:
            yield os.path.join(root, filename)

class DataSource(object):
    def __init__(self, rootDir, name):
        self.__rootDir = rootDir
        self.__name = name

    def getRootDir(self):
        return self.__rootDir

    def getName(self):
        return self.__name

    def isMatch(self, pathToFile):
        return False

    def loadSentences(self, pathToFile, sentences):
        pass

class LeipzigDataSource(DataSource):
    def __init__(self, rootDir):
        super(LeipzigDataSource, self).__init__(rootDir, 'leipzig')

    def isMatch(self, pathToFile):
        return pathToFile.endswith('-sentences.txt')

    def loadSentences(self, pathToFile, sentences):
        with codecs.open(pathToFile, 'r', 'utf-8') as f:
            for line in f.read().split('\n'):
                if not line:
                    continue
                sentences.append(line.split('\t')[1].strip().lower())

def main():
    parser = argparse.ArgumentParser(description='datset generator')
    parser.add_argument('out_file', type=str, help='will be created out_file_train and out_file_test')
    parser.add_argument('-lz', '--leipzig', type=str, help='path to directory with Leipzig Corpora files')
    args = parser.parse_args()

    dataSources = []
    if args.leipzig:
        dataSources.append(LeipzigDataSource(args.leipzig))

    if not dataSources:
        raise Exception('specify at least single data source')

    sentences = []
    for dataSource in dataSources:
        print '[info] loading %s collection' % dataSource.getName()
        for filePath in dirFilesIterator(dataSource.getRootDir()):
            if dataSource.isMatch(filePath):
                dataSource.loadSentences(filePath, sentences)

    print '[info] loaded %d sentences' % len(sentences)
    print '[info] removing duplicates'

    sentences = list(set(sentences))

    print '[info] %d left' % len(sentences)
    print '[info] shuffling'

    random.seed(RANDOM_SEED)
    random.shuffle(sentences)

    total = len(sentences)
    trainHalf = int(total * TRAIN_TEST_SPLIT)
    trainSentences = sentences[:trainHalf]
    testSentences = sentences[trainHalf:]

    print '[info] saving train set'
    saveSentences(trainSentences, args.out_file + '_train.txt')

    print '[info] saving test set'
    saveSentences(testSentences, args.out_file + '_test.txt')

    print '[info] done'

if __name__ == '__main__':
    main()
