#include "CoreRPI.h"

namespace CE::RPI
{

	MeshDrawPacket::~MeshDrawPacket()
	{
		delete drawPacket; drawPacket = nullptr;
	}

	MeshDrawPacket::MeshDrawPacket(ModelLod* modelLod, u32 modelLodMeshIndex, RHI::ShaderResourceGroup* objectSrg, RPI::Material* material)
		: modelLod(modelLod), modelLodMeshIndex(modelLodMeshIndex), objectSrg(objectSrg), material(material)
	{
		
	}

	void MeshDrawPacket::Update(RPI::Scene* scene, bool forceUpdate)
	{
		if (needsUpdate || forceUpdate)
		{
			DoUpdate(scene);
		}
	}

	void MeshDrawPacket::DoUpdate(RPI::Scene* scene)
	{
		needsUpdate = false;

		Mesh* mesh = modelLod->GetMesh(modelLodMeshIndex);

		if (!material)
		{
			CE_LOG(Error, All, "MeshDrawPacket does not have material set!");
			return;
		}

		if (drawPacket != nullptr)
		{
			delete drawPacket; drawPacket = nullptr;
		}

		RHI::DrawPacketBuilder builder{};

		builder.SetDrawArguments(mesh->drawArguments);

		builder.AddShaderResourceGroup(objectSrg);
		builder.AddShaderResourceGroup(material->GetShaderResourceGroup());

		
	}

} // namespace CE::RPI
