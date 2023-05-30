#pragma once

#include "CoreMinimal.h"

#define PACKAGE_MAGIC_NUMBER CE::FromBigEndian((u64)0x005041434b00000a) // .PACK..\n

#define PACKAGE_VERSION_MAJOR (u16)0
#define PACKAGE_VERSION_MINOR (u16)1

#define PACKAGE_OBJECT_MAGIC_NUMBER CE::FromBigEndian((u64)0x004f424a45435400) // .OBJECT.

namespace CE
{

	SavePackageResult Package::SavePackage(Package* package, Object* asset, Stream* stream, const SavePackageArgs& saveArgs)
	{
		if (package == nullptr)
		{
			return SavePackageResult::InvalidPackage;
		}
		if (package->IsTransient())
		{
			CE_LOG(Error, All, "SavePackage() passed with a transient package!");
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
            
			*stream << PACKAGE_OBJECT_MAGIC_NUMBER;
			*stream << objectInstance->GetUuid();
			*stream << (objectInstance->IsAsset() ? (u8)1 : (u8)0);
			if (objectInstance == package)
				*stream << "";
			else
				*stream << objectInstance->GetPathInPackage();
			*stream << objectInstance->GetClass()->GetTypeName();

			u64 dataSizeValuePos = stream->GetCurrentPosition();
			*stream << (u32)0; // 0 data size. updated later

			*stream << (u32)0; // Number of field entries

			curPos = stream->GetCurrentPosition();
			*stream << (u64)(curPos + sizeof(u64)); // Data Start Offset (excluding itself)

			u64 dataStartPos = stream->GetCurrentPosition();
			u32 numEntries = 0;
            
            ClassType* objectClass = objectInstance->GetClass();
            FieldType* firstField = objectClass->GetFirstField();
            if (firstField != nullptr)
            {
                FieldSerializer fieldSerializer{ firstField, objectInstance };
                
                while (fieldSerializer.HasNext())
                {
					if (fieldSerializer.WriteNext(stream))
					{
						numEntries++;
					}
                }
            }

			u64 dataEndPos = stream->GetCurrentPosition();

			stream->Seek(dataSizeValuePos);
			u32 dataSize = (u32)(dataEndPos - dataStartPos);
			*stream << dataSize;
			*stream << numEntries;

			u32 crc = 0;
			//char* dataPtr = new char[dataSize];
			//{
			//	stream->Read(dataPtr, dataSize);
			//	crc = CalculateCRC(dataPtr, dataSize);
			//}
			//delete[] dataPtr; dataPtr = nullptr;

			stream->Seek(dataEndPos);
			*stream << (u32)0; // End of Field Entries
			*stream << crc; // Object CRC checksum
		}

		*stream << (u64)0; // EOF

		return SavePackageResult::Success;
	}

	Package* Package::LoadPackage(Package* outer, Stream* stream, LoadPackageResult& outResult, LoadFlags loadFlags)
	{
		if (stream == nullptr)
		{
			return nullptr;
		}

		Package* package = CreateObject<Package>(outer, String::Format("/Temp/{}", GenerateRandomU64()));

		stream->SetBinaryMode(true);

		u64 magicNumber = 0;
		u16 majorVersion = 0, minorVersion = 0;
		*stream >> magicNumber;
		if (magicNumber != PACKAGE_MAGIC_NUMBER)
		{
			outResult = LoadPackageResult::InvalidPackage;
			package->RequestDestroy();
			return nullptr;
		}

		*stream >> majorVersion;
		*stream >> minorVersion;

		if (majorVersion > PACKAGE_VERSION_MAJOR)
		{
			outResult = LoadPackageResult::InvalidPackage;
			package->RequestDestroy();
			return nullptr;
		}

		u32 fileCrc = 0;
		*stream >> fileCrc;
		u64 dataStartOffset;
		*stream >> dataStartOffset;

		UUID packageUuid = 0;
		*stream >> packageUuid;
		String packageName = "";
		*stream >> packageName;
        
        package->SetPackageName(packageName);

		stream->Seek(dataStartOffset);

		u64 magicObjectNumber = 0;
		*stream >> magicObjectNumber;
        
        package->objectUuidToEntryMap.Clear();

		while (magicObjectNumber == PACKAGE_OBJECT_MAGIC_NUMBER)
		{
			u64 objectUuid = 0;
			*stream >> objectUuid;

			u8 isAsset = 0;
			*stream >> isAsset;

			String pathInPackage{};
			*stream >> pathInPackage;

			Name objectTypeName{};
			*stream >> objectTypeName;

			u32 dataByteSize = 0;
			*stream >> dataByteSize;

			u32 numFields = 0;
			*stream >> numFields;

			u64 dataStartOffset = 0;
			*stream >> dataStartOffset;

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
        LoadFully(&stream);
    }

    void Package::LoadFully(Stream* stream)
    {
        if (stream == nullptr)
            return;
        
        for (auto& [uuid, objectEntry] : objectUuidToEntryMap)
        {
			if (loadedSubobjects.KeyExists(uuid)) // Skip already loaded objects
				continue;

            u64 dataOffset = objectEntry.offsetInFile;
			u32 dataSize = objectEntry.objectDataSize;
            stream->Seek(dataOffset);
			if (!objectEntry.objectClassName.IsValid())
				continue;

			ClassType* objectClass = ClassType::FindClass(objectEntry.objectClassName);

			if (dataSize == 0 || objectClass == nullptr)
				continue;

			const auto& pathInPackage = objectEntry.pathInPackage;
			Array<String> components{};
			pathInPackage.Split(".", components);
			String objectName = "";
			if (components.NonEmpty())
				objectName = components.GetLast();

			Object* objectInstance = CreateObject<Object>(this, objectName, OF_NoFlags, objectClass);
			loadedSubobjects[uuid] = objectInstance;

			objectEntry.isLoaded = true;

			FieldDeserializer fieldDeserializer{ objectClass->GetFirstField(), objectInstance, this };

			while (fieldDeserializer.HasNext())
			{
				fieldDeserializer.ReadNext(stream);
			}
        }

		isFullyLoaded = true;
    }
    
} // namespace CE
