#pragma once

#include "CoreMinimal.h"

#include "../Package.inl"



namespace CE
{

	static void SavePackageDependencies(Stream* stream, const Array<Name>& dependencies)
	{
		u32 numDependencies = dependencies.GetSize();
		*stream << numDependencies;

		for (int i = 0; i < numDependencies; i++)
		{
			*stream << dependencies[i].GetString();
		}
	}

	SavePackageResult Package::SavePackage(Package* package, Object* asset, Stream* stream, const SavePackageArgs& saveArgs)
	{
		if (package == nullptr)
		{
			return SavePackageResult::InvalidPackage;
		}
		if (package->IsTransient())
		{
			CE_LOG(Error, All, "Could not save a transient package: {}", package->GetPackageName());
			return SavePackageResult::InvalidPackage;
		}
		if (stream == nullptr)
		{
			return SavePackageResult::UnknownError;
		}
		if (asset == nullptr)
			asset = package;
		
		HashMap<UUID, Object*> objectsToSerialize{};
		objectsToSerialize.Add({ asset->GetUuid(), asset });
		asset->FetchObjectReferences(objectsToSerialize);
		Array<Name> packageDependencies{};

		for (const auto& [objectUuid, objectInstance] : objectsToSerialize)
		{
			if (objectInstance == nullptr || objectUuid == 0)
				continue;
			if (objectInstance->IsTransient())
				continue;
			auto objectPackage = objectInstance->GetPackage();

			if (package != objectPackage && objectPackage != nullptr)
			{
				packageDependencies.Add(objectPackage->GetPackageName());
			}
		}

		stream->SetBinaryMode(true);

		*stream << PACKAGE_MAGIC_NUMBER;
		*stream << PACKAGE_VERSION_MAJOR;
		*stream << PACKAGE_VERSION_MINOR;

		u64 fileCrcPos = stream->GetCurrentPosition();
		*stream << (u32)0; // 0 CRC

		u64 dataStartOffsetValuePos = stream->GetCurrentPosition();
		*stream << (u64)0; // Data Start Offset

		*stream << package->GetPackageUuid();
		*stream << package->GetPackageName();

		// Other packages that `this` package depends upon
		SavePackageDependencies(stream, packageDependencies);

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

			if (package != objectInstance->GetPackage()) // Don't serialize objects outside of `this` package
			{
				continue;
			}
            
			*stream << PACKAGE_OBJECT_MAGIC_NUMBER; // .OBJECT.
			
			*stream << objectInstance->GetUuid();
			*stream << objectInstance->IsAsset();
			*stream << objectInstance->GetPathInPackage();
			*stream << objectInstance->GetClass()->GetTypeName();
			*stream << objectInstance->GetName();

			if (objectInstance->IsAsset())
			{
				*stream << ((Asset*)objectInstance)->GetSourceAssetRelativePath();
			}

			// Data start offset (excluding itself)
			*stream << stream->GetCurrentPosition() + sizeof(u64);

			auto classType = objectInstance->GetClass();

			BinarySerializer serializer = BinarySerializer(classType, package);
			serializer.Serialize(stream);

			u32 crc = 0;
			*stream << crc; // 0 crc for now. Not needed
		}

		*stream << (u64)0; // EOF

