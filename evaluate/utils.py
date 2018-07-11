#!/usr/bin/env python
# -*- coding: utf-8 -*-

import codecs

ALPHABET = 'abcdefghijklmnopqrstuvwxyz'

def normalize(text):
    letters = []
    for l in text.lower():
        if l in ALPHABET:
            letters.append(l)
        elif l in ".?!":
            letters.append(' ')
            letters.append('.')
            letters.append(' ')
        else:
            letters.append(' ')
    text = ''.join(letters)
    text = ' '.join(text.split())
    return text

assert normalize('AsD?! d!@$%^^ ee   ') == 'asd . . d . ee'

def loadText(fname):
    with codecs.open(fname, 'r', 'utf-8') as f:
        data = f.read()
        return normalize(data).split()

def loadAlphabet(fname):
    global ALPHABET
    with codecs.open(fname, 'r', 'utf-8') as f:
        data = f.read()
        data = data.strip().lower()
        ALPHABET = data

def generateSentences(words):
    sentences = []
    currSent = []
    for w in words:
        if w == '.':
            if currSent:
                sentences.append(currSent)
            currSent = []
        else:
            currSent.append(w)
    if currSent:
        sentences.append(currSent)
    return sentences

