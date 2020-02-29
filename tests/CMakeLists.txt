include_directories(${PROJECT_SOURCE_DIR}/deps/Catch2)

string(REGEX REPLACE "-Werror" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_subdirectory(common)
add_subdirectory(crypto)
add_subdirectory(file)
add_subdirectory(models)
add_subdirectory(net)
add_subdirectory(serialization)
add_subdirectory(util)

add_executable(Tests TestMain.cpp)
add_dependencies(Tests fmt Core::Common Common_Tests Crypto_Tests File_Tests Models_Tests Net_Tests Serialization_Tests Util_Tests)
target_link_libraries(Tests reproc++ fmt Core::Common)