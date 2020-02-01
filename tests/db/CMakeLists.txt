set(TARGET_NAME Database_Tests)

file(GLOB SOURCE_CODE
    "*.cpp"
)

add_executable(${TARGET_NAME} ${SOURCE_CODE})
add_dependencies(${TARGET_NAME} Database)
target_link_libraries(${TARGET_NAME} Database)