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

        virtual void DoLayout();

        void SetProjectionMatrix(const Matrix4x4& mat) { this->projectionMatrix = mat; }

        void SetOwningWidget(FWidget* widget);

        FWidget* GetOwningWidget() const { return owningWidget; }

        void OnWidgetDestroyed(FWidget* widget);

        Vec2 GetAvailableSize() const { return availableSize; }

        void SetAvailableSize(Vec2 value) { availableSize = value; }

        bool IsLayoutDirty() const { return layoutDirty; }

        void MarkLayoutDirty();

        void MarkDirty();

        void QueueDestroy();

        void AddChildContext(FFusionContext* context);

        void RemoveChildContext(FFusionContext* context);

        void SetStyleManager(FStyleManager* styleManager);

        FStyleManager* GetStyleManager();

        bool ShouldClearScreen() const { return clearScreen; }

        void SetClearScreen(bool set);

        // - Rendering / FrameGraph -

        virtual void EmplaceFrameAttachments();

        virtual void EnqueueScopes();

        virtual void SetDrawListMask(RHI::DrawListMask& outMask);

        virtual void EnqueueDrawPackets(RHI::DrawListContext& drawList, u32 imageIndex);

        virtual void SetDrawPackets(RHI::DrawListContext& drawList);

    protected:

        FIELD()
        Array<FFusionContext*> childContexts{};

        FIELD()
        FFusionContext* parentContext = nullptr;

        //! @brief Widget can be owned by a FusionContext directly, or by a native window!
        FIELD()
        FWidget* owningWidget = nullptr;

        FIELD()
        FStyleManager* styleManager = nullptr;

        FIELD()
        b8 isIsolatedContext = true;

        FIELD()
        b8 clearScreen = true;

        bool layoutDirty = true;
        bool dirty = true;
        bool isDestroyed = false;

        Vec2 availableSize{};

        Matrix4x4 projectionMatrix = Matrix4x4::Identity();
        RPI::PerViewConstants viewConstants{};
        Matrix4x4 rootTransform{};

        FUSION_FRIENDS;
    };
    
} // namespace CE

#include "FFusionContext.rtti.h"