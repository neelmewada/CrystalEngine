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

		void Tick(f32 deltaTime) override;

	protected:

		HashMap<Name, ClassType*> registeredSourceExtensions{};
	};

} // namespace CE::Editor

#include "EditorAssetManager.rtti.h"
