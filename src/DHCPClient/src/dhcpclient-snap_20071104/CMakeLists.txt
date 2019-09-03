set(PIDDIR "/var/run")
set(DATE "04 Nov 2007")
set(VERSION "snap_20071104")
set(PREFIX "/usr/local")

add_definitions(-DDHCPCLIENT_SCRIPT="${PREFIX}/sbin/dhcpclient-script" -DDHCPCLIENT_VERSION="${VERSION}" -DDHCPCLIENT_PIDDIR="${PIDDIR}")

add_compile_options(-pedantic -std=gnu99 -g -Os -W -Wall)

file(GLOB SRC "*.c")

add_executable(dhcpresolver ${SRC})

install(TARGETS dhcpresolver DESTINATION ${PROJECT_BINARY_DIR}/output/bin/)
