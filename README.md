# JamSpell

[![Build Status][travis-image]][travis] [![Release][release-image]][releases]

[travis-image]: https://travis-ci.org/bakwc/JamSpell.svg?branch=master
[travis]: https://travis-ci.org/bakwc/JamSpell

[release-image]: https://img.shields.io/badge/release-0.0.11-blue.svg?style=flat
[releases]: https://github.com/bakwc/JamSpell/releases

JamSpell is a spell checking library with following features:

- **accurate** - it consider words surroundings (context) for better correction
- **fast** - near 5K words per second
- **multi-language** - it's written in C++ and available for many languages with swig bindings

## Content
- [Benchmarks](#benchmarks)
- [Usage](#usage)
  - [Python](#python)
  - [C++](#c)
  - [Other languages](#other-languages)
  - [HTTP API](#http-api)
- [Train](#train)

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
    <td>4854</td>
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

To ensure that our model is not too overfitted for wikipedia+news we checked it on "The Adventures of Sherlock Holmes" text:

<table>
  <tr>
    <td></td>
    <td>Errors</td>
    <td>Top 7 Errors</td>
    <td>Fix Rate</td>
    <td>Top 7 Fix Rate</td>
    <td>Broken</td>
    <td>Speed
(words per second)</td>
  </tr>
  <tr>
    <td>JamSpell</td>
    <td>3.56%</td>
    <td>1.27%</td>
    <td>72.03%</td>
    <td>79.73%</td>
    <td>0.50%</td>
    <td>5524</td>
  </tr>
  <tr>
    <td>Norvig</td>
    <td>7.60%</td>
    <td>5.30%</td>
    <td>35.43%</td>
    <td>56.06%</td>
    <td>0.45%</td>
    <td>647</td>
  </tr>
  <tr>
    <td>Hunspell</td>
    <td>9.36%</td>
    <td>6.44%</td>
    <td>39.61%</td>
    <td>65.77%</td>
    <td>2.95%</td>
    <td>284</td>
  </tr>
  <tr>
    <td>Dummy</td>
    <td>11.16%</td>
    <td>11.16%</td>
    <td>0.00%</td>
    <td>0.00%</td>
    <td>0.00%</td>
    <td>-</td>
  </tr>
</table>

More details about reproducing available in "[Train](#train)" section.

## Usage
### Python
1. Install ```swig3``` (usually it is in your distro package manager)

2. Install ```jamspel```:
```bash
pip install jamspell
```
3. [Download](#download-models) or [train](#train) language model

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

### C++
1. Add `jamspell` and `contrib` dirs to your project

2. Use it:

```cpp
#include <jamspell/spell_corrector.hpp>

int main(int argc, const char** argv) {

    NJamSpell::TSpellCorrector corrector;
    corrector.LoadLangModel("model.bin");

    corrector.FixFragment(L"I am the begt spell cherken!");
    // "I am the best spell checker!"

    corrector.GetCandidates({L"i", L"am", L"the", L"begt", L"spell", L"cherken"}, 3);
    // "best", "beat", "belt", "bet", "bent", ... )

    corrector.GetCandidates({L"i", L"am", L"the", L"begt", L"spell", L"cherken"}, 3);
    // "checker", "chicken", "checked", "wherein", "coherent", ... )
    return 0;
}
```

### Other languages
You can generate extensions for other languages using [swig tutorial](http://www.swig.org/tutorial.html). The swig interface file is `jamspell.i`. Pull requests with build scripts are welcome.

## HTTP API
* Install ```cmake```

* Clone and build jamspell (it includes http server):
```bash
git clone https://github.com/bakwc/JamSpell.git
cd JamSpell
mkdir build
cd build
cmake ..
make
```
* [Download](#download-models) or [train](#train) language model
* Run http server:
```bash
./web_server/web_server en.bin localhost 8080
```
* **GET** Request example:
```bash
$ curl "http://localhost:8080/fix?text=I am the begt spell cherken"
I am the best spell checker
```
* **POST** Request example
```bash
$ curl -d "I am the begt spell cherken" http://localhost:8080/fix
I am the best spell checker
```
* Candidate example
```bash
curl "http://localhost:8080/candidates?text=I am the begt spell cherken"
# or
curl -d "I am the begt spell cherken" http://localhost:8080/candidates
```
```javascript
{
    "results": [
        {
            "candidates": [
                "best",
                "beat",
                "belt",
                "bet",
                "bent",
                "beet",
                "beit"
            ],
            "len": 4,
            "pos_from": 9
        },
        {
            "candidates": [
                "checker",
                "chicken",
                "checked",
                "wherein",
                "coherent",
                "cheered",
                "cherokee"
            ],
            "len": 7,
            "pos_from": 20
        }
    ]
}
```
Here `pos_from` - misspelled word first letter position, `len` - misspelled word len

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

## Download models
Here is a few simple models. They trained on 300K news + 300k wikipedia sentences. We strongly recomend to train your own model, at least on a few million sentences to achieve better quality. See [Train](#train) secion above.

 - [en.tar.gz](https://github.com/bakwc/JamSpell-models/raw/master/en.tar.gz) (35Mb)
 - [fr.tar.gz](https://github.com/bakwc/JamSpell-models/raw/master/fr.tar.gz) (31Mb)
 - [ru.tar.gz](https://github.com/bakwc/JamSpell-models/raw/master/ru.tar.gz) (38Mb)
