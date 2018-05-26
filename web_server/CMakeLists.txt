
add_executable(web_server main.cpp)
if(WIN32)
  target_link_libraries(web_server wsock32 ws2_32 jamspell_lib ${CMAKE_THREAD_LIBS_INIT})
else()
  target_link_libraries(web_server jamspell_lib ${CMAKE_THREAD_LIBS_INIT})
endif()

