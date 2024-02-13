#include "VulkanSandbox.h"

namespace CE::Sandbox
{
	static u16 Float32ToFloat16(f32 floatValue)
	{
		f32* ptr = &floatValue;
		unsigned int fltInt32 = *((u32*)ptr);
		u16 fltInt16;

		fltInt16 = (fltInt32 >> 31) << 5;
		u16 tmp = (fltInt32 >> 23) & 0xff;
		tmp = (tmp - 0x70) & ((unsigned int)((int)(0x70 - tmp) >> 4) >> 27);
		fltInt16 = (fltInt16 | tmp) << 10;
		fltInt16 |= (fltInt32 >> 13) & 0x3ff;

		return fltInt16;
	}

	static float CubeVertices[] = {
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
		1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
		// bottom face
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		// top face
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left     
	};

	void VulkanSandbox::InitHDRIs()
	{
		RHI::BufferDescriptor vertexBufferDesc{};
		vertexBufferDesc.name = "Cube Vertices";
		vertexBufferDesc.bufferSize = sizeof(CubeVertices);
		vertexBufferDesc.defaultHeapType = RHI::MemoryHeapType::Default;
		vertexBufferDesc.bindFlags = RHI::BufferBindFlags::VertexBuffer;
		
		RHI::Buffer* vertexBuffer = RHI::gDynamicRHI->CreateBuffer(vertexBufferDesc);
		vertexBuffer->UploadData(CubeVertices, sizeof(CubeVertices));

		IO::Path path = PlatformDirectories::GetLaunchDir() / "Engine/Assets/Textures/HDRI/sample_day.hdr";

		Resource* equirectVertSpv = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/Equirectangular.vert.spv", nullptr);
		Resource* equirectFragSpv = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/Equirectangular.frag.spv", nullptr);
		RHI::ShaderModule* equirectVertShader = nullptr;
		RHI::ShaderModule* equirectFragShader = nullptr;
		{
			RHI::ShaderModuleDescriptor vertDesc{};
			vertDesc.byteCode = equirectVertSpv->GetData();
			vertDesc.byteSize = equirectVertSpv->GetDataSize();
			vertDesc.name = "Equirect Vertex";
			vertDesc.stage = RHI::ShaderStage::Vertex;

			RHI::ShaderModuleDescriptor fragDesc{};
			fragDesc.byteCode = equirectFragSpv->GetData();
			fragDesc.byteSize = equirectFragSpv->GetDataSize();
			fragDesc.name = "Equirect Fragment";
			fragDesc.stage = RHI::ShaderStage::Fragment;

			equirectVertShader = RHI::gDynamicRHI->CreateShaderModule(vertDesc);
			equirectFragShader = RHI::gDynamicRHI->CreateShaderModule(fragDesc);
		}

		CMImage hdrImage = CMImage::LoadFromFile(path);

		RHI::TextureDescriptor hdriFlatMapDesc{};
		hdriFlatMapDesc.name = "HDRI Texture";
		hdriFlatMapDesc.format = RHI::Format::R16G16B16A16_SFLOAT;
		hdriFlatMapDesc.bindFlags = RHI::TextureBindFlags::Color | RHI::TextureBindFlags::ShaderReadWrite;
		hdriFlatMapDesc.width = hdrImage.GetWidth();
		hdriFlatMapDesc.height = hdrImage.GetHeight();
		hdriFlatMapDesc.depth = 1;
		hdriFlatMapDesc.dimension = RHI::Dimension::Dim2D;
		hdriFlatMapDesc.mipLevels = 1;
		hdriFlatMapDesc.arrayLayers = 1;
		hdriFlatMapDesc.sampleCount = 1;
		hdriFlatMapDesc.defaultHeapType = RHI::MemoryHeapType::Default;

		hdriMap = RHI::gDynamicRHI->CreateTexture(hdriFlatMapDesc);

		RHI::TextureDescriptor hdriCubeMapDesc{};
		hdriCubeMapDesc.name = "HDRI CubeMap";
		hdriCubeMapDesc.format = RHI::Format::R16G16B16A16_SFLOAT;
		hdriCubeMapDesc.bindFlags = RHI::TextureBindFlags::Color | RHI::TextureBindFlags::ShaderReadWrite;
		hdriCubeMapDesc.width = 1024;
		hdriCubeMapDesc.height = 1024;
		hdriCubeMapDesc.depth = 1;
		hdriCubeMapDesc.dimension = RHI::Dimension::DimCUBE;
		hdriCubeMapDesc.mipLevels = 1;
		hdriCubeMapDesc.arrayLayers = 6;

		hdriCubeMap = RHI::gDynamicRHI->CreateTexture(hdriCubeMapDesc);

		u32 numPixels = hdrImage.GetWidth() * hdrImage.GetHeight();

		RHI::BufferDescriptor stagingDesc{};
		stagingDesc.name = "Staging Buffer";
		stagingDesc.bindFlags = RHI::BufferBindFlags::StagingBuffer;
		stagingDesc.bufferSize = numPixels * 2 * 4; // f16 * 4: per pixel
		stagingDesc.defaultHeapType = RHI::MemoryHeapType::Upload;

		RHI::Buffer* stagingBuffer = RHI::gDynamicRHI->CreateBuffer(stagingDesc);
		
		void* dataPtr = nullptr;
		stagingBuffer->Map(0, stagingBuffer->GetBufferSize(), &dataPtr);
		{
			u16* dstData = (u16*)dataPtr;

			for (int i = 0; i < numPixels; i++)
			{
				*(dstData + 4 * i + 0) = Float32ToFloat16(*((f32*)hdrImage.GetDataPtr() + 4 * i + 0));
				*(dstData + 4 * i + 1) = Float32ToFloat16(*((f32*)hdrImage.GetDataPtr() + 4 * i + 1));
				*(dstData + 4 * i + 2) = Float32ToFloat16(*((f32*)hdrImage.GetDataPtr() + 4 * i + 2));
				*(dstData + 4 * i + 3) = Float32ToFloat16(1.0f);
			}
		}
		stagingBuffer->Unmap();

		RHI::CommandQueue* queue = RHI::gDynamicRHI->GetPrimaryGraphicsQueue();
		RHI::CommandList* cmdList = RHI::gDynamicRHI->AllocateCommandList(queue);
		RHI::Fence* fence = RHI::gDynamicRHI->CreateFence(false);

		RHI::RenderTarget* renderTarget = nullptr;

		{
			RHI::RenderTargetLayout rtLayout{};

			RHI::RenderAttachmentLayout colorAttachment{};
			colorAttachment.attachmentId = "Color";
			colorAttachment.attachmentUsage = RHI::ScopeAttachmentUsage::Color;
			colorAttachment.format = RHI::Format::R16G16B16A16_SFLOAT;
			colorAttachment.loadAction = RHI::AttachmentLoadAction::Clear;
			colorAttachment.storeAction = RHI::AttachmentStoreAction::Store;
			colorAttachment.multisampleState.sampleCount = 1;
			rtLayout.attachmentLayouts.Add(colorAttachment);

			renderTarget = RHI::gDynamicRHI->CreateRenderTarget(rtLayout);
		}

		RHI::ShaderResourceGroupLayout equirectangulerSrgLayout{};
		equirectangulerSrgLayout.srgType = RHI::SRGType::PerPass;
		equirectangulerSrgLayout.variables.Add({});
		equirectangulerSrgLayout.variables.Top().arrayCount = 1;
		equirectangulerSrgLayout.variables.Top().bindingSlot = 0;
		equirectangulerSrgLayout.variables.Top().name = "_PerViewData";
		equirectangulerSrgLayout.variables.Top().type = RHI::ShaderResourceType::ConstantBuffer;
		equirectangulerSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Vertex | RHI::ShaderStage::Fragment;

		equirectangulerSrgLayout.variables.Add({});
		equirectangulerSrgLayout.variables.Top().arrayCount = 1;
		equirectangulerSrgLayout.variables.Top().bindingSlot = 1;
		equirectangulerSrgLayout.variables.Top().name = "_InputTexture";
		equirectangulerSrgLayout.variables.Top().type = RHI::ShaderResourceType::Texture2D;
		equirectangulerSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

		equirectangulerSrgLayout.variables.Add({});
		equirectangulerSrgLayout.variables.Top().arrayCount = 1;
		equirectangulerSrgLayout.variables.Top().bindingSlot = 2;
		equirectangulerSrgLayout.variables.Top().name = "_InputSampler";
		equirectangulerSrgLayout.variables.Top().type = RHI::ShaderResourceType::SamplerState;
		equirectangulerSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

		RHI::ShaderResourceGroup* equirectangulerSrgs[6] = {};

		RHI::Sampler* sampler = nullptr;
		RHI::Buffer* viewDataBuffers[6] = {};
		
		// Order: right, left, top, bottom, front, back
		Matrix4x4 captureProjection = Matrix4x4::PerspectiveProjection(1.0f, 90.0f, 0.1f, 10.0f);

		Matrix4x4 captureViews[] = {
			Quat::LookRotation(Vec3(-1.0f,  0.0f,  0.0f), Vec3(0.0f, -1.0f,  0.0f)).ToMatrix(),
			Quat::LookRotation(Vec3( 1.0f,  0.0f,  0.0f), Vec3(0.0f, -1.0f,  0.0f)).ToMatrix(),
			Quat::LookRotation(Vec3( 0.0f,  1.0f,  0.0f), Vec3(0.0f,  0.0f, -1.0f)).ToMatrix(),
			Quat::LookRotation(Vec3( 0.0f, -1.0f,  0.0f), Vec3(0.0f,  0.0f,  1.0f)).ToMatrix(),
			Quat::LookRotation(Vec3( 0.0f,  0.0f,  1.0f), Vec3(0.0f, -1.0f,  0.0f)).ToMatrix(),
			Quat::LookRotation(Vec3( 0.0f,  0.0f, -1.0f), Vec3(0.0f, -1.0f,  0.0f)).ToMatrix()
		};

		for (int i = 0; i < 6; i++)
		{
			RHI::BufferDescriptor viewBufferDesc{};
			viewBufferDesc.name = "View Data Buffer";
			viewBufferDesc.bufferSize = sizeof(PerViewData);
			viewBufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;
			viewBufferDesc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;

			PerViewData viewData{};
			viewData.viewPosition = Vec3(0, 0, 0);
			viewData.projectionMatrix = captureProjection;
			viewData.viewMatrix = captureViews[i];
			viewData.viewProjectionMatrix = viewData.projectionMatrix * viewData.viewMatrix;
			
			viewDataBuffers[i] = RHI::gDynamicRHI->CreateBuffer(viewBufferDesc);
			viewDataBuffers[i]->UploadData(&viewData, sizeof(viewData));
		}

		{
			RHI::SamplerDescriptor samplerDesc{};
			samplerDesc.addressModeU = samplerDesc.addressModeV = samplerDesc.addressModeW = RHI::SamplerAddressMode::ClampToBorder;
			samplerDesc.borderColor = RHI::SamplerBorderColor::FloatOpaqueBlack;
			samplerDesc.enableAnisotropy = false;
			samplerDesc.samplerFilterMode = RHI::FilterMode::Linear;
			
			sampler = RHI::gDynamicRHI->CreateSampler(samplerDesc);
		}

		for (int i = 0; i < 6; i++)
		{
			equirectangulerSrgs[i] = RHI::gDynamicRHI->CreateShaderResourceGroup(equirectangulerSrgLayout);
			equirectangulerSrgs[i]->Bind("_InputSampler", sampler);
			equirectangulerSrgs[i]->Bind("_InputTexture", hdriMap);
			equirectangulerSrgs[i]->Bind("_PerViewData", viewDataBuffers[i]);
			equirectangulerSrgs[i]->FlushBindings();
		}

		RHI::PipelineState* equirectangularPipeline = nullptr;
		{
			RHI::GraphicsPipelineDescriptor pipelineDesc{};

			RHI::ColorBlendState colorBlend{};
			colorBlend.alphaBlendOp = RHI::BlendOp::Add;
			colorBlend.colorBlendOp = RHI::BlendOp::Add;
			colorBlend.componentMask = RHI::ColorComponentMask::All;
			colorBlend.srcColorBlend = RHI::BlendFactor::One;
			colorBlend.dstColorBlend = RHI::BlendFactor::Zero;
			colorBlend.srcAlphaBlend = RHI::BlendFactor::One;
			colorBlend.dstAlphaBlend = RHI::BlendFactor::Zero;
			colorBlend.blendEnable = true;
			pipelineDesc.blendState.colorBlends.Add(colorBlend);

			pipelineDesc.depthStencilState.depthState.enable = false;
			pipelineDesc.depthStencilState.stencilState.enable = false;

			pipelineDesc.numViewports = 1;
			pipelineDesc.numScissors = 1;

			pipelineDesc.multisampleState.sampleCount = 1;

			pipelineDesc.vertexInputSlots.Add({});
			pipelineDesc.vertexInputSlots.Top().inputRate = RHI::VertexInputRate::PerVertex;
			pipelineDesc.vertexInputSlots.Top().inputSlot = 0;
			pipelineDesc.vertexInputSlots.Top().stride = sizeof(Vec3);

			pipelineDesc.vertexAttributes.Add({});
			pipelineDesc.vertexAttributes.Top().dataType = RHI::VertexAttributeDataType::Float3;
			pipelineDesc.vertexAttributes.Top().inputSlot = 0;
			pipelineDesc.vertexAttributes.Top().location = 0;
			pipelineDesc.vertexAttributes.Top().offset = 0;

			pipelineDesc.name = "Equirectangular Pipeline";

			pipelineDesc.rasterState.cullMode = RHI::CullMode::None;
			pipelineDesc.rasterState.multisampleEnable = false;
			pipelineDesc.rasterState.fillMode = RHI::FillMode::Solid;

			pipelineDesc.srgLayouts.Add(equirectangulerSrgLayout);

			pipelineDesc.shaderStages.Add({});
			pipelineDesc.shaderStages.Top().entryPoint = "VertMain";
			pipelineDesc.shaderStages.Top().shaderModule = equirectVertShader;
			pipelineDesc.shaderStages.Add({});
			pipelineDesc.shaderStages.Top().entryPoint = "FragMain";
			pipelineDesc.shaderStages.Top().shaderModule = equirectFragShader;

			pipelineDesc.precompileForTarget = renderTarget;

			equirectangularPipeline = RHI::gDynamicRHI->CreateGraphicsPipeline(pipelineDesc);
		}

		RPI::Mesh* cubeMesh = cubeModel->GetMesh(0);
		const auto& vertInfo = cubeMesh->vertexBufferInfos[0];
		auto vertexBufferView = RHI::VertexBufferView(cubeModel->GetBuffer(vertInfo.bufferIndex), vertInfo.byteOffset, vertInfo.byteCount, vertInfo.stride);
		//auto vertexBufferView = RHI::VertexBufferView(vertexBuffer, 0, vertexBuffer->GetBufferSize(), sizeof(Vec4));

		RHI::RenderTargetBuffer* renderTargetBuffers[6] = {};
		RHI::TextureView* textureViews[6] = {};

		for (int i = 0; i < 6; i++)
		{
			RHI::TextureViewDescriptor imageViewDesc{};
			imageViewDesc.format = RHI::Format::R16G16B16A16_SFLOAT;
			imageViewDesc.texture = hdriCubeMap;
			imageViewDesc.dimension = RHI::Dimension::Dim2D;
			imageViewDesc.mipLevelCount = 1;
			imageViewDesc.baseMipLevel = 0;
			imageViewDesc.baseArrayLayer = i;
			imageViewDesc.arrayLayerCount = 1;
			
			textureViews[i] = RHI::gDynamicRHI->CreateTextureView(imageViewDesc);

			renderTargetBuffers[i] = RHI::gDynamicRHI->CreateRenderTargetBuffer(renderTarget, { textureViews[i] });
		}
		
		cmdList->Begin();
		{
			RHI::ResourceBarrierDescriptor barrier{};
			barrier.resource = hdriMap;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::CopyDestination;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			RHI::BufferToTextureCopy copy{};
			copy.srcBuffer = stagingBuffer;
			copy.bufferOffset = 0;
			copy.dstTexture = hdriMap;
			copy.layerCount = 1;
			copy.mipSlice = 0;
			copy.baseArrayLayer = 0;
			cmdList->CopyTextureRegion(copy);

			barrier.resource = hdriMap;
			barrier.fromState = RHI::ResourceState::CopyDestination;
			barrier.toState = RHI::ResourceState::FragmentShaderResource;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			RHI::AttachmentClearValue clearValue{};
			clearValue.clearValue = Color(0, 0, 0, 1).ToVec4();

			barrier.resource = hdriCubeMap;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::ColorOutput;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			for (int i = 0; i < 6; i++)
			{
				cmdList->BeginRenderTarget(renderTarget, renderTargetBuffers[i], &clearValue);

				RHI::ViewportState viewportState{};
				viewportState.x = viewportState.y = 0;
				viewportState.width = textureViews[i]->GetTexture()->GetWidth();
				viewportState.height = textureViews[i]->GetTexture()->GetHeight();
				viewportState.minDepth = 0;
				viewportState.maxDepth = 1;
				cmdList->SetViewports(1, &viewportState);

				RHI::ScissorState scissorState{};
				scissorState.x = scissorState.y = 0;
				scissorState.width = viewportState.width;
				scissorState.height = viewportState.height;
				cmdList->SetScissors(1, &scissorState);

				cmdList->BindPipelineState(equirectangularPipeline);

				cmdList->BindVertexBuffers(0, 1, &vertexBufferView);
				cmdList->BindIndexBuffer(cubeMesh->indexBufferView);

				cmdList->SetShaderResourceGroup(equirectangulerSrgs[i]);

				cmdList->CommitShaderResources();

				cmdList->DrawIndexed(cubeMesh->drawArguments.indexedArgs);

				RHI::DrawLinearArguments args{};
				args.firstInstance = 0;
				args.instanceCount = 1;
				args.vertexCount = COUNTOF(CubeVertices);
				args.vertexOffset = 0;
				//cmdList->DrawLinear(args);

				cmdList->EndRenderTarget();
			}

			barrier.resource = hdriCubeMap;
			barrier.fromState = RHI::ResourceState::ColorOutput;
			barrier.toState = RHI::ResourceState::FragmentShaderResource;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);
		}
		cmdList->End();

		queue->Execute(1, &cmdList, fence);
		fence->WaitForFence();

		fence->Reset();

		hdrImage.Free();
		RHI::gDynamicRHI->FreeCommandLists(1, &cmdList);
		delete stagingBuffer;
		delete fence;
		delete renderTarget;
		delete equirectVertSpv; delete equirectVertShader;
		delete equirectFragSpv; delete equirectFragShader;
		delete equirectangularPipeline;
		delete sampler;
		delete vertexBuffer;

		for (int i = 0; i < 6; i++)
		{
			delete renderTargetBuffers[i];
			delete textureViews[i];
			delete viewDataBuffers[i];
			delete equirectangulerSrgs[i];
		}
	}

	void VulkanSandbox::DestroyHDRIs()
	{
		delete hdriMap; hdriMap = nullptr;
		delete hdriCubeMap; hdriCubeMap = nullptr;
	}

} // namespace CE::Sandbox
