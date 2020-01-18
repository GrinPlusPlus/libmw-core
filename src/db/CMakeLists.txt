set(TARGET_NAME DATABASE)

file(GLOB SOURCE_CODE
	"common/*.cpp"
)

#if(GRINPP_STATIC)
	add_library(${TARGET_NAME} STATIC ${SOURCE_CODE})
#else(GRINPP_STATIC)
#	add_library(${TARGET_NAME} SHARED ${SOURCE_CODE})
#endif(GRINPP_STATIC)

target_compile_definitions(${TARGET_NAME} PRIVATE MW_DATABASE)

add_dependencies(${TARGET_NAME} Common)
target_link_libraries(${TARGET_NAME} PUBLIC Common)