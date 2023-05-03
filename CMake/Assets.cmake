
include_guard(GLOBAL)

# Add assets
function(ce_add_assets NAME)
    set(options GENERATED)
    set(oneValueArgs OUTPUT_DIRECTORY)
    set(multiValueArgs ASSET_FILES)

    cmake_parse_arguments(ce_add_assets "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # OUTPUT_DIRECTORY
    set(OUTPUT_DIRECTORY "${CE_OUTPUT_DIR}/${ce_add_assets_OUTPUT_DIRECTORY}")

    # ASSET_FILES

    set(ASSET_FILES "")
    foreach(asset_file_cmake ${ce_add_assets_ASSET_FILES})
        include(${asset_file_cmake})
        list(APPEND ASSET_FILES ${FILES})
    endforeach()

    set(DEST_ASSET_FILES "")
    set(COPY_COMMANDS "")

    # Commands
    
    foreach(asset_file ${ASSET_FILES})
        cmake_path(RELATIVE_PATH asset_file OUTPUT_VARIABLE dest_asset_file_rel)
        set(dest_asset_file "${OUTPUT_DIRECTORY}/${dest_asset_file_rel}")
        list(APPEND DEST_ASSET_FILES ${dest_asset_file})

        cmake_path(GET dest_asset_file PARENT_PATH dest_asset_file_parent)

        add_custom_command(OUTPUT ${dest_asset_file}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${asset_file}" "${dest_asset_file_parent}"
            DEPENDS ${asset_file}
            VERBATIM
        )
    endforeach()

    add_custom_target(${NAME}
        DEPENDS ${DEST_ASSET_FILES}
        VERBATIM
        SOURCES ${ASSET_FILES}
    )

    ce_group_sources_by_folder(${NAME})

endfunction()

