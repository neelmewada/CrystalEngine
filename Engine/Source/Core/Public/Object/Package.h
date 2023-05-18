#pragma once

#include "Object.h"

#if PAL_TRAIT_BUILD_TESTS
class Package_Writing_Test;
#endif

namespace CE
{
	class SavePackage;

	enum class LoadPackageResult
	{
		Success = 0,
		UnknownError,
		PackageNotFound,
		InvalidPackage,
	};

	enum class SavePackageResult
	{
		Success = 0,
		UnknownError,
		InvalidPath,
		InvalidPackage,
		AssetNotInPackage,
	};

	struct SavePackageArgs
	{

	};

	class CORE_API Package : public Object
	{
		CE_CLASS(Package, CE::Object)
	public:
		Package();
		virtual ~Package();

		// - Static API -

		static Package* LoadPackage(Package* outer, const IO::Path& fullPackagePath, LoadFlags loadFlags = LOAD_Default);
		static Package* LoadPackage(Package* outer, const IO::Path& fullPackagePath, LoadPackageResult& outResult, LoadFlags loadFlags = LOAD_Default);
		static Package* LoadPackage(Package* outer, Stream* inStream, LoadPackageResult& outResult, LoadFlags loadFlags = LOAD_Default);

		static SavePackageResult SavePackage(Package* outer, Object* asset, const IO::Path& fullPackagePath, const SavePackageArgs& saveArgs);
		static SavePackageResult SavePackage(Package* outer, Object* asset, Stream* outputStream, const SavePackageArgs& saveArgs);

		// - Public API -

		bool IsPackage() override { return true; }

		virtual void SetName(const String& newName) override
		{
			this->packageName = newName;
			Super::SetName(newName);
		}
        
        Name GetPackageName()
        {
            return packageName;
        }

		UUID GetPackageUuid()
		{
			return uuid;
		}

		void SetPackageName(const Name& name)
		{
			this->packageName = name;
		}
        
        //void AttachSubobject(Object* subobject) override;
        
        //void DetachSubobject(Object* subobject) override;

		// Returns true if this package contains the given object
		bool ContainsObject(Object* object);
        
    private:
		Name packageName{};
        
#if PAL_TRAIT_BUILD_TESTS
        friend class ::Package_Writing_Test;
#endif
        
		bool isLoaded = false;
        
		// Loading Only Data

		struct FieldEntryHeader
		{
			u32 offsetInFile = 0;
			String fieldName{};
			Name fieldTypeName{};
			u32 fieldDataSize = 0;
		};

		struct ObjectEntryHeader
		{
			u32 offsetInFile = 0;
			UUID instanceUuid = 0;
			b8 isAsset = false;
			String pathInPackage{};
			Name objectClassName{};
			u32 objectDataSize = 0;

			Array<FieldEntryHeader> fieldEntries{};
		};

		HashMap<UUID, ObjectEntryHeader> objectUuidToEntryMap{};
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
