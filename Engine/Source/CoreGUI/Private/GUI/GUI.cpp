
#include "CoreGUI.h"

// Import Dear ImGui from VulkanRHI
#define IMGUI_API DLL_IMPORT
#include "imgui.h"
#include "imgui_internal.h"

#include "misc/cpp/imgui_stdlib.h"

namespace CE::GUI
{
	COREGUI_API ID GetID(const char* strId)
	{
		return ImGui::GetID(strId);
	}

	COREGUI_API ID GetID(const String& strId)
	{
		return ImGui::GetID(strId.GetCString());
	}

    COREGUI_API Vec2 GetCursorPos()
    {
        ImVec2 pos = ImGui::GetCursorPos();
        return Vec2(pos.x, pos.y);
    }

    COREGUI_API void SetCursorPos(const Vec2& pos)
    {
        ImGui::SetCursorPos(ImVec2(pos.x, pos.y));
    }

    COREGUI_API Vec2 GetCursorScreenPos()
    {
        ImVec2 vec = ImGui::GetCursorScreenPos();
        return Vec2(vec.x, vec.y);
    }

	COREGUI_API void SetNextWindowPos(const Vec2& pos, Cond condition, const Vec2& pivot)
	{
		ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y), (int)condition, ImVec2(pivot.x, pivot.y));
	}

	COREGUI_API void SetNextWindowSize(const Vec2& size, Cond condition)
	{
		ImGui::SetNextWindowSize(ImVec2(size.x, size.y), (int)condition);
	}

	COREGUI_API bool BeginWindow(const String& name, bool* isShown, WindowFlags flags)
    {
        if (isShown != nullptr && !(*isShown))
        {
            return false;
        }
		
        if (flags & GUI::WF_FullScreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        }
		
        if (flags & GUI::WF_NoPadding)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        }

        auto retVal = ImGui::Begin(name.GetCString(), isShown, (ImGuiWindowFlags)flags);

        if (flags & GUI::WF_NoPadding)
        {
            ImGui::PopStyleVar(1);
        }

        if (flags & GUI::WF_FullScreen)
        {
            ImGui::PopStyleVar(2);
        }

        return retVal;
    }

    COREGUI_API ID DockSpace(const String& id, Vec2 size, DockFlags dockFlags)
    {
        GUI::ID imguiID = ImGui::GetID(id.GetCString());
        return ImGui::DockSpace(imguiID, ImVec2(size.x, size.y), (ImGuiDockNodeFlags)dockFlags);
    }

    COREGUI_API void EndWindow()
    {
        ImGui::End();
    }

    COREGUI_API bool BeginMenuBar()
    {
        return ImGui::BeginMenuBar();
    }

    COREGUI_API void EndMenuBar()
    {
        ImGui::EndMenuBar();
    }

    COREGUI_API bool BeginMenu(const String& label, bool enabled, const char* icon)
    {
        if (icon != nullptr)
            return ImGui::BeginMenuEx(label.GetCString(), icon, enabled);
        return ImGui::BeginMenu(label.GetCString(), enabled);
    }

    COREGUI_API bool MenuItem(const String& label, const String& shortcut, bool selected, bool enabled)
    {
        return ImGui::MenuItem(label.GetCString(), shortcut.GetCString(), selected, enabled);
    }

    COREGUI_API bool MenuItem(const String& label, const String& shortcut, bool* selected, bool enabled)
    {
        return ImGui::MenuItem(label.GetCString(), shortcut.GetCString(), selected, enabled);
    }

    COREGUI_API void EndMenu()
    {
        ImGui::EndMenu();
    }

    /////////////////////////////////////////////////////////////////////
    // Style

    COREGUI_API bool IsStyleVarOfVectorType(StyleVar var)
    {
        static HashMap<StyleVar, bool> vectorTypes{
            { StyleVar_WindowPadding, true },
            { StyleVar_WindowMinSize, true },
            { StyleVar_WindowTitleAlign, true },
            { StyleVar_FramePadding, true },
            { StyleVar_ItemSpacing, true },
            { StyleVar_ItemInnerSpacing, true },
            { StyleVar_CellPadding, true },
            { StyleVar_ButtonTextAlign, true },
            { StyleVar_SelectableTextAlign, true },
            { StyleVar_SeparatorTextAlign, true },
            { StyleVar_SeparatorTextPadding, true }
        };
        
        return vectorTypes.KeyExists(var) && vectorTypes[var];
    }

    COREGUI_API void PushStyleVar(StyleVar var, f32 value)
    {
        ImGui::PushStyleVar(var, value);
    }

    COREGUI_API void PushStyleVar(StyleVar var, Vec2 value)
    {
        ImGui::PushStyleVar(var, ImVec2(value.x, value.y));
    }

    COREGUI_API void PopStyleVar(int count)
    {
        ImGui::PopStyleVar(count);
    }

    COREGUI_API void PushStyleColor(StyleCol color, const Color& value)
    {
        ImGui::PushStyleColor(color, ImVec4(value.r, value.g, value.b, value.a));
    }

    COREGUI_API void PopStyleColor(int count)
    {
        ImGui::PopStyleColor(count);
    }

