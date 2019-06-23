add_library(ini-reader INTERFACE)
target_include_directories(ini-reader INTERFACE inih)
set(COMPONENTS 
        Logger 
        System 
        Interface 
        Route
        DHCPClient 
        NetworkManager
)

foreach( COMPONENT ${COMPONENTS})
    add_subdirectory(${COMPONENT})
    install(DIRECTORY ${PROJECT_BINARY_DIR}/src/${COMPONENT}/output/ USE_SOURCE_PERMISSIONS DESTINATION ${FLASH_DIR})
endforeach()
