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

} // namespace CE::GUI
