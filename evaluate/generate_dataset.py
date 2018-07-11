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
LANG_DETECT_FRAGMENT_SIZE = 2000


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
    def __init__(self, path, name, lang=None):
        self.__path = path
        self.__name = name
        self.__lang = lang

    def getPath(self):
        return self.__path

    def getName(self):
        return self.__name

    def isMatch(self, pathToFile):
        return False

    def loadSentences(self, pathToFile, sentences):
        pass

    def checkLang(self, textFragment):
        if self.__lang is None:
            return True
        from langdetect import detect
        if detect(textFragment) != self.__lang:
            return False
        return True


class LeipzigDataSource(DataSource):
    def __init__(self, path, lang):
        super(LeipzigDataSource, self).__init__(path, 'leipzig', lang)

    def isMatch(self, pathToFile):
        return pathToFile.endswith('-sentences.txt')

    def loadSentences(self, pathToFile, sentences):
        with codecs.open(pathToFile, 'r', 'utf-8') as f:
            data = f.read()
            if not self.checkLang(data[:LANG_DETECT_FRAGMENT_SIZE]):
                return
            for line in data.split('\n'):
                if not line:
                    continue
                sentences.append(line.split('\t')[1].strip().lower())


class TxtDataSource(DataSource):
    def __init__(self, path, lang):
        super(TxtDataSource, self).__init__(path, 'txt', lang)

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
    def __init__(self, path, lang):
        super(FB2DataSource, self).__init__(path, 'FB2', lang)

    def isMatch(self, pathToFile):
        return pathToFile.endswith('.fb2')

    def loadSentences(self, pathToFile, sentences):
        parser = xml.sax.make_parser()
        handler = FB2Handler(['binary'])
        parser.setContentHandler(handler)
        print('[info] loading file', pathToFile)
        with open(pathToFile, 'rb') as f:
            parser.parse(f)
        data = handler.getBuff()
        if not self.checkLang(data[:LANG_DETECT_FRAGMENT_SIZE]):
            print('[info] wrong language')
        for line in data.split('\n'):
            line = line.strip().lower()
            if not line:
                continue
            sentences.append(line)


def generateDatasetTxt(inFile, outFile):
    source = TxtDataSource(inFile, None)
    sentences = []
    source.loadSentences(inFile, sentences)
    assert sentences
    processSentences(sentences, outFile)


def processSentences(sentences, outFile):
    print('[info] removing duplicates')

    sentences = list(set(sentences))

    print('[info] %d left' % len(sentences))
    print('[info] shuffling')

    random.seed(RANDOM_SEED)
    random.shuffle(sentences)

    total = len(sentences)
    trainHalf = int(total * TRAIN_TEST_SPLIT)
    trainSentences = sentences[:trainHalf]
    testSentences = sentences[trainHalf:]

    print('[info] saving train set')
    saveSentences(trainSentences, outFile + '_train.txt')

    print('[info] saving test set')
    saveSentences(testSentences, outFile + '_test.txt')

    print('[info] done')


def main():
    parser = argparse.ArgumentParser(description='datset generator')
    parser.add_argument('out_file', type=str, help='will be created out_file_train and out_file_test')
    parser.add_argument('-lz', '--leipzig', type=str, help='path to file or dir with Leipzig Corpora files')
    parser.add_argument('-fb2', '--fb2', type=str, help='path to file or dir with files in FB2 format')
    parser.add_argument('-txt', '--txt', type=str, help='path to file or dir with utf-8 txt files')
    parser.add_argument('-lng', '--language', type=str, help='filter by content language')
    args = parser.parse_args()

    lang = None
    if args.language:
        lang = args.language

    dataSources = []
    if args.leipzig:
        dataSources.append(LeipzigDataSource(args.leipzig, lang))
    if args.fb2:
        dataSources.append(FB2DataSource(args.fb2, lang))
    if args.txt:
        dataSources.append(TxtDataSource(args.txt, lang))

    if not dataSources:
        raise Exception('specify at least single data source')

    sentences = []
    for dataSource in dataSources:
        print('[info] loading %s collection' % dataSource.getName())
        path = dataSource.getPath()
        paths = [path] if os.path.isfile(path) else dirFilesIterator(path)
        for filePath in paths:
            if dataSource.isMatch(filePath):
                dataSource.loadSentences(filePath, sentences)

    print('[info] loaded %d sentences' % len(sentences))
    if not sentences:
        print('[error] no sentences loaded')
        return

    processSentences(sentences, args.out_file)


if __name__ == '__main__':
    main()
