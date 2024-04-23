#pragma once

namespace CE::RPI
{
    class Material;

    //! @brief Maintains the draw packet for a particular mesh and other related data.
    class CORERPI_API MeshDrawPacket
    {
    public:

        MeshDrawPacket() = default;

    private:

        RHI::DrawPacket* drawPacket = nullptr;

        RPI::ModelLod* modelLod = nullptr;

        //! @brief Index of this SubMesh in the modelLod
        u32 modelLodMeshIndex = 0;

        RHI::ShaderResourceGroup* objectSrg = nullptr;
        
        RPI::Material* material = nullptr;

        RHI::DrawListMask drawListFilter{};
    };

    using MeshDrawPacketList = Array<MeshDrawPacket>;
    using MeshDrawPacketListByLod = FixedArray<MeshDrawPacketList, RPI::Limits::MaxLodCount>;
    
} // namespace CE::RPI
