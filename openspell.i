%module openspell
%include "std_vector.i"
%include <std_string.i>
%include <std_wstring.i>

// Instantiate templates used by example
namespace std {
   %template(StringVector) vector<wstring>;
}

%{
#include "openspell/spell_corrector.hpp"
%}
%include "openspell/spell_corrector.hpp"
