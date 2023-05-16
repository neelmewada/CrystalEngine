#pragma once

#include "Object.h"
#include "Package/SavePackage.h"


namespace CE
{
	class SavePackage;

	class CORE_API Package : public Object
	{
		CE_CLASS(Package, CE::Object)
	public:
		Package();
		virtual ~Package();

		// - Static API -

		static Package* LoadPackage(const IO::Path& fullPackagePath, LoadFlags loadFlags = LOAD_Default);
		static Package* LoadPackage(const IO::Path& fullPackagePath, LoadPackageResult& outResult, LoadFlags loadFlags = LOAD_Default);

        
    private:
		bool isLoaded = false;

		// Internal Data

		struct FieldEntryHeader
		{
			u32 offset = 0;
			String fieldName;
			Name fieldTypeName;
			u32 fieldDataSize;
		};

		struct ObjectEntryHeader
		{
			u32 offset = 0;
			UUID instanceUuid = 0;
			b8 isAsset = false;
			IO::Path virtualAssetPath{};
			Name objectClassName{};

			Array<FieldEntryHeader> fieldEntries{};
		};

		Array<ObjectEntryHeader> objectEntries{};
	};

} // namespace CE


CE_RTTI_CLASS(CORE_API, CE, Package,
	CE_SUPER(CE::Object),
	CE_NOT_ABSTRACT,
	CE_ATTRIBS(),
	CE_FIELD_LIST(

    ),
	CE_FUNCTION_LIST()
)
