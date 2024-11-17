#pragma once

#include "CoreMinimal.h"

#include "../Bundle.inl"



namespace CE
{

	static void SaveDependencies(Stream* stream, const Array<Name>& dependencies)
	{
		u32 numDependencies = dependencies.GetSize();
		*stream << numDependencies;

		for (int i = 0; i < numDependencies; i++)
		{
			*stream << dependencies[i].GetString();
		}
	}

	BundleSaveResult Bundle::SaveBundleToDisk(const WeakRef<Bundle>& bundleRef, Object* asset, Stream* stream)
	{
		Ref<Bundle> bundle = bundleRef.Lock();
		if (!bundle)
		{
			return BundleSaveResult::InvalidBundle;
		}

		ZoneScoped;
		ZoneTextF(bundle->GetName().GetCString());

		if (bundle->IsTransient())
		{
			CE_LOG(Error, All, "Could not save a transient bundle: {}", bundle->GetBundleName());
			return BundleSaveResult::InvalidBundle;
		}
		if (stream == nullptr || !stream->CanWrite())
		{
			return BundleSaveResult::InvalidStream;
		}

		if (asset == nullptr)
			asset = bundle;
		
		HashMap<Uuid, Object*> objectsToSerialize{};
		objectsToSerialize.Add({ asset->GetUuid(), asset });
		asset->FetchObjectReferences(objectsToSerialize);
		Array<Name> bundleDependencies{};

		for (const auto& [objectUuid, objectInstance] : objectsToSerialize)
		{
			if (objectInstance == nullptr || objectUuid == 0)
				continue;
			if (objectInstance->IsTransient())
				continue;
			auto objectBundle = objectInstance->GetBundle();

			if (bundle != objectBundle && objectBundle != nullptr && !objectBundle->IsTransient())
			{
				bundleDependencies.Add(objectBundle->GetBundleName());
			}
		}

		stream->SetBinaryMode(true);

		*stream << BUNDLE_MAGIC_NUMBER;
		*stream << BUNDLE_VERSION_MAJOR;
		*stream << BUNDLE_VERSION_MINOR;

		u64 fileCrcPos = stream->GetCurrentPosition();
		*stream << (u32)0; // 0 CRC

		u64 dataStartOffsetValuePos = stream->GetCurrentPosition();
		*stream << (u64)0; // Data Start Offset

		*stream << bundle->GetBundleUuid();
		*stream << bundle->GetBundleName();

		// Other bundles that `this` bundle depends upon
		SaveDependencies(stream, bundleDependencies);

		u8 isCooked = 0;
		*stream << isCooked;

		// Data Start

		u64 curPos = stream->GetCurrentPosition();
		stream->Seek(dataStartOffsetValuePos);
		*stream << curPos;
		stream->Seek(curPos);

		// Object Entries
		for (const auto& [objectUuid, objectInstance] : objectsToSerialize)
		{
			if (objectInstance == nullptr || objectUuid == 0)
				continue;
			if (objectInstance->IsTransient())
				continue;

			if (bundle != objectInstance->GetBundle()) // Don't serialize objects outside of `this` bundle
			{
				continue;
			}
            
			*stream << BUNDLE_OBJECT_MAGIC_NUMBER; // .OBJECT.
			
			*stream << objectInstance->GetUuid();
			*stream << objectInstance->IsAsset();
			*stream << objectInstance->GetPathInBundle();
			*stream << objectInstance->GetClass()->GetTypeName();
			*stream << objectInstance->GetName();

			if (objectInstance->IsAsset())
			{
				*stream << ((Asset*)objectInstance)->GetSourceAssetRelativePath();
			}

			// Data start offset from start of file (excluding itself)
			*stream << stream->GetCurrentPosition() + sizeof(u64);

			auto classType = objectInstance->GetClass();

			BinarySerializer serializer = BinarySerializer(classType, objectInstance);
			serializer.Serialize(stream);

			u32 crc = 0;
			*stream << crc; // 0 crc for now. Not needed
		}

		*stream << (u64)0; // EOF

		return BundleSaveResult::Success;
	}

	static void LoadBundleDependencies(Stream* stream, Array<Name>& outDependencies)
	{
		u32 numDependencies = 0;
		*stream >> numDependencies;

		for (int i = 0; i < numDependencies; i++)
		{
			String bundleName = "";
			*stream >> bundleName;
			outDependencies.Add(bundleName);
		}
	}