#pragma region Widgets

	COREGUI_API void ShowDemoWindow(bool* open)
	{
		ImGui::ShowDemoWindow(open);
	}

	COREGUI_API void Text(const char* text)
	{
		ImGui::Text(text);
	}

	COREGUI_API void Text(const String& text)
	{
		ImGui::Text(text.GetCString());
	}

	COREGUI_API void TextColored(const char* text, const Color& color)
	{
		ImGui::TextColored(ImVec4(color.r, color.g, color.b, color.a), text);
	}

	COREGUI_API void TextColored(const String& text, const Color& color)
	{
		ImGui::TextColored(ImVec4(color.r, color.g, color.b, color.a), text.GetCString());
	}

	COREGUI_API bool Button(const String& label, const Vec2& size, ButtonFlags flags)
    {
        return ImGui::ButtonEx(label.GetCString(), ImVec2(size.x, size.y), (ImGuiButtonFlags)flags);
    }

    COREGUI_API void InvisibleButton(const String& id, const Vec2& size)
    {
        ImGui::InvisibleButton(id.GetCString(), ImVec2(size.x, size.y));
    }

#pragma endregion

#pragma region Layout

	COREGUI_API void SameLine(f32 offsetFromStart, f32 spacing)
	{
		ImGui::SameLine(offsetFromStart, spacing);
	}

	COREGUI_API void BeginGroup()
	{
		ImGui::BeginGroup();
	}

	COREGUI_API void EndGroup()
	{
		ImGui::EndGroup();
	}

    COREGUI_API Vec2 CalculateTextSize(const char* text)
    {
        ImVec2 size = ImGui::CalcTextSize(text);
        return Vec2(size.x, size.y);
    }

    COREGUI_API Vec2 CalculateTextSize(const String& text)
    {
        return CalculateTextSize(text.GetCString());
    }

    COREGUI_API Vec2 GetItemRectSize()
    {
        ImVec2 vec = ImGui::GetItemRectSize();
        return Vec2(vec.x, vec.y);
    }

#pragma endregion

#pragma region Events

	COREGUI_API bool IsWindowHovered(HoveredFlags flags)
	{
		return ImGui::IsWindowHovered(flags);
	}

	COREGUI_API bool IsItemHovered(HoveredFlags flags)
	{
		return ImGui::IsItemHovered(flags);
	}

	COREGUI_API bool IsItemClicked(MouseButton button)
	{
		return ImGui::IsItemClicked((int)button);
	}

	COREGUI_API bool IsItemFocused()
	{
		ImGui::GetID("");
		return ImGui::IsItemFocused();
	}

	COREGUI_API bool IsItemVisible()
	{
		return ImGui::IsItemVisible();
	}

	COREGUI_API bool IsItemActive()
	{
		return ImGui::IsItemActive();
	}

#pragma endregion

#pragma region Custom Drawing

    COREGUI_API void DrawRect(const Vec4& rect, const Color& color, Vec4 rounding)
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        if (drawList == nullptr)
            return;
        
        if (rounding.x < 0.5f && rounding.y < 0.5f && rounding.z < 0.5f && rounding.w < 0.5f)
        {
            drawList->AddRect(ImVec2(rect.x, rect.y), ImVec2(rect.z, rect.w), color.ToU32());
            return;
        }
        
        const float roundingTL = rounding.x;
        const float roundingTR = rounding.y;
        const float roundingBR = rounding.z;
        const float roundingBL = rounding.w;
        drawList->PathArcToFast(ImVec2(rect.x + roundingTL, rect.y + roundingTL), roundingTL, 6, 9);
        drawList->PathArcToFast(ImVec2(rect.z - roundingTR, rect.y + roundingTR), roundingTR, 9, 12);
        drawList->PathArcToFast(ImVec2(rect.z - roundingBR, rect.w - roundingBR), roundingBR, 0, 3);
        drawList->PathArcToFast(ImVec2(rect.x + roundingBL, rect.w - roundingBL), roundingBL, 3, 6);
        drawList->PathStroke(color.ToU32());
    }
    
    COREGUI_API void FillRect(const Vec4& rect, const Color& color, Vec4 rounding)
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        if (drawList == nullptr)
            return;
        
        if (rounding.x < 0.5f && rounding.y < 0.5f && rounding.z < 0.5f && rounding.w < 0.5f)
        {
            drawList->PrimReserve(6, 4);
            drawList->PrimRect(ImVec2(rect.x, rect.y), ImVec2(rect.z, rect.w), color.ToU32());
            return;
        }
        
        const float roundingTL = rounding.x;
        const float roundingTR = rounding.y;
        const float roundingBR = rounding.z;
        const float roundingBL = rounding.w;
        drawList->PathArcToFast(ImVec2(rect.x + roundingTL, rect.y + roundingTL), roundingTL, 6, 9);
        drawList->PathArcToFast(ImVec2(rect.z - roundingTR, rect.y + roundingTR), roundingTR, 9, 12);
        drawList->PathArcToFast(ImVec2(rect.z - roundingBR, rect.w - roundingBR), roundingBR, 0, 3);
        drawList->PathArcToFast(ImVec2(rect.x + roundingBL, rect.w - roundingBL), roundingBL, 3, 6);
        drawList->PathFillConvex(color.ToU32());
    }

#pragma endregion

} // namespace CE

