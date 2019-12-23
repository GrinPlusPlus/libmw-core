include_directories(${PROJECT_SOURCE_DIR}/deps/fmt/include)

add_definitions(-DUNICODE)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/fmt)
