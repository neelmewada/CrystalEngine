
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

        mINI::INIFile file = mINI::INIFile(filePath.GetString().GetCString());
        mINI::INIStructure structure;
        
        file.read(structure);

        while (field != nullptr)
        {
            const auto& attribs = field->GetAttributes();

            String category = "Default";

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
                field->SetFieldValue<s32>(structInstance, std::stoi(structure[category.GetCString()][field->GetName().GetCString()]));
            }
            else if (fieldType->GetTypeId() == TYPEID(s64))
            {
                field->SetFieldValue<s64>(structInstance, std::stoi(structure[category.GetCString()][field->GetName().GetCString()]));
            }
            else if (fieldType->GetTypeId() == TYPEID(String))
            {
                std::cout << "Value: " << structure[category.GetCString()][field->GetName().GetCString()] << std::endl;
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
