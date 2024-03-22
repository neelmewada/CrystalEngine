#include "CrystalWidgets.h"

namespace CE::Widgets
{
	static CApplication* instance = nullptr;

	CApplication::CApplication()
	{
		if (IsDefaultInstance())
			return;
		
		CE_ASSERT(instance == nullptr, "Only 1 instance of CApplication is allowed");
		instance = this;
	}

	CApplication::~CApplication()
	{
		if (instance == this)
		{
			instance = nullptr;
		}
	}

	CApplication* CApplication::Get()
	{
		if (instance == nullptr)
		{
			instance = CreateObject<CApplication>(nullptr, "Application", OF_Transient);
		}
		return instance;
	}

	void CApplication::Initialize(const CApplicationInitInfo& initInfo)
	{
		draw2dShader = initInfo.draw2dShader;
		defaultFontName = initInfo.defaultFontName;
		defaultFont = initInfo.defaultFont;
		numFramesInFlight = initInfo.numFramesInFlight;
		scheduler = initInfo.scheduler;

		registeredFonts[defaultFontName] = initInfo.defaultFont;

		PlatformApplication::Get()->AddMessageHandler(this);
	}

	void CApplication::Shutdown()
	{
		PlatformApplication::Get()->RemoveMessageHandler(this);
	}

	void CApplication::Tick()
	{
		constexpr u32 destroyAfterFrames = 8;

		for (int i = destructionQueue.GetSize() - 1; i >= 0; --i)
		{
			destructionQueue[i]->destroyFrameCounter++;
			if (destructionQueue[i]->destroyFrameCounter > destroyAfterFrames)
			{
				destructionQueue[i]->Destroy();
				destructionQueue.RemoveAt(i);
			}
		}

		for (int i = 0; i < windows.GetSize(); i++)
		{
			windows[i]->Tick();
		}
	}

	CWindow* CApplication::CreateWindow(const String& name, const String& title, PlatformWindow* nativeWindow)
	{
		CWindow* window = CreateObject<CWindow>(this, name);
		window->SetPlatformWindow(nativeWindow);
		window->SetTitle(title);
		return window;
	}

	void CApplication::RegisterFont(Name fontName, RPI::FontAtlasAsset* fontAtlas)
	{
		registeredFonts[fontName] = fontAtlas;
		
		for (auto window : windows)
		{
			if (window->renderer != nullptr)
			{
				window->renderer->RegisterFont(fontName, fontAtlas);
			}
		}
	}

	void CApplication::SetGlobalStyleSheet(CStyleSheet* globalStyleSheet)
	{
		this->globalStyleSheet = globalStyleSheet;
	}

	void CApplication::LoadGlobalStyleSheet(const IO::Path& path)
	{
		globalStyleSheet = CSSStyleSheet::Load(path, this);
	}

	void CApplication::BuildFrameGraph()
	{
		for (int i = 0; i < windows.GetSize(); ++i)
		{
			PlatformWindow* platformWindow = windows[i]->nativeWindow;

			if (!platformWindow)
				continue;

			FrameAttachmentDatabase& attachmentDatabase = scheduler->GetAttachmentDatabase();

			attachmentDatabase.EmplaceFrameAttachment(platformWindow->GetTitle(), windows[i]->swapChain);

			if (!platformWindow->IsMinimized() && platformWindow->IsShown())
			{
				scheduler->BeginScope(platformWindow->GetTitle());
				{
					RHI::ImageScopeAttachmentDescriptor swapChainAttachment{};
					swapChainAttachment.attachmentId = platformWindow->GetTitle();
					swapChainAttachment.loadStoreAction.clearValue = Vec4(0, 0, 0, 1);
					swapChainAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Clear;
					swapChainAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;
					swapChainAttachment.multisampleState.sampleCount = 1;
					scheduler->UseAttachment(swapChainAttachment, RHI::ScopeAttachmentUsage::Color, RHI::ScopeAttachmentAccess::Write);

					scheduler->PresentSwapChain(windows[i]->swapChain);
				}
				scheduler->EndScope();
			}
		}
	}

	void CApplication::SetDrawListMasks(RHI::DrawListMask& outMask)
	{
		for (int i = 0; i < windows.GetSize(); ++i)
		{
			outMask.Set(windows[i]->GetDrawListTag());
		}
	}

	void CApplication::FlushDrawPackets(RHI::DrawListContext& drawList, u32 imageIndex)
	{
		for (int i = 0; i < windows.GetSize(); ++i)
		{
			if (!windows[i]->IsVisible() || !windows[i]->IsEnabled())
				continue;

			const auto& packets = windows[i]->FlushDrawPackets(imageIndex);

			for (RHI::DrawPacket* drawPacket : packets)
			{
				drawList.AddDrawPacket(drawPacket);
			}
		}
	}

	void CApplication::SubmitDrawPackets(RHI::DrawListContext& drawList)
	{
		for (int i = 0; i < windows.GetSize(); ++i)
		{
			PlatformWindow* platformWindow = windows[i]->nativeWindow;

			if (!platformWindow)
				continue;

			scheduler->SetScopeDrawList(platformWindow->GetTitle(), &drawList.GetDrawListForTag(windows[i]->GetDrawListTag()));
		}
	}

	void CApplication::OnWindowDestroyed(PlatformWindow* nativeWindow)
	{
		for (int i = windows.GetSize() - 1; i >= 0; --i)
		{
			if (windows[i]->nativeWindow == nativeWindow)
			{
				windows[i]->Destroy();
				windows.RemoveAt(i);
			}
		}
	}

	void CApplication::OnWindowCreated(PlatformWindow* window)
	{

	}

	void CApplication::OnWindowResized(PlatformWindow* nativeWindow, u32 newWidth, u32 newHeight)
	{
		for (CWindow* window : windows)
		{
			if (window->nativeWindow == nativeWindow)
			{
				window->SetNeedsStyle();
				window->SetNeedsLayout();
				window->SetNeedsPaint();
			}
		}
	}

	void CApplication::OnSubobjectDetached(Object* object)
	{
		Super::OnSubobjectDetached(object);

		if (!object)
			return;

		if (object->IsOfType<CWindow>())
		{
			windows.Remove((CWindow*)object);
		}
	}

	void CApplication::OnSubobjectAttached(Object* object)
	{
		Super::OnSubobjectAttached(object);

		if (!object)
			return;

		if (object->IsOfType<CWindow>())
		{
			windows.Add((CWindow*)object);
		}
	}

} // namespace CE::Widgets

