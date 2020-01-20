find_package(Threads REQUIRED)

set(SOURCES
    src/gateway_clients.c
    src/gateway_database.c
    src/gateway_dispatcher.c
    src/gateway_log.c
    src/gateway_main.c
    src/gateway_scheduler.c
    src/gateway_socket.c
    src/gateway_tasks.c
)

set(HEADERS
    src/gateway_common.h
    src/gateway_clients.h
    src/gateway_database.h
    src/gateway_dispatcher.h
    src/gateway_log.h
    src/gateway_main.h
    src/gateway_scheduler.h
    src/gateway_socket.h
    src/gateway_tasks.h
)

add_executable(gateway ${SOURCES} ${HEADERS})
target_link_libraries(gateway Threads::Threads)