	Ref<Bundle> Bundle::LoadBundleFromDisk(const Ref<Bundle>& outer, Stream* stream, const IO::Path& fullBundlePath, BundleLoadResult& outResult, LoadFlags loadFlags)
	{
		String pathStr = fullBundlePath.GetString();
		ZoneScoped;
		ZoneTextF(pathStr.GetCString());

		if (stream == nullptr)
		{
			return nullptr;
		}
		
		// Actual bundle name based on it's path
		String actualBundleName = "";

		if (IO::Path::IsSubDirectory(fullBundlePath, gProjectPath / "Game"))
		{
			auto relativePath = IO::Path::GetRelative(fullBundlePath, gProjectPath).RemoveExtension().GetString().Replace({'\\'}, '/');
			if (!relativePath.StartsWith("/"))
				relativePath = "/" + relativePath;

			actualBundleName = relativePath;
		}

		stream->SetBinaryMode(true);

		u64 magicNumber = 0;
		u32 majorVersion = 0; 
		u32 minorVersion = 0;
		*stream >> magicNumber;
        
		if (magicNumber != BUNDLE_MAGIC_NUMBER)
		{
			outResult = BundleLoadResult::InvalidBundle;
			return nullptr;
		}

		*stream >> majorVersion;

		if (majorVersion > BUNDLE_VERSION_MAJOR)
		{
			outResult = BundleLoadResult::UnsupportedBundleVersion;
			return nullptr;
		}

		*stream >> minorVersion;

		u32 fileCrc = 0;
		*stream >> fileCrc;
		u64 dataStartOffset;
		*stream >> dataStartOffset;

		Uuid bundleUuid = 0;
		*stream >> bundleUuid;
		String bundleName = "";
		*stream >> bundleName;

		if (!actualBundleName.IsEmpty() && actualBundleName != bundleName)
		{
			bundleName = actualBundleName;
		}

		Array<Name> bundleDependencies{};
		LoadBundleDependencies(stream, bundleDependencies);

		u8 isCooked = 0;
		*stream >> isCooked;

		Ref<Bundle> bundle = nullptr;

		{
			LockGuard<SharedMutex> lock{ Bundle::bundleRegistryMutex };

			if (loadedBundles.KeyExists(bundleName)) // Bundle is already loaded
			{
				bundle = loadedBundles[bundleName];
				bundle->bundleDependencies = bundleDependencies;
			}
			else if (loadedBundlesByUuid.KeyExists(bundleUuid)) // Bundle is already loaded
			{
				bundle = loadedBundlesByUuid[bundleUuid];
				bundle->bundleDependencies = bundleDependencies;
			}
			else
			{
				Internal::ObjectCreateParams params{ Bundle::Type() };
				params.name = bundleName;
				params.uuid = bundleUuid;
				params.templateObject = nullptr;
				params.objectFlags = OF_NoFlags;
				params.outer = outer;

				bundle = (Bundle*)Internal::CreateObjectInternal(params);
				bundle->fullBundlePath = fullBundlePath;
				bundle->bundleDependencies = bundleDependencies;
				bundle->isFullyLoaded = false;
				bundle->objectUuidToEntryMap.Clear();
			}

			loadedBundles[bundleName] = bundle;
			loadedBundlesByUuid[bundleUuid] = bundle;
			loadedBundleUuidToPath[bundleUuid] = bundleName;
		}

		bundle->isCooked = isCooked > 0 ? true : false;
		bundle->majorVersion = majorVersion;
		bundle->minorVersion = minorVersion;

		stream->Seek(dataStartOffset);

		u64 magicObjectNumber = 0;
		*stream >> magicObjectNumber;

		bundle->objectUuidToEntryMap.Clear();

		while (magicObjectNumber == BUNDLE_OBJECT_MAGIC_NUMBER)
		{
			u64 objectUuid = 0;
			*stream >> objectUuid;

			b8 isAsset = false;
			*stream >> isAsset;

			String pathInBundle{};
			*stream >> pathInBundle;

			String objectTypeName{};
			*stream >> objectTypeName;

			String objectName{};
			*stream >> objectName;

			String sourceAssetPath = "";
			if (isAsset)
			{
				*stream >> sourceAssetPath;
			}

			u64 dataStartOffset = 0;
			*stream >> dataStartOffset;

			stream->Seek(dataStartOffset);

			u8 typeByte = 0;
			*stream >> typeByte; // Should always be map type

			u64 dataSize = 0;
			*stream >> dataSize;
			stream->Seek(dataSize, SeekMode::Current);
            
            bundle->objectUuidToEntryMap[objectUuid] = ObjectEntryMetaData();
            
            ObjectEntryMetaData& objectEntry = bundle->objectUuidToEntryMap[objectUuid];
            objectEntry.instanceUuid = objectUuid;
            objectEntry.isAsset = isAsset;
            objectEntry.objectClassName = objectTypeName;
            objectEntry.pathInBundle = pathInBundle;
            objectEntry.offsetInFile = dataStartOffset;
            objectEntry.objectDataSize = dataSize;
			objectEntry.objectName = objectName;
			objectEntry.sourceAssetRelativePath = sourceAssetPath;

			u32 crc = 0;
			*stream >> crc; // Object data crc, unused

			*stream >> magicObjectNumber; // magic number is 0 if end of file
		}
        
        bundle->isFullyLoaded = false;
        bundle->isLoaded = true;
        
        if (loadFlags & LOAD_Full)
        {
            bundle->LoadFully();
        }

		outResult = BundleLoadResult::Success;
		return bundle;
	}

