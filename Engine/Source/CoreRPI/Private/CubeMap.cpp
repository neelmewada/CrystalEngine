#include "CoreRPI.h"

#include <algorithm>

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

		u32 cubeMapRes = desc.cubeMapResolution;
		if (cubeMapRes == 0)
			cubeMapRes = desc.sourceImage.GetHeight();

		RHI::Format hdriFormat = RHI::Format::R16G16B16A16_SFLOAT;
		bool usesHalfPrecision = true;
		// If we want to compress to BC6H, then we need the final image to be in 32-bit float format
		if (desc.useCompression)
		{
			hdriFormat = RHI::Format::R32G32B32A32_SFLOAT;
			usesHalfPrecision = false;
		}

		/////////////////////////////////////////////
		// - Samplers -

		RHI::SamplerDescriptor linearSamplerDesc{};
		linearSamplerDesc.samplerFilterMode = RHI::FilterMode::Linear;
		RHI::Sampler* linearSampler = RPISystem::Get().FindOrCreateSampler(linearSamplerDesc);

		RHI::SamplerDescriptor nearestSamplerDesc{};
		nearestSamplerDesc.samplerFilterMode = RHI::FilterMode::Nearest;
		RHI::Sampler* nearest = RPISystem::Get().FindOrCreateSampler(nearestSamplerDesc);

		nearestSamplerDesc.addressModeU = nearestSamplerDesc.addressModeV = nearestSamplerDesc.addressModeW = SamplerAddressMode::ClampToBorder;
		nearestSamplerDesc.borderColor = RHI::SamplerBorderColor::FloatOpaqueBlack;
		nearestSamplerDesc.samplerFilterMode = RHI::FilterMode::Nearest;
		RHI::Sampler* nearestClamped = RPISystem::Get().FindOrCreateSampler(nearestSamplerDesc);

		/////////////////////////////////////////////
		// - Input Resources -
		
		RHI::TextureDescriptor hdriMapDesc{};
		hdriMapDesc.name = "HDRI FlatMap";
		hdriMapDesc.bindFlags = RHI::TextureBindFlags::ShaderRead;
		hdriMapDesc.width = desc.sourceImage.GetWidth();
		hdriMapDesc.height = desc.sourceImage.GetHeight();
		hdriMapDesc.depth = 1;
		hdriMapDesc.dimension = RHI::Dimension::Dim2D;
		hdriMapDesc.mipLevels = 1;
		hdriMapDesc.sampleCount = 1;
		hdriMapDesc.arrayLayers = 1;
		hdriMapDesc.format = hdriFormat;

		RHI::Texture* hdriMap = RHI::gDynamicRHI->CreateTexture(hdriMapDesc);
		RPI::Texture* hdriMapRpi = new RPI::Texture(hdriMap);
		
		/////////////////////////////////////////////
		// - Output Resources -

		RHI::TextureDescriptor cubeMapDesc{};
		cubeMapDesc.name = desc.name;
		cubeMapDesc.bindFlags = RHI::TextureBindFlags::Color | RHI::TextureBindFlags::ShaderRead;
		cubeMapDesc.defaultHeapType = MemoryHeapType::Default;
		cubeMapDesc.width = cubeMapDesc.height = cubeMapRes;
		cubeMapDesc.depth = 1;
		cubeMapDesc.dimension = RHI::Dimension::DimCUBE;
		cubeMapDesc.mipLevels = 1;
		cubeMapDesc.sampleCount = 1;
		cubeMapDesc.arrayLayers = 6;
		cubeMapDesc.format = hdriFormat;
		
		// Final CubeMap texture with 6 faces
		RHI::Texture* cubeMap = RHI::gDynamicRHI->CreateTexture(cubeMapDesc);
		RPI::Texture* cubeMapRpi = new RPI::Texture(cubeMap);

		RHI::Texture* diffuseIrradiance = nullptr;
		u32 diffuseIrradianceResolution = desc.diffuseIrradianceResolution;

		if (diffuseIrradianceResolution > 0) // Diffuse irradiance map
		{
			RHI::TextureDescriptor irradianceDesc{};
			irradianceDesc.name = "Diffuse Irradiance";
			irradianceDesc.bindFlags = RHI::TextureBindFlags::Color | RHI::TextureBindFlags::ShaderRead;
			irradianceDesc.width = irradianceDesc.height = diffuseIrradianceResolution;
			irradianceDesc.depth = 1;
			irradianceDesc.dimension = RHI::Dimension::DimCUBE;
			irradianceDesc.mipLevels = 1;
			irradianceDesc.sampleCount = 1;
			irradianceDesc.arrayLayers = 6;
			irradianceDesc.format = hdriFormat;

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

		// Full screen quad
		Array<RHI::VertexBufferView> fullscreenQuad = RPISystem::Get().GetFullScreenQuad();
		RHI::DrawLinearArguments fullscreenQuadArgs = RPISystem::Get().GetFullScreenQuadDrawArgs();

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
		stagingBufferDesc.bufferSize = hdriMap->GetByteSize();

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
				f32* f32Data = (f32*)dataPtr;

				f32 r = *((f32*)srcPtr + 4 * i + 0);
				f32 g = *((f32*)srcPtr + 4 * i + 1);
				f32 b = *((f32*)srcPtr + 4 * i + 2);
				
				if (usesHalfPrecision)
				{
					// 16 bit float cannot store values higher than 65,000
					if (r > 65000)
						r = 65000;
					if (g > 65000)
						g = 65000;
					if (b > 65000)
						b = 65000;

					*(u16Data + 4 * i + 0) = Math::ToFloat16(r);
					*(u16Data + 4 * i + 1) = Math::ToFloat16(g);
					*(u16Data + 4 * i + 2) = Math::ToFloat16(b);
					*(u16Data + 4 * i + 3) = Math::ToFloat16(1.0f);
				}
				else
				{
					*(f32Data + 4 * i + 0) = r;
					*(f32Data + 4 * i + 1) = g;
					*(f32Data + 4 * i + 2) = b;
					*(f32Data + 4 * i + 3) = 1.0f;
				}
			}
		}
		stagingBuffer->Unmap();
		dataPtr = nullptr;

		RHI::Texture* grayscale = nullptr; RPI::Texture* grayscaleRpi = nullptr;
		RHI::Texture* rowAverage = nullptr; RPI::Texture* rowAverageRpi = nullptr;
		RHI::Texture* columnAverage = nullptr; RPI::Texture* columnAverageRpi = nullptr;
		RHI::Texture* pdfJoint = nullptr; RPI::Texture* pdfJointRpi = nullptr;
		RHI::Texture* pdfMarginal = nullptr; RPI::Texture* pdfMarginalRpi = nullptr;
		RHI::Texture* pdfConditional = nullptr; RPI::Texture* pdfConditionalRpi = nullptr;
		RHI::Texture* cdfMarginalInverse = nullptr; RPI::Texture* cdfMarginalInverseRpi = nullptr;
		RHI::Texture* cdfConditionalInverse = nullptr; RPI::Texture* cdfConditionalInverseRpi = nullptr;

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

			const u32 singleLineWidth = 40;

			RHI::TextureDescriptor grayscaleDesc{};
			grayscaleDesc.name = "Grayscale";
			grayscaleDesc.width = hdriMapDesc.width;
			grayscaleDesc.height = hdriMapDesc.height;
			fillIntermediateDescriptor(grayscaleDesc);

			RHI::TextureDescriptor rowAverageDesc{};
			rowAverageDesc.name = "Row Average";
			rowAverageDesc.width = singleLineWidth;
			rowAverageDesc.height = hdriMapDesc.height;
			fillIntermediateDescriptor(rowAverageDesc);
			
			RHI::TextureDescriptor columnAverageDesc{};
			columnAverageDesc.name = "Column Average";
			columnAverageDesc.width = 1;
			columnAverageDesc.height = 1;
			fillIntermediateDescriptor(columnAverageDesc);

			RHI::TextureDescriptor pdfJointDesc{};
			pdfJointDesc.name = "PDF Joint";
			pdfJointDesc.width = hdriMapDesc.width;
			pdfJointDesc.height = hdriMapDesc.height;
			fillIntermediateDescriptor(pdfJointDesc);

			RHI::TextureDescriptor pdfMarginalDesc{};
			pdfMarginalDesc.name = "PDF Marginal";
			pdfMarginalDesc.width = singleLineWidth;
			pdfMarginalDesc.height = hdriMapDesc.height;
			fillIntermediateDescriptor(pdfMarginalDesc);
			
			RHI::TextureDescriptor cdfMarginalInverseDesc{};
			cdfMarginalInverseDesc.name = "CDF Marginal Inverse";
			cdfMarginalInverseDesc.width = singleLineWidth;
			cdfMarginalInverseDesc.height = hdriMapDesc.height;
			fillIntermediateDescriptor(cdfMarginalInverseDesc);

			RHI::TextureDescriptor pdfConditionalDesc{};
			pdfConditionalDesc.name = "PDF Conditional";
			pdfConditionalDesc.width = hdriMapDesc.width;
			pdfConditionalDesc.height = hdriMapDesc.height;
			fillIntermediateDescriptor(pdfConditionalDesc);

			RHI::TextureDescriptor cdfConditionalInverseDesc{};
			cdfConditionalInverseDesc.name = "CDF Conditional Inverse";
			cdfConditionalInverseDesc.width = hdriMapDesc.width;
			cdfConditionalInverseDesc.height = hdriMapDesc.height;
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

			grayscaleRpi = new RPI::Texture(grayscale);
			rowAverageRpi = new RPI::Texture(rowAverage);
			columnAverageRpi = new RPI::Texture(columnAverage);
			pdfJointRpi = new RPI::Texture(pdfJoint);
			pdfMarginalRpi = new RPI::Texture(pdfMarginal);
			cdfMarginalInverseRpi = new RPI::Texture(cdfMarginalInverse);
			pdfConditionalRpi = new RPI::Texture(pdfConditional);
			cdfConditionalInverseRpi = new RPI::Texture(cdfConditionalInverse);
		}

		/////////////////////////////////////////////
		// - Render Targets & RTBs -

		RHI::RenderTarget* cubeMapRenderTarget = nullptr;
		RHI::TextureView* cubeMapRTVs[6] = {};
		RHI::RenderTargetBuffer* cubeMapRTBs[6] = {};

		{
			RHI::RenderTargetLayout rtLayout{};
			RHI::RenderAttachmentLayout colorAttachment{};
			colorAttachment.attachmentId = "Color";
			colorAttachment.format = hdriFormat;
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

		RPI::Material* grayscaleMaterial = new RPI::Material(desc.grayscaleShader);
		grayscaleMaterial->SetPropertyValue("_InputTexture", hdriMapRpi);
		grayscaleMaterial->SetPropertyValue("_InputSampler", linearSampler);
		grayscaleMaterial->FlushProperties();

		RPI::Material* rowAverageMaterial = new RPI::Material(desc.rowAverageShader);
		rowAverageMaterial->SetPropertyValue("_InputTexture", grayscaleRpi);
		rowAverageMaterial->SetPropertyValue("_InputSampler", nearestClamped);
		rowAverageMaterial->FlushProperties();

		defer(
			delete grayscaleMaterial;
			delete rowAverageMaterial;
		);

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

			barrier.resource = outputBuffer;
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
				
				RHI::PipelineState* pipeline = equirectMaterials[i]->GetCurrentShader()->GetPipeline();
				cmdList->BindPipelineState(pipeline);

				cmdList->BindVertexBuffers(0, 1, &cubeVertexBufferView);
				cmdList->BindIndexBuffer(cubeMesh->indexBufferView);

				cmdList->SetShaderResourceGroup(equirectMaterials[i]->GetShaderResourceGroup());

				cmdList->CommitShaderResources();

				cmdList->DrawIndexed(cubeMesh->drawArguments.indexedArgs);

				cmdList->EndRenderTarget();
			}

			barrier.resource = cubeMap;
			barrier.fromState = ResourceState::ColorOutput;
			barrier.toState = ResourceState::FragmentShaderResource;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			cmdList->ClearShaderResourceGroups();

			if (diffuseIrradiance != nullptr)
			{
				barrier.resource = grayscale;
				barrier.fromState = ResourceState::Undefined;
				barrier.toState = ResourceState::ColorOutput;
				barrier.subresourceRange = RHI::SubresourceRange::All();
				cmdList->ResourceBarrier(1, &barrier);

				// Grayscale image
				cmdList->BeginRenderTarget(grayscaleRenderTarget, grayscaleRTB, &clearValue);
				{
					RHI::ViewportState viewportState{};
					viewportState.x = viewportState.y = 0;
					viewportState.width = grayscale->GetWidth();
					viewportState.height = grayscale->GetHeight();
					viewportState.minDepth = 0;
					viewportState.maxDepth = 1;
					cmdList->SetViewports(1, &viewportState);

					RHI::ScissorState scissorState{};
					scissorState.x = scissorState.y = 0;
					scissorState.width = viewportState.width;
					scissorState.height = viewportState.height;
					cmdList->SetScissors(1, &scissorState);

					RHI::PipelineState* pipeline = grayscaleMaterial->GetCurrentShader()->GetPipeline();
					cmdList->BindPipelineState(pipeline);

					cmdList->BindVertexBuffers(0, fullscreenQuad.GetSize(), fullscreenQuad.GetData());

					cmdList->SetShaderResourceGroup(grayscaleMaterial->GetShaderResourceGroup());

					cmdList->CommitShaderResources();

					cmdList->DrawLinear(fullscreenQuadArgs);
				}
				cmdList->EndRenderTarget();

				barrier.resource = grayscale;
				barrier.fromState = ResourceState::ColorOutput;
				barrier.toState = ResourceState::FragmentShaderResource;
				barrier.subresourceRange = RHI::SubresourceRange::All();
				cmdList->ResourceBarrier(1, &barrier);

				barrier.resource = rowAverage;
				barrier.fromState = ResourceState::Undefined;
				barrier.toState = ResourceState::ColorOutput;
				barrier.subresourceRange = RHI::SubresourceRange::All();
				cmdList->ResourceBarrier(1, &barrier);

				cmdList->ClearShaderResourceGroups();

				// Row Average
				cmdList->BeginRenderTarget(grayscaleRenderTarget, rowAverageRTB, &clearValue);
				{
					RHI::ViewportState viewportState{};
					viewportState.x = viewportState.y = 0;
					viewportState.width = rowAverage->GetWidth();
					viewportState.height = rowAverage->GetHeight();
					viewportState.minDepth = 0;
					viewportState.maxDepth = 1;
					cmdList->SetViewports(1, &viewportState);

					RHI::ScissorState scissorState{};
					scissorState.x = scissorState.y = 0;
					scissorState.width = viewportState.width;
					scissorState.height = viewportState.height;
					cmdList->SetScissors(1, &scissorState);

					RHI::PipelineState* pipeline = rowAverageMaterial->GetCurrentShader()->GetPipeline();
					cmdList->BindPipelineState(pipeline);

					cmdList->BindVertexBuffers(0, fullscreenQuad.GetSize(), fullscreenQuad.GetData());

					cmdList->SetShaderResourceGroup(rowAverageMaterial->GetShaderResourceGroup());

					cmdList->CommitShaderResources();

					cmdList->DrawLinear(fullscreenQuadArgs);
				}
				cmdList->EndRenderTarget();

				barrier.resource = rowAverage;
				barrier.fromState = ResourceState::ColorOutput;
				barrier.toState = ResourceState::FragmentShaderResource;
				barrier.subresourceRange = RHI::SubresourceRange::All();
				cmdList->ResourceBarrier(1, &barrier);
			}

			barrier.resource = cubeMap;
			barrier.fromState = ResourceState::FragmentShaderResource;
			barrier.toState = ResourceState::CopySource;
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
			if (!desc.useCompression)
			{
				output.Reserve(outputBuffer->GetBufferSize());
				memcpy(output.GetDataPtr(), dataPtr, outputBuffer->GetBufferSize());
			}
			else // BC6H compression
			{
				int numPixelsPerFace = cubeMapRes * cubeMapRes;
				u64 compressedByteSizePerFace = numPixelsPerFace; // BC6H uses 1 byte per pixel
				output.Reserve(compressedByteSizePerFace * 6);

				for (int face = 0; face < 6; face++)
				{
					CMImage image = 
						CMImage::LoadRawImageFromMemory((unsigned char*)dataPtr + face * numPixelsPerFace * GetBitsPerPixelForFormat(cubeMapDesc.format) / 8,
							cubeMapRes, cubeMapRes,
							CMImageFormat::RGBA32, CMImageSourceFormat::None, 16, 64);

					CMImageEncoder encoder{};
					
					bool result = encoder.EncodeToBCn(image, output.GetDataPtr() + compressedByteSizePerFace * face, CMImageSourceFormat::BC6H);
					if (!result)
					{
						CE_LOG(Error, All, "BC6H encoding failed: {}", encoder.GetErrorMessage());
					}
				}
			}
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

		delete cubeMapRpi;
		//delete hdriMapRpi;
		delete diffuseIrradiance;
		delete outputBuffer;
		delete cubeModel;
		delete stagingBuffer;
		delete grayscaleRpi;
		//delete rowAverageRpi;
		delete columnAverageRpi;
		delete pdfJointRpi;
		delete pdfMarginalRpi;
		delete pdfConditionalRpi;
		delete cdfMarginalInverseRpi;
		delete cdfConditionalInverseRpi;
	}

} // namespace CE::RPI
