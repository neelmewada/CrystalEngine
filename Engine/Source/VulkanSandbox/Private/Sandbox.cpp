#include "VulkanSandbox.h"

#ifndef __INTELLISENSE__
#include "skybox/back.jpg.h"
#include "skybox/bottom.jpg.h"
#include "skybox/front.jpg.h"
#include "skybox/left.jpg.h"
#include "skybox/right.jpg.h"
#include "skybox/top.jpg.h"
#else
static const char skybox_back_jpg_Data[] = { 0 };
static const u32 skybox_back_jpg_Length = 2;

static const char skybox_bottom_jpg_Data[] = { 0 };
static const u32 skybox_bottom_jpg_Length = 2;

static const char skybox_front_jpg_Data[] = { 0 };
static const u32 skybox_front_jpg_Length = 2;

static const char skybox_left_jpg_Data[] = { 0 };
static const u32 skybox_left_jpg_Length = 2;

static const char skybox_right_jpg_Data[] = { 0 };
static const u32 skybox_right_jpg_Length = 2;

static const char skybox_top_jpg_Data[] = { 0 };
static const u32 skybox_top_jpg_Length = 2;
#endif

namespace CE::Sandbox
{
	constexpr u32 directionalLightArrayBinding = 0;
	constexpr u32 pointLightsBinding = 1;
	constexpr u32 lightDataBinding = 2;
	constexpr u32 perViewDataBinding = 5;
	constexpr u32 perObjectDataBinding = 7;
	constexpr u32 materialDataBinding = 8;
    constexpr u32 skyboxBinding = 3;
    constexpr u32 defaultSamplerBinding = 4;

	static int counter = 0;
	static RHI::RHISystem rhiSystem{};

	void VulkanSandbox::Init(PlatformWindow* window)
	{
		localCounter = counter++;

		RHI::FrameSchedulerDescriptor desc{};
		
		scheduler = new RHI::FrameScheduler(desc);

		mainWindow = window;

		RHI::SwapChainDescriptor swapChainDesc{};
		swapChainDesc.imageCount = 2;
		swapChainDesc.preferredFormats = { RHI::Format::R8G8B8A8_UNORM, RHI::Format::B8G8R8A8_UNORM };

		swapChain = RHI::gDynamicRHI->CreateSwapChain(mainWindow, swapChainDesc);

		mainWindow->GetDrawableWindowSize(&width, &height);

		mainWindow->AddListener(this);
		
		InitCubeMaps();
		InitPipelines();
		InitLights();
		InitDrawPackets();

		BuildFrameGraph();
		CompileFrameGraph();
	}

	void VulkanSandbox::Tick(f32 deltaTime)
	{
		if (destroyed)
			return;

		u32 curWidth = 0, curHeight = 0;
		mainWindow->GetDrawableWindowSize(&curWidth, &curHeight);

		if (width != curWidth || height != curHeight)
		{
			rebuild = recompile = true;
			width = curWidth;
			height = curHeight;
		}

		if (rebuild)
		{
			rebuild = false;
			recompile = true;

			BuildFrameGraph();
		}

		if (recompile)
		{
			recompile = false;
			resubmit = true;

			CompileFrameGraph();
		}
		
		if (resubmit)
		{
			resubmit = false;

			SubmitWork();
		}

		UpdatePerViewData();

		meshRotation += deltaTime * 15.0f;
		if (meshRotation >= 360)
			meshRotation -= 360;

		meshModelMatrix = Matrix4x4::Translation(Vec3(0, 0, 15)) * Quat::EulerDegrees(Vec3(0, meshRotation, 0)).ToMatrix() * Matrix4x4::Scale(Vec3(1, 1, 1));

		RHI::BufferData uploadData{};
		uploadData.dataSize = sizeof(meshModelMatrix);
		uploadData.data = &meshModelMatrix;
		uploadData.startOffsetInBuffer = 0;

		cubeObjectBuffer->UploadData(uploadData);

		scheduler->Execute();
	}

	void VulkanSandbox::UpdatePerViewData()
	{
		float farPlane = 1000.0f;

		perViewData.viewPosition = Vec3(0, 0, -10);
		perViewData.projectionMatrix = Matrix4x4::PerspectiveProjection(swapChain->GetAspectRatio(), 60, 0.1f, farPlane);
		perViewData.viewMatrix = Matrix4x4::Translation(perViewData.viewPosition);
		perViewData.viewProjectionMatrix = perViewData.projectionMatrix * perViewData.viewMatrix;

		skyboxModelMatrix = Matrix4x4::Translation(Vec3()) * Quat::EulerDegrees(Vec3(0, 0, 0)).ToMatrix() * Matrix4x4::Scale(Vec3(farPlane, farPlane, farPlane));

		skyboxObjectBuffer->UploadData(&skyboxObjectBuffer, sizeof(skyboxModelMatrix));

		RHI::BufferData data{};
		data.startOffsetInBuffer = 0;
		data.dataSize = perViewBuffer->GetBufferSize();
		data.data = &perViewData;

		perViewBuffer->UploadData(data);
	}

	void VulkanSandbox::Shutdown()
	{
		DestroyLights();
		DestroyDrawPackets();
		DestroyCubeMaps();

		if (mainWindow)
		{
			mainWindow->RemoveListener(this);
		}

		delete scheduler;
		DestroyPipelines();

		delete swapChain;
	}
	
	inline CMImage LoadImage(const char* data, u32 size)
	{
		return CMImage::LoadFromMemory((unsigned char*)data, size);
	}

#define LOAD_SKYBOX(side) LoadImage(skybox_##side##_jpg_Data, skybox_##side##_jpg_Length)

