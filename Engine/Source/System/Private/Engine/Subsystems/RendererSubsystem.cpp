#include "System.h"

namespace CE
{
	RHI::Buffer* vertexBuffer = nullptr;

    RendererSubsystem::RendererSubsystem()
    {
		
    }

    void RendererSubsystem::RebuildFrameGraph()
    {
		rebuildFrameGraph = recompileFrameGraph = true;
    }

    f32 RendererSubsystem::GetTickPriority() const
    {
		return 2;
    }

    void RendererSubsystem::OnWindowCreated(PlatformWindow* window)
    {
		rebuildFrameGraph = recompileFrameGraph = true;
    }

    void RendererSubsystem::OnWindowDestroyed(PlatformWindow* window)
    {
		if (window && window->IsMainWindow())
		{
			CE_LOG(Info, All, "Main Window Destroyed");
		}

		rebuildFrameGraph = recompileFrameGraph = true;
    }

    void RendererSubsystem::OnWindowClosed(PlatformWindow* window)
    {
		rebuildFrameGraph = recompileFrameGraph = true;
    }

    void RendererSubsystem::OnWindowMinimized(PlatformWindow* window)
    {
		rebuildFrameGraph = recompileFrameGraph = true;
    }

    void RendererSubsystem::OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight)
    {
		rebuildFrameGraph = recompileFrameGraph = true;
    }

    void RendererSubsystem::OnWindowRestored(PlatformWindow* window)
    {
		rebuildFrameGraph = recompileFrameGraph = true;
    }

	void RendererSubsystem::OnWindowExposed(PlatformWindow* window)
	{
		//rebuildFrameGraph = recompileFrameGraph = true;
	}

    void RendererSubsystem::Initialize()
	{
		Super::Initialize();

		auto app = FusionApplication::TryGet();

		if (app)
		{
			app->onRenderViewportDestroyed.Bind(FUNCTION_BINDING(this, RemoveViewport));
		}
	}

	void RendererSubsystem::PostInitialize()
	{
		Super::PostInitialize();

		sceneSubsystem = gEngine->GetSubsystem<SceneSubsystem>();

		// - Feature Processors -

		RegisterFeatureProcessor<StaticMeshComponent, RPI::StaticMeshFeatureProcessor>();

		// - Scheduler -

		RHI::FrameSchedulerDescriptor desc{};
		desc.numFramesInFlight = 2;

		scheduler = RHI::FrameScheduler::Create(desc);

		PlatformWindow* mainWindow = PlatformApplication::Get()->GetMainWindow();

		// TODO: Implement multi scene support
		CE::Scene* mainScene = sceneSubsystem->GetActiveScene();

		if (mainWindow)
		{
			PlatformApplication::Get()->AddMessageHandler(this);

			FusionInitInfo initInfo{};
			initInfo.assetLoader = gEngine->GetAssetManager();

			FusionApplication::Get()->Initialize(initInfo);

			//activeScene->mainRenderViewport = primaryViewport;
		}
	}

	void RendererSubsystem::PreShutdown()
	{
		ZoneScoped;

		Super::PreShutdown();

		FusionApplication* app = FusionApplication::TryGet();

		if (app)
		{
			PlatformApplication::Get()->RemoveMessageHandler(this);

			app->Shutdown();
			app->BeginDestroy();
		}

		delete scheduler; scheduler = nullptr;
	}

	void RendererSubsystem::Shutdown()
	{
		Super::Shutdown();

	}

	void RendererSubsystem::Tick(f32 delta)
	{
		ZoneScoped;

		Super::Tick(delta);

		FusionApplication* app = FusionApplication::TryGet();

		int submittedImageIndex = -1;

		if (app)
		{
			app->Tick();
		}

		if (IsEngineRequestingExit())
		{
			return;
		}

		if (rebuildFrameGraph)
		{
			rebuildFrameGraph = false;
			recompileFrameGraph = true;

			BuildFrameGraph();
			submittedImageIndex = curImageIndex;
		}

		if (recompileFrameGraph)
		{
			recompileFrameGraph = false;

			CompileFrameGraph();
		}

		if (IsEngineRequestingExit())
		{
			return;
		}

		//CE::Scene* scene = sceneSubsystem->GetActiveScene();
		//RPI::Scene* rpiScene = scene->GetRpiScene();
		constexpr bool isSceneWindowActive = false;

		int imageIndex = scheduler->BeginExecution();

		if (imageIndex >= RHI::Limits::MaxSwapChainImageCount || rebuildFrameGraph || recompileFrameGraph)
		{
			rebuildFrameGraph = recompileFrameGraph = true;
			return;
		}

		curImageIndex = imageIndex;

		// ---------------------------------------------------------
		// - Enqueue draw packets to views

		if (submittedImageIndex != curImageIndex)
		{
			RPI::RPISystem::Get().SimulationTick(curImageIndex);
			RPI::RPISystem::Get().RenderTick(curImageIndex);
		}

		// ---------------------------------------------------------
		// - Submit draw lists to scopes for execution

		drawList.Shutdown();

		RHI::DrawListMask drawListMask{};
		HashSet<RHI::DrawListTag> drawListTags{};

		// - Setup draw list mask

		if (app)
		{
			app->UpdateDrawListMask(drawListMask);
		}

		for (FGameWindow* renderViewport : renderViewports)
		{
			RPI::Scene* rpiScene = renderViewport->GetScene();
			if (!rpiScene)
				continue;

			for (int i = 0; i < rpiScene->GetRenderPipelineCount(); ++i)
			{
				RPI::RenderPipeline* renderPipeline = rpiScene->GetRenderPipeline(i);
				if (!renderPipeline)
					continue;

				renderPipeline->GetPassTree()->IterateRecursively([&](RPI::Pass* pass)
					{
						if (!pass)
							return;

						if (pass->GetDrawListTag().IsValid())
						{
							drawListMask.Set(pass->GetDrawListTag());
						}
					});
			}
		}

		// - Enqueue additional draw packets

		for (int i = 0; i < drawListMask.GetSize(); ++i)
		{
			if (drawListMask.Test(i))
			{
				drawListTags.Add((u8)i);
			}
		}

		drawList.Init(drawListMask);

		if (app)
		{
			app->EnqueueDrawPackets(drawList, curImageIndex);
		}

		for (FGameWindow* renderViewport : renderViewports)
		{
			RPI::Scene* rpiScene = renderViewport->GetScene();
			if (!rpiScene)
				continue;

			for (const auto& [viewTag, views] : rpiScene->GetViews())
			{
				for (RPI::View* view : views.views)
				{
					view->GetDrawListContext()->Finalize();

					for (const auto& drawListTag : drawListTags)
					{
						RHI::DrawList& viewDrawList = view->GetDrawList(drawListTag);
						for (int i = 0; i < viewDrawList.GetDrawItemCount(); ++i)
						{
							drawList.AddDrawItem(viewDrawList.GetDrawItem(i), drawListTag);
						}
					}

					if (viewTag == "DirectionalLightShadow")
					{
						break;
					}
				}
			}
		}

		drawList.Finalize();

		// - Set scope draw lists
    
		if (app) // FWidget Scopes & DrawLists
		{
			app->FlushDrawPackets(drawList, curImageIndex);
		}

		for (FGameWindow* renderViewport : renderViewports)
		{
			RPI::Scene* rpiScene = renderViewport->GetScene();
			if (!rpiScene)
				continue;

			for (int i = 0; i < rpiScene->GetRenderPipelineCount(); ++i)
			{
				RPI::RenderPipeline* renderPipeline = rpiScene->GetRenderPipeline(i);
				if (!renderPipeline)
					continue;

				renderPipeline->GetPassTree()->IterateRecursively([&](RPI::Pass* pass)
					{
						if (!pass)
							return;

						RPI::SceneViewTag viewTag = pass->GetViewTag();
						RHI::DrawListTag passDrawTag = pass->GetDrawListTag();
						RHI::ScopeId scopeId = pass->GetScopeId();

						if (passDrawTag.IsValid() && viewTag.IsValid() && scopeId.IsValid())
						{
							scheduler->SetScopeDrawList(scopeId, &drawList.GetDrawListForTag(passDrawTag));
						}
					});
			}
		}

		scheduler->EndExecution();
	}

	void RendererSubsystem::BuildFrameGraph()
	{
		ZoneScoped;

		rebuildFrameGraph = false;
		recompileFrameGraph = true;

		// TODO: Implement multi scene support

		// TODO: Enqueue draw packets early! Some scope producers (shadow, reflection probe, etc.) need to have all draw packets available beforehand.

		RPI::RPISystem::Get().SimulationTick(curImageIndex);
		RPI::RPISystem::Get().RenderTick(curImageIndex);

		RHI::FrameAttachmentDatabase& attachmentDatabase = scheduler->GetAttachmentDatabase();
    	
		scheduler->BeginFrameGraph();
		{
			auto app = FusionApplication::TryGet();

			if (app)
			{
				app->EmplaceFrameAttachments();

				// Cleanup first
				for (FGameWindow* renderViewport : renderViewports)
				{
					FNativeContext* nativeContext = static_cast<FNativeContext*>(renderViewport->GetContext());
					if (!nativeContext)
						continue;

					nativeContext->shaderReadOnlyAttachmentDependencies.Clear();
					nativeContext->shaderWriteAttachmentDependencies.Clear();
				}

				for (FGameWindow* renderViewport : renderViewports)
				{
					RPI::Scene* rpiScene = renderViewport->GetScene();
					CE::Scene* scene = sceneSubsystem->FindRpiSceneOwner(rpiScene);
					if (scene == nullptr || !scene->IsEnabled())
						continue;
					if (!renderViewport->IsVisibleInHierarchy())
						continue;
					FNativeContext* nativeContext = static_cast<FNativeContext*>(renderViewport->GetContext());

					for (CE::RenderPipeline* renderPipeline : scene->renderPipelines)
					{
						RPI::RenderPipeline* rpiPipeline = renderPipeline->GetRpiRenderPipeline();
						const auto& attachments = rpiPipeline->attachments;

						for (RPI::PassAttachment* passAttachment : attachments)
						{
							if (passAttachment->lifetime == RHI::AttachmentLifetimeType::External && passAttachment->name == "PipelineOutput")
							{
								if (renderViewport->IsEmbeddedViewport())
								{
									FViewport* viewport = static_cast<FViewport*>(renderViewport);

									passAttachment->attachmentId = String::Format("Viewport_{}", viewport->GetUuid());

									StaticArray<RHI::Texture*, RHI::Limits::MaxSwapChainImageCount> frameBuffer{};

									RHI::ImageScopeAttachmentDescriptor descriptor{};
									descriptor.attachmentId = passAttachment->attachmentId;
									descriptor.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Load;
									descriptor.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;

									nativeContext->shaderReadOnlyAttachmentDependencies.Add(descriptor);

									for (int i = 0; i < frameBuffer.GetSize(); ++i)
									{
										if (viewport->GetFrame(i) == nullptr)
										{
											viewport->RecreateFrameBuffer();
										}

										frameBuffer[i] = viewport->GetFrame(i)->GetRhiTexture();
									}

									attachmentDatabase.EmplaceFrameAttachment(passAttachment->attachmentId, frameBuffer);
								}
								else
								{
									passAttachment->attachmentId = nativeContext->attachmentId;
								}
							}
							else
							{
								passAttachment->attachmentId = String::Format("{}_{}", passAttachment->name, rpiPipeline->uuid);
							}
						}

						rpiPipeline->ImportScopeProducers(scheduler);
					}
				}

				app->EnqueueScopes();
			}
		}
		scheduler->EndFrameGraph();
	}

	void RendererSubsystem::CompileFrameGraph()
	{
		ZoneScoped;

		recompileFrameGraph = false;

		scheduler->Compile();

		RHI::TransientMemoryPool* pool = scheduler->GetTransientPool();
		RHI::MemoryHeap* imageHeap = pool->GetImagePool();
		if (imageHeap != nullptr)
		{
			//CE_LOG(Info, All, "Transient Image Pool: {} MB", (imageHeap->GetHeapSize() / 1024.0f / 1024.0f));
		}
	}

	void RendererSubsystem::SubmitDrawPackets(int imageIndex)
	{
		
	}

	void RendererSubsystem::RegisterFeatureProcessor(SubClass<ActorComponent> componentClass,
		SubClass<RPI::FeatureProcessor> fpClass)
	{
		if (componentClass == nullptr || fpClass == nullptr)
			return;

		componentClassToFeatureProcessorClass[componentClass] = fpClass;
	}

	SubClass<RPI::FeatureProcessor> RendererSubsystem::GetFeatureProcessClass(SubClass<ActorComponent> componentClass)
	{
		if (componentClass == nullptr)
			return nullptr;

		Class* parentClass = componentClass;

		while (parentClass != nullptr)
		{
			if (parentClass->GetSuperClassCount() == 0 || parentClass == GetStaticClass<Object>())
				break;

			if (componentClassToFeatureProcessorClass.KeyExists(parentClass))
			{
				return componentClassToFeatureProcessorClass[parentClass];
			}
			parentClass = parentClass->GetSuperClass(0);
		}

		return nullptr;
	}

	void RendererSubsystem::AddViewport(FGameWindow* viewport)
	{
		if (renderViewports.Exists(viewport))
		{
			RebuildFrameGraph();
			return;
		}

		renderViewports.Add(viewport);
		RebuildFrameGraph();

		if (viewport->IsEmbeddedViewport())
		{
			FViewport* embeddedViewport = static_cast<FViewport*>(viewport);
			embeddedViewport->OnFrameBufferRecreated(FUNCTION_BINDING(this, RebuildFrameGraph));
		}
	}

	void RendererSubsystem::RemoveViewport(FGameWindow* viewport)
	{
		if (renderViewports.Remove(viewport))
		{
			RebuildFrameGraph();
		}
	}

	void RendererSubsystem::OnSceneDestroyed(CE::Scene* scene)
	{
		if (!scene)
			return;

		for (FGameWindow* renderViewport : renderViewports)
		{
			if (renderViewport != nullptr && renderViewport->GetScene() == scene->GetRpiScene())
			{
				renderViewports.Remove(renderViewport);
				return;
			}
		}
	}

} // namespace CE
