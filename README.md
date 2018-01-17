# JamSpell

[![Build Status][travis-image]][travis] [![Release][release-image]][releases]

[travis-image]: https://travis-ci.org/bakwc/JamSpell.svg?branch=master
[travis]: https://travis-ci.org/bakwc/JamSpell

[release-image]: https://img.shields.io/badge/release-0.0.8-blue.svg?style=flat
[releases]: https://github.com/bakwc/JamSpell/releases

JamSpell is a spell checking library with following features:

- **accurate** - it consider words surroundings for better correction
- **fast** - near 2K words per second
- **multi-language** - it's written in C++ and available for many languages with swig bindings

## Benchmarks

<table>
  <tr>
    <td></td>
    <td>Errors</td>
    <td>Top 7 Errors</td>
    <td>Fix Rate</td>
    <td>Top 7 Fix Rate</td>
    <td>Broken</td>
    <td>Speed<br>
(words/second)</td>
  </tr>
  <tr>
    <td>JamSpell</td>
    <td>3.25%</td>
    <td>1.27%</td>
    <td>79.53%</td>
    <td>84.10%</td>
    <td>0.64%</td>
    <td>1833</td>
  </tr>
  <tr>
    <td>Norvig</td>
    <td>7.62%</td>
    <td>5.00%</td>
    <td>46.58%</td>
    <td>66.51%</td>
    <td>0.69%</td>
    <td>395</td>
  </tr>
  <tr>
    <td>Hunspell</td>
    <td>13.10%</td>
    <td>10.33%</td>
    <td>47.52%</td>
    <td>68.56%</td>
    <td>7.14%</td>
    <td>163</td>
  </tr>
  <tr>
    <td>Dummy</td>
    <td>13.14%</td>
    <td>13.14%</td>
    <td>0.00%</td>
    <td>0.00%</td>
    <td>0.00%</td>
    <td>-</td>
  </tr>
</table>

Model was trained on [300K wikipedia sentences + 300K news sentences (english)](http://wortschatz.uni-leipzig.de/en/download/). 95% was used for train, 5% was used for evaluation. [Errors model](https://github.com/bakwc/JamSpell/blob/master/evaluate/typo_model.py) was used to generate errored text from the original one. JamSpell corrector was compared with [Norvig's one](http://norvig.com/spell-correct.html), [Hunspell](http://hunspell.github.io/) and a dummy one (no corrections).

We used following metrics:
- **Errors** - percent of words with errors after spell checker processed
- **Top 7 Errors** - percent of words missing in top7 candidated
- **Fix Rate** - percent of errored words fixed by spell checker
- **Top 7 Fix Rate** - percent of errored words fixed by one of top7 candidates
- **Broken** - percent of non-errored words broken by spell checker
- **Speed** - number of words per second

More details about reproducing available in "[Train](#train)" section.

## Python
1. Install ```swig3``` (usually it is in your distro package manager)

2. Install ```jamspel```:
```bash
pip install jamspell
```
3. Download or [train](#train) language model

4. Use it:

```python
import jamspell

corrector = jamspell.TSpellCorrector()
corrector.LoadLangModel('model_en.bin')

corrector.FixFragment('I am the begt spell cherken!')
# u'I am the best spell checker!'

corrector.GetCandidates(['i', 'am', 'the', 'begt', 'spell', 'cherken'], 3)
# (u'best', u'beat', u'belt', u'bet', u'bent', ... )

corrector.GetCandidates(['i', 'am', 'the', 'begt', 'spell', 'cherken'], 5)
# (u'checker', u'chicken', u'checked', u'wherein', u'coherent', ...)
```

## Train
To train custom model you need:

1. Install ```cmake```

2. Clone and build jamspell:
```bash
git clone https://github.com/bakwc/JamSpell.git
cd JamSpell
mkdir build
cd build
cmake ..
make
```

3. Prepare a utf-8 text file with sentences to train at (eg. [```sherlockholmes.txt```](https://github.com/bakwc/JamSpell/blob/master/test_data/sherlockholmes.txt)) and another file with language alphabet (eg. [```alphabet_en.txt```](https://github.com/bakwc/JamSpell/blob/master/test_data/alphabet_en.txt))

4. Train model:
```bash
./main/jamspell train ../test_data/alphabet_en.txt ../test_data/sherlockholmes.txt model_sherlock.bin
```
5. To evaluate spellchecker you can use ```evaluate/evaluate.py``` script:
```bash
python evaluate/evalute.py -a alphabet_file.txt -jsp your_model.bin -mx 50000 your_test_data.txt
```
6. You can use ```evaluate/generate_dataset.py``` to generate you train/test data. It supports txt files, [Leipzig Corpora Collection](http://wortschatz.uni-leipzig.de/en/download/) format and fb2 books.

