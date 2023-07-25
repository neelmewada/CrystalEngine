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

	void CStyleManager::LoadGlobalStyleSheet(const Name& stylesheetPath)
	{
		globalStyleSheet->Clear();

		LoadStyleSheet(stylesheetPath, globalStyleSheet);
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

	Resource* CStyleManager::LoadResourceInternal(const String& path)
	{
		ResourceManager* manager = GetResourceManager();

		Resource* loadedResource = nullptr;

		for (int i = 0; i < resourceSearchModules.GetSize() && loadedResource == nullptr; i++)
		{
			Name resPath = "/" + resourceSearchModules[i] + "/Resources" + (path.StartsWith("/") ? "" : "/") + path;

			if (loadedResources.KeyExists(resPath))
			{
				return loadedResources[resPath];
			}
		}

		for (int i = 0; i < resourceSearchModules.GetSize() && loadedResource == nullptr; i++)
		{
			Name resPath = "/" + resourceSearchModules[i] + "/Resources" + (path.StartsWith("/") ? "" : "/") + path;
			loadedResource = manager->LoadResource(resPath, this);

			if (loadedResource != nullptr)
			{
				loadedResources[resPath] = loadedResource;
				break;
			}
		}

		return loadedResource;
	}

	CMImage CStyleManager::SearchImageResource(const String& path)
	{
		for (int i = 0; i < resourceSearchModules.GetSize(); i++)
		{
			Name resPath = "/" + resourceSearchModules[i] + "/Resources" + (path.StartsWith("/") ? "" : "/") + path;
			if (loadedImages.KeyExists(resPath) && loadedImages[resPath].IsValid())
			{
				return loadedImages[resPath];
			}
		}

		Resource* imageResource = LoadResourceInternal(path);
		if (imageResource == nullptr || !imageResource->IsValid())
			return CMImage();

		CMImage image = CMImage::LoadFromMemory(imageResource->GetData(), imageResource->GetDataSize(), 4);
		if (!image.IsValid())
			return CMImage();

		loadedImages[imageResource->GetFullPath()] = image;
		return image;
	}

	void CStyleManager::LoadStyleSheet(const Name& resourcePath, CStyleSheet* styleSheet)
	{
		Resource* resource = GetResourceManager()->LoadResource(resourcePath, this);
		if (resource == nullptr)
			return;

		String css = (const char*)resource->GetData();

		CSSParser::ParseStyleSheet((CSSStyleSheet*)styleSheet, css);

		resource->RequestDestroy();
	}

	String CStyleManager::LoadStyleSheet(const Name& resourcePath)
	{
		Resource* resource = GetResourceManager()->LoadResource(resourcePath, this);
		if (resource == nullptr)
			return nullptr;

		String text = (const char*)resource->GetData();
		resource->RequestDestroy();
		return text;
	}

} // namespace CE::Widgets
