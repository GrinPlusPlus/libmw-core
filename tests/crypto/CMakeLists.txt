set(TARGET_NAME Crypto_Tests)

file(GLOB SOURCE_CODE
    "*.cpp"
)

add_executable(${TARGET_NAME} ${SOURCE_CODE})
add_dependencies(${TARGET_NAME} Core::Crypto fmt::fmt-header-only)
target_link_libraries(${TARGET_NAME} Core::Crypto fmt::fmt-header-only)
target_compile_definitions(${TARGET_NAME} PRIVATE INCLUDE_TEST_MATH)