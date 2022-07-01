cmake_minimum_required(VERSION 3.22)

project(VoxEngine)

set(CMAKE_CXX_STANDARD 17)
set(VK_SDK $ENV{VK_SDK}) # VK_SDK needs to be set up in your system's environment variables

if (APPLE)
    # Be sure to set 'export HOMEBREW ...' in ~/.zshrc on macOS
    # Because on newer macs (m1), homebrew installs on /opt/homebrew/ directory
    set(HOMEBREW_DIR $ENV{HOMEBREW})

    # Signing related
    set(APPLE_DEV_CERT_NAME 8UUBKX96BH)
    set(APPLE_DEV_TEAM_ID 6U5D4S4U6V)
endif()

if (WIN32)
    set(SDL_DIR $ENV{SDL2})
endif()

if (UNIX)
    set(VK_BIN ${VK_SDK}/bin)
    set(INCLUDE_DIR ${VK_SDK}/include)
    set(LIB_DIR ${VK_SDK}/lib)
    if (APPLE)
        set(INCLUDE_DIR ${INCLUDE_DIR} ${HOMEBREW_DIR}/include)
        set(LIB_DIR ${LIB_DIR} ${HOMEBREW_DIR}/lib)
        set(PLATFORM_MACOS 1)
        add_compile_definitions(PLATFORM_MACOS=1)
    else()
        set(INCLUDE_DIR ${INCLUDE_DIR} /usr/local/include)
        set(LIB_DIR ${LIB_DIR} /usr/local/lib)
        set(PLATFORM_LINUX 1)
        add_compile_definitions(PLATFORM_LINUX=1)
    endif()
elseif (WIN32)
    set(VK_BIN ${VK_SDK}/Bin)
    set(INCLUDE_DIR ${VK_SDK}/Include ${SDL_DIR}/include)
    set(LIB_DIR ${VK_SDK}/Lib ${SDL_DIR}/lib/${CMAKE_BUILD_TYPE})
    set(PLATFORM_WIN32 1)
    add_compile_definitions(PLATFORM_WIN32=1)
endif()

add_subdirectory(Engine)
add_subdirectory(RunnerTest)
