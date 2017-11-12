#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

import codecs
import random
import argparse
import typo_model
import time

FNAME = 'sherlockholmes.txt'

class STATE:
    NONE = 0
    LETTER = 1
    DOT = 2
    SPACE = 3

def normalize(text):
    letters = []
    for l in text.lower():
        if l in typo_model.ALPHABET:
            letters.append(l)
        else:
            letters.append(' ')
    text = ''.join(letters)
    text = ' '.join(text.split())
    return text

assert normalize('AsD?! d!@$%^^ ee   ') == 'asd d ee'

def loadText(fname):
    with codecs.open(fname, 'r', 'utf-8') as f:
        data = f.read()
        return normalize(data).split()

def generateTypos(text):
    return map(typo_model.generateTypo, text)

class Corrector(object):
    def __init__(self):
        pass

    def correct(self, sentence, position):
        pass

class DummyCorrector(Corrector):
    def __init__(self):
        super(DummyCorrector, self).__init__()

    def correct(self, sentence, position):
        return sentence[position]

class HunspellCorrector(Corrector):
    def __init__(self, modelPath):
        super(HunspellCorrector, self).__init__()
        import hunspell
        self.__model = hunspell.HunSpell(modelPath + '.dic', modelPath + '.aff')

    def correct(self, sentence, position):
        word = sentence[position]
        if self.__model.spell(word):
            return word
        return self.__model.suggest(word)[0]

def evaluateCorrector(corrector, originalText, erroredText):
    assert len(originalText) == len(erroredText)
    totalErrors = 0
    lastTime = time.time()
    for pos in xrange(len(originalText)):
        erroredWord = erroredText[pos]
        originalWord = originalText[pos]
        fixedWord = corrector.correct(erroredText, pos)
        if fixedWord != originalWord:
            totalErrors += 1
        if pos % 50 == 0 and pos and time.time() - lastTime > 4.0:
            print '[debug] processed %.2f%%, error rate: %.2f%%' % (100.0 * pos / len(originalText), 100.0 * totalErrors / pos)
            lastTime = time.time()

    return float(totalErrors) / len(originalText)

def main():
    parser = argparse.ArgumentParser(description='spelling correctors evaluation')
    parser.add_argument('file', type=str, help='text file to use for evaluation')
    parser.add_argument('-hs', '--hunspell' , type=str, help='path to hunspell model')
    args = parser.parse_args()

    correctors = {
        'dummy': DummyCorrector(),
    }

    if args.hunspell:
        correctors['hunspell'] = HunspellCorrector(args.hunspell)

    random.seed(42)
    print '[info] loading text'
    originalText = loadText(args.file)

    print '[info] generating typos'
    erroredText = generateTypos(originalText)

    print '[info] total words: %d' % len(originalText)
    print '[info] evaluating'

    for correctorName, corrector in correctors.iteritems():
        errorsRate = evaluateCorrector(corrector, originalText, erroredText)
        print '[info] "%s": %.2f%%' % (correctorName, 100.0 * errorsRate)


if __name__ == '__main__':
    main()
