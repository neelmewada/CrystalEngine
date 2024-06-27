#pragma once

namespace CE
{
	class FStyleSet;
	class FusionRenderer;
    class FWidget;
    class FWindow;
    class FPopup;

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

        virtual void DoPaint();

        virtual void TickInput();

        void SetProjectionMatrix(const Matrix4x4& mat) { this->projectionMatrix = mat; }

        void SetOwningWidget(FWidget* widget);

        FWidget* GetOwningWidget() const { return owningWidget; }

        FStyleSet* GetDefaultStyleSet();

        virtual void OnWidgetDestroyed(FWidget* widget);

        Vec2 GetAvailableSize() const { return availableSize; }

        void SetAvailableSize(Vec2 value) { availableSize = value; }

        bool IsLayoutDirty() const { return layoutDirty; }

        virtual bool IsFocused() const;

        bool IsRootContext() const;

        void MarkLayoutDirty();

        void MarkDirty();

        void QueueDestroy();

        void AddChildContext(FFusionContext* context);

        void RemoveChildContext(FFusionContext* context);

        void PushLocalPopup(FPopup* popup, Vec2 globalPosition, Vec2 size = Vec2());
        virtual void PushNativePopup(FPopup* popup, Vec2 globalPosition, Vec2 size = Vec2());
        bool ClosePopup(FPopup* popup);

        void SetDefaultStyleSet(FStyleSet* styleSet);

        bool ShouldClearScreen() const { return clearScreen; }

        void SetClearScreen(bool set);

        void SetClearColor(const Color& color);

        virtual FPainter* GetPainter();

        void SetFocusWidget(FWidget* focusWidget);

        virtual FWidget* HitTest(Vec2 mousePosition);

        // - Rendering / FrameGraph -

        virtual void EmplaceFrameAttachments();

        virtual void EnqueueScopes();

        virtual void SetDrawListMask(RHI::DrawListMask& outMask);

        virtual void EnqueueDrawPackets(RHI::DrawListContext& drawList, u32 imageIndex);

        virtual void SetDrawPackets(RHI::DrawListContext& drawList);

    protected:

        virtual void OnStyleSetDeregistered(FStyleSet* styleSet);

        FIELD()
        Array<FFusionContext*> childContexts{};

        FIELD()
        FFusionContext* parentContext = nullptr;

        //! @brief Widget can be owned by a FusionContext directly, or by a native window!
        FIELD()
        FWidget* owningWidget = nullptr;

        FIELD()
        FStyleSet* defaultStyleSet = nullptr;

        FIELD()
        b8 isIsolatedContext = true;

        FIELD()
        b8 clearScreen = true;

        FIELD()
        Color clearColor = Color::Clear();

        bool layoutDirty = true;
        bool dirty = true;
        bool isDestroyed = false;
        bool isRootContext = false;

        FWidget* curFocusWidget = nullptr;
        FWidget* widgetToFocus = nullptr;

        Vec2 availableSize{};

        Matrix4x4 projectionMatrix = Matrix4x4::Identity();
        RPI::PerViewConstants viewConstants{};

        using FWidgetStack = StableDynamicArray<FWidget*, 128, false>;

        Array<FWidget*> hoveredWidgetStack;

        // Non-native popups that are rendered inside a native window
        Array<FPopup*> localPopupStack;

        FUSION_FRIENDS;
    };
    
} // namespace CE

#include "FFusionContext.rtti.h"