
#include "CoreGUI.h"

#define IMGUI_DEFINE_MATH_OPERATORS

// Import Dear ImGui from VulkanRHI
#define IMGUI_API DLL_IMPORT
#include "imgui.h"
#include "imgui_internal.h"

#include "misc/cpp/imgui_stdlib.h"

#include "imstb_textedit.h"
#include "imstb_rectpack.h"

#define IM_NORMALIZE2F_OVER_ZERO(VX,VY)     { float d2 = VX*VX + VY*VY; if (d2 > 0.0f) { float inv_len = ImRsqrt(d2); VX *= inv_len; VY *= inv_len; } } (void)0
#define IM_FIXNORMAL2F_MAX_INVLEN2          100.0f // 500.0f (see #4053, #3366)
#define IM_FIXNORMAL2F(VX,VY)               { float d2 = VX*VX + VY*VY; if (d2 > 0.000001f) { float inv_len2 = 1.0f / d2; if (inv_len2 > IM_FIXNORMAL2F_MAX_INVLEN2) inv_len2 = IM_FIXNORMAL2F_MAX_INVLEN2; VX *= inv_len2; VY *= inv_len2; } } (void)0


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

	COREGUI_API Vec2 GetWindowPos()
	{
		ImVec2 pos = ImGui::GetWindowPos();
		return Vec2(pos.x, pos.y);
	}

	COREGUI_API Vec2 GetWindowSize()
	{
		ImVec2 pos = ImGui::GetWindowSize();
		return Vec2(pos.x, pos.y);
	}

	COREGUI_API Vec4 GetWindowRect()
	{
		auto pos = GetWindowPos();
		auto size = GetWindowSize();
		return Vec4(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
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

	COREGUI_API bool ButtonEx(const String& label, const Vec4& padding, const Vec2& sizeVec, const Vec4& rounding, 
		TextAlign textAlign, Vec2 minSize, Vec2 maxSize, ButtonFlags buttonFlags)
	{
		return ButtonEx(label, {}, {}, {}, padding, sizeVec, rounding, textAlign, minSize, maxSize, buttonFlags);
	}

	COREGUI_API bool ButtonEx(const String& label, 
		const StyleColor& normalColor, const StyleColor& hoveredColor, const StyleColor& pressedColor, 
		const Vec4& padding, const Vec2& sizeVec, const Vec4& rounding,
		TextAlign textAlign, Vec2 minSize, Vec2 maxSize, ButtonFlags buttonFlags)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		ImVec4 finalPadding{ padding.x, padding.y, padding.z, padding.w };
		finalPadding.x = ImMax(padding.x, style.FramePadding.x);
		finalPadding.y = ImMax(padding.y, style.FramePadding.y);
		finalPadding.z = ImMax(padding.z, style.FramePadding.x);
		finalPadding.w = ImMax(padding.w, style.FramePadding.y);

		int flags = (int)buttonFlags;

		const ImGuiID id = window->GetID(label.GetCString());
		const ImVec2 label_size = ImGui::CalcTextSize(label.GetCString(), NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		if (((int)flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
			pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
		
		ImVec2 size = ImGui::CalcItemSize(ImVec2(sizeVec.x, sizeVec.y),
			label_size.x + style.FramePadding.x * 2.0f,
			label_size.y + style.FramePadding.y * 2.0f);

		if (maxSize.x > 0 && size.x > maxSize.x)
			size.x = maxSize.x;
		if (maxSize.y > 0 && size.y > maxSize.y)
			size.y = maxSize.y;
		if (minSize.x > 0 && size.x < minSize.x)
			size.x = minSize.x;
		if (minSize.y > 0 && size.y < minSize.y)
			size.y = minSize.y;

		const ImRect bb(pos, pos + size + ImVec2(padding.x + padding.z, padding.y + padding.w));
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat)
			flags |= ImGuiButtonFlags_Repeat;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

		// Render
		ImGui::RenderNavHighlight(bb, id);

		const StyleColor& colorValue = (held && hovered) ? pressedColor : hovered ? hoveredColor : normalColor;
		if (colorValue.IsEmpty())
		{
			const ImVec4& col = ImGui::GetStyleColorVec4((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
			GUI::FillRect(Vec4(bb.Min.x, bb.Min.y, bb.Max.x, bb.Max.y), Color(col.x, col.y, col.z, col.w), rounding);
		}
		else if (colorValue.isColor)
		{
			GUI::FillRect(Vec4(bb.Min.x, bb.Min.y, bb.Max.x, bb.Max.y), colorValue.color, rounding);
		}
		else if (colorValue.isGradient)
		{
			GUI::FillRect(Vec4(bb.Min.x, bb.Min.y, bb.Max.x, bb.Max.y), colorValue.gradient, rounding);
		}

		ImVec2 align = style.ButtonTextAlign;
		if (textAlign == TextAlign_TopLeft)
		{
			align = ImVec2(0, 0);
		}
		else if (textAlign == TextAlign_TopCenter)
		{
			align = ImVec2(0.5f, 0);
		}
		else if (textAlign == TextAlign_TopRight)
		{
			align = ImVec2(1, 0);
		}
		else if (textAlign == TextAlign_MiddleLeft)
		{
			align = ImVec2(0, 0.5f);
		}
		else if (textAlign == TextAlign_MiddleCenter)
		{
			align = ImVec2(0.5f, 0.5f);
		}
		else if (textAlign == TextAlign_MiddleRight)
		{
			align = ImVec2(1, 0.5f);
		}
		else if (textAlign == TextAlign_BottomLeft)
		{
			align = ImVec2(0, 1);
		}
		else if (textAlign == TextAlign_BottomCenter)
		{
			align = ImVec2(0.5f, 1);
		}
		else if (textAlign == TextAlign_BottomRight)
		{
			align = ImVec2(1, 1);
		}

		if (g.LogEnabled)
			ImGui::LogSetNextTextDecoration("[", "]");
		ImGui::RenderTextClipped(bb.Min + style.FramePadding + ImVec2(padding.x, padding.y),
			bb.Max - style.FramePadding - ImVec2(padding.z, padding.y),
			label.GetCString(), NULL, &label_size, align, &bb);

		// Automatically close popups
		//if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
		//    CloseCurrentPopup();

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label.GetCString(), g.LastItemData.StatusFlags);
		return pressed;
	}

    COREGUI_API void InvisibleButton(const String& id, const Vec2& size)
    {
        ImGui::InvisibleButton(id.GetCString(), ImVec2(size.x, size.y));
    }

	COREGUI_API bool Checkbox(const String& label, bool* value)
	{
		return ImGui::Checkbox(label.GetCString(), value);
	}

	COREGUI_API bool CheckboxTriState(ID id, s8* v, const Vec4& padding, const Vec4& rounding, f32 borderThickness)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		const float square_sz = g.FontSize + padding.top + padding.bottom;
		const ImVec2 pos = window->DC.CursorPos;
		const ImRect total_bb(pos, 
			pos + ImVec2(square_sz, square_sz + padding.top + padding.bottom));
		ImGui::ItemSize(total_bb, padding.y);
		if (!ImGui::ItemAdd(total_bb, id))
		{
			IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
			return false;
		}

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
		if (pressed)
		{
			if (*v > 0) *v = 0;
			else *v = 1;
			ImGui::MarkItemEdited(id);
		}

		const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
		Vec4 mainRect = Vec4(check_bb.Min.x, check_bb.Min.y, check_bb.Max.x, check_bb.Max.y);

		//ImGui::RenderNavHighlight(total_bb, id);
		//ImGui::RenderFrame(check_bb.Min, check_bb.Max, ImGui::GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, style.FrameRounding);
		
		RenderNavHighlight(mainRect, id, borderThickness, rounding);
		RenderFrame(mainRect, ImGui::GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), borderThickness, rounding);

		
		ImU32 check_col = ImGui::GetColorU32(ImGuiCol_CheckMark);
		bool mixed_value = (g.LastItemData.InFlags & ImGuiItemFlags_MixedValue) != 0;
		if (*v < 0) // Mixed state
		{
			// Undocumented tristate/mixed/indeterminate checkbox (#2644)
			// This may seem awkwardly designed because the aim is to make ImGuiItemFlags_MixedValue supported by all widgets (not just checkbox)
			ImVec2 pad(ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)), ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)));
			window->DrawList->AddRectFilled(check_bb.Min + pad, check_bb.Max - pad, check_col, 0);
		}
		else if (*v > 0) // Checked state
		{
			const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));
			ImGui::RenderCheckMark(window->DrawList, check_bb.Min + ImVec2(pad, pad), check_col, square_sz - pad * 2.0f);
		}

		ImVec2 label_pos = ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + padding.top);
		if (g.LogEnabled)
			ImGui::LogRenderedText(&label_pos, mixed_value ? "[~]" : *v ? "[x]" : "[ ]");

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
		return pressed;
	}

    static int InputTextCalcTextLenAndLineCount(const char* text_begin, const char** out_text_end)
    {
        int line_count = 0;
        const char* s = text_begin;
        while (char c = *s++) // We are only matching for \n so we can ignore UTF-8 decoding
            if (c == '\n')
                line_count++;
        s--;
        if (s[0] != '\n' && s[0] != '\r')
            line_count++;
        *out_text_end = s;
        return line_count;
    }

    static ImVec2 InputTextCalcTextSizeW(ImGuiContext* ctx, const ImWchar* text_begin, const ImWchar* text_end, const ImWchar** remaining, ImVec2* out_offset, bool stop_on_new_line)
    {
        ImGuiContext& g = *ctx;
        ImFont* font = g.Font;
        const float line_height = g.FontSize;
        const float scale = line_height / font->FontSize;

        ImVec2 text_size = ImVec2(0, 0);
        float line_width = 0.0f;

        const ImWchar* s = text_begin;
        while (s < text_end)
        {
            unsigned int c = (unsigned int)(*s++);
            if (c == '\n')
            {
                text_size.x = ImMax(text_size.x, line_width);
                text_size.y += line_height;
                line_width = 0.0f;
                if (stop_on_new_line)
                    break;
                continue;
            }
            if (c == '\r')
                continue;

            const float char_width = font->GetCharAdvance((ImWchar)c) * scale;
            line_width += char_width;
        }

        if (text_size.x < line_width)
            text_size.x = line_width;

        if (out_offset)
            *out_offset = ImVec2(line_width, text_size.y + line_height);  // offset allow for the possibility of sitting after a trailing \n

        if (line_width > 0 || text_size.y == 0.0f)                        // whereas size.y will ignore the trailing \n
            text_size.y += line_height;

        if (remaining)
            *remaining = s;

        return text_size;
    }

	COREGUI_API bool InputTextEx(ID id, const String& hint, String& inputText, const Vec2& size, const Vec4& padding, const Vec4& rounding, f32 borderThickness, Vec2 minSize, Vec2 maxSize, TextInputFlags flags, TextInputCallback callback, void* callback_user_data)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackHistory) && (flags & ImGuiInputTextFlags_Multiline)));        // Can't use both together (they both use up/down keys)
		IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackCompletion) && (flags & ImGuiInputTextFlags_AllowTabInput))); // Can't use both together (they both use tab key)

		ImGuiContext& g = *GImGui;
		ImGuiIO& io = g.IO;
		const ImGuiStyle& style = g.Style;
        
        const bool RENDER_SELECTION_WHEN_INACTIVE = false;
        const bool is_multiline = (flags & ImGuiInputTextFlags_Multiline) != 0;
        const bool is_readonly = (flags & ImGuiInputTextFlags_ReadOnly) != 0;
        const bool is_password = (flags & ImGuiInputTextFlags_Password) != 0;
        const bool is_undoable = (flags & ImGuiInputTextFlags_NoUndoRedo) == 0;
        const bool is_resizable = (flags & ImGuiInputTextFlags_CallbackResize) != 0;
        if (is_resizable)
            IM_ASSERT(callback != NULL); // Must provide a callback if you set the ImGuiInputTextFlags_CallbackResize flag!

        if (is_multiline) // Open group before calling GetID() because groups tracks id created within their scope (including the scrollbar)
            BeginGroup();
        
        const ImVec2 label_size = ImGui::CalcTextSize("Ignore", NULL, true);
        ImVec2 size_arg = ImVec2(size.x, size.y);
        
        const ImVec2 frame_size = ImGui::CalcItemSize(size_arg, ImGui::CalcItemWidth(),
          (is_multiline ? g.FontSize * 8.0f : label_size.y) + padding.top + padding.bottom); // Arbitrary default of 8 lines high for multi-line
        const ImVec2 total_size = ImVec2(frame_size.x, frame_size.y);

        const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);
        const ImRect total_bb(frame_bb.Min, frame_bb.Min + total_size);

        ImGuiWindow* draw_window = window;
        ImVec2 inner_size = frame_size;
        ImGuiItemStatusFlags item_status_flags = 0;
        ImGuiLastItemData item_data_backup;
        if (is_multiline)
        {
            ImVec2 backup_pos = window->DC.CursorPos;
            ImGui::ItemSize(total_bb, style.FramePadding.y);
            if (!ImGui::ItemAdd(total_bb, id, &frame_bb, ImGuiItemFlags_Inputable))
            {
                EndGroup();
                return false;
            }
            item_status_flags = g.LastItemData.StatusFlags;
            item_data_backup = g.LastItemData;
            window->DC.CursorPos = backup_pos;

            // We reproduce the contents of BeginChildFrame() in order to provide 'label' so our window internal data are easier to read/debug.
            // FIXME-NAV: Pressing NavActivate will trigger general child activation right before triggering our own below. Harmless but bizarre.
            ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_FrameBg]);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, style.FrameRounding);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, style.FrameBorderSize);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // Ensure no clip rect so mouse hover can reach FramePadding edges
            bool child_visible = ImGui::BeginChildEx(nullptr, id, frame_bb.GetSize(), true, ImGuiWindowFlags_NoMove);
            PopStyleVar(3);
            PopStyleColor();
            if (!child_visible)
            {
                ImGui::EndChild();
                EndGroup();
                return false;
            }
            draw_window = g.CurrentWindow; // Child window
            draw_window->DC.NavLayersActiveMaskNext |= (1 << draw_window->DC.NavLayerCurrent); // This is to ensure that EndChild() will display a navigation highlight so we can "enter" into it.
            draw_window->DC.CursorPos += style.FramePadding;
            inner_size.x -= draw_window->ScrollbarSizes.x;
        }
        else
        {
            // Support for internal ImGuiInputTextFlags_MergedItem flag, which could be redesigned as an ItemFlags if needed (with test performed in ItemAdd)
            ImGui::ItemSize(total_bb, style.FramePadding.y);
            if (!(flags & ImGuiInputTextFlags_MergedItem))
                if (!ImGui::ItemAdd(total_bb, id, &frame_bb, ImGuiItemFlags_Inputable))
                    return false;
            item_status_flags = g.LastItemData.StatusFlags;
        }
        const bool hovered = ImGui::ItemHoverable(frame_bb, id);
        if (hovered)
            g.MouseCursor = ImGuiMouseCursor_TextInput;

        // We are only allowed to access the state if we are already the active widget.
        ImGuiInputTextState* state = ImGui::GetInputTextState(id);

        const bool input_requested_by_tabbing = (item_status_flags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
        const bool input_requested_by_nav = (g.ActiveId != id) && ((g.NavActivateId == id) && ((g.NavActivateFlags & ImGuiActivateFlags_PreferInput) || (g.NavInputSource == ImGuiInputSource_Keyboard)));

        const bool user_clicked = hovered && io.MouseClicked[0];
        const bool user_scroll_finish = is_multiline && state != NULL && g.ActiveId == 0 && g.ActiveIdPreviousFrame == ImGui::GetWindowScrollbarID(draw_window, ImGuiAxis_Y);
        const bool user_scroll_active = is_multiline && state != NULL && g.ActiveId == ImGui::GetWindowScrollbarID(draw_window, ImGuiAxis_Y);
        bool clear_active_id = false;
        bool select_all = false;

        float scroll_y = is_multiline ? draw_window->Scroll.y : FLT_MAX;
        
        char* buf = inputText.GetCString();
        int buf_size = inputText.GetLength();

        const bool init_changed_specs = (state != NULL && state->Stb.single_line != !is_multiline); // state != NULL means its our state.
        const bool init_make_active = (user_clicked || user_scroll_finish || input_requested_by_nav || input_requested_by_tabbing);
        const bool init_state = (init_make_active || user_scroll_active);
        if ((init_state && g.ActiveId != id) || init_changed_specs)
        {
            // Access state even if we don't own it yet.
            state = &g.InputTextState;
            state->CursorAnimReset();

            // Backup state of deactivating item so they'll have a chance to do a write to output buffer on the same frame they report IsItemDeactivatedAfterEdit (#4714)
            ImGui::InputTextDeactivateHook(state->ID);

            // Take a copy of the initial buffer value (both in original UTF-8 format and converted to wchar)
            // From the moment we focused we are ignoring the content of 'buf' (unless we are in read-only mode)
            const int buf_len = inputText.GetCapacity();
            
            state->InitialTextA.resize(buf_len + 1);    // UTF-8. we use +1 to make sure that .Data is always pointing to at least an empty string.
            memcpy(state->InitialTextA.Data, buf, buf_len + 1);

            // Preserve cursor position and undo/redo stack if we come back to same widget
            // FIXME: Since we reworked this on 2022/06, may want to differenciate recycle_cursor vs recycle_undostate?
            bool recycle_state = (state->ID == id && !init_changed_specs);
            if (recycle_state && (state->CurLenA != buf_len || (state->TextAIsValid && strncmp(state->TextA.Data, buf, buf_len) != 0)))
                recycle_state = false;

            // Start edition
            const char* buf_end = NULL;
            state->ID = id;
            state->TextW.resize(buf_size + 1);          // wchar count <= UTF-8 count. we use +1 to make sure that .Data is always pointing to at least an empty string.
            state->TextA.resize(0);
            state->TextAIsValid = false;                // TextA is not valid yet (we will display buf until then)
            state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, buf_size, buf, NULL, &buf_end);
            state->CurLenA = (int)(buf_end - buf);      // We can't get the result from ImStrncpy() above because it is not UTF-8 aware. Here we'll cut off malformed UTF-8.

            if (recycle_state)
            {
                // Recycle existing cursor/selection/undo stack but clamp position
                // Note a single mouse click will override the cursor/position immediately by calling stb_textedit_click handler.
                state->CursorClamp();
            }
            else
            {
                state->ScrollX = 0.0f;
                stb_textedit_initialize_state(&state->Stb, !is_multiline);
            }

            if (!is_multiline)
            {
                if (flags & ImGuiInputTextFlags_AutoSelectAll)
                    select_all = true;
                if (input_requested_by_nav && (!recycle_state || !(g.NavActivateFlags & ImGuiActivateFlags_TryToPreserveState)))
                    select_all = true;
                if (input_requested_by_tabbing || (user_clicked && io.KeyCtrl))
                    select_all = true;
            }

            if (flags & ImGuiInputTextFlags_AlwaysOverwrite)
                state->Stb.insert_mode = 1; // stb field name is indeed incorrect (see #2863)
        }

        const bool is_osx = io.ConfigMacOSXBehaviors;
        if (g.ActiveId != id && init_make_active)
        {
            IM_ASSERT(state && state->ID == id);
            ImGui::SetActiveID(id, window);
            ImGui::SetFocusID(id, window);
            ImGui::FocusWindow(window);
        }
        if (g.ActiveId == id)
        {
            // Declare some inputs, the other are registered and polled via Shortcut() routing system.
            if (user_clicked)
                ImGui::SetKeyOwner(ImGuiKey_MouseLeft, id);
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
            if (is_multiline || (flags & ImGuiInputTextFlags_CallbackHistory))
                g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Up) | (1 << ImGuiDir_Down);
            ImGui::SetKeyOwner(ImGuiKey_Home, id);
            ImGui::SetKeyOwner(ImGuiKey_End, id);
            if (is_multiline)
            {
                ImGui::SetKeyOwner(ImGuiKey_PageUp, id);
                ImGui::SetKeyOwner(ImGuiKey_PageDown, id);
            }
            if (is_osx)
                ImGui::SetKeyOwner(ImGuiMod_Alt, id);
            if (flags & (ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_AllowTabInput)) // Disable keyboard tabbing out as we will use the \t character.
                ImGui::SetShortcutRouting(ImGuiKey_Tab, id);
        }

        // We have an edge case if ActiveId was set through another widget (e.g. widget being swapped), clear id immediately (don't wait until the end of the function)
        if (g.ActiveId == id && state == NULL)
            ImGui::ClearActiveID();

        // Release focus when we click outside
        if (g.ActiveId == id && io.MouseClicked[0] && !init_state && !init_make_active) //-V560
            clear_active_id = true;

        // Lock the decision of whether we are going to take the path displaying the cursor or selection
        bool render_cursor = (g.ActiveId == id) || (state && user_scroll_active);
        bool render_selection = state && (state->HasSelection() || select_all) && (RENDER_SELECTION_WHEN_INACTIVE || render_cursor);
        bool value_changed = false;
        bool validated = false;

        // When read-only we always use the live data passed to the function
        // FIXME-OPT: Because our selection/cursor code currently needs the wide text we need to convert it when active, which is not ideal :(
        if (is_readonly && state != NULL && (render_cursor || render_selection))
        {
            const char* buf_end = NULL;
            state->TextW.resize(buf_size + 1);
            state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, state->TextW.Size, buf, NULL, &buf_end);
            state->CurLenA = (int)(buf_end - buf);
            state->CursorClamp();
            render_selection &= state->HasSelection();
        }

        // Select the buffer to render.
        const bool buf_display_from_state = (render_cursor || render_selection || g.ActiveId == id) && !is_readonly && state && state->TextAIsValid;
        const bool is_displaying_hint = (hint != NULL && (buf_display_from_state ? state->TextA.Data : buf)[0] == 0);

        // Password pushes a temporary font with only a fallback glyph
        if (is_password && !is_displaying_hint)
        {
            const ImFontGlyph* glyph = g.Font->FindGlyph('*');
            ImFont* password_font = &g.InputTextPasswordFont;
            password_font->FontSize = g.Font->FontSize;
            password_font->Scale = g.Font->Scale;
            password_font->Ascent = g.Font->Ascent;
            password_font->Descent = g.Font->Descent;
            password_font->ContainerAtlas = g.Font->ContainerAtlas;
            password_font->FallbackGlyph = glyph;
            password_font->FallbackAdvanceX = glyph->AdvanceX;
            IM_ASSERT(password_font->Glyphs.empty() && password_font->IndexAdvanceX.empty() && password_font->IndexLookup.empty());
            PushFont(password_font);
        }

        // Process mouse inputs and character inputs
        int backup_current_text_length = 0;
        if (g.ActiveId == id)
        {
            IM_ASSERT(state != NULL);
            backup_current_text_length = state->CurLenA;
            state->Edited = false;
            state->BufCapacityA = buf_size;
            state->Flags = flags;

            // Although we are active we don't prevent mouse from hovering other elements unless we are interacting right now with the widget.
            // Down the line we should have a cleaner library-wide concept of Selected vs Active.
            g.ActiveIdAllowOverlap = !io.MouseDown[0];

            // Edit in progress
            const float mouse_x = (io.MousePos.x - frame_bb.Min.x - style.FramePadding.x) + state->ScrollX;
            const float mouse_y = (is_multiline ? (io.MousePos.y - draw_window->DC.CursorPos.y) : (g.FontSize * 0.5f));

            if (select_all)
            {
                state->SelectAll();
                state->SelectedAllMouseLock = true;
            }
            else if (hovered && io.MouseClickedCount[0] >= 2 && !io.KeyShift)
            {
                stb_textedit_click(state, &state->Stb, mouse_x, mouse_y);
                const int multiclick_count = (io.MouseClickedCount[0] - 2);
                if ((multiclick_count % 2) == 0)
                {
                    // Double-click: Select word
                    // We always use the "Mac" word advance for double-click select vs CTRL+Right which use the platform dependent variant:
                    // FIXME: There are likely many ways to improve this behavior, but there's no "right" behavior (depends on use-case, software, OS)
                    const bool is_bol = (state->Stb.cursor == 0) || ImStb::STB_TEXTEDIT_GETCHAR(state, state->Stb.cursor - 1) == '\n';
                    if (STB_TEXT_HAS_SELECTION(&state->Stb) || !is_bol)
                        state->OnKeyPressed(STB_TEXTEDIT_K_WORDLEFT);
                    //state->OnKeyPressed(STB_TEXTEDIT_K_WORDRIGHT | STB_TEXTEDIT_K_SHIFT);
                    if (!STB_TEXT_HAS_SELECTION(&state->Stb))
                        ImStb::stb_textedit_prep_selection_at_cursor(&state->Stb);
                    state->Stb.cursor = ImStb::STB_TEXTEDIT_MOVEWORDRIGHT_MAC(state, state->Stb.cursor);
                    state->Stb.select_end = state->Stb.cursor;
                    ImStb::stb_textedit_clamp(state, &state->Stb);
                }
                else
                {
                    // Triple-click: Select line
                    const bool is_eol = ImStb::STB_TEXTEDIT_GETCHAR(state, state->Stb.cursor) == '\n';
                    state->OnKeyPressed(STB_TEXTEDIT_K_LINESTART);
                    state->OnKeyPressed(STB_TEXTEDIT_K_LINEEND | STB_TEXTEDIT_K_SHIFT);
                    state->OnKeyPressed(STB_TEXTEDIT_K_RIGHT | STB_TEXTEDIT_K_SHIFT);
                    if (!is_eol && is_multiline)
                    {
                        ImSwap(state->Stb.select_start, state->Stb.select_end);
                        state->Stb.cursor = state->Stb.select_end;
                    }
                    state->CursorFollow = false;
                }
                state->CursorAnimReset();
            }
            else if (io.MouseClicked[0] && !state->SelectedAllMouseLock)
            {
                if (hovered)
                {
                    if (io.KeyShift)
                        stb_textedit_drag(state, &state->Stb, mouse_x, mouse_y);
                    else
                        stb_textedit_click(state, &state->Stb, mouse_x, mouse_y);
                    state->CursorAnimReset();
                }
            }
            else if (io.MouseDown[0] && !state->SelectedAllMouseLock && (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f))
            {
                stb_textedit_drag(state, &state->Stb, mouse_x, mouse_y);
                state->CursorAnimReset();
                state->CursorFollow = true;
            }
            if (state->SelectedAllMouseLock && !io.MouseDown[0])
                state->SelectedAllMouseLock = false;

            // We expect backends to emit a Tab key but some also emit a Tab character which we ignore (#2467, #1336)
            // (For Tab and Enter: Win32/SFML/Allegro are sending both keys and chars, GLFW and SDL are only sending keys. For Space they all send all threes)
            if ((flags & ImGuiInputTextFlags_AllowTabInput) && Shortcut(ImGuiKey_Tab, id) && !is_readonly)
            {
                unsigned int c = '\t'; // Insert TAB
                if (InputTextFilterCharacter(&c, flags, callback, callback_user_data, ImGuiInputSource_Keyboard))
                    state->OnKeyPressed((int)c);
            }

            // Process regular text input (before we check for Return because using some IME will effectively send a Return?)
            // We ignore CTRL inputs, but need to allow ALT+CTRL as some keyboards (e.g. German) use AltGR (which _is_ Alt+Ctrl) to input certain characters.
            const bool ignore_char_inputs = (io.KeyCtrl && !io.KeyAlt) || (is_osx && io.KeySuper);
            if (io.InputQueueCharacters.Size > 0)
            {
                if (!ignore_char_inputs && !is_readonly && !input_requested_by_nav)
                    for (int n = 0; n < io.InputQueueCharacters.Size; n++)
                    {
                        // Insert character if they pass filtering
                        unsigned int c = (unsigned int)io.InputQueueCharacters[n];
                        if (c == '\t') // Skip Tab, see above.
                            continue;
                        if (InputTextFilterCharacter(&c, flags, callback, callback_user_data, ImGuiInputSource_Keyboard))
                            state->OnKeyPressed((int)c);
                    }

                // Consume characters
                io.InputQueueCharacters.resize(0);
            }
        }

        // Process other shortcuts/key-presses
        bool revert_edit = false;
        if (g.ActiveId == id && !g.ActiveIdIsJustActivated && !clear_active_id)
        {
            IM_ASSERT(state != NULL);
            
            const int row_count_per_page = ImMax((int)((inner_size.y - style.FramePadding.y) / g.FontSize), 1);
            state->Stb.row_count_per_page = row_count_per_page;

            const int k_mask = (io.KeyShift ? STB_TEXTEDIT_K_SHIFT : 0);
            const bool is_wordmove_key_down = is_osx ? io.KeyAlt : io.KeyCtrl;                     // OS X style: Text editing cursor movement using Alt instead of Ctrl
            const bool is_startend_key_down = is_osx && io.KeySuper && !io.KeyCtrl && !io.KeyAlt;  // OS X style: Line/Text Start and End using Cmd+Arrows instead of Home/End
            
            // Using Shortcut() with ImGuiInputFlags_RouteFocused (default policy) to allow routing operations for other code (e.g. calling window trying to use CTRL+A and CTRL+B: formet would be handled by InputText)
            // Otherwise we could simply assume that we own the keys as we are active.
            const ImGuiInputFlags f_repeat = ImGuiInputFlags_Repeat;
            const bool is_cut   = (ImGui::Shortcut(ImGuiMod_Shortcut | ImGuiKey_X, id, f_repeat) ||
                                   ImGui::Shortcut(ImGuiMod_Shift | ImGuiKey_Delete, id, f_repeat)) && !is_readonly && !is_password && (!is_multiline || state->HasSelection());
            const bool is_copy  = (ImGui::Shortcut(ImGuiMod_Shortcut | ImGuiKey_C, id) ||
                                   ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Insert, id))  && !is_password && (!is_multiline || state->HasSelection());
            const bool is_paste = (ImGui::Shortcut(ImGuiMod_Shortcut | ImGuiKey_V, id, f_repeat) ||
                                   ImGui::Shortcut(ImGuiMod_Shift | ImGuiKey_Insert, id, f_repeat)) && !is_readonly;
            const bool is_undo  = (ImGui::Shortcut(ImGuiMod_Shortcut | ImGuiKey_Z, id, f_repeat)) && !is_readonly && is_undoable;
            const bool is_redo =  (ImGui::Shortcut(ImGuiMod_Shortcut | ImGuiKey_Y, id, f_repeat) || (is_osx && ImGui::Shortcut(ImGuiMod_Shortcut | ImGuiMod_Shift | ImGuiKey_Z, id, f_repeat))) && !is_readonly && is_undoable;
            const bool is_select_all = ImGui::Shortcut(ImGuiMod_Shortcut | ImGuiKey_A, id);

            // We allow validate/cancel with Nav source (gamepad) to makes it easier to undo an accidental NavInput press with no keyboard wired, but otherwise it isn't very useful.
            const bool nav_gamepad_active = (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) != 0 && (io.BackendFlags & ImGuiBackendFlags_HasGamepad) != 0;
            const bool is_enter_pressed = ImGui::IsKeyPressed(ImGuiKey_Enter, true) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, true);
            const bool is_gamepad_validate = nav_gamepad_active && (ImGui::IsKeyPressed(ImGuiKey_NavGamepadActivate, false) || ImGui::IsKeyPressed(ImGuiKey_NavGamepadInput, false));
            const bool is_cancel = ImGui::Shortcut(ImGuiKey_Escape, id, f_repeat) || (nav_gamepad_active && ImGui::Shortcut(ImGuiKey_NavGamepadCancel, id, f_repeat));
            
            // FIXME: Should use more Shortcut() and reduce IsKeyPressed()+SetKeyOwner(), but requires modifiers combination to be taken account of.
            if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) { state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_LINESTART : is_wordmove_key_down ? STB_TEXTEDIT_K_WORDLEFT : STB_TEXTEDIT_K_LEFT) | k_mask); }
            else if (ImGui::IsKeyPressed(ImGuiKey_RightArrow))                  { state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_LINEEND : is_wordmove_key_down ? STB_TEXTEDIT_K_WORDRIGHT : STB_TEXTEDIT_K_RIGHT) | k_mask); }
            else if (ImGui::IsKeyPressed(ImGuiKey_UpArrow) && is_multiline)     { if (io.KeyCtrl) ImGui::SetScrollY(draw_window, ImMax(draw_window->Scroll.y - g.FontSize, 0.0f)); else state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTSTART : STB_TEXTEDIT_K_UP) | k_mask); }
            else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow) && is_multiline)   { if (io.KeyCtrl) ImGui::SetScrollY(draw_window, ImMin(draw_window->Scroll.y + g.FontSize, GetScrollMaxY())); else state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTEND : STB_TEXTEDIT_K_DOWN) | k_mask); }
            else if (ImGui::IsKeyPressed(ImGuiKey_PageUp) && is_multiline)      { state->OnKeyPressed(STB_TEXTEDIT_K_PGUP | k_mask); scroll_y -= row_count_per_page * g.FontSize; }
            else if (ImGui::IsKeyPressed(ImGuiKey_PageDown) && is_multiline)    { state->OnKeyPressed(STB_TEXTEDIT_K_PGDOWN | k_mask); scroll_y += row_count_per_page * g.FontSize; }
            else if (ImGui::IsKeyPressed(ImGuiKey_Home))                        { state->OnKeyPressed(io.KeyCtrl ? STB_TEXTEDIT_K_TEXTSTART | k_mask : STB_TEXTEDIT_K_LINESTART | k_mask); }
            else if (ImGui::IsKeyPressed(ImGuiKey_End))                         { state->OnKeyPressed(io.KeyCtrl ? STB_TEXTEDIT_K_TEXTEND | k_mask : STB_TEXTEDIT_K_LINEEND | k_mask); }
            else if (ImGui::IsKeyPressed(ImGuiKey_Delete) && !is_readonly && !is_cut)
            {
                if (!state->HasSelection())
                {
                    // OSX doesn't seem to have Super+Delete to delete until end-of-line, so we don't emulate that (as opposed to Super+Backspace)
                    if (is_wordmove_key_down)
                        state->OnKeyPressed(STB_TEXTEDIT_K_WORDRIGHT | STB_TEXTEDIT_K_SHIFT);
                }
                state->OnKeyPressed(STB_TEXTEDIT_K_DELETE | k_mask);
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_Backspace) && !is_readonly)
            {
                if (!state->HasSelection())
                {
                    if (is_wordmove_key_down)
                        state->OnKeyPressed(STB_TEXTEDIT_K_WORDLEFT | STB_TEXTEDIT_K_SHIFT);
                    else if (is_osx && io.KeySuper && !io.KeyAlt && !io.KeyCtrl)
                        state->OnKeyPressed(STB_TEXTEDIT_K_LINESTART | STB_TEXTEDIT_K_SHIFT);
                }
                state->OnKeyPressed(STB_TEXTEDIT_K_BACKSPACE | k_mask);
            }
            else if (is_enter_pressed || is_gamepad_validate)
            {
                // Determine if we turn Enter into a \n character
                bool ctrl_enter_for_new_line = (flags & ImGuiInputTextFlags_CtrlEnterForNewLine) != 0;
                if (!is_multiline || is_gamepad_validate || (ctrl_enter_for_new_line && !io.KeyCtrl) || (!ctrl_enter_for_new_line && io.KeyCtrl))
                {
                    validated = true;
                    if (io.ConfigInputTextEnterKeepActive && !is_multiline)
                        state->SelectAll(); // No need to scroll
                    else
                        clear_active_id = true;
                }
                else if (!is_readonly)
                {
                    unsigned int c = '\n'; // Insert new line
                    if (InputTextFilterCharacter(&c, flags, callback, callback_user_data, ImGuiInputSource_Keyboard))
                        state->OnKeyPressed((int)c);
                }
            }
            else if (is_cancel)
            {
                if (flags & ImGuiInputTextFlags_EscapeClearsAll)
                {
                    if (state->CurLenA > 0)
                    {
                        revert_edit = true;
                    }
                    else
                    {
                        render_cursor = render_selection = false;
                        clear_active_id = true;
                    }
                }
                else
                {
                    clear_active_id = revert_edit = true;
                    render_cursor = render_selection = false;
                }
            }
            else if (is_undo || is_redo)
            {
                state->OnKeyPressed(is_undo ? STB_TEXTEDIT_K_UNDO : STB_TEXTEDIT_K_REDO);
                state->ClearSelection();
            }
            else if (is_select_all)
            {
                state->SelectAll();
                state->CursorFollow = true;
            }
            else if (is_cut || is_copy)
            {
                // Cut, Copy
                if (io.SetClipboardTextFn)
                {
                    const int ib = state->HasSelection() ? ImMin(state->Stb.select_start, state->Stb.select_end) : 0;
                    const int ie = state->HasSelection() ? ImMax(state->Stb.select_start, state->Stb.select_end) : state->CurLenW;
                    const int clipboard_data_len = ImTextCountUtf8BytesFromStr(state->TextW.Data + ib, state->TextW.Data + ie) + 1;
                    char* clipboard_data = (char*)IM_ALLOC(clipboard_data_len * sizeof(char));
                    ImTextStrToUtf8(clipboard_data, clipboard_data_len, state->TextW.Data + ib, state->TextW.Data + ie);
                    SetClipboardText(clipboard_data);
                    MemFree(clipboard_data);
                }
                if (is_cut)
                {
                    if (!state->HasSelection())
                        state->SelectAll();
                    state->CursorFollow = true;
                    stb_textedit_cut(state, &state->Stb);
                }
            }
            else if (is_paste)
            {
                if (const char* clipboard = GetClipboardText())
                {
                    // Filter pasted buffer
                    const int clipboard_len = (int)strlen(clipboard);
                    ImWchar* clipboard_filtered = (ImWchar*)IM_ALLOC((clipboard_len + 1) * sizeof(ImWchar));
                    int clipboard_filtered_len = 0;
                    for (const char* s = clipboard; *s != 0; )
                    {
                        unsigned int c;
                        s += ImTextCharFromUtf8(&c, s, NULL);
                        if (!InputTextFilterCharacter(&c, flags, callback, callback_user_data, ImGuiInputSource_Clipboard))
                            continue;
                        clipboard_filtered[clipboard_filtered_len++] = (ImWchar)c;
                    }
                    clipboard_filtered[clipboard_filtered_len] = 0;
                    if (clipboard_filtered_len > 0) // If everything was filtered, ignore the pasting operation
                    {
                        stb_textedit_paste(state, &state->Stb, clipboard_filtered, clipboard_filtered_len);
                        state->CursorFollow = true;
                    }
                    ImGui::MemFree(clipboard_filtered);
                }
            }

            // Update render selection flag after events have been handled, so selection highlight can be displayed during the same frame.
            render_selection |= state->HasSelection() && (RENDER_SELECTION_WHEN_INACTIVE || render_cursor);
        }

        // Process callbacks and apply result back to user's buffer.
        const char* apply_new_text = NULL;
        int apply_new_text_length = 0;
        if (g.ActiveId == id)
        {
            IM_ASSERT(state != NULL);
            if (revert_edit && !is_readonly)
            {
                if (flags & ImGuiInputTextFlags_EscapeClearsAll)
                {
                    // Clear input
                    apply_new_text = "";
                    apply_new_text_length = 0;
                    STB_TEXTEDIT_CHARTYPE empty_string;
                    stb_textedit_replace(state, &state->Stb, &empty_string, 0);
                }
                else if (strcmp(buf, state->InitialTextA.Data) != 0)
                {
                    // Restore initial value. Only return true if restoring to the initial value changes the current buffer contents.
                    // Push records into the undo stack so we can CTRL+Z the revert operation itself
                    apply_new_text = state->InitialTextA.Data;
                    apply_new_text_length = state->InitialTextA.Size - 1;
                    value_changed = true;
                    ImVector<ImWchar> w_text;
                    if (apply_new_text_length > 0)
                    {
                        w_text.resize(ImTextCountCharsFromUtf8(apply_new_text, apply_new_text + apply_new_text_length) + 1);
                        ImTextStrFromUtf8(w_text.Data, w_text.Size, apply_new_text, apply_new_text + apply_new_text_length);
                    }
                    stb_textedit_replace(state, &state->Stb, w_text.Data, (apply_new_text_length > 0) ? (w_text.Size - 1) : 0);
                }
            }

            // Apply ASCII value
            if (!is_readonly)
            {
                state->TextAIsValid = true;
                state->TextA.resize(state->TextW.Size * 4 + 1);
                ImTextStrToUtf8(state->TextA.Data, state->TextA.Size, state->TextW.Data, NULL);
            }

            // When using 'ImGuiInputTextFlags_EnterReturnsTrue' as a special case we reapply the live buffer back to the input buffer before clearing ActiveId, even though strictly speaking it wasn't modified on this frame.
            // If we didn't do that, code like InputInt() with ImGuiInputTextFlags_EnterReturnsTrue would fail.
            // This also allows the user to use InputText() with ImGuiInputTextFlags_EnterReturnsTrue without maintaining any user-side storage (please note that if you use this property along ImGuiInputTextFlags_CallbackResize you can end up with your temporary string object unnecessarily allocating once a frame, either store your string data, either if you don't then don't use ImGuiInputTextFlags_CallbackResize).
            const bool apply_edit_back_to_user_buffer = !revert_edit || (validated && (flags & ImGuiInputTextFlags_EnterReturnsTrue) != 0);
            if (apply_edit_back_to_user_buffer)
            {
                // Apply new value immediately - copy modified buffer back
                // Note that as soon as the input box is active, the in-widget value gets priority over any underlying modification of the input buffer
                // FIXME: We actually always render 'buf' when calling DrawList->AddText, making the comment above incorrect.
                // FIXME-OPT: CPU waste to do this every time the widget is active, should mark dirty state from the stb_textedit callbacks.

                // User callback
                if ((flags & (ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackEdit | ImGuiInputTextFlags_CallbackAlways)) != 0)
                {
                    IM_ASSERT(callback != NULL);

                    // The reason we specify the usage semantic (Completion/History) is that Completion needs to disable keyboard TABBING at the moment.
                    ImGuiInputTextFlags event_flag = 0;
                    ImGuiKey event_key = ImGuiKey_None;
                    if ((flags & ImGuiInputTextFlags_CallbackCompletion) != 0 && ImGui::Shortcut(ImGuiKey_Tab, id))
                    {
                        event_flag = ImGuiInputTextFlags_CallbackCompletion;
                        event_key = ImGuiKey_Tab;
                    }
                    else if ((flags & ImGuiInputTextFlags_CallbackHistory) != 0 && ImGui::IsKeyPressed(ImGuiKey_UpArrow))
                    {
                        event_flag = ImGuiInputTextFlags_CallbackHistory;
                        event_key = ImGuiKey_UpArrow;
                    }
                    else if ((flags & ImGuiInputTextFlags_CallbackHistory) != 0 && ImGui::IsKeyPressed(ImGuiKey_DownArrow))
                    {
                        event_flag = ImGuiInputTextFlags_CallbackHistory;
                        event_key = ImGuiKey_DownArrow;
                    }
                    else if ((flags & ImGuiInputTextFlags_CallbackEdit) && state->Edited)
                    {
                        event_flag = ImGuiInputTextFlags_CallbackEdit;
                    }
                    else if (flags & ImGuiInputTextFlags_CallbackAlways)
                    {
                        event_flag = ImGuiInputTextFlags_CallbackAlways;
                    }

                    if (event_flag)
                    {
                        ImGuiInputTextCallbackData callback_data;
                        callback_data.Ctx = &g;
                        callback_data.EventFlag = event_flag;
                        callback_data.Flags = flags;
                        callback_data.UserData = callback_user_data;

                        char* callback_buf = is_readonly ? buf : state->TextA.Data;
                        callback_data.EventKey = event_key;
                        callback_data.Buf = callback_buf;
                        callback_data.BufTextLen = state->CurLenA;
                        callback_data.BufSize = state->BufCapacityA;
                        callback_data.BufDirty = false;

                        // We have to convert from wchar-positions to UTF-8-positions, which can be pretty slow (an incentive to ditch the ImWchar buffer, see https://github.com/nothings/stb/issues/188)
                        ImWchar* text = state->TextW.Data;
                        const int utf8_cursor_pos = callback_data.CursorPos = ImTextCountUtf8BytesFromStr(text, text + state->Stb.cursor);
                        const int utf8_selection_start = callback_data.SelectionStart = ImTextCountUtf8BytesFromStr(text, text + state->Stb.select_start);
                        const int utf8_selection_end = callback_data.SelectionEnd = ImTextCountUtf8BytesFromStr(text, text + state->Stb.select_end);

                        // Call user code
                        callback(&callback_data);

                        // Read back what user may have modified
                        callback_buf = is_readonly ? buf : state->TextA.Data; // Pointer may have been invalidated by a resize callback
                        IM_ASSERT(callback_data.Buf == callback_buf);         // Invalid to modify those fields
                        IM_ASSERT(callback_data.BufSize == state->BufCapacityA);
                        IM_ASSERT(callback_data.Flags == flags);
                        const bool buf_dirty = callback_data.BufDirty;
                        if (callback_data.CursorPos != utf8_cursor_pos || buf_dirty)            { state->Stb.cursor = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.CursorPos); state->CursorFollow = true; }
                        if (callback_data.SelectionStart != utf8_selection_start || buf_dirty)  { state->Stb.select_start = (callback_data.SelectionStart == callback_data.CursorPos) ? state->Stb.cursor : ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionStart); }
                        if (callback_data.SelectionEnd != utf8_selection_end || buf_dirty)      { state->Stb.select_end = (callback_data.SelectionEnd == callback_data.SelectionStart) ? state->Stb.select_start : ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionEnd); }
                        if (buf_dirty)
                        {
                            IM_ASSERT((flags & ImGuiInputTextFlags_ReadOnly) == 0);
                            IM_ASSERT(callback_data.BufTextLen == (int)strlen(callback_data.Buf)); // You need to maintain BufTextLen if you change the text!
                            InputTextReconcileUndoStateAfterUserCallback(state, callback_data.Buf, callback_data.BufTextLen); // FIXME: Move the rest of this block inside function and rename to InputTextReconcileStateAfterUserCallback() ?
                            if (callback_data.BufTextLen > backup_current_text_length && is_resizable)
                                state->TextW.resize(state->TextW.Size + (callback_data.BufTextLen - backup_current_text_length)); // Worse case scenario resize
                            state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, state->TextW.Size, callback_data.Buf, NULL);
                            state->CurLenA = callback_data.BufTextLen;  // Assume correct length and valid UTF-8 from user, saves us an extra strlen()
                            state->CursorAnimReset();
                        }
                    }
                }

                // Will copy result string if modified
                if (!is_readonly && strcmp(state->TextA.Data, buf) != 0)
                {
                    apply_new_text = state->TextA.Data;
                    apply_new_text_length = state->CurLenA;
                    value_changed = true;
                }
            }
        }

        // Handle reapplying final data on deactivation (see InputTextDeactivateHook() for details)
        if (g.InputTextDeactivatedState.ID == id)
        {
            if (g.ActiveId != id && IsItemDeactivatedAfterEdit() && !is_readonly)
            {
                apply_new_text = g.InputTextDeactivatedState.TextA.Data;
                apply_new_text_length = g.InputTextDeactivatedState.TextA.Size - 1;
                value_changed |= (strcmp(g.InputTextDeactivatedState.TextA.Data, buf) != 0);
                //IMGUI_DEBUG_LOG("InputText(): apply Deactivated data for 0x%08X: \"%.*s\".\n", id, apply_new_text_length, apply_new_text);
            }
            g.InputTextDeactivatedState.ID = 0;
        }

        // Copy result to user buffer. This can currently only happen when (g.ActiveId == id)
        if (apply_new_text != NULL)
        {
            // We cannot test for 'backup_current_text_length != apply_new_text_length' here because we have no guarantee that the size
            // of our owned buffer matches the size of the string object held by the user, and by design we allow InputText() to be used
            // without any storage on user's side.
            IM_ASSERT(apply_new_text_length >= 0);
            if (is_resizable)
            {
                ImGuiInputTextCallbackData callback_data;
                callback_data.Ctx = &g;
                callback_data.EventFlag = ImGuiInputTextFlags_CallbackResize;
                callback_data.Flags = flags;
                callback_data.Buf = buf;
                callback_data.BufTextLen = apply_new_text_length;
                callback_data.BufSize = ImMax(buf_size, apply_new_text_length + 1);
                callback_data.UserData = callback_user_data;
                callback(&callback_data);
                buf = callback_data.Buf;
                buf_size = callback_data.BufSize;
                apply_new_text_length = ImMin(callback_data.BufTextLen, buf_size - 1);
                IM_ASSERT(apply_new_text_length <= buf_size);
            }
            //IMGUI_DEBUG_PRINT("InputText(\"%s\"): apply_new_text length %d\n", label, apply_new_text_length);

            // If the underlying buffer resize was denied or not carried to the next frame, apply_new_text_length+1 may be >= buf_size.
            ImStrncpy(buf, apply_new_text, ImMin(apply_new_text_length + 1, buf_size));
        }

        // Release active ID at the end of the function (so e.g. pressing Return still does a final application of the value)
        // Otherwise request text input ahead for next frame.
        if (g.ActiveId == id && clear_active_id)
            ClearActiveID();
        else if (g.ActiveId == id)
            g.WantTextInputNextFrame = 1;

        // Render frame
        if (!is_multiline)
        {
            RenderNavHighlight(frame_bb, id);
            ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
        }

        const ImVec4 clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + inner_size.x, frame_bb.Min.y + inner_size.y); // Not using frame_bb.Max because we have adjusted size
        ImVec2 draw_pos = is_multiline ? draw_window->DC.CursorPos : frame_bb.Min + style.FramePadding;
        ImVec2 text_size(0.0f, 0.0f);

        // Set upper limit of single-line InputTextEx() at 2 million characters strings. The current pathological worst case is a long line
        // without any carriage return, which would makes ImFont::RenderText() reserve too many vertices and probably crash. Avoid it altogether.
        // Note that we only use this limit on single-line InputText(), so a pathologically large line on a InputTextMultiline() would still crash.
        const int buf_display_max_length = 2 * 1024 * 1024;
        const char* buf_display = buf_display_from_state ? state->TextA.Data : buf; //-V595
        const char* buf_display_end = NULL; // We have specialized paths below for setting the length
        if (is_displaying_hint)
        {
            buf_display = hint;
            buf_display_end = hint + strlen(hint);
        }

        // Render text. We currently only render selection when the widget is active or while scrolling.
        // FIXME: We could remove the '&& render_cursor' to keep rendering selection when inactive.
        if (render_cursor || render_selection)
        {
            IM_ASSERT(state != NULL);
            if (!is_displaying_hint)
                buf_display_end = buf_display + state->CurLenA;

            // Render text (with cursor and selection)
            // This is going to be messy. We need to:
            // - Display the text (this alone can be more easily clipped)
            // - Handle scrolling, highlight selection, display cursor (those all requires some form of 1d->2d cursor position calculation)
            // - Measure text height (for scrollbar)
            // We are attempting to do most of that in **one main pass** to minimize the computation cost (non-negligible for large amount of text) + 2nd pass for selection rendering (we could merge them by an extra refactoring effort)
            // FIXME: This should occur on buf_display but we'd need to maintain cursor/select_start/select_end for UTF-8.
            const ImWchar* text_begin = state->TextW.Data;
            ImVec2 cursor_offset, select_start_offset;

            {
                // Find lines numbers straddling 'cursor' (slot 0) and 'select_start' (slot 1) positions.
                const ImWchar* searches_input_ptr[2] = { NULL, NULL };
                int searches_result_line_no[2] = { -1000, -1000 };
                int searches_remaining = 0;
                if (render_cursor)
                {
                    searches_input_ptr[0] = text_begin + state->Stb.cursor;
                    searches_result_line_no[0] = -1;
                    searches_remaining++;
                }
                if (render_selection)
                {
                    searches_input_ptr[1] = text_begin + ImMin(state->Stb.select_start, state->Stb.select_end);
                    searches_result_line_no[1] = -1;
                    searches_remaining++;
                }

                // Iterate all lines to find our line numbers
                // In multi-line mode, we never exit the loop until all lines are counted, so add one extra to the searches_remaining counter.
                searches_remaining += is_multiline ? 1 : 0;
                int line_count = 0;
                //for (const ImWchar* s = text_begin; (s = (const ImWchar*)wcschr((const wchar_t*)s, (wchar_t)'\n')) != NULL; s++)  // FIXME-OPT: Could use this when wchar_t are 16-bit
                for (const ImWchar* s = text_begin; *s != 0; s++)
                    if (*s == '\n')
                    {
                        line_count++;
                        if (searches_result_line_no[0] == -1 && s >= searches_input_ptr[0]) { searches_result_line_no[0] = line_count; if (--searches_remaining <= 0) break; }
                        if (searches_result_line_no[1] == -1 && s >= searches_input_ptr[1]) { searches_result_line_no[1] = line_count; if (--searches_remaining <= 0) break; }
                    }
                line_count++;
                if (searches_result_line_no[0] == -1)
                    searches_result_line_no[0] = line_count;
                if (searches_result_line_no[1] == -1)
                    searches_result_line_no[1] = line_count;
                
                // Calculate 2d position by finding the beginning of the line and measuring distance
                cursor_offset.x = InputTextCalcTextSizeW(&g, ImStrbolW(searches_input_ptr[0], text_begin), searches_input_ptr[0]).x;
                cursor_offset.y = searches_result_line_no[0] * g.FontSize;
                if (searches_result_line_no[1] >= 0)
                {
                    select_start_offset.x = InputTextCalcTextSizeW(&g, ImStrbolW(searches_input_ptr[1], text_begin), searches_input_ptr[1]).x;
                    select_start_offset.y = searches_result_line_no[1] * g.FontSize;
                }

                // Store text height (note that we haven't calculated text width at all, see GitHub issues #383, #1224)
                if (is_multiline)
                    text_size = ImVec2(inner_size.x, line_count * g.FontSize);
            }

            // Scroll
            if (render_cursor && state->CursorFollow)
            {
                // Horizontal scroll in chunks of quarter width
                if (!(flags & ImGuiInputTextFlags_NoHorizontalScroll))
                {
                    const float scroll_increment_x = inner_size.x * 0.25f;
                    const float visible_width = inner_size.x - style.FramePadding.x;
                    if (cursor_offset.x < state->ScrollX)
                        state->ScrollX = IM_FLOOR(ImMax(0.0f, cursor_offset.x - scroll_increment_x));
                    else if (cursor_offset.x - visible_width >= state->ScrollX)
                        state->ScrollX = IM_FLOOR(cursor_offset.x - visible_width + scroll_increment_x);
                }
                else
                {
                    state->ScrollX = 0.0f;
                }

                // Vertical scroll
                if (is_multiline)
                {
                    // Test if cursor is vertically visible
                    if (cursor_offset.y - g.FontSize < scroll_y)
                        scroll_y = ImMax(0.0f, cursor_offset.y - g.FontSize);
                    else if (cursor_offset.y - (inner_size.y - style.FramePadding.y * 2.0f) >= scroll_y)
                        scroll_y = cursor_offset.y - inner_size.y + style.FramePadding.y * 2.0f;
                    const float scroll_max_y = ImMax((text_size.y + style.FramePadding.y * 2.0f) - inner_size.y, 0.0f);
                    scroll_y = ImClamp(scroll_y, 0.0f, scroll_max_y);
                    draw_pos.y += (draw_window->Scroll.y - scroll_y);   // Manipulate cursor pos immediately avoid a frame of lag
                    draw_window->Scroll.y = scroll_y;
                }

                state->CursorFollow = false;
            }

            // Draw selection
            const ImVec2 draw_scroll = ImVec2(state->ScrollX, 0.0f);
            if (render_selection)
            {
                const ImWchar* text_selected_begin = text_begin + ImMin(state->Stb.select_start, state->Stb.select_end);
                const ImWchar* text_selected_end = text_begin + ImMax(state->Stb.select_start, state->Stb.select_end);

                ImU32 bg_color = GetColorU32(ImGuiCol_TextSelectedBg, render_cursor ? 1.0f : 0.6f); // FIXME: current code flow mandate that render_cursor is always true here, we are leaving the transparent one for tests.
                float bg_offy_up = is_multiline ? 0.0f : -1.0f;    // FIXME: those offsets should be part of the style? they don't play so well with multi-line selection.
                float bg_offy_dn = is_multiline ? 0.0f : 2.0f;
                ImVec2 rect_pos = draw_pos + select_start_offset - draw_scroll;
                for (const ImWchar* p = text_selected_begin; p < text_selected_end; )
                {
                    if (rect_pos.y > clip_rect.w + g.FontSize)
                        break;
                    if (rect_pos.y < clip_rect.y)
                    {
                        //p = (const ImWchar*)wmemchr((const wchar_t*)p, '\n', text_selected_end - p);  // FIXME-OPT: Could use this when wchar_t are 16-bit
                        //p = p ? p + 1 : text_selected_end;
                        while (p < text_selected_end)
                            if (*p++ == '\n')
                                break;
                    }
                    else
                    {
                        ImVec2 rect_size = InputTextCalcTextSizeW(&g, p, text_selected_end, &p, NULL, true);
                        if (rect_size.x <= 0.0f) rect_size.x = IM_FLOOR(g.Font->GetCharAdvance((ImWchar)' ') * 0.50f); // So we can see selected empty lines
                        ImRect rect(rect_pos + ImVec2(0.0f, bg_offy_up - g.FontSize), rect_pos + ImVec2(rect_size.x, bg_offy_dn));
                        rect.ClipWith(clip_rect);
                        if (rect.Overlaps(clip_rect))
                            draw_window->DrawList->AddRectFilled(rect.Min, rect.Max, bg_color);
                    }
                    rect_pos.x = draw_pos.x - draw_scroll.x;
                    rect_pos.y += g.FontSize;
                }
            }

            // We test for 'buf_display_max_length' as a way to avoid some pathological cases (e.g. single-line 1 MB string) which would make ImDrawList crash.
            if (is_multiline || (buf_display_end - buf_display) < buf_display_max_length)
            {
                ImU32 col = GetColorU32(is_displaying_hint ? ImGuiCol_TextDisabled : ImGuiCol_Text);
                draw_window->DrawList->AddText(g.Font, g.FontSize, draw_pos - draw_scroll, col, buf_display, buf_display_end, 0.0f, is_multiline ? NULL : &clip_rect);
            }

            // Draw blinking cursor
            if (render_cursor)
            {
                state->CursorAnim += io.DeltaTime;
                bool cursor_is_visible = (!g.IO.ConfigInputTextCursorBlink) || (state->CursorAnim <= 0.0f) || ImFmod(state->CursorAnim, 1.20f) <= 0.80f;
                ImVec2 cursor_screen_pos = ImFloor(draw_pos + cursor_offset - draw_scroll);
                ImRect cursor_screen_rect(cursor_screen_pos.x, cursor_screen_pos.y - g.FontSize + 0.5f, cursor_screen_pos.x + 1.0f, cursor_screen_pos.y - 1.5f);
                if (cursor_is_visible && cursor_screen_rect.Overlaps(clip_rect))
                    draw_window->DrawList->AddLine(cursor_screen_rect.Min, cursor_screen_rect.GetBL(), GetColorU32(ImGuiCol_Text));

                // Notify OS of text input position for advanced IME (-1 x offset so that Windows IME can cover our cursor. Bit of an extra nicety.)
                if (!is_readonly)
                {
                    g.PlatformImeData.WantVisible = true;
                    g.PlatformImeData.InputPos = ImVec2(cursor_screen_pos.x - 1.0f, cursor_screen_pos.y - g.FontSize);
                    g.PlatformImeData.InputLineHeight = g.FontSize;
                    g.PlatformImeViewport = window->Viewport->ID;
                }
            }
        }
        else
        {
            // Render text only (no selection, no cursor)
            if (is_multiline)
                text_size = ImVec2(inner_size.x, ImGui::InputTextCalcTextLenAndLineCount(buf_display, &buf_display_end) * g.FontSize); // We don't need width
            else if (!is_displaying_hint && g.ActiveId == id)
                buf_display_end = buf_display + state->CurLenA;
            else if (!is_displaying_hint)
                buf_display_end = buf_display + strlen(buf_display);

            if (is_multiline || (buf_display_end - buf_display) < buf_display_max_length)
            {
                ImU32 col = ImGui::GetColorU32(is_displaying_hint ? ImGuiCol_TextDisabled : ImGuiCol_Text);
                draw_window->DrawList->AddText(g.Font, g.FontSize, draw_pos, col, buf_display, buf_display_end, 0.0f, is_multiline ? NULL : &clip_rect);
            }
        }

        if (is_password && !is_displaying_hint)
            PopFont();

        if (is_multiline)
        {
            // For focus requests to work on our multiline we need to ensure our child ItemAdd() call specifies the ImGuiItemFlags_Inputable (ref issue #4761)...
            Dummy(ImVec2(text_size.x, text_size.y + style.FramePadding.y));
            ImGuiItemFlags backup_item_flags = g.CurrentItemFlags;
            g.CurrentItemFlags |= ImGuiItemFlags_Inputable | ImGuiItemFlags_NoTabStop;
            EndChild();
            item_data_backup.StatusFlags |= (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_HoveredWindow);
            g.CurrentItemFlags = backup_item_flags;

            // ...and then we need to undo the group overriding last item data, which gets a bit messy as EndGroup() tries to forward scrollbar being active...
            // FIXME: This quite messy/tricky, should attempt to get rid of the child window.
            ImGui::EndGroup();
            if (g.LastItemData.ID == 0)
            {
                g.LastItemData.ID = id;
                g.LastItemData.InFlags = item_data_backup.InFlags;
                g.LastItemData.StatusFlags = item_data_backup.StatusFlags;
            }
        }

        // Log as text
        if (g.LogEnabled && (!is_password || is_displaying_hint))
        {
            ImGui::LogSetNextTextDecoration("{", "}");
            ImGui::LogRenderedText(&draw_pos, buf_display, buf_display_end);
        }

        if (label_size.x > 0)
            RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

        if (value_changed && !(flags & ImGuiInputTextFlags_NoMarkEdited))
            MarkItemEdited(id);

        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Inputable);
        if ((flags & ImGuiInputTextFlags_EnterReturnsTrue) != 0)
            return validated;
        else
            return value_changed;
	}


    COREGUI_API bool TreeNode(const String& label, TreeNodeFlags flags)
    {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		GUI:ID id = window->GetID(label.GetCString());

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const bool display_frame = (flags & ImGuiTreeNodeFlags_Framed) != 0;
		const ImVec2 padding = (display_frame || (flags & ImGuiTreeNodeFlags_FramePadding)) ? style.FramePadding : ImVec2(style.FramePadding.x, ImMin(window->DC.CurrLineTextBaseOffset, style.FramePadding.y));

		auto label_end = ImGui::FindRenderedTextEnd(label.GetCString());
		const ImVec2 label_size = ImGui::CalcTextSize(label.GetCString(), label_end, false);

		// We vertically grow up to current line height up the typical widget height.
		const float frame_height = ImMax(ImMin(window->DC.CurrLineSize.y, g.FontSize + style.FramePadding.y * 2), label_size.y + padding.y * 2);
		ImRect frame_bb;
		frame_bb.Min.x = (flags & ImGuiTreeNodeFlags_SpanFullWidth) ? window->WorkRect.Min.x : window->DC.CursorPos.x;
		frame_bb.Min.y = window->DC.CursorPos.y;
		frame_bb.Max.x = window->WorkRect.Max.x;
		frame_bb.Max.y = window->DC.CursorPos.y + frame_height;
		if (display_frame)
		{
			// Framed header expand a little outside the default padding, to the edge of InnerClipRect
			// (FIXME: May remove this at some point and make InnerClipRect align with WindowPadding.x instead of WindowPadding.x*0.5f)
			frame_bb.Min.x -= IM_FLOOR(window->WindowPadding.x * 0.5f - 1.0f);
			frame_bb.Max.x += IM_FLOOR(window->WindowPadding.x * 0.5f);
		}

		const float text_offset_x = g.FontSize + (display_frame ? padding.x * 3 : padding.x * 2);           // Collapser arrow width + Spacing
		const float text_offset_y = ImMax(padding.y, window->DC.CurrLineTextBaseOffset);                    // Latch before ItemSize changes it
		const float text_width = g.FontSize + (label_size.x > 0.0f ? label_size.x + padding.x * 2 : 0.0f);  // Include collapser
		ImVec2 text_pos(window->DC.CursorPos.x + text_offset_x, window->DC.CursorPos.y + text_offset_y);
		ImGui::ItemSize(ImVec2(text_width, frame_height), padding.y);

		// For regular tree nodes, we arbitrary allow to click past 2 worth of ItemSpacing
		ImRect interact_bb = frame_bb;
		if (!display_frame && (flags & (ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth)) == 0)
			interact_bb.Max.x = frame_bb.Min.x + text_width + style.ItemSpacing.x * 2.0f;

		// Store a flag for the current depth to tell if we will allow closing this node when navigating one of its child.
		// For this purpose we essentially compare if g.NavIdIsAlive went from 0 to 1 between TreeNode() and TreePop().
		// This is currently only support 32 level deep and we are fine with (1 << Depth) overflowing into a zero.
		const bool is_leaf = (flags & ImGuiTreeNodeFlags_Leaf) != 0;
		bool is_open = ImGui::TreeNodeUpdateNextOpen(id, flags);
		if (is_open && !g.NavIdIsAlive && (flags & ImGuiTreeNodeFlags_NavLeftJumpsBackHere) && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
			window->DC.TreeJumpToParentOnPopMask |= (1 << window->DC.TreeDepth);

		bool item_add = ImGui::ItemAdd(interact_bb, id);
		g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HasDisplayRect;
		g.LastItemData.DisplayRect = frame_bb;

		if (!item_add)
		{
			if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
				ImGui::TreePushOverrideID(id);
			IMGUI_TEST_ENGINE_ITEM_INFO(g.LastItemData.ID, label, g.LastItemData.StatusFlags | (is_leaf ? 0 : ImGuiItemStatusFlags_Openable) | (is_open ? ImGuiItemStatusFlags_Opened : 0));
			return is_open;
		}

		ImGuiButtonFlags button_flags = ImGuiTreeNodeFlags_None;
		if (flags & ImGuiTreeNodeFlags_AllowItemOverlap)
			button_flags |= ImGuiButtonFlags_AllowItemOverlap;
		if (!is_leaf)
			button_flags |= ImGuiButtonFlags_PressedOnDragDropHold;

		// We allow clicking on the arrow section with keyboard modifiers held, in order to easily
		// allow browsing a tree while preserving selection with code implementing multi-selection patterns.
		// When clicking on the rest of the tree node we always disallow keyboard modifiers.
		const float arrow_hit_x1 = (text_pos.x - text_offset_x) - style.TouchExtraPadding.x;
		const float arrow_hit_x2 = (text_pos.x - text_offset_x) + (g.FontSize + padding.x * 2.0f) + style.TouchExtraPadding.x;
		const bool is_mouse_x_over_arrow = (g.IO.MousePos.x >= arrow_hit_x1 && g.IO.MousePos.x < arrow_hit_x2);
		if (window != g.HoveredWindow || !is_mouse_x_over_arrow)
			button_flags |= ImGuiButtonFlags_NoKeyModifiers;

		// Open behaviors can be altered with the _OpenOnArrow and _OnOnDoubleClick flags.
		// Some alteration have subtle effects (e.g. toggle on MouseUp vs MouseDown events) due to requirements for multi-selection and drag and drop support.
		// - Single-click on label = Toggle on MouseUp (default, when _OpenOnArrow=0)
		// - Single-click on arrow = Toggle on MouseDown (when _OpenOnArrow=0)
		// - Single-click on arrow = Toggle on MouseDown (when _OpenOnArrow=1)
		// - Double-click on label = Toggle on MouseDoubleClick (when _OpenOnDoubleClick=1)
		// - Double-click on arrow = Toggle on MouseDoubleClick (when _OpenOnDoubleClick=1 and _OpenOnArrow=0)
		// It is rather standard that arrow click react on Down rather than Up.
		// We set ImGuiButtonFlags_PressedOnClickRelease on OpenOnDoubleClick because we want the item to be active on the initial MouseDown in order for drag and drop to work.
		if (is_mouse_x_over_arrow)
			button_flags |= ImGuiButtonFlags_PressedOnClick;
		else if (flags & ImGuiTreeNodeFlags_OpenOnDoubleClick)
			button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick;
		else
			button_flags |= ImGuiButtonFlags_PressedOnClickRelease;

		bool selected = (flags & ImGuiTreeNodeFlags_Selected) != 0;
		const bool was_selected = selected;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(interact_bb, id, &hovered, &held, button_flags);
		bool toggled = false;
		if (!is_leaf)
		{
			if (pressed && g.DragDropHoldJustPressedId != id)
			{
				if ((flags & (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick)) == 0 || (g.NavActivateId == id))
					toggled = true;
				if (flags & ImGuiTreeNodeFlags_OpenOnArrow)
					toggled |= is_mouse_x_over_arrow && !g.NavDisableMouseHover; // Lightweight equivalent of IsMouseHoveringRect() since ButtonBehavior() already did the job
				if ((flags & ImGuiTreeNodeFlags_OpenOnDoubleClick) && g.IO.MouseClickedCount[0] == 2)
					toggled = true;
			}
			else if (pressed && g.DragDropHoldJustPressedId == id)
			{
				IM_ASSERT(button_flags & ImGuiButtonFlags_PressedOnDragDropHold);
				if (!is_open) // When using Drag and Drop "hold to open" we keep the node highlighted after opening, but never close it again.
					toggled = true;
			}

			if (g.NavId == id && g.NavMoveDir == ImGuiDir_Left && is_open)
			{
				toggled = true;
				ImGui::NavMoveRequestCancel();
			}
			if (g.NavId == id && g.NavMoveDir == ImGuiDir_Right && !is_open) // If there's something upcoming on the line we may want to give it the priority?
			{
				toggled = true;
				ImGui::NavMoveRequestCancel();
			}

			if (toggled)
			{
				is_open = !is_open;
				window->DC.StateStorage->SetInt(id, is_open);
				g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledOpen;
			}
		}
		if (flags & ImGuiTreeNodeFlags_AllowItemOverlap)
			ImGui::SetItemAllowOverlap();

		// In this branch, TreeNodeBehavior() cannot toggle the selection so this will never trigger.
		if (selected != was_selected) //-V547
			g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

		// Render
		const ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
		ImGuiNavHighlightFlags nav_highlight_flags = ImGuiNavHighlightFlags_TypeThin;
		if (display_frame)
		{
			// Framed type
			const ImU32 bg_col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
			ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, bg_col, true, style.FrameRounding);
			ImGui::RenderNavHighlight(frame_bb, id, nav_highlight_flags);
			if (flags & ImGuiTreeNodeFlags_Bullet)
				ImGui::RenderBullet(window->DrawList, ImVec2(text_pos.x - text_offset_x * 0.60f, text_pos.y + g.FontSize * 0.5f), text_col);
			else if (!is_leaf)
				ImGui::RenderArrow(window->DrawList, ImVec2(text_pos.x - text_offset_x + padding.x, text_pos.y), text_col, is_open ? ImGuiDir_Down : ImGuiDir_Right, 1.0f);
			else // Leaf without bullet, left-adjusted text
				text_pos.x -= text_offset_x;
			if (flags & ImGuiTreeNodeFlags_ClipLabelForTrailingButton)
				frame_bb.Max.x -= g.FontSize + style.FramePadding.x;

			if (g.LogEnabled)
				ImGui::LogSetNextTextDecoration("###", "###");
			ImGui::RenderTextClipped(text_pos, frame_bb.Max, label.GetCString(), label_end, &label_size);
		}
		else
		{
			// Unframed typed for tree nodes
			if (hovered || selected)
			{
				const ImU32 bg_col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
				ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, bg_col, false);
			}
			ImGui::RenderNavHighlight(frame_bb, id, nav_highlight_flags);
			if (flags & ImGuiTreeNodeFlags_Bullet)
				ImGui::RenderBullet(window->DrawList, ImVec2(text_pos.x - text_offset_x * 0.5f, text_pos.y + g.FontSize * 0.5f), text_col);
			else if (!is_leaf)
				ImGui::RenderArrow(window->DrawList, ImVec2(text_pos.x - text_offset_x + padding.x, text_pos.y + g.FontSize * 0.15f), text_col, is_open ? ImGuiDir_Down : ImGuiDir_Right, 0.70f);
			if (g.LogEnabled)
				ImGui::LogSetNextTextDecoration(">", NULL);
			ImGui::RenderText(text_pos, label.GetCString(), label_end, false);
		}

		if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
			ImGui::TreePushOverrideID(id);
		IMGUI_TEST_ENGINE_ITEM_INFO(id, label.GetCString(), 
			g.LastItemData.StatusFlags | (is_leaf ? 0 : ImGuiItemStatusFlags_Openable) | (is_open ? ImGuiItemStatusFlags_Opened : 0));
		return is_open;
    }

    COREGUI_API void TreePop()
    {
		ImGui::TreePop();
    }

    COREGUI_API bool CollapsibleHeader(const String& label, Vec4* customPadding, Vec4* customBorderRadius, f32* customBorderSize, TreeNodeFlags flags)
    {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		auto id = window->GetID(label.GetCString());
		flags |= TNF_CollapsingHeader;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const bool display_frame = (flags & ImGuiTreeNodeFlags_Framed) != 0;
		const ImVec2 defaultPadding = (display_frame || (flags & ImGuiTreeNodeFlags_FramePadding)) ? style.FramePadding : ImVec2(style.FramePadding.x, ImMin(window->DC.CurrLineTextBaseOffset, style.FramePadding.y));
		
		ImVec4 padding = ImVec4(defaultPadding.x, defaultPadding.y, defaultPadding.x, defaultPadding.y);
		if (customPadding != nullptr)
		{
			padding = ImVec4(customPadding->x, customPadding->y, customPadding->z, customPadding->w);
		}

		auto label_end = ImGui::FindRenderedTextEnd(label.GetCString());
		const ImVec2 label_size = ImGui::CalcTextSize(label.GetCString(), label_end, false);

		// We vertically grow up to current line height up the typical widget height.
		const float frame_height = ImMax(ImMin(window->DC.CurrLineSize.y, g.FontSize + style.FramePadding.y * 2), label_size.y + padding.y + padding.w);
		ImRect frame_bb;
		frame_bb.Min.x = (flags & ImGuiTreeNodeFlags_SpanFullWidth) ? window->WorkRect.Min.x : window->DC.CursorPos.x;
		frame_bb.Min.y = window->DC.CursorPos.y;
		frame_bb.Max.x = window->WorkRect.Max.x;
		frame_bb.Max.y = window->DC.CursorPos.y + frame_height;
		if (display_frame)
		{
			// Framed header expand a little outside the default padding, to the edge of InnerClipRect
			// (FIXME: May remove this at some point and make InnerClipRect align with WindowPadding.x instead of WindowPadding.x*0.5f)
			frame_bb.Min.x -= IM_FLOOR(window->WindowPadding.x * 0.5f - 1.0f);
			frame_bb.Max.x += IM_FLOOR(window->WindowPadding.x * 0.5f);
		}

		// Collapser arrow width + Spacing
		const float text_offset_x = g.FontSize + (display_frame ? (padding.x + padding.z + (padding.x + padding.z) / 2.f) : (padding.x + padding.z));
		const float text_offset_y = ImMax(padding.y, window->DC.CurrLineTextBaseOffset);                    // Latch before ItemSize changes it
		const float text_width = g.FontSize + (label_size.x > 0.0f ? label_size.x + padding.x * 2 : 0.0f);  // Include collapser
		ImVec2 text_pos(window->DC.CursorPos.x + text_offset_x, window->DC.CursorPos.y + text_offset_y);
		ImGui::ItemSize(ImVec2(text_width, frame_height), padding.y);

		// For regular tree nodes, we arbitrary allow to click past 2 worth of ItemSpacing
		ImRect interact_bb = frame_bb;
		if (!display_frame && (flags & (ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth)) == 0)
			interact_bb.Max.x = frame_bb.Min.x + text_width + style.ItemSpacing.x * 2.0f;

		// Store a flag for the current depth to tell if we will allow closing this node when navigating one of its child.
		// For this purpose we essentially compare if g.NavIdIsAlive went from 0 to 1 between TreeNode() and TreePop().
		// This is currently only support 32 level deep and we are fine with (1 << Depth) overflowing into a zero.
		const bool is_leaf = (flags & ImGuiTreeNodeFlags_Leaf) != 0;
		bool is_open = ImGui::TreeNodeUpdateNextOpen(id, flags);
		if (is_open && !g.NavIdIsAlive && (flags & ImGuiTreeNodeFlags_NavLeftJumpsBackHere) && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
			window->DC.TreeJumpToParentOnPopMask |= (1 << window->DC.TreeDepth);

		bool item_add = ImGui::ItemAdd(interact_bb, id);
		g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HasDisplayRect;
		g.LastItemData.DisplayRect = frame_bb;

		if (!item_add)
		{
			if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
				ImGui::TreePushOverrideID(id);
			IMGUI_TEST_ENGINE_ITEM_INFO(g.LastItemData.ID, label, g.LastItemData.StatusFlags | (is_leaf ? 0 : ImGuiItemStatusFlags_Openable) | (is_open ? ImGuiItemStatusFlags_Opened : 0));
			return is_open;
		}

		ImGuiButtonFlags button_flags = ImGuiTreeNodeFlags_None;
		if (flags & ImGuiTreeNodeFlags_AllowItemOverlap)
			button_flags |= ImGuiButtonFlags_AllowItemOverlap;
		if (!is_leaf)
			button_flags |= ImGuiButtonFlags_PressedOnDragDropHold;

		// We allow clicking on the arrow section with keyboard modifiers held, in order to easily
		// allow browsing a tree while preserving selection with code implementing multi-selection patterns.
		// When clicking on the rest of the tree node we always disallow keyboard modifiers.
		const float arrow_hit_x1 = (text_pos.x - text_offset_x) - style.TouchExtraPadding.x;
		const float arrow_hit_x2 = (text_pos.x - text_offset_x) + (g.FontSize + padding.x + padding.z) + style.TouchExtraPadding.x;
		const bool is_mouse_x_over_arrow = (g.IO.MousePos.x >= arrow_hit_x1 && g.IO.MousePos.x < arrow_hit_x2);
		if (window != g.HoveredWindow || !is_mouse_x_over_arrow)
			button_flags |= ImGuiButtonFlags_NoKeyModifiers;

		// Open behaviors can be altered with the _OpenOnArrow and _OnOnDoubleClick flags.
		// Some alteration have subtle effects (e.g. toggle on MouseUp vs MouseDown events) due to requirements for multi-selection and drag and drop support.
		// - Single-click on label = Toggle on MouseUp (default, when _OpenOnArrow=0)
		// - Single-click on arrow = Toggle on MouseDown (when _OpenOnArrow=0)
		// - Single-click on arrow = Toggle on MouseDown (when _OpenOnArrow=1)
		// - Double-click on label = Toggle on MouseDoubleClick (when _OpenOnDoubleClick=1)
		// - Double-click on arrow = Toggle on MouseDoubleClick (when _OpenOnDoubleClick=1 and _OpenOnArrow=0)
		// It is rather standard that arrow click react on Down rather than Up.
		// We set ImGuiButtonFlags_PressedOnClickRelease on OpenOnDoubleClick because we want the item to be active on the initial MouseDown in order for drag and drop to work.
		if (is_mouse_x_over_arrow)
			button_flags |= ImGuiButtonFlags_PressedOnClick;
		else if (flags & ImGuiTreeNodeFlags_OpenOnDoubleClick)
			button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick;
		else
			button_flags |= ImGuiButtonFlags_PressedOnClickRelease;

		bool selected = (flags & ImGuiTreeNodeFlags_Selected) != 0;
		const bool was_selected = selected;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(interact_bb, id, &hovered, &held, button_flags);
		bool toggled = false;
		if (!is_leaf)
		{
			if (pressed && g.DragDropHoldJustPressedId != id)
			{
				if ((flags & (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick)) == 0 || (g.NavActivateId == id))
					toggled = true;
				if (flags & ImGuiTreeNodeFlags_OpenOnArrow)
					toggled |= is_mouse_x_over_arrow && !g.NavDisableMouseHover; // Lightweight equivalent of IsMouseHoveringRect() since ButtonBehavior() already did the job
				if ((flags & ImGuiTreeNodeFlags_OpenOnDoubleClick) && g.IO.MouseClickedCount[0] == 2)
					toggled = true;
			}
			else if (pressed && g.DragDropHoldJustPressedId == id)
			{
				IM_ASSERT(button_flags & ImGuiButtonFlags_PressedOnDragDropHold);
				if (!is_open) // When using Drag and Drop "hold to open" we keep the node highlighted after opening, but never close it again.
					toggled = true;
			}

			if (g.NavId == id && g.NavMoveDir == ImGuiDir_Left && is_open)
			{
				toggled = true;
				ImGui::NavMoveRequestCancel();
			}
			if (g.NavId == id && g.NavMoveDir == ImGuiDir_Right && !is_open) // If there's something upcoming on the line we may want to give it the priority?
			{
				toggled = true;
				ImGui::NavMoveRequestCancel();
			}

			if (toggled)
			{
				is_open = !is_open;
				window->DC.StateStorage->SetInt(id, is_open);
				g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledOpen;
			}
		}
		if (flags & ImGuiTreeNodeFlags_AllowItemOverlap)
			ImGui::SetItemAllowOverlap();

		// In this branch, TreeNodeBehavior() cannot toggle the selection so this will never trigger.
		if (selected != was_selected) //-V547
			g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

		// Render
		const ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
		ImGuiNavHighlightFlags nav_highlight_flags = ImGuiNavHighlightFlags_TypeThin;
		if (display_frame)
		{
			// Framed type
			const ImU32 bg_col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
			if (customBorderRadius != nullptr)
			{
				Vec4 rect = Vec4(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Max.x, frame_bb.Max.y);
				RenderFrame(rect, bg_col, 
					customBorderSize != nullptr ? *customBorderSize : g.Style.FrameBorderSize,
					*customBorderRadius);
				RenderNavHighlight(rect, id, nav_highlight_flags, *customBorderRadius);
			}
			else
			{
				ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, bg_col, true, style.FrameRounding);
				ImGui::RenderNavHighlight(frame_bb, id, nav_highlight_flags);
			}

			if (flags & ImGuiTreeNodeFlags_Bullet)
				ImGui::RenderBullet(window->DrawList, ImVec2(text_pos.x - text_offset_x * 0.60f, text_pos.y + g.FontSize * 0.5f), text_col);
			else if (!is_leaf)
				ImGui::RenderArrow(window->DrawList, ImVec2(text_pos.x - text_offset_x + padding.x, text_pos.y), text_col, is_open ? ImGuiDir_Down : ImGuiDir_Right, 1.0f);
			else // Leaf without bullet, left-adjusted text
				text_pos.x -= text_offset_x;
			if (flags & ImGuiTreeNodeFlags_ClipLabelForTrailingButton)
				frame_bb.Max.x -= g.FontSize + style.FramePadding.x;

			if (g.LogEnabled)
				ImGui::LogSetNextTextDecoration("###", "###");
			ImGui::RenderTextClipped(text_pos, frame_bb.Max, label.GetCString(), label_end, &label_size);
		}
		else
		{
			// Unframed typed for tree nodes
			if (hovered || selected)
			{
				const ImU32 bg_col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
				ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, bg_col, false);
			}
			ImGui::RenderNavHighlight(frame_bb, id, nav_highlight_flags);
			if (flags & ImGuiTreeNodeFlags_Bullet)
				ImGui::RenderBullet(window->DrawList, ImVec2(text_pos.x - text_offset_x * 0.5f, text_pos.y + g.FontSize * 0.5f), text_col);
			else if (!is_leaf)
				ImGui::RenderArrow(window->DrawList, ImVec2(text_pos.x - text_offset_x + padding.x, text_pos.y + g.FontSize * 0.15f), text_col, is_open ? ImGuiDir_Down : ImGuiDir_Right, 0.70f);
			if (g.LogEnabled)
				ImGui::LogSetNextTextDecoration(">", NULL);
			ImGui::RenderText(text_pos, label.GetCString(), label_end, false);
		}

		if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
			ImGui::TreePushOverrideID(id);
		IMGUI_TEST_ENGINE_ITEM_INFO(id, label.GetCString(), g.LastItemData.StatusFlags | (is_leaf ? 0 : ImGuiItemStatusFlags_Openable) | (is_open ? ImGuiItemStatusFlags_Opened : 0));
		return is_open;
    }

