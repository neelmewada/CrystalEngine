#pragma once

namespace CE::Widgets
{
	class CWidget;

	ENUM()
	enum class CEventType
	{
		Undefined = 0,
		CustomEvent,

		MouseEnter,
		MouseMove,
		MouseLeave,
		MousePress,
		MouseRelease,
		MouseButtonClick,
		MouseButtonDoubleClick,
		FocusChanged,
		ResizeEvent,

		KeyEvent,
	};
	ENUM_CLASS_FLAGS(CEventType);

    STRUCT()
	struct COREWIDGETS_API CEvent
	{
		CE_STRUCT(CEvent)
	public:

		CEvent() = default;

		inline void MarkHandled()
		{
			isConsumed = true;
		}

		inline void StopPropagation()
		{
			stopPropagation = true;
		}

		inline void HandleAndStopPropagation()
		{
			isConsumed = stopPropagation = true;
		}

		inline bool ShouldPropagate()
		{
			return !isConsumed || !stopPropagation;
		}

		inline CEventType GetEventType() const 
		{ 
			return type; 
		}

		FIELD()
		String name{};

		FIELD()
		CEventType type{};

		FIELD()
		TypeId customEventType{}; // TypeId of the event type struct

		FIELD()
		b8 isConsumed = false;

		FIELD()
		b8 stopPropagation = false;

		FIELD()
		CWidget* sender = nullptr;
	};

	STRUCT()
	struct COREWIDGETS_API CResizeEvent : public CEvent
	{
		CE_STRUCT(CResizeEvent, CEvent)
	public:

		FIELD()
		Vec2 oldSize{};

		FIELD()
		Vec2 newSize{};
	};

	STRUCT()
	struct COREWIDGETS_API CMouseEvent : public CEvent
	{
		CE_STRUCT(CMouseEvent, CE::Widgets::CEvent)
	public:

		FIELD()
		Vec2 mousePos{};

		FIELD()
		Vec2 prevMousePos{};

		FIELD()
		int mouseButton{}; // 0 = left click | 1 = right click | 2 = middle click

		FIELD()
		b8 isInside = false; // If mouse is inside the sender's bounds
	};

	STRUCT()
	struct COREWIDGETS_API CFocusEvent : public CEvent
	{
		CE_STRUCT(CFocusEvent, CE::Widgets::CEvent)
	public:

		inline bool GotFocus() const { return gotFocus; }
		inline bool LostFocus() const { return !gotFocus; }

		FIELD()
		b8 gotFocus = false;
	};

	ENUM()
	enum class CKey
	{
		None = 0,
		Tab = 512,             // == NamedBEGIN
		LeftArrow,
		RightArrow,
		UpArrow,
		DownArrow,
		PageUp,
		PageDown,
		Home,
		End,
		Insert,
		Delete,
		Backspace,
		Space,
		Enter,
		Escape,
		LeftCtrl, LeftShift, LeftAlt, LeftSuper,
		RightCtrl, RightShift, RightAlt, RightSuper,
		Menu,
		N0, N1, N2, N3, N4, N5, N6, N7, N8, N9,
		A, B, C, D, E, F, G, H, I, J,
		K, L, M, N, O, P, Q, R, S, T,
		U, V, W, X, Y, Z,
		F1, F2, F3, F4, F5, F6,
		F7, F8, F9, F10, F11, F12,
		Apostrophe,        // '
		Comma,             // ,
		Minus,             // -
		Period,            // .
		Slash,             // /
		Semicolon,         // ;
		Equal,             // =
		LeftBracket,       // [
		Backslash,         // \ (this text inhibit multiline comment caused by backslash)
		RightBracket,      // ]
		GraveAccent,       // `
		CapsLock,
		ScrollLock,
		NumLock,
		PrintScreen,
		Pause,
		Keypad0, Keypad1, Keypad2, Keypad3, Keypad4,
		Keypad5, Keypad6, Keypad7, Keypad8, Keypad9,
		KeypadDecimal,
		KeypadDivide,
		KeypadMultiply,
		KeypadSubtract,
		KeypadAdd,
		KeypadEnter,
		KeypadEqual,

		Ctrl = 1 << 12, // Ctrl
		Shift = 1 << 13, // Shift
		Alt = 1 << 14, // Option/Menu
		Super = 1 << 15, // Cmd/Super/Windows
		Shortcut = 1 << 11, // Alias for Ctrl (non-macOS) _or_ Super (macOS).
		Mask = 0xF800,  // 5-bits
	};

	STRUCT()
	struct COREWIDGETS_API CKeyEvent : public CEvent
	{
		CE_STRUCT(CKeyEvent, CEvent)
	public:

		inline bool IsPressed() const { return isPressed; }
		inline bool IsReleased() const { return !isPressed; }

		FIELD()
		b8 isPressed = true;

		FIELD()
		CKey key = CKey::None;
	};

} // namespace CE::Widgets

#include "CEvent.rtti.h"
