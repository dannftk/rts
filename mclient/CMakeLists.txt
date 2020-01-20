set(SOURCES
    src/mclient_log.c
    src/mclient_main.c
    src/mclient_socket.c
)

set(HEADERS
    src/mclient_common.h
    src/mclient_log.h
    src/mclient_main.h
    src/mclient_socket.h
)

add_executable(mclient ${SOURCES} ${HEADERS})
