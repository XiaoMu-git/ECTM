set(SOURCES
    main.c
    task_test.c
    task_console.c
    task_files.c
    task_uart.c
    task_bluetooth.c
    task_wifi.c
)

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS "../inc")
