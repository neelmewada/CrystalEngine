#include "CoreMinimal.h"

namespace CE
{
	JValue::JValue()
	{
		valueType = JsonValueType::Null;
	}

	JValue::JValue(std::nullptr_t nullValue)
	{
		valueType = JsonValueType::Null;
	}

	JValue::JValue(const String& string)
		: valueType(JsonValueType::String)
		, stringValue(string)
	{

	}

	JValue::JValue(const char* string)
		: valueType(JsonValueType::String)
		, stringValue(string)
	{
	}

	JValue::JValue(bool boolValue)
		: valueType(JsonValueType::Boolean)
		, boolValue(boolValue)
	{

	}

	JValue::JValue(f64 numberValue)
		: valueType(JsonValueType::Number)
		, numberValue(numberValue)
	{

	}

	JValue::JValue(s32 numberValue)
		: valueType(JsonValueType::Number)
		, numberValue(numberValue)
	{
	}

	JValue::JValue(s64 numberValue)
		: valueType(JsonValueType::Number)
		, numberValue(numberValue)
	{
	}

	JValue::JValue(JObject jsonObject)
		: valueType(JsonValueType::Object)
		, objectValue(jsonObject)
	{

	}

	JValue::JValue(JArray jsonArray)
		: valueType(JsonValueType::Array)
		, arrayValue(jsonArray)
	{

	}

	JValue::~JValue()
	{
		Clear();
	}

	void JValue::Clear()
	{
		
	}

	void JValue::Copy(const JValue& copy)
	{
		if (valueType != copy.valueType)
			Clear();

		valueType = copy.valueType;
		if (IsNumberValue())
		{
			numberValue = copy.numberValue;
		}
		else if (IsBoolValue())
		{
			boolValue = copy.boolValue;
		}
		else if (IsStringValue())
		{
			stringValue = copy.stringValue;
		}
		else if (IsArrayValue())
		{
			arrayValue = copy.arrayValue;
		}
		else if (IsObjectValue())
		{
			objectValue = copy.objectValue;
		}
	}

} // namespace CE