#pragma endregion

#pragma region Table

	COREGUI_API bool BeginTable(const String& label, int columnCount, TableFlags flags, const Vec2& outerSize, f32 innerWidth)
	{
		return ImGui::BeginTable(label.GetCString(), columnCount, flags, ImVec2(outerSize.x, outerSize.y), innerWidth);
	}

	COREGUI_API void EndTable()
	{
		ImGui::EndTable();
	}

	COREGUI_API void TableSetupColumn(const String& label, TableColumnFlags flags)
	{
		ImGui::TableSetupColumn(label.GetCString(), flags);
	}

	COREGUI_API void TableHeadersRow()
	{
		ImGui::TableHeadersRow();
	}

	COREGUI_API void TableNextRow(TableRowFlags flags, f32 minHeight)
	{
		ImGui::TableNextRow((ImGuiTableRowFlags)flags, minHeight);
	}

	COREGUI_API bool TableNextColumn()
	{
		return ImGui::TableNextColumn();
	}

	COREGUI_API Vec4 GetTableClipRect()
	{
		auto table = GImGui->CurrentTable;
		if (table == nullptr)
			return {};

		return Vec4(table->OuterRect.Min.x, table->OuterRect.Min.y, table->OuterRect.Max.x, table->OuterRect.Max.y);
	}

