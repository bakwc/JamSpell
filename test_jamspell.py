import os
import pytest
import jamspell
from evaluate import generate_dataset
from evaluate.evaluate import evaluateJamspell


def removeFile(fname):
    try:
        os.remove(fname)
    except OSError:
        pass

TEMP_MODEL = 'temp_model.bin'
TEMP_SPELL = 'temp_model.bin.spell'
TEMP = 'temp'
TEMP_TEST = TEMP + '_test.txt'
TEMP_TRAIN = TEMP + '_train.txt'
TEST_DATA = 'test_data/'

def teardown_module(module):
    removeFile(TEMP_MODEL)
    removeFile(TEMP_SPELL)
    removeFile(TEMP_TEST)
    removeFile(TEMP_TRAIN)

def trainLangModel(trainText, alphabetFile, modelFile):
    corrector = jamspell.TSpellCorrector()
    corrector.TrainLangModel(trainText, alphabetFile, modelFile)

@pytest.mark.parametrize('sourceFile,alphabetFile,expected', [
    ('sherlockholmes.txt', 'alphabet_en.txt', (0.04519985057900635, 0.7005163511187608, 0.014246804944479363,
                                               0.01363466567052671, 0.7676419965576592)),
])
def test_evaluation(sourceFile, alphabetFile, expected):
    alphabetFile = TEST_DATA + alphabetFile
    generate_dataset.generateDatasetTxt(TEST_DATA + sourceFile, TEMP)
    trainLangModel(TEMP_TRAIN, alphabetFile, TEMP_MODEL)
    results = evaluateJamspell(TEMP_MODEL, TEMP_TEST, alphabetFile)
    assert results == expected
