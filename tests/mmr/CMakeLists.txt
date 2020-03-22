set(TARGET_NAME MMR_Tests)

file(GLOB SOURCE_CODE
    "*.cpp"
)

add_executable(${TARGET_NAME} ${SOURCE_CODE})
add_dependencies(${TARGET_NAME} Core::Crypto Core::File Core::MMR)
target_link_libraries(${TARGET_NAME} fmt::fmt Core::Crypto Core::File Core::MMR)