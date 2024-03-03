#include "VulkanSandbox.h"

#include <algorithm>

namespace CE
{
	constexpr u32 directionalLightArrayBinding = 0;
	constexpr u32 pointLightsBinding = 1;
	constexpr u32 lightDataBinding = 2;
	constexpr u32 shadowSamplerBinding = 3;
	constexpr u32 sceneDataBinding = 4;
	constexpr u32 skyboxBinding = 5;
	constexpr u32 defaultSamplerBinding = 6;
	constexpr u32 skyboxIrradianceBinding = 7;
	constexpr u32 skyboxSamplerBinding = 8;

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
		//InitCubeMapDemo();
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

		//CubeMapDemoTick(deltaTime);

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

	void VulkanSandbox::InitCubeMapDemo()
	{
		IO::Path path = PlatformDirectories::GetLaunchDir() / "Engine/Assets/Textures/HDRI/sample_night2.hdr";
		equirectShader = gEngine->GetAssetManager()->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/CubeMap/Equirectangular");

		CMImage hdrImage = CMImage::LoadFromFile(path);
		defer(
			hdrImage.Free();
		);

		/////////////////////////////////////////////
		// - Input Resources -

		RHI::TextureDescriptor hdriMapDesc{};
		hdriMapDesc.name = "HDRI FlatMap";
		hdriMapDesc.bindFlags = RHI::TextureBindFlags::ShaderRead;
		hdriMapDesc.width = hdrImage.GetWidth();
		hdriMapDesc.height = hdrImage.GetHeight();
		hdriMapDesc.depth = 1;
		hdriMapDesc.dimension = RHI::Dimension::Dim2D;
		hdriMapDesc.mipLevels = 1;
		hdriMapDesc.sampleCount = 1;
		hdriMapDesc.arrayLayers = 1;
		hdriMapDesc.format = RHI::Format::R16G16B16A16_SFLOAT;

		RHI::Texture* hdriMap = RHI::gDynamicRHI->CreateTexture(hdriMapDesc);
		hdriMapRpi = new RPI::Texture(hdriMap);

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

		RPI::Shader* equirectShader = this->equirectShader->GetOrCreateRPIShader(0);

		RHI::SamplerDescriptor samplerDesc{};
		samplerDesc.addressModeU = samplerDesc.addressModeV = samplerDesc.addressModeW = SamplerAddressMode::Repeat;
		samplerDesc.enableAnisotropy = false;
		samplerDesc.samplerFilterMode = RHI::FilterMode::Linear;
		RHI::Sampler* cubeMapSampler = RPISystem::Get().FindOrCreateSampler(samplerDesc);

		for (int i = 0; i < 6; i++)
		{
			equirectMaterials[i] = new RPI::Material(equirectShader);
			equirectMaterials[i]->SelectVariant(0);

			equirectMaterials[i]->SetPropertyValue("viewMatrix", captureViewMatrices[i]);
			equirectMaterials[i]->SetPropertyValue("projectionMatrix", captureProjection);
			equirectMaterials[i]->SetPropertyValue("viewProjectionMatrix", captureProjection * captureViewMatrices[i]);
			equirectMaterials[i]->SetPropertyValue("viewPosition", Vec4(0, 0, 0, 0));
			equirectMaterials[i]->SetPropertyValue("_InputSampler", cubeMapSampler);
			equirectMaterials[i]->SetPropertyValue("_InputTexture", hdriMapRpi);
			equirectMaterials[i]->FlushProperties();
		}

		/////////////////////////////////////////////
		// - Output Resources -

		RHI::TextureDescriptor cubeMapDesc{};
		cubeMapDesc.name = "HDRI CubeMap";
		cubeMapDesc.bindFlags = RHI::TextureBindFlags::Color | RHI::TextureBindFlags::ShaderReadWrite;
		cubeMapDesc.defaultHeapType = MemoryHeapType::Default;
		cubeMapDesc.width = cubeMapDesc.height = 1024;
		cubeMapDesc.depth = 1;
		cubeMapDesc.dimension = RHI::Dimension::DimCUBE;
		cubeMapDesc.mipLevels = 1;
		cubeMapDesc.sampleCount = 1;
		cubeMapDesc.arrayLayers = 6;
		cubeMapDesc.format = RHI::Format::R16G16B16A16_SFLOAT;

		// Final CubeMap texture with 6 faces
		cubeMapDemo = RHI::gDynamicRHI->CreateTexture(cubeMapDesc);

		/////////////////////////////////////////////
		// - Intermediate Resources -
		
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
			void* srcPtr = hdrImage.GetDataPtr();
			int numPixels = hdriMapDesc.width * hdriMapDesc.height;

			for (int i = 0; i < numPixels; i++)
			{
				u16* u16Data = (u16*)dataPtr;
				f32* f32Data = (f32*)dataPtr;

				f32 r = *((f32*)srcPtr + 4 * i + 0);
				f32 g = *((f32*)srcPtr + 4 * i + 1);
				f32 b = *((f32*)srcPtr + 4 * i + 2);

				// 16 bit float cannot store value higher than 65,000
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
		}
		stagingBuffer->Unmap();
		dataPtr = nullptr;

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

			barrier.resource = cubeMapDemo;
			barrier.fromState = ResourceState::Undefined;
			barrier.toState = ResourceState::ColorOutput;
			barrier.subresourceRange = RHI::SubresourceRange::All();
			cmdList->ResourceBarrier(1, &barrier);
		}
		cmdList->End();