	void VulkanSandbox::InitCubeMaps()
	{
		CMImage right = LOAD_SKYBOX(right); // 0
		CMImage left = LOAD_SKYBOX(left); // 1
		CMImage top = LOAD_SKYBOX(top); // 2
		CMImage bottom = LOAD_SKYBOX(bottom); // 3
		CMImage front = LOAD_SKYBOX(front); // 4
		CMImage back = LOAD_SKYBOX(back); // 5
		
		CMImage* layers[] = { &right, &left, &top, &bottom, &front, &back };

		RHI::TextureDescriptor cubeMapDesc{};
		cubeMapDesc.name = "Skybox";
		cubeMapDesc.sampleCount = 1;
		cubeMapDesc.arrayLayers = 6;
		cubeMapDesc.bindFlags = RHI::TextureBindFlags::ShaderRead;
		cubeMapDesc.mipLevels = 1;
		cubeMapDesc.width = back.GetWidth();
		cubeMapDesc.height = back.GetHeight();
		cubeMapDesc.depth = 1;
		cubeMapDesc.dimension = RHI::Dimension::DimCUBE;
        cubeMapDesc.format = RHI::Format::R8G8B8A8_SRGB; //RHI::Format::BC7_SRGB

		const u32 height = cubeMapDesc.height;
		const u32 width = cubeMapDesc.width;

		skyboxCubeMap = RHI::gDynamicRHI->CreateTexture(cubeMapDesc);

		RHI::BufferDescriptor stagingBufferDesc{};
		stagingBufferDesc.name = "CubeMap Transfer";
		stagingBufferDesc.bindFlags = RHI::BufferBindFlags::StagingBuffer;
		stagingBufferDesc.bufferSize = height * width * 4 * 6; // 4 bytes per pixel * 6 array layers
		stagingBufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;

		RHI::CommandQueue* queue = RHI::gDynamicRHI->GetPrimaryGraphicsQueue();

		RHI::CommandList* commandList = RHI::gDynamicRHI->AllocateCommandList(queue);
		RHI::Fence* commandListFence = RHI::gDynamicRHI->CreateFence(false);
		
		RHI::Buffer* stagingBuffer = RHI::gDynamicRHI->CreateBuffer(stagingBufferDesc);

		commandList->Begin();
		
		{
			RHI::ResourceBarrierDescriptor barrier{};
			barrier.resource = skyboxCubeMap;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::CopyDestination;
			commandList->ResourceBarrier(1, &barrier);

			void* dataPtr = nullptr;
			stagingBuffer->Map(0, stagingBuffer->GetBufferSize(), &dataPtr);
			{
				for (int face = 0; face < 6; face++)
				{
					u8* dstData = (u8*)dataPtr + height * width * 4 * face;
					u8* srcData = right.GetDataPtr();
					for (int y = 0; y < height; y++)
					{
						for (int x = 0; x < width; x++)
						{
							int srcPixelIndex = (y * height + x) * 3; // Assuming all CMImage's are RGB8 (3 channels only)
							int dstPixelIndex = (y * height + x) * 4;

							*(dstData + dstPixelIndex) = *(srcData + srcPixelIndex); // R
							*(dstData + dstPixelIndex + 1) = *(srcData + srcPixelIndex + 1); // G
							*(dstData + dstPixelIndex + 2) = *(srcData + srcPixelIndex + 2); // B
							*(dstData + dstPixelIndex + 3) = 0; // A
						}
					}
				}
			}
			stagingBuffer->Unmap();

			RHI::BufferToTextureCopy copyRegion{};
			copyRegion.dstTexture = skyboxCubeMap;
			copyRegion.baseArrayLayer = 0;
			copyRegion.layerCount = 6;
			copyRegion.mipSlice = 0;
			copyRegion.srcBuffer = stagingBuffer;
			copyRegion.bufferOffset = 0;

			commandList->CopyTextureRegion(copyRegion);

			barrier.resource = skyboxCubeMap;
			barrier.fromState = RHI::ResourceState::CopyDestination;
			barrier.toState = RHI::ResourceState::FragmentShaderResource;
			commandList->ResourceBarrier(1, &barrier);
		}

		commandList->End();

		queue->Execute(1, &commandList, commandListFence);
		commandListFence->WaitForFence();

		RHI::gDynamicRHI->DestroyFence(commandListFence);
		RHI::gDynamicRHI->FreeCommandLists(1, &commandList);

		back.Free(); front.Free(); left.Free(); right.Free(); top.Free(); bottom.Free();
		delete stagingBuffer; stagingBuffer = nullptr;
        
        RHI::SamplerDescriptor samplerDesc{};
        samplerDesc.addressModeU = samplerDesc.addressModeV = samplerDesc.addressModeW = RHI::SamplerAddressMode::ClampToEdge;
        samplerDesc.enableAnisotropy = true;
        samplerDesc.maxAnisotropy = 8;
        samplerDesc.samplerFilterMode = RHI::FilterMode::Linear;
        
        defaultSampler = RHI::gDynamicRHI->CreateSampler(samplerDesc);
	}

