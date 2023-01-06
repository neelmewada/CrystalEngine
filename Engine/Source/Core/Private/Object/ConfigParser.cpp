
#include "Object/ConfigParser.h"

#include "mini/ini.h"

#include <string>
#include <iostream>

namespace CE
{
    
    ConfigParser::ConfigParser(StructType* structType) : structType(structType)
    {

    }

    void ConfigParser::Parse(void* structInstance, CE::IO::Path filePath)
    {
        auto field = structType->GetFirstField();

        String filePathStr = filePath.GetString();
        mINI::INIFile file = mINI::INIFile(filePathStr.GetCString());
        mINI::INIStructure structure;

        if (!file.read(structure))
        {
            CE_LOG(Error, All, "Failed to parse config file: {}", filePath);
            return;
        }

        while (field != nullptr)
        {
            const auto& attribs = field->GetAttributes();

            String category = "General";

            for (int i = 0; i < attribs.GetSize(); i++)
            {
                if (attribs[i].GetKey() == "Category" || attribs[i].GetKey() == "category")
                {
                    category = attribs[i].GetValue();
                }
            }

            auto fieldType = field->GetDeclarationType();

            if (fieldType->GetTypeId() == TYPEID(u8))
            {
                field->SetFieldValue<u8>(structInstance, std::stoi(structure[category.GetCString()][field->GetName().GetCString()]));
            }
            else if (fieldType->GetTypeId() == TYPEID(u16))
            {
                field->SetFieldValue<u16>(structInstance, std::stoi(structure[category.GetCString()][field->GetName().GetCString()]));
            }
            else if (fieldType->GetTypeId() == TYPEID(u32))
            {
                field->SetFieldValue<u32>(structInstance, std::stoi(structure[category.GetCString()][field->GetName().GetCString()]));
            }
            else if (fieldType->GetTypeId() == TYPEID(u64))
            {
                field->SetFieldValue<u64>(structInstance, std::stoi(structure[category.GetCString()][field->GetName().GetCString()]));
            }
            else if (fieldType->GetTypeId() == TYPEID(s8))
            {
                field->SetFieldValue<s8>(structInstance, std::stoi(structure[category.GetCString()][field->GetName().GetCString()]));
            }
            else if (fieldType->GetTypeId() == TYPEID(s16))
            {
                field->SetFieldValue<s16>(structInstance, std::stoi(structure[category.GetCString()][field->GetName().GetCString()]));
            }
            else if (fieldType->GetTypeId() == TYPEID(s32))
            {
                auto v = structure[category.GetCString()][field->GetName().GetCString()];
                field->SetFieldValue<s32>(structInstance, std::stoi(v));
            }
            else if (fieldType->GetTypeId() == TYPEID(s64))
            {
                field->SetFieldValue<s64>(structInstance, std::stoi(structure[category.GetCString()][field->GetName().GetCString()]));
            }
            else if (fieldType->GetTypeId() == TYPEID(String))
            {
                field->SetFieldValue<String>(structInstance, String(structure[category.GetCString()][field->GetName().GetCString()]));
            }
            else if (fieldType->GetTypeId() == TYPEID(f32))
            {
                field->SetFieldValue<f32>(structInstance, std::stof(structure[category.GetCString()][field->GetName().GetCString()]));
            }
            else if (fieldType->GetTypeId() == TYPEID(f64))
            {
                field->SetFieldValue<f64>(structInstance, std::stof(structure[category.GetCString()][field->GetName().GetCString()]));
            }
            else if (fieldType->GetTypeId() == TYPEID(bool))
            {
                field->SetFieldValue<bool>(structInstance, (bool)std::stoi(structure[category.GetCString()][field->GetName().GetCString()]));
            }

            field = field->GetNext();
        }
    }

} // namespace CE