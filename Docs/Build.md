# Building

The engine can be built in either of these 2 configurations:
* Editor/Host mode (**required**): Builds the host tools and editor along with the engine run-time. This is the main focus of the engine right now.
* Standalone mode (**not** required): Builds only the engine runtime, but requires access to the built host tools. No need to build this at this time.

Currently, the engine compiles and works on both Windows and Linux (Ubuntu). However, windows is the only primary focus of the enigne.

Although, the Core libraries can be compiled on macOS too.

## Prerequisites

The following is required to be installed to your computer along with environment variables set up.

- Git for windows: This is required for using the bash shell that comes with it. Make sure the folder containing sh.exe `C:\Program Files\Git\bin` is added to the PATH environment variables.
- Vulkan SDK: Minimum required version is **1.3.290**. Older versions won't work because they come with old DxCompiler binaries.
- CMake 3.25 or higher: Use the one that comes with Visual Studio.
- Visual Studio 2022 OR Xcode.
- Visual Studio Code (optional).

### Linux specific requirements

Clang and Clang++ are required. GCC is not supported by the engine! Install clang using:

```sudo apt install clang```

## Cloning

Clone the repo recursively:

```sh
git clone --recursive https://github.com/neelmewada/CrystalEngine.git
git checkout linux-port
git submodule update --init --recursive
```

## Building Editor (required)

Run the following cmake command to make host build from the project's root directory. You can either run the helper script to generate the project, or use cmake manually.

```sh
# Helper script to generate project
bash Scripts/CMakeEditor.sh
```

OR, custom cmake commands:

```sh
# Windows
cmake -B Build/Windows -S . -G "Visual Studio 17 2022" -Wno-dev

#Linux
cmake -B Build/Linux -S . -G "Ninja Multi-Config" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -Wno-dev
```

Now, you can build the editor using the following command:

```sh
# Replace the --build folder to what you have set.
cmake --build Build/Windows --config Debug --target EditorLauncher -Wno-dev
```

### Important Note

The `EditorLauncher` executable launches the Project Browser by default. If you've already created a project and want to launch the project by default on launch, or if the Project Browser fails to launch the project in editor (happens on Linux), then pass the path to your `.cproject` file in the command line arguments when running it through Visual Studio or any other IDE like this:

<p align="center">
    <img src="./Images/EditorLauncherProjectPath.png" width=400>
</p>

## Building standalone (optional, ignore for now)

The standalone build has the GameSystem_Test application, which you can run to play-test the engine in standalone mode.

```sh
cmake -B Build/Windows-Standalone -S . -DCE_STANDALONE=ON -DCE_HOST_BUILD_DIR="<Path To host tools binary dir>" -DCMAKE_SYSTEM_NAME=Windows -Wno-Dev
```

The path to host tools binary directory in our situation is:
`C:/CrystalEngine/Build/Windows/Debug`. `Build/Windows` is the host tools solution directory, and the binary files will be produced in the Debug folder, if you have chosen to build Debug target for host tools.

Note: For now, there is no real use of standalone builds, and they are incomplete yet. You can run the `GameSystem_Test` target in standalone build to do a basic play test of the standalone engine.

