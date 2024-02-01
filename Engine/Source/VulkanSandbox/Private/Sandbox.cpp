#include "VulkanSandbox.h"

namespace CE::Sandbox
{
	constexpr u32 perViewDataBinding = 2;
	constexpr u32 perObjectDataBinding = 4;
	constexpr u32 directionalLightArrayBinding = 0;
	constexpr u32 lightDataBinding = 1;
	constexpr u32 materialDataBinding = 5;

	static int counter = 0;
	static RHI::RHISystem rhiSystem{};

	void VulkanSandbox::Init(PlatformWindow* window)
	{
		localCounter = counter++;

		RHI::FrameSchedulerDescriptor desc{};
		
		scheduler = new FrameScheduler(desc);

		mainWindow = window;

		RHI::SwapChainDescriptor swapChainDesc{};
		swapChainDesc.imageCount = 2;
		swapChainDesc.preferredFormats = { RHI::Format::R8G8B8A8_UNORM, RHI::Format::B8G8R8A8_UNORM };

		swapChain = RHI::gDynamicRHI->CreateSwapChain(mainWindow, swapChainDesc);

		mainWindow->GetDrawableWindowSize(&width, &height);

		mainWindow->AddListener(this);
		
		InitPipelines();
		InitLights();

		BuildFrameGraph();
		CompileFrameGraph();

		InitModels();
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

		UpdateViewSrg();

		meshRotation += deltaTime * 15.0f;
		if (meshRotation >= 360)
			meshRotation -= 360;

		meshModelMatrix = Matrix4x4::Translation(Vec3(0, 0, 15)) * Quat::EulerDegrees(Vec3(0, meshRotation, 0)).ToMatrix() * Matrix4x4::Scale(Vec3(1, 1, 1));

		RHI::BufferData uploadData{};
		uploadData.dataSize = sizeof(meshModelMatrix);
		uploadData.data = &meshModelMatrix;
		uploadData.startOffsetInBuffer = 0;

		meshModelBuffer->UploadData(uploadData);

		scheduler->Execute();
	}

	void VulkanSandbox::UpdateViewSrg()
	{
		perViewData.projectionMatrix = Matrix4x4::PerspectiveProjection(swapChain->GetAspectRatio(), 60, 0.1f, 100.0f);
		perViewData.viewMatrix = Matrix4x4::Translation(Vec3(0, 0, -10));
		perViewData.viewProjectionMatrix = perViewData.projectionMatrix * perViewData.viewMatrix;

		RHI::BufferData data{};
		data.startOffsetInBuffer = 0;
		data.dataSize = perViewBuffer->GetBufferSize();
		data.data = &perViewData;

		perViewBuffer->UploadData(data);
	}

	void VulkanSandbox::Shutdown()
	{
		DestroyLights();
		DestroyModels();

		if (mainWindow)
		{
			mainWindow->RemoveListener(this);
		}

		delete scheduler;
		DestroyPipelines();

		delete swapChain;
	}

