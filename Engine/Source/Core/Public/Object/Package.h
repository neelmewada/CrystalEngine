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
		UnsupportedPackageVersion,
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

	typedef Name PackagePath;

	class CORE_API Package : public Object
	{
		CE_CLASS(Package, CE::Object)
	public:
		Package();
		virtual ~Package();

		// - Static API -

		static IO::Path GetPackagePath(const PackagePath& packageName);

		static Package* LoadPackage(Package* outer, const PackagePath& packageName, LoadFlags loadFlags = LOAD_Default);
		static Package* LoadPackage(Package* outer, const IO::Path& fullPackageFilePath, LoadFlags loadFlags = LOAD_Default);
		static Package* LoadPackage(Package* outer, const IO::Path& fullPackageFilePath, LoadPackageResult& outResult, LoadFlags loadFlags = LOAD_Default);
        
        // Always prefer using paths than streams
		static Package* LoadPackage(Package* outer, Stream* inStream, IO::Path fullPackagePath, LoadPackageResult& outResult, LoadFlags loadFlags = LOAD_Default);

		static SavePackageResult SavePackage(Package* package, Object* asset, const SavePackageArgs& saveArgs = {});
		static SavePackageResult SavePackage(Package* package, Object* asset, const IO::Path& fullPackageFilePath, const SavePackageArgs& saveArgs = {});
		static SavePackageResult SavePackage(Package* package, Object* asset, Stream* outputStream, const SavePackageArgs& saveArgs = {});

		// - Public API -

		bool IsPackage() override { return true; }

		virtual void SetName(const Name& newName) override
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
        
        Object* LoadObject(UUID objectUuid);
		Object* LoadObject(const Name& objectClassName);

		// Returns true if this package contains the given object. Note that the object has to be fully loaded.
		bool ContainsObject(Object* object);

		Object* ResolveObjectReference(UUID objectUuid);
        
    private:
        
        Object* LoadObjectFromEntry(Stream* originalStream, UUID objectUuid);

		// Internal use only! Marks the passed object as 'unloaded'
		void OnObjectUnloaded(Object* object);
        
		friend class Object;
		friend class AssetRegistry;
        
#if PAL_TRAIT_BUILD_TESTS
        friend class ::Package_WriteRead_Test;
#endif

		Name packageName{};
        
		bool isLoaded = true;
        bool isFullyLoaded = true;

		Array<Name> packageDependencies{};
        
		// Loading Only Data
        
		struct ObjectEntryMetaData
		{
			u64 offsetInFile = 0;
			UUID instanceUuid = 0;
			b8 isAsset = false;
			String pathInPackage{};
			Name objectClassName{};
			Name objectName{};
			u32 objectDataSize = 0;
			String sourceAssetRelativePath{};
            
            b8 isLoaded = false;
		};

		HashMap<UUID, ObjectEntryMetaData> objectUuidToEntryMap{};

		HashMap<UUID, Object*> loadedObjects{};
        
		IO::Path fullPackagePath{};

		static HashMap<Name, Package*> loadedPackages;
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
