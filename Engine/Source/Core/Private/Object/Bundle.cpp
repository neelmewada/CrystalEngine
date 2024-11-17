
#include "CoreMinimal.h"


namespace CE
{
	SharedMutex Bundle::bundleRegistryMutex{};

	HashMap<Name, Bundle*> Bundle::loadedBundles{};
	HashMap<Uuid, Bundle*> Bundle::loadedBundlesByUuid{};
	HashMap<Uuid, Name> Bundle::loadedBundleUuidToPath{};

	Array<IBundleResolver*> Bundle::bundleResolvers = {};

	Bundle::Bundle()
	{
		this->bundleName = name;
	}

	Bundle::~Bundle()
	{
		LockGuard<SharedMutex> lock{ bundleRegistryMutex };

        loadedBundles.Remove(bundleName);
		loadedBundlesByUuid.Remove(GetUuid());
		loadedBundleUuidToPath.Remove(GetUuid());
	}

	Ref<Bundle> Bundle::LoadBundleByUuid(Uuid bundleUuid, LoadFlags loadFlags)
	{
		LockGuard<SharedMutex> lock{ bundleRegistryMutex };

		if (loadedBundlesByUuid.KeyExists(bundleUuid))
			return loadedBundlesByUuid[bundleUuid];
		if (bundleResolvers.IsEmpty())
			return nullptr;

		for (int i = bundleResolvers.GetSize() - 1; i >= 0; i--)
		{
			Name bundlePath = bundleResolvers[i]->ResolveBundlePath(bundleUuid);
			if (!bundlePath.IsValid())
				continue;
			Ref<Bundle> bundle = Bundle::LoadBundleFromDisk(nullptr, bundlePath, loadFlags);
			if (bundle != nullptr)
				return bundle;
		}

		return nullptr;
	}

	IO::Path Bundle::GetBundlePath(const Name& bundleName)
	{
		if (!bundleName.IsValid())
			return {};
		String bundleNameStr = bundleName.GetString();

		if (bundleNameStr.StartsWith("/Engine/") || bundleNameStr == "/Engine") // Example: /Engine/Assets/Textures/Noise/Perlin/Perlin04
		{
			if (!gProjectPath.Exists() || !(gProjectPath / "Engine/Assets").Exists())
			{
				return PlatformDirectories::GetEngineRootDir() / (bundleNameStr.GetSubstring(1) + ".casset");
			}
			return gProjectPath / (bundleNameStr.GetSubstring(1) + ".casset");
		}
		else if (bundleNameStr.StartsWith("/Game/") || bundleNameStr == "/Game")
		{
			return gProjectPath / (bundleNameStr.GetSubstring(1) + ".casset");
		}
		else if (bundleNameStr.StartsWith("/Editor/") || bundleNameStr == "/Editor")
		{
			return PlatformDirectories::GetLaunchDir() / (bundleNameStr.GetSubstring(1) + ".casset");
		}
		else if (bundleNameStr.StartsWith("/Temp/") || bundleNameStr == "/Temp")
		{
			return gProjectPath / (bundleNameStr.GetSubstring(1) + ".temp");
		}

		return PlatformDirectories::GetAppRootDir() / (bundleNameStr.GetSubstring(1) + ".casset");
	}

	bool Bundle::DestroyLoadedBundle(const Name& bundleName)
	{
		LockGuard<SharedMutex> lock{ bundleRegistryMutex };

		if (!loadedBundles.KeyExists(bundleName))
			return false;
		loadedBundles[bundleName]->BeginDestroy(); // Destroying bundle automatically removes it from the HashMap.
		return true;
	}

	void Bundle::DestroyAllSubobjects()
	{
		for (int i = 0; i < GetSubObjectCount(); i++)
		{
			Object* subobject = GetSubobject(i);
			DetachSubobject(subobject);
			subobject->BeginDestroy();
		}

		loadedObjects.Clear();
		objectUuidToEntryMap.Clear();
	}

