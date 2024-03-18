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

        bool NeedsPaint();

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

    crystalwidgets_internal:

        YGNodeRef node = nullptr;

    private:

    };

} // namespace CE::Widgets

#include "CWidget.rtti.h"
