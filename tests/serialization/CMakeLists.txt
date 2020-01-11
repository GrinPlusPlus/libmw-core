set(TARGET_NAME Serialization_Tests)

file(GLOB SOURCE_CODE
    "*.cpp"
)

add_executable(${TARGET_NAME} ${SOURCE_CODE})
add_dependencies(${TARGET_NAME} Common)
target_link_libraries(${TARGET_NAME} fmt Common)