	void Bundle::DestroyAllBundles()
	{
		LockGuard<SharedMutex> lock{ bundleRegistryMutex };

		Array<Bundle*> bundlesToDestroy{};
		for (auto& [bundleName, bundle] : loadedBundles)
		{
			bundlesToDestroy.Add(bundle);
		}
		for (auto bundle : bundlesToDestroy)
		{
			bundle->BeginDestroy();
		}
		bundlesToDestroy.Clear();
		loadedBundles.Clear();
	}

	Ref<Bundle> Bundle::LoadBundleFromDisk(const Ref<Bundle>& outer, const Name& bundleName, LoadFlags loadFlags)
	{
		return LoadBundleFromDisk(outer, GetBundlePath(bundleName), loadFlags);
	}

	Ref<Bundle> Bundle::LoadBundleFromDisk(const Ref<Bundle>& bundle, const IO::Path& fullBundlePath, LoadFlags loadFlags)
	{
		BundleLoadResult result{};
		return LoadBundleFromDisk(bundle, fullBundlePath, result, loadFlags);
	}

	Ref<Bundle> Bundle::LoadBundleFromDisk(const Ref<Bundle>& bundle, const IO::Path& fullBundlePath, BundleLoadResult& outResult,
	                                       LoadFlags loadFlags)
	{
		auto path = fullBundlePath;
		if (path.GetExtension().IsEmpty()) // Add extension if one doesn't exist
		{
			path = path.GetString() + ".casset";
		}
		if (!path.Exists())
		{
			outResult = BundleLoadResult::BundleNotFound;
			//CE_LOG(Error, All, "Bundle not found at: {}", path);
			return nullptr;
		}
		if (path.IsDirectory())
		{
			outResult = BundleLoadResult::BundleNotFound;
			CE_LOG(Error, All, "Bundle path passed is a directory: {}", path);
			return nullptr;
		}
        
		FileStream stream = FileStream(path, Stream::Permissions::ReadOnly);
		stream.SetBinaryMode(true);
		return LoadBundleFromDisk(bundle, &stream, path, outResult, loadFlags);
	}

	BundleSaveResult Bundle::SaveBundleToDisk(const WeakRef<Bundle>& bundleRef, Object* asset)
	{
		if (Ref<Bundle> bundle = bundleRef.Lock())
		{
			if (bundle->IsTransient())
			{
				CE_LOG(Error, All, "Could not save a transient bundle: {}", bundle->GetBundleName());
				return BundleSaveResult::InvalidBundle;
			}

			IO::Path bundlePath = GetBundlePath(bundle->GetBundleName());
			if (bundlePath.GetExtension().IsEmpty())
			{
				bundlePath = bundlePath.GetString() + ".casset";
			}

			return SaveBundleToDisk(bundle, asset, bundlePath);
		}
		else
		{
			CE_LOG(Error, All, "SaveBundleToDisk() passed with NULL or destroyed bundle!");
			return BundleSaveResult::InvalidBundle;
		}
	}

	BundleSaveResult Bundle::SaveBundleToDisk(const WeakRef<Bundle>& bundleRef, Object* asset, const IO::Path& fullBundlePath)
	{
		auto path = fullBundlePath;
		if (path.GetExtension().IsEmpty())
		{
			path = path.GetString() + ".casset";
		}

		if (!path.GetParentPath().Exists())
		{
			IO::Path::CreateDirectories(path.GetParentPath());
		}

		if (Ref<Bundle> bundle = bundleRef.Lock())
		{
			if (bundle->IsTransient())
			{
				CE_LOG(Error, All, "SaveBundleToDisk() passed with a transient bundle named: {}", bundle->GetBundleName());
				return BundleSaveResult::InvalidBundle;
			}

			if (path.IsDirectory())
			{
				CE_LOG(Error, All, "SaveBundleToDisk() passed with a bundle path that is a directory!");
				return BundleSaveResult::InvalidPath;
			}
			if (asset != nullptr && asset->GetBundle() != bundle)
			{
				CE_LOG(Error, All, "SaveBundleToDisk() passed with an asset object that is not part of the bundle!");
				return BundleSaveResult::AssetNotInBundle;
			}

			bundle->fullBundlePath = path;

			FileStream stream = FileStream(path, Stream::Permissions::WriteOnly);

			return SaveBundleToDisk(bundle, asset, &stream);
		}
		else
		{
			CE_LOG(Error, All, "SaveBundleToDisk() passed with NULL bundle!");
			return BundleSaveResult::InvalidBundle;
		}
	}

