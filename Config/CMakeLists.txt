cmake_minimum_required(VERSION 3.20)

project(Config)

file(GLOB_RECURSE CONFIG_FILES "*.ini")

ce_add_configs(${PROJECT_NAME}
    CONFIGS
        ${CONFIG_FILES}
    FOLDER "Config"
)
