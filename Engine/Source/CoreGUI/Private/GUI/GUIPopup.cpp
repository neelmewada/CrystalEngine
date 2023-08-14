#include "CoreGUIInternal.h"

namespace CE::GUI
{
	COREGUI_API void OpenPopup(ID id, PopupFlags flags)
	{
		ImGui::OpenPopupEx(id, flags);
	}

	COREGUI_API bool IsPopupOpen(ID id, PopupFlags flags)
	{
		return ImGui::IsPopupOpen(id, flags);
	}

	COREGUI_API void CloseCurrentPopup()
	{
		ImGui::CloseCurrentPopup();
	}

	COREGUI_API bool BeginPopup(const String& title, ID id, WindowFlags flags)
	{
		ImGuiContext& g = *GImGui;
		if (g.OpenPopupStack.Size <= g.BeginPopupStack.Size) // Early out for performance
		{
			g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
			return false;
		}

		flags |= WF_AlwaysAutoResize | WF_NoTitleBar | WF_NoSavedSettings;
		
		if (!ImGui::IsPopupOpen(id, ImGuiPopupFlags_None))
		{
			g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
			return false;
		}
		
		char name[28];
		if (flags & WF_ChildMenu)
			ImFormatString(name, IM_ARRAYSIZE(name), "##Menu_%02d", g.BeginMenuCount); // Recycle windows based on depth
		else
			ImFormatString(name, IM_ARRAYSIZE(name), "##Popup_%08x", id); // Not recycling, so we can close/open during the same frame

		flags |= WF_Popup | WF_NoDocking;

		bool is_open = ImGui::Begin(name, NULL, flags);
		if (!is_open) // NB: Begin can return false when the popup is completely clipped (e.g. zero size display)
			GUI::EndPopup();

		return is_open;
	}

	COREGUI_API void EndPopup()
	{
		ImGui::EndPopup();
	}

    COREGUI_API bool BeginModalPopup(const String& title, ID id, bool* isShown, WindowFlags windowFlags)
    {
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		if (!ImGui::IsPopupOpen(id, ImGuiPopupFlags_None))
		{
			g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
			return false;
		}

		// Center modal windows by default for increased visibility
		// (this won't really last as settings will kick in, and is mostly for backward compatibility. user may do the same themselves)
		// FIXME: Should test for (PosCond & window->SetWindowPosAllowFlags) with the upcoming window.
		if ((g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasPos) == 0)
		{
			const ImGuiViewport* viewport = window->WasActive ? window->Viewport : ImGui::GetMainViewport(); // FIXME-VIEWPORT: What may be our reference viewport?
			ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
		}

		ImGuiWindowFlags flags = windowFlags;

		flags |= ImGuiWindowFlags_Popup | ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
		const bool is_open = ImGui::Begin(title.GetCString(), isShown, flags);
		if (!is_open || (isShown && !*isShown)) // NB: isShown can be 'false' when the popup is completely clipped (e.g. zero size display)
		{
			EndPopup();
			if (is_open)
				ImGui::ClosePopupToLevel(g.BeginPopupStack.Size, true);
			return false;
		}
		return is_open;
    }

    COREGUI_API void EndModalPopup()
    {
		ImGui::EndPopup();
    }

} // namespace CE::GUI
