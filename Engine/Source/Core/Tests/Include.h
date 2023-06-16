#pragma once

#include "CoreMinimal.h"

namespace PackageTests
{
	using namespace CE;

	namespace SomeNamespace = CE; // Used to test AutoRTTI namespace detection
    
	class WritingTestObj1;
	class WritingTestObj2;

	STRUCT()
	struct WritingTestStructBase
	{
		CE_STRUCT(WritingTestStructBase)
	public:

		FIELD()
		String stringValue = "struct default";

		FIELD()
		Object* owner = nullptr;
	};

	STRUCT()
	struct WritingTestStruct1 : WritingTestStructBase
	{
		CE_STRUCT(WritingTestStruct1, WritingTestStructBase)
	public:

		FIELD()
		WritingTestObj1* obj1Ptr = nullptr;
	};
	
	CLASS()
	class WritingTestObj1 : public CE::Object
	{
		CE_CLASS(WritingTestObj1, CE::Object)
	public:

		FIELD()
		String stringValue = "default";

		FIELD()
		f32 floatValue = 12.421f;

		FIELD()
		Array<String> stringArray{};

		FIELD()
		Object* objPtr = nullptr;
	};

	CLASS()
	class WritingTestObj2 : public CE::Object
	{
		CE_CLASS(WritingTestObj2, CE::Object)
	public:

		FIELD()
		WritingTestStruct1 testStruct{};

		FIELD()
		Array<Object*> objectArray{};

		FIELD()
		u32 value;
	};
}

namespace ObjectTests
{
	using namespace CE;

	CLASS()
	class Sender : public CE::Object
	{
		CE_CLASS(Sender, CE::Object)
	public:

		CE_SIGNAL(MySignal1, String);

	};

	CLASS()
	class Receiver : public CE::Object
	{
		CE_CLASS(Receiver, CE::Object)
	public:

		FUNCTION()
		void PrintString(String string)
		{
			printValue = string;
		}

		FIELD()
		String printValue{};
	};

	STRUCT()
	struct SenderStruct
	{
		CE_STRUCT(SenderStruct)
	public:

		CE_SIGNAL(StructSignal1, String);

	};

	STRUCT()
	struct ReceiverStruct
	{
		CE_STRUCT(ReceiverStruct)
	public:

		FUNCTION()
		void PrintStringValue(String string)
		{
			stringValue = string;
		}

		FIELD()
		String stringValue{};

		FIELD()
		Array<String> arrayValue{};
	};
}

namespace JsonTests
{
	using namespace CE;

	STRUCT()
	struct InnerStruct
	{
		CE_STRUCT(InnerStruct)
	public:

		InnerStruct()
		{}

		InnerStruct(const String& str, u32 i) : value(str), myInt(i)
		{}

		FIELD()
		String value{};

		FIELD()
		u32 myInt{};

		FIELD()
		Array<InnerStruct> nextInner{};
	};

	STRUCT()
	struct SerializedData
	{
		CE_STRUCT(SerializedData)
	public:

		FIELD()
		String strVal{};

		FIELD()
		Array<String> strArray{};

		FIELD()
		InnerStruct inner{};

		FIELD()
		Array<InnerStruct> innerArray{};
	};
}

#include "Include.rtti.h"

