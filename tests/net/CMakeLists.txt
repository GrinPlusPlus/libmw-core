set(TARGET_NAME Net_Tests)

file(GLOB SOURCE_CODE
    "*.cpp"
    "proxy/*.cpp"
    "servers/*.cpp"
)

add_executable(${TARGET_NAME} ${SOURCE_CODE})
add_dependencies(${TARGET_NAME} Core::Net)
target_link_libraries(${TARGET_NAME} fmt::fmt Core::Net)