#include "VulkanSandbox.h"

#include <algorithm>

namespace CE::Sandbox
{
	constexpr u32 directionalLightArrayBinding = 0;
	constexpr u32 pointLightsBinding = 1;
	constexpr u32 lightDataBinding = 2;
	constexpr u32 shadowSamplerBinding = 3;
	constexpr u32 sceneDataBinding = 4;
	constexpr u32 skyboxBinding = 5;
	constexpr u32 defaultSamplerBinding = 6;
	constexpr u32 skyboxIrradianceBinding = 7;

	constexpr u32 perViewDataBinding = 0;
	constexpr u32 perObjectDataBinding = 0;
	constexpr u32 materialDataBinding = 0;
	constexpr u32 albedoTexBinding = 1;
	constexpr u32 roughnessTexBinding = 2;
	constexpr u32 normalTexBinding = 3;
	constexpr u32 metallicTexBinding = 4;
	constexpr u32 directionalShadowMapBinding = 0;

	static int counter = 0;
	static RPI::RPISystem& rpiSystem = RPI::RPISystem::Get();

	void VulkanSandbox::Init(PlatformWindow* window)
	{
		auto prevTime = clock();

		localCounter = counter++;
		rpiSystem.Initialize();

		RHI::FrameSchedulerDescriptor desc{};
		
		scheduler = RHI::FrameScheduler::Create(desc);

		mainWindow = window;

		RHI::SwapChainDescriptor swapChainDesc{};
		swapChainDesc.imageCount = 2;
		swapChainDesc.preferredFormats = { RHI::Format::R8G8B8A8_UNORM, RHI::Format::B8G8R8A8_UNORM };
#if FORCE_SRGB
		swapChainDesc.preferredFormats = { RHI::Format::R8G8B8A8_SRGB, RHI::Format::B8G8R8A8_SRGB };
#endif

		swapChain = RHI::gDynamicRHI->CreateSwapChain(mainWindow, swapChainDesc);

		mainWindow->GetDrawableWindowSize(&width, &height);

		mainWindow->AddListener(this);

		// Load Cube mesh at first
		cubeModel = RPI::ModelLod::CreateCubeModel();
		sphereModel = RPI::ModelLod::CreateSphereModel();
		
		InitCubeMaps();
		InitTextures();
		InitHDRIs();
		InitPipelines();
		InitLights();
		InitDrawPackets();

		BuildFrameGraph();
		CompileFrameGraph();

		f32 timeTaken = ((f32)(clock() - prevTime)) / CLOCKS_PER_SEC;

		CE_LOG(Info, All, "Initialization time: {} seconds", timeTaken);
	}

	float sphereRoughness = 0.0f;

	void VulkanSandbox::Tick(f32 deltaTime)
	{
		if (destroyed)
			return;

		sceneConstants.timeElapsed += deltaTime;

		sceneConstantBuffer->UploadData(&sceneConstants, sizeof(sceneConstants));

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

		u32 imageIndex = scheduler->BeginExecution();

		sphereRoughness += deltaTime * 0.2f;
		if (sphereRoughness >= 1)
			sphereRoughness = 0.001f;

		//sphereMaterial->SetPropertyValue("_Roughness", sphereRoughness);

		skyboxMaterial->FlushProperties(imageIndex);
		cubeMaterial->FlushProperties(imageIndex);
		sphereMaterial->FlushProperties(imageIndex);

		UpdatePerViewData(imageIndex);

		cameraRotation += deltaTime * 20.0f;
		if (cameraRotation >= 360)
			cameraRotation -= 360;
        
		sphereRotation += deltaTime * 15.0f;
		if (sphereRotation >= 360)
			sphereRotation -= 360;
		Vec3 spherePivot = Vec3(0, 0, 5);
		Vec3 spherePos = spherePivot;
		
        //cubeRotation += deltaTime * 5;
        if (cubeRotation >= 360)
            cubeRotation -= 360;
		
        cubeModelMatrix = Matrix4x4::Translation(Vec3(0, -0.75f, 5)) * Quat::EulerDegrees(Vec3(0, cubeRotation, 0)).ToMatrix() * Matrix4x4::Scale(Vec3(5, 0.2f, 5));
        
        cubeObjectBufferPerImage[imageIndex]->UploadData(&cubeModelMatrix, sizeof(cubeModelMatrix), 0);

		sphereModelMatrix = Matrix4x4::Translation(spherePos) * Quat::EulerDegrees(Vec3(0, sphereRotation, 0)).ToMatrix() * Matrix4x4::Scale(Vec3(1, 1, 1));

		RHI::BufferData uploadData{};
		uploadData.dataSize = sizeof(sphereModelMatrix);
		uploadData.data = &sphereModelMatrix;
		uploadData.startOffsetInBuffer = 0;

		sphereObjectBufferPerImage[imageIndex]->UploadData(uploadData);

		scheduler->EndExecution();
	}

	void VulkanSandbox::UpdatePerViewData(int imageIndex)
	{
		float farPlane = 1000.0f;
		
		//cameraRotation = 0;
		Vec3 spherePos = Vec3(0, 0.5f, 5);
		perViewData.viewPosition = Vec3(-2.5f, 0.5f, 5);
		perViewData.viewPosition = Vec3(0, 0.5f, 5) + Vec3(Math::Cos(TO_RADIANS(cameraRotation)), 0, Math::Sin(TO_RADIANS(cameraRotation))) * 2.0f;
		Vec3 camRot = Vec3(0, 0, 0);
		Vec3 lookDir = spherePos - perViewData.viewPosition;
		//camRot = Quat::LookRotation(lookDir) * Vec3(0, 0, 1);

		perViewData.projectionMatrix = Matrix4x4::PerspectiveProjection(swapChain->GetAspectRatio(), 65, 0.1f, farPlane);
		// View matrix has the order inverted compared to model matrix, i.e. order is S * R * T and all positions and rotations are negated.
		//perViewData.viewMatrix = Quat::EulerRadians(-camRot).ToMatrix() * Matrix4x4::Translation(-perViewData.viewPosition);
		perViewData.viewMatrix = Quat::LookRotation(lookDir).ToMatrix() * Matrix4x4::Translation(-perViewData.viewPosition);
		perViewData.viewProjectionMatrix = perViewData.projectionMatrix * perViewData.viewMatrix;

		//perViewData = directionalLightViewData;

		RHI::BufferData data{};
		data.startOffsetInBuffer = 0;
		data.dataSize = perViewBufferPerImage[imageIndex]->GetBufferSize();
		data.data = &perViewData;

		perViewBufferPerImage[imageIndex]->UploadData(data);

		skyboxModelMatrix = Matrix4x4::Translation(Vec3()) * Quat::EulerDegrees(Vec3(0, 0, 0)).ToMatrix() * Matrix4x4::Scale(skyboxScale);

		skyboxObjectBufferPerImage[imageIndex]->UploadData(&skyboxModelMatrix, sizeof(skyboxModelMatrix));
	}

	void VulkanSandbox::Shutdown()
	{
		DestroyLights();
		DestroyDrawPackets();
		DestroyTextures();
		DestroyCubeMaps();
		DestroyHDRIs();

		if (mainWindow)
		{
			mainWindow->RemoveListener(this);
		}

		delete scheduler;
		DestroyPipelines();

		delete swapChain;

		rpiSystem.Shutdown();
	}
	

	void VulkanSandbox::InitCubeMaps()
	{
		
	}

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

	void VulkanSandbox::InitTextures()
	{
		auto prevTime = clock();
		woodFloorTextures = MaterialTextureGroup::Load("WoodFloor");
		auto timeTaken0 = ((f32)(clock() - prevTime)) / CLOCKS_PER_SEC;

		prevTime = clock();
		plasticTextures = MaterialTextureGroup::Load("Plastic");
		auto timeTaken1 = ((f32)(clock() - prevTime)) / CLOCKS_PER_SEC;

		prevTime = clock();
		//rustedTextures = MaterialTextureGroup::Load("RustedIron");
		auto timeTaken2 = ((f32)(clock() - prevTime)) / CLOCKS_PER_SEC;
	}