	void VulkanSandbox::InitPipelines()
	{
		// Per View SRG
		{
			RHI::ShaderResourceGroupLayout perViewSrgLayout{};
			perViewSrgLayout.srgType = RHI::SRGType::PerView;

			SRGVariableDescriptor perViewDataDesc{};
			perViewDataDesc.bindingSlot = perViewDataBinding;
			perViewDataDesc.arrayCount = 1;
			perViewDataDesc.name = "_PerViewData";
			perViewDataDesc.type = RHI::ShaderResourceType::ConstantBuffer;
			perViewDataDesc.shaderStages = RHI::ShaderStage::Vertex;

			perViewSrgLayout.variables.Add(perViewDataDesc);

			perViewSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(perViewSrgLayout);

			RHI::BufferDescriptor bufferDesc{};
			bufferDesc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
			bufferDesc.bufferSize = sizeof(PerViewData);
			bufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;
			bufferDesc.name = "PerViewData";

			perViewBuffer = RHI::gDynamicRHI->CreateBuffer(bufferDesc);

			UpdateViewSrg();

			perViewSrg->Bind("_PerViewData", RHI::BufferView(perViewBuffer));
			
			perViewSrg->Compile();
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
			depthPipelineDesc.vertexInputSlots[0].stride = Mesh::VertexBufferStride;

			Array<RHI::VertexAttributeDescriptor>& vertexAttribs = depthPipelineDesc.vertexAttributes;
			
			vertexAttribs.Add({});
			vertexAttribs[0].dataType = RHI::VertexAttributeDataType::Float3;
			vertexAttribs[0].inputSlot = 0;
			vertexAttribs[0].location = 0;
			vertexAttribs[0].offset = offsetof(VertexStruct, position);

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
			opaquePipelineDesc.rasterState.cullMode = RHI::CullMode::None;

			opaquePipelineDesc.vertexInputSlots.Add({});
			opaquePipelineDesc.vertexInputSlots[0].inputRate = RHI::VertexInputRate::PerVertex;
			opaquePipelineDesc.vertexInputSlots[0].inputSlot = 0;
			opaquePipelineDesc.vertexInputSlots[0].stride = Mesh::VertexBufferStride;

			Array<RHI::VertexAttributeDescriptor>& vertexAttribs = opaquePipelineDesc.vertexAttributes;

			vertexAttribs.Add({});
			vertexAttribs[0].dataType = RHI::VertexAttributeDataType::Float3; // Position
			vertexAttribs[0].inputSlot = 0;
			vertexAttribs[0].location = 0;
			vertexAttribs[0].offset = 0;

			vertexAttribs.Add({});
			vertexAttribs[1].dataType = RHI::VertexAttributeDataType::Float3; // Normal
			vertexAttribs[1].inputSlot = 0;
			vertexAttribs[1].location = 1;
			vertexAttribs[1].offset = sizeof(f32) * 4;

			Array<RHI::ShaderResourceGroupLayout>& srgLayouts = opaquePipelineDesc.srgLayouts;
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
				albedoMember.name = "albedo";
				perMaterialSRGLayout.variables.Top().structMembers.Add(albedoMember);
			}

			srgLayouts.Add(perMaterialSRGLayout);

			opaquePipelineDesc.name = "Opaque Pipeline";

			//opaquePipeline = RHI::gDynamicRHI->CreateGraphicsPipeline(opaquePipelineDesc);
            
            RPI::ShaderVariantDescriptor variantDesc{};
            variantDesc.defineFlags = {};
            variantDesc.pipelineDesc = opaquePipelineDesc;
            opaqueShader->AddVariant(variantDesc);

			opaqueMaterial = new RPI::Material(opaqueShader);

			delete opaqueVert;
			delete opaqueFrag;
		}

