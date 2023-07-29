#pragma once

namespace CE::Editor
{

    CLASS()
	class EDITORSYSTEM_API EditorAssetManager : public AssetManager
	{
		CE_CLASS(EditorAssetManager, AssetManager)
	public:

		EditorAssetManager();
		virtual ~EditorAssetManager();


	protected:

	};

} // namespace CE::Editor

#include "EditorAssetManager.rtti.h"
