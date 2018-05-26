
add_executable(web_server main.cpp)
target_link_libraries(web_server jamspell_lib ${CMAKE_THREAD_LIBS_INIT})
