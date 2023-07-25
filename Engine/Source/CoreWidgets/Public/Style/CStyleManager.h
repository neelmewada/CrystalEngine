#pragma once

namespace CE::Widgets
{
    CLASS()
    class COREWIDGETS_API CStyleManager : public Object
    {
        CE_CLASS(CStyleManager, Object)
    public:
        
        CStyleManager();
        virtual ~CStyleManager();

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

		CMImage LoadImageResource(const String& resourceSearchPath);

		void LoadStyleSheet(const Name& resourcePath, CStyleSheet* styleSheet);
        
    private:

		Resource* LoadResourceInternal(const String& path);
        
		HashMap<Name, CMImage> loadedImages{};
		HashMap<Name, Resource*> loadedResources{};
		Array<String> resourceSearchModules{};
		
		CStyleSheet* globalStyleSheet = nullptr;
		String globalStyleSheetText = "";

		friend class CWidget;
    };
    
} // namespace CE::Widgets


#include "CStyleManager.rtti.h"
