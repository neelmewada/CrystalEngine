#pragma once

namespace CE
{
    constexpr u32 MaxUVChannelCount = 4;

    enum class CMSceneLoader
    {
        None = 0,
        FBX
    };
    ENUM_CLASS(CMSceneLoader);

    enum class CMObjectType
    {
        Root = 0,
        Geometry,
        Shape,
        Material,
        Mesh,
        Texture,
        Camera,
        Light,
    };
    ENUM_CLASS(CMObjectType);

    struct CMObject
    {
        Matrix4x4 localTransform{};
        Matrix4x4 globalTransform{};

        Vec3 localPosition{};
        Vec3 localRotation{};
        Vec3 localScaling{};

        virtual CMObjectType GetObjectType() const { return CMObjectType::Root; }
    };

    enum class CMLightType
    {
        Point = 0,
        Directional,
        Spot,
    };

    enum class CMDecayType
    {
        NoDecay = 0,
        Linear,
        Quadratic,
        Cubic,
        COUNT
    };

    struct CMLight : CMObject
    {
        virtual CMObjectType GetObjectType() const override final { return CMObjectType::Light; }

        CMLightType lightType = CMLightType::Point;
        bool castLight = true;
        Color color = { 1, 1, 1, 1};
        f64 intensity = 0;

        f64 innerAngle = 0.0;
        f64 outerAngle = 45.0;

        f64 fog = 50;

        CMDecayType decayType = CMDecayType::Quadratic;
        f64 decayStart = 1.0;

        bool enableNearAttenuation = false;
        f64 nearAttenuationStart = 0.0;
        f64 nearAttenuationEnd = 0.0;

        bool enableFarAttenuation = false;
        f64 farAttenuationStart = 0.0;
        f64 farAttenuationEnd = 0.0;

        bool castShadows = true;
        Color shadowColor = { 0, 0, 0, 0 };
    };

    struct CMMeshPartition
    {
        Array<u32> indices{};
        u32 materialIndex = 0;
    };

    struct CMMaterial
    {

    };

    struct CMMesh
    {
        Array<Vec3> positions{};
        Array<Vec3> normals{};
        Array<Color> colors{};
        Array<Vec3> tangents{};
        StaticArray<Array<Vec2>, MaxUVChannelCount> uvs{};
        Array<CMMeshPartition> submeshes{};
        Array<CMMaterial> materials{};
    };

    class COREMESH_API CMScene final
    {
    private:
        CMScene();

    public:

        ~CMScene();

    private:

        CMSceneLoader loader = CMSceneLoader::None;

        Array<CMLight> lights{};
        Array<CMMesh> meshes{};

        friend class FbxImporter;
    };

} // namespace CE