		queue->Execute(1, &cmdList, fence);
		fence->WaitForFence();

		RHI::gDynamicRHI->FreeCommandLists(1, &cmdList);
		RHI::gDynamicRHI->DestroyFence(fence);
		hdrFence = RHI::gDynamicRHI->CreateFence(false);

		hdrCmdList = RHI::gDynamicRHI->AllocateCommandList(queue);
		hdrQueue = queue;

		RHI::RenderTargetLayout rtLayout{};
		RHI::RenderAttachmentLayout colorAttachment{};
		colorAttachment.attachmentId = "Color";
		colorAttachment.attachmentUsage = ScopeAttachmentUsage::Color;
		colorAttachment.format = RHI::Format::R16G16B16A16_SFLOAT;
		colorAttachment.multisampleState.sampleCount = 1;
		colorAttachment.loadAction = AttachmentLoadAction::Clear;
		colorAttachment.storeAction = AttachmentStoreAction::Store;
		colorAttachment.loadActionStencil = AttachmentLoadAction::DontCare;
		colorAttachment.storeActionStencil = AttachmentStoreAction::DontCare;
		rtLayout.attachmentLayouts.Add(colorAttachment);
		hdrRenderTarget = RHI::gDynamicRHI->CreateRenderTarget(rtLayout);

