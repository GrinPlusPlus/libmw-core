set(TARGET_NAME Models_Tests)

file(GLOB SOURCE_CODE
    "*.cpp"
	"crypto/*.cpp"
	"block/*.cpp"
	"tx/*.cpp"
	"net/*.cpp"
	"net/rpc/*.cpp"
)

add_executable(${TARGET_NAME} ${SOURCE_CODE})
add_dependencies(${TARGET_NAME} Core::Crypto)
target_link_libraries(${TARGET_NAME} fmt Core::Crypto)