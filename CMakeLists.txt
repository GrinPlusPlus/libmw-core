cmake_minimum_required(VERSION 3.8)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version")

project(LIBMW-CORE)

include(cmake/SetOutputDir.cmake)
include(cmake/SetOptimizations.cmake)
include(cmake/SetCpp17.cmake)

if(NOT MSVC) # TODO: This is for GCC. Need to determine clang flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-copy -Wno-pessimizing-move")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(WITH_GFLAGS OFF)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Dependencies
include(deps/dependencies.cmake)

if (WIN32)
    add_definitions(-DMW_ENABLE_WSTRING -DUNICODE -DNOMINMAX)
endif()

add_subdirectory(src/common)
add_subdirectory(src/db)
add_subdirectory(src/net)
add_subdirectory(src/crypto)
add_subdirectory(src/traits)

if (MSVC)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4 /WX" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
     endif()
endif (MSVC)

option(GRINPP_TESTS "Build tests" true)
if(GRINPP_TESTS)
    add_subdirectory(tests)
endif(GRINPP_TESTS)
