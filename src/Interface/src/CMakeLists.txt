add_library(interface SHARED Interface.cpp IP.cpp)
find_package (Threads)
target_link_libraries(interface PUBLIC system ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(interface PUBLIC ../include/)
install(TARGETS interface DESTINATION ${PROJECT_BINARY_DIR}/output/lib)

add_subdirectory(test)