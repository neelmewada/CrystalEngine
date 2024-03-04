#include "VulkanSandbox.h"

namespace CE
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

	struct ShaderPipeline
	{
		RHI::ShaderModule* vertShader = nullptr;
		RHI::ShaderModule* fragShader = nullptr;
		RHI::ShaderResourceGroup* srg = nullptr;
		RHI::PipelineState* pipeline = nullptr;

		inline void Destroy()
		{
			delete pipeline;
			delete srg;
			delete vertShader;
			delete fragShader;
		}

		inline void CreateShaderModules(const String& name, Resource* vertSpv, Resource* fragSpv)
		{
			RHI::ShaderModuleDescriptor vertModuleDesc{};
			vertModuleDesc.name = name + "Vert";
			vertModuleDesc.stage = RHI::ShaderStage::Vertex;
			vertModuleDesc.byteCode = vertSpv->GetData();
			vertModuleDesc.byteSize = vertSpv->GetDataSize();
			vertShader = RHI::gDynamicRHI->CreateShaderModule(vertModuleDesc);

			RHI::ShaderModuleDescriptor fragModuleDesc{};
			fragModuleDesc.name = name + "Frag";
			fragModuleDesc.stage = RHI::ShaderStage::Fragment;
			fragModuleDesc.byteCode = fragSpv->GetData();
			fragModuleDesc.byteSize = fragSpv->GetDataSize();
			fragShader = RHI::gDynamicRHI->CreateShaderModule(fragModuleDesc);
		}
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

	static void InitFullScreenQuadPipeline(RHI::GraphicsPipelineDescriptor& pipelineDesc, RHI::ShaderModule* vert, RHI::ShaderModule* frag, 
		RHI::RenderTarget * precompileTarget = nullptr)
	{
		pipelineDesc.rasterState.cullMode = RHI::CullMode::None;
		pipelineDesc.multisampleState.sampleCount = 1;
		pipelineDesc.depthStencilState.stencilState.enable = false;
		pipelineDesc.depthStencilState.depthState.enable = false;

		pipelineDesc.vertexInputSlots.Add({});
		pipelineDesc.vertexInputSlots.Top().inputRate = VertexInputRate::PerVertex;
		pipelineDesc.vertexInputSlots.Top().inputSlot = 0;
		pipelineDesc.vertexInputSlots.Top().stride = sizeof(QuadVertex);
		
		pipelineDesc.vertexAttributes.Add({});
		pipelineDesc.vertexAttributes.Top().dataType = VertexAttributeDataType::Float3;
		pipelineDesc.vertexAttributes.Top().inputSlot = 0;
		pipelineDesc.vertexAttributes.Top().location = 0;
		pipelineDesc.vertexAttributes.Top().offset = offsetof(QuadVertex, position);

		pipelineDesc.vertexAttributes.Add({});
		pipelineDesc.vertexAttributes.Top().dataType = VertexAttributeDataType::Float2;
		pipelineDesc.vertexAttributes.Top().inputSlot = 0;
		pipelineDesc.vertexAttributes.Top().location = 1;
		pipelineDesc.vertexAttributes.Top().offset = offsetof(QuadVertex, uv);

		RHI::ShaderStageDescriptor vertDesc{};
		vertDesc.entryPoint = "VertMain";
		vertDesc.shaderModule = vert;
		pipelineDesc.shaderStages.Add(vertDesc);

		RHI::ShaderStageDescriptor fragDesc{};
		fragDesc.entryPoint = "FragMain";
		fragDesc.shaderModule = frag;
		pipelineDesc.shaderStages.Add(fragDesc);

		pipelineDesc.renderTarget = precompileTarget;
	}

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

		IO::Path path = PlatformDirectories::GetLaunchDir() / "Engine/Assets/Textures/HDRI/sample_night.hdr";

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
			irradianceMapDesc.width = 32;
			irradianceMapDesc.height = 32;
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

		RHI::Sampler* nearestSampler = nullptr;
		{
			RHI::SamplerDescriptor samplerDesc{};
			samplerDesc.addressModeU = samplerDesc.addressModeV = samplerDesc.addressModeW = RHI::SamplerAddressMode::ClampToEdge;
			samplerDesc.borderColor = RHI::SamplerBorderColor::FloatOpaqueBlack;
			samplerDesc.enableAnisotropy = false;
			samplerDesc.samplerFilterMode = RHI::FilterMode::Nearest;

			nearestSampler = RHI::gDynamicRHI->CreateSampler(samplerDesc);
		}

		CMImage hdrImage = CMImage::LoadFromFile(path);

		equirectShader = gEngine->GetAssetManager()->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/CubeMap/Equirectangular");
		iblShader = gEngine->GetAssetManager()->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/CubeMap/IBL");
		iblConvolutionShader = gEngine->GetAssetManager()->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/CubeMap/IBLConvolution");
		mipMapShader = gEngine->GetAssetManager()->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/Utils/MipMapGen");

		if (false) // Skip the block
		{
			BinaryBlob irradianceBlob{};

			CubeMapProcessor cubeMapProcessor{};
			CubeMapOfflineProcessInfo info{};
			info.name = "Test HDR";
			info.sourceImage = hdrImage;
			info.equirectangularShader = equirectShader->GetOrCreateRPIShader(0);
			info.grayscaleShader = iblShader->GetOrCreateRPIShader(0);
			info.rowAverageShader = iblShader->GetOrCreateRPIShader(1);
			info.columnAverageShader = iblShader->GetOrCreateRPIShader(2);
			info.divisionShader = iblShader->GetOrCreateRPIShader(3);
			info.cdfMarginalInverseShader = iblShader->GetOrCreateRPIShader(4);
			info.cdfConditionalInverseShader = iblShader->GetOrCreateRPIShader(5);
			info.diffuseConvolutionShader = iblConvolutionShader->GetOrCreateRPIShader(0);
			info.mipMapShader = mipMapShader->GetOrCreateRPIShader(0);
			info.useCompression = false;
			info.diffuseIrradianceResolution = 32;
			info.diffuseIrradianceOutput = &irradianceBlob;
			info.compressDiffuseIrradiance = false;
			info.specularConvolution = true;
			info.specularConvolutionShader = iblConvolutionShader->GetOrCreateRPIShader(1);

			BinaryBlob testBlob{};
			u32 outMipLevels = 0;

			cubeMapProcessor.ProcessCubeMapOffline(info, testBlob, outMipLevels);
		}

		RPI::Shader* brdfGenShader = iblConvolutionShader->GetOrCreateRPIShader(2);
		RPI::Material* brdfGenMaterial = new RPI::Material(brdfGenShader);
		brdfGenMaterial->FlushProperties();
		defer(
			delete brdfGenMaterial;
		);

		RHI::TextureDescriptor brdfLutDesc{};
		brdfLutDesc.name = "BRDF LUT";
		brdfLutDesc.bindFlags = TextureBindFlags::Color | TextureBindFlags::ShaderRead;
		brdfLutDesc.width = 512;
		brdfLutDesc.height = 512;
		brdfLutDesc.depth = 1;
		brdfLutDesc.format = RHI::Format::R8G8_UNORM;
		brdfLutDesc.mipLevels = 1;
		brdfLutDesc.sampleCount = 1;
		brdfLutDesc.dimension = Dimension::Dim2D;
		brdfLutDesc.defaultHeapType = MemoryHeapType::Default;

		auto brdfLut = RHI::gDynamicRHI->CreateTexture(brdfLutDesc);
		brdfLutRpi = new RPI::Texture(brdfLut);

		RHI::RenderTarget* brdfLutRT = nullptr;
		RHI::RenderTargetBuffer* brdfLutRTB = nullptr;
		{
			RHI::RenderTargetLayout rtLayout{};
			RHI::RenderAttachmentLayout colorAttachment{};
			colorAttachment.attachmentId = "BRDF LUT";
			colorAttachment.format = RHI::Format::R8G8_UNORM;
			colorAttachment.attachmentUsage = ScopeAttachmentUsage::Color;
			colorAttachment.loadAction = AttachmentLoadAction::Clear;
			colorAttachment.storeAction = AttachmentStoreAction::Store;
			colorAttachment.multisampleState.sampleCount = 1;
			rtLayout.attachmentLayouts.Add(colorAttachment);

			brdfLutRT = RHI::gDynamicRHI->CreateRenderTarget(rtLayout);
			brdfLutRTB = RHI::gDynamicRHI->CreateRenderTargetBuffer(brdfLutRT, { brdfLut });
		}

		defer(
			delete brdfLutRT;
			delete brdfLutRTB;
		);

		if (false)
		{
			CE::TextureCube* cubeMapTex = gEngine->GetAssetManager()->LoadAssetAtPath<CE::TextureCube>("/Engine/Assets/Textures/HDRI/sample_night2");

			if (cubeMapTex != nullptr)
			{
				cubeMapTex->GetRpiTexture();

				if (cubeMapTex->GetDiffuseConvolution() != nullptr)
				{
					cubeMapTex->GetDiffuseConvolution()->GetRpiTexture();
				}
			}
		}

		RHI::TextureDescriptor hdriFlatMapDesc{};
		hdriFlatMapDesc.name = "HDRI Texture";
		hdriFlatMapDesc.format = RHI::Format::R16G16B16A16_SFLOAT;
		hdriFlatMapDesc.bindFlags = RHI::TextureBindFlags::Color | RHI::TextureBindFlags::ShaderRead;
		hdriFlatMapDesc.width = hdrImage.GetWidth();
		hdriFlatMapDesc.height = hdrImage.GetHeight();
		hdriFlatMapDesc.depth = 1;
		hdriFlatMapDesc.dimension = RHI::Dimension::Dim2D;
		hdriFlatMapDesc.mipLevels = 1;
		hdriFlatMapDesc.arrayLayers = 1;
		hdriFlatMapDesc.sampleCount = 1;
		hdriFlatMapDesc.defaultHeapType = RHI::MemoryHeapType::Default;

		hdriMap = RHI::gDynamicRHI->CreateTexture(hdriFlatMapDesc);
		hdriIrradiance = RHI::gDynamicRHI->CreateTexture(hdriFlatMapDesc);

		hdriFlatMapDesc.name = "HDRI Grayscale";
		hdriFlatMapDesc.format = RHI::Format::R16_SFLOAT;
		hdriGrayscaleMap = RHI::gDynamicRHI->CreateTexture(hdriFlatMapDesc);
		hdriPDFJoint = RHI::gDynamicRHI->CreateTexture(hdriFlatMapDesc);
		hdriPDFConditional = RHI::gDynamicRHI->CreateTexture(hdriFlatMapDesc);
		hdriCDFConditionalInverse = RHI::gDynamicRHI->CreateTexture(hdriFlatMapDesc);

		hdriFlatMapDesc.width = 1; // Only set higher width when needed for debugging purposes
		hdriRowAverage = RHI::gDynamicRHI->CreateTexture(hdriFlatMapDesc);
		hdriPDFMarginal = RHI::gDynamicRHI->CreateTexture(hdriFlatMapDesc);
		hdriCDFMarginalInverse = RHI::gDynamicRHI->CreateTexture(hdriFlatMapDesc);

		hdriFlatMapDesc.width = hdriFlatMapDesc.height = 1;
		hdriColumnAverage = RHI::gDynamicRHI->CreateTexture(hdriFlatMapDesc);

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
		RHI::RenderTargetBuffer* pdfConditionalRTB = nullptr;

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

			pdfConditionalRTB = RHI::gDynamicRHI->CreateRenderTargetBuffer(grayscaleRenderTarget, { hdriPDFConditional });
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

			grayscalePipelineDesc.srgLayouts.Add(grayscaleSrgLayout);

			InitFullScreenQuadPipeline(grayscalePipelineDesc, grayscaleVertShader, grayscaleFragShader, grayscaleRenderTarget);

			grayscalePipeline = RHI::gDynamicRHI->CreateGraphicsPipeline(grayscalePipelineDesc);

			delete grayscaleVert;
			delete grayscaleFrag;
		}

		ShaderPipeline rowAveragePipeline{};
		RHI::RenderTargetBuffer* rowAverageRTB = nullptr;
		{
			Resource* vertSpv = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/ImageRowAverage.vert.spv", nullptr);
			Resource* fragSpv = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/ImageRowAverage.frag.spv", nullptr);

			rowAveragePipeline.CreateShaderModules("ImageRowAverage", vertSpv, fragSpv);

			RHI::GraphicsPipelineDescriptor rowAverageDesc{};
			rowAverageDesc.name = "Row Average Shader";

			RHI::ShaderResourceGroupLayout rowAverageSrgLayout{};
			rowAverageSrgLayout.srgType = RHI::SRGType::PerPass;
			rowAverageSrgLayout.variables.Add({});
			rowAverageSrgLayout.variables.Top().arrayCount = 1;
			rowAverageSrgLayout.variables.Top().name = "_InputTexture";
			rowAverageSrgLayout.variables.Top().bindingSlot = 0;
			rowAverageSrgLayout.variables.Top().type = RHI::ShaderResourceType::Texture2D;
			rowAverageSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			rowAverageSrgLayout.variables.Add({});
			rowAverageSrgLayout.variables.Top().arrayCount = 1;
			rowAverageSrgLayout.variables.Top().name = "_InputSampler";
			rowAverageSrgLayout.variables.Top().bindingSlot = 1;
			rowAverageSrgLayout.variables.Top().type = RHI::ShaderResourceType::SamplerState;
			rowAverageSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			rowAverageDesc.srgLayouts.Add(rowAverageSrgLayout);

			InitFullScreenQuadPipeline(rowAverageDesc, rowAveragePipeline.vertShader, rowAveragePipeline.fragShader, grayscaleRenderTarget);

			rowAveragePipeline.srg = RHI::gDynamicRHI->CreateShaderResourceGroup(rowAverageSrgLayout);

			rowAveragePipeline.srg->Bind("_InputTexture", hdriGrayscaleMap);
			rowAveragePipeline.srg->Bind("_InputSampler", sampler);
			rowAveragePipeline.srg->FlushBindings();

			rowAveragePipeline.pipeline = RHI::gDynamicRHI->CreateGraphicsPipeline(rowAverageDesc);

			vertSpv->Destroy();
			fragSpv->Destroy();

			rowAverageRTB = RHI::gDynamicRHI->CreateRenderTargetBuffer(grayscaleRenderTarget, { hdriRowAverage });
		}

		ShaderPipeline convolutionPipeline{};
		RHI::RenderTargetBuffer* convolutionRTB = nullptr;
		{
			Resource* vertSpv = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/FlatConvolution.vert.spv", nullptr);
			Resource* fragSpv = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/FlatConvolution.frag.spv", nullptr);

			convolutionPipeline.CreateShaderModules("FlatConvolution", vertSpv, fragSpv);

			RHI::GraphicsPipelineDescriptor convolutionDesc{};
			convolutionDesc.name = "Flat Convolution";

			RHI::ShaderResourceGroupLayout srgLayout{};
			srgLayout.srgType = RHI::SRGType::PerPass;
			srgLayout.variables.Add({});
			srgLayout.variables.Top().arrayCount = 1;
			srgLayout.variables.Top().name = "_InputTexture";
			srgLayout.variables.Top().bindingSlot = 0;
			srgLayout.variables.Top().type = RHI::ShaderResourceType::Texture2D;
			srgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			srgLayout.variables.Add({});
			srgLayout.variables.Top().arrayCount = 1;
			srgLayout.variables.Top().name = "pdfJoint";
			srgLayout.variables.Top().bindingSlot = 1;
			srgLayout.variables.Top().type = RHI::ShaderResourceType::Texture2D;
			srgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			srgLayout.variables.Add({});
			srgLayout.variables.Top().arrayCount = 1;
			srgLayout.variables.Top().name = "cdfYInv";
			srgLayout.variables.Top().bindingSlot = 2;
			srgLayout.variables.Top().type = RHI::ShaderResourceType::Texture2D;
			srgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			srgLayout.variables.Add({});
			srgLayout.variables.Top().arrayCount = 1;
			srgLayout.variables.Top().name = "cdfXInv";
			srgLayout.variables.Top().bindingSlot = 3;
			srgLayout.variables.Top().type = RHI::ShaderResourceType::Texture2D;
			srgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			srgLayout.variables.Add({});
			srgLayout.variables.Top().arrayCount = 1;
			srgLayout.variables.Top().name = "_InputSampler";
			srgLayout.variables.Top().bindingSlot = 4;
			srgLayout.variables.Top().type = RHI::ShaderResourceType::SamplerState;
			srgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			convolutionDesc.srgLayouts.Add(srgLayout);

			InitFullScreenQuadPipeline(convolutionDesc, convolutionPipeline.vertShader, convolutionPipeline.fragShader, grayscaleRenderTarget);

			convolutionPipeline.pipeline = RHI::gDynamicRHI->CreateGraphicsPipeline(convolutionDesc);

			convolutionPipeline.srg = RHI::gDynamicRHI->CreateShaderResourceGroup(srgLayout);
			
			convolutionPipeline.srg->Bind("_InputTexture", hdriMap);
			convolutionPipeline.srg->Bind("pdfJoint", hdriPDFJoint);
			convolutionPipeline.srg->Bind("cdfYInv", hdriCDFMarginalInverse);
			convolutionPipeline.srg->Bind("cdfXInv", hdriCDFConditionalInverse);
			convolutionPipeline.srg->Bind("_InputSampler", nearestSampler);
			convolutionPipeline.srg->FlushBindings();

			convolutionRTB = RHI::gDynamicRHI->CreateRenderTargetBuffer(renderTarget, { hdriIrradiance });

			vertSpv->Destroy();
			fragSpv->Destroy();
		}

		ShaderPipeline columnAveragePipeline{};
		RHI::RenderTargetBuffer* columnAverageRTB = nullptr;
		ShaderPipeline cdfMarginalInversePipeline{};
		RHI::RenderTargetBuffer* cdfMarginalInverseRTB = nullptr;
		{
			Resource* vertSpv = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/ImageColumnAverage.vert.spv", nullptr);
			Resource* fragSpv = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/ImageColumnAverage.frag.spv", nullptr);

			Resource* cdfMarginalInvVertSpv = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/CDFMarginalInverse.vert.spv", nullptr);
			Resource* cdfMarginalInvFragSpv = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/CDFMarginalInverse.frag.spv", nullptr);

			columnAveragePipeline.CreateShaderModules("ImageColumnAverage", vertSpv, fragSpv);
			cdfMarginalInversePipeline.CreateShaderModules("CDFMarginalInverse", cdfMarginalInvVertSpv, cdfMarginalInvFragSpv);

			RHI::GraphicsPipelineDescriptor columnAverageDesc{};
			columnAverageDesc.name = "Column Average Shader";

			RHI::ShaderResourceGroupLayout srgLayout{};
			srgLayout.srgType = RHI::SRGType::PerPass;
			srgLayout.variables.Add({});
			srgLayout.variables.Top().arrayCount = 1;
			srgLayout.variables.Top().name = "_InputTexture";
			srgLayout.variables.Top().bindingSlot = 0;
			srgLayout.variables.Top().type = RHI::ShaderResourceType::Texture2D;
			srgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			srgLayout.variables.Add({});
			srgLayout.variables.Top().arrayCount = 1;
			srgLayout.variables.Top().name = "_InputSampler";
			srgLayout.variables.Top().bindingSlot = 1;
			srgLayout.variables.Top().type = RHI::ShaderResourceType::SamplerState;
			srgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			columnAverageDesc.srgLayouts.Add(srgLayout);

			InitFullScreenQuadPipeline(columnAverageDesc, columnAveragePipeline.vertShader, columnAveragePipeline.fragShader, grayscaleRenderTarget);

			columnAveragePipeline.srg = RHI::gDynamicRHI->CreateShaderResourceGroup(srgLayout);

			columnAveragePipeline.srg->Bind("_InputTexture", hdriRowAverage);
			columnAveragePipeline.srg->Bind("_InputSampler", sampler);
			columnAveragePipeline.srg->FlushBindings();

			columnAveragePipeline.pipeline = RHI::gDynamicRHI->CreateGraphicsPipeline(columnAverageDesc);

			columnAverageRTB = RHI::gDynamicRHI->CreateRenderTargetBuffer(grayscaleRenderTarget, { hdriColumnAverage });

			cdfMarginalInverseRTB = RHI::gDynamicRHI->CreateRenderTargetBuffer(grayscaleRenderTarget, { hdriCDFMarginalInverse });

			RHI::GraphicsPipelineDescriptor cdfMarginalInverseDesc{};
			cdfMarginalInverseDesc.name = "CDF Marginal Inverse";

			cdfMarginalInverseDesc.srgLayouts.Add(srgLayout);

			InitFullScreenQuadPipeline(cdfMarginalInverseDesc, cdfMarginalInversePipeline.vertShader, cdfMarginalInversePipeline.fragShader, grayscaleRenderTarget);

			cdfMarginalInversePipeline.srg = RHI::gDynamicRHI->CreateShaderResourceGroup(srgLayout);

			cdfMarginalInversePipeline.srg->Bind("_InputTexture", hdriPDFMarginal);
			cdfMarginalInversePipeline.srg->Bind("_InputSampler", nearestSampler);
			cdfMarginalInversePipeline.srg->FlushBindings();

			cdfMarginalInversePipeline.pipeline = RHI::gDynamicRHI->CreateGraphicsPipeline(cdfMarginalInverseDesc);

			vertSpv->Destroy();
			fragSpv->Destroy();
			cdfMarginalInvVertSpv->Destroy();
			cdfMarginalInvFragSpv->Destroy();
		}

		ShaderPipeline cdfCondInversePipeline{};
		RHI::RenderTargetBuffer* cdfCondInverseRTB = nullptr;
		{
			Resource* vertSpv = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/CDFConditionalInverse.vert.spv", nullptr);
			Resource* fragSpv = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/CDFConditionalInverse.frag.spv", nullptr);

			cdfCondInversePipeline.CreateShaderModules("CDFConditionalInverse", vertSpv, fragSpv);

			RHI::GraphicsPipelineDescriptor cdfCondInverseDesc{};
			cdfCondInverseDesc.name = "Column Average Shader";

			RHI::ShaderResourceGroupLayout srgLayout{};
			srgLayout.srgType = RHI::SRGType::PerPass;
			srgLayout.variables.Add({});
			srgLayout.variables.Top().arrayCount = 1;
			srgLayout.variables.Top().name = "_InputTexture";
			srgLayout.variables.Top().bindingSlot = 0;
			srgLayout.variables.Top().type = RHI::ShaderResourceType::Texture2D;
			srgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			srgLayout.variables.Add({});
			srgLayout.variables.Top().arrayCount = 1;
			srgLayout.variables.Top().name = "_InputSampler";
			srgLayout.variables.Top().bindingSlot = 1;
			srgLayout.variables.Top().type = RHI::ShaderResourceType::SamplerState;
			srgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			cdfCondInverseDesc.srgLayouts.Add(srgLayout);

			InitFullScreenQuadPipeline(cdfCondInverseDesc, cdfCondInversePipeline.vertShader, cdfCondInversePipeline.fragShader, grayscaleRenderTarget);

			cdfCondInversePipeline.srg = RHI::gDynamicRHI->CreateShaderResourceGroup(srgLayout);

			cdfCondInversePipeline.srg->Bind("_InputTexture", hdriPDFConditional);
			cdfCondInversePipeline.srg->Bind("_InputSampler", nearestSampler);
			cdfCondInversePipeline.srg->FlushBindings();

			cdfCondInversePipeline.pipeline = RHI::gDynamicRHI->CreateGraphicsPipeline(cdfCondInverseDesc);

			cdfCondInverseRTB = RHI::gDynamicRHI->CreateRenderTargetBuffer(grayscaleRenderTarget, { hdriCDFConditionalInverse });

			vertSpv->Destroy();
			fragSpv->Destroy();
		}

		ShaderPipeline divisionPipeline{};
		RHI::RenderTargetBuffer* pdfMarginalRTB = nullptr;
		RHI::ShaderResourceGroup* pdfMarginalSrg = nullptr;
		RHI::ShaderResourceGroup* pdfConditionalSrg = nullptr;

		Array<RHI::VertexBufferView> fullscreenQuad = RPISystem::Get().GetFullScreenQuad();
		RHI::DrawLinearArguments fullscreenQuadArgs = RPISystem::Get().GetFullScreenQuadDrawArgs();

		RHI::RenderTargetBuffer* pdfJointRTB = nullptr;
		RHI::ShaderResourceGroup* pdfJointSrg = nullptr;

		{
			Resource* vertSpv = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/PDFDivision.vert.spv", nullptr);
			Resource* fragSpv = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/PDFDivision.frag.spv", nullptr);

			divisionPipeline.CreateShaderModules("PDFDivision", vertSpv, fragSpv);

			RHI::GraphicsPipelineDescriptor divisionDesc{};
			divisionDesc.name = "Division Shader";

			RHI::ShaderResourceGroupLayout srgLayout{};
			srgLayout.srgType = RHI::SRGType::PerPass;
			srgLayout.variables.Add({});
			srgLayout.variables.Top().arrayCount = 1;
			srgLayout.variables.Top().name = "_InputA";
			srgLayout.variables.Top().bindingSlot = 0;
			srgLayout.variables.Top().type = RHI::ShaderResourceType::Texture2D;
			srgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			srgLayout.variables.Add({});
			srgLayout.variables.Top().arrayCount = 1;
			srgLayout.variables.Top().name = "_InputB";
			srgLayout.variables.Top().bindingSlot = 1;
			srgLayout.variables.Top().type = RHI::ShaderResourceType::Texture2D;
			srgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			srgLayout.variables.Add({});
			srgLayout.variables.Top().arrayCount = 1;
			srgLayout.variables.Top().name = "_InputSampler";
			srgLayout.variables.Top().bindingSlot = 2;
			srgLayout.variables.Top().type = RHI::ShaderResourceType::SamplerState;
			srgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			divisionDesc.srgLayouts.Add(srgLayout);

			InitFullScreenQuadPipeline(divisionDesc, divisionPipeline.vertShader, divisionPipeline.fragShader, grayscaleRenderTarget);

			pdfMarginalSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(srgLayout);
			pdfMarginalSrg->Bind("_InputA", hdriRowAverage);
			pdfMarginalSrg->Bind("_InputB", hdriColumnAverage);
			pdfMarginalSrg->Bind("_InputSampler", nearestSampler);
			pdfMarginalSrg->FlushBindings();

			pdfConditionalSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(srgLayout);
			pdfConditionalSrg->Bind("_InputA", hdriGrayscaleMap);
			pdfConditionalSrg->Bind("_InputB", hdriRowAverage);
			pdfConditionalSrg->Bind("_InputSampler", nearestSampler);
			pdfConditionalSrg->FlushBindings();

			pdfJointSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(srgLayout);
			pdfJointSrg->Bind("_InputA", hdriGrayscaleMap);
			pdfJointSrg->Bind("_InputB", hdriColumnAverage);
			pdfJointSrg->Bind("_InputSampler", nearestSampler);
			pdfJointSrg->FlushBindings();

			pdfJointRTB = RHI::gDynamicRHI->CreateRenderTargetBuffer(grayscaleRenderTarget, { hdriPDFJoint });

			divisionPipeline.pipeline = RHI::gDynamicRHI->CreateGraphicsPipeline(divisionDesc);

			pdfMarginalRTB = RHI::gDynamicRHI->CreateRenderTargetBuffer(grayscaleRenderTarget, { hdriPDFMarginal });

			vertSpv->Destroy();
			fragSpv->Destroy();
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
		hdriCubeMapRpi = new RPI::Texture(hdriCubeMap);

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
				f32 r = *((f32*)hdrImage.GetDataPtr() + 4 * i + 0);
				f32 g = *((f32*)hdrImage.GetDataPtr() + 4 * i + 1);
				f32 b = *((f32*)hdrImage.GetDataPtr() + 4 * i + 2);
				if (r > 65000)
					r = 65000;
				if (g > 65000)
					g = 65000;
				if (b > 65000)
					b = 65000;

				*(dstData + 4 * i + 0) = Math::ToFloat16(r);
				*(dstData + 4 * i + 1) = Math::ToFloat16(g);
				*(dstData + 4 * i + 2) = Math::ToFloat16(b);
				*(dstData + 4 * i + 3) = Math::ToFloat16(1.0f);
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
			Quat::LookRotation(Vec3(1.0f,  0.0f,  0.0f),  Vec3(0.0f, -1.0f,  0.0f)).ToMatrix(),
			Quat::LookRotation(Vec3(0.0f,  1.0f,  0.0f),  Vec3(0.0f,  0.0f, -1.0f)).ToMatrix(),
			Quat::LookRotation(Vec3(0.0f, -1.0f,  0.0f),  Vec3(0.0f,  0.0f,  1.0f)).ToMatrix(),
			Quat::LookRotation(Vec3(0.0f,  0.0f,  1.0f),  Vec3(0.0f, -1.0f,  0.0f)).ToMatrix(),
			Quat::LookRotation(Vec3(0.0f,  0.0f, -1.0f),  Vec3(0.0f, -1.0f,  0.0f)).ToMatrix()
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
		//Matrix4x4 captureViews2[] = {
		//	Quat::LookRotation(Vec3(1.0f,  0.0f,  0.0f), Vec3(0.0f,  1.0f,  0.0f)).ToMatrix(),
		//	Quat::LookRotation(Vec3(-1.0f,  0.0f,  0.0f), Vec3(0.0f,  1.0f,  0.0f)).ToMatrix(),
		//	Quat::LookRotation(Vec3(0.0f,  1.0f,  0.0f), Vec3(0.0f,  0.0f,  1.0f)).ToMatrix(),
		//	Quat::LookRotation(Vec3(0.0f, -1.0f,  0.0f), Vec3(0.0f,  0.0f, -1.0f)).ToMatrix(),
		//	Quat::LookRotation(Vec3(0.0f,  0.0f,  1.0f), Vec3(0.0f,  1.0f,  0.0f)).ToMatrix(),
		//	Quat::LookRotation(Vec3(0.0f,  0.0f, -1.0f), Vec3(0.0f,  1.0f,  0.0f)).ToMatrix()
		//};

		RHI::RenderTargetBuffer* renderTargetBuffers[6] = {};
		RHI::TextureView* textureViews[6] = {};

		RHI::RenderTargetBuffer* irradianceRTBs[6] = {};
		RHI::TextureView* irradianceTextureViews[6] = {};

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

			irradianceTextureViews[i] = RHI::gDynamicRHI->CreateTextureView(imageViewDesc);

			irradianceRTBs[i] = RHI::gDynamicRHI->CreateRenderTargetBuffer(renderTarget, { irradianceTextureViews[i] });
		}

		//RHI::Buffer* viewDataBuffers2[6] = {};

		//for (int i = 0; i < 6; i++)
		//{
		//	RHI::BufferDescriptor viewBufferDesc{};
		//	viewBufferDesc.name = "View Data Buffer 2";
		//	viewBufferDesc.bufferSize = sizeof(PerViewData);
		//	viewBufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;
		//	viewBufferDesc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;

		//	PerViewData viewData{};
		//	viewData.viewPosition = Vec3(0, 0, 0);
		//	viewData.projectionMatrix = captureProjection;
		//	viewData.viewMatrix = captureViews2[i];
		//	viewData.viewProjectionMatrix = viewData.projectionMatrix * viewData.viewMatrix;

		//	viewDataBuffers2[i] = RHI::gDynamicRHI->CreateBuffer(viewBufferDesc);
		//	viewDataBuffers2[i]->UploadData(&viewData, sizeof(viewData));
		//}

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
			irradianceSrgs[i] = RHI::gDynamicRHI->CreateShaderResourceGroup(equirectangulerSrgLayout);
			irradianceSrgs[i]->Bind("_InputSampler", sampler);
			irradianceSrgs[i]->Bind("_InputTexture", hdriIrradiance);
			irradianceSrgs[i]->Bind("_PerViewData", viewDataBuffers[i]);
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

			pipelineDesc.renderTarget = renderTarget;

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
				cmdList->CommitShaderResources();

				cmdList->BindVertexBuffers(0, 1, &quadVertBufferView);

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

			barrier.resource = hdriRowAverage;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::ColorOutput;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			cmdList->ClearShaderResourceGroups();

			// Row Average
			clearValue.clearValue = Vec4(0, 0, 0, 1);
			cmdList->BeginRenderTarget(grayscaleRenderTarget, rowAverageRTB, &clearValue);
			{
				RHI::ViewportState viewportState{};
				viewportState.x = viewportState.y = 0;
				viewportState.width = hdriRowAverage->GetWidth();
				viewportState.height = hdriRowAverage->GetHeight();
				viewportState.minDepth = 0;
				viewportState.maxDepth = 1;
				cmdList->SetViewports(1, &viewportState);

				RHI::ScissorState scissorState{};
				scissorState.x = scissorState.y = 0;
				scissorState.width = viewportState.width;
				scissorState.height = viewportState.height;
				cmdList->SetScissors(1, &scissorState);

				cmdList->BindPipelineState(rowAveragePipeline.pipeline);

				cmdList->SetShaderResourceGroup(rowAveragePipeline.srg);
				cmdList->CommitShaderResources();

				cmdList->BindVertexBuffers(0, 1, &quadVertBufferView);

				RHI::DrawLinearArguments args{};
				args.firstInstance = 0;
				args.instanceCount = 1;
				args.vertexOffset = 0;
				args.vertexCount = COUNTOF(quadVertices);
				cmdList->DrawLinear(args);
			}
			cmdList->EndRenderTarget();

			barrier.resource = hdriRowAverage;
			barrier.fromState = RHI::ResourceState::ColorOutput;
			barrier.toState = RHI::ResourceState::FragmentShaderResource;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			barrier.resource = hdriColumnAverage;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::ColorOutput;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			// Column Average
			cmdList->BeginRenderTarget(grayscaleRenderTarget, columnAverageRTB, &clearValue);
			{
				RHI::ViewportState viewportState{};
				viewportState.x = viewportState.y = 0;
				viewportState.width = hdriColumnAverage->GetWidth();
				viewportState.height = hdriColumnAverage->GetHeight();
				viewportState.minDepth = 0;
				viewportState.maxDepth = 1;
				cmdList->SetViewports(1, &viewportState);

				RHI::ScissorState scissorState{};
				scissorState.x = scissorState.y = 0;
				scissorState.width = viewportState.width;
				scissorState.height = viewportState.height;
				cmdList->SetScissors(1, &scissorState);

				cmdList->BindPipelineState(columnAveragePipeline.pipeline);

				cmdList->SetShaderResourceGroup(columnAveragePipeline.srg);
				cmdList->CommitShaderResources();

				cmdList->BindVertexBuffers(0, 1, &quadVertBufferView);

				RHI::DrawLinearArguments args{};
				args.firstInstance = 0;
				args.instanceCount = 1;
				args.vertexOffset = 0;
				args.vertexCount = COUNTOF(quadVertices);
				cmdList->DrawLinear(args);
			}
			cmdList->EndRenderTarget();

			barrier.resource = hdriColumnAverage;
			barrier.fromState = RHI::ResourceState::ColorOutput;
			barrier.toState = RHI::ResourceState::FragmentShaderResource;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			barrier.resource = hdriPDFMarginal;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::ColorOutput;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);
			
			// PDF Marginal
			cmdList->ClearShaderResourceGroups();
			cmdList->BeginRenderTarget(grayscaleRenderTarget, pdfMarginalRTB, &clearValue);
			{
				RHI::ViewportState viewportState{};
				viewportState.x = viewportState.y = 0;
				viewportState.width = hdriPDFMarginal->GetWidth();
				viewportState.height = hdriPDFMarginal->GetHeight();
				viewportState.minDepth = 0;
				viewportState.maxDepth = 1;
				cmdList->SetViewports(1, &viewportState);

				RHI::ScissorState scissorState{};
				scissorState.x = scissorState.y = 0;
				scissorState.width = viewportState.width;
				scissorState.height = viewportState.height;
				cmdList->SetScissors(1, &scissorState);

				cmdList->BindPipelineState(divisionPipeline.pipeline);

				cmdList->SetShaderResourceGroup(pdfMarginalSrg);
				cmdList->CommitShaderResources();

				cmdList->BindVertexBuffers(0, 1, &quadVertBufferView);

				RHI::DrawLinearArguments args{};
				args.firstInstance = 0;
				args.instanceCount = 1;
				args.vertexOffset = 0;
				args.vertexCount = COUNTOF(quadVertices);
				cmdList->DrawLinear(args);
			}
			cmdList->EndRenderTarget();

			barrier.resource = hdriPDFMarginal;
			barrier.fromState = RHI::ResourceState::ColorOutput;
			barrier.toState = RHI::ResourceState::FragmentShaderResource;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			barrier.resource = hdriCDFMarginalInverse;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::ColorOutput;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			// CDF Marginal Inverse
			cmdList->ClearShaderResourceGroups();
			cmdList->BeginRenderTarget(grayscaleRenderTarget, cdfMarginalInverseRTB, &clearValue);
			{
				RHI::ViewportState viewportState{};
				viewportState.x = viewportState.y = 0;
				viewportState.width = hdriCDFMarginalInverse->GetWidth();
				viewportState.height = hdriCDFMarginalInverse->GetHeight();
				viewportState.minDepth = 0;
				viewportState.maxDepth = 1;
				cmdList->SetViewports(1, &viewportState);

				RHI::ScissorState scissorState{};
				scissorState.x = scissorState.y = 0;
				scissorState.width = viewportState.width;
				scissorState.height = viewportState.height;
				cmdList->SetScissors(1, &scissorState);

				cmdList->BindPipelineState(cdfMarginalInversePipeline.pipeline);

				cmdList->SetShaderResourceGroup(cdfMarginalInversePipeline.srg);
				cmdList->CommitShaderResources();

				cmdList->BindVertexBuffers(0, 1, &quadVertBufferView);

				RHI::DrawLinearArguments args{};
				args.firstInstance = 0;
				args.instanceCount = 1;
				args.vertexOffset = 0;
				args.vertexCount = COUNTOF(quadVertices);
				cmdList->DrawLinear(args);
			}
			cmdList->EndRenderTarget();

			barrier.resource = hdriCDFMarginalInverse;
			barrier.fromState = RHI::ResourceState::ColorOutput;
			barrier.toState = RHI::ResourceState::FragmentShaderResource;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			barrier.resource = hdriPDFConditional;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::ColorOutput;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			// PDF Conditional
			cmdList->ClearShaderResourceGroups();
			cmdList->BeginRenderTarget(grayscaleRenderTarget, pdfConditionalRTB, &clearValue);
			{
				RHI::ViewportState viewportState{};
				viewportState.x = viewportState.y = 0;
				viewportState.width = hdriPDFConditional->GetWidth();
				viewportState.height = hdriPDFConditional->GetHeight();
				viewportState.minDepth = 0;
				viewportState.maxDepth = 1;
				cmdList->SetViewports(1, &viewportState);

				RHI::ScissorState scissorState{};
				scissorState.x = scissorState.y = 0;
				scissorState.width = viewportState.width;
				scissorState.height = viewportState.height;
				cmdList->SetScissors(1, &scissorState);

				cmdList->BindPipelineState(divisionPipeline.pipeline);

				cmdList->SetShaderResourceGroup(pdfConditionalSrg);
				cmdList->CommitShaderResources();

				cmdList->BindVertexBuffers(0, 1, &quadVertBufferView);

				RHI::DrawLinearArguments args{};
				args.firstInstance = 0;
				args.instanceCount = 1;
				args.vertexOffset = 0;
				args.vertexCount = COUNTOF(quadVertices);
				cmdList->DrawLinear(args);
			}
			cmdList->EndRenderTarget();

			barrier.resource = hdriPDFConditional;
			barrier.fromState = RHI::ResourceState::ColorOutput;
			barrier.toState = RHI::ResourceState::FragmentShaderResource;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			barrier.resource = hdriCDFConditionalInverse;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::ColorOutput;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			cmdList->BeginRenderTarget(grayscaleRenderTarget, cdfCondInverseRTB, &clearValue);
			{
				RHI::ViewportState viewportState{};
				viewportState.x = viewportState.y = 0;
				viewportState.width = hdriCDFConditionalInverse->GetWidth();
				viewportState.height = hdriCDFConditionalInverse->GetHeight();
				viewportState.minDepth = 0;
				viewportState.maxDepth = 1;
				cmdList->SetViewports(1, &viewportState);

				RHI::ScissorState scissorState{};
				scissorState.x = scissorState.y = 0;
				scissorState.width = viewportState.width;
				scissorState.height = viewportState.height;
				cmdList->SetScissors(1, &scissorState);

				cmdList->BindPipelineState(cdfCondInversePipeline.pipeline);

				cmdList->SetShaderResourceGroup(cdfCondInversePipeline.srg);
				cmdList->CommitShaderResources();

				cmdList->BindVertexBuffers(0, 1, &quadVertBufferView);

				RHI::DrawLinearArguments args{};
				args.firstInstance = 0;
				args.instanceCount = 1;
				args.vertexOffset = 0;
				args.vertexCount = COUNTOF(quadVertices);
				cmdList->DrawLinear(args);
			}
			cmdList->EndRenderTarget();

			barrier.resource = hdriCDFConditionalInverse;
			barrier.fromState = RHI::ResourceState::ColorOutput;
			barrier.toState = RHI::ResourceState::FragmentShaderResource;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			barrier.resource = hdriPDFJoint;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::ColorOutput;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			cmdList->BeginRenderTarget(grayscaleRenderTarget, pdfJointRTB, &clearValue);
			{
				RHI::ViewportState viewportState{};
				viewportState.x = viewportState.y = 0;
				viewportState.width = hdriPDFJoint->GetWidth();
				viewportState.height = hdriPDFJoint->GetHeight();
				viewportState.minDepth = 0;
				viewportState.maxDepth = 1;
				cmdList->SetViewports(1, &viewportState);

				RHI::ScissorState scissorState{};
				scissorState.x = scissorState.y = 0;
				scissorState.width = viewportState.width;
				scissorState.height = viewportState.height;
				cmdList->SetScissors(1, &scissorState);

				cmdList->BindPipelineState(divisionPipeline.pipeline);
				
				cmdList->SetShaderResourceGroup(pdfJointSrg);
				cmdList->CommitShaderResources();

				cmdList->BindVertexBuffers(0, 1, &quadVertBufferView);

				RHI::DrawLinearArguments args{};
				args.firstInstance = 0;
				args.instanceCount = 1;
				args.vertexOffset = 0;
				args.vertexCount = COUNTOF(quadVertices);
				cmdList->DrawLinear(args);
			}
			cmdList->EndRenderTarget();

			barrier.resource = hdriPDFJoint;
			barrier.fromState = RHI::ResourceState::ColorOutput;
			barrier.toState = RHI::ResourceState::FragmentShaderResource;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			barrier.resource = hdriIrradiance;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::ColorOutput;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			cmdList->ClearShaderResourceGroups();
			cmdList->BeginRenderTarget(renderTarget, convolutionRTB, &clearValue);
			{
				RHI::ViewportState viewportState{};
				viewportState.x = viewportState.y = 0;
				viewportState.width = hdriIrradiance->GetWidth();
				viewportState.height = hdriIrradiance->GetHeight();
				viewportState.minDepth = 0;
				viewportState.maxDepth = 1;
				cmdList->SetViewports(1, &viewportState);

				RHI::ScissorState scissorState{};
				scissorState.x = scissorState.y = 0;
				scissorState.width = viewportState.width;
				scissorState.height = viewportState.height;
				cmdList->SetScissors(1, &scissorState);

				cmdList->BindPipelineState(convolutionPipeline.pipeline);

				cmdList->SetShaderResourceGroup(convolutionPipeline.srg);
				cmdList->CommitShaderResources();

				cmdList->BindVertexBuffers(0, 1, &quadVertBufferView);

				RHI::DrawLinearArguments args{};
				args.firstInstance = 0;
				args.instanceCount = 1;
				args.vertexOffset = 0;
				args.vertexCount = COUNTOF(quadVertices);
				cmdList->DrawLinear(args);
			}
			cmdList->EndRenderTarget();

			barrier.resource = irradianceMap;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::ColorOutput;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			barrier.resource = hdriIrradiance;
			barrier.fromState = RHI::ResourceState::ColorOutput;
			barrier.toState = RHI::ResourceState::FragmentShaderResource;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			for (int i = 0; i < 6; i++) // Convert equirectangular HDR flat image to HDR CubeMap
			{
				cmdList->BeginRenderTarget(renderTarget, irradianceRTBs[i], &clearValue);

				RHI::ViewportState viewportState{};
				viewportState.x = viewportState.y = 0;
				viewportState.width = irradianceTextureViews[i]->GetTexture()->GetWidth();
				viewportState.height = irradianceTextureViews[i]->GetTexture()->GetHeight();
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

				cmdList->SetShaderResourceGroup(irradianceSrgs[i]);
				cmdList->CommitShaderResources();

				cmdList->DrawIndexed(cubeMesh->drawArguments.indexedArgs);

				cmdList->EndRenderTarget();
			}

			barrier.resource = irradianceMap;
			barrier.fromState = RHI::ResourceState::ColorOutput;
			barrier.toState = RHI::ResourceState::FragmentShaderResource;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			barrier.resource = brdfLut;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::ColorOutput;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);

			cmdList->ClearShaderResourceGroups();

			cmdList->BeginRenderTarget(brdfLutRT, brdfLutRTB, &clearValue);
			{
				RHI::ViewportState viewportState{};
				viewportState.x = viewportState.y = 0;
				viewportState.width = brdfLut->GetWidth();
				viewportState.height = brdfLut->GetHeight();
				viewportState.minDepth = 0;
				viewportState.maxDepth = 1;
				cmdList->SetViewports(1, &viewportState);

				RHI::ScissorState scissorState{};
				scissorState.x = scissorState.y = 0;
				scissorState.width = viewportState.width;
				scissorState.height = viewportState.height;
				cmdList->SetScissors(1, &scissorState);

				cmdList->BindPipelineState(brdfGenMaterial->GetCurrentShader()->GetPipeline());

				cmdList->BindVertexBuffers(0, fullscreenQuad.GetSize(), fullscreenQuad.GetData());

				cmdList->DrawLinear(fullscreenQuadArgs);
			}
			cmdList->EndRenderTarget();

			barrier.resource = brdfLut;
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
		delete sampler; delete nearestSampler;
		delete quadVertexBuffer;
		delete grayscaleSrg;
		delete grayscaleRTV;
		delete grayscalePipeline;
		delete grayscaleRenderTarget;
		delete grayscaleVertShader; delete grayscaleFragShader;
		delete rowAverageRTB;
		rowAveragePipeline.Destroy();
		rowAveragePipeline = {};
		columnAveragePipeline.Destroy();
		columnAveragePipeline = {};
		divisionPipeline.Destroy();
		divisionPipeline = {};
		cdfMarginalInversePipeline.Destroy();
		cdfMarginalInversePipeline = {};
		cdfCondInversePipeline.Destroy();
		cdfCondInversePipeline = {};
		convolutionPipeline.Destroy();
		convolutionPipeline = {};
		delete convolutionRTB;
		delete cdfCondInverseRTB;
		delete columnAverageRTB;
		delete pdfMarginalRTB;
		delete cdfMarginalInverseRTB;
		delete pdfMarginalSrg;
		delete pdfConditionalRTB;
		delete pdfConditionalSrg;
		delete pdfJointRTB;
		delete pdfJointSrg;

		for (int i = 0; i < 6; i++)
		{
			//delete viewDataBuffers2[i];
			delete irradianceTextureViews[i];
			delete irradianceRTBs[i];
            delete irradianceSrgs[i];
			delete renderTargetBuffers[i];
			delete textureViews[i];
			delete viewDataBuffers[i];
			delete equirectangulerSrgs[i];
		}

		DestroyIntermediateHDRIs();
	}

	void VulkanSandbox::DestroyIntermediateHDRIs()
	{
		delete hdriMap; hdriMap = nullptr;
		delete hdriGrayscaleMap; hdriGrayscaleMap = nullptr;
		delete hdriRowAverage; hdriRowAverage = nullptr;
		delete hdriColumnAverage; hdriColumnAverage = nullptr;
		delete hdriCDFMarginalInverse; hdriCDFMarginalInverse = nullptr;
		delete hdriPDFConditional; hdriPDFConditional = nullptr;
		delete hdriCDFConditionalInverse; hdriCDFConditionalInverse = nullptr;
		delete hdriPDFMarginal; hdriPDFMarginal = nullptr;
		delete hdriPDFJoint; hdriPDFJoint = nullptr;
	}

	void VulkanSandbox::DestroyHDRIs()
	{
		DestroyIntermediateHDRIs();
		
		delete brdfLutRpi; brdfLutRpi = nullptr;
		delete hdriIrradiance; hdriIrradiance = nullptr;
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
