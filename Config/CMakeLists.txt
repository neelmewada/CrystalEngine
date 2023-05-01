cmake_minimum_required(VERSION 3.20)

project(Config)

file(GLOB_RECURSE CONFIG_FILES "*.ini")

if(NOT ${PAL_TRAIT_BUILD_TESTS_SUPPORTED})
    list(FILTER ${FILES_LIST} EXCLUDE REGEX "*Test.ini") 
endif()


ce_add_configs(${PROJECT_NAME}
    CONFIGS
        ${CONFIG_FILES}
    FOLDER "Config"
    OUTPUT_DIRECTORY "Config"
)
