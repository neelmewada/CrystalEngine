#pragma once

#include "Object.h"

#if PAL_TRAIT_BUILD_TESTS
class Package_WriteRead_Test;
#endif

namespace CE
{
	class SavePackage;
	class Package;

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

	struct IPackageResolver
	{
		virtual Name GetPackagePath(Uuid packageUuid) = 0;
	};

	class CORE_API Package : public Object
	{
		CE_CLASS(Package, CE::Object)
	public:
		Package();
		virtual ~Package();

		// - Static API -

		inline static void PushPackageResolver(IPackageResolver* resolver) { Self::packageResolvers.Push(resolver); }
		inline static void PopPackageResolver() { if (!Self::packageResolvers.IsEmpty()) Self::packageResolvers.Pop(); }

		inline static IPackageResolver* GetPackageResolver() 
		{ 
			if (!Self::packageResolvers.IsEmpty()) 
				return packageResolvers.Top();
			return nullptr;
		}

		static Package* LoadPackageByUuid(Uuid packageUuid, LoadFlags loadFlags = LOAD_Default);

		static IO::Path GetPackagePath(const Name& packageName);

		static bool DestroyLoadedPackage(const Name& packageName);
		static void DestroyAllPackages();

		static Package* LoadPackage(Package* outer, const Name& packageName, LoadFlags loadFlags = LOAD_Default);
		static Package* LoadPackage(Package* outer, const IO::Path& fullPackageFilePath, LoadFlags loadFlags = LOAD_Default);
		static Package* LoadPackage(Package* outer, const IO::Path& fullPackageFilePath, LoadPackageResult& outResult, LoadFlags loadFlags = LOAD_Default);
        
        // Always prefer using paths than streams
		static Package* LoadPackage(Package* outer, Stream* inStream, IO::Path fullPackagePath, LoadPackageResult& outResult, LoadFlags loadFlags = LOAD_Default);

		static SavePackageResult SavePackage(Package* package, Object* asset, const SavePackageArgs& saveArgs = {});
		static SavePackageResult SavePackage(Package* package, Object* asset, const IO::Path& fullPackageFilePath, const SavePackageArgs& saveArgs = {});
		static SavePackageResult SavePackage(Package* package, Object* asset, Stream* outputStream, const SavePackageArgs& saveArgs = {});

		static SavePackageResult SavePackage2(Package* package, Object* asset, Stream* outputStream, const SavePackageArgs& saveArgs = {});

		// - Public API -

		bool IsPackage() const override { return true; }

		virtual void SetName(const Name& newName) override
		{
			this->packageName = newName;
			Super::SetName(newName);
		}
        
		inline Name GetPackageName() { return packageName; }

		inline Uuid GetPackageUuid(){ return uuid; }

		inline void SetPackageName(const Name& name) { this->packageName = name; }
        
        inline bool IsLoaded() const { return isLoaded; }
        
		inline bool IsFullyLoaded() const { return isFullyLoaded; }
        
        void LoadFully();
        void LoadFully(Stream* originalStream);
        
        Object* LoadObject(Uuid objectUuid);
		Object* LoadObject(const Name& objectClassName);

		void DestroyAllSubobjects();

		template<typename TObject> requires TIsBaseClassOf<CE::Object, TObject>::Value
		TObject* LoadObject()
		{
			return (TObject*)LoadObject(TYPENAME(TObject));
		}

		// Returns true if this package contains the given object. Note that the object has to be fully loaded.
		bool ContainsObject(Object* object);

		// Find and return an already loaded object with the given Uuid
		Object* ResolveObjectReference(Uuid objectUuid);

		const Name& GetPrimaryObjectName();
		const Name& GetPrimaryObjectTypeName();
		Uuid GetPrimaryObjectUuid();
		String GetPrimarySourceAssetRelativePath();
        
    private:

		void OnSubobjectDetached(Object* subobject) override;

		void OnAfterDeserialize() override;
        
        Object* LoadObjectFromEntry(Stream* originalStream, Uuid objectUuid);

		// Internal use only! Marks the passed object as 'unloaded'
		void OnObjectUnloaded(Object* object);
        
		friend class Object;
		friend class AssetRegistry;
        
#if PAL_TRAIT_BUILD_TESTS
        friend class ::Package_WriteRead_Test;
#endif

		Name packageName{};

		b8 isCooked = false;
        
		bool isLoaded = true;
        bool isFullyLoaded = true;

		u32 majorVersion = 0;
		u32 minorVersion = 0;

		// Name of first object in package
		Name primaryObjectName{};
		// Long type name of first object in package
		Name primaryObjectTypeName = Name();
		// Primary object uuid
		Uuid primaryObjectUuid = 0;

		Array<Name> packageDependencies{};
        
		// Loading Only Data
        
		struct ObjectEntryMetaData
		{
			u64 offsetInFile = 0;
			Uuid instanceUuid = 0;
			b8 isAsset = false;
			String pathInPackage{};
			Name objectClassName{};
			Name objectName{};
			u32 objectDataSize = 0;
			String sourceAssetRelativePath{};
            
            b8 isLoaded = false;
		};

		SharedMutex loadedObjectsMutex{};
		HashMap<Uuid, ObjectEntryMetaData> objectUuidToEntryMap{};
		HashMap<Uuid, Object*> loadedObjects{};
        
		IO::Path fullPackagePath{};

		static SharedMutex packageRegistryMutex;
		static HashMap<Name, Package*> loadedPackages;
		static HashMap<Uuid, Package*> loadedPackagesByUuid;
		static HashMap<Uuid, Name> loadedPackageUuidToPath;

		static Array<IPackageResolver*> packageResolvers;

		friend class CoreModule;
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
