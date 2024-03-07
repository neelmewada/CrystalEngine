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

        virtual void SetProperty(const Name& name, CE::Texture* value, const Vec2& offset = Vec2(0, 0), const Vec2& scaling = Vec2(1, 1)) override;

        virtual void ApplyProperties() override;

        virtual void SetPass(u32 passIndex) override;

        virtual RPI::Material* GetRpiMaterial() override;

        virtual void OnAfterDeserialize() override;

        virtual CE::Shader* GetShader() override
        {
            return shader;
        }

    private:

        virtual HashMap<Name, MaterialProperty> GetAllProperties() override;

        RPI::Material* material = nullptr;

        FIELD()
        CE::Shader* shader = nullptr;

        FIELD()
        Array<MaterialProperty> properties{};

        bool valuesModified = true;

        HashMap<Name, MaterialProperty> propertyMap{};

        friend class MaterialInstance;
    };

} // namespace CE

#include "Material.rtti.h"
