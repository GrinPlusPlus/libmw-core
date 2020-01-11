set(TARGET_NAME civetweb)

set(CIVETWEB_SOURCES
    src/civetweb.c
    src/CivetServer.cpp
)

add_library(${TARGET_NAME} ${CIVETWEB_SOURCES})