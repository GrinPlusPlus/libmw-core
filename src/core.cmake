
if(NOT BUILD_DIR)
    set(BUILD_DIR build)
endif()

if (WIN32)
    add_definitions(-DMW_ENABLE_WSTRING -DUNICODE -DNOMINMAX)
endif()

if (MW_STATIC)
    add_definitions(-DMW_STATIC)
endif()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/common ${BUILD_DIR}/core/common)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/db ${BUILD_DIR}/core/db)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/file ${BUILD_DIR}/core/file)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/net ${BUILD_DIR}/core/net)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/crypto ${BUILD_DIR}/core/crypto)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/traits ${BUILD_DIR}/core/traits)