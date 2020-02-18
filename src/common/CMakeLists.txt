set(TARGET_NAME Common)

file(GLOB SOURCE_CODE
	"LoggerImpl.cpp"
	"ThreadManagerImpl.cpp"
)

if(MW_STATIC)
	add_library(${TARGET_NAME} STATIC ${SOURCE_CODE})
else(MW_STATIC)
	add_library(${TARGET_NAME} SHARED ${SOURCE_CODE})
endif(MW_STATIC)

target_compile_definitions(${TARGET_NAME} PRIVATE MW_COMMON)