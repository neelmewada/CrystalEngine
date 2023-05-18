#pragma once

#include "Object.h"

#if PAL_TRAIT_BUILD_TESTS
class Package_WriteRead_Test;
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
        
        // Always prefer using paths than streams
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
        
        bool IsLoaded() const
        {
            return isLoaded;
        }
        
        bool IsFullyLoaded() const
        {
            return isFullyLoaded;
        }
        
        void LoadFully();
        void LoadFully(Stream* originalStream);

		// Returns true if this package contains the given object
		bool ContainsObject(Object* object);
        
    private:
		Name packageName{};
        
#if PAL_TRAIT_BUILD_TESTS
        friend class ::Package_WriteRead_Test;
#endif
        
		bool isLoaded = true;
        bool isFullyLoaded = true;
        
		// Loading Only Data
        
		struct ObjectEntryHeader
		{
			u64 offsetInFile = 0;
			UUID instanceUuid = 0;
			b8 isAsset = false;
			String pathInPackage{};
			Name objectClassName{};
			u32 objectDataSize = 0;
		};

		HashMap<UUID, ObjectEntryHeader> objectUuidToEntryMap{};
        
        IO::Path fullPackagePath;
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
