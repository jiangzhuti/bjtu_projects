aux_source_directory(. LIB_SRCS)
add_library(libsimplelog SHARED ${LIB_SRCS})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set_target_properties(libsimplelog PROPERTIES OUTPUT_NAME "simplelog")
