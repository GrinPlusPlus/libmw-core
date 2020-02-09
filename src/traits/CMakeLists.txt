set(TARGET_NAME Traits)

file(GLOB SOURCE_CODE
	"Serializable.cpp"
)

add_library(${TARGET_NAME} STATIC ${SOURCE_CODE})

add_dependencies(${TARGET_NAME} Common)
target_link_libraries(${TARGET_NAME} PUBLIC Common)