	void VulkanSandbox::InitPipelines()
	{
		// Load Cube mesh at first
		cubeModel = RPI::ModelLod::CreateCubeModel();
		sphereModel = RPI::ModelLod::CreateSphereModel();

		// Mesh SRG
		{
			RHI::ShaderResourceGroupLayout meshSrgLayout{};
			meshSrgLayout.srgType = RHI::SRGType::PerObject;

			RHI::SRGVariableDescriptor variable{};
			variable.name = "_ObjectData";
			variable.bindingSlot = perObjectDataBinding;
			variable.arrayCount = 1;
			variable.shaderStages = RHI::ShaderStage::Vertex;
			variable.type = RHI::ShaderResourceType::ConstantBuffer;

			meshSrgLayout.variables.Add(variable);

			sphereObjectSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(meshSrgLayout);

			RHI::BufferDescriptor desc{};
			desc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
			desc.bufferSize = sizeof(Matrix4x4);
			desc.defaultHeapType = RHI::MemoryHeapType::Upload;
			desc.name = "_ObjectData";

			cubeObjectBuffer = RHI::gDynamicRHI->CreateBuffer(desc);

			meshModelMatrix = Matrix4x4::Translation(Vec3(0, 0, 15)) * Quat::EulerDegrees(Vec3(0, meshRotation, 0)).ToMatrix() * Matrix4x4::Scale(Vec3(1, 1, 1));

			RHI::BufferData uploadData{};
			uploadData.dataSize = desc.bufferSize;
			uploadData.data = &meshModelMatrix;
			uploadData.startOffsetInBuffer = 0;

			cubeObjectBuffer->UploadData(uploadData);

			sphereObjectSrg->Bind("_ObjectData", RHI::BufferView(cubeObjectBuffer));

			sphereObjectSrg->FlushBindings();
		}

		// Per View SRG
		{
			RHI::ShaderResourceGroupLayout perViewSrgLayout{};
			perViewSrgLayout.srgType = RHI::SRGType::PerView;

			RHI::SRGVariableDescriptor perViewDataDesc{};
			perViewDataDesc.bindingSlot = perViewDataBinding;
			perViewDataDesc.arrayCount = 1;
			perViewDataDesc.name = "_PerViewData";
			perViewDataDesc.type = RHI::ShaderResourceType::ConstantBuffer;
			perViewDataDesc.shaderStages = RHI::ShaderStage::Vertex;

			perViewSrgLayout.variables.Add(perViewDataDesc);

			depthPerViewSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(perViewSrgLayout);

			perViewSrgLayout.variables.Top().shaderStages |= RHI::ShaderStage::Fragment;
			perViewSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(perViewSrgLayout);
			
			RHI::BufferDescriptor bufferDesc{};
			bufferDesc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
			bufferDesc.bufferSize = sizeof(PerViewData);
			bufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;
			bufferDesc.name = "PerViewData";

			perViewBuffer = RHI::gDynamicRHI->CreateBuffer(bufferDesc);

			perViewSrg->Bind("_PerViewData", perViewBuffer);
			depthPerViewSrg->Bind("_PerViewData", perViewBuffer);
			
			perViewSrg->FlushBindings();
			depthPerViewSrg->FlushBindings();
		}

		// Depth Pipeline
		{
			Resource* depthVert = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/Depth.vert.spv", nullptr);
			RHI::ShaderModuleDescriptor vertDesc{};
			vertDesc.name = "Depth Vertex";
			vertDesc.stage = RHI::ShaderStage::Vertex;
			vertDesc.byteCode = depthVert->GetData();
			vertDesc.byteSize = depthVert->GetDataSize();

			depthShaderVert = RHI::gDynamicRHI->CreateShaderModule(vertDesc);

			RHI::GraphicsPipelineDescriptor depthPipelineDesc{};
			RHI::ColorBlendState colorBlend{};
			colorBlend.alphaBlendOp = RHI::BlendOp::Add;
			colorBlend.colorBlendOp = RHI::BlendOp::Add;
			colorBlend.componentMask = RHI::ColorComponentMask::All;
			colorBlend.srcColorBlend = RHI::BlendFactor::SrcAlpha;
			colorBlend.dstColorBlend = RHI::BlendFactor::OneMinusSrcAlpha;
			colorBlend.srcAlphaBlend = RHI::BlendFactor::One;
			colorBlend.dstAlphaBlend = RHI::BlendFactor::Zero;
			colorBlend.blendEnable = true;
			depthPipelineDesc.blendState.colorBlends.Add(colorBlend);

			depthPipelineDesc.depthStencilState.depthState.enable = true;
			depthPipelineDesc.depthStencilState.depthState.testEnable = true;
			depthPipelineDesc.depthStencilState.depthState.writeEnable = true;
			depthPipelineDesc.depthStencilState.depthState.compareOp = RHI::CompareOp::LessOrEqual;
			depthPipelineDesc.depthStencilState.stencilState.enable = false;
			
			depthPipelineDesc.multisampleState.sampleCount = 1;
			depthPipelineDesc.multisampleState.quality = 0;

			depthPipelineDesc.shaderStages.Add({});
			depthPipelineDesc.shaderStages[0].entryPoint = "VertMain";
			depthPipelineDesc.shaderStages[0].shaderModule = depthShaderVert;

			depthPipelineDesc.rasterState = {};
			depthPipelineDesc.rasterState.cullMode = RHI::CullMode::None;

			depthPipelineDesc.vertexInputSlots.Add({});
			depthPipelineDesc.vertexInputSlots[0].inputRate = RHI::VertexInputRate::PerVertex;
			depthPipelineDesc.vertexInputSlots[0].inputSlot = 0;
			depthPipelineDesc.vertexInputSlots[0].stride = sizeof(Vec3);

			Array<RHI::VertexAttributeDescriptor>& vertexAttribs = depthPipelineDesc.vertexAttributes;
			
			vertexAttribs.Add({});
			vertexAttribs[0].dataType = RHI::VertexAttributeDataType::Float3;
			vertexAttribs[0].inputSlot = 0;
			vertexAttribs[0].location = 0;
			vertexAttribs[0].offset = 0;

			Array<RHI::ShaderResourceGroupLayout>& srgLayouts = depthPipelineDesc.srgLayouts;
			RHI::ShaderResourceGroupLayout perViewSRGLayout{};
			perViewSRGLayout.srgType = RHI::SRGType::PerView;
			perViewSRGLayout.variables.Add({});
			perViewSRGLayout.variables[0].arrayCount = 1;
			perViewSRGLayout.variables[0].name = "_PerViewData";
			perViewSRGLayout.variables[0].bindingSlot = perViewDataBinding;
			perViewSRGLayout.variables[0].type = RHI::ShaderResourceType::ConstantBuffer;
			perViewSRGLayout.variables[0].shaderStages = RHI::ShaderStage::Vertex;
			srgLayouts.Add(perViewSRGLayout);
			
			RHI::ShaderResourceGroupLayout perObjectSRGLayout{};
			perObjectSRGLayout.srgType = RHI::SRGType::PerObject;
			perObjectSRGLayout.variables.Add({});
			perObjectSRGLayout.variables[0].arrayCount = 1;
			perObjectSRGLayout.variables[0].name = "_ObjectData";
			perObjectSRGLayout.variables[0].bindingSlot = perObjectDataBinding;
			perObjectSRGLayout.variables[0].type = RHI::ShaderResourceType::ConstantBuffer;
			perObjectSRGLayout.variables[0].shaderStages = RHI::ShaderStage::Vertex;
			srgLayouts.Add(perObjectSRGLayout);

			depthPipelineDesc.name = "Depth Pipeline";

			depthPipeline = RHI::gDynamicRHI->CreateGraphicsPipeline(depthPipelineDesc);

			delete depthVert;
		}

		// Skybox Pipeline
		{
			Resource* skyboxVert = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/Skybox.vert.spv", nullptr);
			Resource* skyboxFrag = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/Skybox.frag.spv", nullptr);

			RHI::ShaderModuleDescriptor vertDesc{};
			vertDesc.name = "Skybox Vertex";
			vertDesc.stage = RHI::ShaderStage::Vertex;
			vertDesc.byteCode = skyboxVert->GetData();
			vertDesc.byteSize = skyboxVert->GetDataSize();

			RHI::ShaderModuleDescriptor fragDesc{};
			fragDesc.name = "Skybox Fragment";
			fragDesc.stage = RHI::ShaderStage::Fragment;
			fragDesc.byteCode = skyboxFrag->GetData();
			fragDesc.byteSize = skyboxFrag->GetDataSize();

			auto skyboxShaderVert = RHI::gDynamicRHI->CreateShaderModule(vertDesc);
			auto skyboxShaderFrag = RHI::gDynamicRHI->CreateShaderModule(fragDesc);

			RHI::GraphicsPipelineDescriptor skyboxPipelineDesc{};

			RHI::ColorBlendState colorBlend{};
			colorBlend.alphaBlendOp = RHI::BlendOp::Add;
			colorBlend.colorBlendOp = RHI::BlendOp::Add;
			colorBlend.componentMask = RHI::ColorComponentMask::All;
			colorBlend.srcColorBlend = RHI::BlendFactor::SrcAlpha;
			colorBlend.dstColorBlend = RHI::BlendFactor::OneMinusSrcAlpha;
			colorBlend.srcAlphaBlend = RHI::BlendFactor::One;
			colorBlend.dstAlphaBlend = RHI::BlendFactor::Zero;
			colorBlend.blendEnable = true;
			skyboxPipelineDesc.blendState.colorBlends.Add(colorBlend);

			skyboxPipelineDesc.depthStencilState.depthState.enable = false;

			skyboxPipelineDesc.shaderStages.Add({});
			skyboxPipelineDesc.shaderStages.Top().entryPoint = "VertMain";
			skyboxPipelineDesc.shaderStages.Top().shaderModule = skyboxShaderVert;
			skyboxPipelineDesc.shaderStages.Add({});
			skyboxPipelineDesc.shaderStages.Top().entryPoint = "FragMain";
			skyboxPipelineDesc.shaderStages.Top().shaderModule = skyboxShaderFrag;

			skyboxPipelineDesc.rasterState = {};
			skyboxPipelineDesc.rasterState.cullMode = RHI::CullMode::Front;

			skyboxPipelineDesc.vertexInputSlots.Add({});
			skyboxPipelineDesc.vertexInputSlots.Top().inputRate = RHI::VertexInputRate::PerVertex;
			skyboxPipelineDesc.vertexInputSlots.Top().inputSlot = 0;
			skyboxPipelineDesc.vertexInputSlots.Top().stride = sizeof(Vec3); // float3 Position;

			Array<RHI::VertexAttributeDescriptor>& vertexAttribs = skyboxPipelineDesc.vertexAttributes;

			vertexAttribs.Add({});
			vertexAttribs[0].dataType = RHI::VertexAttributeDataType::Float3; // Position
			vertexAttribs[0].inputSlot = 0;
			vertexAttribs[0].location = 0;
			vertexAttribs[0].offset = 0;

			Array<RHI::ShaderResourceGroupLayout>& srgLayouts = skyboxPipelineDesc.srgLayouts;
			RHI::ShaderResourceGroupLayout perViewSRGLayout{};
			perViewSRGLayout.srgType = RHI::SRGType::PerView;
			perViewSRGLayout.variables.Add({});
			perViewSRGLayout.variables[0].arrayCount = 1;
			perViewSRGLayout.variables[0].name = "_PerViewData";
			perViewSRGLayout.variables[0].bindingSlot = perViewDataBinding;
			perViewSRGLayout.variables[0].type = RHI::ShaderResourceType::ConstantBuffer;
			perViewSRGLayout.variables[0].shaderStages = RHI::ShaderStage::Vertex | RHI::ShaderStage::Fragment;
			srgLayouts.Add(perViewSRGLayout);
			

			RHI::ShaderResourceGroupLayout perObjectSRGLayout{};
			perObjectSRGLayout.srgType = RHI::SRGType::PerObject;
			perObjectSRGLayout.variables.Add({});
			perObjectSRGLayout.variables[0].arrayCount = 1;
			perObjectSRGLayout.variables[0].name = "_ObjectData";
			perObjectSRGLayout.variables[0].bindingSlot = perObjectDataBinding;
			perObjectSRGLayout.variables[0].type = RHI::ShaderResourceType::ConstantBuffer;
			perObjectSRGLayout.variables[0].shaderStages = RHI::ShaderStage::Vertex;
			srgLayouts.Add(perObjectSRGLayout);

			RHI::ShaderResourceGroupLayout perSceneSRGLayout{};
			perSceneSRGLayout.srgType = RHI::SRGType::PerScene;

			perSceneSRGLayout.variables.Add({});
			perSceneSRGLayout.variables.Top().arrayCount = 1;
			perSceneSRGLayout.variables.Top().name = "_Skybox";
			perSceneSRGLayout.variables.Top().bindingSlot = skyboxBinding;
			perSceneSRGLayout.variables.Top().type = RHI::ShaderResourceType::TextureCube;
			perSceneSRGLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			perSceneSRGLayout.variables.Add({});
			perSceneSRGLayout.variables.Top().arrayCount = 1;
			perSceneSRGLayout.variables.Top().name = "_DefaultSampler";
			perSceneSRGLayout.variables.Top().bindingSlot = defaultSamplerBinding;
			perSceneSRGLayout.variables.Top().type = RHI::ShaderResourceType::SamplerState;
			perSceneSRGLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			srgLayouts.Add(perSceneSRGLayout);

			skyboxPipelineDesc.name = "Skybox Pipeline";

			skyboxShader = new RPI::Shader();
			RPI::ShaderVariantDescriptor variant{};
			variant.pipelineDesc = skyboxPipelineDesc;
			skyboxShader->AddVariant(variant);

			skyboxMaterial = new RPI::Material(skyboxShader);

			skyboxPerSceneSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(perSceneSRGLayout);

			skyboxPerSceneSrg->Bind("_Skybox", skyboxCubeMap);
			skyboxPerSceneSrg->Bind("_DefaultSampler", defaultSampler);

			skyboxPerSceneSrg->FlushBindings();

			RHI::BufferDescriptor desc{};
			desc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
			desc.bufferSize = sizeof(Matrix4x4);
			desc.defaultHeapType = RHI::MemoryHeapType::Upload;
			desc.name = "_ObjectData";

			skyboxObjectBuffer = RHI::gDynamicRHI->CreateBuffer(desc);

			skyboxModelMatrix = Matrix4x4::Translation(Vec3()) * Quat::EulerDegrees(Vec3(0, 0, 0)).ToMatrix() * Matrix4x4::Scale(Vec3(1000, 1000, 1000));

			skyboxObjectBuffer->UploadData(&skyboxObjectBuffer, sizeof(skyboxModelMatrix));

			skyboxObjectSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(perObjectSRGLayout);
			skyboxObjectSrg->Bind("_ObjectData", skyboxObjectBuffer);
			skyboxObjectSrg->FlushBindings();

			delete skyboxVert; delete skyboxFrag;
		}

		// Opaque Pipeline
		{
			Resource* opaqueVert = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/Opaque.vert.spv", nullptr);
			Resource* opaqueFrag = GetResourceManager()->LoadResource("/" MODULE_NAME "/Resources/Shaders/Opaque.frag.spv", nullptr);

			RHI::ShaderModuleDescriptor vertDesc{};
			vertDesc.name = "Opaque Vertex";
			vertDesc.stage = RHI::ShaderStage::Vertex;
			vertDesc.byteCode = opaqueVert->GetData();
			vertDesc.byteSize = opaqueVert->GetDataSize();

			RHI::ShaderModuleDescriptor fragDesc{};
			fragDesc.name = "Opaque Fragment";
			fragDesc.stage = RHI::ShaderStage::Fragment;
			fragDesc.byteCode = opaqueFrag->GetData();
			fragDesc.byteSize = opaqueFrag->GetDataSize();

			auto opaqueShaderVert = RHI::gDynamicRHI->CreateShaderModule(vertDesc);
			auto opaqueShaderFrag = RHI::gDynamicRHI->CreateShaderModule(fragDesc);
            
            opaqueShader = new RPI::Shader();

			RHI::GraphicsPipelineDescriptor opaquePipelineDesc{};
			
			RHI::ColorBlendState colorBlend{};
			colorBlend.alphaBlendOp = RHI::BlendOp::Add;
			colorBlend.colorBlendOp = RHI::BlendOp::Add;
			colorBlend.componentMask = RHI::ColorComponentMask::All;
			colorBlend.srcColorBlend = RHI::BlendFactor::SrcAlpha;
			colorBlend.dstColorBlend = RHI::BlendFactor::OneMinusSrcAlpha;
			colorBlend.srcAlphaBlend = RHI::BlendFactor::One;
			colorBlend.dstAlphaBlend = RHI::BlendFactor::Zero;
			colorBlend.blendEnable = true;
			opaquePipelineDesc.blendState.colorBlends.Add(colorBlend);

			opaquePipelineDesc.depthStencilState.depthState.enable = true; // Read-Only depth state
			opaquePipelineDesc.depthStencilState.depthState.testEnable = true;
			opaquePipelineDesc.depthStencilState.depthState.writeEnable = false;
			opaquePipelineDesc.depthStencilState.depthState.compareOp = RHI::CompareOp::LessOrEqual;
			opaquePipelineDesc.depthStencilState.stencilState.enable = false;

			opaquePipelineDesc.shaderStages.Add({});
			opaquePipelineDesc.shaderStages[0].entryPoint = "VertMain";
			opaquePipelineDesc.shaderStages[0].shaderModule = opaqueShaderVert;

			opaquePipelineDesc.shaderStages.Add({});
			opaquePipelineDesc.shaderStages[1].entryPoint = "FragMain";
			opaquePipelineDesc.shaderStages[1].shaderModule = opaqueShaderFrag;

			opaquePipelineDesc.rasterState = {};
			opaquePipelineDesc.rasterState.cullMode = RHI::CullMode::Back;

			opaquePipelineDesc.vertexInputSlots.Add({});
			opaquePipelineDesc.vertexInputSlots.Top().inputRate = RHI::VertexInputRate::PerVertex;
			opaquePipelineDesc.vertexInputSlots.Top().inputSlot = 0;
			opaquePipelineDesc.vertexInputSlots.Top().stride = sizeof(Vec3); // float3 Position;

			opaquePipelineDesc.vertexInputSlots.Add({});
			opaquePipelineDesc.vertexInputSlots.Top().inputRate = RHI::VertexInputRate::PerVertex;
			opaquePipelineDesc.vertexInputSlots.Top().inputSlot = 1;
			opaquePipelineDesc.vertexInputSlots.Top().stride = sizeof(Vec3); // float3 Normal;

			Array<RHI::VertexAttributeDescriptor>& vertexAttribs = opaquePipelineDesc.vertexAttributes;

			vertexAttribs.Add({});
			vertexAttribs[0].dataType = RHI::VertexAttributeDataType::Float3; // Position
			vertexAttribs[0].inputSlot = 0;
			vertexAttribs[0].location = 0;
			vertexAttribs[0].offset = 0;

			vertexAttribs.Add({});
			vertexAttribs[1].dataType = RHI::VertexAttributeDataType::Float3; // Normal
			vertexAttribs[1].inputSlot = 1;
			vertexAttribs[1].location = 1;
			vertexAttribs[1].offset = 0;

			Array<RHI::ShaderResourceGroupLayout>& srgLayouts = opaquePipelineDesc.srgLayouts;
			RHI::ShaderResourceGroupLayout perViewSRGLayout{};
			perViewSRGLayout.srgType = RHI::SRGType::PerView;
			perViewSRGLayout.variables.Add({});
			perViewSRGLayout.variables[0].arrayCount = 1;
			perViewSRGLayout.variables[0].name = "_PerViewData";
			perViewSRGLayout.variables[0].bindingSlot = perViewDataBinding;
			perViewSRGLayout.variables[0].type = RHI::ShaderResourceType::ConstantBuffer;
			perViewSRGLayout.variables[0].shaderStages = RHI::ShaderStage::Vertex | RHI::ShaderStage::Fragment;
			srgLayouts.Add(perViewSRGLayout);

			RHI::ShaderResourceGroupLayout perObjectSRGLayout{};
			perObjectSRGLayout.srgType = RHI::SRGType::PerObject;
			perObjectSRGLayout.variables.Add({});
			perObjectSRGLayout.variables[0].arrayCount = 1;
			perObjectSRGLayout.variables[0].name = "_ObjectData";
			perObjectSRGLayout.variables[0].bindingSlot = perObjectDataBinding;
			perObjectSRGLayout.variables[0].type = RHI::ShaderResourceType::ConstantBuffer;
			perObjectSRGLayout.variables[0].shaderStages = RHI::ShaderStage::Vertex;
			srgLayouts.Add(perObjectSRGLayout);

			RHI::ShaderResourceGroupLayout perSceneSRGLayout{};
			perSceneSRGLayout.srgType = RHI::SRGType::PerScene;

			perSceneSRGLayout.variables.Add({});
			perSceneSRGLayout.variables.Top().arrayCount = 1;
			perSceneSRGLayout.variables.Top().name = "_DirectionalLightsArray";
			perSceneSRGLayout.variables.Top().bindingSlot = directionalLightArrayBinding;
			perSceneSRGLayout.variables.Top().type = RHI::ShaderResourceType::ConstantBuffer;
			perSceneSRGLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			perSceneSRGLayout.variables.Add({});
			perSceneSRGLayout.variables.Top().arrayCount = 1;
			perSceneSRGLayout.variables.Top().name = "_PointLights";
			perSceneSRGLayout.variables.Top().bindingSlot = pointLightsBinding;
			perSceneSRGLayout.variables.Top().type = RHI::ShaderResourceType::StructuredBuffer;
			perSceneSRGLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			perSceneSRGLayout.variables.Add({});
			perSceneSRGLayout.variables.Top().arrayCount = 1;
			perSceneSRGLayout.variables.Top().name = "_LightData";
			perSceneSRGLayout.variables.Top().bindingSlot = lightDataBinding;
			perSceneSRGLayout.variables.Top().type = RHI::ShaderResourceType::ConstantBuffer;
			perSceneSRGLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			srgLayouts.Add(perSceneSRGLayout);

			RHI::ShaderResourceGroupLayout perMaterialSRGLayout{};
			perMaterialSRGLayout.srgType = RHI::SRGType::PerMaterial;
			perMaterialSRGLayout.variables.Add({});
			perMaterialSRGLayout.variables.Top().arrayCount = 1;
			perMaterialSRGLayout.variables.Top().name = "_MaterialData";
			perMaterialSRGLayout.variables.Top().bindingSlot = materialDataBinding;
			perMaterialSRGLayout.variables.Top().type = RHI::ShaderResourceType::ConstantBuffer;
			perMaterialSRGLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;
			// Struct Members
			{
				RHI::ShaderStructMember albedoMember{};
				albedoMember.dataType = RHI::ShaderStructMemberType::Float4;
				albedoMember.name = "_Albedo";
				perMaterialSRGLayout.variables.Top().structMembers.Add(albedoMember);

				RHI::ShaderStructMember specularMember{};
				specularMember.dataType = RHI::ShaderStructMemberType::Float;
				specularMember.name = "_SpecularStrength";
				perMaterialSRGLayout.variables.Top().structMembers.Add(specularMember);

				RHI::ShaderStructMember shininessMember{};
				shininessMember.dataType = RHI::ShaderStructMemberType::UInt;
				shininessMember.name = "_Shininess";
				perMaterialSRGLayout.variables.Top().structMembers.Add(shininessMember);
			}

			srgLayouts.Add(perMaterialSRGLayout);

			opaquePipelineDesc.name = "Opaque Pipeline";
            
            RPI::ShaderVariantDescriptor variantDesc{};
            variantDesc.defineFlags = {};
            variantDesc.pipelineDesc = opaquePipelineDesc;
            opaqueShader->AddVariant(variantDesc);

			opaqueMaterial = new RPI::Material(opaqueShader);
			
            opaqueMaterial->SetPropertyValue("_Albedo", Color(0.5f, 0.5f, 0.25f, 1.0f));
			opaqueMaterial->SetPropertyValue("_SpecularStrength", 1.0f);
			opaqueMaterial->SetPropertyValue("_Shininess", (u32)64);
            opaqueMaterial->FlushProperties();

			delete opaqueVert;
			delete opaqueFrag;
		}

		// Transparent Pipeline
		{

		}

		UpdatePerViewData();
	}

