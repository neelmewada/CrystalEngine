#include "CoreWidgets.h"

namespace CE::Widgets
{

	CStyleManager::CStyleManager()
	{
		globalStyleSheet = CreateObject<CSSStyleSheet>(this, "GlobalStyleSheet");
	}

	CStyleManager::~CStyleManager()
	{
		for (auto& [_, image] : loadedImages)
		{
			image.Free();
		}
		loadedImages.Clear();

		for (const auto& [_, resource] : loadedResources)
		{
			resource->RequestDestroy();
		}
		loadedResources.Clear();
	}

	void CStyleManager::SetGlobalStyleSheet(const String& stylesheet)
	{
		this->globalStyleSheetText = stylesheet;
		globalStyleSheet->Clear();

		CSSParser::ParseStyleSheet((CSSStyleSheet*)globalStyleSheet, globalStyleSheetText);
	}

	void CStyleManager::AddResourceSearchModule(const String& moduleName)
	{
		if (!resourceSearchModules.Exists(moduleName))
			resourceSearchModules.Add(moduleName);
	}

	void CStyleManager::RemoveResourceSearchModule(const String& moduleName)
	{
		resourceSearchModules.Remove(moduleName);
	}

	Resource* CStyleManager::LoadResource(const String& path)
	{
		ResourceManager* manager = GetResourceManager();

		Resource* loadedResource = nullptr;

		for (int i = 0; i < resourceSearchModules.GetSize() && loadedResource == nullptr; i++)
		{
			String resPath = "/" + resourceSearchModules[i] + "/Resources" + (path.StartsWith("/") ? "" : "/") + path;
			loadedResource = manager->LoadResource(resPath, this);
		}

		return loadedResource;
	}

	CMImage CStyleManager::LoadImageResource(const String& path)
	{
		for (int i = 0; i < resourceSearchModules.GetSize(); i++)
		{
			Name resPath = "/" + resourceSearchModules[i] + "/Resources" + (path.StartsWith("/") ? "" : "/") + path;
			if (loadedImages.KeyExists(resPath) && loadedImages[resPath].IsValid())
			{
				return loadedImages[resPath];
			}
		}

		Resource* imageResource = LoadResource(path);
		if (imageResource == nullptr || !imageResource->IsValid())
			return CMImage();

		CMImage image = CMImage::LoadFromMemory(imageResource->GetData(), imageResource->GetDataSize(), 4);
		loadedImages[imageResource->GetFullPath()] = image;
		return image;
	}

} // namespace CE::Widgets
