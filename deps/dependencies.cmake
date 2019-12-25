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
#add_definitions(-DSPDLOG_WCHAR_FILENAMES)

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