	void VulkanSandbox::InitPipelines()
	{
		RHI::SamplerDescriptor samplerDesc{};
		samplerDesc.addressModeU = samplerDesc.addressModeV = samplerDesc.addressModeW = RHI::SamplerAddressMode::ClampToEdge;
		samplerDesc.enableAnisotropy = true;
		samplerDesc.maxAnisotropy = 8;
		samplerDesc.samplerFilterMode = RHI::FilterMode::Linear;
		defaultSampler = RHI::gDynamicRHI->CreateSampler(samplerDesc);

		RHI::ShaderResourceGroupLayout meshSrgLayout{};
		meshSrgLayout.srgType = RHI::SRGType::PerObject;

		RHI::SRGVariableDescriptor variable{};
		variable.name = "_ObjectData";
		variable.bindingSlot = perObjectDataBinding;
		variable.arrayCount = 1;
		variable.shaderStages = RHI::ShaderStage::Vertex | RHI::ShaderStage::Fragment;
		variable.type = RHI::ShaderResourceType::ConstantBuffer;

		meshSrgLayout.variables.Add(variable);

		{
			cubeObjectSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(meshSrgLayout);

			cubeModelMatrix = Matrix4x4::Translation(Vec3(0, -0.75f, 15)) * Quat::EulerDegrees(Vec3()).ToMatrix() * Matrix4x4::Scale(Vec3(5, 0.2f, 5));
			
			RHI::BufferDescriptor cubeObjectBufferDesc{};
			cubeObjectBufferDesc.name = "Cube Object Matrix";
			cubeObjectBufferDesc.bufferSize = sizeof(cubeModelMatrix);
			cubeObjectBufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;
			cubeObjectBufferDesc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;

			for (int i = 0; i < RHI::Limits::MaxSwapChainImageCount; i++)
			{
				cubeObjectBufferPerImage[i] = RHI::gDynamicRHI->CreateBuffer(cubeObjectBufferDesc);

				cubeObjectBufferPerImage[i]->UploadData(&cubeModelMatrix, sizeof(cubeModelMatrix));

				cubeObjectSrg->Bind(i, "_ObjectData", RHI::BufferView(cubeObjectBufferPerImage[i]));
			}

			cubeObjectSrg->FlushBindings();
		}

		// Main SRG Layouts
		{
			perSceneSrgLayout.variables.Clear();
			perSceneSrgLayout.srgType = RHI::SRGType::PerScene;

			perSceneSrgLayout.variables.Add({});
			perSceneSrgLayout.variables.Top().arrayCount = 1;
			perSceneSrgLayout.variables.Top().name = "_DirectionalLightsArray";
			perSceneSrgLayout.variables.Top().bindingSlot = directionalLightArrayBinding;
			perSceneSrgLayout.variables.Top().type = RHI::ShaderResourceType::ConstantBuffer;
			perSceneSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			perSceneSrgLayout.variables.Add({});
			perSceneSrgLayout.variables.Top().arrayCount = 1;
			perSceneSrgLayout.variables.Top().name = "_PointLights";
			perSceneSrgLayout.variables.Top().bindingSlot = pointLightsBinding;
			perSceneSrgLayout.variables.Top().type = RHI::ShaderResourceType::StructuredBuffer;
			perSceneSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			perSceneSrgLayout.variables.Add({});
			perSceneSrgLayout.variables.Top().arrayCount = 1;
			perSceneSrgLayout.variables.Top().name = "_LightData";
			perSceneSrgLayout.variables.Top().bindingSlot = lightDataBinding;
			perSceneSrgLayout.variables.Top().type = RHI::ShaderResourceType::ConstantBuffer;
			perSceneSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			perSceneSrgLayout.variables.Add({});
			perSceneSrgLayout.variables.Top().arrayCount = 1;
			perSceneSrgLayout.variables.Top().name = "_ShadowMapSampler";
			perSceneSrgLayout.variables.Top().bindingSlot = shadowSamplerBinding;
			perSceneSrgLayout.variables.Top().type = RHI::ShaderResourceType::SamplerState;
			perSceneSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			perSceneSrgLayout.variables.Add({});
			perSceneSrgLayout.variables.Top().arrayCount = 1;
			perSceneSrgLayout.variables.Top().name = "_Skybox";
			perSceneSrgLayout.variables.Top().bindingSlot = skyboxBinding;
			perSceneSrgLayout.variables.Top().type = RHI::ShaderResourceType::TextureCube;
			perSceneSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			perSceneSrgLayout.variables.Add({});
			perSceneSrgLayout.variables.Top().arrayCount = 1;
			perSceneSrgLayout.variables.Top().name = "_DefaultSampler";
			perSceneSrgLayout.variables.Top().bindingSlot = defaultSamplerBinding;
			perSceneSrgLayout.variables.Top().type = RHI::ShaderResourceType::SamplerState;
			perSceneSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			perSceneSrgLayout.variables.Add({});
			perSceneSrgLayout.variables.Top().arrayCount = 1;
			perSceneSrgLayout.variables.Top().name = "_SkyboxIrradiance";
			perSceneSrgLayout.variables.Top().bindingSlot = skyboxIrradianceBinding;
			perSceneSrgLayout.variables.Top().type = RHI::ShaderResourceType::TextureCube;
			perSceneSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			perSceneSrgLayout.variables.Add({});
			perSceneSrgLayout.variables.Top().arrayCount = 1;
			perSceneSrgLayout.variables.Top().name = "_SceneData";
			perSceneSrgLayout.variables.Top().bindingSlot = sceneDataBinding;
			perSceneSrgLayout.variables.Top().type = RHI::ShaderResourceType::ConstantBuffer;
			perSceneSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Vertex | RHI::ShaderStage::Fragment;

			perSceneSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(perSceneSrgLayout);

			RHI::BufferDescriptor sceneConstantBufferDesc{};
			sceneConstantBufferDesc.name = "Scene Constants";
			sceneConstantBufferDesc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
			sceneConstantBufferDesc.bufferSize = sizeof(sceneConstants);
			sceneConstantBufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;
			
			sceneConstantBuffer = RHI::gDynamicRHI->CreateBuffer(sceneConstantBufferDesc);

			sceneConstantBuffer->UploadData(&sceneConstants, sizeof(sceneConstants));

			RHI::SamplerDescriptor shadowSamplerDesc{};
			shadowSamplerDesc.borderColor = RHI::SamplerBorderColor::FloatOpaqueWhite;
			shadowSamplerDesc.addressModeU = shadowSamplerDesc.addressModeV = shadowSamplerDesc.addressModeW = RHI::SamplerAddressMode::ClampToBorder;
			shadowSamplerDesc.enableAnisotropy = false;
			shadowSamplerDesc.maxAnisotropy = 8;
			shadowSamplerDesc.samplerFilterMode = RHI::FilterMode::Linear;
			
			shadowMapSampler = RHI::gDynamicRHI->CreateSampler(shadowSamplerDesc);

			perSceneSrg->Bind("_SceneData", sceneConstantBuffer);
			perSceneSrg->Bind("_ShadowMapSampler", shadowMapSampler);
			perSceneSrg->FlushBindings();
		}

		// Mesh SRG
		{
			sphereObjectSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(meshSrgLayout);

			RHI::BufferDescriptor desc{};
			desc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
			desc.bufferSize = sizeof(Matrix4x4);
			desc.defaultHeapType = RHI::MemoryHeapType::Upload;
			desc.name = "_ObjectData";

			for (int i = 0; i < RHI::Limits::MaxSwapChainImageCount; i++)
			{
				sphereObjectBufferPerImage[i] = RHI::gDynamicRHI->CreateBuffer(desc);

				sphereModelMatrix = Matrix4x4::Translation(Vec3(0, 0, 15)) * Quat::EulerDegrees(Vec3(0, sphereRotation, 0)).ToMatrix() * Matrix4x4::Scale(Vec3(1, 1, 1));

				RHI::BufferData uploadData{};
				uploadData.dataSize = desc.bufferSize;
				uploadData.data = &sphereModelMatrix;
				uploadData.startOffsetInBuffer = 0;

				sphereObjectBufferPerImage[i]->UploadData(uploadData);

				sphereObjectSrg->Bind(i, "_ObjectData", sphereObjectBufferPerImage[i]);
			}

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
			perViewDataDesc.shaderStages = RHI::ShaderStage::Vertex | RHI::ShaderStage::Fragment;

			perViewSrgLayout.variables.Add(perViewDataDesc);

			depthPerViewSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(perViewSrgLayout);

			perViewSrgLayout.variables.Top().shaderStages |= RHI::ShaderStage::Fragment;
			perViewSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(perViewSrgLayout);
			
			RHI::BufferDescriptor bufferDesc{};
			bufferDesc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
			bufferDesc.bufferSize = sizeof(PerViewData);
			bufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;
			bufferDesc.name = "PerViewData";

			for (int i = 0; i < RHI::Limits::MaxSwapChainImageCount; i++)
			{
				perViewBufferPerImage[i] = RHI::gDynamicRHI->CreateBuffer(bufferDesc);

				perViewSrg->Bind(i, "_PerViewData", perViewBufferPerImage[i]);
				depthPerViewSrg->Bind(i, "_PerViewData", perViewBufferPerImage[i]);
			}
			
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
			perViewSRGLayout.variables[0].shaderStages = RHI::ShaderStage::Vertex | RHI::ShaderStage::Fragment;
			srgLayouts.Add(perViewSRGLayout);
			
			RHI::ShaderResourceGroupLayout perObjectSRGLayout{};
			perObjectSRGLayout.srgType = RHI::SRGType::PerObject;
			perObjectSRGLayout.variables.Add({});
			perObjectSRGLayout.variables[0].arrayCount = 1;
			perObjectSRGLayout.variables[0].name = "_ObjectData";
			perObjectSRGLayout.variables[0].bindingSlot = perObjectDataBinding;
			perObjectSRGLayout.variables[0].type = RHI::ShaderResourceType::ConstantBuffer;
			perObjectSRGLayout.variables[0].shaderStages = RHI::ShaderStage::Vertex | RHI::ShaderStage::Fragment;
			srgLayouts.Add(perObjectSRGLayout);

			srgLayouts.Add(perSceneSrgLayout);

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
			skyboxPipelineDesc.rasterState.cullMode = RHI::CullMode::None;

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
			perObjectSRGLayout.variables[0].shaderStages = RHI::ShaderStage::Vertex | RHI::ShaderStage::Fragment;
			srgLayouts.Add(perObjectSRGLayout);

			srgLayouts.Add(perSceneSrgLayout);

			skyboxPipelineDesc.name = "Skybox Pipeline";

			skyboxShader = new RPI::Shader();
			RPI::ShaderVariantDescriptor variant{};
			variant.pipelineDesc = skyboxPipelineDesc;
			skyboxShader->AddVariant(variant);

			skyboxMaterial = new RPI::Material(skyboxShader);
			
			perSceneSrg->Bind("_Skybox", hdriCubeMap);
			perSceneSrg->Bind("_SkyboxIrradiance", irradianceMap);
			perSceneSrg->Bind("_DefaultSampler", defaultSampler);

			perSceneSrg->FlushBindings();

			RHI::BufferDescriptor desc{};
			desc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
			desc.bufferSize = sizeof(Matrix4x4);
			desc.defaultHeapType = RHI::MemoryHeapType::Upload;
			desc.name = "_ObjectData";

			skyboxObjectSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(perObjectSRGLayout);

			skyboxScale = Vec3(1000, 1000, 1000);
			skyboxModelMatrix = Matrix4x4::Translation(Vec3()) * Quat::EulerDegrees(Vec3(0, 0, 0)).ToMatrix() * Matrix4x4::Scale(skyboxScale);

			for (int i = 0; i < RHI::Limits::MaxSwapChainImageCount; i++)
			{
				skyboxObjectBufferPerImage[i] = RHI::gDynamicRHI->CreateBuffer(desc);

				skyboxObjectBufferPerImage[i]->UploadData(&skyboxModelMatrix, sizeof(skyboxModelMatrix));

				skyboxObjectSrg->Bind(i, "_ObjectData", skyboxObjectBufferPerImage[i]);
			}
			
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
			colorBlend.srcColorBlend = RHI::BlendFactor::One;
			colorBlend.dstColorBlend = RHI::BlendFactor::Zero;
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
			opaquePipelineDesc.rasterState.cullMode = RHI::CullMode::None;

			opaquePipelineDesc.vertexInputSlots.Add({});
			opaquePipelineDesc.vertexInputSlots.Top().inputRate = RHI::VertexInputRate::PerVertex;
			opaquePipelineDesc.vertexInputSlots.Top().inputSlot = 0;
			opaquePipelineDesc.vertexInputSlots.Top().stride = sizeof(Vec3); // float3 Position;

			opaquePipelineDesc.vertexInputSlots.Add({});
			opaquePipelineDesc.vertexInputSlots.Top().inputRate = RHI::VertexInputRate::PerVertex;
			opaquePipelineDesc.vertexInputSlots.Top().inputSlot = 1;
			opaquePipelineDesc.vertexInputSlots.Top().stride = sizeof(Vec3); // float3 Normal;

			opaquePipelineDesc.vertexInputSlots.Add({});
			opaquePipelineDesc.vertexInputSlots.Top().inputRate = RHI::VertexInputRate::PerVertex;
			opaquePipelineDesc.vertexInputSlots.Top().inputSlot = 2;
			opaquePipelineDesc.vertexInputSlots.Top().stride = sizeof(Vec4); // float4 Tangent;

			opaquePipelineDesc.vertexInputSlots.Add({});
			opaquePipelineDesc.vertexInputSlots.Top().inputRate = RHI::VertexInputRate::PerVertex;
			opaquePipelineDesc.vertexInputSlots.Top().inputSlot = 3;
			opaquePipelineDesc.vertexInputSlots.Top().stride = sizeof(Vec2); // float2 UV;

			Array<RHI::VertexAttributeDescriptor>& vertexAttribs = opaquePipelineDesc.vertexAttributes;

			vertexAttribs.Add({});
			vertexAttribs[0].dataType = RHI::VertexAttributeDataType::Float3; // float3 Position
			vertexAttribs[0].inputSlot = 0;
			vertexAttribs[0].location = 0;
			vertexAttribs[0].offset = 0;

			vertexAttribs.Add({});
			vertexAttribs[1].dataType = RHI::VertexAttributeDataType::Float3; // float3 Normal
			vertexAttribs[1].inputSlot = 1;
			vertexAttribs[1].location = 1;
			vertexAttribs[1].offset = 0;

			vertexAttribs.Add({});
			vertexAttribs[2].dataType = RHI::VertexAttributeDataType::Float4; // float4 Tangent
			vertexAttribs[2].inputSlot = 2;
			vertexAttribs[2].location = 2;
			vertexAttribs[2].offset = 0;

			vertexAttribs.Add({});
			vertexAttribs[3].dataType = RHI::VertexAttributeDataType::Float2; // float2 UV
			vertexAttribs[3].inputSlot = 3;
			vertexAttribs[3].location = 3;
			vertexAttribs[3].offset = 0;

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
			perObjectSRGLayout.variables[0].shaderStages = RHI::ShaderStage::Vertex | RHI::ShaderStage::Fragment;
			srgLayouts.Add(perObjectSRGLayout);

			srgLayouts.Add(perSceneSrgLayout);

			RHI::ShaderResourceGroupLayout perPassSRGLayout{};
			perPassSRGLayout.srgType = RHI::SRGType::PerPass;
			perPassSRGLayout.variables.Add({});
			perPassSRGLayout.variables.Top().arrayCount = 1;
			perPassSRGLayout.variables.Top().name = "DirectionalShadowMap";
			perPassSRGLayout.variables.Top().bindingSlot = directionalShadowMapBinding;
			perPassSRGLayout.variables.Top().type = RHI::ShaderResourceType::Texture2D;
			perPassSRGLayout.variables.Top().format = RHI::Format::R8_UNORM;
			perPassSRGLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			srgLayouts.Add(perPassSRGLayout);

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

				RHI::ShaderStructMember metallicMember{};
				metallicMember.dataType = RHI::ShaderStructMemberType::Float;
				metallicMember.name = "_Metallic";
				perMaterialSRGLayout.variables.Top().structMembers.Add(metallicMember);

				RHI::ShaderStructMember roughnessMember{};
				roughnessMember.dataType = RHI::ShaderStructMemberType::Float;
				roughnessMember.name = "_Roughness";
				perMaterialSRGLayout.variables.Top().structMembers.Add(roughnessMember);

				RHI::ShaderStructMember normalMember{};
				normalMember.dataType = RHI::ShaderStructMemberType::Float;
				normalMember.name = "_NormalStrength";
				perMaterialSRGLayout.variables.Top().structMembers.Add(normalMember);

				RHI::ShaderStructMember shininessMember{};
				shininessMember.dataType = RHI::ShaderStructMemberType::UInt;
				shininessMember.name = "_Shininess";
				perMaterialSRGLayout.variables.Top().structMembers.Add(shininessMember);

				RHI::ShaderStructMember aoMember{};
				aoMember.dataType = RHI::ShaderStructMemberType::Float;
				aoMember.name = "_AmbientOcclusion";
				perMaterialSRGLayout.variables.Top().structMembers.Add(aoMember);
			}

			perMaterialSRGLayout.variables.Add({});
			perMaterialSRGLayout.variables.Top().arrayCount = 1;
			perMaterialSRGLayout.variables.Top().name = "_AlbedoTex";
			perMaterialSRGLayout.variables.Top().bindingSlot = albedoTexBinding;
			perMaterialSRGLayout.variables.Top().type = RHI::ShaderResourceType::Texture2D;
			perMaterialSRGLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;
			
			perMaterialSRGLayout.variables.Add({});
			perMaterialSRGLayout.variables.Top().arrayCount = 1;
			perMaterialSRGLayout.variables.Top().name = "_RoughnessTex";
			perMaterialSRGLayout.variables.Top().bindingSlot = roughnessTexBinding;
			perMaterialSRGLayout.variables.Top().type = RHI::ShaderResourceType::Texture2D;
			perMaterialSRGLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			perMaterialSRGLayout.variables.Add({});
			perMaterialSRGLayout.variables.Top().arrayCount = 1;
			perMaterialSRGLayout.variables.Top().name = "_NormalTex";
			perMaterialSRGLayout.variables.Top().bindingSlot = normalTexBinding;
			perMaterialSRGLayout.variables.Top().type = RHI::ShaderResourceType::Texture2D;
			perMaterialSRGLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			perMaterialSRGLayout.variables.Add({});
			perMaterialSRGLayout.variables.Top().arrayCount = 1;
			perMaterialSRGLayout.variables.Top().name = "_MetallicTex";
			perMaterialSRGLayout.variables.Top().bindingSlot = metallicTexBinding;
			perMaterialSRGLayout.variables.Top().type = RHI::ShaderResourceType::Texture2D;
			perMaterialSRGLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

			srgLayouts.Add(perMaterialSRGLayout);

			opaquePipelineDesc.name = "Opaque Pipeline";
            
            RPI::ShaderVariantDescriptor variantDesc{};
            variantDesc.defineFlags = {};
            variantDesc.pipelineDesc = opaquePipelineDesc;
            opaqueShader->AddVariant(variantDesc);

			sphereMaterial = new RPI::Material(opaqueShader);
			
			{
				sphereMaterial->SetPropertyValue("_Albedo", Color(1, 1, 1, 1));
				sphereMaterial->SetPropertyValue("_SpecularStrength", 1.0f);
				sphereMaterial->SetPropertyValue("_Shininess", (u32)64);
				sphereMaterial->SetPropertyValue("_Metallic", 0.0f);
				sphereMaterial->SetPropertyValue("_Roughness", 1.0f);
				sphereMaterial->SetPropertyValue("_NormalStrength", 1.0f);
				sphereMaterial->SetPropertyValue("_AmbientOcclusion", 1.0f);

				sphereMaterial->SetPropertyValue("_AlbedoTex", plasticTextures.albedo);
				sphereMaterial->SetPropertyValue("_RoughnessTex", plasticTextures.roughnessMap);
				sphereMaterial->SetPropertyValue("_NormalTex", plasticTextures.normalMap);
				sphereMaterial->SetPropertyValue("_MetallicTex", plasticTextures.metallicMap);
                
                sphereMaterial->SetPropertyValue("_AlbedoTex", rpiSystem.GetDefaultAlbedoTex());
                sphereMaterial->SetPropertyValue("_RoughnessTex", rpiSystem.GetDefaultRoughnessTex());
                sphereMaterial->SetPropertyValue("_NormalTex", rpiSystem.GetDefaultNormalTex());
                sphereMaterial->SetPropertyValue("_MetallicTex", rpiSystem.GetDefaultAlbedoTex());

				//sphereMaterial->SetPropertyValue("_AlbedoTex", rustedTextures.albedo);
				//sphereMaterial->SetPropertyValue("_RoughnessTex", rustedTextures.roughnessMap);
				//sphereMaterial->SetPropertyValue("_NormalTex", rustedTextures.normalMap);
				//sphereMaterial->SetPropertyValue("_MetallicTex", rustedTextures.metallicMap);

				//sphereMaterial->SetPropertyValue("_AlbedoTex", woodFloorTextures.albedo);
				//sphereMaterial->SetPropertyValue("_RoughnessTex", woodFloorTextures.roughnessMap);
				//sphereMaterial->SetPropertyValue("_NormalTex", woodFloorTextures.normalMap);
				//sphereMaterial->SetPropertyValue("_MetallicTex", woodFloorTextures.metallicMap);
			}

            sphereMaterial->FlushProperties();

			cubeMaterial = new RPI::Material(opaqueShader);

			{
				cubeMaterial->SetPropertyValue("_Albedo", Color(1, 1, 1, 1));
				cubeMaterial->SetPropertyValue("_SpecularStrength", 1.0f);
				cubeMaterial->SetPropertyValue("_Shininess", (u32)64);
				cubeMaterial->SetPropertyValue("_Metallic", 1.0f);
				cubeMaterial->SetPropertyValue("_Roughness", 1.0f);
				cubeMaterial->SetPropertyValue("_NormalStrength", 1.0f);
				cubeMaterial->SetPropertyValue("_AmbientOcclusion", 1.0f);

				cubeMaterial->SetPropertyValue("_AlbedoTex", woodFloorTextures.albedo); //rpiSystem.GetDefaultAlbedoTex());
				cubeMaterial->SetPropertyValue("_RoughnessTex", woodFloorTextures.roughnessMap); //rpiSystem.GetDefaultRoughnessTex());
				cubeMaterial->SetPropertyValue("_NormalTex", woodFloorTextures.normalMap); //rpiSystem.GetDefaultNormalTex());
				cubeMaterial->SetPropertyValue("_MetallicTex", woodFloorTextures.metallicMap); // White metallic tex
			}

			cubeMaterial->FlushProperties();

			delete opaqueVert;
			delete opaqueFrag;
		}

