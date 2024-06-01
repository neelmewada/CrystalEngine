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

        bool IsEnabled() const;

        bool IsSelfEnabled() const { return enabled; }

        bool IsVisible() const;

        bool IsSelfVisible() const { return visible; }

        bool IsInteractable() const;

        virtual bool IsLayoutCalculationRoot() { return IsWindow(); }

        virtual bool CanPaint() const { return true; }

        virtual bool IsSubWidgetAllowed(Class* subWidgetClass);

        virtual bool IsContainer() const { return true; }

        bool IsWindow();

        bool IsFocussed() const { return EnumHasFlag(stateFlags, CStateFlag::Focused); }

        void SetVisible(bool visible);
        void SetEnabled(bool enabled);
        void SetInteractable(bool interactable);

        bool ClipsChildren() const { return clipChildren; }

        void SetClipChildren(bool set) { clipChildren = set; }

        f32 GetRotation() const { return rotation; }

        void SetRotation(f32 rotation);

        bool IsVerticalScrollAllowed() const { return allowVerticalScroll; }

        bool IsHorizontalScrollAllowed() const { return allowHorizontalScroll; }

        void SetVerticalScrollAllowed(bool value) { allowVerticalScroll = value; }

        void SetHorizontalScrollAllowed(bool value) { allowHorizontalScroll = value; }

        FUNCTION()
        void SetNeedsPaint();

        FUNCTION()
        void SetNeedsLayout();

        FUNCTION()
        void SetNeedsStyle();

        bool NeedsLayout();

        bool NeedsPaint();

        bool NeedsStyle();

        void LoadStyleSheet(const IO::Path& fullPath);

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

        virtual void OnAfterUpdateLayout();

        virtual Vec2 CalculateIntrinsicSize(f32 width, f32 height);

        //! @brief Called before painting the borders/edges, but after painting the background
        virtual void OnPaintEarly(CPaintEvent* paintEvent);

        CBehavior* AddBehavior(SubClass<CBehavior> behaviorClass);

        template<typename T> requires TIsBaseClassOf<CBehavior, T>::Value
        T* AddBehavior()
        {
            return (T*)AddBehavior(GetStaticClass<T>());
        }

        CBehavior* GetBehavior(SubClass<CBehavior> behaviorClass);

        template<typename T> requires TIsBaseClassOf<CBehavior, T>::Value
        T* GetBehavior()
        {
            return (T*)GetBehavior(GetStaticClass<T>());
        }

        // - Style API -

        CStateFlag GetStateFlags() const { return stateFlags; }

        void SetStateFlags(CStateFlag flags) { stateFlags = flags; SetNeedsStyle(); SetNeedsPaint(); }

        Vec4 GetFinalRootPadding();

        void SetBackgroundImage(const Name& imagePath) { this->backgroundImageOverride = imagePath; SetNeedsPaint(); }

        Name GetBackgroundImage() const { return backgroundImageOverride; }

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
                SetNeedsPaint();
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
                SetNeedsPaint();
            }
        }

        bool StyleClassExists(const String& styleClass) const
        {
            return styleClasses.Exists(styleClass);
        }

        virtual Vec2 GetComputedLayoutTopLeft();
        virtual Vec2 GetComputedLayoutSize();

        virtual Vec2 GetAvailableSizeForChild(CWidget* childWidget);


        Vec4 GetComputedLayoutPadding() {
            return Vec4(YGNodeLayoutGetPadding(node, YGEdgeLeft),
                YGNodeLayoutGetPadding(node, YGEdgeTop),
                YGNodeLayoutGetPadding(node, YGEdgeRight),
                YGNodeLayoutGetPadding(node, YGEdgeBottom));
        }

        Rect GetScreenSpaceRect();
        Rect GetWindowSpaceRect();

        // - Transformation Utils -

        Vec2 LocalToScreenSpacePos(const Vec2& point);
        Rect LocalToScreenSpaceRect(const Rect& localRect);
        Vec2 LocalToWindowSpacePos(const Vec2& point);
        Rect LocalToWindowSpaceRect(const Rect& localRect);
        Vec2 ScreenToLocalSpacePoint(const Vec2& point);
        Vec2 ScreenToWindowSpacePoint(const Vec2& point);

        //! @brief Returns the underlying Renderer2D that is used for rendering all widgets.
        //! @details Do NOT use this directly to draw 2d graphics, use CPainter instead from OnPaint and OnPaintOverlay methods.
        //! This is useful in cases where you only need to calculate the bounds of a text and need Renderer2D for it.
        Renderer2D* GetRenderer();

        CPlatformWindow* GetNativeWindow();

        CWindow* GetRootWindow();

        void QueueDestroy();

        Vec2 CalculateTextSize(const String& text, f32 fontSize, Name fontName, f32 width);

        void Focus();
        void Unfocus();

        // - Events -

        EVENT()
        CVoidEvent onFocused;

        EVENT()
        CVoidEvent onUnfocused;

        EVENT()
        CVoidEvent onMouseLeftPress;

        EVENT()
        CVoidEvent onMouseLeftClick;

        virtual void OnFocusGained() {}
        virtual void OnFocusLost() {}

        virtual void OnBeforeComputeStyle() {}

        //! @brief Override to get a callback on after computing the style.
        //! @return Override and return true if you are changing the layout in this function.
        virtual bool PostComputeStyle() { return false; }

        void SetNeedsPaintRecursively(bool newValue = false);

        virtual void Construct();

        void OnAfterConstruct() override final;

        virtual void OnPaint(CPaintEvent* paintEvent);
        virtual void OnPaintOverlay(CPaintEvent* paintEvent);

        virtual void HandleEvent(CEvent* event);

        virtual CWidget* HitTest(Vec2 windowSpaceMousePos);

    protected:

        virtual void OnDestroyQueued();

        virtual void OnEnabled();
        virtual void OnDisabled();

        void OnSubobjectDetached(Object* object) override;
        void OnSubobjectAttached(Object* object) override;

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

        FIELD()
        Name backgroundImageOverride{};

        FIELD()
        Array<CBehavior*> behaviors{};

        FIELD()
        b8 canDrawBgImage = true;

        FIELD()
        f32 rotation = 0.0f;

        // - Configs -

        FIELD(Config)
        f32 scrollSensitivity = 1.0f;

        void ClearChildNodes();
        void ReAddChildNodes();

        YGNodeRef node = nullptr;
        YGNodeRef detachedNode = nullptr;

        CStyle computedStyle;
        CCursor hoverCursor = CCursor::Inherited;

        b8 wasClickedInside = false;

        int destroyFrameCounter = 0;
        b8 isQueuedForDestruction = false;

    protected:

        CBehavior* AddDefaultBehavior(SubClass<CBehavior> behaviorClass);

        template<typename T> requires TIsBaseClassOf<CBehavior, T>::Value
        T* AddDefaultBehavior()
        {
            return (T*)AddDefaultBehavior(GetStaticClass<T>());
        }

        virtual void OnBeforeDestroy() override;

        static YGSize MeasureFunctionCallback(YGNodeConstRef node,
            float width,
            YGMeasureMode widthMode,
            float height,
            YGMeasureMode heightMode);

        CE_WIDGET_FRIENDS()
    };

} // namespace CE::Widgets

#include "CWidget.rtti.h"
