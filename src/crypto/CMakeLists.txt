set(TARGET_NAME Crypto)

file(GLOB SOURCE_CODE
	"AggSig.cpp"
	"Bulletproofs.cpp"
	"ConversionUtil.cpp"
	"Crypto.cpp"
	"Pedersen.cpp"
	"PublicKeys.cpp"
)

if(MW_STATIC)
	add_library(${TARGET_NAME} STATIC ${SOURCE_CODE})
else(MW_STATIC)
	add_library(${TARGET_NAME} SHARED ${SOURCE_CODE})
endif(MW_STATIC)

add_library(Core::${TARGET_NAME} ALIAS ${TARGET_NAME})

target_compile_definitions(${TARGET_NAME} PRIVATE MW_CRYPTO)

add_dependencies(${TARGET_NAME} crypto_deps secp256k1-zkp fmt::fmt Core::Common Core::Traits)
target_link_libraries(${TARGET_NAME} PUBLIC crypto_deps secp256k1-zkp fmt::fmt Core::Common Core::Traits)