	void VulkanSandbox::InitLights()
	{
		DirectionalLight mainLight{};
		mainLight.direction = Vec3(-0.5f, -0.25f, 1);
		mainLight.colorAndIntensity = Vec4(1.0f, 0.95f, 0.7f);
		mainLight.colorAndIntensity.w = 1.0f; // intensity
		mainLight.temperature = 100;

		directionalLights.Clear();
		directionalLights.Add(mainLight);

		PointLight pointLight{};
		pointLight.position = Vec4(0, 0, 0);
		pointLight.colorAndIntensity = Vec3(1.0f, 0.5f, 0);
		pointLight.colorAndIntensity.w = 1.0f; // Intensity
		pointLight.radius = 35.0f;

		pointLights.Clear();
		pointLights.Add(pointLight);
		
		lightData.totalDirectionalLights = 0;// directionalLights.GetSize();
		lightData.ambientColor = Color(0, 0.1f, 0.5f, 1).ToVec4();
		lightData.totalPointLights = pointLights.GetSize();

		{
			RHI::BufferDescriptor bufferDesc{};
			bufferDesc.bindFlags = RHI::BufferBindFlags::StructuredBuffer;
			bufferDesc.bufferSize = pointLights.GetSize() * sizeof(PointLight);
			bufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;
			bufferDesc.name = "Point Lights Buffer";

			pointLightsBuffer = RHI::gDynamicRHI->CreateBuffer(bufferDesc);

			pointLightsBuffer->UploadData(pointLights.GetData(), bufferDesc.bufferSize);
		}

		{
			RHI::BufferDescriptor bufferDesc{};
			bufferDesc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
            bufferDesc.bufferSize = directionalLights.GetCapacity() * sizeof(DirectionalLight);
			bufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;
			bufferDesc.name = "Directional Lights Buffer";

			directionalLightsBuffer = RHI::gDynamicRHI->CreateBuffer(bufferDesc);

			directionalLightsBuffer->UploadData(directionalLights.GetData(), bufferDesc.bufferSize);
		}

		{
			RHI::BufferDescriptor bufferDesc{};
			bufferDesc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
			bufferDesc.bufferSize = sizeof(LightData);
			bufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;
			bufferDesc.name = "Light Data Buffer";

			lightDataBuffer = RHI::gDynamicRHI->CreateBuffer(bufferDesc);

			lightDataBuffer->UploadData(&lightData, bufferDesc.bufferSize);
		}

		RHI::ShaderResourceGroupLayout perSceneSRGLayout{};
		perSceneSRGLayout.srgType = RHI::SRGType::PerScene;

		perSceneSRGLayout.variables.Add({});
		perSceneSRGLayout.variables.Top().arrayCount = 1;
		perSceneSRGLayout.variables.Top().name = "_DirectionalLightsArray";
		perSceneSRGLayout.variables.Top().bindingSlot = directionalLightArrayBinding;
		perSceneSRGLayout.variables.Top().type = RHI::ShaderResourceType::ConstantBuffer;
		perSceneSRGLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

		perSceneSRGLayout.variables.Add({});
		perSceneSRGLayout.variables.Top().arrayCount = 1;
		perSceneSRGLayout.variables.Top().name = "_PointLights";
		perSceneSRGLayout.variables.Top().bindingSlot = pointLightsBinding;
		perSceneSRGLayout.variables.Top().type = RHI::ShaderResourceType::StructuredBuffer;
		perSceneSRGLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

		perSceneSRGLayout.variables.Add({});
		perSceneSRGLayout.variables.Top().arrayCount = 1;
		perSceneSRGLayout.variables.Top().name = "_LightData";
		perSceneSRGLayout.variables.Top().bindingSlot = lightDataBinding;
		perSceneSRGLayout.variables.Top().type = RHI::ShaderResourceType::ConstantBuffer;
		perSceneSRGLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

		perSceneSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(perSceneSRGLayout);

		perSceneSrg->Bind("_DirectionalLightsArray", directionalLightsBuffer);
		perSceneSrg->Bind("_PointLights", pointLightsBuffer);
		perSceneSrg->Bind("_LightData", lightDataBuffer);

		perSceneSrg->FlushBindings();
	}

