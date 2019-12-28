include_directories(${PROJECT_SOURCE_DIR}/deps/Catch2)

string(REGEX REPLACE "-Werror" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_subdirectory(crypto)
add_subdirectory(models)
add_subdirectory(util)

add_executable(Tests TestMain.cpp)
add_dependencies(Tests fmt Common)
target_link_libraries(Tests reproc++ fmt Common)