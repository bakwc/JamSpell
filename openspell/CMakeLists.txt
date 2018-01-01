
add_library(openspell_lib spell_corrector.cpp lang_model.cpp utils.cpp phf_utils.cpp)
target_link_libraries(openspell_lib phf cityhash)
