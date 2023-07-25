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
            RHI::gDynamicRHI->RemoveImGuiTexture(image);
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

    CTextureID CStyleManager::SearchImageResource(const String& path)
	{
		for (int i = 0; i < resourceSearchModules.GetSize(); i++)
		{
			Name resPath = "/" + resourceSearchModules[i] + "/Resources" + (path.StartsWith("/") ? "" : "/") + path;
			if (loadedImages.KeyExists(resPath) && loadedImages[resPath] != nullptr)
			{
				return loadedImages[resPath];
			}
		}

		Resource* imageResource = LoadResourceInternal(path);
		if (imageResource == nullptr || !imageResource->IsValid())
			return nullptr;

		CMImage image = CMImage::LoadFromMemory(imageResource->GetData(), imageResource->GetDataSize(), 4);
		if (!image.IsValid())
			return nullptr;
        RHI::TextureDesc desc{};
        desc.width = image.GetWidth();
        desc.height = image.GetHeight();
        desc.depth = 1;
        desc.name = "";
        if (image.GetNumChannels() == 1)
            desc.format = RHI::TextureFormat::R32_SFLOAT;
        else if (image.GetNumChannels() == 3)
            desc.format = RHI::TextureFormat::B8G8R8_SRGB;
        else if (image.GetNumChannels() == 4)
            desc.format = RHI::TextureFormat::R8G8B8_SRGB;
        desc.dimension = RHI::TextureDimension::Dim2D;
        desc.mipLevels = 1;
        desc.sampleCount = 1;
        desc.usageFlags = RHI::TextureUsageFlags::SampledImage;
        
        RHI::Texture* texture = RHI::gDynamicRHI->CreateTexture(desc);
        if (texture == nullptr)
        {
            image.Free();
            return nullptr;
        }
        
        texture->UploadData(image.GetDataPtr());
        
        image.Free();
        
        RHI::SamplerDesc samplerDesc{};
        samplerDesc.addressModeU = RHI::SAMPLER_ADDRESS_MODE_REPEAT;
        samplerDesc.addressModeV = RHI::SAMPLER_ADDRESS_MODE_REPEAT;
        samplerDesc.addressModeW = RHI::SAMPLER_ADDRESS_MODE_REPEAT;
        samplerDesc.enableAnisotropy = true;
        samplerDesc.maxAnisotropy = 8;
        samplerDesc.filterMode = RHI::SAMPLER_FILTER_LINEAR;
        
        RHI::Sampler* sampler = RHI::gDynamicRHI->CreateSampler(samplerDesc);
        if (sampler == nullptr)
        {
            RHI::gDynamicRHI->DestroyTexture(texture);
            return nullptr;
        }
        
        CTextureID textureId = RHI::gDynamicRHI->AddImGuiTexture(texture, sampler);
        if (textureId == nullptr)
        {
            RHI::gDynamicRHI->DestroySampler(sampler);
            RHI::gDynamicRHI->DestroyTexture(texture);
            return nullptr;
        }
        
		loadedImages[imageResource->GetFullPath()] = textureId;
		return textureId;
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
