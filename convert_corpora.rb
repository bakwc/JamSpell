# encoding: UTF-8
require 'csv'

#
# Converts RusCorpora to CSV
# http://www.ruscorpora.ru/corpora-freq.html
#
# Usage:
#     for i in 1grams-3.txt 2grams-3.txt 3grams-3.txt; do ruby ../convert_corpora.rb < $i > $i.csv; done
#

while s = gets
  CSV do |csv|
    regex = /^(\d+)\s+([а-я]+)(?:[\s.,]+([а-я]+))?(?:[\s.,]+([а-я]+))?\s*$/
    if s =~ regex
      csv << [$1, $2, $3, $4].compact
    end
  end
end
