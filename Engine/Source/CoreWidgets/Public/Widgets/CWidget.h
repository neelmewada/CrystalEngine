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

	ENUM()
	enum class CDebugBackgroundFilter : u8
	{
		None = 0,
		IntrinsicSize = BIT(0),
		Padding = BIT(1),
		Border = BIT(2),
		Margin = BIT(3),
		All = IntrinsicSize | Padding | Border | Margin,
	};
	ENUM_CLASS_FLAGS(CDebugBackgroundFilter);

	class CWindow;
	class CMenu;
	class CContextMenu;

	CLASS(Abstract)
	class COREWIDGETS_API CWidget : public Object
	{
		CE_CLASS(CWidget, CE::Object)
	public:

		virtual ~CWidget();
		
		void OnBeforeDestroy() override;

		// - Public API -

		virtual void OnBeforeComputeStyle() {}
		virtual void OnAfterComputeStyle() {}

		inline bool IsEnabled() const { return !isDisabled; }
		inline bool IsDisabled() const { return isDisabled; }
		void SetEnabled(bool enabled);

		inline bool IsVisible() const { return isVisible; }
		inline void SetVisible(bool visible) { isVisible = visible; }

		bool IsDebugModeEnabled();

		inline bool IsDebugDrawEnabled() const { return debugDraw; }
		inline void SetDebugDrawEnabled(bool enable) { this->debugDraw = enable; }

		void ShowContextMenu();
		void HideContextMenu();

		virtual void Construct();

        // For internal use only!
		virtual void Render();

		virtual bool IsMenu() { return false; }
		virtual bool IsMenuBar() { return false; }
        virtual bool IsWindow() { return false; }

		/// Should return true if a widget can contain & render subwidgets
		virtual bool IsContainer() { return IsWindow(); }
        
        void SetWidgetFlags(WidgetFlags flags);
        
        WidgetFlags GetWidgetFlags() const;

		inline CStateFlag GetStateFlags() const { return stateFlags; }
		inline void SetStateFlags(CStateFlag flags) { stateFlags = flags; }

		inline YGNodeRef GetNode() const { return node; }

		inline void SetIndependentLayout(bool set) { this->independentLayout = set; }

		// - Layout -

		virtual Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) { return Vec2(); }

		/// Override and return true if the widget is the root of layout calculation for it's children
		virtual bool IsLayoutCalculationRoot();

		virtual bool ShouldHandleBackgroundDraw() { return true; }

		bool NeedsLayout(bool recursive = false);
		bool NeedsStyle(bool recursive = false);

		bool ChildrenNeedsLayout();
		bool ChildrenNeedsStyle();

		inline bool NeedsLayoutRecursive()
		{
			return NeedsLayout(true);
		}

		inline bool NeedsStyleRecursive()
		{
			return NeedsStyle(true);
		}

		void SetNeedsLayout(bool set = true, bool recursive = false);
		void SetNeedsStyle(bool set = true, bool recursive = false);

		inline void SetNeedsLayoutRecursive(bool set = true)
		{
			SetNeedsLayout(set, true);
		}

		inline void SetNeedsStyleRecursive(bool set = true)
		{
			SetNeedsStyle(set, true);
		}

		virtual void UpdateLayoutIfNeeded();
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

		inline const String& GetStyleClass(u32 index) const
		{
			return styleClasses[index];
		}

		inline void AddStyleClass(const String& styleClass)
		{
			if (!StyleClassExists(styleClass))
			{
				styleClasses.Add(styleClass);
				SetNeedsStyleRecursive();
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
				SetNeedsStyleRecursive();
				SetNeedsLayout();
			}
		}

		inline bool StyleClassExists(const String& styleClass) const
		{
			return styleClasses.Exists(styleClass);
		}

		void SetStyleSheet(const String& stylesheet);
		const String& GetStyleSheet() const;

		void LoadStyleSheet(const Name& resourcePath);

		inline CStyleSheet* GetStyleSheetObject() const { return stylesheet; }

		bool IsHovered() const { return isHovered; }
		bool IsFocused() const { return isFocused; }

		bool IsLeftMouseHeld() const { return isLeftMousePressedInside; }

		inline bool IsInteractable() const { return isInteractable; }
		inline void SetInteractable(bool interactable) { isInteractable = interactable; }

		// Hierarchy API

		CWidget* GetOwner();

		CWindow* GetOwnerWindow();

		/// Returns true if the given widget is present in the parent hierarchy of this widget
		bool IsWidgetPresentInParentHierarchy(CWidget* widget);

		int GetSubWidgetIndex(CWidget* siblingWidget);
		int GetSubWidgetCount();

		CWidget* FindSubWidget(const Name& name);
		CWidget* GetSubWidgetAt(int index);

		void AddSubWidget(CWidget* subWidget);
		void RemoveSubWidget(CWidget* subWidget);

		Array<CWidget*> RemoveAllSubWidgets();
		void DestroyAllSubWidgets();

		virtual bool IsSubWidgetAllowed(ClassType* subWidgetClass);

		inline CContextMenu* GetContextMenu() const { return contextMenu; }
		inline void SetContextMenu(CContextMenu* contextMenu) { this->contextMenu = contextMenu; }

		// Calculated Layout

		inline Vec2 GetScreenPosition() const { return screenPos; }

		inline Vec2 GetComputedLayoutTopLeft() const { return Vec2(YGNodeLayoutGetLeft(node), YGNodeLayoutGetTop(node)); }
		inline Vec2 GetComputedLayoutSize() const { return Vec2(YGNodeLayoutGetWidth(node), YGNodeLayoutGetHeight(node)); }

		CStyleValue GetComputedPropertyValue(CStylePropertyType property);

		inline Vec4 GetComputedLayoutMargin() const
		{
			return Vec4(
				YGNodeLayoutGetMargin(node, YGEdgeLeft),
				YGNodeLayoutGetMargin(node, YGEdgeTop),
				YGNodeLayoutGetMargin(node, YGEdgeRight),
				YGNodeLayoutGetMargin(node, YGEdgeBottom)
			);
		}

		inline Vec4 GetComputedLayoutPadding() const
		{
			return Vec4(
				YGNodeLayoutGetPadding(node, YGEdgeLeft),
				YGNodeLayoutGetPadding(node, YGEdgeTop),
				YGNodeLayoutGetPadding(node, YGEdgeRight),
				YGNodeLayoutGetPadding(node, YGEdgeBottom)
			);
		}

		inline Vec4 GetComputedLayoutBorder() const
		{
			return Vec4(
				YGNodeLayoutGetBorder(node, YGEdgeLeft),
				YGNodeLayoutGetBorder(node, YGEdgeTop),
				YGNodeLayoutGetBorder(node, YGEdgeRight),
				YGNodeLayoutGetBorder(node, YGEdgeBottom)
			);
		}

		inline Vec2 GetComputedLayoutIntrinsicSize() const
		{
			return GetComputedLayoutSize() - 
				Vec2(YGNodeLayoutGetPadding(node, YGEdgeLeft) + YGNodeLayoutGetPadding(node, YGEdgeRight),
					YGNodeLayoutGetPadding(node, YGEdgeTop) + YGNodeLayoutGetPadding(node, YGEdgeBottom));
		}

		inline Rect GetComputedLayoutRect() const
		{
			Vec2 pos = GetComputedLayoutTopLeft();
			Vec2 size = GetComputedLayoutSize();
			return Rect(pos, pos + size);
		}

		inline Rect GetComputedMarginRect() const
		{
			auto layoutRect = GetComputedLayoutRect();
			return Rect(layoutRect.left - YGNodeLayoutGetMargin(node, YGEdgeLeft),
				layoutRect.top - YGNodeLayoutGetMargin(node, YGEdgeTop),
				layoutRect.right + YGNodeLayoutGetMargin(node, YGEdgeRight),
				layoutRect.bottom + YGNodeLayoutGetMargin(node, YGEdgeBottom));
		}

		inline Rect GetComputedBorderRect() const
		{
			auto layoutRect = GetComputedLayoutRect();
			return Rect(layoutRect.left - YGNodeLayoutGetBorder(node, YGEdgeLeft),
				layoutRect.top - YGNodeLayoutGetBorder(node, YGEdgeTop),
				layoutRect.right + YGNodeLayoutGetBorder(node, YGEdgeRight),
				layoutRect.bottom + YGNodeLayoutGetBorder(node, YGEdgeBottom));
		}

		inline Rect GetComputedPaddingRect() const
		{
			auto layoutRect = GetComputedLayoutRect();
			return layoutRect;
			return Rect(layoutRect.left + YGNodeLayoutGetPadding(node, YGEdgeLeft),
				layoutRect.top + YGNodeLayoutGetPadding(node, YGEdgeTop),
				layoutRect.right - YGNodeLayoutGetPadding(node, YGEdgeRight),
				layoutRect.bottom - YGNodeLayoutGetPadding(node, YGEdgeBottom));
		}

		inline Rect GetComputedIntrinsicSizeRect() const
		{
			auto layoutRect = GetComputedPaddingRect();
			return Rect(layoutRect.left + YGNodeLayoutGetPadding(node, YGEdgeLeft),
				layoutRect.top + YGNodeLayoutGetPadding(node, YGEdgeTop),
				layoutRect.right - YGNodeLayoutGetPadding(node, YGEdgeRight),
				layoutRect.bottom - YGNodeLayoutGetPadding(node, YGEdgeBottom));
		}

	protected:

		CWidget();

		void LoadGuiStyleState(const CStyle& from, GUI::GuiStyleState& outState);
		void LoadGuiStyleStateProperty(CStylePropertyType property, const CStyleValue& styleValue, GUI::GuiStyleState& outState);

		// Draw Helpers
		void DrawShadow(const GUI::GuiStyleState& styleState);
		void DrawShadow(const GUI::GuiStyleState& styleState, const Rect& localRect);

		void DrawBackground(const GUI::GuiStyleState& styleState, bool allowDebug = true);
		void DrawBackground(const GUI::GuiStyleState& styleState, const Rect& localRect, bool allowDebug = true);

		void DrawDebugBackground(CDebugBackgroundFilter filter = CDebugBackgroundFilter::All);

		void FillRect(const Color& color, const Rect& localRect, const Vec4& borderRadius = {});
		void DrawRect(const Color& color, const Rect& localRect, f32 borderThickness = 1.0f, const Vec4& borderRadius = {});
		void DrawImage(const CTexture& image, const Rect& localRect, const Color& tintColor = Color::White());

		void FillCircle(const Color& color, const Rect& localRect);
		void DrawCircle(const Color& color, const Rect& localRect, f32 borderThickness = 1.0f);
		
		inline void DrawDefaultBackground()
		{
			DrawBackground(defaultStyleState);
		}

		inline void DrawDefaultBackground(const Rect& localRect)
		{
			DrawBackground(defaultStyleState, localRect);
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

		/// Override to use custom focus validation method
		virtual bool TestFocus();

		/// Must be called at the end of all GUI draw calls
		virtual void PollEvents();

		// Optimizations


	public: // Signals

		CE_SIGNAL(OnMouseClick, CMouseEvent*);
		
		CE_SIGNAL(OnMouseRightClick, CMouseEvent*);

		CE_SIGNAL(OnFocusChanged, bool);

		CE_SIGNAL(OnKeyPressed, CKey);
		CE_SIGNAL(OnKeyReleased, CKey);

	protected:

		static YGSize MeasureFunctionCallback(YGNodeRef nodeRef, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode);

		virtual void OnSubobjectAttached(Object* subobject) override;
		virtual void OnSubobjectDetached(Object* subobject) override;

	corewidgets_protected_internal:

		// Helpers

		void ClearChildNodes();
		void ReAddChildNodes();

		// Fields


		FIELD()
		WidgetFlags widgetFlags{};

		FIELD()
		Array<CWidget*> attachedWidgets{};

		FIELD()
		Array<String> styleClasses{};

		FIELD()
		CStateFlag stateFlags{};

		FIELD()
		CSubControl subControl = CSubControl::None;

		FIELD()
		b8 isDisabled = false;

		FIELD()
		b8 isVisible = true;

		FIELD()
		b8 isInteractable = true;

		FIELD()
		CWidget* parent = nullptr; 

		FIELD()
		CWidget* owner = nullptr;

		FIELD()
		CWindow* ownerWindow = nullptr;

		FIELD()
		CContextMenu* contextMenu = nullptr;

		FIELD()
		b8 needsLayout = true;

		FIELD()
		b8 needsStyle = true;

		FIELD()
		b8 independentLayout = false;

		FIELD()
		Vec2 sizeConstraint = Vec2(YGUndefined, YGUndefined);

		Array<CMenu*> attachedMenus{};
		
		FIELD(ReadOnly)
		CStyleSheet* stylesheet = nullptr;

		String stylesheetText = "";

		YGNodeRef node{};
		YGNodeRef detachedNode{};

		// Style states

		CStyle computedStyle{};

		GUI::GuiStyleState defaultStyleState{};

		// States
		b8 isHovered = false;
		b8 isFocused = false;
		b8 isLeftMousePressedInside = false;

	corewidgets_private_internal:

		// Internals

		Vec2 screenPos{};
		Vec2 prevHoverPos{};
		b8 prevLeftMouseDown = false;
		b8 firstDraw = true;

		CDebugBackgroundFilter filter = CDebugBackgroundFilter::All;
		b8 debugDraw = false;
		CDebugBackgroundFilter forceDebugDrawMode = CDebugBackgroundFilter::None;
		u32 debugId = 0;

		friend class CWidgetDebugger;
        
		template<typename TWidget>
		friend TWidget* CreateWidget(Object* owner,
				String widgetName,
				ClassType* widgetClass,
				ObjectFlags objectFlags);
	};
    
} // namespace CE::Widgets

#include "CWidget.rtti.h"


