set(TARGET_NAME Models_Tests)

file(GLOB SOURCE_CODE
    "*.cpp"
	"crypto/*.cpp"
)

add_executable(${TARGET_NAME} ${SOURCE_CODE})