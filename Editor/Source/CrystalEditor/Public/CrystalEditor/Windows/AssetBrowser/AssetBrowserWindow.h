#pragma once

namespace CE::Editor
{

	CLASS()
	class CRYSTALEDITOR_API AssetBrowserWindow final : public MinorEditorWindow
	{
		CE_CLASS(AssetBrowserWindow, MinorEditorWindow)
	public:

		AssetBrowserWindow();

		virtual ~AssetBrowserWindow();

	private:

		void Construct() override;

	};

} // namespace CE::Editor

#include "AssetBrowserWindow.rtti.h"