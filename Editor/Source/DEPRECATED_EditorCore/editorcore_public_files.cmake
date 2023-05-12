
file(GLOB_RECURSE FILES "Public/*.h" "Public/*.cpp" "Public/*.qrc" "Public/*.ui")

ce_filter_platform_files(FILES)
