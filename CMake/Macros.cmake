
include_guard(GLOBAL)

macro(error)
    message(FATAL_ERROR ${ARGN})
endmacro()