	bool Bundle::ContainsObject(Object* object)
	{
		return IsObjectPresentInHierarchy(object);
	}

	Object* Bundle::ResolveObjectReference(Uuid objectUuid)
	{
		if (!loadedObjects.KeyExists(objectUuid))
			return nullptr;
		return loadedObjects[objectUuid];
	}

	void Bundle::OnSubobjectDetached(Object* subobject)
	{
		Super::OnSubobjectDetached(subobject);

		if (subobject->GetUuid() == primaryObjectUuid)
		{
			primaryObjectName = "";
			primaryObjectTypeName = "";
			primaryObjectUuid = 0;
		}
	}

	void Bundle::OnAfterDeserialize()
	{
		Super::OnAfterDeserialize();

		SetName(bundleName);
	}

	const Name& Bundle::GetPrimaryObjectName()
	{
		GetPrimaryObjectUuid();

		if (!primaryObjectName.IsValid())
		{
			const auto& map = GetSubObjectMap();

			if (isFullyLoaded && GetSubObjectCount() > 0)
			{
				for (const auto& objectRef : map)
				{
					Object* object = objectRef.Get();

					if (object == nullptr)
						continue;

					if (object->IsOfType<Asset>())
					{
						primaryObjectName = object->GetName();
						return primaryObjectName;
					}

					primaryObjectName = object->GetName();
				}
			}
			else
			{
				for (const auto& [uuid, objectEntry] : objectUuidToEntryMap)
				{
					String pathInBundle = objectEntry.pathInBundle;
					if (pathInBundle.Contains(".") || pathInBundle.IsEmpty()) // object is not a root object in this bundle
						continue;

					ClassType* objectClass = ClassType::FindClass(objectEntry.objectClassName);
					if (objectClass == nullptr)
						continue;

					if (objectClass->IsSubclassOf<Asset>())
					{
						primaryObjectName = objectEntry.objectName;
						return primaryObjectName;
					}

					primaryObjectName = objectEntry.objectName;
				}
			}
		}
		return primaryObjectName;
	}

	const Name& Bundle::GetPrimaryObjectTypeName()
	{
		GetPrimaryObjectUuid();

		if (!primaryObjectTypeName.IsValid())
		{
			const auto& map = GetSubObjectMap();

			if (isFullyLoaded && GetSubObjectCount() > 0)
			{
				for (const auto& objectRef : map)
				{
					Object* object = objectRef.Get();
					if (object == nullptr)
						continue;

					if (object->IsOfType<Asset>())
					{
						primaryObjectTypeName = object->GetClass()->GetTypeName();
						return primaryObjectTypeName;
					}

					primaryObjectTypeName = object->GetClass()->GetTypeName();
				}
			}
			else
			{
				for (const auto& [uuid, objectEntry] : objectUuidToEntryMap)
				{
					String pathInBundle = objectEntry.pathInBundle;
					if (pathInBundle.Contains(".") || pathInBundle.IsEmpty()) // object is not a root object in this bundle
						continue;

					ClassType* objectClass = ClassType::FindClass(objectEntry.objectClassName);
					if (objectClass == nullptr)
						continue;

					if (objectClass->IsSubclassOf<Asset>())
					{
						primaryObjectTypeName = objectClass->GetTypeName();
						return primaryObjectTypeName;
					}

					primaryObjectTypeName = objectClass->GetTypeName();
				}
			}
		}
		return primaryObjectTypeName;
	}

