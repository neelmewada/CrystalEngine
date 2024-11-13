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

        FFusionContext* GetRootContext() const;

        void SetProjectionMatrix(const Matrix4x4& mat) { this->projectionMatrix = mat; }

        void SetOwningWidget(FWidget* widget);

        FWidget* GetOwningWidget() const { return owningWidget; }

        FStyleSet* GetDefaultStyleSet();

        virtual void OnWidgetDestroyed(FWidget* widget);

        Vec2 GetAvailableSize() const { return availableSize; }

        void SetAvailableSize(Vec2 value) { availableSize = value; }

        bool IsLayoutDirty() const { return layoutDirty; }

        FFusionContext* GetParentContext() const { return parentContext; }

        bool ParentContextExistsRecursive(FFusionContext* parent) const;

        virtual bool IsFocused() const;
        virtual bool IsShown() const;

        bool IsRootContext() const;

        void MarkLayoutDirty();

        void MarkDirty();

        void QueueDestroy();

        const auto& GetChildContexts() const { return childContexts; }

        void AddChildContext(FFusionContext* context);

        void RemoveChildContext(FFusionContext* context);

        void PushLocalPopup(FPopup* popup, Vec2 globalPosition, Vec2 size = Vec2(), Vec2 controlSize = Vec2());
        virtual void PushNativePopup(FPopup* popup, Vec2 globalPosition, Vec2 size = Vec2());
        bool ClosePopup(FPopup* popup);

        void SetDefaultStyleSet(FStyleSet* styleSet);

        bool ShouldClearScreen() const { return clearScreen; }

        void SetClearScreen(bool set);

        void SetClearColor(const Color& color);

        virtual FPainter* GetPainter();

        void SetFocusWidget(FWidget* focusWidget);

        bool IsPopupWindow() const;

        virtual Vec2 GlobalToScreenSpacePosition(Vec2 pos);
        virtual Vec2 ScreenToGlobalSpacePosition(Vec2 pos);

        virtual void OnQueuedDestroy();

        //! @brief Performs a hit-test and returns the bottom-most widget that is under the mouse position.
        //! @param mousePosition The position of mouse in context-space coordinates. i.e. native window space coords for FNativeContext
        virtual FWidget* HitTest(Vec2 mousePosition);

        // - Rendering / FrameGraph -

        virtual void EmplaceFrameAttachments();

        virtual void EnqueueScopes();

        virtual void SetDrawListMask(RHI::DrawListMask& outMask);

        virtual void EnqueueDrawPackets(RHI::DrawListContext& drawList, u32 imageIndex);

        virtual void SetDrawPackets(RHI::DrawListContext& drawList);

    protected:

        virtual void OnStyleSetDeregistered(FStyleSet* styleSet);

        virtual void NotifyWindowEvent(FEventType eventType, FNativeContext* nativeContext);

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

        Array<FWidget*> hoveredWidgetStack;

        KeyModifier keyModifierStates{};
        BitSet<128> keyPressStates{};

        FWidget* draggedWidget = nullptr;
        FWidget* prevHoveredWidget = nullptr;
        StaticArray<FWidget*, 6> widgetsPressedPerMouseButton{};

        // Previous mouse position in window space
        Vec2 prevMousePos = Vec2();

        Vec2 prevScreenMousePos = Vec2();

        // Non-native popups that are rendered inside a native window
        Array<FPopup*> localPopupStack;

        FUSION_FRIENDS;
    };
    
} // namespace CE

#include "FFusionContext.rtti.h"