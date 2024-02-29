#include "CoreRPI.h"

namespace CE::RPI
{
	CubeMap::CubeMap(const CubeMapDescriptor& desc) : desc(desc)
	{
		
	}

	CubeMap::~CubeMap()
	{
		delete cubeMap; cubeMap = nullptr;
		delete diffuseIrradiance; diffuseIrradiance = nullptr;
	}

	void CubeMapProcessor::ProcessCubeMapOffline(const CubeMapProcessInfo& desc, BinaryBlob& output)
	{
#if !PLATFORM_DESKTOP
		return nullptr; // Only for desktop platforms
#endif

		if (!desc.sourceImage.IsValid())
			return;
		
		CMImageFormat sourceFormat = desc.sourceImage.GetFormat();
		bool isSourceFloatFormat = false;
		if (sourceFormat == CMImageFormat::RGBA32 || sourceFormat == CMImageFormat::RGB32)
		{
			isSourceFloatFormat = true;
		}
		else if (sourceFormat == CMImageFormat::RGBA8 || sourceFormat == CMImageFormat::RGB8)
		{
			isSourceFloatFormat = false;
		}
		else
		{
			return;
		}

		bool isDestFloatFormat = false;
		EnumType* formatEnum = GetStaticEnum<RHI::Format>();
		EnumConstant* formatConst = formatEnum->FindConstantWithValue((s64)desc.preferredFormat);
		if (formatConst)
		{
			String name = formatConst->GetName().GetString();
			if (name.EndsWith("FLOAT"))
				isDestFloatFormat = true;
		}

		u32 cubeMapRes = desc.cubeMapResolution;
		if (cubeMapRes == 0)
			cubeMapRes = desc.sourceImage.GetHeight();

		/////////////////////////////////////////////
		// - Input Resources -
		
		RHI::TextureDescriptor hdriMapDesc{};
		hdriMapDesc.name = "HDRI FlatMap";
		hdriMapDesc.bindFlags = RHI::TextureBindFlags::ShaderRead;
		hdriMapDesc.width = desc.sourceImage.GetWidth();
		hdriMapDesc.height = desc.sourceImage.GetHeight();
		hdriMapDesc.depth = 1;
		hdriMapDesc.dimension = RHI::Dimension::DimCUBE;
		hdriMapDesc.mipLevels = 1;
		hdriMapDesc.sampleCount = 1;
		hdriMapDesc.arrayLayers = 1;
		hdriMapDesc.format = RHI::Format::R16G16B16A16_SFLOAT;

		RHI::Texture* hdriMap = RHI::gDynamicRHI->CreateTexture(hdriMapDesc);
		RPI::Texture* hdriMapRpi = new RPI::Texture(hdriMap);

		/////////////////////////////////////////////
		// - Output Resources -

		RHI::TextureDescriptor cubeMapDesc{};
		cubeMapDesc.name = desc.name;
		cubeMapDesc.bindFlags = RHI::TextureBindFlags::Color | RHI::TextureBindFlags::ShaderReadWrite;
		cubeMapDesc.defaultHeapType = MemoryHeapType::Default;
		cubeMapDesc.width = cubeMapDesc.height = cubeMapRes;
		cubeMapDesc.depth = 1;
		cubeMapDesc.dimension = RHI::Dimension::DimCUBE;
		cubeMapDesc.mipLevels = 1;
		cubeMapDesc.sampleCount = 1;
		cubeMapDesc.arrayLayers = 6;
		cubeMapDesc.format = desc.preferredFormat;
		
		// Final CubeMap texture with 6 faces
		RHI::Texture* cubeMap = RHI::gDynamicRHI->CreateTexture(cubeMapDesc);

		RHI::Texture* diffuseIrradiance = nullptr;
		u32 diffuseIrradianceResolution = desc.diffuseIrradianceResolution;

		if (diffuseIrradianceResolution > 0) // Diffuse irradiance map
		{
			RHI::TextureDescriptor irradianceDesc{};
			irradianceDesc.name = "Diffuse Irradiance";
			irradianceDesc.bindFlags = RHI::TextureBindFlags::Color | RHI::TextureBindFlags::ShaderReadWrite;
			irradianceDesc.width = irradianceDesc.height = diffuseIrradianceResolution;
			irradianceDesc.depth = 1;
			irradianceDesc.dimension = RHI::Dimension::DimCUBE;
			irradianceDesc.mipLevels = 1;
			irradianceDesc.sampleCount = 1;
			irradianceDesc.arrayLayers = 6;
			irradianceDesc.format = desc.preferredFormat;

			diffuseIrradiance = RHI::gDynamicRHI->CreateTexture(irradianceDesc);
		}

		// CubeMap output buffer
		RHI::BufferDescriptor outputBufferDesc{};
		outputBufferDesc.name = "Output Buffer";
		outputBufferDesc.bindFlags = RHI::BufferBindFlags::StagingBuffer;
		outputBufferDesc.bufferSize = cubeMapRes * cubeMapRes * GetBitsPerPixelForFormat(cubeMapDesc.format) / 8 * 6;
		outputBufferDesc.defaultHeapType = MemoryHeapType::Upload;
		
		RHI::Buffer* outputBuffer = RHI::gDynamicRHI->CreateBuffer(outputBufferDesc);

		/////////////////////////////////////////////
		// - Intermediate Resources -

		// Cube Model
		RPI::ModelLod* cubeModel = RPI::ModelLod::CreateCubeModel();
		RPI::Mesh* cubeMesh = cubeModel->GetMesh(0);
		const auto& cubeVertInfo = cubeMesh->vertexBufferInfos[0];
		auto cubeVertexBufferView = RHI::VertexBufferView(cubeModel->GetBuffer(cubeVertInfo.bufferIndex), cubeVertInfo.byteOffset, cubeVertInfo.byteCount, cubeVertInfo.stride);
		
		// Staging buffer
		RHI::BufferDescriptor stagingBufferDesc{};
		stagingBufferDesc.name = "Staging Buffer";
		stagingBufferDesc.bindFlags = RHI::BufferBindFlags::StagingBuffer;
		stagingBufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;
		stagingBufferDesc.bufferSize = hdriMapDesc.width * hdriMapDesc.height * 2 * 4;

		// To upload input HDRI equirect image to a texture
		RHI::Buffer* stagingBuffer = RHI::gDynamicRHI->CreateBuffer(stagingBufferDesc);

		void* dataPtr;
		stagingBuffer->Map(0, stagingBuffer->GetBufferSize(), &dataPtr);
		{
			void* srcPtr = desc.sourceImage.GetDataPtr();
			int numPixels = hdriMapDesc.width * hdriMapDesc.height;

			for (int i = 0; i < numPixels; i++)
			{
				u16* u16Data = (u16*)dataPtr;

				if (isSourceFloatFormat)
				{
					f32 r = *((f32*)srcPtr + 4 * i + 0);
					f32 g = *((f32*)srcPtr + 4 * i + 1);
					f32 b = *((f32*)srcPtr + 4 * i + 2);
					
					*(u16Data + 4 + i + 0) = Math::ToFloat16(r);
					*(u16Data + 4 + i + 1) = Math::ToFloat16(g);
					*(u16Data + 4 + i + 2) = Math::ToFloat16(b);
					*(u16Data + 4 + i + 3) = Math::ToFloat16(1.0f);
				}
				else
				{
					u8 r = *((u8*)srcPtr + 4 * i + 0);
					u8 g = *((u8*)srcPtr + 4 * i + 1);
					u8 b = *((u8*)srcPtr + 4 * i + 2);

					*(u16Data + 4 + i + 0) = Math::ToFloat16(r);
					*(u16Data + 4 + i + 1) = Math::ToFloat16(g);
					*(u16Data + 4 + i + 2) = Math::ToFloat16(b);
					*(u16Data + 4 + i + 3) = Math::ToFloat16(1.0f);
				}
			}
		}
		stagingBuffer->Unmap();
		dataPtr = nullptr;

		RHI::Texture* grayscale = nullptr;
		RHI::Texture* rowAverage = nullptr;
		RHI::Texture* columnAverage = nullptr;
		RHI::Texture* pdfJoint = nullptr;
		RHI::Texture* pdfMarginal = nullptr;
		RHI::Texture* pdfConditional = nullptr;
		RHI::Texture* cdfMarginalInverse = nullptr;
		RHI::Texture* cdfConditionalInverse = nullptr;

		if (diffuseIrradiance != nullptr)
		{
			auto fillIntermediateDescriptor = [=](RHI::TextureDescriptor& textureDesc)
				{
					textureDesc.bindFlags = RHI::TextureBindFlags::Color | RHI::TextureBindFlags::ShaderRead;
					textureDesc.depth = 1;
					textureDesc.dimension = RHI::Dimension::Dim2D;
					textureDesc.mipLevels = 1;
					textureDesc.sampleCount = 1;
					textureDesc.arrayLayers = 1;
					textureDesc.format = RHI::Format::R16_SFLOAT;
				};

			RHI::TextureDescriptor grayscaleDesc{};
			grayscaleDesc.name = "Grayscale";
			grayscaleDesc.width = grayscaleDesc.height = cubeMapRes;
			fillIntermediateDescriptor(grayscaleDesc);

			RHI::TextureDescriptor rowAverageDesc{};
			rowAverageDesc.name = "Row Average";
			rowAverageDesc.width = 1;
			rowAverageDesc.height = cubeMapRes;
			fillIntermediateDescriptor(rowAverageDesc);
			
			RHI::TextureDescriptor columnAverageDesc{};
			columnAverageDesc.name = "Column Average";
			columnAverageDesc.width = 1;
			columnAverageDesc.height = 1;
			fillIntermediateDescriptor(columnAverageDesc);

			RHI::TextureDescriptor pdfJointDesc{};
			pdfJointDesc.name = "PDF Joint";
			pdfJointDesc.width = cubeMapRes;
			pdfJointDesc.height = cubeMapRes;
			fillIntermediateDescriptor(pdfJointDesc);

			RHI::TextureDescriptor pdfMarginalDesc{};
			pdfMarginalDesc.name = "PDF Marginal";
			pdfMarginalDesc.width = 1;
			pdfMarginalDesc.height = cubeMapRes;
			fillIntermediateDescriptor(pdfMarginalDesc);
			
			RHI::TextureDescriptor cdfMarginalInverseDesc{};
			cdfMarginalInverseDesc.name = "CDF Marginal Inverse";
			cdfMarginalInverseDesc.width = 1;
			cdfMarginalInverseDesc.height = cubeMapRes;
			fillIntermediateDescriptor(cdfMarginalInverseDesc);

			RHI::TextureDescriptor pdfConditionalDesc{};
			pdfConditionalDesc.name = "PDF Conditional";
			pdfConditionalDesc.width = cubeMapRes;
			pdfConditionalDesc.height = cubeMapRes;
			fillIntermediateDescriptor(pdfConditionalDesc);

			RHI::TextureDescriptor cdfConditionalInverseDesc{};
			cdfConditionalInverseDesc.name = "CDF Conditional Inverse";
			cdfConditionalInverseDesc.width = cubeMapRes;
			cdfConditionalInverseDesc.height = cubeMapRes;
			fillIntermediateDescriptor(cdfConditionalInverseDesc);

			// TODO: Try allocating all textures' memory in one go
			//ResourceMemoryRequirements requirements[8] = {};
			//u64 addressOffsets[6] = {};
			//RHI::gDynamicRHI->GetTextureMemoryRequirements(grayscaleDesc, requirements[0]);
			//RHI::gDynamicRHI->GetTextureMemoryRequirements(rowAverageDesc, requirements[1]);
			//RHI::gDynamicRHI->GetTextureMemoryRequirements(columnAverageDesc, requirements[2]);
			//RHI::gDynamicRHI->GetTextureMemoryRequirements(pdfJointDesc, requirements[3]);
			//RHI::gDynamicRHI->GetTextureMemoryRequirements(pdfMarginalDesc, requirements[4]);
			//RHI::gDynamicRHI->GetTextureMemoryRequirements(cdfMarginalInverseDesc, requirements[5]);
			//RHI::gDynamicRHI->GetTextureMemoryRequirements(pdfConditionalDesc, requirements[6]);
			//RHI::gDynamicRHI->GetTextureMemoryRequirements(cdfConditionalInverseDesc, requirements[7]);
			
			grayscale = RHI::gDynamicRHI->CreateTexture(grayscaleDesc);
			rowAverage = RHI::gDynamicRHI->CreateTexture(rowAverageDesc);
			columnAverage = RHI::gDynamicRHI->CreateTexture(columnAverageDesc);
			pdfJoint = RHI::gDynamicRHI->CreateTexture(pdfJointDesc);
			pdfMarginal = RHI::gDynamicRHI->CreateTexture(pdfMarginalDesc);
			cdfMarginalInverse = RHI::gDynamicRHI->CreateTexture(cdfMarginalInverseDesc);
			pdfConditional = RHI::gDynamicRHI->CreateTexture(pdfConditionalDesc);
			cdfConditionalInverse = RHI::gDynamicRHI->CreateTexture(cdfConditionalInverseDesc);
		}
		
		/////////////////////////////////////////////
		// - Samplers -

		RHI::SamplerDescriptor linearSamplerDesc{};
		linearSamplerDesc.samplerFilterMode = RHI::FilterMode::Linear;
		RHI::Sampler* linearSampler = RPISystem::Get().FindOrCreateSampler(linearSamplerDesc);

		RHI::SamplerDescriptor nearestSamplerDesc{};
		nearestSamplerDesc.samplerFilterMode = RHI::FilterMode::Nearest;
		RHI::Sampler* nearest = RPISystem::Get().FindOrCreateSampler(nearestSamplerDesc);

		/////////////////////////////////////////////
		// - Render Targets & RTBs -

		RHI::RenderTarget* cubeMapRenderTarget = nullptr;
		RHI::TextureView* cubeMapRTVs[6] = {};
		RHI::RenderTargetBuffer* cubeMapRTBs[6] = {};

		{
			RHI::RenderTargetLayout rtLayout{};
			RHI::RenderAttachmentLayout colorAttachment{};
			colorAttachment.attachmentId = "Color";
			colorAttachment.format = desc.preferredFormat;
			colorAttachment.attachmentUsage = RHI::ScopeAttachmentUsage::Color;
			colorAttachment.loadAction = RHI::AttachmentLoadAction::Clear;
			colorAttachment.storeAction = RHI::AttachmentStoreAction::Store;
			colorAttachment.loadActionStencil = RHI::AttachmentLoadAction::DontCare;
			colorAttachment.storeActionStencil = RHI::AttachmentStoreAction::DontCare;
			colorAttachment.multisampleState.sampleCount = 1;

			rtLayout.attachmentLayouts.Add(colorAttachment);

			cubeMapRenderTarget = RHI::gDynamicRHI->CreateRenderTarget(rtLayout);

			for (int i = 0; i < 6; i++)
			{
				RHI::TextureViewDescriptor viewDesc{};
				viewDesc.texture = cubeMap;
				viewDesc.baseMipLevel = 0;
				viewDesc.mipLevelCount = 1;
				viewDesc.baseArrayLayer = i;
				viewDesc.arrayLayerCount = 1;
				viewDesc.dimension = RHI::Dimension::Dim2D;
				viewDesc.format = cubeMap->GetFormat();

				cubeMapRTVs[i] = RHI::gDynamicRHI->CreateTextureView(viewDesc);
				cubeMapRTBs[i] = RHI::gDynamicRHI->CreateRenderTargetBuffer(cubeMapRenderTarget, { cubeMapRTVs[i] });
			}
		}

		RHI::RenderTarget* grayscaleRenderTarget = nullptr;
		RHI::RenderTargetBuffer* grayscaleRTB = nullptr;
		RHI::RenderTargetBuffer* rowAverageRTB = nullptr;
		RHI::RenderTargetBuffer* columnAverageRTB = nullptr;
		RHI::RenderTargetBuffer* pdfJointRTB = nullptr;
		RHI::RenderTargetBuffer* pdfMarginalRTB = nullptr;
		RHI::RenderTargetBuffer* pdfConditionalRTB = nullptr;
		RHI::RenderTargetBuffer* cdfMarginalInverseRTB = nullptr;
		RHI::RenderTargetBuffer* cdfConditionalInverseRTB = nullptr;

		if (diffuseIrradiance != nullptr)
		{
			RHI::RenderTargetLayout rtLayout{};
			RHI::RenderAttachmentLayout colorAttachment{};
			colorAttachment.attachmentId = "Color";
			colorAttachment.format = RHI::Format::R16_SFLOAT;
			colorAttachment.attachmentUsage = RHI::ScopeAttachmentUsage::Color;
			colorAttachment.loadAction = RHI::AttachmentLoadAction::Clear;
			colorAttachment.storeAction = RHI::AttachmentStoreAction::Store;
			colorAttachment.loadActionStencil = RHI::AttachmentLoadAction::DontCare;
			colorAttachment.storeActionStencil = RHI::AttachmentStoreAction::DontCare;
			colorAttachment.multisampleState.sampleCount = 1;

			rtLayout.attachmentLayouts.Add(colorAttachment);

			grayscaleRenderTarget = RHI::gDynamicRHI->CreateRenderTarget(rtLayout);

			grayscaleRTB = gDynamicRHI->CreateRenderTargetBuffer(grayscaleRenderTarget, { grayscale });
			rowAverageRTB = gDynamicRHI->CreateRenderTargetBuffer(grayscaleRenderTarget, { rowAverage });
			columnAverageRTB = gDynamicRHI->CreateRenderTargetBuffer(grayscaleRenderTarget, { columnAverage });
			pdfJointRTB = gDynamicRHI->CreateRenderTargetBuffer(grayscaleRenderTarget, { pdfJoint });
			pdfMarginalRTB = gDynamicRHI->CreateRenderTargetBuffer(grayscaleRenderTarget, { pdfMarginal });
			pdfConditionalRTB = gDynamicRHI->CreateRenderTargetBuffer(grayscaleRenderTarget, { pdfConditional });
			cdfMarginalInverseRTB = gDynamicRHI->CreateRenderTargetBuffer(grayscaleRenderTarget, { cdfMarginalInverse });
			cdfConditionalInverseRTB = gDynamicRHI->CreateRenderTargetBuffer(grayscaleRenderTarget, { cdfConditionalInverse });
		}
		
		defer(
			delete cubeMapRenderTarget;
			for (int i = 0; i < 6; i++)
			{
				delete cubeMapRTVs[i];
				delete cubeMapRTBs[i];
			}

			delete grayscaleRenderTarget;
			delete grayscaleRTB;
			delete rowAverageRTB;
			delete columnAverageRTB;
			delete pdfJointRTB;
			delete pdfMarginalRTB;
			delete pdfConditionalRTB;
			delete cdfMarginalInverseRTB;
			delete cdfConditionalInverseRTB;
		);

		/////////////////////////////////////////////
		// - Matrices/Data -

		static Matrix4x4 captureProjection = Matrix4x4::PerspectiveProjection(1.0f, 90.0f, 0.1f, 10.0f);

		// Order: right, left, top, bottom, front, back
		static Matrix4x4 captureViewMatrices[] = {
			Quat::LookRotation(Vec3(-1.0f,  0.0f,  0.0f), Vec3(0.0f, -1.0f,  0.0f)).ToMatrix(),
			Quat::LookRotation(Vec3(1.0f,  0.0f,  0.0f),  Vec3(0.0f, -1.0f,  0.0f)).ToMatrix(),
			Quat::LookRotation(Vec3(0.0f,  1.0f,  0.0f),  Vec3(0.0f,  0.0f, -1.0f)).ToMatrix(),
			Quat::LookRotation(Vec3(0.0f, -1.0f,  0.0f),  Vec3(0.0f,  0.0f,  1.0f)).ToMatrix(),
			Quat::LookRotation(Vec3(0.0f,  0.0f,  1.0f),  Vec3(0.0f, -1.0f,  0.0f)).ToMatrix(),
			Quat::LookRotation(Vec3(0.0f,  0.0f, -1.0f),  Vec3(0.0f, -1.0f,  0.0f)).ToMatrix()
		};

		RPI::Shader* equirectShader = desc.equirectangularShader;
		RPI::Material* equirectMaterials[6] = {};

		for (int i = 0; i < 6; i++)
		{
			equirectMaterials[i] = new RPI::Material(equirectShader);
			equirectMaterials[i]->SelectVariant(0);

			equirectMaterials[i]->SetPropertyValue("viewMatrix", captureViewMatrices[i]);
			equirectMaterials[i]->SetPropertyValue("projectionMatrix", captureProjection);
			equirectMaterials[i]->SetPropertyValue("viewProjectionMatrix", captureProjection * captureViewMatrices[i]);
			equirectMaterials[i]->SetPropertyValue("viewPosition", Vec4(0, 0, 0, 0));
			equirectMaterials[i]->SetPropertyValue("_InputSampler", linearSampler);
			equirectMaterials[i]->SetPropertyValue("_InputTexture", hdriMapRpi);
			equirectMaterials[i]->FlushProperties();
		}

		/////////////////////////////////////////////
		// - Command List Submission -

		RHI::CommandQueue* queue = RHI::gDynamicRHI->GetPrimaryGraphicsQueue();
		RHI::CommandList* cmdList = RHI::gDynamicRHI->AllocateCommandList(queue);
		RHI::Fence* fence = RHI::gDynamicRHI->CreateFence(false);

		cmdList->Begin();
		{
			// - Load Input HDRI Image as a Flat 2D HDR texture -

			RHI::ResourceBarrierDescriptor barrier{};
			barrier.resource = hdriMap;
			barrier.fromState = ResourceState::Undefined;
			barrier.toState = ResourceState::CopyDestination;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			{
				RHI::BufferToTextureCopy copyRegion{};
				copyRegion.srcBuffer = stagingBuffer;
				copyRegion.bufferOffset = 0;
				copyRegion.dstTexture = hdriMap;
				copyRegion.baseArrayLayer = 0;
				copyRegion.layerCount = 1;
				copyRegion.mipSlice = 0;

				cmdList->CopyTextureRegion(copyRegion);
			}

			barrier.resource = hdriMap;
			barrier.fromState = ResourceState::CopyDestination;
			barrier.toState = ResourceState::FragmentShaderResource;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			RHI::AttachmentClearValue clearValue{};
			clearValue.clearValue = Color(0, 0, 0, 1).ToVec4();

			barrier.resource = cubeMap;
			barrier.fromState = ResourceState::Undefined;
			barrier.toState = ResourceState::ColorOutput;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			// Convert equirectangular HDR flat image to HDR CubeMap
			for (int i = 0; i < 6; i++) 
			{
				cmdList->BeginRenderTarget(cubeMapRenderTarget, cubeMapRTBs[i], &clearValue);

				RHI::ViewportState viewportState{};
				viewportState.x = viewportState.y = 0;
				viewportState.width = cubeMapRTVs[i]->GetTexture()->GetWidth();
				viewportState.height = cubeMapRTVs[i]->GetTexture()->GetHeight();
				viewportState.minDepth = 0;
				viewportState.maxDepth = 1;
				cmdList->SetViewports(1, &viewportState);

				RHI::ScissorState scissorState{};
				scissorState.x = scissorState.y = 0;
				scissorState.width = viewportState.width;
				scissorState.height = viewportState.height;
				cmdList->SetScissors(1, &scissorState);
				
				cmdList->BindPipelineState(equirectMaterials[i]->GetCurrentShader()->GetPipeline());

				cmdList->BindVertexBuffers(0, 1, &cubeVertexBufferView);
				cmdList->BindIndexBuffer(cubeMesh->indexBufferView);

				cmdList->SetShaderResourceGroup(equirectMaterials[i]->GetShaderResourceGroup());

				cmdList->CommitShaderResources();

				cmdList->DrawIndexed(cubeMesh->drawArguments.indexedArgs);

				cmdList->EndRenderTarget();
			}

			if (diffuseIrradiance != nullptr)
			{

			}

			barrier.resource = cubeMap;
			barrier.fromState = ResourceState::ColorOutput;
			barrier.toState = ResourceState::CopySource;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			barrier.resource = outputBuffer;
			barrier.fromState = ResourceState::Undefined;
			barrier.toState = ResourceState::CopyDestination;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			for (int i = 0; i < 6; i++)
			{
				RHI::TextureToBufferCopy copyRegion{};
				copyRegion.srcTexture = cubeMap;
				copyRegion.mipSlice = 0;
				copyRegion.baseArrayLayer = i;
				copyRegion.layerCount = 1;
				copyRegion.dstBuffer = outputBuffer;
				copyRegion.bufferOffset = i * cubeMapRes * cubeMapRes * GetBitsPerPixelForFormat(cubeMapDesc.format) / 8;

				cmdList->CopyTextureRegion(copyRegion);
			}
		}
		cmdList->End();

		queue->Execute(1, &cmdList, fence);
		fence->WaitForFence();

		/////////////////////////////////////////////
		// - Output Data -

		outputBuffer->Map(0, outputBuffer->GetBufferSize(), &dataPtr);
		{
			output.Reserve(outputBuffer->GetBufferSize());
			memcpy(output.GetDataPtr(), dataPtr, outputBuffer->GetBufferSize());
		}
		outputBuffer->Unmap();

		/////////////////////////////////////////////
		// - Free up memory -

		RHI::gDynamicRHI->FreeCommandLists(1, &cmdList);
		RHI::gDynamicRHI->DestroyFence(fence);

		for (int i = 0; i < 6; i++)
		{
			delete equirectMaterials[i];
			equirectMaterials[i] = nullptr;
		}

		delete cubeMap;
		delete outputBuffer;
		delete cubeModel;
		delete stagingBuffer;
		delete hdriMap;
		delete hdriMapRpi;
		delete grayscale;
		delete rowAverage;
		delete columnAverage;
		delete pdfJoint;
		delete pdfMarginal;
		delete pdfConditional;
		delete cdfMarginalInverse;
		delete cdfConditionalInverse;
	}

} // namespace CE::RPI
