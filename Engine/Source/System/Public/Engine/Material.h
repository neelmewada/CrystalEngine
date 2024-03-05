#pragma once

namespace CE
{
    CLASS()
    class SYSTEM_API Material : public MaterialInterface
    {
        CE_CLASS(Material, MaterialInterface)
    public:

        Material();
        virtual ~Material();

        void SetShader(CE::Shader* shader);

        virtual void SetProperty(const Name& name, u32 value) override;

        virtual void SetProperty(const Name& name, s32 value) override;

        virtual void SetProperty(const Name& name, f32 value) override;

        virtual void SetProperty(const Name& name, Vec4 value) override;

        virtual void SetProperty(const Name& name, Color value) override;

        virtual void SetProperty(const Name& name, const Matrix4x4& value) override;

        virtual void SetProperty(const Name& name, CE::Texture* value) override;

        virtual void SetPass(u32 passIndex) override;

        virtual RPI::Material* GetRpiMaterial() override;

    private:

        RPI::Material* material = nullptr;

        FIELD()
        CE::Shader* shader = nullptr;

        friend class MaterialInstance;
    };

} // namespace CE

#include "Material.rtti.h"
