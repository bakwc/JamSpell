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