		// Transparent Pipeline
		{

		}

		UpdatePerViewData(0);
		UpdatePerViewData(1);
	}

	void VulkanSandbox::InitLights()
	{
		directionalLights.Clear();

		DirectionalLight mainLight{};
		mainLight.direction = Vec3(-0.25f, -0.5f, 0.5f).GetNormalized();
		//mainLight.direction = Vec3(0, 0, 1);
		mainLight.colorAndIntensity = Vec4(1.0f, 0.95f, 0.75f);
		mainLight.colorAndIntensity.w = 5.0f; // intensity
		mainLight.temperature = 100;

		// Directional Light Shadow
		{
			RHI::ShaderResourceGroupLayout directionalLightViewSrgLayout{};
			directionalLightViewSrgLayout.srgType = RHI::SRGType::PerView;
			directionalLightViewSrgLayout.variables.Add({});
			directionalLightViewSrgLayout.variables.Top().arrayCount = 1;
			directionalLightViewSrgLayout.variables.Top().name = "_PerViewData";
			directionalLightViewSrgLayout.variables.Top().bindingSlot = perViewDataBinding;
			directionalLightViewSrgLayout.variables.Top().type = RHI::ShaderResourceType::ConstantBuffer;
			directionalLightViewSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Vertex | RHI::ShaderStage::Fragment;

			directionalLightViewSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(directionalLightViewSrgLayout);

			Matrix4x4 multiplier = Matrix4x4({
				{ 1, 0, 0, 0 },
				{ 0, -1, 0, 0 },
				{ 0, 0, 0.5f, 0.5f },
				{ 0, 0, 0, 1 },
			});

			directionalLightViewData.viewPosition = Vec3(0, 5, 0);
			directionalLightViewData.projectionMatrix = Matrix4x4::OrthographicProjection(-5, 5, 5, -5, 1.0f, 100.0f);
			directionalLightViewData.viewMatrix = Matrix4x4::Translation(-directionalLightViewData.viewPosition) *
				Quat::LookRotation(mainLight.direction).ToMatrix();

			directionalLightViewData.viewProjectionMatrix = directionalLightViewData.projectionMatrix * directionalLightViewData.viewMatrix;

			mainLight.lightSpaceMatrix = directionalLightViewData.viewProjectionMatrix;

			for (int i = 0; i < directionalLightViewPerImage.GetSize(); i++)
			{
				RHI::BufferDescriptor desc{};
				desc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
				desc.bufferSize = sizeof(directionalLightViewData);
				desc.defaultHeapType = RHI::MemoryHeapType::Upload;
				desc.name = "Directional Light View";

				directionalLightViewPerImage[i] = RHI::gDynamicRHI->CreateBuffer(desc);

				directionalLightViewPerImage[i]->UploadData(&directionalLightViewData, sizeof(directionalLightViewData));

				directionalLightViewSrg->Bind(i, "_PerViewData", directionalLightViewPerImage[i]);
			}

			directionalLightViewSrg->FlushBindings();
		}

		directionalLights.Add(mainLight);

		pointLights.Clear();

		{
			PointLight pointLight{};
			pointLight.position = Vec4(0, 0, 0);
			pointLight.colorAndIntensity = Vec3(0.5f, 0.5f, 0);
			pointLight.colorAndIntensity.w = 1.0f; // Intensity
			pointLight.radius = 25.0f;

			pointLights.Add(pointLight);
		}
		{
			PointLight pointLight{};
			pointLight.position = Vec4(15, 0, 0);
			pointLight.colorAndIntensity = Vec3(1.0f, 0.5f, 0);
			pointLight.colorAndIntensity.w = 1.0f; // Intensity
			pointLight.radius = 35.0f;

			//pointLights.Add(pointLight);
		}

		lightData.totalDirectionalLights = directionalLights.GetSize();
		lightData.totalDirectionalLights = 0;
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

		perSceneSrg->Bind("_DirectionalLightsArray", directionalLightsBuffer);
		perSceneSrg->Bind("_PointLights", pointLightsBuffer);
		perSceneSrg->Bind("_LightData", lightDataBuffer);

		perSceneSrg->FlushBindings();
	}

	void VulkanSandbox::BuildCubeMeshDrawPacket()
	{
		RHI::DrawPacketBuilder builder{};

		builder.SetDrawArguments(cubeModel->GetMesh(0)->drawArguments);

		builder.AddShaderResourceGroup(cubeObjectSrg);

		RPI::Mesh* mesh = cubeModel->GetMesh(0);

		// Depth Item
		{
			RHI::DrawPacketBuilder::DrawItemRequest request{};
			const auto& vertInfo = mesh->vertexBufferInfos[0];
			auto vertBufferView = RHI::VertexBufferView(cubeModel->GetBuffer(vertInfo.bufferIndex), vertInfo.byteOffset, vertInfo.byteCount, vertInfo.stride);
			request.vertexBufferViews.Add(vertBufferView);
			request.indexBufferView = mesh->indexBufferView;

			request.drawItemTag = rpiSystem.GetDrawListTagRegistry()->AcquireTag("depth");
			request.drawFilterMask = RHI::DrawFilterMask::ALL;
			request.pipelineState = depthPipeline;

			builder.AddDrawItem(request);
		}

		// Opaque Item
		{
			RHI::DrawPacketBuilder::DrawItemRequest request{};

			// The order here should MATCH with the graphics pipeline's vertex input order
			for (const auto& vertInfo : mesh->vertexBufferInfos)
			{
				if (vertInfo.semantic.attribute != RHI::VertexInputAttribute::Position)
					continue;

				auto vertBufferView = RHI::VertexBufferView(cubeModel->GetBuffer(vertInfo.bufferIndex), vertInfo.byteOffset, vertInfo.byteCount, vertInfo.stride);
				request.vertexBufferViews.Add(vertBufferView);
			}
			for (const auto& vertInfo : mesh->vertexBufferInfos)
			{
				if (vertInfo.semantic.attribute != RHI::VertexInputAttribute::Normal)
					continue;

				auto vertBufferView = RHI::VertexBufferView(cubeModel->GetBuffer(vertInfo.bufferIndex), vertInfo.byteOffset, vertInfo.byteCount, vertInfo.stride);
				request.vertexBufferViews.Add(vertBufferView);
			}
			for (const auto& vertInfo : mesh->vertexBufferInfos)
			{
				if (vertInfo.semantic.attribute != RHI::VertexInputAttribute::Tangent)
					continue;

				auto vertBufferView = RHI::VertexBufferView(cubeModel->GetBuffer(vertInfo.bufferIndex), vertInfo.byteOffset, vertInfo.byteCount, vertInfo.stride);
				request.vertexBufferViews.Add(vertBufferView);
			}
			for (const auto& vertInfo : mesh->vertexBufferInfos)
			{
				if (vertInfo.semantic.attribute != RHI::VertexInputAttribute::UV)
					continue;

				auto vertBufferView = RHI::VertexBufferView(cubeModel->GetBuffer(vertInfo.bufferIndex), vertInfo.byteOffset, vertInfo.byteCount, vertInfo.stride);
				request.vertexBufferViews.Add(vertBufferView);
			}
			request.indexBufferView = mesh->indexBufferView;

			request.drawItemTag = rpiSystem.GetDrawListTagRegistry()->AcquireTag("opaque");
			request.drawFilterMask = RHI::DrawFilterMask::ALL;
			request.pipelineState = sphereMaterial->GetCurrentShader()->GetPipeline();

			request.uniqueShaderResourceGroups.Add(cubeMaterial->GetShaderResourceGroup());

			builder.AddDrawItem(request);
		}

		cubeDrawPacket = builder.Build();
	}

	void VulkanSandbox::BuildSphereMeshDrawPacket()
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

			request.drawItemTag = rpiSystem.GetDrawListTagRegistry()->AcquireTag("depth");
			request.drawFilterMask = RHI::DrawFilterMask::ALL;
			request.pipelineState = depthPipeline;
			
			builder.AddDrawItem(request);
		}

		// Opaque Item
		{
			RHI::DrawPacketBuilder::DrawItemRequest request{};
			for (const auto& vertInfo : mesh->vertexBufferInfos)
			{
				if (vertInfo.semantic.attribute != RHI::VertexInputAttribute::Position)
					continue;

				auto vertBufferView = RHI::VertexBufferView(sphereModel->GetBuffer(vertInfo.bufferIndex), vertInfo.byteOffset, vertInfo.byteCount, vertInfo.stride);
				request.vertexBufferViews.Add(vertBufferView);
			}
			for (const auto& vertInfo : mesh->vertexBufferInfos)
			{
				if (vertInfo.semantic.attribute != RHI::VertexInputAttribute::Normal)
					continue;

				auto vertBufferView = RHI::VertexBufferView(sphereModel->GetBuffer(vertInfo.bufferIndex), vertInfo.byteOffset, vertInfo.byteCount, vertInfo.stride);
				request.vertexBufferViews.Add(vertBufferView);
			}
			for (const auto& vertInfo : mesh->vertexBufferInfos)
			{
				if (vertInfo.semantic.attribute != RHI::VertexInputAttribute::Tangent)
					continue;

				auto vertBufferView = RHI::VertexBufferView(sphereModel->GetBuffer(vertInfo.bufferIndex), vertInfo.byteOffset, vertInfo.byteCount, vertInfo.stride);
				request.vertexBufferViews.Add(vertBufferView);
			}
			for (const auto& vertInfo : mesh->vertexBufferInfos)
			{
				if (vertInfo.semantic.attribute != RHI::VertexInputAttribute::UV)
					continue;

				auto vertBufferView = RHI::VertexBufferView(sphereModel->GetBuffer(vertInfo.bufferIndex), vertInfo.byteOffset, vertInfo.byteCount, vertInfo.stride);
				request.vertexBufferViews.Add(vertBufferView);
			}
			request.indexBufferView = mesh->indexBufferView;

			request.drawItemTag = rpiSystem.GetDrawListTagRegistry()->AcquireTag("opaque");
			request.drawFilterMask = RHI::DrawFilterMask::ALL;
			request.pipelineState = sphereMaterial->GetCurrentShader()->GetPipeline();

			request.uniqueShaderResourceGroups.Add(sphereMaterial->GetShaderResourceGroup());

			builder.AddDrawItem(request);
		}

		// Transparent Item
		{

		}

		sphereDrawPacket = builder.Build();
	}

	void VulkanSandbox::BuildSkyboxDrawPacket()
	{
		RHI::DrawPacketBuilder builder{};

		//auto model = cubeModel;
		auto model = sphereModel;

		builder.SetDrawArguments(model->GetMesh(0)->drawArguments);

		builder.AddShaderResourceGroup(perSceneSrg);
		builder.AddShaderResourceGroup(skyboxObjectSrg);

		RPI::Mesh* mesh = model->GetMesh(0);

		// Skybox Item
		{
			RHI::DrawPacketBuilder::DrawItemRequest request{};
			for (const auto& vertInfo : mesh->vertexBufferInfos)
			{
				if (vertInfo.semantic.attribute != RHI::VertexInputAttribute::Position)
					continue;

				auto vertBufferView = RHI::VertexBufferView(model->GetBuffer(vertInfo.bufferIndex), vertInfo.byteOffset, vertInfo.byteCount, vertInfo.stride);
				request.vertexBufferViews.Add(vertBufferView);
			}
			request.indexBufferView = mesh->indexBufferView;

			request.drawItemTag = rpiSystem.GetDrawListTagRegistry()->AcquireTag("skybox");
			request.drawFilterMask = RHI::DrawFilterMask::ALL;
			request.pipelineState = skyboxMaterial->GetCurrentShader()->GetPipeline();

			builder.AddDrawItem(request);
		}

		skyboxDrawPacket = builder.Build();
	}

	void VulkanSandbox::InitDrawPackets()
	{
		BuildCubeMeshDrawPacket();
		BuildSphereMeshDrawPacket();
		BuildSkyboxDrawPacket();
	}

	void VulkanSandbox::DestroyDrawPackets()
	{
		scheduler->WaitUntilIdle();
		
		delete cubeDrawPacket; cubeDrawPacket = nullptr;
		delete sphereDrawPacket; sphereDrawPacket = nullptr;
		delete skyboxDrawPacket; skyboxDrawPacket = nullptr;
	}

	void VulkanSandbox::DestroyLights()
	{
		scheduler->WaitUntilIdle();

		for (int i = 0; i < directionalLightViewPerImage.GetSize(); i++)
		{
			delete directionalLightViewPerImage[i];
			directionalLightViewPerImage[i] = nullptr;
		}

		delete directionalLightViewSrg; directionalLightViewSrg = nullptr;

		delete pointLightsBuffer; pointLightsBuffer = nullptr;
		delete directionalLightsBuffer; directionalLightsBuffer = nullptr;
		delete lightDataBuffer; lightDataBuffer = nullptr;
	}

	void VulkanSandbox::DestroyPipelines()
	{
		delete shadowMapSampler; shadowMapSampler = nullptr;
		delete sceneConstantBuffer; sceneConstantBuffer = nullptr;
		delete cubeModel; cubeModel = nullptr;
		delete sphereModel; sphereModel = nullptr;

		for (int i = 0; i < RHI::Limits::MaxSwapChainImageCount; i++)
		{
			delete cubeObjectBufferPerImage[i]; cubeObjectBufferPerImage[i] = nullptr;
			delete sphereObjectBufferPerImage[i]; sphereObjectBufferPerImage[i] = nullptr;
			delete skyboxObjectBufferPerImage[i]; skyboxObjectBufferPerImage[i] = nullptr;
			delete perViewBufferPerImage[i]; perViewBufferPerImage[i] = nullptr;
		}
		delete cubeObjectSrg; cubeObjectSrg = nullptr;
		delete sphereObjectSrg; sphereObjectSrg = nullptr;

		delete depthPerViewSrg; depthPerViewSrg = nullptr;
		delete perViewSrg; perViewSrg = nullptr;

		delete depthPipeline; depthPipeline = nullptr;
		delete depthShaderVert; depthShaderVert = nullptr;
		
		delete skyboxShader; skyboxShader = nullptr;
		delete skyboxMaterial; skyboxMaterial = nullptr;
		delete skyboxObjectSrg; skyboxObjectSrg = nullptr;
		delete perSceneSrg; perSceneSrg = nullptr;

        delete opaqueShader; opaqueShader = nullptr;
		delete sphereMaterial; sphereMaterial = nullptr;
		delete cubeMaterial; cubeMaterial = nullptr;

		delete transparentPipeline; transparentPipeline = nullptr;
	}

	void VulkanSandbox::DestroyTextures()
	{
		woodFloorTextures.Release();
		woodFloorTextures = {};

		plasticTextures.Release();
		plasticTextures = {};

		rustedTextures.Release();
		rustedTextures = {};
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
			depthDesc.depth = 1;
			depthDesc.bindFlags = RHI::TextureBindFlags::Depth;
			depthDesc.format = RHI::gDynamicRHI->GetAvailableDepthOnlyFormats()[0];
			depthDesc.name = "DepthStencil";

			RHI::ImageDescriptor shadowMapDesc{};
			shadowMapDesc.width = 1024;
			shadowMapDesc.height = 1024;
			shadowMapDesc.depth = 1;
			shadowMapDesc.dimension = RHI::Dimension::Dim2D;
			shadowMapDesc.bindFlags = RHI::TextureBindFlags::Depth | RHI::TextureBindFlags::ShaderRead;
			shadowMapDesc.format = RHI::gDynamicRHI->GetAvailableDepthOnlyFormats()[0];
			shadowMapDesc.name = "DirectionalShadowMap";

			attachmentDatabase.EmplaceFrameAttachment("DepthStencil", depthDesc);
			attachmentDatabase.EmplaceFrameAttachment("SwapChain", swapChain);
			attachmentDatabase.EmplaceFrameAttachment("DirectionalShadowMap", shadowMapDesc);

			scheduler->SetVariableInitialValue("DrawSunShadows", true);
			
			scheduler->BeginScope("DirectionalShadowCast");
			{
				//scheduler->ExecuteOnlyIf("DrawSunShadows", RHI::FrameGraphVariableComparison::Equal, true);

				RHI::ImageScopeAttachmentDescriptor shadowMapAttachment{};
				shadowMapAttachment.attachmentId = "DirectionalShadowMap";
				shadowMapAttachment.loadStoreAction.clearValueDepth = 1.0f;
				shadowMapAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Clear;
				shadowMapAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;
				scheduler->UseAttachment(shadowMapAttachment, RHI::ScopeAttachmentUsage::DepthStencil, RHI::ScopeAttachmentAccess::Write);

				scheduler->UseShaderResourceGroup(perSceneSrg);
				scheduler->UseShaderResourceGroup(directionalLightViewSrg);

				scheduler->UsePipeline(depthPipeline);

				//scheduler->SetVariableAfterExecution("DrawSunShadows", false);
			}
			scheduler->EndScope();

			//scheduler->BeginScopeGroup("MainPass");
			scheduler->BeginScope("Skybox");
			{
				RHI::ImageScopeAttachmentDescriptor swapChainAttachment{};
				swapChainAttachment.attachmentId = "SwapChain";
                swapChainAttachment.loadStoreAction.clearValue = Vec4(0, 0.5f, 0.5f, 1);
                swapChainAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Clear;
				swapChainAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;
				scheduler->UseAttachment(swapChainAttachment, RHI::ScopeAttachmentUsage::Color, RHI::ScopeAttachmentAccess::Write);

				scheduler->UseShaderResourceGroup(perSceneSrg);
				scheduler->UseShaderResourceGroup(perViewSrg);
				
				scheduler->UsePipeline(skyboxMaterial->GetCurrentShader()->GetPipeline());
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
				swapChainAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;

				scheduler->UseAttachment(swapChainAttachment, RHI::ScopeAttachmentUsage::Color, RHI::ScopeAttachmentAccess::Write);

				RHI::ImageScopeAttachmentDescriptor shadowMapAttachment{};
				shadowMapAttachment.attachmentId = "DirectionalShadowMap";
				shadowMapAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Load;
				shadowMapAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;

				scheduler->UseAttachment(shadowMapAttachment, RHI::ScopeAttachmentUsage::Shader, RHI::ScopeAttachmentAccess::Read);

				scheduler->UseShaderResourceGroup(perSceneSrg);
				scheduler->UseShaderResourceGroup(perViewSrg);

				for (int i = 0; i < opaqueShader->GetVariantCount(); i++)
				{
					scheduler->UsePipeline(opaqueShader->GetVariant(i)->GetPipeline());
				}

				scheduler->PresentSwapChain(swapChain);
			}
			scheduler->EndScope();
			//scheduler->EndScopeGroup();
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
		resubmit = false;
		drawList.Shutdown();
		RHI::DrawListMask drawListMask{};
		auto skyboxTag = rpiSystem.GetDrawListTagRegistry()->AcquireTag("skybox");
		auto depthTag = rpiSystem.GetDrawListTagRegistry()->AcquireTag("depth");
		auto opaqueTag = rpiSystem.GetDrawListTagRegistry()->AcquireTag("opaque");
		//auto transparentTag = rpiSystem.GetDrawListTagRegistry()->AcquireTag("transparent");
		drawListMask.Set(skyboxTag);
		drawListMask.Set(depthTag);
		drawListMask.Set(opaqueTag);
		//drawListMask.Set(transparentTag);
		drawList.Init(drawListMask);
		
		// Add items
		drawList.AddDrawPacket(sphereDrawPacket);
		drawList.AddDrawPacket(cubeDrawPacket);
		drawList.AddDrawPacket(skyboxDrawPacket);

		// Finalize
		drawList.Finalize();
		scheduler->SetScopeDrawList("Skybox", &drawList.GetDrawListForTag(skyboxTag));
		scheduler->SetScopeDrawList("DirectionalShadowCast", &drawList.GetDrawListForTag(depthTag));
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

	template<class Func>
	void ParallelFor(int numThreads, int start, int end, Func&& func) {
		std::vector<std::thread> threads;
		int chunkSize = (end - start) / numThreads;

		for (int i = 0; i < numThreads; ++i) {
			int chunkStart = start + i * chunkSize;
			int chunkEnd = (i == numThreads - 1) ? end : chunkStart + chunkSize;
			threads.emplace_back([chunkStart, chunkEnd, &func]() {
				for (int j = chunkStart; j < chunkEnd; ++j) {
					func(j);
				}
				});
		}

		for (auto& t : threads) {
			t.join();
		}
	}

	MaterialTextureGroup MaterialTextureGroup::Load(const Name& pathName)
	{
		MaterialTextureGroup result{};

		IO::Path basePath = PlatformDirectories::GetLaunchDir() / "Engine/Assets/Textures/" / pathName.GetString();
		if (!basePath.Exists())
			return {};

		IO::Path albedoPath = basePath / "albedo.png";
		IO::Path normalPath = basePath / "normal.png";
		IO::Path roughnessPath = basePath / "roughness.png";
		IO::Path metallicPath = basePath / "metallic.png";

		if (!albedoPath.Exists() || !normalPath.Exists() || !roughnessPath.Exists() || !metallicPath.Exists())
			return {};

		auto prevTime = clock();

		CMImage imageArray[4] = { CMImage(), CMImage(), CMImage(), CMImage() };

		CMImage& albedoImage = imageArray[0];
		CMImage& normalImage = imageArray[1];
		CMImage& roughnessImage = imageArray[2];
		CMImage& metallicImage = imageArray[3];

		{
			auto t1 = Thread([&]()
				{
					albedoImage = CMImage::LoadFromFile(albedoPath);
				});
			auto t2 = Thread([&]()
				{
					normalImage = CMImage::LoadFromFile(normalPath);
				});
			auto t3 = Thread([&]()
				{
					roughnessImage = CMImage::LoadFromFile(roughnessPath);
				});
			auto t4 = Thread([&]()
				{
					metallicImage = CMImage::LoadFromFile(metallicPath);
				});

			t1.Join();
			t2.Join();
			t3.Join();
			t4.Join();
		}

		RHI::CommandQueue* queue = RHI::gDynamicRHI->GetPrimaryGraphicsQueue();
		auto commandList = RHI::gDynamicRHI->AllocateCommandList(queue);
		auto uploadFence = RHI::gDynamicRHI->CreateFence(false);

		RHI::TextureDescriptor textureDesc{};
		textureDesc.name = pathName.GetString() + " Albedo";
		textureDesc.bindFlags = RHI::TextureBindFlags::ShaderRead;
		textureDesc.defaultHeapType = RHI::MemoryHeapType::Default;
		textureDesc.width = albedoImage.GetWidth();
		textureDesc.height = albedoImage.GetHeight();
		textureDesc.mipLevels = 1;
		textureDesc.depth = 1;
		textureDesc.dimension = RHI::Dimension::Dim2D;
		textureDesc.sampleCount = 1;
		textureDesc.format = RHI::Format::R8G8B8A8_UNORM;

		result.albedo = RHI::gDynamicRHI->CreateTexture(textureDesc);

		textureDesc.name = pathName.GetString() + " Normal";
		textureDesc.width = normalImage.GetWidth();
		textureDesc.height = normalImage.GetHeight();
		
		result.normalMap = RHI::gDynamicRHI->CreateTexture(textureDesc);

		textureDesc.name = pathName.GetString() + " Roughness";
		textureDesc.width = roughnessImage.GetWidth();
		textureDesc.height = roughnessImage.GetHeight();
		textureDesc.format = RHI::Format::R8_UNORM;

		result.roughnessMap = RHI::gDynamicRHI->CreateTexture(textureDesc);

		textureDesc.name = pathName.GetString() + " Metallic";
		textureDesc.width = metallicImage.GetWidth();
		textureDesc.height = metallicImage.GetHeight();
		textureDesc.format = RHI::Format::R8_UNORM;

		result.metallicMap = RHI::gDynamicRHI->CreateTexture(textureDesc);

		const u32 albedoNumPixels = albedoImage.GetWidth() * albedoImage.GetHeight();
		const u32 normalNumPixels = normalImage.GetWidth() * normalImage.GetHeight();
		const u32 roughnessNumPixels = roughnessImage.GetWidth() * roughnessImage.GetHeight();
		const u32 metallicNumPixels = metallicImage.GetWidth() * metallicImage.GetHeight();

		const u64 albedoByteSize = albedoImage.GetWidth() * albedoImage.GetHeight() * 4; // RGBA32
		const u64 normalByteSize = normalImage.GetWidth() * normalImage.GetHeight() * 4; // RGBA32
		const u64 roughnessByteSize = roughnessImage.GetWidth() * roughnessImage.GetHeight() * 1; // R8
		const u64 metallicByteSize = metallicImage.GetWidth() * metallicImage.GetHeight() * 1; // R8
		
		RHI::BufferDescriptor bufferDesc{};
		bufferDesc.name = "Texture Copy Buffer";
		bufferDesc.bufferSize = albedoByteSize + normalByteSize + roughnessByteSize + metallicByteSize;
		bufferDesc.bindFlags = RHI::BufferBindFlags::StagingBuffer;
		bufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;
		bufferDesc.structureByteStride = result.albedo->GetByteSize();

		RHI::Buffer* stagingBuffer = RHI::gDynamicRHI->CreateBuffer(bufferDesc);

		void* dataPtr = nullptr;
		stagingBuffer->Map(0, stagingBuffer->GetBufferSize(), &dataPtr);
		{
			u8* albedoPtr = ((u8*)dataPtr);
			u8* normalPtr = ((u8*)dataPtr + albedoByteSize);
			u8* roughnessPtr = ((u8*)dataPtr + albedoByteSize + normalByteSize);
			u8* metallicPtr = ((u8*)dataPtr + albedoByteSize + normalByteSize + roughnessByteSize);

			u32 albedoBytesPerPixel = albedoImage.GetBitsPerPixel() / 8;
			u32 normalBytesPerPixel = normalImage.GetBitsPerPixel() / 8;
			u32 roughnessBytesPerPixel = roughnessImage.GetBitsPerPixel() / 8;
			u32 metallicBytesPerPixel = metallicImage.GetBitsPerPixel() / 8;
			albedoBytesPerPixel = 4;
			normalBytesPerPixel = 4;

			int numThreads = Thread::GetHardwareConcurrency();

			auto t1 = Thread([&]()
				{
					for (int i = 0; i < albedoNumPixels; i++)
					{
						*(albedoPtr + i * 4 + 0) = *((u8*)albedoImage.GetDataPtr() + i * albedoBytesPerPixel + 0);
						*(albedoPtr + i * 4 + 1) = *((u8*)albedoImage.GetDataPtr() + i * albedoBytesPerPixel + 1);
						*(albedoPtr + i * 4 + 2) = *((u8*)albedoImage.GetDataPtr() + i * albedoBytesPerPixel + 2);
						*(albedoPtr + i * 4 + 3) = (u8)0xff;
					}
				});

			auto t2 = Thread([&]()
				{
					for (int i = 0; i < normalNumPixels; i++)
					{
						*(normalPtr + i * 4 + 0) = *((u8*)normalImage.GetDataPtr() + i * normalBytesPerPixel + 0);
						*(normalPtr + i * 4 + 1) = *((u8*)normalImage.GetDataPtr() + i * normalBytesPerPixel + 1);
						*(normalPtr + i * 4 + 2) = *((u8*)normalImage.GetDataPtr() + i * normalBytesPerPixel + 2);
						*(normalPtr + i * 4 + 3) = (u8)0xff;
					}
				});

			auto t3 = Thread([&]()
				{
					for (int i = 0; i < roughnessByteSize; i++)
					{
						*(roughnessPtr + i) = *((u8*)roughnessImage.GetDataPtr() + i * 4);
					}
				});

			auto t4 = Thread([&]()
				{
					for (int i = 0; i < metallicByteSize; i++)
					{
						*(metallicPtr + i) = *((u8*)metallicImage.GetDataPtr() + i * 4);
					}
				});

			t1.Join();
			t2.Join();
			t3.Join();
			t4.Join();
		}
		stagingBuffer->Unmap();

		commandList->Begin();
		{
			RHI::ResourceBarrierDescriptor barrier{};
			barrier.resource = result.albedo;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::CopyDestination;
			commandList->ResourceBarrier(1, &barrier);

			barrier.resource = result.normalMap;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::CopyDestination;
			commandList->ResourceBarrier(1, &barrier);

			barrier.resource = result.roughnessMap;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::CopyDestination;
			commandList->ResourceBarrier(1, &barrier);

			barrier.resource = result.metallicMap;
			barrier.fromState = RHI::ResourceState::Undefined;
			barrier.toState = RHI::ResourceState::CopyDestination;
			commandList->ResourceBarrier(1, &barrier);

			RHI::BufferToTextureCopy bufferCopy{};
			bufferCopy.srcBuffer = stagingBuffer;
			bufferCopy.bufferOffset = 0;
			bufferCopy.dstTexture = result.albedo;
			bufferCopy.baseArrayLayer = 0;
			bufferCopy.layerCount = 1;
			bufferCopy.mipSlice = 0;

			commandList->CopyTextureRegion(bufferCopy);

			bufferCopy.srcBuffer = stagingBuffer;
			bufferCopy.bufferOffset = albedoByteSize;
			bufferCopy.dstTexture = result.normalMap;
			bufferCopy.baseArrayLayer = 0;
			bufferCopy.layerCount = 1;
			bufferCopy.mipSlice = 0;

			commandList->CopyTextureRegion(bufferCopy);

			bufferCopy.srcBuffer = stagingBuffer;
			bufferCopy.bufferOffset = albedoByteSize + normalByteSize;
			bufferCopy.dstTexture = result.roughnessMap;
			bufferCopy.baseArrayLayer = 0;
			bufferCopy.layerCount = 1;
			bufferCopy.mipSlice = 0;

			commandList->CopyTextureRegion(bufferCopy);

			bufferCopy.srcBuffer = stagingBuffer;
			bufferCopy.bufferOffset = albedoByteSize + normalByteSize + roughnessByteSize;
			bufferCopy.dstTexture = result.metallicMap;
			bufferCopy.baseArrayLayer = 0;
			bufferCopy.layerCount = 1;
			bufferCopy.mipSlice = 0;

			commandList->CopyTextureRegion(bufferCopy);

			barrier.resource = result.albedo;
			barrier.fromState = RHI::ResourceState::CopyDestination;
			barrier.toState = RHI::ResourceState::FragmentShaderResource;
			commandList->ResourceBarrier(1, &barrier);

			barrier.resource = result.normalMap;
			barrier.fromState = RHI::ResourceState::CopyDestination;
			barrier.toState = RHI::ResourceState::FragmentShaderResource;
			commandList->ResourceBarrier(1, &barrier);

			barrier.resource = result.roughnessMap;
			barrier.fromState = RHI::ResourceState::CopyDestination;
			barrier.toState = RHI::ResourceState::FragmentShaderResource;
			commandList->ResourceBarrier(1, &barrier);

			barrier.resource = result.metallicMap;
			barrier.fromState = RHI::ResourceState::CopyDestination;
			barrier.toState = RHI::ResourceState::FragmentShaderResource;
			commandList->ResourceBarrier(1, &barrier);
		}
		commandList->End();

		queue->Execute(1, &commandList, uploadFence);

		uploadFence->WaitForFence();

		// Cleanup
		RHI::gDynamicRHI->FreeCommandLists(1, &commandList);
		delete uploadFence;
		delete stagingBuffer;

		if (albedoImage.IsValid())
			albedoImage.Free();
		if (normalImage.IsValid())
			normalImage.Free();
		if (roughnessImage.IsValid())
			roughnessImage.Free();
		if (metallicImage.IsValid())
			metallicImage.Free();

		auto timeTaken = ((f32)(clock() - prevTime)) / CLOCKS_PER_SEC;

		return result;
	}

	void MaterialTextureGroup::Release()
	{
		delete albedo; albedo = nullptr;
		delete normalMap; normalMap = nullptr;
		delete roughnessMap; roughnessMap = nullptr;
		delete metallicMap; metallicMap = nullptr;

		delete memoryAllocation; memoryAllocation = nullptr;
	}

} // namespace CE
