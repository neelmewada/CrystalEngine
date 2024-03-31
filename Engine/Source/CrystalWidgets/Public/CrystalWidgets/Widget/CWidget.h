#pragma once

namespace CE::Widgets
{
	class CPainter;
	class CWindow;

    CLASS(Config = Engine)
    class CRYSTALWIDGETS_API CWidget : public Object
    {
        CE_CLASS(CWidget, Object)
    public:

        CWidget();
        virtual ~CWidget();

        inline bool IsEnabled() const { return enabled; }

        inline bool IsVisible() const { return visible; }

        bool IsInteractable() const;

        virtual bool IsLayoutCalculationRoot() { return IsWindow(); }

        virtual bool CanPaint() const { return true; }

        virtual bool IsSubWidgetAllowed(Class* subWidgetClass);

        bool IsWindow();

        bool IsFocussed() const { return EnumHasFlag(stateFlags, CStateFlag::Focused); }

        void SetVisible(bool visible);
        void SetEnabled(bool enabled);
        void SetInteractable(bool interactable);

        void SetNeedsPaint();

        void SetNeedsLayout();
        void ClearNeedsLayout();

        void SetNeedsStyle();

        bool NeedsLayout();

        bool NeedsPaint();

        bool NeedsStyle();

        void AddSubWidget(CWidget* widget);
        void RemoveSubWidget(CWidget* widget);

        u32 GetSubWidgetCount() const { return attachedWidgets.GetSize(); }
        CWidget* GetSubWidget(u32 index) const { return attachedWidgets[index]; }

        u32 GetSubWidgetIndex(CWidget* widget) const { return attachedWidgets.IndexOf(widget); }

        bool SubWidgetExistsRecursive(CWidget* subWidget);

        bool ParentWidgetExistsRecursive(CWidget* widget);

        bool StyleClassExists(const Name& name) const { return styleClasses.Exists(name); }

        void UpdateStyleIfNeeded();
        virtual void UpdateLayoutIfNeeded();

        virtual void OnAfterUpdateLayout() {}

        virtual Vec2 CalculateIntrinsicSize(f32 width, f32 height) { return Vec2(); }

        // - Style API -

        const CStyle& GetComputedStyle()
        {
            return computedStyle;
        }

        u32 GetStyleClassesCount() const
        {
            return styleClasses.GetSize();
        }

        const Name& GetStyleClass(u32 index) const
        {
            return styleClasses[index];
        }

        void AddStyleClass(const String& styleClass)
        {
            if (!StyleClassExists(styleClass))
            {
                styleClasses.Add(styleClass);
                SetNeedsStyle();
                SetNeedsLayout();
            }
        }

        void AddStyleClasses(const Array<String>& styleClasses)
        {
            for (const auto& styleClass : styleClasses)
            {
                AddStyleClass(styleClass);
            }
        }

        void RemoveStyleClass(const String& styleClass)
        {
            if (styleClasses.Remove(styleClass))
            {
                SetNeedsStyle();
                SetNeedsLayout();
            }
        }

        bool StyleClassExists(const String& styleClass) const
        {
            return styleClasses.Exists(styleClass);
        }

        virtual Vec2 GetComputedLayoutTopLeft() { return Vec2(YGNodeLayoutGetLeft(node), YGNodeLayoutGetTop(node)); }
        virtual Vec2 GetComputedLayoutSize() { return Vec2(YGNodeLayoutGetWidth(node), YGNodeLayoutGetHeight(node)); }

        Vec4 GetComputedLayoutPadding() {
            return Vec4(YGNodeLayoutGetPadding(node, YGEdgeLeft),
                YGNodeLayoutGetPadding(node, YGEdgeTop),
                YGNodeLayoutGetPadding(node, YGEdgeRight),
                YGNodeLayoutGetPadding(node, YGEdgeBottom));
        }

        Rect GetScreenSpaceRect();
        Rect LocalToScreenSpaceRect(const Rect& localRect);
        Rect LocalToWindowSpaceRect(const Rect& localRect);
        Vec2 ScreenSpaceToLocalPoint(const Vec2& point);

        Renderer2D* GetRenderer();

        PlatformWindow* GetNativeWindow();

        void QueueDestroy();

        Vec2 CalculateTextSize(const String& text, f32 fontSize, Name fontName, f32 width);

        void Focus();
        void Unfocus();

        // - Signals -

        CE_SIGNAL(OnFocused);

        CE_SIGNAL(OnUnfocused);

    crystalwidgets_protected_internal:

        virtual void OnFocusGained() {}
        virtual void OnFocusLost() {}

        void SetNeedsPaintRecursively(bool newValue = false);

        virtual void Construct();

        void OnAfterConstruct() override final;

        virtual void OnPaint(CPaintEvent* paintEvent);
        virtual void OnPaintOverlay(CPaintEvent* paintEvent);

        void OnSubobjectDetached(Object* object) override;
        void OnSubobjectAttached(Object* object) override;

        virtual void HandleEvent(CEvent* event);

        FIELD()
        Array<CWidget*> attachedWidgets{};

        FIELD()
        Vec4 rootPadding = Vec4();

        FIELD()
        CWidget* parent = nullptr;

        FIELD()
        CWindow* ownerWindow = nullptr;

        FIELD()
        Vec2 rootOrigin = Vec2(); // Origin position based on window space coordinates

        FIELD()
        b8 enabled = true;

        FIELD()
		b8 visible = true;

        FIELD()
        b8 interactable = true;

        FIELD()
        b8 needsPaint = true;

        FIELD()
        b8 needsLayout = true;

        FIELD()
        b8 needsStyle = true;

        FIELD(ReadOnly)
        b8 isPressed = false;

        FIELD()
		CStateFlag stateFlags{};

		FIELD()
		CSubControl subControl = CSubControl::None;

        FIELD()
        b8 receiveMouseEvents = true;

        FIELD()
        b8 receiveDragEvents = false;

        FIELD()
        b8 receiveKeyEvents = true;

        FIELD()
        b8 clipChildren = false;

        FIELD()
        Array<Name> styleClasses{};

        FIELD()
        CStyleSheet* styleSheet = nullptr;

        FIELD()
		b8 allowHorizontalScroll = false;

		FIELD()
		b8 allowVerticalScroll = false;

        FIELD()
        Vec2 normalizedScroll{};

        FIELD(ReadOnly)
        Vec2 contentSize{};

        // - Configs -

        FIELD(Config)
        f32 scrollSensitivity = 1.0f;

    crystalwidgets_internal:

        void ClearChildNodes();
        void ReAddChildNodes();

        YGNodeRef node = nullptr;
        YGNodeRef detachedNode = nullptr;

        CStyle computedStyle{};
        CCursor hoverCursor = CCursor::Inherited;

        b8 isMouseInside = false;

        int destroyFrameCounter = 0;
        b8 isQueuedForDestruction = false;

    protected:

        virtual void OnBeforeDestroy() override;

        static YGSize MeasureFunctionCallback(YGNodeConstRef node,
            float width,
            YGMeasureMode widthMode,
            float height,
            YGMeasureMode heightMode);

    };

} // namespace CE::Widgets

#include "CWidget.rtti.h"