#pragma endregion

#pragma region Layout

	COREGUI_API void Spacing()
	{
		ImGui::Spacing();
	}

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

	COREGUI_API Vec2 GetContentRegionAvailableSpace()
	{
		ImVec2 space = ImGui::GetContentRegionAvail();
		return Vec2(space.x, space.y);
	}

	COREGUI_API f32 GetTextLineHeight()
	{
		return ImGui::GetTextLineHeight();
	}

	COREGUI_API f32 GetTextLineHeightWithSpacing()
	{
		return ImGui::GetTextLineHeightWithSpacing();
	}

    COREGUI_API void Indent(f32 indent)
    {
        ImGui::Indent(indent);
    }

    COREGUI_API void Unindent(f32 indent)
    {
        ImGui::Unindent(indent);
    }

	COREGUI_API Vec4 GetDefaultPadding()
	{
		auto padding = ImGui::GetStyle().FramePadding;
		return Vec4(padding.x, padding.y, padding.x, padding.y);
	}

#pragma endregion

#pragma region Events

	COREGUI_API Vec4 GetItemRect()
	{
		ImVec2 min = ImGui::GetItemRectMin();
		ImVec2 max = ImGui::GetItemRectMax();
		return Vec4(min.x, min.y, max.x, max.y);
	}

	COREGUI_API bool IsWindowHovered(HoveredFlags flags)
	{
		return ImGui::IsWindowHovered((int)flags);
	}

	COREGUI_API bool IsWindowHovered(const String& windowId, HoveredFlags flags)
	{
		return IsWindowHovered(flags) && windowId == GImGui->HoveredWindow->Name;
	}

	COREGUI_API bool IsWindowFocused(FocusFlags flags)
	{
		return ImGui::IsWindowFocused((int)flags);
	}

	COREGUI_API bool IsWindowFocused(const String& windowId, FocusFlags flags)
	{
		if (GImGui->WindowsFocusOrder.Size == 0)
			return ImGui::IsWindowFocused((int)flags);
		return windowId == GImGui->WindowsFocusOrder[GImGui->WindowsFocusOrder.Size - 1]->Name && ImGui::IsWindowFocused((int)flags);
	}

	COREGUI_API bool IsItemHovered(HoveredFlags flags)
	{
		return ImGui::IsItemHovered(flags);
	}

	COREGUI_API bool IsMouseDown(MouseButton button)
	{
		return ImGui::IsMouseDown((int)button);
	}

	COREGUI_API bool IsItemClicked(MouseButton button)
	{
		return ImGui::IsItemClicked((int)button);
	}

	COREGUI_API bool IsItemDoubleClicked(MouseButton button)
	{
		return ImGui::IsMouseDoubleClicked((int)button) && ImGui::IsItemHovered();
	}

	COREGUI_API bool IsItemFocused()
	{
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

	COREGUI_API void SetWantMouseCapture(bool value)
	{
		ImGui::GetIO().WantCaptureMouse = value;
	}

	COREGUI_API bool IsMouseClicked(MouseButton button)
	{
		return ImGui::IsMouseClicked((int)button);
	}

	COREGUI_API bool IsMouseDoubleClicked(MouseButton button)
	{
		return ImGui::IsMouseDoubleClicked((int)button);
	}

	COREGUI_API Vec2 GetMousePos()
	{
		ImVec2 pos = ImGui::GetMousePos();
		return Vec2(pos.x, pos.y);
	}

#pragma endregion

#pragma region Custom Drawing

    COREGUI_API void DrawRect(const Vec4& rect, const Color& color, Vec4 rounding, f32 thickness)
    {
		DrawRect(rect, color.ToU32(), rounding, thickness);
    }

	COREGUI_API void DrawRect(const Vec4& rect, u32 color, Vec4 rounding, f32 thickness)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		if (drawList == nullptr)
			return;

		if (rounding.x < 0.5f && rounding.y < 0.5f && rounding.z < 0.5f && rounding.w < 0.5f)
		{
			drawList->AddRect(ImVec2(rect.x, rect.y), ImVec2(rect.z, rect.w), color);
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
		drawList->PathStroke(color, 0, thickness);
	}
    
    COREGUI_API void FillRect(const Vec4& rect, const Color& color, Vec4 rounding)
    {
		FillRect(rect, color.ToU32(), rounding);
    }

	COREGUI_API void FillRect(const Vec4& rect, u32 color, Vec4 rounding)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		if (drawList == nullptr)
			return;

		if (rounding.x < 0.5f && rounding.y < 0.5f && rounding.z < 0.5f && rounding.w < 0.5f)
		{
			drawList->PrimReserve(6, 4);
			drawList->PrimRect(ImVec2(rect.x, rect.y), ImVec2(rect.z, rect.w), color);
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
		drawList->PathFillConvex(color);
	}

	COREGUI_API void FillRect(const Vec4& rect, const Gradient& gradient, Vec4 rounding)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		if (drawList == nullptr)
			return;

		ImVec2 uv = drawList->_Data->TexUvWhitePixel;

		f32 width = abs(rect.z - rect.x);
		f32 height = abs(rect.w - rect.y);

		const float roundingTL = rounding.x;
		const float roundingTR = rounding.y;
		const float roundingBR = rounding.z;
		const float roundingBL = rounding.w;

		drawList->PathArcToFast(ImVec2(rect.x + roundingTL, rect.y + roundingTL), roundingTL, 6, 9);
		drawList->PathArcToFast(ImVec2(rect.z - roundingTR, rect.y + roundingTR), roundingTR, 9, 12);
		drawList->PathArcToFast(ImVec2(rect.z - roundingBR, rect.w - roundingBR), roundingBR, 0, 3);
		drawList->PathArcToFast(ImVec2(rect.x + roundingBL, rect.w - roundingBL), roundingBL, 3, 6);

		// Fill

		if (drawList->Flags & ImDrawListFlags_AntiAliasedFill)
		{
			// Anti-aliased Fill
			const float AA_SIZE = drawList->_FringeScale;
			//const ImU32 col_trans = col & ~IM_COL32_A_MASK;
			const int idx_count = (drawList->_Path.Size - 2) * 3 + drawList->_Path.Size * 6;
			const int vtx_count = (drawList->_Path.Size * 2);
			drawList->PrimReserve(idx_count, vtx_count);

			// Add indexes for fill
			unsigned int vtx_inner_idx = drawList->_VtxCurrentIdx;
			unsigned int vtx_outer_idx = drawList->_VtxCurrentIdx + 1;
			for (int i = 2; i < drawList->_Path.Size; i++)
			{
				drawList->_IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx); 
				drawList->_IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + ((i - 1) << 1));
				drawList->_IdxWritePtr[2] = (ImDrawIdx)(vtx_inner_idx + (i << 1));
				drawList->_IdxWritePtr += 3;
			}

			// Compute normals
			drawList->_Data->TempBuffer.reserve_discard(drawList->_Path.Size);
			ImVec2* temp_normals = drawList->_Data->TempBuffer.Data;
			for (int i0 = drawList->_Path.Size - 1, i1 = 0; i1 < drawList->_Path.Size; i0 = i1++)
			{
				const ImVec2& p0 = drawList->_Path[i0];
				const ImVec2& p1 = drawList->_Path[i1];
				float dx = p1.x - p0.x;
				float dy = p1.y - p0.y;
				IM_NORMALIZE2F_OVER_ZERO(dx, dy);
				temp_normals[i0].x = dy;
				temp_normals[i0].y = -dx;
			}

			for (int i0 = drawList->_Path.Size - 1, i1 = 0; i1 < drawList->_Path.Size; i0 = i1++)
			{
				// Average normals
				const ImVec2& n0 = temp_normals[i0];
				const ImVec2& n1 = temp_normals[i1];
				float dm_x = (n0.x + n1.x) * 0.5f;
				float dm_y = (n0.y + n1.y) * 0.5f;
				IM_FIXNORMAL2F(dm_x, dm_y);
				dm_x *= AA_SIZE * 0.5f;
				dm_y *= AA_SIZE * 0.5f;

				// Add vertices
				drawList->_VtxWritePtr[0].pos.x = (drawList->_Path[i1].x - dm_x); 
				drawList->_VtxWritePtr[0].pos.y = (drawList->_Path[i1].y - dm_y); 

				ImVec2 pos0 = drawList->_VtxWritePtr[0].pos;
				f32 ratio0 = 0;
				if (gradient.GetDirection() == Gradient::LeftToRight)
				{
					ratio0 = Math::Clamp01((pos0.x - rect.x) / (rect.z - rect.x));
				}
				else
				{
					ratio0 = Math::Clamp01((pos0.y - rect.y) / (rect.w - rect.y));
				}

				drawList->_VtxWritePtr[0].uv = uv; 
				drawList->_VtxWritePtr[0].col = gradient.Evaluate(ratio0).ToU32(); // Inner

				drawList->_VtxWritePtr[1].pos.x = (drawList->_Path[i1].x + dm_x); 
				drawList->_VtxWritePtr[1].pos.y = (drawList->_Path[i1].y + dm_y); 

				ImVec2 pos1 = drawList->_VtxWritePtr[1].pos;
				f32 ratio1 = 0;
				if (gradient.GetDirection() == Gradient::TopToBottom)
				{
					ratio1 = Math::Clamp01((pos1.x - rect.x) / (rect.z - rect.x));
				}
				else
				{
					ratio1 = Math::Clamp01((pos1.y - rect.y) / (rect.w - rect.y));
				}

				drawList->_VtxWritePtr[1].uv = uv; 
				drawList->_VtxWritePtr[1].col = gradient.Evaluate(ratio1).ToU32() & ~IM_COL32_A_MASK;  // Outer
				drawList->_VtxWritePtr += 2;

				// Add indexes for fringes
				drawList->_IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1)); 
				drawList->_IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + (i0 << 1)); 
				drawList->_IdxWritePtr[2] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1));
				drawList->_IdxWritePtr[3] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1)); 
				drawList->_IdxWritePtr[4] = (ImDrawIdx)(vtx_outer_idx + (i1 << 1)); 
				drawList->_IdxWritePtr[5] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1));
				drawList->_IdxWritePtr += 6;
			}
			drawList->_VtxCurrentIdx += (ImDrawIdx)vtx_count;
		}
		else
		{
			// Non Anti-aliased Fill
			const int idx_count = (drawList->_Path.Size - 2) * 3;
			const int vtx_count = drawList->_Path.Size;
			drawList->PrimReserve(idx_count, vtx_count);
			for (int i = 0; i < vtx_count; i++)
			{
				ImVec2 pos = drawList->_Path.Data[i];
				f32 ratio = 0;
				if (gradient.GetDirection() == Gradient::LeftToRight)
				{
					ratio = Math::Clamp01((pos.x - rect.x) / (rect.z - rect.x));
				}
				else
				{
					ratio = Math::Clamp01((pos.y - rect.y) / (rect.w - rect.y));
				}

				drawList->_VtxWritePtr[0].pos = pos;
				drawList->_VtxWritePtr[0].uv = uv; 
				drawList->_VtxWritePtr[0].col = gradient.Evaluate(ratio).ToU32();
				drawList->_VtxWritePtr++;
			}
			for (int i = 2; i < drawList->_Path.Size; i++)
			{
				drawList->_IdxWritePtr[0] = (ImDrawIdx)(drawList->_VtxCurrentIdx);
				drawList->_IdxWritePtr[1] = (ImDrawIdx)(drawList->_VtxCurrentIdx + i - 1);
				drawList->_IdxWritePtr[2] = (ImDrawIdx)(drawList->_VtxCurrentIdx + i);
				drawList->_IdxWritePtr += 3;
			}
			drawList->_VtxCurrentIdx += (ImDrawIdx)vtx_count;
		}

		drawList->_Path.Size = 0;
	}

	COREGUI_API void RenderFrame(const Vec4& rect, const Color& color, f32 borderSize, Vec4 rounding)
	{
		RenderFrame(rect, color.ToU32(), borderSize, rounding);
	}

	COREGUI_API void RenderFrame(const Vec4& rect, u32 color, f32 borderSize, Vec4 rounding)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		FillRect(rect, color, rounding);

		ImVec2 p_min = ImVec2(rect.x, rect.y);
		ImVec2 p_max = ImVec2(rect.z, rect.w);

		if (borderSize > 0.1f)
		{
			DrawRect(rect + Vec4(1, 1, 1, 1), ImGui::GetColorU32(ImGuiCol_BorderShadow), rounding, borderSize);
			DrawRect(rect, ImGui::GetColorU32(ImGuiCol_Border), rounding, borderSize);
		}
	}


	COREGUI_API void RenderNavHighlight(const Vec4& rect, ID id, f32 thickness, Vec4 rounding, NavHighlightFlags flags)
	{
		ImGuiContext& g = *GImGui;
		if (id != g.NavId)
			return;
		if (g.NavDisableHighlight && !(flags & ImGuiNavHighlightFlags_AlwaysDraw))
			return;
		ImGuiWindow* window = g.CurrentWindow;
		if (window->DC.NavHideHighlightOneFrame)
			return;

		//float rounding = (flags & ImGuiNavHighlightFlags_NoRounding) ? 0.0f : g.Style.FrameRounding;
		ImRect display_rect = ImRect(rect.x, rect.y, rect.z, rect.w);
		display_rect.ClipWith(window->ClipRect);

		if (flags & ImGuiNavHighlightFlags_TypeDefault)
		{
			const float THICKNESS = 2.0f;
			const float DISTANCE = 3.0f + THICKNESS * 0.5f;
			display_rect.Expand(ImVec2(DISTANCE, DISTANCE));
			bool fully_visible = window->ClipRect.Contains(display_rect);
			if (!fully_visible)
				window->DrawList->PushClipRect(display_rect.Min, display_rect.Max);
			ImVec2 min = display_rect.Min + ImVec2(THICKNESS * 0.5f, THICKNESS * 0.5f);
			ImVec2 max = display_rect.Max - ImVec2(THICKNESS * 0.5f, THICKNESS * 0.5f);

			DrawRect(Vec4(min.x, min.y, max.x, max.y), ImGui::GetColorU32(ImGuiCol_NavHighlight), rounding, thickness);
			if (!fully_visible)
				window->DrawList->PopClipRect();
		}

		if (flags & ImGuiNavHighlightFlags_TypeThin)
		{
			DrawRect(Vec4(display_rect.Min.x, display_rect.Min.y, display_rect.Max.x, display_rect.Max.y), 
				ImGui::GetColorU32(ImGuiCol_NavHighlight), rounding, thickness);
		}
	}

#pragma endregion

} // namespace CE

