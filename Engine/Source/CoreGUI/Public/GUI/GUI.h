#pragma once

#include "GUIDefinitions.h"

namespace CE::GUI
{

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

    COREGUI_API void PushStyleVar(StyleVar var, f32 value);

    COREGUI_API void PushStyleVar(StyleVar var, Vec2 value);

    COREGUI_API void PopStyleVar(int count = 1);

    COREGUI_API void PushStyleColor(StyleCol color, const Color& value);

    COREGUI_API void PopStyleColor(int count = 1);

#pragma endregion


#pragma region Widgets

    COREGUI_API bool Button(const String& label, const Vec2& size = Vec2(0, 0), ButtonFlags flags = ButtonFlags::None);

#pragma endregion

#pragma region Layout

	COREGUI_API void SameLine(f32 offsetFromStart = 0, f32 spacing = -1.0f);

#pragma endregion
    
} // namespace CE

