#include "CoreWidgets.h"

namespace CE::Widgets
{

	CStyleManager::CStyleManager()
	{
		globalStyleSheet = CreateDefaultSubobject<CSSStyleSheet>("GlobalStyleSheet");
	}

	CStyleManager::~CStyleManager()
	{
		// PreShutdown should be called before shutting down ImGui and before destroying CStyleManager
		PreShutdown();
	}

	void CStyleManager::PreShutdown()
	{

		for (auto& [_, image] : loadedImages)
		{
			RHI::gDynamicRHI->RemoveImGuiTexture(image.id);

			RHI::gDynamicRHI->DestroyTexture(image.texture);
		}
		loadedImages.Clear();

		if (imageSampler != nullptr)
		{
			RHI::gDynamicRHI->DestroySampler(imageSampler);
		}
		imageSampler = nullptr;

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

	Resource* CStyleManager::SearchResource(const String& path)
	{
		ResourceManager* manager = GetResourceManager();

		Resource* loadedResource = nullptr;

		for (int i = resourceSearchModules.GetSize() - 1; i >= 0 && loadedResource == nullptr; i--)
		{
			Name resPath = "/" + resourceSearchModules[i] + "/Resources" + (path.StartsWith("/") ? "" : "/") + path;
            
			if (loadedResources.KeyExists(resPath))
			{
				return loadedResources[resPath];
			}
		}

		for (int i = resourceSearchModules.GetSize() - 1; i >= 0 && loadedResource == nullptr; i--)
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

    CTexture CStyleManager::SearchImageResource(const String& path)
	{
		for (int i = resourceSearchModules.GetSize() - 1; i >= 0; i--)
		{
			Name resPath = "/" + resourceSearchModules[i] + "/Resources" + (path.StartsWith("/") ? "" : "/") + path;
			if (loadedImages.KeyExists(resPath) && loadedImages[resPath].id != nullptr)
			{
				return loadedImages[resPath];
			}
		}

		Resource* imageResource = SearchResource(path);
		if (imageResource == nullptr || !imageResource->IsValid())
			return {};

		CMImage image = CMImage::LoadFromMemory(imageResource->GetData(), imageResource->GetDataSize());
		if (!image.IsValid())
			return {};
        RHI::TextureDesc desc{};
        desc.width = image.GetWidth();
        desc.height = image.GetHeight();
        desc.depth = 1;
        desc.name = "";
		switch (image.GetFormat())
		{
		case CMImageFormat::R:
			desc.format = RHI::TextureFormat::R8G8B8_UNORM;
			break;
		case CMImageFormat::RG:
			desc.format = RHI::TextureFormat::R8G8B8A8_UNORM;// R16G16_UNORM;
			break;
		case CMImageFormat::RGB:
			desc.format = RHI::TextureFormat::R8G8B8_UNORM;
			break;
		case CMImageFormat::RGBA:
			desc.format = RHI::TextureFormat::R8G8B8A8_UNORM;
			break;
		default:
			return {};
		}
        desc.dimension = RHI::TextureDimension::Dim2D;
        desc.mipLevels = 1;
        desc.sampleCount = 1;
        desc.usageFlags = RHI::TextureUsageFlags::SampledImage;
        
        RHI::Texture* texture = RHI::gDynamicRHI->CreateTexture(desc);
        if (texture == nullptr)
        {
            image.Free();
			return {};
        }
        
        texture->UploadData(image.GetDataPtr(), image.GetDataSize());

		CTexture tex{};
		tex.size = Vec2i(image.GetWidth(), image.GetHeight());
		tex.numChannels = image.GetNumChannels();
        
        image.Free();
        
		if (imageSampler == nullptr)
		{
			RHI::SamplerDesc samplerDesc{};
			samplerDesc.addressModeU = RHI::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerDesc.addressModeV = RHI::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerDesc.addressModeW = RHI::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerDesc.enableAnisotropy = true;
			samplerDesc.maxAnisotropy = 8;
			samplerDesc.filterMode = RHI::SAMPLER_FILTER_LINEAR;

			imageSampler = RHI::gDynamicRHI->CreateSampler(samplerDesc);
			if (imageSampler == nullptr)
			{
				RHI::gDynamicRHI->DestroyTexture(texture);
				return {};
			}
		}
       
        
        CTextureID textureId = RHI::gDynamicRHI->AddImGuiTexture(texture, imageSampler);
        if (textureId == nullptr)
        {
            //RHI::gDynamicRHI->DestroySampler(sampler);
            RHI::gDynamicRHI->DestroyTexture(texture);
			return {};
        }
		
		tex.id = textureId;
		tex.texture = texture;
		//tex.textureSampler = sampler;
        
		loadedImages[imageResource->GetFullPath()] = tex;
		return tex;
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
