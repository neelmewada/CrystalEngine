#include "EditorSystem.h"

namespace CE::Editor
{
	static ColorPickerTool* colorPicker = nullptr;

	ColorPickerTool* ColorPickerTool::Open()
	{
		if (!colorPicker)
		{
			// TODO
		}

		colorPicker->Show();

		return colorPicker;
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
