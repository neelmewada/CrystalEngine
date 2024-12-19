#pragma once

namespace CE
{

	class CORE_API ObjectSerializer final
	{
	public:

		ObjectSerializer(const Ref<Bundle>& bundle, Object* target, u32 schemaIndex);

		void Serialize(Stream* stream);

		void Deserialize(Stream* stream);

	private:

		void SerializeField(const Ptr<FieldType>& field, void* instance, Stream* stream);

		void DeserializeField(const Ptr<FieldType>& field, void* instance, Stream* stream, const Bundle::FieldSchema& fieldSchema);

		Ref<Object> LoadObjectReference(Uuid objectUuid, Uuid bundleUuid);

		Ref<Bundle> bundle = nullptr;
		Object* target = nullptr;
		u32 schemaIndex = 0;

		friend class Bundle;
	};

}
