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

        if (material)
        {
            delete material;
            material = nullptr;
        }

        this->shader = shader;
    }

    void CE::Material::SetProperty(const Name& name, u32 value)
    {
        material->SetPropertyValue(name, RPI::MaterialPropertyValue(value));
    }

    void CE::Material::SetProperty(const Name& name, s32 value)
    {
        material->SetPropertyValue(name, RPI::MaterialPropertyValue(value));
    }

    void CE::Material::SetProperty(const Name& name, f32 value)
    {
        material->SetPropertyValue(name, RPI::MaterialPropertyValue(value));
    }

    void CE::Material::SetProperty(const Name& name, Vec4 value)
    {
        material->SetPropertyValue(name, RPI::MaterialPropertyValue(value));
    }

    void CE::Material::SetProperty(const Name& name, Color value)
    {
        material->SetPropertyValue(name, RPI::MaterialPropertyValue(value));
    }

    void CE::Material::SetProperty(const Name& name, const Matrix4x4& value)
    {
        material->SetPropertyValue(name, RPI::MaterialPropertyValue(value));
    }

    void CE::Material::SetProperty(const Name& name, CE::Texture* value)
    {

    }
    
} // namespace CE
