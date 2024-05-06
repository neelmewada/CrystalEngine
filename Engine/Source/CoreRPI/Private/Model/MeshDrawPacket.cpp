#include "CoreRPI.h"

namespace CE::RPI
{

	MeshDrawPacket::~MeshDrawPacket()
	{
		for (int i = 0; i < perDrawSrgs.GetSize(); ++i)
		{
			delete perDrawSrgs[i]; perDrawSrgs[i] = nullptr;
		}
		perDrawSrgs.Clear();

		delete drawPacket; drawPacket = nullptr;
	}

	MeshDrawPacket::MeshDrawPacket(ModelLod* modelLod, u32 modelLodMeshIndex, RHI::ShaderResourceGroup* objectSrg, RPI::Material* material)
		: modelLod(modelLod), modelLodMeshIndex(modelLodMeshIndex), objectSrg(objectSrg), material(material)
	{
		
	}

	bool MeshDrawPacket::Update(RPI::Scene* scene, bool forceUpdate)
	{
		if (needsUpdate || forceUpdate)
		{
			DoUpdate(scene);
			return true;
		}

		return false;
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

		material->FlushProperties();

		builder.AddShaderResourceGroup(objectSrg);

		for (int i = 0; i < perDrawSrgs.GetSize(); ++i)
		{
			delete perDrawSrgs[i]; perDrawSrgs[i] = nullptr;
		}
		perDrawSrgs.Clear();

		auto opaqueTag = RPISystem::Get().GetBuiltinDrawListTag(BuiltinDrawItemTag::Opaque);
		auto transparentTag = RPISystem::Get().GetBuiltinDrawListTag(BuiltinDrawItemTag::Transparent);

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

			if (shader == nullptr)
			{
				continue;
			}

			// TODO: Implement dynamic shader variant selection based on flags & shader options
			RPI::ShaderVariant* variant = shader->GetVariant(shader->GetDefaultVariantIndex());

			const auto& shaderReflection = variant->GetShaderReflection();

			RHI::ShaderResourceGroup* perDrawSrg = nullptr;

			DrawPacketBuilder::DrawItemRequest drawItem{};
			drawItem.drawFilterMask = DrawFilterMask::ALL;
			drawItem.drawItemTag = drawListTag;
			drawItem.stencilRef = stencilRef;

			if (variant->HasSrgLayout(SRGType::PerDraw))
			{
				const ShaderResourceGroupLayout& drawSrgLayout = variant->GetSrgLayout(RHI::SRGType::PerDraw);
				perDrawSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(drawSrgLayout);
				perDrawSrgs.Add(perDrawSrg);
				drawItem.uniqueShaderResourceGroups.Add(perDrawSrg);
			}

			if (drawListTag == opaqueTag ||
				drawListTag == transparentTag ||
				material->GetCurrentShader() == shader)
			{
				drawItem.uniqueShaderResourceGroups.Add(material->GetShaderResourceGroup());
			}

			RHI::MultisampleState multisampleState{};
			scene->GetPipelineMultiSampleState(drawListTag, multisampleState);

			// TODO: Get correct pipeline based on MSAA, color formats, etc. from the RPI::Scene's cache
			drawItem.pipelineState = variant->GetPipeline(multisampleState);

			for (const Name& vertexInputName : shaderReflection.vertexInputs)
			{
				RHI::ShaderSemantic semantic = RHI::ShaderSemantic::Parse(vertexInputName.GetString());

				if (semantic.attribute != VertexInputAttribute::None)
				{
					for (const auto& vertInfo : mesh->vertexBufferInfos)
					{
						if (vertInfo.semantic.attribute != semantic.attribute)
							continue;

						auto vertBufferView = RHI::VertexBufferView(modelLod->GetBuffer(vertInfo.bufferIndex), vertInfo.byteOffset, vertInfo.byteCount, vertInfo.stride);
						drawItem.vertexBufferViews.Add(vertBufferView);
						break;
					}
				}
			}

			drawItem.indexBufferView = mesh->indexBufferView;
			
			builder.AddDrawItem(drawItem);
		}

		drawPacket = builder.Build();
	}

} // namespace CE::RPI
