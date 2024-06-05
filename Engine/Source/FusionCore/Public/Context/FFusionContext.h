#pragma once

namespace CE
{
    class FusionRenderer;
    class FWidget;
    class FWindow;

    CLASS()
    class FUSIONCORE_API FFusionContext : public Object
    {
        CE_CLASS(FFusionContext, Object)
    public:

        FFusionContext();

        virtual ~FFusionContext();

        bool IsIsolatedContext() const { return isIsolatedContext; }

        bool IsNativeContext() const;

        virtual void Tick();

        void SetProjectionMatrix(const Matrix4x4& mat) { this->projectionMatrix = mat; }

    protected:

        FIELD()
        Array<FFusionContext*> childContexts{};

        //! @brief Widget can be owned by a FusionContext directly, or by a native window!
        FIELD()
        FWidget* owningWidget = nullptr;

        FIELD()
        b8 isIsolatedContext = true;

        FIELD()
        FLayoutManager* layoutManager = nullptr;

        Matrix4x4 projectionMatrix = Matrix4x4::Identity();
        RPI::PerViewConstants viewConstants{};
        Matrix4x4 rootTransform{};

        FUSION_FRIENDS;
    };
    
} // namespace CE

#include "FFusionContext.rtti.h"