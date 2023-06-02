
#include "CoreGUI.h"

// Import Dear ImGui from VulkanRHI
#define IMGUI_API DLL_IMPORT
#include "imgui.h"
#include "imgui_internal.h"

namespace CE::GUI
{

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

    COREGUI_API bool Button(const String& label, const Vec2& size, ButtonFlags flags)
    {
        return ImGui::ButtonEx(label.GetCString(), ImVec2(size.x, size.y), (ImGuiButtonFlags)flags);
    }

	COREGUI_API void SameLine(f32 offsetFromStart, f32 spacing)
	{
		ImGui::SameLine(offsetFromStart, spacing);
	}

#pragma endregion

} // namespace CE

