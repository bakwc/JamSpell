%module jamspell
%include <std_pair.i>
%include "std_vector.i"
%include <std_string.i>
%include <std_wstring.i>

// Instantiate templates used by example
namespace std {
   %template(StringVector) vector<wstring>;
   %template() pair<wstring,double>;
   %template(PairVector) vector<pair<wstring,double> >;
}

%{
#include "jamspell/spell_corrector.hpp"
%}
%include "jamspell/spell_corrector.hpp"
