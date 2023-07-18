#pragma once

namespace CE::Editor
{
	struct AssetDatabaseEntry
	{

		enum Type
		{
			None = 0,
			Directory,
			Asset
		};

		~AssetDatabaseEntry()
		{
			for (auto child : children)
			{
				delete child;
			}
			children.Clear();
		}

		String name = "";
		IO::Path relativePath{};
		Type type = None;
		

		Array<AssetDatabaseEntry*> children{};
		AssetDatabaseEntry* parent = nullptr;
	};

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
