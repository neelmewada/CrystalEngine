
#include "Core.h"

namespace CE
{
    SharedMutex Bundle::bundleRegistryMutex{};
    HashMap<Uuid, WeakRef<Bundle>> Bundle::loadedBundlesByUuid{};

    Array<IBundleResolver*> Bundle::bundleResolvers{};

    Bundle::Bundle()
    {

    }

    void Bundle::OnAfterConstruct()
    {
        Super::OnAfterConstruct();

        LockGuard lock{ bundleRegistryMutex };
        loadedBundlesByUuid[GetUuid()] = this;
    }

    void Bundle::OnBeginDestroy()
    {
	    Super::OnBeginDestroy();

        LockGuard lock{ bundleRegistryMutex };
        loadedBundlesByUuid.Remove(GetUuid());
    }

    void Bundle::OnObjectUnloaded(Object* object)
    {
        if (object != nullptr)
        {
            LockGuard lock{ loadedObjectsMutex };

            loadedObjectsByUuid.Remove(object->GetUuid());
        }
    }

    void Bundle::PushBundleResolver(IBundleResolver* resolver)
    {
        bundleResolvers.Add(resolver);
    }

    void Bundle::PopBundleResolver(IBundleResolver* resolver)
    {
        bundleResolvers.Remove(resolver);
    }

    IO::Path Bundle::GetAbsoluteBundlePath(const Name& bundlePath)
    {
        if (!bundlePath.IsValid())
            return {};
        String bundleNameStr = bundlePath.GetString();

        if (bundleNameStr.StartsWith("/Engine/") || bundleNameStr == "/Engine") // Example: /Engine/Assets/Textures/Noise/Perlin/Perlin04
        {
            if (!gProjectPath.Exists() || !(gProjectPath / "Engine/Assets").Exists())
            {
                return EngineDirectories::GetEngineInstallDirectory() / (bundleNameStr.GetSubstring(1) + ".casset");
            }
            return gProjectPath / (bundleNameStr.GetSubstring(1) + ".casset");
        }
        else if (bundleNameStr.StartsWith("/Game/") || bundleNameStr == "/Game") // Example: /Game/Assets/Textures/SomeTexture
        {
            return gProjectPath / (bundleNameStr.GetSubstring(1) + ".casset");
        }
        else if (bundleNameStr.StartsWith("/Editor/") || bundleNameStr == "/Editor")
        {
            return EngineDirectories::GetEngineInstallDirectory() / (bundleNameStr.GetSubstring(1) + ".casset");
        }
        else if (bundleNameStr.StartsWith("/Temp/") || bundleNameStr == "/Temp")
        {
            return gProjectPath / (bundleNameStr.GetSubstring(1) + ".temp");
        }

        if (bundleNameStr[0] != '/')
        {
            bundleNameStr.InsertAt('/', 0);
        }

        return PlatformDirectories::GetLaunchDir() / (bundleNameStr.GetSubstring(1) + ".casset");
    }

    Ref<Bundle> Bundle::LoadFromDisk(const Name& path, const LoadBundleArgs& loadArgs)
    {
        BundleLoadResult result;
        return LoadFromDisk(path, result, loadArgs);
    }

    Ref<Bundle> Bundle::LoadFromDisk(const Name& path, BundleLoadResult& outResult, const LoadBundleArgs& loadArgs)
    {
        IO::Path absolutePath = GetAbsoluteBundlePath(path);

        if (!absolutePath.Exists())
        {
            CE_LOG(Error, All, "Bundle::LoadFromDisk(): Bundle doesn't exist at path {}", absolutePath);
            outResult = BundleLoadResult::BundleNotFound;
            return nullptr;
        }

        FileStream stream = FileStream(absolutePath, Stream::Permissions::ReadOnly);
        stream.SetBinaryMode(true);

        return LoadFromDisk(&stream, outResult, loadArgs);
    }

    BundleSaveResult Bundle::SaveToDisk(const Ref<Bundle>& bundle, Ref<Object> asset)
    {
        if (bundle == nullptr)
        {
            CE_LOG(Error, All, "SaveToDisk() with a NULL bundle!");
            return BundleSaveResult::InvalidBundle;
        }

        if (bundle->IsTransient())
        {
            CE_LOG(Error, All, "SaveToDisk() passed with a transient bundle named: {}", bundle->GetName());
            return BundleSaveResult::InvalidBundle;
        }

        IO::Path absolutePath = GetAbsoluteBundlePath(bundle->GetName());
        return SaveToDisk(bundle, asset, absolutePath);
    }

