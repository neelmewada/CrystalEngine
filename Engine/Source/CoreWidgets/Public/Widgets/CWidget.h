#pragma once

namespace CE::Widgets
{
	namespace Internal
	{
		struct WidgetInitializer;
	}

	class CWidgetException : public std::exception
	{
	public:

		CWidgetException(const String& message) : std::exception(message.GetCString())
		{}

	};

	class CWindow;

	CLASS(Abstract)
	class COREWIDGETS_API CWidget : public Object
	{
		CE_CLASS(CWidget, CE::Object)
	public:

		virtual ~CWidget();

		// - Public API -

		void UpdateLayoutIfNeeded();
		void UpdateStyleIfNeeded();

		virtual void OnComputeStyle() {}

		inline bool NeedsLayout() const { return needsLayout; }
		inline bool NeedsStyle() const { return needsStyle; }

		inline void SetNeedsLayout() { needsLayout = true; }
		inline void SetNeedsStyle() { needsStyle = true; }

		virtual void Construct();

        // For internal use only!
		virtual void RenderGUI();
        
        virtual bool IsWindow() { return false; }

		/// Should return true if a widget can contain & render subwidgets
		virtual bool IsContainer() { return IsWindow(); }

		virtual Vec2 CalculateEstimateSize() { return Vec2(); }

		virtual Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) { return Vec2(); }
        
        void SetWidgetFlags(WidgetFlags flags);
        
        WidgetFlags GetWidgetFlags() const;
        
		inline CStyle& GetStyle()
		{
			return style;
		}

		inline u32 GetStyleClassesCount() const
		{
			return styleClasses.GetSize();
		}

		inline const String& GetStyleClass(u32 index) const
		{
			return styleClasses[index];
		}

		inline void AddStyleClass(const String& styleClass)
		{
			styleClasses.Add(styleClass);
			SetNeedsStyle();
		}

		inline void AddStyleClasses(const Array<String>& styleClasses)
		{
			this->styleClasses.AddRange(styleClasses);
			SetNeedsStyle();
		}

		inline void RemoveStyleClass(const String& styleClass)
		{
			styleClasses.Remove(styleClass);
			SetNeedsStyle();
		}

		inline bool StyleClassExists(const String& styleClass) const
		{
			return styleClasses.Exists(styleClass);
		}

		inline CStyleValue& AddStyleProperty(CStylePropertyType property, const CStyleValue& value, CStateFlag state = CStateFlag::Default, CSubControl subControl = CSubControl::None)
		{
			return style.AddProperty(property, value, state, subControl);
		}

		CWidget* GetOwner();

		CWindow* GetOwnerWindow();

		bool IsHovered() const { return isHovered; }
		bool IsFocused() const { return isFocused; }

		bool IsLeftMouseHeld() const { return isLeftMousePressedInside; }

		CWidget* FindSubWidget(const Name& name);

		// Calculated Layout

		inline Vec2 GetComputedLayoutTopLeft() const { return Vec2(YGNodeLayoutGetLeft(node), YGNodeLayoutGetTop(node)); }
		inline Vec2 GetComputedLayoutSize() const { return Vec2(YGNodeLayoutGetWidth(node), YGNodeLayoutGetHeight(node)); }

		inline Rect GetComputedLayoutRect() const
		{
			Vec2 pos = GetComputedLayoutTopLeft();
			Vec2 size = GetComputedLayoutSize();
			return Rect(pos, pos + size);
		}

	protected:

		CWidget();

		void LoadGuiStyleStateProperty(CStylePropertyType property, const CStyleValue& styleValue, GUI::GuiStyleState& outState);

		// Events

		virtual void HandleEvent(CEvent* event);

		// Protected API

		virtual void OnSubWidgetAttached(CWidget* widget) {}
		virtual void OnSubWidgetDetached(CWidget* widget) {}

		virtual void OnAttachedTo(CWidget* parent);
		virtual void OnDetachedFrom(CWidget* parent);

		/// Abstract method. Must be overriden to call low level GUI draw functions.
		virtual void OnDrawGUI() = 0;
        
        /// Override this method to build child widget hierarchy
        virtual void Build() {}

		/// Must be called at the end of all GUI draw calls
		virtual void PollEvents();

	public: // Signals

		CE_SIGNAL(OnMouseClick, CMouseEvent*);

	private:

		static YGSize MeasureFunctionCallback(YGNodeRef nodeRef, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode);

		virtual void OnSubobjectAttached(Object* subobject) override;
		virtual void OnSubobjectDetached(Object* subobject) override;

	protected: // Fields

		FIELD()
		WidgetFlags widgetFlags{};

		FIELD()
		Array<CWidget*> attachedWidgets{};

		FIELD()
		Array<String> styleClasses{};

		FIELD()
		CStyle style{};

		YGNodeRef node{};

		// Style states

		GUI::GuiStyleState defaultStyleState{};
		GUI::GuiStyleState hoveredStyleState{};
		GUI::GuiStyleState pressedStyleState{};

    private:

		b8 needsLayout = true;
		b8 needsStyle = true;

		b8 inheritedPropertiesInitialized = false;

		// States
		b8 isHovered = false;
		b8 isFocused = false;
		b8 isLeftMousePressedInside = false;

		// Internals
		Vec2 prevHoverPos{};
		b8 prevLeftMouseDown = false;
        
	private:

        
	};
    
} // namespace CE::Widgets

#include "CWidget.rtti.h"


