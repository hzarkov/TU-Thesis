project("TU-Thesis")
cmake_minimum_required(VERSION 3.14)

set(FLASH_DIR ${PROJECT_BINARY_DIR}/flash)

set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
if(NOT DEFINED INSTALL_PATH)
    message(WARNING "Using ${FLASH_DIR} for rpath. To change it you can set INSTALL_PATH")
    set(INSTALL_PATH ${FLASH_DIR})
endif(NOT DEFINED INSTALL_PATH)

set(CMAKE_INSTALL_RPATH ${INSTALL_PATH}/lib)

install(FILES nm.service DESTINATION ${INSTALL_PATH}/etc/systemd/system/)
install(PROGRAMS start.sh DESTINATION ${INSTALL_PATH}/)

add_subdirectory(src)
