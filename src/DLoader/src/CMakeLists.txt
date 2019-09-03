add_library(dloader SHARED DLoader.cpp)
target_link_libraries(dloader PUBLIC logger pthread ${CMAKE_DL_LIBS})
target_include_directories(dloader PUBLIC ../include/)
install(TARGETS dloader DESTINATION ${PROJECT_BINARY_DIR}/output/lib)