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

		ShaderCollection* shaderCollection = material->GetShaderCollection();
		if (shaderCollection == nullptr)
			return;

		RHI::DrawPacketBuilder builder{};

		builder.SetDrawArguments(mesh->drawArguments);

		builder.AddShaderResourceGroup(objectSrg);
		builder.AddShaderResourceGroup(material->GetShaderResourceGroup());

		for (int i = 0; i < perDrawSrgs.GetSize(); ++i)
		{
			delete perDrawSrgs[i]; perDrawSrgs[i] = nullptr;
		}
		perDrawSrgs.Clear();

		for (int i = 0; i < shaderCollection->GetSize(); i++)
		{
			const ShaderCollection::Item& shaderItem = shaderCollection->At(i);

			DrawListTag drawListTag = shaderItem.drawListOverride;
			if (!drawListTag.IsValid())
			{
				drawListTag = shaderItem.shader->GetDrawListTag();
			}

			if (!shaderItem.enabled || !drawListTag.IsValid())
			{
				continue;
			}

			RPI::Shader* shader = shaderItem.shader;

			// TODO: Implement dynamic shader variant selection
			RPI::ShaderVariant* variant = shader->GetVariant(shader->GetDefaultVariantIndex());

			const ShaderResourceGroupLayout& drawSrgLayout = variant->GetSrgLayout(RHI::SRGType::PerDraw);

			RHI::ShaderResourceGroup* perDrawSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(drawSrgLayout);
			perDrawSrgs.Add(perDrawSrg);

			DrawPacketBuilder::DrawItemRequest drawItem{};
			drawItem.drawFilterMask = DrawFilterMask::ALL;
			drawItem.drawItemTag = drawListTag;
			drawItem.uniqueShaderResourceGroups.Add(perDrawSrg);
			drawItem.stencilRef = stencilRef;
			// TODO: Get correct pipeline based on MSAA, color formats, etc
			drawItem.pipelineState = variant->GetPipeline();

			builder.AddDrawItem(drawItem);
		}
	}

} // namespace CE::RPI
