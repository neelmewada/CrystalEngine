#pragma once

namespace CE
{
	class FWindow;
	class FLayoutManager;
	class FWidget;
	class FusionRenderer;

	class FUSIONCORE_API FNativeWindow final
	{
	public:

		FNativeWindow();

	fusioncore_internal:

		
		FWindow* targetWindow = nullptr;
	};

} // namespace CE

