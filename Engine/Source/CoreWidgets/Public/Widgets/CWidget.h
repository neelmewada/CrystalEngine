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

		CWidgetException(const String& message) : message(message)
		{}
        
        virtual const char* what() const noexcept override
        {
            return message.GetCString();
        }

    private:
        String message{};
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

		virtual void OnBeforeComputeStyle() {}
		virtual void OnAfterComputeStyle() {}

		bool NeedsLayout();
		bool NeedsStyle();

		void SetNeedsLayout(bool set = true);
		void SetNeedsStyle(bool set = true);

		inline bool IsEnabled() const { return !isDisabled; }
		inline bool IsDisabled() const { return isDisabled; }
		inline void SetEnabled(bool enabled) { isDisabled = !enabled; }

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

		inline CStateFlag GetStateFlags() const { return stateFlags; }

		// - Style API -
        
		inline CStyle& GetStyle()
		{
			return style;
		}

		inline const CStyle& GetComputedStyle()
		{
			return computedStyle;
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

		inline CStyleValue& AddStyleProperty(CStylePropertyType property, const CStyleValue& value)
		{
			return style.AddProperty(property, value);
		}

		void SetStyleSheet(const String& stylesheet);

		const String& GetStyleSheet() const;

		bool IsHovered() const { return isHovered; }
		bool IsFocused() const { return isFocused; }

		bool IsLeftMouseHeld() const { return isLeftMousePressedInside; }

		// Hierarchy API

		CWidget* GetOwner();

		CWindow* GetOwnerWindow();

		int GetSubWidgetIndex(CWidget* siblingWidget);

		int GetSubWidgetCount();

		CWidget* FindSubWidget(const Name& name);

		CWidget* GetSubWidgetAt(int index);

		// Calculated Layout

		inline Vec2 GetComputedLayoutTopLeft() const { return Vec2(YGNodeLayoutGetLeft(node), YGNodeLayoutGetTop(node)); }
		inline Vec2 GetComputedLayoutSize() const { return Vec2(YGNodeLayoutGetWidth(node), YGNodeLayoutGetHeight(node)); }

		inline Rect GetComputedLayoutRect() const
		{
			Vec2 pos = GetComputedLayoutTopLeft();
			Vec2 size = GetComputedLayoutSize();
			return Rect(pos, pos + size);
		}

		inline Vec4 GetComputedLayoutPadding() const
		{
			Vec4 padding{};
			padding.left = YGNodeLayoutGetPadding(node, YGEdgeLeft);
			padding.top = YGNodeLayoutGetPadding(node, YGEdgeTop);
			padding.right = YGNodeLayoutGetPadding(node, YGEdgeRight);
			padding.bottom = YGNodeLayoutGetPadding(node, YGEdgeBottom);
			return padding;
		}

	protected:

		CWidget();

		void LoadGuiStyleStateProperty(CStylePropertyType property, const CStyleValue& styleValue, GUI::GuiStyleState& outState);

		// Draw Helpers

		void DrawBackground(const GUI::GuiStyleState& styleState);
		
		inline void DrawDefaultBackground()
		{
			DrawBackground(defaultStyleState);
		}

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

		void HandleBasicMouseEvents(bool hovered, bool leftMouseHeld);

		FIELD()
		WidgetFlags widgetFlags{};

		FIELD()
		Array<CWidget*> attachedWidgets{};

		FIELD()
		Array<String> styleClasses{};

		FIELD(NonSerialized)
		CStyle style{}; // Override local styles

		FIELD()
		CStateFlag stateFlags{};

		FIELD()
		CSubControl subControl = CSubControl::None;

		FIELD()
		b8 isDisabled = false;
		
		CStyleSheet* stylesheet = nullptr;
		String stylesheetText = "";

		YGNodeRef node{};

		// Style states

		CStyle computedStyle{};

		GUI::GuiStyleState defaultStyleState{};

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
        
	};
    
} // namespace CE::Widgets

#include "CWidget.rtti.h"


