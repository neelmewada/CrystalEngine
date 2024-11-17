#pragma once

#include "Object.h"

#if PAL_TRAIT_BUILD_TESTS
class Bundle_WriteRead_Test;
#endif

namespace CE
{
	class Bundle;

	enum class BundleLoadResult
	{
		Success = 0,
		UnknownError,
		BundleNotFound,
		InvalidBundle,
		UnsupportedBundleVersion,
	};

	enum class BundleSaveResult
	{
		Success = 0,
		UnknownError,
		InvalidStream,
		InvalidPath,
		InvalidBundle,
		AssetNotInBundle,
	};

	struct IBundleResolver
	{
		virtual ~IBundleResolver() = default;

		virtual Name ResolveBundlePath(Uuid bundleUuid) = 0;
	};

	
	/**
	 * @brief A bundle is a collection of objects that can serialized to disk.
	 */
	class CORE_API Bundle : public Object
	{
		CE_CLASS(Bundle, CE::Object)
	public:
		Bundle();
		virtual ~Bundle();

		// - Static API -

		inline static void PushBundleResolver(IBundleResolver* resolver) { Self::bundleResolvers.Push(resolver); }
		inline static void PopBundleResolver() { if (!Self::bundleResolvers.IsEmpty()) Self::bundleResolvers.Pop(); }

		inline static IBundleResolver* GetBundleResolver() 
		{ 
			if (!Self::bundleResolvers.IsEmpty()) 
				return bundleResolvers.Top();
			return nullptr;
		}

		static Ref<Bundle> LoadBundleByUuid(Uuid bundleUuid, LoadFlags loadFlags = LOAD_Default);

		static IO::Path GetBundlePath(const Name& bundleName);

		static bool DestroyLoadedBundle(const Name& bundleName);
		static void DestroyAllBundles();

		static Ref<Bundle> LoadBundleFromDisk(const Ref<Bundle>& outer, const Name& bundleName, LoadFlags loadFlags = LOAD_Default);
		static Ref<Bundle> LoadBundleFromDisk(const Ref<Bundle>& outer, const IO::Path& fullBundleFilePath,
		                                      LoadFlags loadFlags = LOAD_Default);
		static Ref<Bundle> LoadBundleFromDisk(const Ref<Bundle>& outer, const IO::Path& fullBundleFilePath,
		                                      BundleLoadResult& outResult, LoadFlags loadFlags = LOAD_Default);
        
        // Always prefer using paths than streams
		static Ref<Bundle> LoadBundleFromDisk(const Ref<Bundle>& outer, Stream* inStream, const IO::Path& fullBundlePath, BundleLoadResult& outResult, LoadFlags loadFlags = LOAD_Default);

		static BundleSaveResult SaveBundleToDisk(const WeakRef<Bundle>& bundle, Object* asset);
		static BundleSaveResult SaveBundleToDisk(const WeakRef<Bundle>& bundle, Object* asset, const IO::Path& fullBundleFilePath);
		static BundleSaveResult SaveBundleToDisk(const WeakRef<Bundle>& bundle, Object* asset, Stream* outputStream);

		// - Public API -

		bool IsBundle() const override { return true; }

		virtual void SetName(const Name& newName) override
		{
			this->bundleName = newName;
			Super::SetName(newName);
		}
        
		inline Name GetBundleName() { return bundleName; }

		inline Uuid GetBundleUuid(){ return uuid; }

		inline void SetBundleName(const Name& name) { this->bundleName = name; }
        
        inline bool IsLoaded() const { return isLoaded; }
        
		inline bool IsFullyLoaded() const { return isFullyLoaded; }
        
        void LoadFully();
        void LoadFully(Stream* originalStream);

		Ref<Object> LoadObject(Uuid objectUuid);
		Ref<Object> LoadObject(const Name& objectClassName);

		void DestroyAllSubobjects();

		template<typename TObject> requires TIsBaseClassOf<CE::Object, TObject>::Value
		TObject* LoadObject()
		{
			return (TObject*)LoadObject(TYPENAME(TObject));
		}

		// Returns true if this bundle contains the given object. Note that the object has to be fully loaded.
		bool ContainsObject(Object* object);

		// Find and return an already loaded object with the given Uuid
		Object* ResolveObjectReference(Uuid objectUuid);

		const Name& GetPrimaryObjectName();
		const Name& GetPrimaryObjectTypeName();
		Uuid GetPrimaryObjectUuid();
		String GetPrimarySourceAssetRelativePath();

		void SetObjectUuid(Object* object, Uuid newUuid);
        
    private:

		void OnSubobjectDetached(Object* subobject) override;

		void OnAfterDeserialize() override;

		Ref<Object> LoadObjectFromEntry(Stream* originalStream, Uuid objectUuid);

		// Internal use only! Marks the passed object as 'unloaded'
		void OnObjectUnloaded(Object* object);
        
		friend class Object;
		friend class AssetRegistry;
        
#if PAL_TRAIT_BUILD_TESTS
        friend class ::Bundle_WriteRead_Test;
#endif

		Name bundleName{};

		b8 isCooked = false;
        
		bool isLoaded = true;
        bool isFullyLoaded = true;

		u32 majorVersion = 0;
		u32 minorVersion = 0;

		// Name of first object in bundle
		Name primaryObjectName{};
		// Long type name of first object in bundle
		Name primaryObjectTypeName = Name();
		// Primary object uuid
		Uuid primaryObjectUuid = 0;

		Array<Name> bundleDependencies{};
        
		// Loading Only Data
        
		struct ObjectEntryMetaData
		{
			u64 offsetInFile = 0;
			Uuid instanceUuid = 0;
			b8 isAsset = false;
			String pathInBundle{};
			Name objectClassName{};
			Name objectName{};
			u32 objectDataSize = 0;
			String sourceAssetRelativePath{};
            
            b8 isLoaded = false;
		};

		SharedMutex loadedObjectsMutex{};
		HashMap<Uuid, ObjectEntryMetaData> objectUuidToEntryMap{};
		HashMap<Uuid, Object*> loadedObjects{};
        
		IO::Path fullBundlePath{};

		static SharedMutex bundleRegistryMutex;
		static HashMap<Name, Bundle*> loadedBundles;
		static HashMap<Uuid, Bundle*> loadedBundlesByUuid;
		static HashMap<Uuid, Name> loadedBundleUuidToPath;

		static Array<IBundleResolver*> bundleResolvers;

		friend class CoreModule;
	};

} // namespace CE


