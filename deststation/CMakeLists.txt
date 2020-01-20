set(SOURCES
    src/deststation_log.c
    src/deststation_main.c
    src/deststation_socket.c
)

set(HEADERS
    src/deststation_common.h
    src/deststation_log.h
    src/deststation_main.h
    src/deststation_socket.h
)

add_executable(deststation ${SOURCES} ${HEADERS})
