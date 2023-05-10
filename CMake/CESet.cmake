
get_directory_property(CE_PARENT_SCOPE PARENT_DIRECTORY)
if(CE_PARENT_SCOPE)
    set(CE_PARENT_SCOPE ${CE_PARENT_SCOPE} PARENT_SCOPE)
endif()

#! ce_set: convenient function to set and export variable to the parent scope in scenarios
#          where CMAKE_SOURCE_DIR != CMAKE_CURRENT_LIST_DIR (e.g. when the engine's cmake 
#          files are included from a project)
#
macro(ce_set name)
    set(${name} "${ARGN}")
    if(CE_PARENT_SCOPE)
        set(${name} "${ARGN}" PARENT_SCOPE)
    endif()
endmacro()

macro(ce_set_parent name)
    set(${name} "${ARGN}")
    set(${name} "${ARGN}" PARENT_SCOPE)
endmacro()

