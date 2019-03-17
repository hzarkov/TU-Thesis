add_subdirectory(Logger)
install(DIRECTORY ${PROJECT_BINARY_DIR}/src/Logger/output/ USE_SOURCE_PERMISSIONS DESTINATION ${FLASH_DIR})