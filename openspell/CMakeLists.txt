
add_library(openspell_lib spell_corrector.cpp lang_model.cpp utils.cpp perfect_hash.cpp)
target_link_libraries(openspell_lib phf cityhash)
