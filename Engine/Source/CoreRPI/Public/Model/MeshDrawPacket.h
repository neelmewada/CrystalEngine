#pragma once

namespace CE::RPI
{
    class Material;
    class Scene;

    //! @brief Maintains the draw packet for a particular mesh and other related data.
    class CORERPI_API MeshDrawPacket
    {
    public:

        MeshDrawPacket() = default;

        ~MeshDrawPacket();

        MeshDrawPacket(ModelLod* modelLod, u32 modelLodMeshIndex, RHI::ShaderResourceGroup* objectSrg, RPI::Material* material);

        void Update(RPI::Scene* scene, bool forceUpdate = false);

        void SetStencilRef(u8 stencilRef) { this->stencilRef = stencilRef; }

    private:
        void DoUpdate(RPI::Scene* scene);

        RHI::DrawPacket* drawPacket = nullptr;

        RPI::ModelLod* modelLod = nullptr;

        //! @brief Index of this SubMesh in the modelLod
        u32 modelLodMeshIndex = 0;

        RHI::ShaderResourceGroup* objectSrg = nullptr;

        FixedArray<RHI::ShaderResourceGroup*, RHI::DrawPacketBuilder::DrawItemCountMax> perDrawSrgs{};
        
        RPI::Material* material = nullptr;

        RHI::DrawListMask drawListFilter{};

        u8 stencilRef = 0;

        bool needsUpdate = true;

    };

    using MeshDrawPacketList = Array<MeshDrawPacket>;
    using MeshDrawPacketsByLod = FixedArray<MeshDrawPacketList, RPI::Limits::MaxLodCount>;
    
} // namespace CE::RPI