		// Transparent Pipeline
		{

		}
	}

	void VulkanSandbox::InitLights()
	{
		DirectionalLight mainLight{};
		mainLight.color = Vec3(1.0f, 0.95f, 0.7f);
		mainLight.direction = Vec3(-0.5f, -0.25f, 1);
		mainLight.intensity = 1.0f;
		mainLight.temperature = 100;

		directionalLights.Clear();
		directionalLights.Add(mainLight);
		lightData.totalDirectionalLights = directionalLights.GetSize();

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
		perSceneSRGLayout.variables.Top().name = "_LightData";
		perSceneSRGLayout.variables.Top().bindingSlot = lightDataBinding;
		perSceneSRGLayout.variables.Top().type = RHI::ShaderResourceType::ConstantBuffer;
		perSceneSRGLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

		perSceneSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(perSceneSRGLayout);

		perSceneSrg->Bind("_DirectionalLightsArray", directionalLightsBuffer);
		perSceneSrg->Bind("_LightData", lightDataBuffer);

		perSceneSrg->Compile();
	}

	void VulkanSandbox::InitModels()
	{
		Mesh* cubeMesh = new Mesh();

		cubeMesh->vertices = {
			Vec3(0.5, -0.5, 0.5),
			Vec3(-0.5, -0.5, 0.5),
			Vec3(0.5, 0.5, 0.5),
			Vec3(-0.5, 0.5, 0.5),

			Vec3(0.5, 0.5, -0.5),
			Vec3(-0.5, 0.5, -0.5),
			Vec3(0.5, -0.5, -0.5),
			Vec3(-0.5, -0.5, -0.5),

			Vec3(0.5, 0.5, 0.5),
			Vec3(-0.5, 0.5, 0.5),
			Vec3(0.5, 0.5, -0.5),
			Vec3(-0.5, 0.5, -0.5),

			Vec3(0.5, -0.5, -0.5),
			Vec3(0.5, -0.5, 0.5),
			Vec3(-0.5, -0.5, 0.5),
			Vec3(-0.5, -0.5, -0.5),

			Vec3(-0.5, -0.5, 0.5),
			Vec3(-0.5, 0.5, 0.5),
			Vec3(-0.5, 0.5, -0.5),
			Vec3(-0.5, -0.5, -0.5),

			Vec3(0.5, -0.5, -0.5),
			Vec3(0.5, 0.5, -0.5),
			Vec3(0.5, 0.5, 0.5),
			Vec3(0.5, -0.5, 0.5)
		};

		cubeMesh->indices = {
			0, 2, 3,
			0, 3, 1,
			8, 4, 5,
			8, 5, 9,
			10, 6, 7,
			10, 7, 11,
			12, 13, 14,
			12, 14, 15,
			16, 17, 18,
			16, 18, 19,
			20, 21, 22,
			20, 22, 23
		};

		cubeMesh->uvCoords = {
			Vec2(0, 0),
			Vec2(1, 0),
			Vec2(0, 1),
			Vec2(1, 1),

			Vec2(0, 1),
			Vec2(1, 1),
			Vec2(0, 1),
			Vec2(1, 1),

			Vec2(0, 0),
			Vec2(1, 0),
			Vec2(0, 0),
			Vec2(1, 0),

			Vec2(0, 0),
			Vec2(0, 1),
			Vec2(1, 1),
			Vec2(1, 0),

			Vec2(0, 0),
			Vec2(0, 1),
			Vec2(1, 1),
			Vec2(1, 0),

			Vec2(0, 0),
			Vec2(0, 1),
			Vec2(1, 1),
			Vec2(1, 0)
		};

		cubeMesh->normals = {
			Vec3(0, 0, 1),
			Vec3(0, 0, 1),
			Vec3(0, 0, 1),
			Vec3(0, 0, 1),

			Vec3(0, 1, 0),
			Vec3(0, 1, 0),
			Vec3(0, 0, -1),
			Vec3(0, 0, -1),

			Vec3(0, 1, 0),
			Vec3(0, 1, 0),
			Vec3(0, 0, -1),
			Vec3(0, 0, -1),

			Vec3(0, -1, 0),
			Vec3(0, -1, 0),
			Vec3(0, -1, 0),
			Vec3(0, -1, 0),

			Vec3(-1, 0, 0),
			Vec3(-1, 0, 0),
			Vec3(-1, 0, 0),
			Vec3(-1, 0, 0),

			Vec3(1, 0, 0),
			Vec3(1, 0, 0),
			Vec3(1, 0, 0),
			Vec3(1, 0, 0)
		};

		cubeMesh->tangents = {
			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),

			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),

			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),

			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),

			Vec4(0, 0, -1, -1),
			Vec4(0, 0, -1, -1),
			Vec4(0, 0, -1, -1),
			Vec4(0, 0, -1, -1),

			Vec4(0, 0, 1, -1),
			Vec4(0, 0, 1, -1),
			Vec4(0, 0, 1, -1),
			Vec4(0, 0, 1, -1),
		};

		cubeMesh->CreateBuffer();
		meshes.Add(cubeMesh);

		DrawPacketBuilder builder{};

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

			meshObjectSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(meshSrgLayout);

			RHI::BufferDescriptor desc{};
			desc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
			desc.bufferSize = sizeof(Matrix4x4);
			desc.defaultHeapType = RHI::MemoryHeapType::Upload;
			desc.name = "_ObjectData";

			meshModelBuffer = RHI::gDynamicRHI->CreateBuffer(desc);

			meshModelMatrix = Matrix4x4::Translation(Vec3(0, 0, 15)) * Quat::EulerDegrees(Vec3(0, meshRotation, 0)).ToMatrix() * Matrix4x4::Scale(Vec3(1, 1, 1));

			RHI::BufferData uploadData{};
			uploadData.dataSize = desc.bufferSize;
			uploadData.data = &meshModelMatrix;
			uploadData.startOffsetInBuffer = 0;

			meshModelBuffer->UploadData(uploadData);

			meshObjectSrg->Bind("_ObjectData", RHI::BufferView(meshModelBuffer));

			meshObjectSrg->Compile();
		}

		RHI::DrawIndexedArguments indexedArgs{};
		indexedArgs.firstIndex = 0;
		indexedArgs.firstInstance = 0;
		indexedArgs.instanceCount = 1;
		indexedArgs.indexCount = cubeMesh->indices.GetSize();
		indexedArgs.vertexOffset = 0;
		RHI::DrawArguments args = RHI::DrawArguments(indexedArgs);
		builder.SetDrawArguments(args);

		builder.AddShaderResourceGroup(meshObjectSrg);

		// Depth Item
		{
			DrawPacketBuilder::DrawItemRequest request{};
			RHI::VertexBufferView vertexBufferView = RHI::VertexBufferView(cubeMesh->buffer, cubeMesh->vertexBufferOffset,
				cubeMesh->vertexBufferSize,
				sizeof(Vec3) * 3 + sizeof(Vec2));
			request.vertexBufferViews.Add(vertexBufferView);

			RHI::IndexBufferView indexBufferView = RHI::IndexBufferView(cubeMesh->buffer, cubeMesh->indexBufferOffset, cubeMesh->indexBufferSize, RHI::IndexFormat::Uint16);
			request.indexBufferView = indexBufferView;

			request.drawItemTag = rhiSystem.GetDrawListTagRegistry()->AcquireTag("depth");
			request.drawFilterMask = RHI::DrawFilterMask::ALL;
			request.pipelineState = depthPipeline;
			
			builder.AddDrawItem(request);
		}
		
		// Opaque Item
		{
			DrawPacketBuilder::DrawItemRequest request{};
			RHI::VertexBufferView vertexBufferView = RHI::VertexBufferView(cubeMesh->buffer, cubeMesh->vertexBufferOffset,
				cubeMesh->vertexBufferSize,
				sizeof(Vec3) * 3 + sizeof(Vec2));
			request.vertexBufferViews.Add(vertexBufferView);

			RHI::IndexBufferView indexBufferView = RHI::IndexBufferView(cubeMesh->buffer, cubeMesh->indexBufferOffset, cubeMesh->indexBufferSize, RHI::IndexFormat::Uint16);
			request.indexBufferView = indexBufferView;
			
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

	void Mesh::CreateBuffer()
	{
		RHI::BufferDescriptor bufferDesc{};
		bufferDesc.bindFlags = RHI::BufferBindFlags::VertexBuffer | RHI::BufferBindFlags::IndexBuffer;
		bufferDesc.bufferSize = vertices.GetSize() * sizeof(Vec3)
			+ normals.GetSize() * sizeof(Vec3)
			+ tangents.GetSize() * sizeof(Vec3)
			+ uvCoords.GetSize() * sizeof(Vec2)
			+ indices.GetSize() * sizeof(u16);
		bufferDesc.name = "Cube Mesh Buffer";

		buffer = RHI::gDynamicRHI->CreateBuffer(bufferDesc);

		u8* data = (u8*)malloc(bufferDesc.bufferSize);
		SIZE_T offset = 0;
		vertexBufferOffset = offset;
		vertexBufferSize = 0;

		for (int i = 0; i < vertices.GetSize(); i++)
		{
			Vec3* vertexPtr = (Vec3*)(data + offset); offset += sizeof(Vec3);
			Vec3* normalPtr = (Vec3*)(data + offset); offset += sizeof(Vec3);
			Vec3* tangentPtr = (Vec3*)(data + offset); offset += sizeof(Vec3);
			Vec2* uvPtr = (Vec2*)(data + offset); offset += sizeof(Vec2);

			*vertexPtr = vertices[i];
			*normalPtr = normals[i];
			*tangentPtr = tangents[i];
			*uvPtr = uvCoords[i];
		}

		vertexBufferStride = sizeof(Vec3) * 3 + sizeof(Vec2);

		vertexBufferSize = offset;
		indexBufferOffset = offset;

		for (int i = 0; i < indices.GetSize(); i++)
		{
			u16* indexPtr = (u16*)(data + offset); offset += sizeof(u16);
			*indexPtr = (u16)indices[i];
		}
		indexBufferSize = indices.GetSize() * sizeof(u16);
		
		{
			RHI::BufferData uploadData{};
			uploadData.startOffsetInBuffer = 0;
			uploadData.dataSize = buffer->GetBufferSize();
			uploadData.data = data;
			
			buffer->UploadData(uploadData);
		}

		free(data);
	}

	void VulkanSandbox::DestroyModels()
	{
		scheduler->WaitUntilIdle();

		delete meshModelBuffer; meshModelBuffer = nullptr;
		delete meshObjectSrg; meshObjectSrg = nullptr;
		delete meshDrawPacket; meshDrawPacket = nullptr;

		for (auto mesh : meshes)
		{
			delete mesh;
		}
		meshes.Clear();
	}

	void VulkanSandbox::DestroyLights()
	{
		scheduler->WaitUntilIdle();

		delete perSceneSrg; perSceneSrg = nullptr;

		delete directionalLightsBuffer; directionalLightsBuffer = nullptr;
		delete lightDataBuffer; lightDataBuffer = nullptr;
	}

	void VulkanSandbox::DestroyPipelines()
	{
		delete perViewSrg; perViewSrg = nullptr;
		delete perViewBuffer; perViewBuffer = nullptr;

		delete depthPipeline; depthPipeline = nullptr;
		delete depthShaderVert; depthShaderVert = nullptr;

        delete opaqueShader; opaqueShader = nullptr;
		delete opaqueMaterial; opaqueMaterial = nullptr;

		delete transparentPipeline; transparentPipeline = nullptr;
	}

	void VulkanSandbox::BuildFrameGraph()
	{
		rebuild = false;
		recompile = true;

		scheduler->BeginFrameGraph();
		{
			FrameAttachmentDatabase& attachmentDatabase = scheduler->GetFrameAttachmentDatabase();

			RHI::ImageDescriptor depthDesc{};
			depthDesc.width = swapChain->GetWidth();
			depthDesc.height = swapChain->GetHeight();
			depthDesc.bindFlags = RHI::TextureBindFlags::Depth;
			depthDesc.format = RHI::gDynamicRHI->GetAvailableDepthOnlyFormats()[0];
			depthDesc.name = "DepthStencil";

			attachmentDatabase.EmplaceFrameAttachment("DepthStencil", depthDesc);
			attachmentDatabase.EmplaceFrameAttachment("SwapChain", swapChain);

			//scheduler->BeginScopeGroup("MainPass");
			scheduler->BeginScope("Depth");
			{
				RHI::ImageScopeAttachmentDescriptor depthAttachment{};
				depthAttachment.attachmentId = "DepthStencil";
				depthAttachment.loadStoreAction.clearValueDepth = 1.0f;
				depthAttachment.loadStoreAction.clearValueStencil = 0;
				depthAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Clear;
				depthAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;

				scheduler->UseAttachment(depthAttachment, RHI::ScopeAttachmentUsage::DepthStencil, RHI::ScopeAttachmentAccess::Write);
				
				scheduler->UseShaderResourceGroup(perViewSrg);

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
				swapChainAttachment.loadStoreAction.clearValue = Vec4(0, 0.5f, 0.5f, 1);
				swapChainAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Clear;
				swapChainAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;

				scheduler->UseAttachment(swapChainAttachment, RHI::ScopeAttachmentUsage::RenderTarget, RHI::ScopeAttachmentAccess::Write);

				scheduler->UseShaderResourceGroup(perSceneSrg);
				scheduler->UseShaderResourceGroup(perViewSrg);

				scheduler->UsePipeline(opaqueShader->GetVariant(0)->GetPipeline());

				scheduler->PresentSwapChain(swapChain);
			}
			scheduler->EndScope();

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

		TransientMemoryPool* pool = scheduler->GetTransientPool();
		RHI::MemoryHeap* imageHeap = pool->GetImagePool();
		if (imageHeap != nullptr)
		{
			CE_LOG(Info, All, "Transient Image Pool: {} MB", (imageHeap->GetHeapSize() / 1024.0f / 1024.0f));
		}
	}

	void VulkanSandbox::SubmitWork()
	{
		UpdateViewSrg();

		resubmit = false;
		drawList.Shutdown();
		RHI::DrawListMask drawListMask{};
		auto depthTag = rhiSystem.GetDrawListTagRegistry()->AcquireTag("depth");
		auto opaqueTag = rhiSystem.GetDrawListTagRegistry()->AcquireTag("opaque");
		//auto transparentTag = rhiSystem.GetDrawListTagRegistry()->AcquireTag("transparent");
		drawListMask.Set(depthTag);
		drawListMask.Set(opaqueTag);
		//drawListMask.Set(transparentTag);
		drawList.Init(drawListMask);
		
		// Add items
		drawList.AddDrawPacket(meshDrawPacket);

		// Finalize
		drawList.Finalize();
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
