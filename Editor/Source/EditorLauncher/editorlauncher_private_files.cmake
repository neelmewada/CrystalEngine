
file(GLOB_RECURSE FILES "Private/*.h" "Private/*.cpp")

list(APPEND FILES "appicon.rc")

ce_exclude_platform_files(FILES)
