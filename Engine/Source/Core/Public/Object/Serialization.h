#pragma once

namespace CE
{
	class FieldType;

	class CORE_API FieldSerializer
	{
	public:
        FieldSerializer(FieldType* fieldChain, void* instance);
        FieldSerializer(Array<FieldType*> fieldList, void* instance);

		bool HasNext();

		bool WriteNext(Stream* stream);

		FieldType* GetNext();

		void SkipHeader(bool skip);

	private:
		
		Array<FieldType*> fields{};
		void* rawInstance = nullptr;

		bool skipHeader = false;
	};
	
}
