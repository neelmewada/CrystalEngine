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

		registeredFonts[defaultFontName] = initInfo.defaultFont;

		PlatformApplication::Get()->AddMessageHandler(this);
	}

	void CApplication::Shutdown()
	{
		PlatformApplication::Get()->RemoveMessageHandler(this);
	}

	void CApplication::Tick()
	{
		// TODO: Update and render all windows

		for (CWindow* window : windows)
		{
			window->Tick();
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

	void CApplication::OnWindowResized(PlatformWindow* nativeWindow, u32 newWidth, u32 newHeight)
	{
		for (CWindow* window : windows)
		{
			if (window->nativeWindow == nativeWindow)
			{
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

