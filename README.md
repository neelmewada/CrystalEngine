# Crystal Engine

A Work-in-progress Vulkan game engine with PBR rendering, FrameGraph based render architecture, and a Widget library that uses CSS & Yoga Layout engine.

PBR Demo            |  Widgets Demo
:-------------------------:|:-------------------------:
![](./Screenshots/IBL%20Demo%20Night.png)  |  ![](./Screenshots/WidgetDemo.gif)

Check more [screenshots below](#screenshots).

## Requirements

Only Windows is supported for now. Although, the Core libraries compile on macOS too.

## Building

Please look at the [Build.md](./Docs/Build.md) to know the steps & dependencies required to build.

## Features

- Modular engine architecture.
- Use HLSL to write shaders for Vulkan.
- Forward PBR rendering with CubeMap IBL.
- Directional shadow maps.
- Multi-threaded job system, used in Asset Processing.
- Asset processing to generate binary assets for use by engine.
- Automatic C++ runtime reflection generation.
- Object serialization: Custom binary format and JSON.
- Render graph based pipeline with automatic dependencies.
- **CrystalWidgets** framework for GUI apps with SDF text & geometry rendering.
- **CrystalWidgets** uses the engine's builtin renderer instead of 3rd party imgui frameworks.

## Modules Overview

The engine is divided into different domains, and each domain can have multiple modules.

#### Core domain
All modules inside the core domain are at the low level of the engine.

* **Core module**: The foundation of the engine. Provides runtime type reflection system, Binary & JSON serialization, containers, Object system, Job System, etc.
* **CoreApplication**: Low level application layer to handle OS application specific things. Uses SDL2 underneath the abstractions.
* **CoreMedia**: Low level image handling and BC1-7 compression.
* **CoreShader**: Low level shader compilation and reflection. Uses DxCompiler & spirv reflect.
* **CoreRHI**: Rendering Hardware Interface. A graphics API abstraction layer that is used for low level GPU operations. Also implements frame graph rendering.
    * **VulkanRHI**: Vulkan implementation of the RHI layer.
* **CoreRPI**: Render Pipeline Interface. Provides a render pipeline architecture layer on top of RHI, allowing engine to build complex render pipeline with many passes.
* **CrystalWidgets**: Widgets library used to build GUI applications with CSS & FlexBox. Uses CoreRPI for 2D rendering.

#### System domain
System domain modules are at high level of the engine.

* **System module**: The main module that contains the high level game engine systems, game framework, etc.
* **GameSystem**: Only for standalone builds. Runtime implementation of System module.

#### Editor domain

There's no actual GUI editor that exists yet. It's only CLI tools like AssetProcessor, AutoRTTI, ResourceCompiler, etc.

* **EditorCore**: Implements Asset processing and serves as the foundation of the editor & host tools.
* **EditorSystem**: Host/editor implementation of the System module.

## Screenshots

Project browser window:

![](./Screenshots/ProjectBrowser.png)




