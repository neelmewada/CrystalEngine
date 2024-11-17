#pragma once

namespace CE
{

    enum class BundleSaveResult
	{
		Success = 0,
		UnknownError,
		InvalidStream,
		InvalidPath,
		InvalidBundle,
		AssetNotInBundle,
	};

    class CORE_API Bundle : public Object
    {
    public:

        // - Static API -

        static BundleSaveResult SaveToDisk(const WeakRef<Bundle>& bundle, const Ref<Object>& asset, const IO::Path& fullBundleFilePath);

    private:

        struct FieldSchema
        {
            Name fieldName{};
            u8 typeByte = 0;
            int schemaIndex = 0;
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
			String pathInBundle{};
            Name objectClassName{};
            Name objectName{};
            u32 objectDataSize = 0;

            b8 isLoaded = false;
        };

        Array<SchemaEntry> schemaTable;
        HashMap<Uuid, Ref<Object>> loadedObjectsByUuid;
        HashMap<Uuid, SerializedObjectEntry> objectEntriesByUuid;
        
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
