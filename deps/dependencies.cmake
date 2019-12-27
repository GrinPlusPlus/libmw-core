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
#add_definitions(-DASIO_STANDALONE)

if(MSVC)
    set(CMAKE_CXX_FLAGS_ORIG "${CMAKE_CXX_FLAGS}")
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W2" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W2")
    endif()
endif()

# spdlog
add_definitions(-DSPDLOG_WCHAR_FILENAMES)
include_directories(${PROJECT_SOURCE_DIR}/deps/spdlog/include)

# caches
include_directories(${PROJECT_SOURCE_DIR}/deps/caches/include)

# nlohmann JSON
include_directories(${PROJECT_SOURCE_DIR}/deps/nlohmann/include)

# Reproc++
set(REPROC++ ON)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/reproc)
include_directories(
    ${PROJECT_SOURCE_DIR}/deps/reproc/reproc++/include
    ${CMAKE_CURRENT_BINARY_DIR}/deps/reproc/reproc++/include
)

# fmt
add_definitions(-DUNICODE)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/fmt)
include_directories(${PROJECT_SOURCE_DIR}/deps/fmt/include)

# ghc
include_directories(${PROJECT_SOURCE_DIR}/deps/ghc/include)

# crypto
include_directories(${PROJECT_SOURCE_DIR}/deps/crypto/include)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/crypto)

# secp256k1-zkp
include_directories(${PROJECT_SOURCE_DIR}/deps/secp256k1-zkp/include)

if(WIN32)
    configure_file(${PROJECT_SOURCE_DIR}/deps/secp256k1-zkp/libsecp256k1-config.h ${PROJECT_SOURCE_DIR}/deps/secp256k1-zkp/src/libsecp256k1-config.h COPYONLY)
else()
    include_directories(${GOBJECT_INCLUDE_DIR})
    configure_file(${PROJECT_SOURCE_DIR}/deps/secp256k1-zkp/libsecp256k1-config-nix.h ${PROJECT_SOURCE_DIR}/deps/secp256k1-zkp/src/libsecp256k1-config.h COPYONLY)
endif()

add_library(secp256k1-zkp STATIC ${PROJECT_SOURCE_DIR}/deps/secp256k1-zkp/src/secp256k1.c)
target_compile_definitions(secp256k1-zkp PRIVATE HAVE_CONFIG_H SECP256K1_BUILD)
target_include_directories(secp256k1-zkp PRIVATE ${PROJECT_SOURCE_DIR}/deps/secp256k1-zkp)



if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_ORIG}")
endif()