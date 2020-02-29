set(TARGET_NAME File)

file(GLOB SOURCE_CODE
	"*.cpp"
)

if(MW_STATIC)
	add_library(${TARGET_NAME} STATIC ${SOURCE_CODE})
else(MW_STATIC)
	add_library(${TARGET_NAME} SHARED ${SOURCE_CODE})
endif(MW_STATIC)

add_library(Core::${TARGET_NAME} ALIAS ${TARGET_NAME})

target_compile_definitions(${TARGET_NAME} PRIVATE MW_NET)

add_dependencies(${TARGET_NAME} Core::Common Core::Traits)
target_link_libraries(${TARGET_NAME} PUBLIC Core::Common Core::Traits)