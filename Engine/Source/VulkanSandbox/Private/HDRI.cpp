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

	struct QuadVertex
	{
		Vec3 position{};
		Vec2 uv{};
	};

	static QuadVertex quadVertices[] = {
		{{-1.0f, -1.0f, 0}, {0.0f, 0.0f}},  // Bottom-left
		{{1.0f, -1.0f, 0}, {1.0f, 0.0f}},   // Bottom-right
		{{-1.0f, 1.0f, 0}, {0.0f, 1.0f}},   // Top-left
		// Triangle 2
		{{-1.0f, 1.0f, 0}, {0.0f, 1.0f}},   // Top-left
		{{1.0f, -1.0f, 0}, {1.0f, 0.0f}},   // Bottom-right
		{{1.0f, 1.0f, 0}, {1.0f, 1.0f}}     // Top-right
	};

	void VulkanSandbox::InitHDRIs()
	{
		RHI::BufferDescriptor quadVertexBufferDesc{};
		quadVertexBufferDesc.name = "Quad Vertex Buffer";
		quadVertexBufferDesc.bindFlags = RHI::BufferBindFlags::VertexBuffer;
		quadVertexBufferDesc.defaultHeapType = RHI::MemoryHeapType::Default;
		quadVertexBufferDesc.bufferSize = COUNTOF(quadVertices) * sizeof(QuadVertex);

		RHI::Buffer* quadVertexBuffer = RHI::gDynamicRHI->CreateBuffer(quadVertexBufferDesc);
		quadVertexBuffer->UploadData(quadVertices, quadVertexBuffer->GetBufferSize(), 0);
        RHI::VertexBufferView quadVertBufferView = RHI::VertexBufferView(quadVertexBuffer, 0, quadVertexBuffer->GetBufferSize(), sizeof(QuadVertex));
        
		IO::Path path = PlatformDirectories::GetLaunchDir() / "Engine/Assets/Textures/HDRI/sample_night2.hdr";

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
        
        // Irradiance map
        {
            RHI::TextureDescriptor irradianceMapDesc{};
            irradianceMapDesc.name = "HDRI Irradiance Map";
            irradianceMapDesc.format = RHI::Format::R16G16B16A16_SFLOAT;
            irradianceMapDesc.bindFlags = RHI::TextureBindFlags::Color | RHI::TextureBindFlags::ShaderReadWrite;
            irradianceMapDesc.width = 64;
            irradianceMapDesc.height = 64;
            irradianceMapDesc.depth = 1;
            irradianceMapDesc.dimension = RHI::Dimension::DimCUBE;
            irradianceMapDesc.mipLevels = 1;
            irradianceMapDesc.arrayLayers = 6;
            
            irradianceMap = RHI::gDynamicRHI->CreateTexture(irradianceMapDesc);
        }

		RHI::Sampler* sampler = nullptr;
		{
			RHI::SamplerDescriptor samplerDesc{};
			samplerDesc.addressModeU = samplerDesc.addressModeV = samplerDesc.addressModeW = RHI::SamplerAddressMode::ClampToBorder;
			samplerDesc.borderColor = RHI::SamplerBorderColor::FloatOpaqueBlack;
			samplerDesc.enableAnisotropy = false;
			samplerDesc.samplerFilterMode = RHI::FilterMode::Linear;

			sampler = RHI::gDynamicRHI->CreateSampler(samplerDesc);
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

		hdriFlatMapDesc.name = "HDRI Grayscale";
		hdriFlatMapDesc.format = RHI::Format::R16_SFLOAT;
		hdriGrayscaleMap = RHI::gDynamicRHI->CreateTexture(hdriFlatMapDesc);

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
        
        RHI::RenderTarget* grayscaleRenderTarget = nullptr;
        RHI::RenderTargetBuffer* grayscaleRTV = nullptr;
        {
            RHI::RenderTargetLayout rtLayout{};
            
            RHI::RenderAttachmentLayout colorAttachment{};
            colorAttachment.attachmentId = "Color";
            colorAttachment.attachmentUsage = RHI::ScopeAttachmentUsage::Color;
            colorAttachment.format = RHI::Format::R16_SFLOAT;
            colorAttachment.loadAction = RHI::AttachmentLoadAction::Clear;
            colorAttachment.storeAction = RHI::AttachmentStoreAction::Store;
            colorAttachment.multisampleState.sampleCount = 1;
            rtLayout.attachmentLayouts.Add(colorAttachment);

            grayscaleRenderTarget = RHI::gDynamicRHI->CreateRenderTarget(rtLayout);
            
            grayscaleRTV = RHI::gDynamicRHI->CreateRenderTargetBuffer(grayscaleRenderTarget, { hdriGrayscaleMap });
        }

		RHI::ShaderResourceGroup* grayscaleSrg = nullptr;
		RHI::PipelineState* grayscalePipeline = nullptr;
		{
			Resource* grayscaleVert = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/Grayscale.vert.spv", nullptr);
			Resource* grayscaleFrag = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/Grayscale.frag.spv", nullptr);

			RHI::ShaderModuleDescriptor vertModuleDesc{};
			vertModuleDesc.name = "Grayscale Vertex";
			vertModuleDesc.stage = RHI::ShaderStage::Vertex;
			vertModuleDesc.byteCode = grayscaleVert->GetData();
			vertModuleDesc.byteSize = grayscaleVert->GetDataSize();

			grayscaleVertShader = RHI::gDynamicRHI->CreateShaderModule(vertModuleDesc);

			RHI::ShaderModuleDescriptor fragModuleDesc{};
			fragModuleDesc.name = "Grayscale Fragment";
			fragModuleDesc.stage = RHI::ShaderStage::Fragment;
			fragModuleDesc.byteCode = grayscaleFrag->GetData();
			fragModuleDesc.byteSize = grayscaleFrag->GetDataSize();

			grayscaleFragShader = RHI::gDynamicRHI->CreateShaderModule(fragModuleDesc);

			RHI::ShaderResourceGroupLayout grayscaleSrgLayout{};
			grayscaleSrgLayout.srgType = RHI::SRGType::PerPass;
			grayscaleSrgLayout.variables.Add({});
			grayscaleSrgLayout.variables.Top().arrayCount = 1;
			grayscaleSrgLayout.variables.Top().name = "_InputTexture";
			grayscaleSrgLayout.variables.Top().bindingSlot = 0;
			grayscaleSrgLayout.variables.Top().type = RHI::ShaderResourceType::Texture2D;
			grayscaleSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			grayscaleSrgLayout.variables.Add({});
			grayscaleSrgLayout.variables.Top().arrayCount = 1;
			grayscaleSrgLayout.variables.Top().name = "_InputSampler";
			grayscaleSrgLayout.variables.Top().bindingSlot = 1;
			grayscaleSrgLayout.variables.Top().type = RHI::ShaderResourceType::SamplerState;
			grayscaleSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			grayscaleSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(grayscaleSrgLayout);

			grayscaleSrg->Bind("_InputSampler", sampler);
			grayscaleSrg->Bind("_InputTexture", hdriMap);
			grayscaleSrg->FlushBindings();

			RHI::GraphicsPipelineDescriptor grayscalePipelineDesc{};
			grayscalePipelineDesc.name = "Grayscale Pipeline";

			RHI::ShaderStageDescriptor vertStageDesc{};
			vertStageDesc.entryPoint = "VertMain";
			vertStageDesc.shaderModule = grayscaleVertShader;
			grayscalePipelineDesc.shaderStages.Add(vertStageDesc);

			RHI::ShaderStageDescriptor fragStageDesc{};
			fragStageDesc.entryPoint = "FragMain";
			fragStageDesc.shaderModule = grayscaleFragShader;
			grayscalePipelineDesc.shaderStages.Add(fragStageDesc);

			grayscalePipelineDesc.rasterState.cullMode = RHI::CullMode::None;
			grayscalePipelineDesc.multisampleState.sampleCount = 1;
			grayscalePipelineDesc.depthStencilState.stencilState.enable = false;
			grayscalePipelineDesc.depthStencilState.depthState.enable = false;
			grayscalePipelineDesc.numScissors = 1;
			grayscalePipelineDesc.numViewports = 1;

			grayscalePipelineDesc.vertexInputSlots.Add({});
			grayscalePipelineDesc.vertexInputSlots.Top().inputRate = VertexInputRate::PerVertex;
			grayscalePipelineDesc.vertexInputSlots.Top().inputSlot = 0;
			grayscalePipelineDesc.vertexInputSlots.Top().stride = sizeof(QuadVertex);

			grayscalePipelineDesc.vertexAttributes.Add({});
			grayscalePipelineDesc.vertexAttributes.Top().dataType = VertexAttributeDataType::Float3;
			grayscalePipelineDesc.vertexAttributes.Top().inputSlot = 0;
			grayscalePipelineDesc.vertexAttributes.Top().location = 0;
			grayscalePipelineDesc.vertexAttributes.Top().offset = offsetof(QuadVertex, position);

			grayscalePipelineDesc.vertexAttributes.Add({});
			grayscalePipelineDesc.vertexAttributes.Top().dataType = VertexAttributeDataType::Float2;
			grayscalePipelineDesc.vertexAttributes.Top().inputSlot = 0;
			grayscalePipelineDesc.vertexAttributes.Top().location = 1;
			grayscalePipelineDesc.vertexAttributes.Top().offset = offsetof(QuadVertex, uv);
			grayscalePipelineDesc.srgLayouts.Add(grayscaleSrgLayout);
            
            grayscalePipelineDesc.precompileForTarget = grayscaleRenderTarget;

			grayscalePipeline = RHI::gDynamicRHI->CreateGraphicsPipeline(grayscalePipelineDesc);

			delete grayscaleVert;
			delete grayscaleFrag;
		}
		
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

		RHI::ShaderResourceGroupLayout irradianceSrgLayout{};
		irradianceSrgLayout.srgType = RHI::SRGType::PerPass;
		irradianceSrgLayout.variables.Add({});
		irradianceSrgLayout.variables.Top().arrayCount = 1;
		irradianceSrgLayout.variables.Top().type = RHI::ShaderResourceType::ConstantBuffer;
		irradianceSrgLayout.variables.Top().name = "_PerViewData";
		irradianceSrgLayout.variables.Top().bindingSlot = 0;
		irradianceSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Vertex | RHI::ShaderStage::Fragment;

		irradianceSrgLayout.variables.Add({});
		irradianceSrgLayout.variables.Top().arrayCount = 1;
		irradianceSrgLayout.variables.Top().type = RHI::ShaderResourceType::TextureCube;
		irradianceSrgLayout.variables.Top().name = "_CubeMap";
		irradianceSrgLayout.variables.Top().bindingSlot = 1;
		irradianceSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

		irradianceSrgLayout.variables.Add({});
		irradianceSrgLayout.variables.Top().arrayCount = 1;
		irradianceSrgLayout.variables.Top().type = RHI::ShaderResourceType::SamplerState;
		irradianceSrgLayout.variables.Top().name = "_CubeMapSampler";
		irradianceSrgLayout.variables.Top().bindingSlot = 2;
		irradianceSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

		RHI::ShaderResourceGroup* equirectangulerSrgs[6] = {};
		
		RHI::Buffer* viewDataBuffers[6] = {};
		
		Matrix4x4 captureProjection = Matrix4x4::PerspectiveProjection(1.0f, 90.0f, 0.1f, 10.0f);

		// Order: right, left, top, bottom, front, back
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

		// Order: right, left, top, bottom, front, back
		Matrix4x4 captureViews2[] = {
			Quat::LookRotation(Vec3( 1.0f,  0.0f,  0.0f), Vec3(0.0f,  1.0f,  0.0f)).ToMatrix(),
			Quat::LookRotation(Vec3(-1.0f,  0.0f,  0.0f), Vec3(0.0f,  1.0f,  0.0f)).ToMatrix(),
			Quat::LookRotation(Vec3( 0.0f,  1.0f,  0.0f), Vec3(0.0f,  0.0f,  1.0f)).ToMatrix(),
			Quat::LookRotation(Vec3( 0.0f, -1.0f,  0.0f), Vec3(0.0f,  0.0f, -1.0f)).ToMatrix(),
			Quat::LookRotation(Vec3( 0.0f,  0.0f,  1.0f), Vec3(0.0f,  1.0f,  0.0f)).ToMatrix(),
			Quat::LookRotation(Vec3( 0.0f,  0.0f, -1.0f), Vec3(0.0f,  1.0f,  0.0f)).ToMatrix()
		};

		RHI::Buffer* viewDataBuffers2[6] = {};

		for (int i = 0; i < 6; i++)
		{
			RHI::BufferDescriptor viewBufferDesc{};
			viewBufferDesc.name = "View Data Buffer 2";
			viewBufferDesc.bufferSize = sizeof(PerViewData);
			viewBufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;
			viewBufferDesc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;

			PerViewData viewData{};
			viewData.viewPosition = Vec3(0, 0, 0);
			viewData.projectionMatrix = captureProjection;
			viewData.viewMatrix = captureViews2[i];
			viewData.viewProjectionMatrix = viewData.projectionMatrix * viewData.viewMatrix;

			viewDataBuffers2[i] = RHI::gDynamicRHI->CreateBuffer(viewBufferDesc);
			viewDataBuffers2[i]->UploadData(&viewData, sizeof(viewData));
		}

		for (int i = 0; i < 6; i++)
		{
			equirectangulerSrgs[i] = RHI::gDynamicRHI->CreateShaderResourceGroup(equirectangulerSrgLayout);
			equirectangulerSrgs[i]->Bind("_InputSampler", sampler);
			equirectangulerSrgs[i]->Bind("_InputTexture", hdriMap);
			equirectangulerSrgs[i]->Bind("_PerViewData", viewDataBuffers[i]);
			equirectangulerSrgs[i]->FlushBindings();
		}

		RHI::ShaderResourceGroup* irradianceSrgs[6] = {};
		for (int i = 0; i < 6; i++)
		{
			irradianceSrgs[i] = RHI::gDynamicRHI->CreateShaderResourceGroup(irradianceSrgLayout);
			irradianceSrgs[i]->Bind("_CubeMap", hdriCubeMap);
			irradianceSrgs[i]->Bind("_PerViewData", viewDataBuffers2[i]);
			irradianceSrgs[i]->Bind("_CubeMapSampler", sampler);
			irradianceSrgs[i]->FlushBindings();
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
        
        InitIrradiancePipeline(irradianceSrgLayout);

		RPI::Mesh* cubeMesh = cubeModel->GetMesh(0);
		const auto& vertInfo = cubeMesh->vertexBufferInfos[0];
		auto vertexBufferView = RHI::VertexBufferView(cubeModel->GetBuffer(vertInfo.bufferIndex), vertInfo.byteOffset, vertInfo.byteCount, vertInfo.stride);

		RPI::Mesh* sphereMesh = sphereModel->GetMesh(0);
		const auto& sphereVertInfo = sphereMesh->vertexBufferInfos[0];
		auto sphereVertexBufferView = RHI::VertexBufferView(sphereModel->GetBuffer(sphereVertInfo.bufferIndex), sphereVertInfo.byteOffset, 
			sphereVertInfo.byteCount, sphereVertInfo.stride);

		RHI::RenderTargetBuffer* renderTargetBuffers[6] = {};
		RHI::TextureView* textureViews[6] = {};

		RHI::RenderTargetBuffer* convolutionRenderTargetBuffers[6] = {};
		RHI::TextureView* convolutionTextureViews[6] = {};

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
        
        for (int i = 0; i < 6; i++)
        {
			RHI::TextureViewDescriptor imageViewDesc{};
			imageViewDesc.format = RHI::Format::R16G16B16A16_SFLOAT;
			imageViewDesc.texture = irradianceMap;
			imageViewDesc.dimension = RHI::Dimension::Dim2D;
			imageViewDesc.mipLevelCount = 1;
			imageViewDesc.baseMipLevel = 0;
			imageViewDesc.baseArrayLayer = i;
			imageViewDesc.arrayLayerCount = 1;

			convolutionTextureViews[i] = RHI::gDynamicRHI->CreateTextureView(imageViewDesc);

			convolutionRenderTargetBuffers[i] = RHI::gDynamicRHI->CreateRenderTargetBuffer(renderTarget, { convolutionTextureViews[i] });
        }
		
		cmdList->Begin();
		{
			RHI::ResourceBarrierDescriptor barrier{};
			barrier.resource = hdriMap;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::CopyDestination;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);
            
            barrier.resource = irradianceMap;
            barrier.fromState = RHI::ResourceState::Undefined;
            barrier.toState = RHI::ResourceState::ColorOutput;
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

			for (int i = 0; i < 6; i++) // Convert equirectangular HDR flat image to HDR CubeMap
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
                
				cmdList->EndRenderTarget();
			}

			barrier.resource = hdriCubeMap;
			barrier.fromState = RHI::ResourceState::ColorOutput;
			barrier.toState = RHI::ResourceState::FragmentShaderResource;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);
            
            // Disabled
			for (int i = 6; i < 6; i++) // Convolute the CubeMap
			{
				cmdList->BeginRenderTarget(renderTarget, convolutionRenderTargetBuffers[i], &clearValue);

				RHI::ViewportState viewportState{};
				viewportState.x = viewportState.y = 0;
				viewportState.width = convolutionTextureViews[i]->GetTexture()->GetWidth();
				viewportState.height = convolutionTextureViews[i]->GetTexture()->GetHeight();
				viewportState.minDepth = 0;
				viewportState.maxDepth = 1;
				cmdList->SetViewports(1, &viewportState);

				RHI::ScissorState scissorState{};
				scissorState.x = scissorState.y = 0;
				scissorState.width = viewportState.width;
				scissorState.height = viewportState.height;
				cmdList->SetScissors(1, &scissorState);

				cmdList->BindPipelineState(irradiancePipeline);

				cmdList->BindVertexBuffers(0, 1, &vertexBufferView);
				cmdList->BindIndexBuffer(cubeMesh->indexBufferView);

				cmdList->SetShaderResourceGroup(irradianceSrgs[i]);

				cmdList->CommitShaderResources();

				cmdList->DrawIndexed(cubeMesh->drawArguments.indexedArgs);

				cmdList->EndRenderTarget();
			}

			barrier.resource = hdriGrayscaleMap;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::ColorOutput;
			barrier.subresourceRange = RHI::SubresourceRange::All();
            cmdList->ResourceBarrier(1, &barrier);
            
            cmdList->ClearShaderResourceGroups();
            
            clearValue.clearValue = Vec4(0, 0, 0, 1.0f);
            cmdList->BeginRenderTarget(grayscaleRenderTarget, grayscaleRTV, &clearValue);
            {
                RHI::ViewportState viewportState{};
                viewportState.x = viewportState.y = 0;
                viewportState.width = hdriGrayscaleMap->GetWidth();
                viewportState.height = hdriGrayscaleMap->GetHeight();
                viewportState.minDepth = 0;
                viewportState.maxDepth = 1;
                cmdList->SetViewports(1, &viewportState);

                RHI::ScissorState scissorState{};
                scissorState.x = scissorState.y = 0;
                scissorState.width = viewportState.width;
                scissorState.height = viewportState.height;
                cmdList->SetScissors(1, &scissorState);
                
                cmdList->BindPipelineState(grayscalePipeline);
                
                cmdList->SetShaderResourceGroup(grayscaleSrg);
                
                cmdList->BindVertexBuffers(0, 1, &quadVertBufferView);
                
                cmdList->CommitShaderResources();
                
                RHI::DrawLinearArguments args{};
                args.firstInstance = 0;
                args.instanceCount = 1;
                args.vertexOffset = 0;
                args.vertexCount = COUNTOF(quadVertices);
                cmdList->DrawLinear(args);
            }
            cmdList->EndRenderTarget();

			barrier.resource = hdriGrayscaleMap;
			barrier.fromState = RHI::ResourceState::ColorOutput;
			barrier.toState = RHI::ResourceState::FragmentShaderResource;
			barrier.subresourceRange = RHI::SubresourceRange::All();
            cmdList->ResourceBarrier(1, &barrier);

			barrier.resource = irradianceMap;
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
		delete quadVertexBuffer;
		delete grayscaleSrg;
		delete grayscaleRTV;
		delete grayscalePipeline;
        delete grayscaleRenderTarget;

		for (int i = 0; i < 6; i++)
		{
			delete viewDataBuffers2[i];
			delete convolutionTextureViews[i];
			delete convolutionRenderTargetBuffers[i];
            delete irradianceSrgs[i];
			delete renderTargetBuffers[i];
			delete textureViews[i];
			delete viewDataBuffers[i];
			delete equirectangulerSrgs[i];
		}
	}

	void VulkanSandbox::DestroyHDRIs()
	{
		delete hdriMap; hdriMap = nullptr;
		delete hdriGrayscaleMap; hdriGrayscaleMap = nullptr;
		delete hdriCubeMap; hdriCubeMap = nullptr;
        delete irradianceMap; irradianceMap = nullptr;
        
        delete irradiancePipeline; irradiancePipeline = nullptr;
        delete convolutionVertShader; convolutionVertShader = nullptr;
        delete convolutionFragShader; convolutionFragShader = nullptr;
	}

    void VulkanSandbox::InitIrradiancePipeline(const RHI::ShaderResourceGroupLayout& irradianceSrgLayout)
    {
        Resource* convolutionVertSpv = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/EnvMapConvolution.vert.spv", nullptr);
        Resource* convolutionFragSpv = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/EnvMapConvolution.frag.spv", nullptr);
        
        RHI::ShaderModuleDescriptor vertShaderDesc{};
        vertShaderDesc.name = "VertMain";
        vertShaderDesc.stage = RHI::ShaderStage::Vertex;
        vertShaderDesc.byteCode = convolutionVertSpv->GetData();
        vertShaderDesc.byteSize = convolutionVertSpv->GetDataSize();
        convolutionVertShader = RHI::gDynamicRHI->CreateShaderModule(vertShaderDesc);
        
        RHI::ShaderModuleDescriptor fragShaderDesc{};
        fragShaderDesc.name = "FragMain";
        fragShaderDesc.stage = RHI::ShaderStage::Fragment;
        fragShaderDesc.byteCode = convolutionFragSpv->GetData();
        fragShaderDesc.byteSize = convolutionFragSpv->GetDataSize();
        convolutionFragShader = RHI::gDynamicRHI->CreateShaderModule(fragShaderDesc);
        
        {
            RHI::GraphicsPipelineDescriptor pipelineDesc{};
            pipelineDesc.name = "Irradiance Convolution";
            
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
            
            pipelineDesc.rasterState.cullMode = RHI::CullMode::None;
            pipelineDesc.rasterState.multisampleEnable = false;
            pipelineDesc.rasterState.fillMode = RHI::FillMode::Solid;

            pipelineDesc.srgLayouts.Add(irradianceSrgLayout);

            pipelineDesc.shaderStages.Add({});
            pipelineDesc.shaderStages.Top().entryPoint = "VertMain";
            pipelineDesc.shaderStages.Top().shaderModule = convolutionVertShader;
            pipelineDesc.shaderStages.Add({});
            pipelineDesc.shaderStages.Top().entryPoint = "FragMain";
            pipelineDesc.shaderStages.Top().shaderModule = convolutionFragShader;
            
            irradiancePipeline = RHI::gDynamicRHI->CreateGraphicsPipeline(pipelineDesc);
        }
        
        delete convolutionVertSpv;
        delete convolutionFragSpv;
    }

} // namespace CE::Sandbox
