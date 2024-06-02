#pragma once

namespace CE
{
	class Resource;

	struct RawData
	{
		u8* data = nullptr;
		u32 dataSize = 0;
	};
    
	class CORE_API ResourceManager : public Object
	{
		CE_CLASS(ResourceManager, Object)
	public:
		ResourceManager();
		virtual ~ResourceManager();

		void RegisterResource(const String& moduleName, const String& pathToResource, void* data, u32 dataSizeInBytes);

		void DeregisterResource(const String& moduleName, const String& pathToResource);

		CE::Resource* LoadResource(const Name& path, Object* outer = GetTransientPackage());

		RawData GetRawData(const Name& path);

		String LoadTextResource(const Name& path);

	protected:

		PathTree pathTree{};
	};

} // namespace CE

CE_RTTI_CLASS(CORE_API, CE, ResourceManager,
	CE_SUPER(Object),
	CE_NOT_ABSTRACT,
	CE_ATTRIBS(),
	CE_FIELD_LIST(),
	CE_FUNCTION_LIST()
)