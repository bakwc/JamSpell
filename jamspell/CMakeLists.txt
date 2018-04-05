
add_library(jamspell_lib spell_corrector.cpp lang_model.cpp utils.cpp perfect_hash.cpp bloom_filter)
target_link_libraries(jamspell_lib phf cityhash)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(jamspell_lib ${Boost_LIBRARIES})
endif()
