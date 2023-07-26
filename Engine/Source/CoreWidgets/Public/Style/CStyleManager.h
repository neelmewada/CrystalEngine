#pragma once

namespace CE::Widgets
{
    typedef void* CTextureID;

	struct CTexture
	{
		Vec2i size{};
		u32 numChannels = 4;
		CTextureID id = nullptr;

		inline bool IsValid() const
		{
			return id != nullptr && numChannels > 0 && size.x > 0 && size.y > 0;
		}

		RHI::Texture* texture = nullptr;
		RHI::Sampler* textureSampler = nullptr;
	};

    CLASS()
    class COREWIDGETS_API CStyleManager : public Object
    {
        CE_CLASS(CStyleManager, Object)
    public:
        
        CStyleManager();
        virtual ~CStyleManager();

		void PreShutdown();

		inline CStyleSheet* GetGlobalStyleSheet() const
		{
			return globalStyleSheet;
		}

		inline const String& GetGlobalStyleSheetText() const
		{
			return globalStyleSheetText;
		}

		void SetGlobalStyleSheet(const String& stylesheet);
		void LoadGlobalStyleSheet(const Name& stylesheetResourcePath);

		void AddResourceSearchModule(const String& moduleName);
		void RemoveResourceSearchModule(const String& moduleName);

		/// Search for resource in the search module paths
		Resource* SearchResource(const String& resourceSearchPath);

		/// Searches for image in search modules
		CTexture SearchImageResource(const String& resourceSearchPath);

		void LoadStyleSheet(const Name& resourcePath, CStyleSheet* styleSheet);
		String LoadStyleSheet(const Name& resourcePath);
        
    private:
        
		HashMap<Name, CTexture> loadedImages{};
		HashMap<Name, Resource*> loadedResources{};
		Array<String> resourceSearchModules{};
		
		CStyleSheet* globalStyleSheet = nullptr;
		String globalStyleSheetText = "";

		RHI::Sampler* imageSampler = nullptr;

		friend class CWidget;
    };
    
} // namespace CE::Widgets

namespace CE
{
	template<>
	FORCE_INLINE SIZE_T GetHash<Widgets::CTexture>(const CTexture& value)
	{
		return (SIZE_T)value.id;
	}
}

#include "CStyleManager.rtti.h"
