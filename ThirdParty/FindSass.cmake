#.rst:
# FindSass
# -----------
#
# Try to find Sass compiler.
#
# If the Sass compiler executable is not in your PATH, you can provide
# an alternative name or full path location with the ``Sass_EXECUTABLE`` variable.
# In this case, do not forget to set ``Sass_COMPILER_TYPE`` variable also.
#
# This will define the following variables:
#
# ``Sass_FOUND``
#     True if sass is available.
#
# ``Sass_EXECUTABLE``
#     The Sass compiler executable.
#
# ``Sass_COMPILER_TYPE``
#     Sass compiler type: ``sass`` or ``sassc``.
#
# If ``Sass_FOUND`` is TRUE, it will also define the following imported
# target:
#
# ``Sass::Sass``
#     The Sass compiler executable.
#

#=============================================================================
# SPDX-FileCopyrightText: 2018 Alexander Kernozhitsky <sh200105@mail.ru>
#
# SPDX-License-Identifier: BSD-3-Clause
#=============================================================================

set_package_properties(Sass PROPERTIES
  DESCRIPTION "SASS compiler"
  URL "https://sass-lang.com/"
  PURPOSE "Required for building GTK themes")

find_program(Sass_EXECUTABLE NAMES sassc)

if(Sass_EXECUTABLE)
    if(NOT Sass_COMPILER_TYPE)
      set(Sass_COMPILER_TYPE sassc)
    endif()
else()
    find_program(Sass_EXECUTABLE NAMES sass)
    set(Sass_COMPILER_TYPE sass)
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Sass
    FOUND_VAR
        Sass_FOUND
    REQUIRED_VARS
        Sass_EXECUTABLE
)
mark_as_advanced(Sass_EXECUTABLE)

if (Sass_FOUND)
    if (NOT TARGET Sass::Sass)
        add_executable(Sass::Sass IMPORTED)
        set_target_properties(Sass::Sass PROPERTIES
            IMPORTED_LOCATION "${Sass_EXECUTABLE}"
        )
    endif()
endif()
