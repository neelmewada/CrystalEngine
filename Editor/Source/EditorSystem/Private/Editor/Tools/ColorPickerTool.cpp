#include "EditorSystem.h"

namespace CE::Editor
{
	static ColorPickerTool* colorPicker = nullptr;

	ColorPickerTool* ColorPickerTool::Open()
	{
		if (!colorPicker)
		{
			PlatformWindowInfo windowInfo{};
			windowInfo.fullscreen = windowInfo.resizable = windowInfo.maximised = false;
			windowInfo.hidden = true;
			windowInfo.windowFlags = PlatformWindowFlags::DestroyOnClose | PlatformWindowFlags::Utility;

			PlatformWindow* nativeWindow = PlatformApplication::Get()->CreatePlatformWindow("Color Picker", 450, 550, windowInfo);
			nativeWindow->SetAlwaysOnTop(true);
			nativeWindow->SetBorderless(true);

			colorPicker = CreateWindow<ColorPickerTool>("ColorPicker", nativeWindow);
		}

		colorPicker->Show();

		return colorPicker;
	}

	void ColorPickerTool::EnableAlpha(bool enable)
	{
		this->enableAlpha = enable;
	}

	void ColorPickerTool::SetOriginalColor(const Color& original)
	{
		this->original = original;
	}

	void ColorPickerTool::SetColor(const Color& color)
	{
		this->value = color;
	}

	ColorPickerTool::ColorPickerTool()
	{
		title = "Color Picker";

		canBeClosed = true;
		canBeMaximized = canBeMinimized = false;
	}

	ColorPickerTool::~ColorPickerTool()
	{
		if (colorPicker == this)
		{
			colorPicker = nullptr;
		}
	}

	void ColorPickerTool::Construct()
	{
		Super::Construct();

	}

} // namespace CE::Editor