	Uuid Bundle::GetPrimaryObjectUuid()
	{
		if (primaryObjectUuid == 0)
		{
			const auto& map = GetSubObjectMap();

			if (isFullyLoaded && GetSubObjectCount() > 0)
			{
				for (const auto& objectRef : map)
				{
					Object* object = objectRef.Get();
					if (object == nullptr)
						continue;

					if (object->IsOfType<Asset>())
					{
						primaryObjectUuid = object->GetUuid();
						return primaryObjectUuid;
					}

					primaryObjectUuid = object->GetUuid();
				}
			}
			else
			{
				for (const auto& [uuid, objectEntry] : objectUuidToEntryMap)
				{
					String pathInBundle = objectEntry.pathInBundle;
					if (pathInBundle.Contains(".") || pathInBundle.IsEmpty()) // object is not a root object in this bundle
						continue;

					ClassType* objectClass = ClassType::FindClass(objectEntry.objectClassName);
					if (objectClass == nullptr)
						continue;

					if (objectClass->IsSubclassOf<Asset>())
					{
						primaryObjectUuid = objectEntry.instanceUuid;
						return primaryObjectUuid;
					}

					primaryObjectUuid = objectEntry.instanceUuid;
				}
			}
		}
		return primaryObjectUuid;
	}

	String Bundle::GetPrimarySourceAssetRelativePath()
	{
#if PAL_TRAIT_BUILD_EDITOR
		const auto& map = GetSubObjectMap();

		if (isFullyLoaded && GetSubObjectCount() > 0)
		{
			for (const auto& objectRef : map)
			{
				Object* object = objectRef.Get();

				if (object != nullptr && object->IsOfType<Asset>())
					return ((Asset*)object)->GetSourceAssetRelativePath();
			}
		}
		else
		{
			for (const auto& [uuid, objectEntry] : objectUuidToEntryMap)
			{
				ClassType* objectClass = ClassType::FindClass(objectEntry.objectClassName);
				if (objectClass != nullptr && objectClass->IsSubclassOf<Asset>())
				{
					return objectEntry.sourceAssetRelativePath;
				}
			}
		}
#endif

		return String();
	}

	void Bundle::SetObjectUuid(Object* object, Uuid newUuid)
	{
		if (object == nullptr)
			return;

		loadedObjectsMutex.Lock();

		Uuid oldUuid = object->GetUuid();

		if (GetPrimaryObjectUuid() == oldUuid)
		{
			primaryObjectUuid = 0;
		}

		if (objectUuidToEntryMap.KeyExists(oldUuid))
		{
			objectUuidToEntryMap[newUuid] = objectUuidToEntryMap[oldUuid];
			objectUuidToEntryMap.Remove(oldUuid);

			objectUuidToEntryMap[newUuid].instanceUuid = newUuid;
		}

		if (loadedObjects.KeyExists(oldUuid))
		{
			loadedObjects[newUuid] = loadedObjects[oldUuid];
			loadedObjects.Remove(oldUuid);
		}

		loadedObjectsMutex.Unlock();

		bundleRegistryMutex.Lock();

		if (object->IsBundle())
		{
			Bundle* bundle = (Bundle*)object;

			if (loadedBundlesByUuid.KeyExists(oldUuid))
			{
				loadedBundlesByUuid[newUuid] = loadedBundlesByUuid[oldUuid];
				loadedBundlesByUuid.Remove(oldUuid);
			}

			if (loadedBundleUuidToPath.KeyExists(oldUuid))
			{
				loadedBundleUuidToPath[newUuid] = loadedBundleUuidToPath[oldUuid];
				loadedBundleUuidToPath.Remove(oldUuid);
			}
		}

		bundleRegistryMutex.Unlock();

		object->uuid = newUuid;
	}

	void Bundle::OnObjectUnloaded(Object* object)
	{
		if (object == nullptr)
			return;
		LockGuard lock{ loadedObjectsMutex };
		loadedObjects.Remove(object->GetUuid());
	}

} // namespace CE

CE_RTTI_CLASS_IMPL(CORE_API, CE, Bundle)


