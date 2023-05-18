#pragma once

#include "CoreMinimal.h"

#define PACKAGE_MAGIC_NUMBER FROM_BIG_ENDIAN((u64)0x005041434b00000a) // .PACK..\n

#define PACKAGE_VERSION_MAJOR (u16)0
#define PACKAGE_VERSION_MINOR (u16)1

#define PACKAGE_OBJECT_MAGIC_NUMBER FROM_BIG_ENDIAN((u64)0x004f424a45435400) // .OBJECT.

namespace CE
{

	SavePackageResult Package::SavePackage(Package* package, Object* asset, Stream* stream, const SavePackageArgs& saveArgs)
	{
		if (package == nullptr)
		{
			return SavePackageResult::InvalidPackage;
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

		u32 fileCrcPos = stream->GetCurrentPosition();
		*stream << (u32)0; // 0 CRC

		u32 dataStartOffsetValuePos = stream->GetCurrentPosition();
		*stream << (u64)0; // Data Start Offset

		*stream << package->GetPackageUuid();
		*stream << package->GetPackageName();

		u64 curPos = stream->GetCurrentPosition();
		stream->Seek(dataStartOffsetValuePos);
		*stream << curPos;
		stream->Seek(curPos);

		// Object Entries
		for (const auto& [objectUuid, objectInstance] : objectsToSerialize)
		{
			if (objectInstance == nullptr || objectUuid == 0)
				continue;
            
			*stream << PACKAGE_OBJECT_MAGIC_NUMBER;
			*stream << objectInstance->GetUuid();
			*stream << (objectInstance->IsAsset() ? (u8)1 : (u8)0);
			if (objectInstance == package)
				*stream << "";
			else
				*stream << objectInstance->GetPathInPackage();
			*stream << objectInstance->GetClass()->GetName();

			u64 dataSizeValuePos = stream->GetCurrentPosition();
			*stream << (u32)0; // 0 data size. updated later

			*stream << (u32)0; // Number of field entries

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
    
} // namespace CE
