#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

import codecs
import random
import argparse
import typo_model
import time
import copy

try:
    import readline
except:
    pass

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

def generateTypos(text):
    return map(typo_model.generateTypo, text)

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

class NorvigCorrector(Corrector):
    def __init__(self, trainFile):
        super(NorvigCorrector, self).__init__()
        import norvig_spell
        norvig_spell.init(trainFile)

    def correct(self, sentence, position):
        word = sentence[position]
        import norvig_spell
        return norvig_spell.correction(word)

class ContextCorrector(Corrector):
    def __init__(self, modelPath):
        super(ContextCorrector, self).__init__()
        import context_spell
        context_spell.init(modelPath + '.txt', modelPath + '.binary')

    def correct(self, sentence, position):
        import context_spell
        return context_spell.correction(sentence, position)

class ContextPrototypeCorrector(Corrector):
    def __init__(self, modelPath):
        super(ContextPrototypeCorrector, self).__init__()
        import context_spell_prototype
        context_spell_prototype.init(modelPath + '.txt', modelPath + '.binary')

    def correct(self, sentence, position):
        import context_spell_prototype
        return context_spell_prototype.correction(sentence, position)

def evaluateCorrector(correctorName, corrector, originalSentences, erroredSentences, maxWords=None):
    totalErrors = 0
    origErrors = 0
    fixedErrors = 0
    broken = 0

    erroredSentences = copy.deepcopy(erroredSentences)

    lastTime = time.time()
    n = 0
    for sentID in xrange(len(originalSentences)):
        originalText = originalSentences[sentID]
        erroredText = erroredSentences[sentID]
        for pos in xrange(len(originalText)):
            erroredWord = erroredText[pos]
            originalWord = originalText[pos]
            fixedWord = corrector.correct(erroredText, pos)
            erroredText[pos] = fixedWord
            n += 1

            if erroredWord != originalWord:
                origErrors += 1
                if fixedWord == originalWord:
                    fixedErrors += 1
            else:
                if fixedWord != originalWord:
                    broken += 1

            if fixedWord != originalWord:
                totalErrors += 1
            if sentID % 1 == 0 and pos and time.time() - lastTime > 4.0:
                progress = float(sentID) / len(originalSentences)
                err_rate = float(totalErrors) / n
                if maxWords is not None:
                    progress = float(n) / maxWords
                print '[debug] %s: processed %.2f%%, error rate: %.2f%%' % \
                      (correctorName, 100.0 * progress, 100.0 * err_rate)
                lastTime = time.time()

            if maxWords is not None and n >= maxWords:
                break

        if maxWords is not None and n >= maxWords:
            break

            #if originalWord != fixedWord:
            #    print '%s (%s=>%s):\n%s\n\n' % (originalWord, erroredWord, fixedWord, ' '.join(erroredText))

        #if fixedWord != originalWord:
        #    print originalWord, erroredWord, fixedWord

    return float(totalErrors) / n, float(fixedErrors) / origErrors, float(broken) / n

def testMode(corrector):
    while True:
        sentence = raw_input(">> ").lower().strip()
        sentence = normalize(sentence).split()
        if not sentence:
            continue
        newSentence = []
        for i in xrange(len(sentence)):
            newSentence.append(corrector.correct(sentence, i))
        print ' '.join(newSentence)


def main():
    parser = argparse.ArgumentParser(description='spelling correctors evaluation')
    parser.add_argument('file', type=str, help='text file to use for evaluation')
    parser.add_argument('-hs', '--hunspell' , type=str, help='path to hunspell model')
    parser.add_argument('-ns', '--norvig', type=str, help='path to train file for Norvig spell corrector')
    parser.add_argument('-cs', '--context', type=str, help='path to context spell model')
    parser.add_argument('-csp', '--context_prototype', type=str, help='path to context spell prototype model')
    parser.add_argument('-t', '--test', action="store_true")
    parser.add_argument('-mx', '--max_words', type=int, help='max words to evaluate')
    args = parser.parse_args()

    correctors = {
        'dummy': DummyCorrector(),
    }
    #corrector = correctors['dummy']

    maxWords = args.max_words

    print '[info] loading models'

    if args.hunspell:
        corrector = correctors['hunspell'] = HunspellCorrector(args.hunspell)

    if args.norvig:
        corrector = correctors['norvig'] = NorvigCorrector(args.norvig)

    if args.context:
        corrector = correctors['context'] = ContextCorrector(args.context)

    if args.context_prototype:
        corrector = correctors['prototype'] = ContextPrototypeCorrector(args.context_prototype)

    if args.test:
        return testMode(corrector)

    random.seed(42)
    print '[info] loading text'
    originalText = loadText(args.file)

    print '[info] generating typos'
    erroredText = generateTypos(originalText)

    assert len(originalText) == len(erroredText)

    originalSentences = generateSentences(originalText)
    erroredSentences = generateSentences(erroredText)

    assert len(originalSentences) == len(erroredSentences)

    #for s in originalSentences[:50]:
    #    print ' '.join(s) + '.'

    print '[info] total words: %d' % len(originalText)
    print '[info] evaluating'

    results = {}

    for correctorName, corrector in correctors.iteritems():
        errorsRate, fixRate, broken = \
            evaluateCorrector(correctorName, corrector, originalSentences, erroredSentences, maxWords)
        results[correctorName] = errorsRate, fixRate, broken

    print

    print '[info] %12s %8s  %8s  %8s' % ('', 'errRate', 'fixRate', 'broken')
    for k, _ in sorted(results.items(), key=lambda x: x[1]):
        print '[info] %10s  %8.2f%% %8.2f%% %8.2f%%' % \
              (k,
               100.0 * results[k][0],
               100.0 * results[k][1],
               100.0 * results[k][2])


if __name__ == '__main__':
    main()
