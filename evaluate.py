#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

import codecs
import random
import argparse
import typo_model

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

def main():
    parser = argparse.ArgumentParser(description='spelling correctors evaluation')
    parser.add_argument("file", type=str, help="text file to use for evaluation")
    args = parser.parse_args()

    random.seed(42)
    print '[info] loading text'
    originalText = loadText(args.file)
    print '[info] generating typos'
    errorsText = generateTypos(originalText)

    assert len(originalText) == len(errorsText)

    totalErrors = 0

    for i in xrange(len(originalText)):
        if originalText[i] != errorsText[i]:
            totalErrors += 1

    print '[info] Total words: %d, errors: %d (%.2f%%)' % (
                                                        len(originalText),
                                                        totalErrors,
                                                        100.0 * totalErrors / len(originalText))


if __name__ == '__main__':
    main()
