%module jamspell
%include "std_vector.i"
%include <std_string.i>
%include <std_wstring.i>

// Instantiate templates used by example
namespace std {
   %template(StringVector) vector<wstring>;
}

%{
#include "jamspell/spell_corrector.hpp"
%}
%include "jamspell/spell_corrector.hpp"