	void VulkanSandbox::BuildCubeMeshDrawPacket()
	{
		RHI::DrawPacketBuilder builder{};

		builder.SetDrawArguments(sphereModel->GetMesh(0)->drawArguments);

		builder.AddShaderResourceGroup(sphereObjectSrg);

		RPI::Mesh* mesh = sphereModel->GetMesh(0);

		// Depth Item
		{
			RHI::DrawPacketBuilder::DrawItemRequest request{};
			const auto& vertInfo = mesh->vertexBufferInfos[0];
			auto vertBufferView = RHI::VertexBufferView(sphereModel->GetBuffer(vertInfo.bufferIndex), vertInfo.byteOffset, vertInfo.byteCount, vertInfo.stride);
			request.vertexBufferViews.Add(vertBufferView);
			request.indexBufferView = mesh->indexBufferView;

			request.drawItemTag = rhiSystem.GetDrawListTagRegistry()->AcquireTag("depth");
			request.drawFilterMask = RHI::DrawFilterMask::ALL;
			request.pipelineState = depthPipeline;

			builder.AddDrawItem(request);
		}

		// Opaque Item
		{
			RHI::DrawPacketBuilder::DrawItemRequest request{};
			for (const auto& vertInfo : mesh->vertexBufferInfos)
			{
				if (vertInfo.semantic.attribute != RHI::VertexInputAttribute::Position &&
					vertInfo.semantic.attribute != RHI::VertexInputAttribute::Normal)
					continue;

				auto vertBufferView = RHI::VertexBufferView(sphereModel->GetBuffer(vertInfo.bufferIndex), vertInfo.byteOffset, vertInfo.byteCount, vertInfo.stride);
				request.vertexBufferViews.Add(vertBufferView);
			}
			request.indexBufferView = mesh->indexBufferView;

			request.drawItemTag = rhiSystem.GetDrawListTagRegistry()->AcquireTag("opaque");
			request.drawFilterMask = RHI::DrawFilterMask::ALL;
			request.pipelineState = opaqueMaterial->GetCurrentShader()->GetPipeline();

			builder.AddDrawItem(request);
		}

		// Transparent Item
		{

		}

		meshDrawPacket = builder.Build();
	}

