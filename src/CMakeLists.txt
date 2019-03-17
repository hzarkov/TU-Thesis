project("TU-Thesis")
cmake_minimum_required(VERSION 3.14)

set(FLASH_DIR ${PROJECT_BINARY_DIR}/flash)

set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
if(NOT DEFINED INSTALL_PATH)
    message(WARNING "Using ${FLASH_DIR} for rpath. To change it you can set INSTALL_PATH")
    set(INSTALL_PATH ${FLASH_DIR})
endif(NOT DEFINED INSTALL_PATH)

set(CMAKE_INSTALL_RPATH ${INSTALL_PATH})

add_subdirectory(Logger)
install(DIRECTORY ${PROJECT_BINARY_DIR}/Logger/output/ USE_SOURCE_PERMISSIONS DESTINATION ${FLASH_DIR})