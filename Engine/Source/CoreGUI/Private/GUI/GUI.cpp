
#include "CoreGUI.h"

#define IMGUI_DEFINE_MATH_OPERATORS

// Import Dear ImGui from VulkanRHI
#define IMGUI_API DLL_IMPORT
#include "imgui.h"
#include "imgui_internal.h"

#include "misc/cpp/imgui_stdlib.h"

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

