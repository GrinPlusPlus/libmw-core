set(TARGET_NAME File)

file(GLOB SOURCE_CODE
	"*.cpp"
)

add_library(${TARGET_NAME} STATIC ${SOURCE_CODE})
add_library(Core::${TARGET_NAME} ALIAS ${TARGET_NAME})

add_dependencies(${TARGET_NAME} Core::Common Core::Traits)
target_link_libraries(${TARGET_NAME} PUBLIC Core::Common Core::Traits)