import sys
import csv
import re

#
# Converts RusCorpora to CSV
# http://www.ruscorpora.ru/corpora-freq.html
#
# Usage:
#     for i in 1grams-3.txt 2grams-3.txt 3grams-3.txt; do python convert_corpora.py alphabet.txt < $i > $i.csv; done
#

w = csv.writer(sys.stdout)
a = open(sys.argv[1]).read()
p = re.compile("^(\d+)\s+([%s]+)(?:[\s.,]+([%s]+))?(?:[\s.,]+([%s]+))?\s*$" % (a,a,a))

for line in sys.stdin:
  m = re.match(p, line.lower())
  if m:
    w.writerow([x for x in m.group(1,2,3,4) if x is not None])
