#include "CoreMinimal.h"

namespace CE
{
	BasePrefs::BasePrefs()
	{

	}

	BasePrefs::~BasePrefs()
	{

	}

	bool BasePrefs::KeyExists(const String& key)
	{
		return data.is_object() && data.find(key.ToStdString()) != data.end();
	}

	bool BasePrefs::GetStringValue(const String& key, String& outString)
	{
		std::string keyStr = key.ToStdString();

		if (!KeyExists(key))
			return false;
		if (!data[keyStr].is_string())
			return false;

		outString = data[keyStr].get<std::string>();
		return true;
	}

	bool BasePrefs::GetNumberValue(const String& key, f32& outNumber)
	{
		std::string keyStr = key.ToStdString();

		if (!KeyExists(key))
			return false;
		if (!data[keyStr].is_number())
			return false;

		outNumber = data[keyStr].get<f32>();
		return true;
	}

	bool BasePrefs::GetBooleanValue(const String& key, bool& outBoolean)
	{
		std::string keyStr = key.ToStdString();

		if (!KeyExists(key))
			return false;
		if (!data[keyStr].is_boolean())
			return false;

		outBoolean = data[keyStr].get<bool>();
		return true;
	}

	bool BasePrefs::IsNullValue(const String& key)
	{
		std::string keyStr = key.ToStdString();
		if (!KeyExists(key))
			return false;

		return data[keyStr].is_null();
	}

	bool BasePrefs::IsObjectValue(const String& key)
	{
		std::string keyStr = key.ToStdString();

		if (!KeyExists(key))
			return false;
		return data[keyStr].is_object();
	}

	bool BasePrefs::IsArrayValue(const String& key)
	{
		std::string keyStr = key.ToStdString();
		if (!KeyExists(key))
			return false;
		return data[keyStr].is_array();
	}

	JsonValueType BasePrefs::GetValueType(const String& key)
	{
		std::string keyStr = key.ToStdString();
		if (!KeyExists(key))
			return JsonValueType::None;

		if (data[keyStr].is_number())
			return JsonValueType::Number;
		else if (data[keyStr].is_boolean())
			return JsonValueType::Boolean;
		else if (data[keyStr].is_string())
			return JsonValueType::String;
		else if (data[keyStr].is_array())
			return JsonValueType::Array;
		else if (data[keyStr].is_object())
			return JsonValueType::Object;
		else if (data[keyStr].is_null())
			return JsonValueType::Null;
		
		return JsonValueType::None;
	}

	bool BasePrefs::GetStruct(const String& key, StructType* type, void* instance)
	{
		

		return true;
	}

	bool BasePrefs::SetStruct(const String& key, StructType* type, void* instance)
	{
		auto field = type->GetFirstField();
		auto keyStr = key.ToStdString();

		while (field != nullptr)
		{
			WriteField(data[keyStr], field, instance);

			field = field->GetNext();
		}

		return true;
	}

	void BasePrefs::ClearPrefs()
	{
		SavePrefs();
	}

	void BasePrefs::ReadField(const json& jsonData, FieldType* field, void* instance)
	{
		if (field == nullptr || instance == nullptr)
			return;


	}

	void BasePrefs::WriteField(json& jsonData, FieldType* field, void* instance)
	{
		if (field == nullptr || instance == nullptr)
			return;

		auto fieldDeclType = field->GetDeclarationType();
		auto fieldTypeId = field->GetDeclarationTypeId();

		auto fieldName = field->GetName().GetString().ToStdString();

		if (field->IsIntegerField())
		{
			if (fieldTypeId == TYPEID(u8))
				jsonData[fieldName] = field->GetFieldValue<u8>(instance);
			else if (fieldTypeId == TYPEID(u16))
				jsonData[fieldName] = field->GetFieldValue<u16>(instance);
			else if (fieldTypeId == TYPEID(u32))
				jsonData[fieldName] = field->GetFieldValue<u32>(instance);
			else if (fieldTypeId == TYPEID(u64))
				jsonData[fieldName] = field->GetFieldValue<u64>(instance);
			else if (fieldTypeId == TYPEID(s8))
				jsonData[fieldName] = field->GetFieldValue<u8>(instance);
			else if (fieldTypeId == TYPEID(s16))
				jsonData[fieldName] = field->GetFieldValue<s16>(instance);
			else if (fieldTypeId == TYPEID(s32))
				jsonData[fieldName] = field->GetFieldValue<s32>(instance);
			else if (fieldTypeId == TYPEID(s64))
				jsonData[fieldName] = field->GetFieldValue<s64>(instance);
		}
		else if (field->IsDecimalField())
		{
			if (fieldTypeId == TYPEID(f32))
				jsonData[fieldName] = field->GetFieldValue<f32>(instance);
			else if (fieldTypeId == TYPEID(f64))
				jsonData[fieldName] = field->GetFieldValue<f64>(instance);
		}
		else if (fieldTypeId == TYPEID(bool))
		{
			jsonData[fieldName] = field->GetFieldValue<bool>(instance);
		}
	}

	void BasePrefs::LoadPrefs(const IO::Path& prefsPath)
	{
		if (!prefsPath.Exists())
		{
			data = json::parse("{}");
			return;
		}

		std::ifstream stream(prefsPath);
		data = json::parse(stream);
	}

	void BasePrefs::SavePrefs(const IO::Path& prefsPath)
	{
		if (prefsPath.Exists())
		{
			IO::Path::Remove(prefsPath);
		}

		FileStream fileStream = FileStream(prefsPath, Stream::Permissions::WriteOnly, false);
		fileStream.SetAsciiMode(true);
		std::string jsonString = data.dump(4);

		std::ofstream stream(prefsPath);
		stream << jsonString;
		stream << "\n";
	}

	void BasePrefs::ClearPrefs(const IO::Path& prefsPath)
	{
		data = json::parse("{}");
		SavePrefs(prefsPath);
	}

} // namespace CE

