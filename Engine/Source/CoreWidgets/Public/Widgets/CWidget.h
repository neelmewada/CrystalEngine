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

        // For internal use only!
		virtual void RenderGUI();
        
        virtual bool IsWindow() { return false; }
		virtual bool IsContainer() { return IsWindow(); }

		virtual Vec2 CalculateEstimateSize() { return Vec2(); }
        
        void SetWidgetFlags(WidgetFlags flags);
        
        WidgetFlags GetWidgetFlags() const;
        
        CStyle& GetStyle()
        {
            return style;
        }
        
        void SetStyle(const CStyle& style)
        {
            this->style = style;
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
			ComputeStyles();
		}

		inline void AddStyleClasses(const Array<String>& styleClasses)
		{
			this->styleClasses.AddRange(styleClasses);
			ComputeStyles();
		}

		inline void RemoveStyleClass(const String& styleClass)
		{
			styleClasses.Remove(styleClass);
			ComputeStyles();
		}

		inline bool StyleClassExists(const String& styleClass) const
		{
			return styleClasses.Exists(styleClass);
		}

		CWidget* GetOwner();

		CWindow* GetOwnerWindow();

		bool IsHovered() const { return isHovered; }
		bool IsFocused() const { return isFocused; }

		bool IsLeftMouseHeld() const { return isLeftMousePressedInside; }

		CWidget* FindSubWidget(const Name& name);
		
		void ComputeStyles();
		
		virtual void Construct();

		virtual void UpdateStyle();

	protected:

		CWidget();

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

		virtual void OnSubobjectAttached(Object* subobject) override;
		virtual void OnSubobjectDetached(Object* subobject) override;

	protected: // Fields

		FIELD()
		WidgetFlags widgetFlags{};

		FIELD()
		Array<CWidget*> attachedWidgets{};
        
        FIELD()
        CStyle style{}; // Final computed style after inheritance, etc

		FIELD()
		Array<String> styleClasses{};

    private:

		b8 inheritedPropertiesInitialized = false;

		// States
		b8 isHovered = false;
		b8 isFocused = false;
		b8 isLeftMousePressedInside = false;

		// Internals
		Vec2 prevHoverPos{};
		b8 prevLeftMouseDown = false;
        
	private:

		struct PushedProperties
		{
			u32 pushedColors = 0;
			u32 pushedVars = 0;
		};

		Array<PushedProperties> pushedPropertiesStack{};
        
	};
    
} // namespace CE::Widgets

#include "CWidget.rtti.h"


