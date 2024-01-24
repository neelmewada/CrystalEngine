#include "VulkanSandbox.h"

namespace CE::Sandbox
{

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

			CompileFrameGraph();
		}
		
		// Submit work
		{

		}

		scheduler->Execute();
	}

	void VulkanSandbox::Shutdown()
	{
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
			depthPipelineDesc.depthStencilState.depthState.compareOp = RHI::CompareOp::Less;
			depthPipelineDesc.depthStencilState.stencilState.enable = false;
			
			depthPipelineDesc.multisampleState.sampleCount = 1;
			depthPipelineDesc.multisampleState.quality = 0;

			depthPipelineDesc.shaderStages.Add({});
			depthPipelineDesc.shaderStages[0].entryPoint = "VertMain";
			depthPipelineDesc.shaderStages[0].shaderModule = depthShaderVert;

			depthPipelineDesc.rasterState = {};

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

			// Depth shader only uses position
			/*vertexAttribs.Add({});
			vertexAttribs[1].dataType = RHI::VertexAttributeDataType::Float3;
			vertexAttribs[1].inputSlot = 0;
			vertexAttribs[1].location = 1;
			vertexAttribs[1].offset = offsetof(VertexStruct, normal);

			vertexAttribs.Add({});
			vertexAttribs[2].dataType = RHI::VertexAttributeDataType::Float3;
			vertexAttribs[2].inputSlot = 0;
			vertexAttribs[2].location = 2;
			vertexAttribs[2].offset = offsetof(VertexStruct, tangent);

			vertexAttribs.Add({});
			vertexAttribs[3].dataType = RHI::VertexAttributeDataType::Float2;
			vertexAttribs[3].inputSlot = 0;
			vertexAttribs[3].location = 3;
			vertexAttribs[3].offset = offsetof(VertexStruct, uvCoord);*/

			Array<RHI::ShaderResourceGroupLayout>& srgLayouts = depthPipelineDesc.srgLayouts;
			RHI::ShaderResourceGroupLayout perViewSRG{};
			perViewSRG.srgType = RHI::SRGType::PerView;
			perViewSRG.variables.Add({});
			perViewSRG.variables[0].arrayCount = 1;
			perViewSRG.variables[0].name = "_PerViewData";
			perViewSRG.variables[0].bindingSlot = 0;
			perViewSRG.variables[0].type = RHI::ShaderResourceType::ConstantBuffer;
			perViewSRG.variables[0].shaderStages = RHI::ShaderStage::Vertex;
			srgLayouts.Add(perViewSRG);
			
			RHI::ShaderResourceGroupLayout perObjectSRG{};
			perObjectSRG.srgType = RHI::SRGType::PerObject;
			perObjectSRG.variables.Add({});
			perObjectSRG.variables[0].arrayCount = 1;
			perObjectSRG.variables[0].name = "_ObjectData";
			perObjectSRG.variables[0].bindingSlot = 1;
			perObjectSRG.variables[0].type = RHI::ShaderResourceType::ConstantBuffer;
			perObjectSRG.variables[0].shaderStages = RHI::ShaderStage::Vertex;
			srgLayouts.Add(perObjectSRG);

			depthPipelineDesc.name = "Depth Pipeline";

			depthPipeline = RHI::gDynamicRHI->CreateGraphicsPipeline(depthPipelineDesc);

			delete depthVert;
		}

		// Opaque Pipeline
		{

		}

		// Transparent Pipeline
		{

		}
	}

	void VulkanSandbox::InitModels()
	{
		Mesh* mesh = new Mesh();

		mesh->vertices = {
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

		mesh->indices = {
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

		mesh->uvCoords = {
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

		mesh->normals = {
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

		mesh->tangents = {
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

		mesh->CreateBuffer();
		meshes.Add(mesh);

		DrawPacketBuilder builder{};
		RHI::IndexBufferView indexBufferView = RHI::IndexBufferView(mesh->buffer, mesh->indexBufferOffset, mesh->indexBufferSize, RHI::IndexFormat::Uint16);
		builder.SetIndexBufferView(indexBufferView);

		// Depth Item
		{
			DrawPacketBuilder::DrawItemRequest request{};
			RHI::VertexBufferView vertexBufferView = RHI::VertexBufferView(mesh->buffer, mesh->vertexBufferOffset,
				mesh->vertexBufferSize,
				sizeof(Vec3) * 3 + sizeof(Vec2));
			request.vertexBufferViews.Add(vertexBufferView);
			request.drawItemTag = rhiSystem.GetDrawListTagRegistry()->AcquireTag("depth");
			request.drawFilterMask = RHI::DrawFilterMask::ALL;
			request.pipelineState = depthPipeline;
			builder.AddDrawItem(request);
		}
		
		// Opaque Item
		{
			DrawPacketBuilder::DrawItemRequest request{};
			RHI::VertexBufferView vertexBufferView = RHI::VertexBufferView(mesh->buffer, mesh->vertexBufferOffset,
				mesh->vertexBufferSize,
				sizeof(Vec3) * 3 + sizeof(Vec2));
			request.vertexBufferViews.Add(vertexBufferView);
			request.drawItemTag = rhiSystem.GetDrawListTagRegistry()->AcquireTag("opaque");
			request.drawFilterMask = RHI::DrawFilterMask::ALL;
			request.pipelineState = opaquePipeline;
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
		delete meshDrawPacket;

		for (auto mesh : meshes)
		{
			delete mesh;
		}
		meshes.Clear();
	}

	void VulkanSandbox::DestroyPipelines()
	{
		delete depthPipeline; depthPipeline = nullptr;
		delete depthShaderVert; depthShaderVert = nullptr;

		delete opaquePipeline;
		opaquePipeline = nullptr;

		delete transparentPipeline;
		transparentPipeline = nullptr;
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
			depthDesc.bindFlags = RHI::TextureBindFlags::DepthStencil;
			depthDesc.format = RHI::gDynamicRHI->GetAvailableDepthStencilFormats()[0];
			depthDesc.name = "DepthStencil";

			attachmentDatabase.EmplaceFrameAttachment("DepthStencil", depthDesc);
			attachmentDatabase.EmplaceFrameAttachment("SwapChain", swapChain);

			//scheduler->BeginScopeGroup("MainPass");
			scheduler->BeginScope("Depth");
			{
				RHI::ImageScopeAttachmentDescriptor depthAttachment{};
				depthAttachment.attachmentId = "DepthStencil";
				depthAttachment.loadStoreAction.clearValueDepth = 0;
				depthAttachment.loadStoreAction.clearValueStencil = 0;
				depthAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Clear;
				depthAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;
				depthAttachment.loadStoreAction.loadActionStencil = RHI::AttachmentLoadAction::DontCare;
				depthAttachment.loadStoreAction.storeActionStencil = RHI::AttachmentStoreAction::DontCare;

				scheduler->UseAttachment(depthAttachment, RHI::ScopeAttachmentUsage::DepthStencil, RHI::ScopeAttachmentAccess::Write);

				scheduler->UsePipeline(depthPipeline);
			}
			scheduler->EndScope();

			scheduler->BeginScope("Opaque");
			{
				RHI::ImageScopeAttachmentDescriptor depthAttachment{};
				depthAttachment.attachmentId = "DepthStencil";
				depthAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Load;
				depthAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;
				depthAttachment.loadStoreAction.loadActionStencil = RHI::AttachmentLoadAction::Load;
				depthAttachment.loadStoreAction.storeActionStencil = RHI::AttachmentStoreAction::Store;

				scheduler->UseAttachment(depthAttachment, RHI::ScopeAttachmentUsage::DepthStencil, RHI::ScopeAttachmentAccess::Read);

				RHI::ImageScopeAttachmentDescriptor swapChainAttachment{};
				swapChainAttachment.attachmentId = "SwapChain";
				swapChainAttachment.loadStoreAction.clearValue = Vec4(0, 0.5f, 0.5f, 1);
				swapChainAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Clear;
				swapChainAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;

				scheduler->UseAttachment(swapChainAttachment, RHI::ScopeAttachmentUsage::RenderTarget, RHI::ScopeAttachmentAccess::Write);

				//scheduler->UsePipeline(opaquePipeline);
			}
			scheduler->EndScope();

			scheduler->BeginScope("Transparent");
			{
				RHI::ImageScopeAttachmentDescriptor depthAttachment{};
				depthAttachment.attachmentId = "DepthStencil";
				depthAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Load;
				depthAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;
				depthAttachment.loadStoreAction.loadActionStencil = RHI::AttachmentLoadAction::Load;
				depthAttachment.loadStoreAction.storeActionStencil = RHI::AttachmentStoreAction::Store;

				scheduler->UseAttachment(depthAttachment, RHI::ScopeAttachmentUsage::DepthStencil, RHI::ScopeAttachmentAccess::Read);

				RHI::ImageScopeAttachmentDescriptor swapChainAttachment{};
				swapChainAttachment.attachmentId = "SwapChain";
				swapChainAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Load;
				swapChainAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;

				scheduler->UseAttachment(swapChainAttachment, RHI::ScopeAttachmentUsage::RenderTarget, RHI::ScopeAttachmentAccess::ReadWrite);

				//scheduler->UsePipeline(depthPipeline);

				scheduler->PresentSwapChain(swapChain);
			}
			scheduler->EndScope();
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
		resubmit = false;

		depthDrawList.ClearAll();
		{
			
		}
		scheduler->SetScopeDrawList("Depth", &depthDrawList);

		opaqueDrawList.ClearAll();
		{

		}
		scheduler->SetScopeDrawList("Opaque", &opaqueDrawList);

		transparentDrawList.ClearAll();
		{

		}
		scheduler->SetScopeDrawList("Transparent", &transparentDrawList);
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
