set(TARGET_NAME Common_Tests)

file(GLOB SOURCE_CODE
    "*.cpp"
)

add_executable(${TARGET_NAME} ${SOURCE_CODE})
add_dependencies(${TARGET_NAME} Core::Common fmt::fmt)
target_link_libraries(${TARGET_NAME} Core::Common fmt::fmt)