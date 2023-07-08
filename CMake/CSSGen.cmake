
set(GENERATE_CSS FALSE)
set(CSS_SOURCE "#pragma once\n\nstatic const CE::String css = R\"(")

foreach(src_file ${SOURCES})
    cmake_path(GET src_file EXTENSION LAST_ONLY src_file_ext)
    cmake_path(GET src_file STEM LAST_ONLY src_file_stem)
    if(${src_file_ext} STREQUAL ".css")
        set(GENERATE_CSS TRUE)
        file(READ ${src_file} css_src)
        string(APPEND CSS_SOURCE "\n${css_src}\n")
    endif()
endforeach()

string(APPEND CSS_SOURCE "\n)\";\n")

if(GENERATE_CSS)
    file(WRITE "${OUT_DIR}/Style.css.h" "${CSS_SOURCE}")
endif()


