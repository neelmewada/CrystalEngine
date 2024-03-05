#pragma once

namespace CE
{

    CLASS(Abstract)
    class SYSTEM_API MaterialInterface : public Object
    {
        CE_CLASS(MaterialInterface, Object)
    public:

        MaterialInterface();
        virtual ~MaterialInterface();

        virtual void SetProperty(const Name& name, u32 value) = 0;

        virtual void SetProperty(const Name& name, s32 value) = 0;

        virtual void SetProperty(const Name& name, f32 value) = 0;

        virtual void SetProperty(const Name& name, Vec4 value) = 0;

        virtual void SetProperty(const Name& name, Color value) = 0;

        virtual void SetProperty(const Name& name, const Matrix4x4& value) = 0;

        virtual void SetProperty(const Name& name, CE::Texture* value) = 0;

        virtual void SetPass(u32 passIndex) = 0;

        virtual RPI::Material* GetRpiMaterial() = 0;

    protected:

        u32 passIndex = 0;

    };

} // namespace CE

#include "MaterialInterface.rtti.h"
