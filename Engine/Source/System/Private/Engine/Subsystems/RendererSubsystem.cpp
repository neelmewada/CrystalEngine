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
		
	}

    void RendererSubsystem::Initialize()
	{
		Super::Initialize();


	}

	void RendererSubsystem::PostInitialize()
	{
		Super::PostInitialize();

		sceneSubsystem = gEngine->GetSubsystem<SceneSubsystem>();

		// - Feature Processors -

		RegisterFeatureProcessor<StaticMeshComponent, StaticMeshFeatureProcessor>();

		// - Scheduler -

		RHI::FrameSchedulerDescriptor desc{};
		desc.numFramesInFlight = 2;

		scheduler = FrameScheduler::Create(desc);

		PlatformWindow* mainWindow = PlatformApplication::Get()->GetMainWindow();

		// TODO: Implement multi scene support
		CE::Scene* activeScene = sceneSubsystem->GetMainScene();

		if (mainWindow)
		{
			PlatformApplication::Get()->AddMessageHandler(this);

			auto assetManager = gEngine->GetAssetManager();

			auto renderer2dShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/2D/SDFGeometry");

			auto fontAsset = assetManager->LoadAssetAtPath<Font>("/Engine/Assets/Fonts/Roboto");
			auto poppinsFont = assetManager->LoadAssetAtPath<Font>("/Engine/Assets/Fonts/Poppins");

			auto atlasData = fontAsset->GetAtlasData();

			RPI::ShaderCollection* draw2dShaderCollection = renderer2dShader->GetShaderCollection();
			
			/*CApplicationInitInfo appInitInfo{};
			appInitInfo.draw2dShader = draw2dShaderCollection->At(0).shader;
			appInitInfo.defaultFont = atlasData;
			appInitInfo.defaultFontName = "Roboto";
			appInitInfo.scheduler = GetScheduler();
			appInitInfo.resourceLoader = this;

			CApplication::Get()->Initialize(appInitInfo);

			CApplication::Get()->RegisterFont("Poppins", poppinsFont->GetAtlasData());*/

			// TODO: Implement editor window support later
			//gameWindow = CreateWindow<CGameWindow>(gProjectName, mainWindow);

			//activeScene->mainRenderViewport = primaryViewport;
		}
	}

	void RendererSubsystem::PreShutdown()
	{
		Super::PreShutdown();

		/*CApplication* app = CApplication::TryGet();

		if (app)
		{
			PlatformApplication::Get()->RemoveMessageHandler(this);

			app->Shutdown();
			app->Destroy();
		}*/

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
		bool isExiting = IsEngineRequestingExit();

		/*CApplication* app = CApplication::TryGet();
		if (app)
		{
			app->Tick();
		}*/

		int submittedImageIndex = -1;

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

		CE::Scene* scene = sceneSubsystem->GetMainScene();
		RPI::Scene* rpiScene = scene->GetRpiScene();
		bool isSceneWindowActive = true;

		/*if (sceneSubsystem->mainViewport != nullptr)
		{
			CPlatformWindow* nativeWindow = sceneSubsystem->mainViewport->GetNativeWindow();
			if (nativeWindow && nativeWindow->GetPlatformWindow()->IsMinimized())
			{
				isSceneWindowActive = false;
			}
		}*/

		int imageIndex = scheduler->BeginExecution();

		if (imageIndex >= RHI::Limits::MaxSwapChainImageCount)
		{
			rebuildFrameGraph = recompileFrameGraph = true;
			return;
		}

		curImageIndex = imageIndex;

		// ---------------------------------------------------------
		// - Enqueue draw packets to views

		if (submittedImageIndex != curImageIndex)
		{
			RPISystem::Get().SimulationTick(curImageIndex);
			RPISystem::Get().RenderTick(curImageIndex);
		}

		// ---------------------------------------------------------
		// - Submit draw lists to scopes for execution

		drawList.Shutdown();

		RHI::DrawListMask drawListMask{};
		HashSet<RHI::DrawListTag> drawListTags{};

		// - Setup draw list mask

		/*if (app)
		{
			app->SetDrawListMasks(drawListMask);
		}*/

		for (int i = 0; i < rpiScene->GetRenderPipelineCount(); ++i)
		{
			RPI::RenderPipeline* renderPipeline = rpiScene->GetRenderPipeline(i);
			if (!renderPipeline)
				continue;

			renderPipeline->GetPassTree()->IterateRecursively([&](Pass* pass)
				{
					if (!pass)
						return;

					if (pass->GetDrawListTag().IsValid())
					{
						drawListMask.Set(pass->GetDrawListTag());
					}
				});
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

		/*if (app)
		{
			app->FlushDrawPackets(drawList, curImageIndex);
		}*/

		if (isSceneWindowActive)
	    {
		    for (const auto& [viewTag, views] : rpiScene->GetViews())
		    {
		    	for (View* view : views.views)
		    	{
		    		view->GetDrawListContext()->Finalize();

		    		for (const auto& drawListTag : drawListTags)
		    		{
		    			DrawList& viewDrawList = view->GetDrawList(drawListTag);
		    			for (int i = 0; i < viewDrawList.GetDrawItemCount(); ++i)
		    			{
		    				drawList.AddDrawItem(viewDrawList.GetDrawItem(i), drawListTag);
		    			}
		    		}
		    	}
		    }
	    }

		drawList.Finalize();

		// - Set scope draw lists
    
		/*if (app) // CWidget Scopes & DrawLists
		{
			app->SubmitDrawPackets(drawList);
		}*/

		for (int i = 0; isSceneWindowActive && i < rpiScene->GetRenderPipelineCount(); ++i)
		{
			RPI::RenderPipeline* renderPipeline = rpiScene->GetRenderPipeline(i);
			if (!renderPipeline)
				continue;

			renderPipeline->GetPassTree()->IterateRecursively([&](Pass* pass)
				{
					if (!pass)
						return;

					SceneViewTag viewTag = pass->GetViewTag();
					DrawListTag passDrawTag = pass->GetDrawListTag();
					ScopeId scopeId = pass->GetScopeId();

					if (passDrawTag.IsValid() && viewTag.IsValid() && scopeId.IsValid())
					{
						scheduler->SetScopeDrawList(scopeId, &drawList.GetDrawListForTag(passDrawTag));
					}
				});
		}

		scheduler->EndExecution();
	}

	void RendererSubsystem::BuildFrameGraph()
	{
		rebuildFrameGraph = false;
		recompileFrameGraph = true;

		// TODO: Implement multi scene support
    	CE::Scene* scene = sceneSubsystem->GetMainScene();

		bool isSceneWindowActive = true;
		
		/*if (sceneSubsystem->mainViewport != nullptr)
		{
			CPlatformWindow* nativeWindow = sceneSubsystem->mainViewport->GetNativeWindow();
			if (nativeWindow && nativeWindow->GetPlatformWindow()->IsMinimized())
			{
				isSceneWindowActive = false;
			}
		}*/

		// TODO: Enqueue draw packets early! Some scope producers need to have all draw packets available beforehand.
		RPISystem::Get().SimulationTick(curImageIndex);
		RPISystem::Get().RenderTick(curImageIndex);

		FrameAttachmentDatabase& attachmentDatabase = scheduler->GetAttachmentDatabase();
    	
		scheduler->BeginFrameGraph();
		{
			/*auto cApp = CApplication::TryGet();
			
			if (cApp)
			{
				cApp->FrameGraphBegin();

				if (isSceneWindowActive && scene->IsEnabled())
				{
					CWindow* renderWindow = sceneSubsystem->mainViewport;

					for (CameraComponent* camera : scene->cameras)
					{
						if (!camera->IsEnabled())
							continue;
						// TODO: Add support for multiple cameras in scene
						if (camera != scene->mainCamera)
							continue;

						if (renderWindow && renderWindow->GetCurrentNativeWindow())
						{
							CPlatformWindow* nativeWindow = renderWindow->GetCurrentNativeWindow();
							PlatformWindow* platformWindow = nativeWindow->GetPlatformWindow();

							for (CE::RenderPipeline* renderPipeline : scene->renderPipelines)
							{
								RPI::RenderPipeline* rpiPipeline = renderPipeline->GetRpiRenderPipeline();
								const auto& attachments = rpiPipeline->attachments;

								for (PassAttachment* passAttachment : attachments)
								{
									if (passAttachment->lifetime == AttachmentLifetimeType::External && passAttachment->name == "PipelineOutput")
									{
										passAttachment->attachmentId = String::Format("Window_{}", platformWindow->GetWindowId());
									}
									else
									{
										passAttachment->attachmentId = String::Format("{}_{}", passAttachment->name, rpiPipeline->uuid);
									}
								}

								rpiPipeline->ImportScopeProducers(scheduler);
							}
						}
						else if (renderWindow && renderWindow->IsViewportWindow())
						{
							CViewport* viewport = static_cast<CViewport*>(renderWindow);

							for (CE::RenderPipeline* renderPipeline : scene->renderPipelines)
							{
								RPI::RenderPipeline* rpiPipeline = renderPipeline->GetRpiRenderPipeline();
								const auto& attachments = rpiPipeline->attachments;

								for (PassAttachment* passAttachment : attachments)
								{
									if (passAttachment->lifetime == AttachmentLifetimeType::External && passAttachment->name == "PipelineOutput")
									{
										passAttachment->attachmentId = String::Format("Viewport_{}", viewport->GetUuid());

										StaticArray<RHI::Texture*, RHI::Limits::MaxSwapChainImageCount> frameBuffer{};
										CPlatformWindow* nativeWindow = viewport->GetNativeWindow();

										cApp->FrameGraphShaderDependency(nativeWindow, passAttachment->attachmentId);

										for (int i = 0; i < frameBuffer.GetSize(); ++i)
										{
											frameBuffer[i] = viewport->GetFrame(i)->GetRhiTexture();
										}

										attachmentDatabase.EmplaceFrameAttachment(passAttachment->attachmentId, frameBuffer);
									}
									else
									{
										passAttachment->attachmentId = String::Format("{}_{}", passAttachment->name, rpiPipeline->uuid);
									}
								}

								rpiPipeline->ImportScopeProducers(scheduler);
							}
						}
					}
				}

				cApp->FrameGraphEnd();
			}*/
		}
		scheduler->EndFrameGraph();
	}

	void RendererSubsystem::CompileFrameGraph()
	{
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
		SubClass<FeatureProcessor> fpClass)
	{
		if (componentClass == nullptr || fpClass == nullptr)
			return;

		componentClassToFeatureProcessorClass[componentClass] = fpClass;
	}

	SubClass<FeatureProcessor> RendererSubsystem::GetFeatureProcessClass(SubClass<ActorComponent> componentClass)
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

} // namespace CE
