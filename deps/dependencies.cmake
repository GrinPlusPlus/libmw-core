#include(sqlite3)
#include(minizip)
#include(jsoncpp)
#include(zlib)
#include(rocksdb)
#include(croaring)
#include(civetweb)
#include(uint128)
#include(ctaes)
#include(scrypt)
#include(secp256k1-zkp)
#include(sha3)
#add_subdirectory(deps/cuckoo)
#add_subdirectory(deps/cppcodec)
#add_subdirectory(deps/ed25519-donna)

if(NOT BUILD_DIR)
    set(BUILD_DIR build)
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS_ORIG "${CMAKE_CXX_FLAGS}")
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W2" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W2")
    endif()
endif()

# spdlog
if (WIN32)
    #add_definitions(-DSPDLOG_WCHAR_TO_UTF8_SUPPORT)
endif()
add_definitions(-DSPDLOG_WCHAR_FILENAMES)
include_directories(${CMAKE_CURRENT_LIST_DIR}/spdlog/include)

# mio (memmap)
include_directories(${CMAKE_CURRENT_LIST_DIR}/mio/include)

# caches
include_directories(${CMAKE_CURRENT_LIST_DIR}/caches/include)

# nlohmann JSON
include_directories(${CMAKE_CURRENT_LIST_DIR}/nlohmann/include)

# Reproc++
set(REPROC++ ON)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/reproc ${BUILD_DIR}/reproc)
include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/reproc/reproc++/include
    ${CMAKE_CURRENT_BINARY_DIR}/deps/reproc/reproc++/include
)

# fmt
add_definitions(-DUNICODE)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/fmt ${BUILD_DIR}/fmt)
include_directories(${CMAKE_CURRENT_LIST_DIR}/fmt/include)

# ghc
include_directories(${CMAKE_CURRENT_LIST_DIR}/ghc/include)

# optional
include_directories(${CMAKE_CURRENT_LIST_DIR}/optional/include)

# asio
add_definitions(-DASIO_STANDALONE)
include_directories(${CMAKE_CURRENT_LIST_DIR}/asio/include)

# crypto
include_directories(${CMAKE_CURRENT_LIST_DIR}/crypto/include)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/crypto ${BUILD_DIR}/crypto)

# civetweb
include_directories(${CMAKE_CURRENT_LIST_DIR}/civetweb/include)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/civetweb ${BUILD_DIR}/civetweb)

# leveldb # TODO: Add Snappy dependency
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/leveldb-1.22 ${BUILD_DIR}/leveldb)
include_directories(${CMAKE_CURRENT_LIST_DIR}/leveldb-1.22/include)

# secp256k1-zkp
include_directories(${CMAKE_CURRENT_LIST_DIR}/secp256k1-zkp/include)

if(WIN32)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/secp256k1-zkp/libsecp256k1-config.h ${CMAKE_CURRENT_LIST_DIR}/secp256k1-zkp/src/libsecp256k1-config.h COPYONLY)
else()
    include_directories(${GOBJECT_INCLUDE_DIR})
    configure_file(${CMAKE_CURRENT_LIST_DIR}/secp256k1-zkp/libsecp256k1-config-nix.h ${CMAKE_CURRENT_LIST_DIR}/secp256k1-zkp/src/libsecp256k1-config.h COPYONLY)
endif()

add_library(secp256k1-zkp STATIC ${CMAKE_CURRENT_LIST_DIR}/secp256k1-zkp/src/secp256k1.c)
target_compile_definitions(secp256k1-zkp PRIVATE HAVE_CONFIG_H SECP256K1_BUILD)
target_include_directories(secp256k1-zkp PRIVATE ${CMAKE_CURRENT_LIST_DIR}/secp256k1-zkp ${CMAKE_CURRENT_LIST_DIR}/secp256k1-zkp/src)



if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_ORIG}")
endif()