	void VulkanSandbox::BuildSkyboxDrawPacket()
	{
		RHI::DrawPacketBuilder builder{};

		builder.SetDrawArguments(cubeModel->GetMesh(0)->drawArguments);

		builder.AddShaderResourceGroup(skyboxPerSceneSrg);
		builder.AddShaderResourceGroup(skyboxObjectSrg);

		RPI::Mesh* mesh = cubeModel->GetMesh(0);

		// Skybox Item
		{
			RHI::DrawPacketBuilder::DrawItemRequest request{};
			const auto& vertInfo = mesh->vertexBufferInfos[0];
			auto vertBufferView = RHI::VertexBufferView(sphereModel->GetBuffer(vertInfo.bufferIndex), vertInfo.byteOffset, vertInfo.byteCount, vertInfo.stride);
			request.vertexBufferViews.Add(vertBufferView);
			request.indexBufferView = mesh->indexBufferView;

			request.drawItemTag = rhiSystem.GetDrawListTagRegistry()->AcquireTag("skybox");
			request.drawFilterMask = RHI::DrawFilterMask::ALL;
			request.pipelineState = skyboxMaterial->GetCurrentShader()->GetPipeline();

			builder.AddDrawItem(request);
		}

		skyboxDrawPacket = builder.Build();
	}

