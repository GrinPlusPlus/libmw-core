set(TARGET_NAME Database)

file(GLOB SOURCE_CODE
	"*.cpp"
)

if(MW_STATIC)
	add_library(${TARGET_NAME} STATIC ${SOURCE_CODE})
else(MW_STATIC)
	add_library(${TARGET_NAME} SHARED ${SOURCE_CODE})
endif(MW_STATIC)

add_library(Core::${TARGET_NAME} ALIAS ${TARGET_NAME})

target_compile_definitions(${TARGET_NAME} PRIVATE MW_DATABASE)

add_dependencies(${TARGET_NAME} leveldb Core::Common Core::Traits Core::Crypto)
target_link_libraries(${TARGET_NAME} PUBLIC leveldb Core::Common Core::Traits Core::Crypto)