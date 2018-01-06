#!/usr/bin/env python
# -*- coding: utf-8 -*-

import codecs
import random
import os
import argparse
import xml.sax
from collections import defaultdict

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

class FB2Handler(xml.sax.handler.ContentHandler):
    def __init__(self, tagsToExclude):
        xml.sax.handler.ContentHandler.__init__(self)
        self.__tagsToExclude = tagsToExclude
        self.__counters = defaultdict(int)
        self.__buff = []

    def getBuff(self):
        return ''.join(self.__buff)

    def _mayProcess(self):
        for counter in self.__counters.itervalues():
            if counter > 0:
                return False
        return True

    def startElement(self, name, attrs):
        if name in self.__tagsToExclude:
            self.__counters[name] += 1

    def endElement(self, name):
        if name in self.__tagsToExclude:
            self.__counters[name] -= 1

    def characters(self, content):
        if self._mayProcess():
            self.__buff.append(content)

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

class TxtDataSource(DataSource):
    def __init__(self, rootDir):
        super(TxtDataSource, self).__init__(rootDir, 'txt')

    def isMatch(self, pathToFile):
        return pathToFile.endswith('.txt')

    def loadSentences(self, pathToFile, sentences):
        with codecs.open(pathToFile, 'r', 'utf-8') as f:
            for line in f.read().split('\n'):
                line = line.strip().lower()
                if not line:
                    continue
                sentences.append(line)

class FB2DataSource(DataSource):
    def __init__(self, rootDir):
        super(FB2DataSource, self).__init__(rootDir, 'FB2')

    def isMatch(self, pathToFile):
        return pathToFile.endswith('.fb2')

    def loadSentences(self, pathToFile, sentences):
        parser = xml.sax.make_parser()
        handler = FB2Handler(['binary'])
        parser.setContentHandler(handler)
        print 'loading file', pathToFile
        with open(pathToFile, 'rb') as f:
            parser.parse(f)
        for line in handler.getBuff().split('\n'):
            line = line.strip().lower()
            if not line:
                continue
            sentences.append(line)

def main():
    parser = argparse.ArgumentParser(description='datset generator')
    parser.add_argument('out_file', type=str, help='will be created out_file_train and out_file_test')
    parser.add_argument('-lz', '--leipzig', type=str, help='path to dir with Leipzig Corpora files')
    parser.add_argument('-fb2', '--fb2', type=str, help='path to dir with files in FB2 format')
    parser.add_argument('-txt', '--txt', type=str, help='path to dir with utf-8 txt files')
    args = parser.parse_args()

    dataSources = []
    if args.leipzig:
        dataSources.append(LeipzigDataSource(args.leipzig))
    if args.fb2:
        dataSources.append(FB2DataSource(args.fb2))
    if args.txt:
        dataSources.append(TxtDataSource(args.txt))

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