	void VulkanSandbox::InitDrawPackets()
	{
		BuildCubeMeshDrawPacket();
		BuildSkyboxDrawPacket();
	}

	void VulkanSandbox::DestroyDrawPackets()
	{
		scheduler->WaitUntilIdle();
		
		delete meshDrawPacket; meshDrawPacket = nullptr;
		delete skyboxDrawPacket; skyboxDrawPacket = nullptr;
	}

	void VulkanSandbox::DestroyLights()
	{
		scheduler->WaitUntilIdle();

		delete perSceneSrg; perSceneSrg = nullptr;

		delete pointLightsBuffer; pointLightsBuffer = nullptr;
		delete directionalLightsBuffer; directionalLightsBuffer = nullptr;
		delete lightDataBuffer; lightDataBuffer = nullptr;
	}

	void VulkanSandbox::DestroyPipelines()
	{
		delete cubeModel; cubeModel = nullptr;
		delete sphereModel; sphereModel = nullptr;

		delete cubeObjectBuffer; cubeObjectBuffer = nullptr;
		delete sphereObjectSrg; sphereObjectSrg = nullptr;

		delete depthPerViewSrg; depthPerViewSrg = nullptr;
		delete perViewSrg; perViewSrg = nullptr;
		delete perViewBuffer; perViewBuffer = nullptr;

		delete depthPipeline; depthPipeline = nullptr;
		delete depthShaderVert; depthShaderVert = nullptr;

		delete skyboxObjectBuffer; skyboxObjectBuffer = nullptr;
		delete skyboxShader; skyboxShader = nullptr;
		delete skyboxMaterial; skyboxMaterial = nullptr;
		delete skyboxObjectSrg; skyboxObjectSrg = nullptr;
		delete skyboxPerSceneSrg; skyboxPerSceneSrg = nullptr;

        delete opaqueShader; opaqueShader = nullptr;
		delete opaqueMaterial; opaqueMaterial = nullptr;

		delete transparentPipeline; transparentPipeline = nullptr;
	}

	void VulkanSandbox::DestroyCubeMaps()
	{
        delete defaultSampler; defaultSampler = nullptr;
		delete skyboxCubeMap; skyboxCubeMap = nullptr;
	}

