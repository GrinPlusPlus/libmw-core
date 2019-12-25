include_directories(${PROJECT_SOURCE_DIR}/deps/Catch2)

add_subdirectory(models)
add_subdirectory(util)

add_executable(Tests TestMain.cpp Test_ChildProcess.cpp)
target_link_libraries(Tests reproc++)