    BundleSaveResult Bundle::SaveToDisk(const Ref<Bundle>& bundle, Ref<Object> asset, const IO::Path& fullPath)
    {
        auto path = fullPath;
        if (path.GetExtension().IsEmpty())
        {
            path = path.GetString() + ".casset";
        }

        if (!path.GetParentPath().Exists())
        {
            IO::Path::CreateDirectories(path.GetParentPath());
        }

        if (bundle == nullptr)
        {
            CE_LOG(Error, All, "SaveToDisk() with a NULL bundle!");
            return BundleSaveResult::InvalidBundle;
        }

        if (bundle->IsTransient())
        {
            CE_LOG(Error, All, "SaveToDisk() passed with a transient bundle named: {}", bundle->GetName());
            return BundleSaveResult::InvalidBundle;
        }

        if (path.IsDirectory())
        {
            CE_LOG(Error, All, "SaveToDisk() passed with a bundle path that is a directory!");
            return BundleSaveResult::InvalidPath;
        }
        if (asset != nullptr && asset->GetBundle() != bundle.Get())
        {
            CE_LOG(Error, All, "SaveToDisk() passed with an asset object that is not part of the bundle!");
            return BundleSaveResult::AssetNotInBundle;
        }

        bundle->fullBundlePath = path;

        FileStream stream = FileStream(path, Stream::Permissions::WriteOnly, true, true);
        stream.SetBinaryMode(true);

        return SaveToDisk(bundle, asset, &stream);
    }

    Ref<Object> Bundle::LoadObject(Uuid objectUuid)
    {
        if (objectUuid.IsNull())
        {
            return nullptr;
        }

        {
            LockGuard lock{ loadedObjectsMutex };

            if (loadedObjectsByUuid.KeyExists(objectUuid))
            {
                if (Ref<Object> object = loadedObjectsByUuid[objectUuid].Lock())
                {
                    return object;
                }
            }
        }

        if (readerStream != nullptr)
        {
            return LoadObject(readerStream, objectUuid);
        }

        if (fullBundlePath.Exists())
        {
            FileStream stream = FileStream(fullBundlePath, Stream::Permissions::ReadOnly);
            stream.SetBinaryMode(true);

            readerStream = &stream;

            Ref<Object> object = LoadObject(&stream, objectUuid);

            readerStream = nullptr;
            return object;
        }

        return nullptr;
    }

    void Bundle::FetchAllSchemaTypes(Array<ClassType*>& outClasses, Array<StructType*>& outStructs)
    {
        HashSet<ClassType*> classes;
        HashSet<StructType*> structs;

        std::function<void(StructType*)> fetchInternalStructs = [&](StructType* structType)
            {
                if (structType == nullptr || structs.Exists(structType))
                    return;

                structs.Add(structType);

                for (int i = 0; i < structType->GetFieldCount(); ++i)
                {
                    FieldType* field = structType->GetFieldAt(i);

                    if (field->IsStructField())
                    {
                        Struct* childStruct = StructType::FindStruct(field->GetDeclarationTypeId());

                        fetchInternalStructs(childStruct);
                    }
                }
            };

        std::function<void(Object*)> fetchStructs = [&](Object* object)
            {
                if (object == nullptr)
                    return;

                Class* clazz = object->GetClass();

                for (int i = 0; i < clazz->GetFieldCount(); ++i)
                {
                    FieldType* field = clazz->GetFieldAt(i);

                    if (field->IsStructField())
                    {
                        Struct* structType = StructType::FindStruct(field->GetDeclarationTypeId());

                        fetchInternalStructs(structType);
                    }
                    else if (field->IsArrayField())
                    {
                        TypeInfo* underlyingType = field->GetUnderlyingType();
                        if (underlyingType != nullptr && underlyingType->IsStruct())
                        {
                            StructType* structType = static_cast<StructType*>(underlyingType);

                            fetchInternalStructs(structType);
                        }
                    }
                }
            };

        std::function<void(Object*)> visitor = [&](Object* object)
            {
                if (object == nullptr)
                    return;

				if (!classes.Exists(object->GetClass()))
		        {
			        classes.Add(object->GetClass());

                	fetchStructs(object);
		        }

                for (int i = 0; i < object->GetSubObjectCount(); ++i)
                {
                    visitor(object->GetSubObject(i));
                }
            };

        visitor(this);

        for (ClassType* clazz : classes)
        {
            outClasses.Add(clazz);
        }

        for (StructType* structType : structs)
        {
            outStructs.Add(structType);
        }
    }
    
} // namespace CE

CE_RTTI_CLASS_IMPL(CORE_API, CE, Bundle)