	void VulkanSandbox::BuildFrameGraph()
	{
		rebuild = false;
		recompile = true;

		scheduler->BeginFrameGraph();
		{
			RHI::FrameAttachmentDatabase& attachmentDatabase = scheduler->GetFrameAttachmentDatabase();

			RHI::ImageDescriptor depthDesc{};
			depthDesc.width = swapChain->GetWidth();
			depthDesc.height = swapChain->GetHeight();
			depthDesc.bindFlags = RHI::TextureBindFlags::Depth;
			depthDesc.format = RHI::gDynamicRHI->GetAvailableDepthOnlyFormats()[0];
			depthDesc.name = "DepthStencil";

			attachmentDatabase.EmplaceFrameAttachment("DepthStencil", depthDesc);
			attachmentDatabase.EmplaceFrameAttachment("SwapChain", swapChain);

			//scheduler->BeginScopeGroup("MainPass");
			scheduler->BeginScope("Skybox");
			{
				RHI::ImageScopeAttachmentDescriptor swapChainAttachment{};
				swapChainAttachment.attachmentId = "SwapChain";
				swapChainAttachment.loadStoreAction.clearValue = Vec4(0, 0.5f, 0.5f, 1);
				swapChainAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Clear;
				swapChainAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;
				scheduler->UseAttachment(swapChainAttachment, RHI::ScopeAttachmentUsage::RenderTarget, RHI::ScopeAttachmentAccess::Write);
				//scheduler->UseShaderResourceGroup(skyboxPerSceneSrg);
				//scheduler->UseShaderResourceGroup(perViewSrg);
				
				//scheduler->UsePipeline(skyboxMaterial->GetCurrentShader()->GetPipeline());
			}
			scheduler->EndScope();
			
			scheduler->BeginScope("Depth");
			{
				RHI::ImageScopeAttachmentDescriptor depthAttachment{};
				depthAttachment.attachmentId = "DepthStencil";
				depthAttachment.loadStoreAction.clearValueDepth = 1.0f;
				depthAttachment.loadStoreAction.clearValueStencil = 0;
				depthAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Clear;
				depthAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;

				scheduler->UseAttachment(depthAttachment, RHI::ScopeAttachmentUsage::DepthStencil, RHI::ScopeAttachmentAccess::Write);
				
				scheduler->UseShaderResourceGroup(depthPerViewSrg);

				scheduler->UsePipeline(depthPipeline);
			}
			scheduler->EndScope();

			scheduler->BeginScope("Opaque");
			{
				RHI::ImageScopeAttachmentDescriptor depthAttachment{};
				depthAttachment.attachmentId = "DepthStencil";
				depthAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Load;
				depthAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;

				scheduler->UseAttachment(depthAttachment, RHI::ScopeAttachmentUsage::DepthStencil, RHI::ScopeAttachmentAccess::Read);

				RHI::ImageScopeAttachmentDescriptor swapChainAttachment{};
				swapChainAttachment.attachmentId = "SwapChain";
				swapChainAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Load;
				//swapChainAttachment.loadStoreAction.clearValue = Vec4(0, 0.5f, 0.5f, 1);
				//swapChainAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Clear;
				swapChainAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;

				scheduler->UseAttachment(swapChainAttachment, RHI::ScopeAttachmentUsage::RenderTarget, RHI::ScopeAttachmentAccess::Write);

				scheduler->UseShaderResourceGroup(perSceneSrg);
				scheduler->UseShaderResourceGroup(perViewSrg);
                scheduler->UseShaderResourceGroup(opaqueMaterial->GetShaderResourceGroup());

				scheduler->UsePipeline(opaqueMaterial->GetCurrentShader()->GetPipeline());

				scheduler->PresentSwapChain(swapChain);
			}
			scheduler->EndScope();
			//scheduler->EndScopeGroup();

			/*scheduler->BeginScope("Transparent");
			{
				RHI::ImageScopeAttachmentDescriptor depthAttachment{};
				depthAttachment.attachmentId = "DepthStencil";
				depthAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Load;
				depthAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;

				scheduler->UseAttachment(depthAttachment, RHI::ScopeAttachmentUsage::DepthStencil, RHI::ScopeAttachmentAccess::Read);

				RHI::ImageScopeAttachmentDescriptor swapChainAttachment{};
				swapChainAttachment.attachmentId = "SwapChain";
				swapChainAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Load;
				swapChainAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;

				scheduler->UseAttachment(swapChainAttachment, RHI::ScopeAttachmentUsage::RenderTarget, RHI::ScopeAttachmentAccess::ReadWrite);

				scheduler->PresentSwapChain(swapChain);
			}
			scheduler->EndScope();*/
		}
		scheduler->EndFrameGraph();
	}

	void VulkanSandbox::CompileFrameGraph()
	{
		recompile = false;
		scheduler->Compile();

		RHI::TransientMemoryPool* pool = scheduler->GetTransientPool();
		RHI::MemoryHeap* imageHeap = pool->GetImagePool();
		if (imageHeap != nullptr)
		{
			CE_LOG(Info, All, "Transient Image Pool: {} MB", (imageHeap->GetHeapSize() / 1024.0f / 1024.0f));
		}
	}

	void VulkanSandbox::SubmitWork()
	{
		UpdatePerViewData();

		resubmit = false;
		drawList.Shutdown();
		RHI::DrawListMask drawListMask{};
		auto skyboxTag = rhiSystem.GetDrawListTagRegistry()->AcquireTag("skybox");
		auto depthTag = rhiSystem.GetDrawListTagRegistry()->AcquireTag("depth");
		auto opaqueTag = rhiSystem.GetDrawListTagRegistry()->AcquireTag("opaque");
		//auto transparentTag = rhiSystem.GetDrawListTagRegistry()->AcquireTag("transparent");
		drawListMask.Set(skyboxTag);
		drawListMask.Set(depthTag);
		drawListMask.Set(opaqueTag);
		//drawListMask.Set(transparentTag);
		drawList.Init(drawListMask);
		
		// Add items
		drawList.AddDrawPacket(meshDrawPacket);

		// Finalize
		drawList.Finalize();
		scheduler->SetScopeDrawList("Skybox", &drawList.GetDrawListForTag(skyboxTag));
		scheduler->SetScopeDrawList("Depth", &drawList.GetDrawListForTag(depthTag));
		scheduler->SetScopeDrawList("Opaque", &drawList.GetDrawListForTag(opaqueTag));
		//scheduler->SetScopeDrawList("Transparent", &drawList.GetDrawListForTag(transparentTag));
	}

	void VulkanSandbox::OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight)
	{
		if (window == this->mainWindow)
			rebuild = recompile = true;
	}

	void VulkanSandbox::OnWindowDestroyed(PlatformWindow* window)
	{
		if (window == this->mainWindow)
		{
			destroyed = true;
			this->mainWindow = nullptr;
		}
	}

} // namespace CE
