#pragma once

namespace CE
{

	class CORE_API Resource : public Object
	{
		CE_CLASS(Resource, Object)
	public:

		Resource();
		virtual ~Resource();

		inline u8* GetData() const { return data; }
		inline u32 GetDataSize() const { return dataSize; }

		inline const String& GetExtension() const { return extension; }

		inline const Name& GetFullPath() const { return fullPath; }

		inline bool IsValid() const { return data != nullptr; }

	protected:

		u8* data = nullptr;
		u32 dataSize = 0;

		String extension = "";

		Name fullPath{};

		friend class ResourceManager;
	};
    
} // namespace CE

CE_RTTI_CLASS(CORE_API, CE, Resource,
	CE_SUPER(Object),
	CE_NOT_ABSTRACT,
	CE_ATTRIBS(),
	CE_FIELD_LIST(),
	CE_FUNCTION_LIST()
)