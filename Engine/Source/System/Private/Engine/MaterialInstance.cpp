#include "System.h"

namespace CE
{

    MaterialInstance::~MaterialInstance()
    {
        if (materialOverride)
        {
            delete materialOverride;
        }
    }

    void MaterialInstance::SetProperty(const Name& name, u32 value)
    {
        if (material == nullptr)
        {
            CE_LOG(Error, All, "SetProperty() called on a material without a shader!");
            return;
        }
        materialOverride->SetPropertyValue(name, RPI::MaterialPropertyValue(value));
    }

    void MaterialInstance::SetProperty(const Name& name, s32 value)
    {

    }

    void MaterialInstance::SetProperty(const Name& name, f32 value)
    {
    }

    void MaterialInstance::SetProperty(const Name& name, Vec4 value)
    {
    }

    void MaterialInstance::SetProperty(const Name& name, Color value)
    {
    }

    void MaterialInstance::SetProperty(const Name& name, const Matrix4x4& value)
    {
    }

    void MaterialInstance::SetProperty(const Name& name, CE::Texture* value)
    {
    }

    RPI::Material* MaterialInstance::GetRpiMaterial()
    {
        return nullptr;
    }

} // namespace CE
