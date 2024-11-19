#pragma once

namespace CE
{

	class CORE_API ObjectSerializer final
	{
	public:

		ObjectSerializer(const Ref<Bundle>& bundle, Object* target, const HashMap<StructType*, int>& schemaTypeToIndex);

		void Serialize(Stream* stream);

	private:

		void SerializeField(FieldType* field, void* instance, Stream* stream, bool storeByteSize = true);

		Ref<Bundle> bundle = nullptr;
		Object* target = nullptr;
		const HashMap<StructType*, int>& schemeTypeToIndex;

		friend class Bundle;
	};

}