    void Bundle::LoadFully()
    {
		ZoneScoped;

        if (isFullyLoaded)
            return;
        if (!fullBundlePath.Exists() || fullBundlePath.IsDirectory())
            return;
        
        FileStream stream = FileStream(fullBundlePath, Stream::Permissions::ReadOnly);
        stream.SetBinaryMode(true);
        LoadFully(&stream);
    }

    void Bundle::LoadFully(Stream* stream)
    {
		ZoneScoped;

        if (stream == nullptr)
            return;

		int i = 0;
        
        for (auto& [uuid, objectEntry] : objectUuidToEntryMap)
        {
            LoadObjectFromEntry(stream, uuid);
			++i;
        }
        
		isFullyLoaded = true;
    }

    Ref<Object> Bundle::LoadObject(Uuid objectUuid)
    {
		ZoneScoped;

        if (objectUuid != 0 && loadedObjects.KeyExists(objectUuid))
            return loadedObjects[objectUuid];
        if (!fullBundlePath.Exists() || fullBundlePath.IsDirectory() || objectUuid == 0)
            return nullptr;
        
        FileStream fileStream = FileStream(fullBundlePath, Stream::Permissions::ReadOnly);
        fileStream.SetBinaryMode(true);
        return LoadObjectFromEntry(&fileStream, objectUuid);
    }

    Ref<Object> Bundle::LoadObject(const Name& objectClassName)
	{
		ZoneScoped;

		for (const auto& [uuid, entry] : objectUuidToEntryMap)
		{
			if (entry.objectClassName == objectClassName)
			{
				if (loadedObjects.KeyExists(uuid))
					return loadedObjects[uuid];

				FileStream fileStream = FileStream(fullBundlePath, Stream::Permissions::ReadOnly);
				fileStream.SetBinaryMode(true);
				return LoadObjectFromEntry(&fileStream, uuid);
			}
		}

		for (const auto& subobject : GetSubObjectMap())
		{
			if (subobject != nullptr && subobject->GetClass()->GetTypeName() == objectClassName)
				return subobject;
		}

		return nullptr;
	}

	Ref<Object> Bundle::LoadObjectFromEntry(Stream* stream, Uuid objectUuid)
    {
		ZoneScoped;
		
        if (loadedObjects.KeyExists(objectUuid))
            return loadedObjects[objectUuid];
        if (!objectUuidToEntryMap.KeyExists(objectUuid))
            return nullptr;
        ObjectEntryMetaData& entry = objectUuidToEntryMap[objectUuid];
        
        u64 dataOffset = entry.offsetInFile;
        u32 dataSize = entry.objectDataSize;
        stream->Seek(dataOffset);
        if (!entry.objectClassName.IsValid())
            return nullptr;
        
        ClassType* objectClass = ClassType::FindClass(entry.objectClassName);

        if (dataSize == 0 || objectClass == nullptr)
            return nullptr;

		Ref<Object> objectInstance = nullptr;

		if (objectUuid == this->GetBundleUuid() && objectClass->IsSubclassOf<Bundle>())
		{
			objectInstance = this;
		}
		else
		{
			Internal::ObjectCreateParams params{ objectClass };
			params.outer = nullptr;
			params.name = entry.objectName.GetString();
			params.templateObject = nullptr;
			params.objectFlags = OF_NoFlags;
			params.uuid = objectUuid;

			objectInstance = Internal::CreateObjectInternal(params);
		}

        loadedObjects[objectUuid] = objectInstance;
        
        entry.isLoaded = true;

		BinaryDeserializer deserializer{ objectClass, objectInstance };
		deserializer.Deserialize(stream);

		bool fullyLoaded = true;
		for (const auto& [uuid, objectEntry] : objectUuidToEntryMap)
		{
			if (!objectEntry.isLoaded)
			{
				fullyLoaded = false;
				break;
			}
		}

		if (fullyLoaded)
			this->isFullyLoaded = true;
        
        return objectInstance;
    }
    
} // namespace CE
