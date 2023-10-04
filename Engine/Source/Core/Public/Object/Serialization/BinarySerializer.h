#pragma once

namespace CE
{
    
	class CORE_API BinarySerializer
	{
	public:
		BinarySerializer(TypeInfo* targetType, void* targetInstance);

		inline bool IsValid() const
		{
			return targetType != nullptr && instance != nullptr;
		}

		/// Serialize data to output stream
		int Serialize(Stream* stream);

	protected:

		bool SerializeField(Stream* stream, Field* field, void* instance);

		TypeInfo* targetType = nullptr;
		void* instance = nullptr;
		bool isMap = false;
	};

	class CORE_API BinaryDeserializer
	{
	public:
		BinaryDeserializer(TypeInfo* targetType, void* targetInstance);

		inline bool IsValid() const
		{
			return targetType != nullptr && instance != nullptr;
		}

		int Deserialize(Stream* stream);

	protected:

		bool DeserializeField(Stream* stream, Field* field, void* instance);

		void SkipFieldValue(Stream* stream);

		TypeInfo* targetType = nullptr;
		void* instance = nullptr;
	};

} // namespace CE

