<br>
<img width=200" align="left" src="https://github.com/Pikachuxxxx/Razix/blob/master/Branding/RazixLogo.png"> <h1> Razix Engine </h1>
Razix is a High Performance Research Engine for production pipeline with emphasis on experimenting with different rendering techniques. Razix supports Windows, Mac, Linux, PSVita and PS3 systems.

 ---

|:warning: WARNING:Currently Razix is WIP and the renderer is undergoing major design overhaul hence nothing will make sense, check the RoadMap below for release information |
| --- |

# About
Cross-Platform 2D and 3D engine with multi render API support (OpenGL, Vulkan DirectX 11, GXM, GCM, GNM and GNMX). Supports a wide range of Renders with extreme emphasis on scene optimization and implementing state-of-the art rendering techniques. The engine architecture supports a very educational and optimized design.

View the [Trello Board](https://trello.com/b/yvWKH1Xr/razix-engine) and the [Architecture notes](https://drive.google.com/file/d/1y5ZFf-h02z3cx6WmUzR8giKScvORzmwx/view?usp=sharing)

# Features
- Support for Windows, Linux, macOS, PSVita and PS3 systems.
- Support for OpenGL, Vulkan DirectX 11, GXM, GCM.
- Framegraph based on EA's frostbite engine (currently code-driven) **WIP Node Editor**
| Node based <br /> Frame Graph Editor  | <img src="./Branding/DemoImages/ReleaseNotes/Frame_Graph_Editor_WIP.png" width="200"> |
| ------------- | ------------- |
- 3D audio using OpenAL
- Complete Deferred pipeline + Bindless resources + nanite-like renderers(Future support for customizable render graph editor for complete control).
- Level Editor & tools built using QT
- Multi Physics engine support.
- 3D physics using Jolt.
- Basic lua scripting support for entities.
- Extremely detailed profiling using Tracy and RenderDoc integrated
- Supports GLSL, HLSL shading languages to create custom materials + Custom Razix Shaders files for easy compilation
- Resource Pool base management and custom asset format
| Resource Viewer | <img src="./Branding/DemoImages/ReleaseNotes/ResourceViewerPreview.png" width="200"> |
| ------------- | ------------- |
- Data drive pipeline architecture - **WIP**
- Custom [Razix STL](https://github.com/Pikachuxxxx/RZSTL) and [Razix Memory](https://github.com/Pikachuxxxx/RazixMemory)


# Tools
Razix also provides a rich set of GUI and Commmand Line [**Tools**](https://github.com/Pikachuxxxx/Razix/tree/master/Tools) along with the Editor for various purposes such as
- [**Razix Asset Packer**](https://github.com/Pikachuxxxx/RazixAssetPacker) - **CLI Tool + API for Editor** - Export 3D Models into custom razix format Meshes and Materials
- [**Razix Code Editor**](https://github.com/Pikachuxxxx/RazixCodeEditor) - Shaders and Scripts code editor for Razix Engine (Sony ATF)

### TODO
    - Blender exporter and Importer for model and navmesh formats
    - Blender Integration
    - Script debugger for Lua

# RoadMap

## V 1.0.0 - RC
![](./Docs/Architecture/RazixEngine-RoadMap-V1.0.0RC.png)

# Contributing
More information will be update on creating PRs and other ways to contribute to Razix Engine.
### Style Guide
Check the [style guide](./Docs/RazixStyleGuide.md) for Razix Style enforcements

# Screenshots/Demos
![Editor](./Branding/DemoImages/ReleaseNotes/Razix_Editor_CornellBox_PBR_demo.png)
![Editor](./Branding/DemoImages/ReleaseNotes/Razix_Engine_Volumetric_Clouds.png)

# Architecture

#### :warning: Still a work in progress
![](./Docs/Architecture/RazixEngine-Architecture-V.1.0.0.RC.png)

**ex. Lua Scriping API cheatsheet** ![Lua Scriping API cheatsheet](./Docs/RazixLuaScriptingAPICheatSheet.png)

**For individual module architecture and documentation check the Docs folder or check the individual folders for a detailed description
ex. Core Systems ![Core Systems](./Docs/Architecture/RazixEngine-CoreSystems.png)**

# Changelog and Release Notes

[Click to view changelog](./Docs/CHANGELOG.md)

[Click to view ReleaseNotes](./Docs/ReleaseNotes.md)

# License
Razix Engine is distributed under Apache 2.0 license. [LICENSE](https://github.com/Pikachuxxxx/Razix/blob/master/LICENSE)
