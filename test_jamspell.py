import os
import pytest
#import jamspell
from evaluate import generate_dataset
from evaluate.evaluate import evaluateJamspell
from evaluate.typo_model import generateTypos
from evaluate.utils import loadText


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
    ('kapitanskaya_dochka.txt', 'alphabet_ru.txt', (0.12330535829567463, 0.391304347826087, 0.03866565579984837,
                                                    0.05422853453841188, 0.4391304347826087)),
])
def test_evaluation(sourceFile, alphabetFile, expected):
    alphabetFile = TEST_DATA + alphabetFile
    generate_dataset.generateDatasetTxt(TEST_DATA + sourceFile, TEMP)
    trainLangModel(TEMP_TRAIN, alphabetFile, TEMP_MODEL)
    results = evaluateJamspell(TEMP_MODEL, TEMP_TEST, alphabetFile)
    assert results == expected

@pytest.mark.parametrize('sourceFile', ['sherlockholmes.txt'])
def test_generateTypos(sourceFile):
    origText = loadText(TEST_DATA + sourceFile)
    errText = generateTypos(origText)

    assert len(origText) == len(errText)

    for w in errText:
        assert isinstance(w, unicode) or \
            isinstance(w, type(None)) or \
            isinstance(w, tuple)
