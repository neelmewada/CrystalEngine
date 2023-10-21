
include_guard(GLOBAL)

ce_set(VERSION_MAJOR 0)
ce_set(VERSION_MINOR 1)
ce_set(VERSION_PATCH 0)
ce_set(VERSION_BUILD 1)

add_compile_definitions(CE_VERSION_MAJOR=${VERSION_MAJOR})
add_compile_definitions(CE_VERSION_MINOR=${VERSION_MINOR})
add_compile_definitions(CE_VERSION_PATCH=${VERSION_PATCH})
add_compile_definitions(CE_VERSION_BUILD=${VERSION_BUILD})

