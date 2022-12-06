#!/bin/sh

# Setup variables

base_dir=$(pwd)
cmake_subdir=$2

cd $1

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    platform="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    platform="mac"
elif [[ "$OSTYPE" == "cygwin" ]]; then
    platform="windows"
elif [[ "$OSTYPE" == "msys" ]]; then
    platform="windows"
elif [[ "$OSTYPE" == "win32" ]]; then
    platform="windows"
elif [[ "$OSTYPE" == "freebsd"* ]]; then
    platform="linux"
else
    echo "Unknown Operating System"
    exit 1
fi


PrintBuildingProject() {
    echo ""
    echo "------------------------------"
    echo "Building $1"
    echo "------------------------------"
}


CleanCMake() {
    rm -rf cmake-build
    mkdir cmake-build
    rm -rf Debug Development Release
    mkdir Debug
    mkdir Development
    mkdir Release
}


BuildCMake() {
    PrintBuildingProject $1

    local BuildType="Debug"
    local Symbols="On"
    local DbgSuffix=""
    local CxxFlags=""
    local LdFlags=""
    if [ $1 == "Development" ];
    then
        BuildType="RelWithDebInfo"
    elif [ $1 == "Release" ];
    then
        BuildType="MinSizeRel"
        Symbols="Off"
    else
        BuildType="Debug"
        if [ $platform == "windows" ];
        then
            DbgSuffix="d"
        fi
    fi

    CMakeBuildSystem="-G Ninja"
    CMakeBuildFlags=""
    CmakeRootDir=$cmake_subdir

    if [ $platform == "windows" ]; then
        CxxFlags="${CxxFlags} /MD${DbgSuffix}"
        CMakeBuildSystem=""
        CMakeBuildFlags="--config ${BuildType}"
    fi

    MacFlags=""

    if [ $platform == "mac" ]; then
        MacFlags="CMAKE_OSX_ARCHITECTURES=arm64;x86_64"
    fi

    echo "Build Type: " $1

    rm -rf cmake-build
    mkdir cmake-build
    
    cd cmake-build

    cmake -E env CXXFLAGS="${CxxFlags}" ${MacFlags} cmake ${CMakeBuildSystem} -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=$BuildType ../${CmakeRootDir}
    
    cmake -E env CXXFLAGS="${CxxFlags}" cmake --build . -j 8 ${CMakeBuildFlags}
    
    if [[ $platform == "windows" ]]; then
        echo "Copying windows files to " $1
        find . -type f -name "*.lib"
        find . -type f -name "*.exe" -exec cp '{}' ../$1 \;
        find . -type f -name "*.lib" -exec cp '{}' ../$1 \;
        find . -type f -name "*.dll" -exec cp '{}' ../$1 \;
        if [ $Symbols == "On" ]; then
            find . -type f -name "*.pdb" -exec cp '{}' ../$1 \;
            find . -type f -name "*.exp" -exec cp '{}' ../$1 \;
            find . -type f -name "*.idb" -exec cp '{}' ../$1 \;
        fi
    else
        echo "Copying Unix files"
        find . -perm +111 -type f -exec cp '{}' ../$1 \;
        find . -perm +111 -type l -exec cp '{}' ../$1 \;
        find . -type f -name "*.dylib" -exec cp '{}' ../$1 \;
        find . -type f -name "*.a" -exec cp '{}' ../$1 \;
        find . -type f -name "*.so" -exec cp '{}' ../$1 \; # linux uses .so for dynamic libs
    fi

    cd ..
}

# Build flecs

CleanCMake
BuildCMake Debug
BuildCMake Development
BuildCMake Release

cd $base_dir