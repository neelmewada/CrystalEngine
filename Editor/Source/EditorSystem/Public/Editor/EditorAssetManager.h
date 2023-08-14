#pragma once

namespace CE::Editor
{
	class AssetImporter;

    CLASS()
	class EDITORSYSTEM_API EditorAssetManager : public AssetManager
	{
		CE_CLASS(EditorAssetManager, AssetManager)
	public:

		EditorAssetManager();
		virtual ~EditorAssetManager();

		static EditorAssetManager* Get();

		void Initialize() override;
		void Shutdown() override;

		void Tick(f32 deltaTime) override;

		void ImportSourceAssets();

	protected:

		bool ImportSourceAsset(IO::Path sourceAssetPath, AssetImporter* importer);

		Array<IO::Path> sourceAssetsToImport{};
		Array<Name> recentlyProcessedPackageNames{};

		Array<AssetImporter*> importersInProgress{};

		f32 waitToImportSourceAssets = 0;

		int numAssetsBeingImported = 0;

		Mutex mutex{};
	};

} // namespace CE::Editor

#include "EditorAssetManager.rtti.h"
