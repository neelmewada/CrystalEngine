
set(CE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/$<CONFIG>/)

# Set default output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CE_OUTPUT_DIR}/Libraries CACHE PATH "Build directory for static libraries and import libraries")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CE_OUTPUT_DIR}/ CACHE PATH "Build directory for shared libraries")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CE_OUTPUT_DIR}/ CACHE PATH "Build directory for executables")

set(CE_STANDALONE_BINARY_DIR ${CE_OUTPUT_DIR}/Standalone/${PAL_PLATFORM_NAME}/)

