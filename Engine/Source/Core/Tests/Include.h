#pragma once

#include "CoreMinimal.h"

using namespace CE;

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

	class DerivedClassA;

	CLASS()
	class BaseClass : public Object
	{
		CE_CLASS(BaseClass, Object)
	public:

		FIELD()
		String string = "base class";

		FIELD()
		SubClassType<BaseClass> assignedClass{};

		FIELD()
		SubClassType<DerivedClassA> derivedClassType{};
	};

	CLASS()
	class DerivedClassA : public BaseClass
	{
		CE_CLASS(DerivedClassA, BaseClass)
	public:

		FIELD()
		SubClassType<Object> anyClass{};
	};

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

// CDI

namespace CDITests
{
	class TestObject;
	class AnotherObject;

	STRUCT()
	struct TestStruct
	{
		CE_STRUCT(TestStruct)
	public:

		FIELD()
		Array<String> stringArray = {};

		FIELD()
		TestObject* testObject = nullptr;

		FIELD()
		AnotherObject* another = nullptr;
	};

	CLASS()
	class AnotherObject : public Object
	{
		CE_CLASS(AnotherObject, Object)
	public:

		AnotherObject()
		{
			data.stringArray = {
				"another0", "another1", "another2"
			};
			data.another = this;
		}

		FIELD()
		TestStruct data{};

		FIELD()
		String myString = "default";

		FIELD()
		TestObject* test = nullptr;

		FIELD()
		Object* transient = nullptr;
	};

	CLASS()
	class TestObject : public Object
	{
		CE_CLASS(TestObject, Object)
	public:

		TestObject()
		{
			subobject = CreateDefaultSubobject<AnotherObject>("SomeSubobject");
			subobject->myString = "modified from TestObject";
			subobject->data.stringArray = {
				"test0", "test1"
			};

			subobject->data.another = nullptr;
			subobject->data.testObject = this;
			
			if (!IsDefaultInstance()) // Not a CDI
			{
				another = CreateDefaultSubobject<AnotherObject>("AnotherSubobject");
				another->myString = "modified manually";
			}
		}

		FIELD()
		AnotherObject* subobject = nullptr;

		FIELD()
		AnotherObject* another = nullptr;

		FIELD()
		Object* transient = nullptr;
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

CE_RTTI_CLASS(, PackageTests, WritingTestObj1,
    CE_SUPER(CE::Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(stringValue)
        CE_FIELD(floatValue)
        CE_FIELD(stringArray)
        CE_FIELD(objPtr)
    ),
    CE_FUNCTION_LIST(
    )
)
CE_RTTI_CLASS(, PackageTests, WritingTestObj2,
    CE_SUPER(CE::Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(testStruct)
        CE_FIELD(objectArray)
        CE_FIELD(value)
    ),
    CE_FUNCTION_LIST(
    )
)
CE_RTTI_CLASS(, ObjectTests, BaseClass,
    CE_SUPER(Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(string)
        CE_FIELD(assignedClass)
        CE_FIELD(derivedClassType)
    ),
    CE_FUNCTION_LIST(
    )
)
CE_RTTI_CLASS(, ObjectTests, DerivedClassA,
    CE_SUPER(BaseClass),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(anyClass)
    ),
    CE_FUNCTION_LIST(
    )
)
CE_RTTI_CLASS(, ObjectTests, Sender,
    CE_SUPER(CE::Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
    ),
    CE_FUNCTION_LIST(
        CE_FUNCTION2(MySignal1, auto, (String), Signal)
    )
)
CE_RTTI_CLASS(, ObjectTests, Receiver,
    CE_SUPER(CE::Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(printValue)
    ),
    CE_FUNCTION_LIST(
        CE_FUNCTION2(PrintString, void, (String string))
    )
)
CE_RTTI_CLASS(, CDITests, AnotherObject,
    CE_SUPER(Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(data)
        CE_FIELD(myString)
        CE_FIELD(test)
        CE_FIELD(transient)
    ),
    CE_FUNCTION_LIST(
    )
)
CE_RTTI_CLASS(, CDITests, TestObject,
    CE_SUPER(Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
		CE_FIELD(subobject)
		CE_FIELD(another)
        CE_FIELD(transient)
    ),
    CE_FUNCTION_LIST(
    )
)
CE_RTTI_STRUCT(, PackageTests, WritingTestStructBase,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(stringValue)
        CE_FIELD(owner)
    ),
    CE_FUNCTION_LIST(
    )
)
CE_RTTI_STRUCT(, PackageTests, WritingTestStruct1,
    CE_SUPER(WritingTestStructBase),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(obj1Ptr)
    ),
    CE_FUNCTION_LIST(
    )
)
CE_RTTI_STRUCT(, ObjectTests, SenderStruct,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
    ),
    CE_FUNCTION_LIST(
        CE_FUNCTION2(StructSignal1, auto, (String), Signal)
    )
)
CE_RTTI_STRUCT(, ObjectTests, ReceiverStruct,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(stringValue)
        CE_FIELD(arrayValue)
    ),
    CE_FUNCTION_LIST(
        CE_FUNCTION2(PrintStringValue, void, (String string))
    )
)
CE_RTTI_STRUCT(, CDITests, TestStruct,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(stringArray)
        CE_FIELD(testObject)
        CE_FIELD(another)
    ),
    CE_FUNCTION_LIST(
    )
)
CE_RTTI_STRUCT(, JsonTests, InnerStruct,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(value)
        CE_FIELD(myInt)
        CE_FIELD(nextInner)
    ),
    CE_FUNCTION_LIST(
    )
)
CE_RTTI_STRUCT(, JsonTests, SerializedData,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(strVal)
        CE_FIELD(strArray)
        CE_FIELD(inner)
        CE_FIELD(innerArray)
    ),
    CE_FUNCTION_LIST(
    )
)