		return SavePackageResult::Success;
	}

	static void LoadPackageDependencies(Stream* stream, Array<Name>& outDependencies)
	{
		u32 numDependencies = 0;
		*stream >> numDependencies;

		for (int i = 0; i < numDependencies; i++)
		{
			String packageName = "";
			*stream >> packageName;
			outDependencies.Add(packageName);
		}
	}

	Package* Package::LoadPackage(Package* outer, Stream* stream, IO::Path fullPackagePath, LoadPackageResult& outResult, LoadFlags loadFlags)
	{
		if (stream == nullptr)
		{
			return nullptr;
		}

		// Actual package name based on it's path
		String actualPackageName = "";

		if (IO::Path::IsSubDirectory(fullPackagePath, gProjectPath / "Game"))
		{
			auto relativePath = IO::Path::GetRelative(fullPackagePath, gProjectPath).RemoveExtension().GetString().Replace({'\\'}, '/');
			if (!relativePath.StartsWith("/"))
				relativePath = "/" + relativePath;

			actualPackageName = relativePath;
		}

		stream->SetBinaryMode(true);

		u64 magicNumber = 0;
		u32 majorVersion = 0; 
		u32 minorVersion = 0;
		*stream >> magicNumber;
        
		if (magicNumber != PACKAGE_MAGIC_NUMBER)
		{
			outResult = LoadPackageResult::InvalidPackage;
			return nullptr;
		}

		*stream >> majorVersion;

		if (majorVersion > PACKAGE_VERSION_MAJOR)
		{
			outResult = LoadPackageResult::UnsupportedPackageVersion;
			return nullptr;
		}

		*stream >> minorVersion;

		u32 fileCrc = 0;
		*stream >> fileCrc;
		u64 dataStartOffset;
		*stream >> dataStartOffset;

		UUID packageUuid = 0;
		*stream >> packageUuid;
		String packageName = "";
		*stream >> packageName;

		if (!actualPackageName.IsEmpty() && actualPackageName != packageName)
		{
			packageName = actualPackageName;
		}

		// Package dependencies list: v1.1
		Array<Name> packageDependendies{};
		if (IsVersionGreaterThanOrEqualTo(majorVersion, minorVersion, PackageDependencies_Major, PackageDependencies_Minor))
		{
			LoadPackageDependencies(stream, packageDependendies);
		}

		u8 isCooked = 0;
		if (IsVersionGreaterThanOrEqualTo(majorVersion, minorVersion, IsCookedValue_Major, IsCookedValue_Minor))
		{
			*stream >> isCooked;
		}

		Package* package = nullptr;

		if (loadedPackages.KeyExists(packageName))
		{
			package = loadedPackages[packageName];
			package->packageDependencies = packageDependendies;
		}
		else
		{
			Internal::ConstructObjectParams params{ Package::Type() };
			params.name = packageName;
			params.uuid = packageUuid;
			params.templateObject = nullptr;
			params.objectFlags = OF_NoFlags;
			params.outer = nullptr;

			package = (Package*)Internal::StaticConstructObject(params);
			package->fullPackagePath = fullPackagePath;
			package->packageDependencies = packageDependendies;
			package->isFullyLoaded = false;
			package->objectUuidToEntryMap.Clear();
		}

		loadedPackages[packageName] = package;

		package->isCooked = isCooked > 0 ? true : false;
		package->majorVersion = majorVersion;
		package->minorVersion = minorVersion;

		stream->Seek(dataStartOffset);

		u64 magicObjectNumber = 0;
		*stream >> magicObjectNumber;

		while (magicObjectNumber == PACKAGE_OBJECT_MAGIC_NUMBER)
		{
			u64 objectUuid = 0;
			*stream >> objectUuid;

			u8 isAsset = 0;
			*stream >> isAsset;

			String pathInPackage{};
			*stream >> pathInPackage;

			String objectTypeName{};
			*stream >> objectTypeName;

			u32 dataByteSize = 0;
			*stream >> dataByteSize;

			u32 numFields = 0;
			*stream >> numFields;

			u64 dataStartOffset = 0;
			*stream >> dataStartOffset;

			String objectName{};
			*stream >> objectName;

			String sourceAssetRelativePath{};
			*stream >> sourceAssetRelativePath;

			stream->Seek(dataStartOffset);
			stream->Seek(dataByteSize, SeekMode::Current); // Skip data for now
            
            package->objectUuidToEntryMap[objectUuid] = ObjectEntryMetaData();
            
            ObjectEntryMetaData& objectEntry = package->objectUuidToEntryMap[objectUuid];
            objectEntry.instanceUuid = objectUuid;
            objectEntry.isAsset = isAsset > 0 ? true : false;
            objectEntry.objectClassName = objectTypeName;
            objectEntry.pathInPackage = pathInPackage;
            objectEntry.offsetInFile = dataStartOffset;
            objectEntry.objectDataSize = dataByteSize;
			objectEntry.objectName = objectName;
			objectEntry.sourceAssetRelativePath = sourceAssetRelativePath;

			u32 crc = 0;
			*stream >> crc; // End marker, ignored
			*stream >> crc; // Actual crc, unused

			*stream >> magicObjectNumber; // magic number is 0 if end of file
		}
        
        package->isFullyLoaded = false;
        package->isLoaded = true;
        
        if (loadFlags & LOAD_Full)
        {
            package->LoadFully();
        }

		outResult = LoadPackageResult::Success;
		return package;
	}

    void Package::LoadFully()
    {
        if (isFullyLoaded)
            return;
        if (!fullPackagePath.Exists() || fullPackagePath.IsDirectory())
            return;
        
        FileStream stream = FileStream(fullPackagePath, Stream::Permissions::ReadOnly);
        stream.SetBinaryMode(true);
        LoadFully(&stream);
    }

    void Package::LoadFully(Stream* stream)
    {
        if (stream == nullptr)
            return;
        
        for (auto& [uuid, objectEntry] : objectUuidToEntryMap)
        {
            LoadObjectFromEntry(stream, uuid);
        }
        
		isFullyLoaded = true;
    }

    Object* Package::LoadObject(UUID objectUuid)
    {
        if (objectUuid != 0 && loadedObjects.KeyExists(objectUuid))
            return loadedObjects[objectUuid];
        if (!fullPackagePath.Exists() || fullPackagePath.IsDirectory() || objectUuid == 0)
            return nullptr;
        
        FileStream fileStream = FileStream(fullPackagePath, Stream::Permissions::ReadOnly);
        fileStream.SetBinaryMode(true);
        return LoadObjectFromEntry(&fileStream, objectUuid);
    }

	Object* Package::LoadObject(const Name& objectClassName)
	{
		for (const auto& [uuid, entry] : objectUuidToEntryMap)
		{
			if (entry.objectClassName == objectClassName)
			{
				FileStream fileStream = FileStream(fullPackagePath, Stream::Permissions::ReadOnly);
				fileStream.SetBinaryMode(true);
				return LoadObjectFromEntry(&fileStream, uuid);
			}
		}

		for (auto subobject : GetSubObjectMap())
		{
			if (subobject != nullptr && subobject->GetClass()->GetTypeName() == objectClassName)
				return subobject;
		}

		return nullptr;
	}

	Object* Package::LoadObjectFromEntry(Stream* stream, UUID objectUuid)
    {
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

		Object* objectInstance = nullptr;

		if (objectUuid == this->GetPackageUuid() && objectClass->IsSubclassOf<Package>())
		{
			objectInstance = this;
		}
		else
		{
			Internal::ConstructObjectParams params{ objectClass };
			params.outer = nullptr;
			params.name = entry.objectName.GetString();
			params.templateObject = nullptr;
			params.objectFlags = OF_NoFlags;
			params.uuid = objectUuid;

			objectInstance = Internal::StaticConstructObject(params);
		}

        loadedObjects[objectUuid] = objectInstance;
        
        entry.isLoaded = true;
        
        FieldDeserializer fieldDeserializer{ objectClass->GetFirstField(), objectInstance, this };
		fieldDeserializer.packageMajor = majorVersion;
		fieldDeserializer.packageMinor = minorVersion;
        
        while (fieldDeserializer.HasNext())
        {
            fieldDeserializer.ReadNext(stream);
        }

		objectInstance->OnAfterDeserialize();

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
