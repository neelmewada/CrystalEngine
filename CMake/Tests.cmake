
if(NOT ${PAL_TRAIT_BUILD_TESTS_SUPPORTED})
    return()
endif()

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)

add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)


function(ce_add_test NAME)
    if(NOT ${CE_BUILD_TESTS})
        return()
    endif()
    

    set(options AUTORTTI)
    set(oneValueArgs TARGET FOLDER)
    set(multiValueArgs SOURCES BUILD_DEPENDENCIES)
    set(include_dirs "${CMAKE_CURRENT_SOURCE_DIR}")

    cmake_parse_arguments(ce_add_test "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    add_executable(${NAME} ${ce_add_test_SOURCES})

    set_target_properties(${NAME} 
        PROPERTIES
            FOLDER "${ce_add_test_FOLDER}"
    )
    
    target_link_libraries(${NAME}
        PRIVATE
            GTest::GTest
            ${ce_add_test_TARGET}
    )

    target_compile_definitions(${NAME} 
        PRIVATE
            PACKAGE_NAME="/Code/${NAME}"
            MODULE_NAME="${NAME}"
    )

    if(${PAL_PLATFORM_IS_MAC})
        target_link_libraries(${NAME} PRIVATE "c" "c++")
    endif()

    # AUTORTTI

    if(${ce_add_test_AUTORTTI})
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Generated")
        list(APPEND include_dirs "${CMAKE_CURRENT_BINARY_DIR}/Generated")

        add_custom_command(TARGET ${NAME} PRE_BUILD
            COMMAND "AutoRTTI" -m ${NAME} --noapi -d "${CMAKE_CURRENT_SOURCE_DIR}/" -o "${CMAKE_CURRENT_BINARY_DIR}/Generated"
            VERBATIM
        )
    endif()

    target_include_directories(${NAME} 
        PRIVATE ${include_dirs}
    )

    # BUILD_DEPENDENCIES

    set(multiValueArgs PRIVATE PUBLIC INTERFACE TARGETS MACFRAMEWORKS)
    cmake_parse_arguments(ce_add_test_BUILD_DEPENDENCIES "" "" "${multiValueArgs}" ${ce_add_test_BUILD_DEPENDENCIES})

    if(${PAL_PLATFORM_IS_MAC})
        list(APPEND ce_add_test_BUILD_DEPENDENCIES_PRIVATE "c")
        list(APPEND ce_add_test_BUILD_DEPENDENCIES_PRIVATE "c++")
        list(APPEND ce_add_test_BUILD_DEPENDENCIES_PRIVATE "-framework CoreServices")
        if(ce_add_test_BUILD_DEPENDENCIES_MACFRAMEWORKS)
            foreach(framework ${ce_add_test_BUILD_DEPENDENCIES_MACFRAMEWORKS})
                list(APPEND ce_add_test_BUILD_DEPENDENCIES_PRIVATE "-framework ${framework}")
            endforeach()
        endif()
    endif()

    if(ce_add_test_BUILD_DEPENDENCIES_PRIVATE OR ce_add_test_BUILD_DEPENDENCIES_PUBLIC OR ce_add_test_BUILD_DEPENDENCIES_INTERFACE)
        target_link_libraries(${NAME}
            PRIVATE   ${ce_add_test_BUILD_DEPENDENCIES_PRIVATE}
            PUBLIC    ${ce_add_test_BUILD_DEPENDENCIES_PUBLIC}
            INTERFACE ${ce_add_test_BUILD_DEPENDENCIES_INTERFACE}
        )
    endif()

    if(ce_add_test_BUILD_DEPENDENCIES_TARGETS)
        add_dependencies(${NAME} ${ce_add_test_BUILD_DEPENDENCIES_TARGETS})
    endif()

    add_test(${NAME} ${NAME})
    
endfunction()


