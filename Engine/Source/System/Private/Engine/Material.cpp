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

        this->shader = shader;
        auto newMaterial = new RPI::Material(shader->GetOrCreateRPIShader(passIndex));

        if (material)
        {
            newMaterial->CopyPropertiesFrom(material);

            delete material;
            material = nullptr;
        }

        this->material = newMaterial;
    }

    void CE::Material::SetPass(u32 passIndex)
    {
        if (this->passIndex == passIndex)
            return;

        this->passIndex = passIndex;

        auto newMaterial = new RPI::Material(shader->GetOrCreateRPIShader(passIndex));

        if (material)
        {
            newMaterial->CopyPropertiesFrom(material);

            delete material;
            material = nullptr;
        }
        
        this->material = newMaterial;
    }

    RPI::Material* CE::Material::GetRpiMaterial()
    {
        return material;
    }

    void CE::Material::SetProperty(const Name& name, u32 value)
    {
        if (material == nullptr)
        {
            CE_LOG(Error, All, "SetProperty() called on a material without a shader!");
            return;
        }
        material->SetPropertyValue(name, RPI::MaterialPropertyValue(value));
    }

    void CE::Material::SetProperty(const Name& name, s32 value)
    {
        if (material == nullptr)
        {
            CE_LOG(Error, All, "SetProperty() called on a material without a shader!");
            return;
        }
        material->SetPropertyValue(name, RPI::MaterialPropertyValue(value));
    }

    void CE::Material::SetProperty(const Name& name, f32 value)
    {
        if (material == nullptr)
        {
            CE_LOG(Error, All, "SetProperty() called on a material without a shader!");
            return;
        }
        material->SetPropertyValue(name, RPI::MaterialPropertyValue(value));
    }

    void CE::Material::SetProperty(const Name& name, Vec4 value)
    {
        if (material == nullptr)
        {
            CE_LOG(Error, All, "SetProperty() called on a material without a shader!");
            return;
        }
        material->SetPropertyValue(name, RPI::MaterialPropertyValue(value));
    }

    void CE::Material::SetProperty(const Name& name, Color value)
    {
        if (material == nullptr)
        {
            CE_LOG(Error, All, "SetProperty() called on a material without a shader!");
            return;
        }
        material->SetPropertyValue(name, RPI::MaterialPropertyValue(value));
    }

    void CE::Material::SetProperty(const Name& name, const Matrix4x4& value)
    {
        if (material == nullptr)
        {
            CE_LOG(Error, All, "SetProperty() called on a material without a shader!");
            return;
        }
        material->SetPropertyValue(name, RPI::MaterialPropertyValue(value));
    }

    void CE::Material::SetProperty(const Name& name, CE::Texture* value)
    {
        if (material == nullptr)
        {
            CE_LOG(Error, All, "SetProperty() called on a material without a shader!");
            return;
        }
        material->SetPropertyValue(name, value->GetRpiTexture());
    }
    
} // namespace CE
