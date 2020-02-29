set(TARGET_NAME File_Tests)

file(GLOB SOURCE_CODE
    "*.cpp"
)

add_executable(${TARGET_NAME} ${SOURCE_CODE})
add_dependencies(${TARGET_NAME} Core::File)
target_link_libraries(${TARGET_NAME} fmt Core::File)