		for (int i = 0; i < 6; i++)
		{
			RHI::TextureViewDescriptor viewDesc{};
			viewDesc.texture = cubeMapDemo;
			viewDesc.baseArrayLayer = i;
			viewDesc.arrayLayerCount = 1;
			viewDesc.baseMipLevel = 0;
			viewDesc.mipLevelCount = 1;
			viewDesc.dimension = RHI::Dimension::Dim2D;
			viewDesc.format = Format::R16G16B16A16_SFLOAT;

			hdrFaceRTVs[i] = RHI::gDynamicRHI->CreateTextureView(viewDesc);
			hdrFaceRTBs[i] = RHI::gDynamicRHI->CreateRenderTargetBuffer(hdrRenderTarget, { hdrFaceRTVs[i] });
		}
	}

	void VulkanSandbox::CubeMapDemoTick(f32 deltaTime)
	{
		// Cube Model
		RPI::Mesh* cubeMesh = cubeModel->GetMesh(0);
		const auto& cubeVertInfo = cubeMesh->vertexBufferInfos[0];
		auto cubeVertexBufferView = RHI::VertexBufferView(cubeModel->GetBuffer(cubeVertInfo.bufferIndex), cubeVertInfo.byteOffset, cubeVertInfo.byteCount, cubeVertInfo.stride);

		RHI::AttachmentClearValue clearValue{};
		clearValue.clearValue = Vec4(0, 0, 0, 1);

		hdrCmdList->Begin();
		{
			for (int i = 0; i < 6; i++)
			{
				hdrCmdList->BeginRenderTarget(hdrRenderTarget, hdrFaceRTBs[i], &clearValue);
				{
					RHI::ViewportState viewport{};
					viewport.x = viewport.y = 0;
					viewport.width = cubeMapDemo->GetWidth();
					viewport.height = cubeMapDemo->GetHeight();
					viewport.minDepth = 0;
					viewport.maxDepth = 1;
					hdrCmdList->SetViewports(1, &viewport);

					RHI::ScissorState scissor{};
					scissor.x = scissor.y = 0;
					scissor.width = viewport.width;
					scissor.height = viewport.height;
					hdrCmdList->SetScissors(1, &scissor);

					RHI::PipelineState* pipeline = equirectMaterials[i]->GetCurrentShader()->GetPipeline();
					hdrCmdList->BindPipelineState(pipeline);

					hdrCmdList->BindVertexBuffers(0, 1, &cubeVertexBufferView);
					hdrCmdList->BindIndexBuffer(cubeMesh->indexBufferView);

					hdrCmdList->SetShaderResourceGroup(equirectMaterials[i]->GetShaderResourceGroup());

					hdrCmdList->CommitShaderResources();

					hdrCmdList->DrawIndexed(cubeMesh->drawArguments.indexedArgs);
				}
				hdrCmdList->EndRenderTarget();
			}
		}
		hdrCmdList->End();

		hdrQueue->Execute(1, &hdrCmdList, hdrFence);
		hdrFence->WaitForFence();
		hdrFence->Reset();

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

			perSceneSrgLayout.variables.Add({});
			perSceneSrgLayout.variables.Top().arrayCount = 1;
			perSceneSrgLayout.variables.Top().name = "_SkyboxSampler";
			perSceneSrgLayout.variables.Top().bindingSlot = skyboxSamplerBinding;
			perSceneSrgLayout.variables.Top().type = RHI::ShaderResourceType::SamplerState;
			perSceneSrgLayout.variables.Top().shaderStages = RHI::ShaderStage::Fragment;

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

			RHI::SamplerDescriptor skyboxSamplerDesc{};
			skyboxSamplerDesc.borderColor = RHI::SamplerBorderColor::FloatOpaqueWhite;
			skyboxSamplerDesc.addressModeU = shadowSamplerDesc.addressModeV = shadowSamplerDesc.addressModeW = RHI::SamplerAddressMode::Repeat;
			skyboxSamplerDesc.enableAnisotropy = false;
			skyboxSamplerDesc.maxAnisotropy = 0;
			skyboxSamplerDesc.samplerFilterMode = RHI::FilterMode::Linear;
			skyboxSampler = RHI::gDynamicRHI->CreateSampler(skyboxSamplerDesc);

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

			CE::TextureCube* cubeMapTex = gEngine->GetAssetManager()->LoadAssetAtPath<CE::TextureCube>("/Engine/Assets/Textures/HDRI/sample_night");
			if (cubeMapTex != nullptr)
			{
				perSceneSrg->Bind("_Skybox", cubeMapTex->GetRpiTexture()->GetRhiTexture());
				if (cubeMapTex->GetDiffuseConvolution() != nullptr)
				{
					perSceneSrg->Bind("_SkyboxIrradiance", cubeMapTex->GetDiffuseConvolution()->GetRpiTexture()->GetRhiTexture());
				}
				else
				{
					perSceneSrg->Bind("_SkyboxIrradiance", irradianceMap);
				}
			}
			else
			{
				perSceneSrg->Bind("_Skybox", hdriCubeMap);
				perSceneSrg->Bind("_SkyboxIrradiance", irradianceMap);
			}

			perSceneSrg->Bind("_DefaultSampler", defaultSampler);
			perSceneSrg->Bind("_SkyboxSampler", skyboxSampler);

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
            
			AssetManager* assetManager = gEngine->GetAssetManager();
			opaqueShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/PBR/Standard");

			opaqueRpiShader = opaqueShader->GetOrCreateRPIShader(0); // 0 pass index

			sphereMaterial = new RPI::Material(opaqueRpiShader);
			
			{
				sphereMaterial->SetPropertyValue("_Albedo", Color(1, 1, 1, 1));
				sphereMaterial->SetPropertyValue("_SpecularStrength", 1.0f);
				sphereMaterial->SetPropertyValue("_Shininess", (u32)64);
				sphereMaterial->SetPropertyValue("_Metallic", 0.0f);
				sphereMaterial->SetPropertyValue("_Roughness", 1.0f);
				sphereMaterial->SetPropertyValue("_NormalStrength", 1.0f);
				sphereMaterial->SetPropertyValue("_AmbientOcclusion", 1.0f);

                sphereMaterial->SetPropertyValue("_AlbedoTex", rpiSystem.GetDefaultAlbedoTex());
                sphereMaterial->SetPropertyValue("_RoughnessTex", rpiSystem.GetDefaultRoughnessTex());
                sphereMaterial->SetPropertyValue("_NormalTex", rpiSystem.GetDefaultNormalTex());
                sphereMaterial->SetPropertyValue("_MetallicTex", rpiSystem.GetDefaultAlbedoTex());

				//sphereMaterial->SetPropertyValue("_AlbedoTex", plasticTextures.albedo->GetRpiTexture());
				//sphereMaterial->SetPropertyValue("_RoughnessTex", plasticTextures.roughnessMap->GetRpiTexture());
				//sphereMaterial->SetPropertyValue("_NormalTex", plasticTextures.normalMap->GetRpiTexture());
				//sphereMaterial->SetPropertyValue("_MetallicTex", plasticTextures.metallicMap->GetRpiTexture());

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

			cubeMaterial = new RPI::Material(opaqueRpiShader);
			
			{
				cubeMaterial->SetPropertyValue("_Albedo", Color(1, 1, 1, 1));
				cubeMaterial->SetPropertyValue("_SpecularStrength", 1.0f);
				cubeMaterial->SetPropertyValue("_Shininess", (u32)64);
				cubeMaterial->SetPropertyValue("_Metallic", 0.0f);
				cubeMaterial->SetPropertyValue("_Roughness", 1.0f);
				cubeMaterial->SetPropertyValue("_NormalStrength", 1.0f);
				cubeMaterial->SetPropertyValue("_AmbientOcclusion", 1.0f);

				cubeMaterial->SetPropertyValue("_AlbedoTex", woodFloorTextures.albedo->GetRpiTexture()); //rpiSystem.GetDefaultAlbedoTex());
				cubeMaterial->SetPropertyValue("_RoughnessTex", woodFloorTextures.roughnessMap->GetRpiTexture()); //rpiSystem.GetDefaultRoughnessTex());
				cubeMaterial->SetPropertyValue("_NormalTex", woodFloorTextures.normalMap->GetRpiTexture()); //rpiSystem.GetDefaultNormalTex());
				cubeMaterial->SetPropertyValue("_MetallicTex", woodFloorTextures.metallicMap->GetRpiTexture()); // White metallic tex

				//cubeMaterial->SetPropertyValue("_AlbedoTex", rpiSystem.GetDefaultAlbedoTex());
				//cubeMaterial->SetPropertyValue("_RoughnessTex", rpiSystem.GetDefaultRoughnessTex());
				//cubeMaterial->SetPropertyValue("_NormalTex", rpiSystem.GetDefaultNormalTex());
				//cubeMaterial->SetPropertyValue("_MetallicTex", rpiSystem.GetDefaultAlbedoTex());
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
			u32 variantIdx = cubeMaterial->GetCurrentVariantIndex();
			const Array<Name>& vertexInputs = opaqueShader->GetShaderPass(0)->variants[variantIdx].reflectionInfo.vertexInputs;

			RHI::DrawPacketBuilder::DrawItemRequest request{};

			for (const auto& vertexInputName : vertexInputs)
			{
				RHI::ShaderSemantic curSemantic = RHI::ShaderSemantic::Parse(vertexInputName.GetString());

				for (const auto& vertInfo : mesh->vertexBufferInfos)
				{
					if (vertInfo.semantic == curSemantic)
					{
						auto vertBufferView = RHI::VertexBufferView(cubeModel->GetBuffer(vertInfo.bufferIndex), vertInfo.byteOffset, vertInfo.byteCount, vertInfo.stride);
						request.vertexBufferViews.Add(vertBufferView);
						break;
					}
				}
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
			u32 variantIdx = sphereMaterial->GetCurrentVariantIndex();
			const Array<Name>& vertexInputs = opaqueShader->GetShaderPass(0)->variants[variantIdx].reflectionInfo.vertexInputs;

			RHI::DrawPacketBuilder::DrawItemRequest request{};

			for (const auto& vertexInputName : vertexInputs)
			{
				RHI::ShaderSemantic curSemantic = RHI::ShaderSemantic::Parse(vertexInputName.GetString());

				for (const auto& vertInfo : mesh->vertexBufferInfos)
				{
					if (vertInfo.semantic == curSemantic)
					{
						auto vertBufferView = RHI::VertexBufferView(sphereModel->GetBuffer(vertInfo.bufferIndex), vertInfo.byteOffset, vertInfo.byteCount, vertInfo.stride);
						request.vertexBufferViews.Add(vertBufferView);
						break;
					}
				}
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
		equirectShader->Destroy(); equirectShader = nullptr;
		delete shadowMapSampler; shadowMapSampler = nullptr;
		delete skyboxSampler; skyboxSampler = nullptr;
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

        //delete opaqueShader; opaqueShader = nullptr;
		opaqueShader->Destroy(); opaqueShader = nullptr;
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

				for (int i = 0; i < opaqueRpiShader->GetVariantCount(); i++)
				{
					scheduler->UsePipeline(opaqueRpiShader->GetVariant(i)->GetPipeline());
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

		String basePath = "/Engine/Assets/Textures/" + pathName.GetString();

		Name albedoPath = basePath + "/albedo";
		Name normalPath = basePath + "/normal";
		Name roughnessPath = basePath + "/roughness";
		Name metallicPath = basePath + "/metallic";

		CE::Texture2D* albedoImage = nullptr;
		CE::Texture2D* normalImage = nullptr;
		CE::Texture2D* roughnessImage = nullptr;
		CE::Texture2D* metallicImage = nullptr;

		auto assetManager = AssetManager::Get();

		auto prevTime = clock();

		if (true)
		{
			auto t1 = Thread([&]()
				{
					albedoImage = assetManager->LoadAssetAtPath<CE::Texture2D>(albedoPath);
				});
			auto t2 = Thread([&]()
				{
					normalImage = assetManager->LoadAssetAtPath<CE::Texture2D>(normalPath);
				});
			auto t3 = Thread([&]()
				{
					roughnessImage = assetManager->LoadAssetAtPath<CE::Texture2D>(roughnessPath);
				});
			auto t4 = Thread([&]()
				{
					metallicImage = assetManager->LoadAssetAtPath<CE::Texture2D>(metallicPath);
				});

			t1.Join();
			t2.Join();
			t3.Join();
			t4.Join();
		}
		else
		{
			albedoImage = assetManager->LoadAssetAtPath<CE::Texture2D>(albedoPath);
			normalImage = assetManager->LoadAssetAtPath<CE::Texture2D>(normalPath);
			roughnessImage = assetManager->LoadAssetAtPath<CE::Texture2D>(roughnessPath);
			metallicImage = assetManager->LoadAssetAtPath<CE::Texture2D>(metallicPath);
		}

		auto timeTaken = ((f32)(clock() - prevTime)) / CLOCKS_PER_SEC;

		result.name = pathName;
		result.albedo = albedoImage;
		result.normalMap = normalImage;
		result.roughnessMap = roughnessImage;
		result.metallicMap = metallicImage;

		return result;
	}

	void MaterialTextureGroup::Release()
	{
		//delete albedo; albedo = nullptr;
		//delete normalMap; normalMap = nullptr;
		//delete roughnessMap; roughnessMap = nullptr;
		//delete metallicMap; metallicMap = nullptr;
	}

} // namespace CE
