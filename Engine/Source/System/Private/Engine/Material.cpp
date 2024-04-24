#include "System.h"

namespace CE
{

    CE::Material::Material()
    {
        
    }

    CE::Material::~Material()
    {
        if (material)
        {
            delete material;
            material = nullptr;
        }
    }

    void CE::Material::SetShader(CE::Shader* shader)
    {
        if (this->shader == shader)
            return;

        valuesModified = true;

        shaderCollection = shader->GetShaderCollection();

        this->shader = shader;
        auto newMaterial = new RPI::Material(shader->GetShaderCollection());

        if (material)
        {
            newMaterial->CopyPropertiesFrom(material);

            delete material;
            material = nullptr;
        }

        this->material = newMaterial;
    }

    void CE::Material::SetCustomPass(u32 passIndex)
    {
        if (this->passIndex == passIndex || material == nullptr)
            return;

        if (material)
        {
            this->passIndex = passIndex;
            material->SetCustomShaderItem(passIndex);
        }
    }

    RPI::Material* CE::Material::GetRpiMaterial()
    {
        return material;
    }

    void CE::Material::OnAfterDeserialize()
    {
        Super::OnAfterDeserialize();

        valuesModified = true;

        if (shader != nullptr)
        {
            CE::Shader* resetShader = shader;
            shaderCollection = nullptr;
            this->shader = nullptr;

            SetShader(resetShader);
        }

        propertyMap.Clear();

        for (const auto& prop : properties)
        {
            propertyMap[prop.name] = prop;
        }
    }

    HashMap<Name, MaterialProperty> CE::Material::GetAllProperties()
    {
        return propertyMap;
    }

    void CE::Material::SetProperty(const Name& name, u32 value)
    {
        valuesModified = true;

        if (propertyMap.KeyExists(name))
        {
            properties.RemoveFirst([&](const MaterialProperty& prop) { return prop.name == name; });
        }

        MaterialProperty prop{};
        prop.name = name;
        prop.u32Value = value;
        prop.propertyType = MaterialPropertyType::UInt;

        properties.Add(prop);
        propertyMap[name] = prop;
    }

    void CE::Material::SetProperty(const Name& name, s32 value)
    {

        valuesModified = true;
        if (propertyMap.KeyExists(name))
        {
            properties.RemoveFirst([&](const MaterialProperty& prop) { return prop.name == name; });
        }

        MaterialProperty prop{};
        prop.name = name;
        prop.s32Value = value;
        prop.propertyType = MaterialPropertyType::Int;

        properties.Add(prop);
        propertyMap[name] = prop;
    }

    void CE::Material::SetProperty(const Name& name, f32 value)
    {
        valuesModified = true;

        if (propertyMap.KeyExists(name))
        {
            properties.RemoveFirst([&](const MaterialProperty& prop) { return prop.name == name; });
        }

        MaterialProperty prop{};
        prop.name = name;
        prop.floatValue = value;
        prop.propertyType = MaterialPropertyType::Float;

        properties.Add(prop);
        propertyMap[name] = prop;
    }

    void CE::Material::SetProperty(const Name& name, Vec4 value)
    {
        valuesModified = true;

        if (propertyMap.KeyExists(name))
        {
            properties.RemoveFirst([&](const MaterialProperty& prop) { return prop.name == name; });
        }

        MaterialProperty prop{};
        prop.name = name;
        prop.vectorValue = value;
        prop.propertyType = MaterialPropertyType::Vector;

        properties.Add(prop);
        propertyMap[name] = prop;
    }

    void CE::Material::SetProperty(const Name& name, Color value)
    {
        valuesModified = true;

        if (propertyMap.KeyExists(name))
        {
            properties.RemoveFirst([&](const MaterialProperty& prop) { return prop.name == name; });
        }

        MaterialProperty prop{};
        prop.name = name;
        prop.colorValue = value;
        prop.propertyType = MaterialPropertyType::Color;

        properties.Add(prop);
        propertyMap[name] = prop;
    }

    void CE::Material::SetProperty(const Name& name, const Matrix4x4& value)
    {
        valuesModified = true;

        if (propertyMap.KeyExists(name))
        {
            properties.RemoveFirst([&](const MaterialProperty& prop) { return prop.name == name; });
        }

        MaterialProperty prop{};
        prop.name = name;
        prop.matrixValue = value;
        prop.propertyType = MaterialPropertyType::Matrix;

        properties.Add(prop);
        propertyMap[name] = prop;
    }

    void CE::Material::SetProperty(const Name& name, CE::Texture* value, const Vec2& offset, const Vec2& scaling)
    {
        valuesModified = true;

        if (propertyMap.KeyExists(name))
        {
            properties.RemoveFirst([&](const MaterialProperty& prop) { return prop.name == name; });
        }

        MaterialProperty prop{};
        prop.name = name;
        prop.textureValue.texture = value;
        prop.textureValue.offset = offset;
        prop.textureValue.scaling = scaling;
        prop.propertyType = MaterialPropertyType::Texture;

        properties.Add(prop);
        propertyMap[name] = prop;
    }

    void CE::Material::ApplyProperties()
    {
        if (material == nullptr)
        {
            CE_LOG(Error, All, "SetProperty() called on a material without a shader!");
            return;
        }

        if (!valuesModified)
            return;

        for (const auto& prop : properties)
        {
            switch (prop.propertyType)
            {
            case MaterialPropertyType::UInt:
                material->SetPropertyValue(prop.name, prop.u32Value);
                break;
            case MaterialPropertyType::Int:
                material->SetPropertyValue(prop.name, prop.s32Value);
                break;
            case MaterialPropertyType::Float:
                material->SetPropertyValue(prop.name, prop.floatValue);
                break;
            case MaterialPropertyType::Vector:
                material->SetPropertyValue(prop.name, prop.vectorValue);
                break;
            case MaterialPropertyType::Color:
                material->SetPropertyValue(prop.name, prop.colorValue);
                break;
            case MaterialPropertyType::Matrix:
                material->SetPropertyValue(prop.name, prop.matrixValue);
                break;
            case MaterialPropertyType::Texture:
                if (prop.textureValue.texture != nullptr)
                {
                    material->SetPropertyValue(prop.name, prop.textureValue.texture->GetRpiTexture());
                    material->SetPropertyValue(prop.name.GetString() + "Transform", Vec4(prop.textureValue.offset, prop.textureValue.scaling));
                }
                break;
            }
        }

        valuesModified = false;
    }
    
} // namespace CE
