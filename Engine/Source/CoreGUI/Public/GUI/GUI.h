#pragma once

#include "GUIDefinitions.h"

namespace CE::GUI
{
	COREGUI_API ID GetID(const char* strId);

	COREGUI_API ID GetID(const String& strId);

    COREGUI_API Vec2 GetCursorPos();
    COREGUI_API void SetCursorPos(const Vec2& pos);
    COREGUI_API Vec2 GetCursorScreenPos();

	COREGUI_API Vec2 GetWindowPos();
	COREGUI_API Vec2 GetWindowSize();
	COREGUI_API Vec4 GetWindowRect();

	COREGUI_API void SetNextWindowPos(const Vec2& pos, Cond condition = Cond::None, const Vec2& pivot = Vec2(0, 0));

	COREGUI_API void SetNextWindowSize(const Vec2& size, Cond condition = Cond::None);

    COREGUI_API bool BeginWindow(const String& name, bool* isShown, WindowFlags flags = WF_None);

    COREGUI_API ID DockSpace(const String& id, Vec2 size = Vec2(0, 0), DockFlags dockFlags = DockFlags_None);

    COREGUI_API void EndWindow();

    COREGUI_API bool BeginMenuBar();

    COREGUI_API void EndMenuBar();

    COREGUI_API bool BeginMenu(const String& label, bool enabled = true, const char* icon = nullptr);

    COREGUI_API bool MenuItem(const String& label, const String& shortcut = "", bool selected = false, bool enabled = true);

    COREGUI_API bool MenuItem(const String& label, const String& shortcut, bool* selected, bool enabled = true);

    COREGUI_API void EndMenu();

#pragma region Style

    COREGUI_API bool IsStyleVarOfVectorType(StyleVar var);

    COREGUI_API void PushStyleVar(StyleVar var, f32 value);

    COREGUI_API void PushStyleVar(StyleVar var, Vec2 value);

    COREGUI_API void PopStyleVar(int count = 1);

    COREGUI_API void PushStyleColor(StyleCol color, const Color& value);

    COREGUI_API void PopStyleColor(int count = 1);

#pragma endregion


#pragma region Widgets

	COREGUI_API void ShowDemoWindow(bool* open = nullptr);

	COREGUI_API void Text(const char* text);
	COREGUI_API void Text(const String& text);

	COREGUI_API void TextColored(const char* text, const Color& color);
	COREGUI_API void TextColored(const String& text, const Color& color);

    COREGUI_API bool Button(const String& label, const Vec2& size = Vec2(0, 0), ButtonFlags flags = ButtonFlags::None);

	COREGUI_API bool Button(const String& label, const Vec4& padding, const Vec2& size = Vec2(0, 0), ButtonFlags flags = ButtonFlags::None);

    COREGUI_API void InvisibleButton(const String& id, const Vec2& size);

#pragma endregion

#pragma region Layout

	COREGUI_API void SameLine(f32 offsetFromStart = 0, f32 spacing = -1.0f);

	COREGUI_API void BeginGroup();
	COREGUI_API void EndGroup();

    COREGUI_API Vec2 CalculateTextSize(const char* text);
    COREGUI_API Vec2 CalculateTextSize(const String& text);

    COREGUI_API Vec2 GetItemRectSize();

#pragma endregion

#pragma region Events/States

	COREGUI_API Vec4 GetItemRect();

	COREGUI_API bool IsWindowHovered(HoveredFlags flags = Hovered_None);

	COREGUI_API bool IsWindowHovered(const String& windowId, HoveredFlags flags = Hovered_None);

	COREGUI_API bool IsWindowFocused(FocusFlags flags = FOCUS_None);

	COREGUI_API bool IsWindowFocused(const String& windowId, FocusFlags flags = FOCUS_None);

	COREGUI_API bool IsItemHovered(HoveredFlags flags = Hovered_None);

	COREGUI_API bool IsMouseDown(MouseButton button = MouseButton::Left);

	COREGUI_API bool IsItemClicked(MouseButton button = MouseButton::Left);
	COREGUI_API bool IsItemDoubleClicked(MouseButton button = MouseButton::Left);

	COREGUI_API bool IsItemFocused();

	COREGUI_API bool IsItemVisible();

	COREGUI_API bool IsItemActive();

	COREGUI_API void SetWantMouseCapture(bool value);

	COREGUI_API bool IsMouseClicked(MouseButton button = MouseButton::Left);
	COREGUI_API bool IsMouseDoubleClicked(MouseButton button = MouseButton::Left);

	COREGUI_API Vec2 GetMousePos();

#pragma endregion

#pragma region Custom Drawing

    COREGUI_API void DrawRect(const Vec4& rect, const Color& color, Vec4 rounding = { 0, 0, 0, 0 });

    COREGUI_API void FillRect(const Vec4& rect, const Color& color, Vec4 rounding = { 0, 0, 0, 0 });

	COREGUI_API void FillRect(const Vec4& rect, const Gradient& gradient, Vec4 rounding = { 0, 0, 0, 0 });

#pragma endregion
    
} // namespace CE

