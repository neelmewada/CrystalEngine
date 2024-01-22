#include "VulkanSandbox.h"

namespace CE::Sandbox
{

	static int counter = 0;

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
		delete swapChain;
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

		indexBufferOffset = offset;

		for (int i = 0; i < indices.GetSize(); i++)
		{
			u16* indexPtr = (u16*)(data + offset); offset += sizeof(u16);
			*indexPtr = (u16)indices[i];
		}
		
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
		for (auto mesh : meshes)
		{
			delete mesh;
		}
		meshes.Clear();
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
