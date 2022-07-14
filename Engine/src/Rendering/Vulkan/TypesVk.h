#pragma once

namespace Vox
{

enum ShaderResourceVariableBaseType
{
    Struct, UInt, Int, Sampler2D, Sampler3D, Float, Vec2, Vec3, Vec4, Mat2, Mat3, Mat4,
    UVec2, UVec3, UVec4, IVec2, IVec3, IVec4
};

enum ShaderResourceVariableType
{
    UniformBuffer, StorageBuffer, SampledImage2D
};

struct ShaderBlockMember
{
    Uint32 offset;
    Uint32 size;
    std::string name;
    ShaderResourceVariableBaseType baseType;
    bool isArray;
};


// Indices of Queue families if they exist
struct QueueFamilyInfo
{
    int graphicsFamilyIndex = -1;
    int presentationFamilyIndex = -1;
    int computeFamilyIndex = -1;

    // Check if graphics queue family are valid
    bool IsValidGraphicsFamily()
    { return graphicsFamilyIndex >= 0; }

    bool IsValidPresentationFamily()
    { return presentationFamilyIndex >= 0; }

    bool IsValidComputeFamily()
    { return computeFamilyIndex >= 0; }
};

// Struct containing compatability info of surface for Swap Chain creation
struct SurfaceCompatInfo
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;   // Surface properties: image size, extent, etc
    std::vector<VkSurfaceFormatKHR> surfaceFormats;  // Surface image supported formats
    std::vector<VkPresentModeKHR> presentationModes; // How images should be presented to the screen
};

}
