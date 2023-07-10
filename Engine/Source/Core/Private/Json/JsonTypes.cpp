#include "CoreMinimal.h"

namespace CE
{
	JValue::JValue()
	{
		Clear();
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

	JValue::JValue(int numberValue)
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
		if (valueType == JsonValueType::String)
		{
			stringValue.~String();
		}
		else if (valueType == JsonValueType::Object)
		{
			objectValue.~JObject();
		}
		else if (valueType == JsonValueType::Array)
		{
			arrayValue.~JArray();
		}

		memset(this, 0, sizeof(JValue));
	}

	void JValue::Copy(const JValue& copy)
	{
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
