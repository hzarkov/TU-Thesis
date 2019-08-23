add_library(ini_reader INTERFACE)
target_include_directories(ini_reader INTERFACE inih)
set(COMPONENTS 
        Logger 
        System 
        Interface 
        Route 
        XTables
        DHCPClient 
        DNSMasqController
        DHCPServer
        DLoader
        InterfaceControllers
        NetworkManager
        FileConfigurators
        InternetSwitchers
)

foreach( COMPONENT ${COMPONENTS})
    add_subdirectory(${COMPONENT})
    install(DIRECTORY ${PROJECT_BINARY_DIR}/src/${COMPONENT}/output/ USE_SOURCE_PERMISSIONS DESTINATION ${FLASH_DIR})
endforeach()
