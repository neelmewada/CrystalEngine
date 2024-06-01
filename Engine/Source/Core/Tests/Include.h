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

namespace EventTests
{
	typedef ScriptEvent<void(const String& text)> TextEvent;
	typedef ScriptEvent<void(Object* sender)> ObjectEvent;
	DECLARE_SCRIPT_EVENT(FileActionEvent, CE::IO::FileAction fileAction);

	CLASS()
	class SenderClass : public Object
	{
		CE_CLASS(SenderClass, Object)
	public:

		TextEvent onTextChanged{};

		ObjectEvent onObjectEvent{};

		FileActionEvent fileActionEvent{};
	};

	CLASS()
	class ReceiverClass : public Object
	{
		CE_CLASS(ReceiverClass, Object)
	public:

		void OnTextChangedCallback(const String& text)
		{
			this->text = text;
		}

		void OnObjectEvent(Object* sender)
		{
			this->object = sender;
		}

		void OnFileAction(IO::FileAction fileAction)
		{
			this->fileAction = fileAction;
		}

		String text{};
		Object* object = nullptr;
		IO::FileAction fileAction = IO::FileAction::Add;
	};
	
}

CE_RTTI_CLASS(, EventTests, SenderClass,
	CE_SUPER(CE::Object),
	CE_NOT_ABSTRACT,
	CE_ATTRIBS(),
	CE_FIELD_LIST(
		CE_FIELD(onTextChanged)
		CE_FIELD(onObjectEvent)
		CE_FIELD(fileActionEvent)
	),
	CE_FUNCTION_LIST()
)
CE_RTTI_CLASS_IMPL(, EventTests, SenderClass)

CE_RTTI_CLASS(, EventTests, ReceiverClass,
	CE_SUPER(CE::Object),
	CE_NOT_ABSTRACT,
	CE_ATTRIBS(),
	CE_FIELD_LIST(
		CE_FIELD(text)
		CE_FIELD(object)
		CE_FIELD(fileAction)
	),
	CE_FUNCTION_LIST(
		CE_FUNCTION(OnTextChangedCallback)
		CE_FUNCTION(OnObjectEvent)
		CE_FUNCTION(OnFileAction)
	)
)
CE_RTTI_CLASS_IMPL(, EventTests, ReceiverClass)

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
	class BottomObject : public Object
	{
		CE_CLASS(BottomObject, Object)
	public:

		BottomObject()
		{
			
		}

		void OnSubobjectAttached(Object* subobject) override
		{
			String::IsAlphabet('a');
		}

		FIELD()
		String someString = "bottom";

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

			bottomObject = CreateDefaultSubobject<BottomObject>("BottomMostSubobject");
			bottomObject->someString = "Created from AnotherObject";
		}

		void OnSubobjectAttached(Object* subobject) override
		{
			String::IsAlphabet('a');
		}

		FIELD()
		BottomObject* bottomObject = nullptr;

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

		void OnSubobjectAttached(Object* subobject) override
		{
			String::IsAlphabet('a');
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

		FIELD()
		Vec4 vecValue{};
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

CE_RTTI_CLASS(, CDITests, BottomObject,
	CE_SUPER(Object),
	CE_NOT_ABSTRACT,
	CE_ATTRIBS(),
	CE_FIELD_LIST(
		CE_FIELD(someString)
	),
	CE_FUNCTION_LIST(
	)
)
CE_RTTI_CLASS(, CDITests, AnotherObject,
    CE_SUPER(Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
		CE_FIELD(bottomObject)
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
		CE_FIELD(vecValue)
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

