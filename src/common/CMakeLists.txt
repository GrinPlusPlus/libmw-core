set(TARGET_NAME Common)

file(GLOB SOURCE_CODE
	"LoggerImpl.cpp"
	"ThreadManagerImpl.cpp"
)

#if(GRINPP_STATIC)
	add_library(${TARGET_NAME} STATIC ${SOURCE_CODE})
#else(GRINPP_STATIC)
#	add_library(${TARGET_NAME} SHARED ${SOURCE_CODE})
#endif(GRINPP_STATIC)

target_compile_definitions(${TARGET_NAME} PRIVATE MW_COMMON)