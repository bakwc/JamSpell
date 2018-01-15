import os
import jamspell
from evaluate import generate_dataset

def trainLangModel(trainText, alphabetFile, modelFile):
    corrector = jamspell.TSpellCorrector()
    corrector.TrainLangModel(trainText, alphabetFile, modelFile)

def test_evaluation():
    generate_dataset.generateDatasetTxt('test_data/sherlockholmes.txt', 'sherlock')
    trainLangModel('sherlock_train.txt', 'test_data/alphabet_en.txt', 'sherlock.bin')

def removeFile(fname):
    try:
        os.remove(fname)
    except OSError:
        pass

def teardown_module(module):
    removeFile('sherlock_test.txt')
    removeFile('sherlock_train.txt')
    removeFile('sherlock.bin')
    removeFile('sherlock.bin.spell')
