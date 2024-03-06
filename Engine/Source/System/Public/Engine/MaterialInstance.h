#pragma once

namespace CE
{

    CLASS(Abstract)
    class SYSTEM_API MaterialInstance : public MaterialInterface
    {
        CE_CLASS(MaterialInstance, MaterialInterface)
    public:

        ~MaterialInstance();

        virtual void SetProperty(const Name& name, u32 value) override;

        virtual void SetProperty(const Name& name, s32 value) override;

        virtual void SetProperty(const Name& name, f32 value) override;

        virtual void SetProperty(const Name& name, Vec4 value) override;

        virtual void SetProperty(const Name& name, Color value) override;

        virtual void SetProperty(const Name& name, const Matrix4x4& value) override;

        virtual void SetProperty(const Name& name, CE::Texture* value) override;

        virtual RPI::Material* GetRpiMaterial() override;

    private:

        FIELD()
        CE::Material* material = nullptr;

        RPI::Material* materialOverride = nullptr;

    };
    
} // namespace CE

#include "MaterialInstance.rtti.h"