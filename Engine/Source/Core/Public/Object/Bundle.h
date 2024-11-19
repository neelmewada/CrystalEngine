#pragma once

namespace CE
{

    enum class BundleLoadResult
    {
        Success = 0,
        UnknownError,
        InvalidStream,
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

    class SerializationException : public Exception
    {
    public:

        SerializationException() : Exception("Serialization Error")
		{}

        SerializationException(const String& msg) : Exception("Serialization Error: " + msg)
		{}
    };

    class CORE_API Bundle : public Object
    {
        CE_CLASS(Bundle, Object)
    public:

        // - Static API -

        static IO::Path GetAbsoluteBundlePath(const Name& bundlePath);

        static BundleSaveResult SaveToDisk(const Ref<Bundle>& bundle, Ref<Object> asset);
        static BundleSaveResult SaveToDisk(const Ref<Bundle>& bundle, Ref<Object> asset, const IO::Path& fullPath);

    protected:

        static Ref<Bundle> LoadFromDisk(Stream* stream, BundleLoadResult& outResult, bool loadFully = true);
        static BundleSaveResult SaveToDisk(const Ref<Bundle>& bundle, Ref<Object> asset, Stream* stream);

        void OnAfterConstruct() override;
        void OnBeginDestroy() override;

    private:

        Bundle();

        void OnObjectUnloaded(Object* object);

        static bool IsFieldSerialized(FieldType* field, StructType* schemaType);

        static void SerializeSchemaTable(const Ref<Bundle>& bundle, Stream* stream, const Array<StructType*>& schemaTypes, 
            const HashMap<StructType*, int>& schemaTypeToIndex);

        static void SerializeFieldSchema(FieldType* field, Stream* stream, const HashMap<StructType*, int>& schemaTypeToIndex);

        void FetchAllSchemaTypes(Array<ClassType*>& outClasses, Array<StructType*>& outStructs);

        struct FieldSchema
        {
            Name fieldName{};
            u8 typeByte = 0;
            u32 schemaIndexOfFieldType = 0;
            u8 underlyingTypeByte = 0;
        };

        struct SchemaEntry
        {
            bool isStruct = false;
            bool isClass = false;
            Name fullTypeName = "";
            Array<FieldSchema> fields{};
        };

        struct SerializedObjectEntry
        {
            u64 offsetInFile = 0;
            Uuid instanceUuid = Uuid::Null();
            b8 isAsset = false;
            u32 schemaIndex = 0;
			String pathInBundle{};
            Name objectClassName{};
            Name objectName{};
            u64 objectSerializedDataSize = 0;

            b8 isLoaded = false;
            b8 isDeserialized = false;
        };

        IO::Path fullBundlePath{};

        u32 majorVersion = 0;
        u32 minorVersion = 0;

        // If this bundle was created from deserialization
        b8 isLoadedFromDisk = false;
        b8 isFullyLoaded = false;

        Array<SchemaEntry> schemaTable;
        Array<Uuid> dependencies;

        Array<SerializedObjectEntry> serializedObjectEntries;

        // Used to prevent bundle simultaneous Load and Store operations.
        SharedMutex bundleMutex{};

        SharedMutex loadedObjectsMutex{};
        HashMap<Uuid, WeakRef<Object>> loadedObjectsByUuid{};

        static SharedMutex bundleRegistryMutex;
        static HashMap<Uuid, WeakRef<Bundle>> loadedBundlesByUuid;

        friend class ObjectSerializer;
        friend class Object;
    };

}

CE_RTTI_CLASS(CORE_API, CE, Bundle,
	CE_SUPER(CE::Object),
	CE_NOT_ABSTRACT,
	CE_ATTRIBS(),
	CE_FIELD_LIST(

    ),
	CE_FUNCTION_LIST()
)
