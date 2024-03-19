#pragma once

namespace CE::Widgets
{
	class CPainter;
	class CWindow;

    CLASS()
    class CRYSTALWIDGETS_API CWidget : public Object
    {
        CE_CLASS(CWidget, Object)
    public:

        CWidget();
        virtual ~CWidget();

        inline bool IsEnabled() const { return enabled; }

        inline bool IsVisible() const { return visible; }

        inline bool IsInteractable() const { return interactable; }

        virtual bool CanPaint() const { return true; }

        inline void SetVisible(bool visible) { this->visible = visible; }
        inline void SetEnabled(bool enabled) { this->enabled = enabled; }

        void SetNeedsPaint() { needsPaint = true; }

        void SetNeedsLayout();

        void SetNeedsStyle();

        bool NeedsPaint();

        bool NeedsStyle();

        void AddSubWidget(CWidget* widget);
        void RemoveSubWidget(CWidget* widget);

        u32 GetSubWidgetCount() const { return attachedWidgets.GetSize(); }
        CWidget* GetSubWidget(u32 index) const { return attachedWidgets[index]; }

        u32 GetSubWidgetIndex(CWidget* widget) const { return attachedWidgets.IndexOf(widget); }

        bool StyleClassExists(const Name& name) const { return styleClasses.Exists(name); }

        void UpdateStyleIfNeeded();

        // - Style API -

        inline const CStyle& GetComputedStyle()
        {
            return computedStyle;
        }

        inline u32 GetStyleClassesCount() const
        {
            return styleClasses.GetSize();
        }

        inline const Name& GetStyleClass(u32 index) const
        {
            return styleClasses[index];
        }

        inline void AddStyleClass(const String& styleClass)
        {
            if (!StyleClassExists(styleClass))
            {
                styleClasses.Add(styleClass);
                SetNeedsStyle();
                SetNeedsLayout();
            }
        }

        inline void AddStyleClasses(const Array<String>& styleClasses)
        {
            for (const auto& styleClass : styleClasses)
            {
                AddStyleClass(styleClass);
            }
        }

        inline void RemoveStyleClass(const String& styleClass)
        {
            if (styleClasses.Remove(styleClass))
            {
                SetNeedsStyle();
                SetNeedsLayout();
            }
        }

        inline bool StyleClassExists(const String& styleClass) const
        {
            return styleClasses.Exists(styleClass);
        }

    protected:

        void SetNeedsPaintRecursively(bool newValue = false);

        virtual void Construct();

        void OnAfterConstruct() override final;

        virtual void OnPaint(CPaintEvent* paintEvent);

        void OnSubobjectDetached(Object* object) override;
        void OnSubobjectAttached(Object* object) override;

        virtual void HandleEvent(CEvent* event);

    crystalwidgets_protected_internal:

        FIELD()
        Array<CWidget*> attachedWidgets{};

        FIELD()
        CWidget* parent = nullptr;

        FIELD()
        CWindow* ownerWindow = nullptr;

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

        FIELD()
		CStateFlag stateFlags{};

		FIELD()
		CSubControl subControl = CSubControl::None;

        FIELD()
        Array<Name> styleClasses{};

        FIELD()
        CStyleSheet* styleSheet = nullptr;

    crystalwidgets_internal:

        YGNodeRef node = nullptr;

        CStyle computedStyle{};

    private:

    };

} // namespace CE::Widgets

#include "CWidget.rtti.h"
