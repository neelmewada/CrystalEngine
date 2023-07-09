
file(GLOB_RECURSE FILES "Private/*.h" "Private/*.cpp" "Private/*.qrc" "Private/*.ui" "Private/*.mm")

ce_filter_platform_files(FILES)

