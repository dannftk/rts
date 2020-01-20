set(SOURCES
    src/vclient_log.c
    src/vclient_main.c
    src/vclient_socket.c
)

set(HEADERS
    src/vclient_common.h
    src/vclient_log.h
    src/vclient_main.h
    src/vclient_socket.h
)

add_executable(vclient ${SOURCES} ${HEADERS})
