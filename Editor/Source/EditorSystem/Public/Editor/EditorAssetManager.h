#pragma once

namespace CE::Editor
{

	struct SourceAssetFileType
	{
		// extension with a '.' prefix
		String extension = "";
		ClassType* assetClass = nullptr;
	};

    CLASS()
	class EDITORSYSTEM_API EditorAssetManager : public AssetManager
	{
		CE_CLASS(EditorAssetManager, AssetManager)
	public:

		EditorAssetManager();
		virtual ~EditorAssetManager();

		void Tick(f32 deltaTime) override;

	protected:

	};

} // namespace CE::Editor

#include "EditorAssetManager.rtti.h"
