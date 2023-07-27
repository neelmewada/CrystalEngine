
#include "CoreGUIInternal.h"

//#define IMGUI_DEFINE_MATH_OPERATORS
//
//// Import Dear ImGui from VulkanRHI
//#define IMGUI_API DLL_IMPORT
//#include "imgui.h"
//#include "imgui_internal.h"
//
//#define IM_NORMALIZE2F_OVER_ZERO(VX,VY)     { float d2 = VX*VX + VY*VY; if (d2 > 0.0f) { float inv_len = ImRsqrt(d2); VX *= inv_len; VY *= inv_len; } } (void)0
//#define IM_FIXNORMAL2F_MAX_INVLEN2          100.0f // 500.0f (see #4053, #3366)
//#define IM_FIXNORMAL2F(VX,VY)               { float d2 = VX*VX + VY*VY; if (d2 > 0.000001f) { float inv_len2 = 1.0f / d2; if (inv_len2 > IM_FIXNORMAL2F_MAX_INVLEN2) inv_len2 = IM_FIXNORMAL2F_MAX_INVLEN2; VX *= inv_len2; VY *= inv_len2; } } (void)0
//
//#include "misc/cpp/imgui_stdlib.h"

namespace CE::GUI
{
	using namespace ImStb;

	COREGUI_API Array<Vec2> gPaddingXStack{};
	
	static Array<Rect> coordinateSpaceStack{};

	static Rect ToGlobalSpace(const Rect& localSpaceRect)
	{
		if (coordinateSpaceStack.IsEmpty())
			return localSpaceRect;

		const auto& parentSpace = coordinateSpaceStack.Top();
		auto originalSize = localSpaceRect.max - localSpaceRect.min;

		return Rect(parentSpace.min + localSpaceRect.min, parentSpace.min + localSpaceRect.min + originalSize);
	}

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

	COREGUI_API Vec2 GetWindowScroll()
	{
		ImGuiWindow* window = GImGui->CurrentWindow;
		if (window == nullptr)
			return {};
		return Vec2(window->Scroll.x, window->Scroll.y);
	}

	COREGUI_API void SetWindowFontScale(f32 scale)
	{
		ImGui::SetWindowFontScale(scale);
	}

	COREGUI_API f32 GetWindowFontScale()
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		return window->FontWindowScale;
	}

	COREGUI_API void PushFont(void* fontHandle)
	{
		ImGui::PushFont((ImFont*)fontHandle);
	}

	COREGUI_API void PopFont()
	{
		ImGui::PopFont();
	}

	COREGUI_API void SetCurrentFont(void* fontHandle)
	{
		ImGui::SetCurrentFont((ImFont*)fontHandle);
	}

	COREGUI_API f32 GetFontSize()
	{
		return ImGui::GetFontSize();
	}

	COREGUI_API Viewport* GetCurrentViewport()
	{
		return (Viewport*)ImGui::GetWindowViewport();
	}

	COREGUI_API Viewport* GetMainViewport()
	{
		return (Viewport*)ImGui::GetMainViewport();
	}

	COREGUI_API Vec4 WindowRectToGlobalRect(const Vec4& rectInWindow)
	{
		Vec2 windowPos = GetWindowPos();
		Vec2 scroll = GetWindowScroll();
		return ToWindowCoordinateSpace(rectInWindow + Vec4(windowPos.x - scroll.x, windowPos.y - scroll.y, windowPos.x - scroll.x, windowPos.y - scroll.y));
	}

	COREGUI_API Vec4 GlobalRectToWindowRect(const Vec4& globalRect)
	{
		Vec2 windowPos = GetWindowPos();
		Vec2 scroll = GetWindowScroll();
		return globalRect - Vec4(windowPos.x - scroll.x, windowPos.y - scroll.y, windowPos.x - scroll.x, windowPos.y - scroll.y);
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

	COREGUI_API ID DockSpace(ID id, Vec2 size, DockFlags dockFlags)
	{
		return ImGui::DockSpace(id, ImVec2(size.x, size.y), (ImGuiDockNodeFlags)dockFlags);
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

	COREGUI_API bool BeginChild(const String& name, ID id, const Vec2& sizeArg, f32 borderThickness,
		const Vec2& minSize, const Vec2& maxSize, const Vec4& padding, const Color& background, WindowFlags flags)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* parent_window = g.CurrentWindow;
		
		flags |= WF_NoTitleBar | WF_NoResize | WF_NoSavedSettings | WF_ChildWindow | WF_NoDocking;
		flags |= ((WindowFlags)parent_window->Flags & WF_NoMove);  // Inherit the NoMove flag

		// Size
		const ImVec2 contentAvail = ImGui::GetContentRegionAvail();
		ImVec2 size = ImFloor(ImVec2(sizeArg.x, sizeArg.y));
		const int auto_fit_axises = ((size.x == 0.0f) ? (1 << ImGuiAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << ImGuiAxis_Y) : 0x00);
		if (size.x <= 0.0f)
			size.x = ImMax(contentAvail.x + size.x, 4.0f); // Arbitrary minimum child size (0.0f causing too many issues)
		if (size.y <= 0.0f)
			size.y = ImMax(contentAvail.y + size.y, 4.0f);
		if (minSize.x > 0 && size.x < minSize.x)
			size.x = minSize.x;
		if (minSize.y > 0 && size.y < minSize.y)
			size.y = minSize.y;
		if (maxSize.x > 0 && size.x > maxSize.x)
			size.x = maxSize.x;
		if (maxSize.y > 0 && size.y > maxSize.y)
			size.y = maxSize.y;

		ImGui::SetNextWindowSize(size);

		// Build up name. If you need to append to a same child from multiple location in the ID stack, use BeginChild(ImGuiID id) with a stable value.
		const char* temp_window_name;
		if (!name.IsEmpty())
			ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, name.GetCString(), id);
		else
			ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%08X", parent_window->Name, id);

		const float backup_border_size = g.Style.ChildBorderSize;
		g.Style.ChildBorderSize = borderThickness;

		ImGui::PushStyleColor(ImGuiCol_WindowBg, background.ToU32());

		if (gPaddingXStack.NonEmpty())
		{
			auto avail = ImGui::GetContentRegionAvail();
			ImGui::SetNextWindowSizeConstraints(ImVec2(-1, -1), ImVec2(avail.x - gPaddingXStack.Top().top, -1));
		}

		bool ret = ImGui::Begin(temp_window_name, NULL, flags);

		ImGui::PopStyleColor(1);
		
		g.Style.ChildBorderSize = backup_border_size;

		ImGuiWindow* child_window = g.CurrentWindow;
		child_window->ChildId = id;
		child_window->AutoFitChildAxises = (ImS8)auto_fit_axises;
		child_window->DC.CursorPos += ImVec2(0, padding.top);

		// Set the cursor to handle case where the user called SetNextWindowPos()+BeginChild() manually.
		// While this is not really documented/defined, it seems that the expected thing to do.
		if (child_window->BeginCount == 1)
			parent_window->DC.CursorPos = child_window->Pos;
		
		// Process navigation-in immediately so NavInit can run on first frame
		// Can enter a child if (A) it has navigatable items or (B) it can be scrolled.
		if (g.NavActivateId == id && !(flags & WF_NavFlattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY))
		{
			ImGui::FocusWindow(child_window);
			ImGui::NavInitWindow(child_window, false);
			ImGui::SetActiveID(id + 1, child_window); // Steal ActiveId with another arbitrary id so that key-press won't activate child item
			g.ActiveIdSource = g.NavInputSource;
		}

		return ret;
	}

	COREGUI_API void EndChild()
	{
		ImGui::EndChild();
	}

	COREGUI_API bool BeginChild(const Rect& localRect, ID id, const String& title, const Vec4& padding, const GuiStyleState& styleState, WindowFlags windowFlags)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* parent_window = g.CurrentWindow;
		
		ImGuiWindowFlags flags = windowFlags;

		flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoDocking;
		flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove);  // Inherit the NoMove flag

		auto rect = WindowRectToGlobalRect(localRect);

		SetNextWindowPos(rect.min);
		SetNextWindowSize(rect.max - rect.min);

		// Build up name. If you need to append to a same child from multiple location in the ID stack, use BeginChild(ImGuiID id) with a stable value.
		const char* temp_window_name;
		if (!title.IsEmpty())
			ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, title.GetCString(), id);
		else
			ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%08X", parent_window->Name, id);
		
		const float backup_border_size = g.Style.ChildBorderSize;
		bool ret = ImGui::Begin(temp_window_name, NULL, flags);

		ImGuiWindow* child_window = g.CurrentWindow;
		child_window->ChildId = id;

		// Set the cursor to handle case where the user called SetNextWindowPos()+BeginChild() manually.
		// While this is not really documented/defined, it seems that the expected thing to do.
		if (child_window->BeginCount == 1)
			parent_window->DC.CursorPos = child_window->Pos;

		// Process navigation-in immediately so NavInit can run on first frame
		// Can enter a child if (A) it has navigatable items or (B) it can be scrolled.
		if (g.NavActivateId == id && !(flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY))
		{
			ImGui::FocusWindow(child_window);
			ImGui::NavInitWindow(child_window, false);
			ImGui::SetActiveID(id + 1, child_window); // Steal ActiveId with another arbitrary id so that key-press won't activate child item
			g.ActiveIdSource = g.NavInputSource;
		}

		return ret;
	}

	/////////////////////////////////////////////////////////////////////
	// Style

	COREGUI_API GUI::Style& GetStyle()
	{
		return *(GUI::Style*)&ImGui::GetStyle();
	}

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

	COREGUI_API void Image(const Rect& localRect, GuiTextureID textureId, const GuiStyleState& style, const Vec2& uvMin, const Vec2& uvMax)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;

		Rect rect = WindowRectToGlobalRect(localRect);

		ImRect bb(rect.min.x, rect.min.y, rect.max.x, rect.max.y);

		window->DrawList->AddImage(textureId, bb.Min, bb.Max, ImVec2(uvMin.x, uvMin.y), ImVec2(uvMax.x, uvMax.y));
	}

	COREGUI_API void Text(const Rect& localRect, const String& text, const GuiStyleState& style)
	{
		GUI::Text(localRect, text.GetCString(), style);
	}

	COREGUI_API void Text(const Rect& localRect, const char* text, const GuiStyleState& style)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;
		ImGuiContext& g = *GImGui;

		Rect rect = ToWindowCoordinateSpace(localRect);

		const char* text_end = NULL;

		// Accept null ranges
		if (text == text_end)
			text = text_end = "";
		ImGuiTextFlags flags = ImGuiTextFlags_None;

		// Calculate length
		const char* text_begin = text;
		if (text_end == NULL)
			text_end = text + strlen(text); // FIXME-OPT

		auto textAlign = style.textAlign;
		ImVec2 align = ImVec2(0.5f, 0.5f);

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

		ImGui::SetCursorPos(ImVec2(rect.left, rect.top));

		ImVec2 inSize = ImVec2(rect.right - rect.left, rect.bottom - rect.top);
		
		const ImVec2 text_pos(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
		const float wrap_pos_x = window->DC.TextWrapPos;
		const bool wrap_enabled = (wrap_pos_x >= 0.0f);
		//if (text_end - text <= 2000)
		{
			// Common case
			//const float wrap_width = wrap_enabled ? ImGui::CalcWrapWidthForPos(window->DC.CursorPos, wrap_pos_x) : 0.0f;
			const ImVec2 text_size = ImGui::CalcTextSize(text_begin, text_end, false);// , wrap_width);

			ImRect bb(text_pos, text_pos + inSize);
			ImGui::ItemSize(inSize, 0.0f);
			if (!ImGui::ItemAdd(bb, 0))
				return;

			ImGui::PushStyleColor(StyleCol_Text, style.foreground.ToU32());
			ImGui::RenderTextClipped(bb.Min, bb.Max, text_begin, text_end, &text_size, align);
			
			//ImVec4 clipRect = bb.ToVec4();
			//window->DrawList->AddText(g.Font, g.FontSize, bb.Min, style.foreground.ToU32(), text, text_end, rect.right - rect.left);

			ImGui::PopStyleColor(1);
		}
	}

	COREGUI_API void TextWrapped(const Rect& localRect, const String& text, const GuiStyleState& style)
	{
		return GUI::TextWrapped(localRect, text.GetCString(), style);
	}

	COREGUI_API void TextWrapped(const Rect& localRect, const char* text, const GuiStyleState& style)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;
		ImGuiContext& g = *GImGui;

		Rect rect = ToWindowCoordinateSpace(localRect);

		const char* text_end = NULL;

		// Accept null ranges
		if (text == text_end)
			text = text_end = "";
		ImGuiTextFlags flags = ImGuiTextFlags_None;

		// Calculate length
		const char* text_begin = text;
		if (text_end == NULL)
			text_end = text + strlen(text); // FIXME-OPT

		ImGui::SetCursorPos(ImVec2(rect.left, rect.top));

		ImVec2 inSize = ImVec2(rect.right - rect.left, rect.bottom - rect.top);

		const ImVec2 text_pos(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
		const float wrap_pos_x = window->DC.TextWrapPos;
		const bool wrap_enabled = (wrap_pos_x >= 0.0f);
		//if (text_end - text <= 2000)
		{
			// Common case
			//const float wrap_width = wrap_enabled ? ImGui::CalcWrapWidthForPos(window->DC.CursorPos, wrap_pos_x) : 0.0f;
			const ImVec2 text_size = ImGui::CalcTextSize(text_begin, text_end, false);// , wrap_width);

			ImRect bb(text_pos, text_pos + inSize);
			ImGui::ItemSize(inSize, 0.0f);
			if (!ImGui::ItemAdd(bb, 0))
				return;

			ImVec4 clipRect = bb.ToVec4();
			window->DrawList->AddText(g.Font, g.FontSize, bb.Min, style.foreground.ToU32(), text, text_end, rect.right - rect.left);
		}
	}

	COREGUI_API void Text(const char* text, const Vec2& size, TextAlign textAlign)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;
		ImGuiContext& g = *GImGui;

		const char* text_end = NULL;

		// Accept null ranges
		if (text == text_end)
			text = text_end = "";
		ImGuiTextFlags flags = ImGuiTextFlags_None;

		Vec2 paddingX = {};
		if (gPaddingXStack.NonEmpty())
		{
			paddingX = gPaddingXStack.Top();
		}

		bool fixedSize = size.x > 0 || size.y > 0;

		// Calculate length
		const char* text_begin = text;
		if (text_end == NULL)
			text_end = text + strlen(text); // FIXME-OPT

		ImVec2 align = ImVec2(0.5f, 0.5f);
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

		const ImVec2 text_pos(window->DC.CursorPos.x + paddingX.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
		const float wrap_pos_x = window->DC.TextWrapPos;
		const bool wrap_enabled = (wrap_pos_x >= 0.0f);
		if (text_end - text <= 2000 || wrap_enabled)
		{
			// Common case
			const float wrap_width = wrap_enabled ? ImGui::CalcWrapWidthForPos(window->DC.CursorPos, wrap_pos_x) : 0.0f;
			const ImVec2 text_size = ImGui::CalcTextSize(text_begin, text_end, false, wrap_width);
			
			if (fixedSize)
			{
				ImVec2 bbSize = ImVec2(size.x > 0 ? size.x : text_size.x, size.y > 0 ? size.y : text_size.y);
				ImRect bb(text_pos, text_pos + bbSize);
				ImGui::ItemSize(bbSize, 0.0f);
				if (!ImGui::ItemAdd(bb, 0))
					return;

				ImGui::RenderTextClipped(bb.Min, bb.Max, text_begin, text_end, &text_size, align, &bb);
			}
			else
			{
				ImRect bb(text_pos, text_pos + text_size);
				ImGui::ItemSize(text_size, 0.0f);
				if (!ImGui::ItemAdd(bb, 0))
					return;

				// Render (we don't hide text after ## in this end-user function)
				ImGui::RenderTextWrapped(bb.Min, text_begin, text_end, wrap_width);
			}
		}
		else
		{
			// Long text!
			// Perform manual coarse clipping to optimize for long multi-line text
			// - From this point we will only compute the width of lines that are visible. Optimization only available when word-wrapping is disabled.
			// - We also don't vertically center the text within the line full height, which is unlikely to matter because we are likely the biggest and only item on the line.
			// - We use memchr(), pay attention that well optimized versions of those str/mem functions are much faster than a casually written loop.
			const char* line = text;
			const float line_height = GetTextLineHeight();
			ImVec2 text_size(0, 0);

			// Lines to skip (can't skip when logging text)
			ImVec2 pos = text_pos;
			if (!g.LogEnabled)
			{
				int lines_skippable = (int)((window->ClipRect.Min.y - text_pos.y) / line_height);
				if (lines_skippable > 0)
				{
					int lines_skipped = 0;
					while (line < text_end && lines_skipped < lines_skippable)
					{
						const char* line_end = (const char*)memchr(line, '\n', text_end - line);
						if (!line_end)
							line_end = text_end;
						if ((flags & ImGuiTextFlags_NoWidthForLargeClippedText) == 0)
							text_size.x = ImMax(text_size.x, ImGui::CalcTextSize(line, line_end).x);
						line = line_end + 1;
						lines_skipped++;
					}
					pos.y += lines_skipped * line_height;
				}
			}

			// Lines to render
			if (line < text_end)
			{
				ImRect line_rect(pos, pos + ImVec2(FLT_MAX, line_height));
				while (line < text_end)
				{
					if (ImGui::IsClippedEx(line_rect, 0))
						break;

					const char* line_end = (const char*)memchr(line, '\n', text_end - line);
					if (!line_end)
						line_end = text_end;
					text_size.x = ImMax(text_size.x, ImGui::CalcTextSize(line, line_end).x);
					ImGui::RenderText(pos, line, line_end, false);
					line = line_end + 1;
					line_rect.Min.y += line_height;
					line_rect.Max.y += line_height;
					pos.y += line_height;
				}

				// Count remaining lines
				int lines_skipped = 0;
				while (line < text_end)
				{
					const char* line_end = (const char*)memchr(line, '\n', text_end - line);
					if (!line_end)
						line_end = text_end;
					if ((flags & ImGuiTextFlags_NoWidthForLargeClippedText) == 0)
						text_size.x = ImMax(text_size.x, ImGui::CalcTextSize(line, line_end).x);
					line = line_end + 1;
					lines_skipped++;
				}
				pos.y += lines_skipped * line_height;
			}
			text_size.y = (pos - text_pos).y;

			ImRect bb(text_pos, text_pos + text_size);
			ImGui::ItemSize(text_size, 0.0f);
			ImGui::ItemAdd(bb, 0);
		}
	}

	COREGUI_API void Text(const String& text, const Vec2& size, TextAlign align)
	{
		Text(text.GetCString(), size, align);
	}

	COREGUI_API void TextColored(const char* text, const Color& color)
	{
		ImGui::TextColored(ImVec4(color.r, color.g, color.b, color.a), text);
	}

	COREGUI_API void TextColored(const String& text, const Color& color)
	{
		ImGui::TextColored(ImVec4(color.r, color.g, color.b, color.a), text.GetCString());
	}

	COREGUI_API bool Button(const Rect& localRect, const String& label, 
		const GuiStyleState& styleState, bool& isHovered, bool& isHeld,
		const Vec4& padding, ButtonFlags buttonFlags)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		Rect rect = ToWindowCoordinateSpace(localRect);

		int flags = (int)buttonFlags;

		const ImGuiID id = window->GetID(label.GetCString());
		const ImVec2 label_size = ImGui::CalcTextSize(label.GetCString(), NULL, true);

		ImGui::SetCursorPos(ImVec2(rect.left, rect.top));

		ImVec2 pos = window->DC.CursorPos;
		if (((int)flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
			pos.y += window->DC.CurrLineTextBaseOffset - padding.top;//style.FramePadding.y;

		ImVec2 size = ImVec2(rect.right - rect.left, rect.bottom - rect.top);

		const ImRect bb(pos, pos + size);
		const ImRect bbInner(bb.Min.x + padding.x, bb.Min.y + padding.y, bb.Max.x - padding.z, bb.Max.y - padding.w);
		Rect innerRect{ bb.Min.x + padding.x, bb.Min.y + padding.y, bb.Max.x - padding.z, bb.Max.y - padding.w };

		ImGui::ItemSize(bb, padding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat)
			flags |= ImGuiButtonFlags_Repeat;

		bool pressed = ImGui::ButtonBehavior(bb, id, &isHovered, &isHeld, flags);

		// Render highlight
		ImGui::RenderNavHighlight(bb, id);

		const GUI::GuiStyleState& curState = styleState;
		GUI::FillRect(WindowRectToGlobalRect(localRect), curState.background, curState.borderRadius);

		if (curState.borderThickness > 0 && curState.borderColor.a > 0)
		{
			GUI::DrawRect(WindowRectToGlobalRect(localRect), curState.borderColor, curState.borderRadius, curState.borderThickness);
		}

		auto textAlign = curState.textAlign;
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
		ImGui::RenderTextClipped(bbInner.Min, bbInner.Max, label.GetCString(), NULL, &label_size, align, &bb);

		// Automatically close popups
		//if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
		//    CloseCurrentPopup();

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label.GetCString(), g.LastItemData.StatusFlags);
		return pressed;
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

	COREGUI_API bool SelectableEx(ID id, bool selected, SelectableFlags flags, const Vec2& sizeVec,
		f32 borderThickness, const Color& hoveredCol, const Color& pressedCol, const Color& activeCol,
		const Color& hoveredBorderCol, const Color& pressedBorderCol, const Color& activeBorderCol)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		Vec4 containerPadding{};
		if (gPaddingXStack.NonEmpty())
		{
			containerPadding.left = gPaddingXStack.Top().left;
			containerPadding.right = gPaddingXStack.Top().right;
		}

		window->DC.CursorPos += ImVec2(containerPadding.x, 0);

		// Submit label or explicit size to ItemSize(), whereas ItemAdd() will submit a larger/spanning rectangle.
		ImVec2 label_size = ImGui::CalcTextSize("", NULL, true);
		ImVec2 size(sizeVec.x != 0.0f ? sizeVec.x : label_size.x, sizeVec.y != 0.0f ? sizeVec.y : label_size.y);
		if (size.x > 0)
		{
			size.x -= containerPadding.right;
			containerPadding.right = 0;
		}

		ImVec2 pos = window->DC.CursorPos;
		pos.y += window->DC.CurrLineTextBaseOffset;
		ImGui::ItemSize(ImVec2(size.x, size.y), 0.0f);

		// Fill horizontal space
		// We don't support (size < 0.0f) in Selectable() because the ItemSpacing extension would make explicitly right-aligned sizes not visibly match other widgets.
		const bool span_all_columns = (flags & ImGuiSelectableFlags_SpanAllColumns) != 0;
		const float min_x = span_all_columns ? window->ParentWorkRect.Min.x : pos.x;
		const float max_x = span_all_columns ? window->ParentWorkRect.Max.x : window->WorkRect.Max.x;
		if (size.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth))
		{
			size.x = ImMax(label_size.x, max_x - min_x);
			size.x -= containerPadding.right;
			containerPadding.right = 0;
		}

		// Text stays at the submission position, but bounding box may be extended on both sides
		const ImVec2 text_min = pos;
		const ImVec2 text_max(min_x + size.x, pos.y + size.y);

		// Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
		ImRect bb(min_x, pos.y, text_max.x, text_max.y);
		if ((flags & ImGuiSelectableFlags_NoPadWithHalfSpacing) == 0)
		{
			const float spacing_x = span_all_columns ? 0.0f : style.ItemSpacing.x;
			const float spacing_y = style.ItemSpacing.y;
			const float spacing_L = IM_FLOOR(spacing_x * 0.50f);
			const float spacing_U = IM_FLOOR(spacing_y * 0.50f);
			bb.Min.x -= spacing_L;
			bb.Min.y -= spacing_U;
			bb.Max.x += (spacing_x - spacing_L);
			bb.Max.y += (spacing_y - spacing_U);
		}

		Vec4 bbRect{ bb.Min.x, bb.Min.y, bb.Max.x, bb.Max.y };
		//if (g.IO.KeyCtrl) { GetForegroundDrawList()->AddRect(bb.Min, bb.Max, IM_COL32(0, 255, 0, 255)); }

		// Modify ClipRect for the ItemAdd(), faster than doing a PushColumnsBackground/PushTableBackground for every Selectable..
		const float backup_clip_rect_min_x = window->ClipRect.Min.x;
		const float backup_clip_rect_max_x = window->ClipRect.Max.x;
		if (span_all_columns)
		{
			window->ClipRect.Min.x = window->ParentWorkRect.Min.x;
			window->ClipRect.Max.x = window->ParentWorkRect.Max.x;
		}

		const bool disabled_item = (flags & ImGuiSelectableFlags_Disabled) != 0;
		const bool item_add = ImGui::ItemAdd(bb, id, NULL, disabled_item ? ImGuiItemFlags_Disabled : ImGuiItemFlags_None);
		if (span_all_columns)
		{
			window->ClipRect.Min.x = backup_clip_rect_min_x;
			window->ClipRect.Max.x = backup_clip_rect_max_x;
		}

		if (!item_add)
			return false;

		const bool disabled_global = (g.CurrentItemFlags & ImGuiItemFlags_Disabled) != 0;
		if (disabled_item && !disabled_global) // Only testing this as an optimization
			ImGui::BeginDisabled();

		// FIXME: We can standardize the behavior of those two, we could also keep the fast path of override ClipRect + full push on render only,
		// which would be advantageous since most selectable are not selected.
		if (span_all_columns && window->DC.CurrentColumns)
			ImGui::PushColumnsBackground();
		else if (span_all_columns && g.CurrentTable)
			ImGui::TablePushBackgroundChannel();

		// We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
		ImGuiButtonFlags button_flags = 0;
		if (flags & ImGuiSelectableFlags_NoHoldingActiveID) { button_flags |= ImGuiButtonFlags_NoHoldingActiveId; }
		if (flags & ImGuiSelectableFlags_NoSetKeyOwner) { button_flags |= ImGuiButtonFlags_NoSetKeyOwner; }
		if (flags & ImGuiSelectableFlags_SelectOnClick) { button_flags |= ImGuiButtonFlags_PressedOnClick; }
		if (flags & ImGuiSelectableFlags_SelectOnRelease) { button_flags |= ImGuiButtonFlags_PressedOnRelease; }
		if (flags & ImGuiSelectableFlags_AllowDoubleClick) { button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick; }
		if (flags & ImGuiSelectableFlags_AllowItemOverlap) { button_flags |= ImGuiButtonFlags_AllowItemOverlap; }

		const bool was_selected = selected;
		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, button_flags);

		// Auto-select when moved into
		// - This will be more fully fleshed in the range-select branch
		// - This is not exposed as it won't nicely work with some user side handling of shift/control
		// - We cannot do 'if (g.NavJustMovedToId != id) { selected = false; pressed = was_selected; }' for two reasons
		//   - (1) it would require focus scope to be set, need exposing PushFocusScope() or equivalent (e.g. BeginSelection() calling PushFocusScope())
		//   - (2) usage will fail with clipped items
		//   The multi-select API aim to fix those issues, e.g. may be replaced with a BeginSelection() API.
		if ((flags & ImGuiSelectableFlags_SelectOnNav) && g.NavJustMovedToId != 0 && g.NavJustMovedToFocusScopeId == g.CurrentFocusScopeId)
			if (g.NavJustMovedToId == id)
				selected = pressed = true;

		// Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
		if (pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
		{
			if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
			{
				ImGui::SetNavID(id, window->DC.NavLayerCurrent, g.CurrentFocusScopeId, ImGui::WindowRectAbsToRel(window, bb)); // (bb == NavRect)
				g.NavDisableHighlight = true;
			}
		}
		if (pressed)
			ImGui::MarkItemEdited(id);

		if (flags & ImGuiSelectableFlags_AllowItemOverlap)
			ImGui::SetItemAllowOverlap();

		// In this branch, Selectable() cannot toggle the selection so this will never trigger.
		if (selected != was_selected) //-V547
			g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

		// Render
		if (hovered || selected)
		{
			//const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
			//ImGui::RenderFrame(bb.Min, bb.Max, col, false, 0.0f);
			Color col = (held && hovered) ? pressedCol : hovered ? hoveredCol : activeCol;
			Color borderCol = (held && hovered) ? pressedBorderCol : hovered ? hoveredBorderCol : activeBorderCol;
			GUI::RenderFrame(bbRect, col, borderCol, borderThickness);
		}
		ImGui::RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);

		if (span_all_columns && window->DC.CurrentColumns)
			ImGui::PopColumnsBackground();
		else if (span_all_columns && g.CurrentTable)
			ImGui::TablePopBackgroundChannel();

		//ImGui::RenderTextClipped(text_min, text_max, label, NULL, &label_size, style.SelectableTextAlign, &bb);

		// Automatically close popups
		if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiSelectableFlags_DontClosePopups) && !(g.LastItemData.InFlags & ImGuiItemFlags_SelectableDontClosePopup))
			ImGui::CloseCurrentPopup();

		if (disabled_item && !disabled_global)
			ImGui::EndDisabled();

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
		return pressed; //-V1020
	}

	COREGUI_API bool Selectable(const Rect& localRect, ID id, bool selected, 
		const GuiStyleState& hoveredState, const GuiStyleState& pressedState, const GuiStyleState& selectedState, 
		SelectableFlags flags)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		auto rect = ToWindowCoordinateSpace(localRect);
		auto rectSize = rect.max - rect.min;

		auto globalRect = GUI::WindowRectToGlobalRect(localRect);

		GUI::SetCursorPos(rect.min);

		// Submit label or explicit size to ItemSize(), whereas ItemAdd() will submit a larger/spanning rectangle.
		//ImVec2 label_size = ImGui::CalcTextSize("label", NULL, true);
		ImVec2 size(rectSize.x, rectSize.y);
		ImVec2 pos = window->DC.CursorPos;
		//pos.y += window->DC.CurrLineTextBaseOffset;
		ImGui::ItemSize(size, 0.0f);

		// Fill horizontal space
		// We don't support (size < 0.0f) in Selectable() because the ItemSpacing extension would make explicitly right-aligned sizes not visibly match other widgets.
		const bool span_all_columns = (flags & ImGuiSelectableFlags_SpanAllColumns) != 0;
		const float min_x = span_all_columns ? window->ParentWorkRect.Min.x : pos.x;
		const float max_x = span_all_columns ? window->ParentWorkRect.Max.x : window->WorkRect.Max.x;
		//if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth))
		//	size.x = ImMax(label_size.x, max_x - min_x);

		// Text stays at the submission position, but bounding box may be extended on both sides
		//const ImVec2 text_min = pos;
		//const ImVec2 text_max(min_x + size.x, pos.y + size.y);

		// Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
		ImRect bb(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
		//if (g.IO.KeyCtrl) { GetForegroundDrawList()->AddRect(bb.Min, bb.Max, IM_COL32(0, 255, 0, 255)); }

		// Modify ClipRect for the ItemAdd(), faster than doing a PushColumnsBackground/PushTableBackground for every Selectable..
		const float backup_clip_rect_min_x = window->ClipRect.Min.x;
		const float backup_clip_rect_max_x = window->ClipRect.Max.x;
		if (span_all_columns)
		{
			window->ClipRect.Min.x = window->ParentWorkRect.Min.x;
			window->ClipRect.Max.x = window->ParentWorkRect.Max.x;
		}

		const bool disabled_item = (flags & ImGuiSelectableFlags_Disabled) != 0;
		const bool item_add = ImGui::ItemAdd(bb, id, NULL, disabled_item ? ImGuiItemFlags_Disabled : ImGuiItemFlags_None);
		if (span_all_columns)
		{
			window->ClipRect.Min.x = backup_clip_rect_min_x;
			window->ClipRect.Max.x = backup_clip_rect_max_x;
		}

		if (!item_add)
			return false;

		const bool disabled_global = (g.CurrentItemFlags & ImGuiItemFlags_Disabled) != 0;
		if (disabled_item && !disabled_global) // Only testing this as an optimization
			ImGui::BeginDisabled();

		// FIXME: We can standardize the behavior of those two, we could also keep the fast path of override ClipRect + full push on render only,
		// which would be advantageous since most selectable are not selected.
		if (span_all_columns && window->DC.CurrentColumns)
			ImGui::PushColumnsBackground();
		else if (span_all_columns && g.CurrentTable)
			ImGui::TablePushBackgroundChannel();

		// We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
		ImGuiButtonFlags button_flags = 0;
		if (flags & ImGuiSelectableFlags_NoHoldingActiveID) { button_flags |= ImGuiButtonFlags_NoHoldingActiveId; }
		if (flags & ImGuiSelectableFlags_NoSetKeyOwner) { button_flags |= ImGuiButtonFlags_NoSetKeyOwner; }
		if (flags & ImGuiSelectableFlags_SelectOnClick) { button_flags |= ImGuiButtonFlags_PressedOnClick; }
		if (flags & ImGuiSelectableFlags_SelectOnRelease) { button_flags |= ImGuiButtonFlags_PressedOnRelease; }
		if (flags & ImGuiSelectableFlags_AllowDoubleClick) { button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick; }
		if (flags & ImGuiSelectableFlags_AllowItemOverlap) { button_flags |= ImGuiButtonFlags_AllowItemOverlap; }

		const bool was_selected = selected;
		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, button_flags);

		const GuiStyleState* curState = nullptr;
		if (hovered) curState = &hoveredState;
		if (held) curState = &pressedState;
		if (was_selected) curState = &selectedState;

		// Auto-select when moved into
		// - This will be more fully fleshed in the range-select branch
		// - This is not exposed as it won't nicely work with some user side handling of shift/control
		// - We cannot do 'if (g.NavJustMovedToId != id) { selected = false; pressed = was_selected; }' for two reasons
		//   - (1) it would require focus scope to be set, need exposing PushFocusScope() or equivalent (e.g. BeginSelection() calling PushFocusScope())
		//   - (2) usage will fail with clipped items
		//   The multi-select API aim to fix those issues, e.g. may be replaced with a BeginSelection() API.
		if ((flags & ImGuiSelectableFlags_SelectOnNav) && g.NavJustMovedToId != 0 && g.NavJustMovedToFocusScopeId == g.CurrentFocusScopeId)
			if (g.NavJustMovedToId == id)
				selected = pressed = true;

		// Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
		if (pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
		{
			if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
			{
				ImGui::SetNavID(id, window->DC.NavLayerCurrent, g.CurrentFocusScopeId, ImGui::WindowRectAbsToRel(window, bb)); // (bb == NavRect)
				g.NavDisableHighlight = true;
			}
		}
		if (pressed)
			ImGui::MarkItemEdited(id);

		if (flags & ImGuiSelectableFlags_AllowItemOverlap)
			ImGui::SetItemAllowOverlap();

		// In this branch, Selectable() cannot toggle the selection so this will never trigger.
		if (selected != was_selected) //-V547
			g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

		// Render
		if (curState != nullptr)
		{
			GUI::RenderFrame(globalRect, curState->background, curState->borderThickness, curState->borderRadius);
		}
		ImGui::RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);

		if (span_all_columns && window->DC.CurrentColumns)
			ImGui::PopColumnsBackground();
		else if (span_all_columns && g.CurrentTable)
			ImGui::TablePopBackgroundChannel();

		// Automatically close popups
		if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiSelectableFlags_DontClosePopups) && !(g.LastItemData.InFlags & ImGuiItemFlags_SelectableDontClosePopup))
			ImGui::CloseCurrentPopup();

		if (disabled_item && !disabled_global)
			ImGui::EndDisabled();

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
		return pressed; //-V1020
	}

	COREGUI_API void InvisibleButton(const String& id, const Vec2& size)
	{
		ImGui::InvisibleButton(id.GetCString(), ImVec2(size.x, size.y));
	}

	COREGUI_API bool InvisibleButton(const Rect& localRect, ID id, GUI::ButtonFlags flags)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		auto rect = ToWindowCoordinateSpace(localRect);

		ImGui::SetCursorPos(ImVec2(rect.left, rect.top));

		ImVec2 size = ImVec2(rect.right - rect.left, rect.bottom - rect.top);
		const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
		ImGui::ItemSize(size);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, (int)flags);

		IMGUI_TEST_ENGINE_ITEM_INFO(id, "", g.LastItemData.StatusFlags);
		return pressed;
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

	static ImVec2 InputTextCalcTextSizeW(ImGuiContext* ctx, const ImWchar* text_begin, const ImWchar* text_end, const ImWchar** remaining = NULL, ImVec2* out_offset = NULL, bool stop_on_new_line = false);
	static int    InputTextCalcTextLenAndLineCount(const char* text_begin, const char** out_text_end);

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
}

#pragma region ImStb_TextEdit

// Wrapper for stb_textedit.h to edit text (our wrapper is for: statically sized buffer, single-line, wchar characters. InputText converts between UTF-8 and wchar)

#include "imstb_textedit.h"


static int     STB_TEXTEDIT_STRINGLEN(const ImGuiInputTextState* obj) { return obj->CurLenW; }
static ImWchar STB_TEXTEDIT_GETCHAR(const ImGuiInputTextState* obj, int idx) { return obj->TextW[idx]; }
static float   STB_TEXTEDIT_GETWIDTH(ImGuiInputTextState* obj, int line_start_idx, int char_idx) { ImWchar c = obj->TextW[line_start_idx + char_idx]; if (c == '\n') return STB_TEXTEDIT_GETWIDTH_NEWLINE; ImGuiContext& g = *obj->Ctx; return g.Font->GetCharAdvance(c) * (g.FontSize / g.Font->FontSize); }
static int     STB_TEXTEDIT_KEYTOTEXT(int key) { return key >= 0x200000 ? 0 : key; }
static ImWchar STB_TEXTEDIT_NEWLINE = '\n';
static void    STB_TEXTEDIT_LAYOUTROW(ImStb::StbTexteditRow* r, ImGuiInputTextState* obj, int line_start_idx)
{
	const ImWchar* text = obj->TextW.Data;
	const ImWchar* text_remaining = NULL;
	const ImVec2 size = ::CE::GUI::InputTextCalcTextSizeW(obj->Ctx, text + line_start_idx, text + obj->CurLenW, &text_remaining, NULL, true);
	r->x0 = 0.0f;
	r->x1 = size.x;
	r->baseline_y_delta = size.y;
	r->ymin = 0.0f;
	r->ymax = size.y;
	r->num_chars = (int)(text_remaining - (text + line_start_idx));
}

static bool is_separator(unsigned int c)
{
	return c == ',' || c == ';' || c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == '|' || c == '\n' || c == '\r' || c == '.' || c == '!';
}

static int is_word_boundary_from_right(ImGuiInputTextState* obj, int idx)
{
	// When ImGuiInputTextFlags_Password is set, we don't want actions such as CTRL+Arrow to leak the fact that underlying data are blanks or separators.
	if ((obj->Flags & ImGuiInputTextFlags_Password) || idx <= 0)
		return 0;

	bool prev_white = ImCharIsBlankW(obj->TextW[idx - 1]);
	bool prev_separ = is_separator(obj->TextW[idx - 1]);
	bool curr_white = ImCharIsBlankW(obj->TextW[idx]);
	bool curr_separ = is_separator(obj->TextW[idx]);
	return ((prev_white || prev_separ) && !(curr_separ || curr_white)) || (curr_separ && !prev_separ);
}
static int is_word_boundary_from_left(ImGuiInputTextState* obj, int idx)
{
	if ((obj->Flags & ImGuiInputTextFlags_Password) || idx <= 0)
		return 0;

	bool prev_white = ImCharIsBlankW(obj->TextW[idx]);
	bool prev_separ = is_separator(obj->TextW[idx]);
	bool curr_white = ImCharIsBlankW(obj->TextW[idx - 1]);
	bool curr_separ = is_separator(obj->TextW[idx - 1]);
	return ((prev_white) && !(curr_separ || curr_white)) || (curr_separ && !prev_separ);
}
static int  STB_TEXTEDIT_MOVEWORDLEFT_IMPL(ImGuiInputTextState* obj, int idx) { idx--; while (idx >= 0 && !is_word_boundary_from_right(obj, idx)) idx--; return idx < 0 ? 0 : idx; }
static int  STB_TEXTEDIT_MOVEWORDRIGHT_MAC(ImGuiInputTextState* obj, int idx) { idx++; int len = obj->CurLenW; while (idx < len && !is_word_boundary_from_left(obj, idx)) idx++; return idx > len ? len : idx; }
static int  STB_TEXTEDIT_MOVEWORDRIGHT_WIN(ImGuiInputTextState* obj, int idx) { idx++; int len = obj->CurLenW; while (idx < len && !is_word_boundary_from_right(obj, idx)) idx++; return idx > len ? len : idx; }
static int  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL(ImGuiInputTextState* obj, int idx) { ImGuiContext& g = *obj->Ctx; if (g.IO.ConfigMacOSXBehaviors) return STB_TEXTEDIT_MOVEWORDRIGHT_MAC(obj, idx); else return STB_TEXTEDIT_MOVEWORDRIGHT_WIN(obj, idx); }
#define STB_TEXTEDIT_MOVEWORDLEFT   STB_TEXTEDIT_MOVEWORDLEFT_IMPL  // They need to be #define for stb_textedit.h
#define STB_TEXTEDIT_MOVEWORDRIGHT  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL

#define STB_TEXT_HAS_SELECTION(s)   ((s)->select_start != (s)->select_end)

static void STB_TEXTEDIT_DELETECHARS(ImGuiInputTextState* obj, int pos, int n)
{
	ImWchar* dst = obj->TextW.Data + pos;

	// We maintain our buffer length in both UTF-8 and wchar formats
	obj->Edited = true;
	obj->CurLenA -= ImTextCountUtf8BytesFromStr(dst, dst + n);
	obj->CurLenW -= n;

	// Offset remaining text (FIXME-OPT: Use memmove)
	const ImWchar* src = obj->TextW.Data + pos + n;
	while (ImWchar c = *src++)
		*dst++ = c;
	*dst = '\0';
}

static bool STB_TEXTEDIT_INSERTCHARS(ImGuiInputTextState* obj, int pos, const ImWchar* new_text, int new_text_len)
{
	const bool is_resizable = (obj->Flags & ImGuiInputTextFlags_CallbackResize) != 0;
	const int text_len = obj->CurLenW;
	IM_ASSERT(pos <= text_len);

	const int new_text_len_utf8 = ImTextCountUtf8BytesFromStr(new_text, new_text + new_text_len);
	if (!is_resizable && (new_text_len_utf8 + obj->CurLenA + 1 > obj->BufCapacityA))
		return false;

	// Grow internal buffer if needed
	if (new_text_len + text_len + 1 > obj->TextW.Size)
	{
		if (!is_resizable)
			return false;
		IM_ASSERT(text_len < obj->TextW.Size);
		obj->TextW.resize(text_len + ImClamp(new_text_len * 4, 32, ImMax(256, new_text_len)) + 1);
	}

	ImWchar* text = obj->TextW.Data;
	if (pos != text_len)
		memmove(text + pos + new_text_len, text + pos, (size_t)(text_len - pos) * sizeof(ImWchar));
	memcpy(text + pos, new_text, (size_t)new_text_len * sizeof(ImWchar));

	obj->Edited = true;
	obj->CurLenW += new_text_len;
	obj->CurLenA += new_text_len_utf8;
	obj->TextW[obj->CurLenW] = '\0';

	return true;
}


	// We don't use an enum so we can build even with conflicting symbols (if another user of stb_textedit.h leak their STB_TEXTEDIT_K_* symbols)
#define STB_TEXTEDIT_K_LEFT         0x200000 // keyboard input to move cursor left
#define STB_TEXTEDIT_K_RIGHT        0x200001 // keyboard input to move cursor right
#define STB_TEXTEDIT_K_UP           0x200002 // keyboard input to move cursor up
#define STB_TEXTEDIT_K_DOWN         0x200003 // keyboard input to move cursor down
#define STB_TEXTEDIT_K_LINESTART    0x200004 // keyboard input to move cursor to start of line
#define STB_TEXTEDIT_K_LINEEND      0x200005 // keyboard input to move cursor to end of line
#define STB_TEXTEDIT_K_TEXTSTART    0x200006 // keyboard input to move cursor to start of text
#define STB_TEXTEDIT_K_TEXTEND      0x200007 // keyboard input to move cursor to end of text
#define STB_TEXTEDIT_K_DELETE       0x200008 // keyboard input to delete selection or character under cursor
#define STB_TEXTEDIT_K_BACKSPACE    0x200009 // keyboard input to delete selection or character left of cursor
#define STB_TEXTEDIT_K_UNDO         0x20000A // keyboard input to perform undo
#define STB_TEXTEDIT_K_REDO         0x20000B // keyboard input to perform redo
#define STB_TEXTEDIT_K_WORDLEFT     0x20000C // keyboard input to move cursor left one word
#define STB_TEXTEDIT_K_WORDRIGHT    0x20000D // keyboard input to move cursor right one word
#define STB_TEXTEDIT_K_PGUP         0x20000E // keyboard input to move cursor up a page
#define STB_TEXTEDIT_K_PGDOWN       0x20000F // keyboard input to move cursor down a page
#define STB_TEXTEDIT_K_SHIFT        0x400000

using namespace ImStb;

#define STB_TEXTEDIT_IMPLEMENTATION
#include "imstb_textedit.h"

// stb_textedit internally allows for a single undo record to do addition and deletion, but somehow, calling
// the stb_textedit_paste() function creates two separate records, so we perform it manually. (FIXME: Report to nothings/stb?)
static void stb_textedit_replace(ImGuiInputTextState* str, STB_TexteditState* state, const STB_TEXTEDIT_CHARTYPE* text, int text_len)
{
	stb_text_makeundo_replace(str, state, 0, str->CurLenW, text_len);
	STB_TEXTEDIT_DELETECHARS(str, 0, str->CurLenW);
	state->cursor = state->select_start = state->select_end = 0;
	if (text_len <= 0)
		return;
	if (STB_TEXTEDIT_INSERTCHARS(str, 0, text, text_len))
	{
		state->cursor = state->select_start = state->select_end = text_len;
		state->has_preferred_x = 0;
		return;
	}
	IM_ASSERT(0); // Failed to insert character, normally shouldn't happen because of how we currently use stb_textedit_replace()
}

static void InputTextReconcileUndoStateAfterUserCallback(ImGuiInputTextState* state, const char* new_buf_a, int new_length_a)
{
	ImGuiContext& g = *GImGui;
	const ImWchar* old_buf = state->TextW.Data;
	const int old_length = state->CurLenW;
	const int new_length = ImTextCountCharsFromUtf8(new_buf_a, new_buf_a + new_length_a);
	g.TempBuffer.reserve_discard((new_length + 1) * sizeof(ImWchar));
	ImWchar* new_buf = (ImWchar*)(void*)g.TempBuffer.Data;
	ImTextStrFromUtf8(new_buf, new_length + 1, new_buf_a, new_buf_a + new_length_a);

	const int shorter_length = ImMin(old_length, new_length);
	int first_diff;
	for (first_diff = 0; first_diff < shorter_length; first_diff++)
		if (old_buf[first_diff] != new_buf[first_diff])
			break;
	if (first_diff == old_length && first_diff == new_length)
		return;

	int old_last_diff = old_length - 1;
	int new_last_diff = new_length - 1;
	for (; old_last_diff >= first_diff && new_last_diff >= first_diff; old_last_diff--, new_last_diff--)
		if (old_buf[old_last_diff] != new_buf[new_last_diff])
			break;

	const int insert_len = new_last_diff - first_diff + 1;
	const int delete_len = old_last_diff - first_diff + 1;
	if (insert_len > 0 || delete_len > 0)
		if (STB_TEXTEDIT_CHARTYPE* p = stb_text_createundo(&state->Stb.undostate, first_diff, delete_len, insert_len))
			for (int i = 0; i < delete_len; i++)
				p[i] = STB_TEXTEDIT_GETCHAR(state, first_diff + i);
}

#pragma endregion

namespace CE::GUI
{
	using namespace ImStb;

	static bool InputTextFilterCharacter(unsigned int* p_char, ImGuiInputTextFlags flags, TextInputCallback callback, void* user_data, ImGuiInputSource input_source,
		int cursor = 0)
	{
		IM_ASSERT(input_source == ImGuiInputSource_Keyboard || input_source == ImGuiInputSource_Clipboard);
		unsigned int c = *p_char;

		// Filter non-printable (NB: isprint is unreliable! see #2467)
		bool apply_named_filters = true;
		if (c < 0x20)
		{
			bool pass = false;
			pass |= (c == '\n' && (flags & ImGuiInputTextFlags_Multiline)); // Note that an Enter KEY will emit \r and be ignored (we poll for KEY in InputText() code)
			pass |= (c == '\t' && (flags & ImGuiInputTextFlags_AllowTabInput));
			if (!pass)
				return false;
			apply_named_filters = false; // Override named filters below so newline and tabs can still be inserted.
		}

		if (input_source != ImGuiInputSource_Clipboard)
		{
			// We ignore Ascii representation of delete (emitted from Backspace on OSX, see #2578, #2817)
			if (c == 127)
				return false;

			// Filter private Unicode range. GLFW on OSX seems to send private characters for special keys like arrow keys (FIXME)
			if (c >= 0xE000 && c <= 0xF8FF)
				return false;
		}

		// Filter Unicode ranges we are not handling in this build
		if (c > IM_UNICODE_CODEPOINT_MAX)
			return false;

		// Generic named filters
		if (apply_named_filters && (flags & (ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsScientific)))
		{
			// The libc allows overriding locale, with e.g. 'setlocale(LC_NUMERIC, "de_DE.UTF-8");' which affect the output/input of printf/scanf to use e.g. ',' instead of '.'.
			// The standard mandate that programs starts in the "C" locale where the decimal point is '.'.
			// We don't really intend to provide widespread support for it, but out of empathy for people stuck with using odd API, we support the bare minimum aka overriding the decimal point.
			// Change the default decimal_point with:
			//   ImGui::GetCurrentContext()->PlatformLocaleDecimalPoint = *localeconv()->decimal_point;
			// Users of non-default decimal point (in particular ',') may be affected by word-selection logic (is_word_boundary_from_right/is_word_boundary_from_left) functions.
			ImGuiContext& g = *GImGui;
			const unsigned c_decimal_point = (unsigned int)g.PlatformLocaleDecimalPoint;

			// Full-width -> half-width conversion for numeric fields (https://en.wikipedia.org/wiki/Halfwidth_and_Fullwidth_Forms_(Unicode_block)
			// While this is mostly convenient, this has the side-effect for uninformed users accidentally inputting full-width characters that they may
			// scratch their head as to why it works in numerical fields vs in generic text fields it would require support in the font.
			if (flags & (ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsScientific | ImGuiInputTextFlags_CharsHexadecimal))
				if (c >= 0xFF01 && c <= 0xFF5E)
					c = c - 0xFF01 + 0x21;

			// Allow 0-9 . - + * /
			if (flags & ImGuiInputTextFlags_CharsDecimal)
				if (!(c >= '0' && c <= '9') && (c != c_decimal_point) && (c != '-') && (c != '+') && (c != '*') && (c != '/'))
					return false;

			// Allow 0-9 . - + * / e E
			if (flags & ImGuiInputTextFlags_CharsScientific)
				if (!(c >= '0' && c <= '9') && (c != c_decimal_point) && (c != '-') && (c != '+') && (c != '*') && (c != '/') && (c != 'e') && (c != 'E'))
					return false;

			// Allow 0-9 a-F A-F
			if (flags & ImGuiInputTextFlags_CharsHexadecimal)
				if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f') && !(c >= 'A' && c <= 'F'))
					return false;

			// Turn a-z into A-Z
			if (flags & ImGuiInputTextFlags_CharsUppercase)
				if (c >= 'a' && c <= 'z')
					c += (unsigned int)('A' - 'a');

			if (flags & ImGuiInputTextFlags_CharsNoBlank)
				if (ImCharIsBlankW(c))
					return false;

			*p_char = c;
		}

		// Custom callback filter
		if (flags & ImGuiInputTextFlags_CallbackCharFilter && callback != nullptr)
		{
			ImGuiContext& g = *GImGui;
			TextInputCallbackData callback_data;
			callback_data.ctx = &g;
			callback_data.eventFlag = (TextInputFlags)ImGuiInputTextFlags_CallbackCharFilter;
			callback_data.eventChar = (ImWchar)c;
			callback_data.flags = (TextInputFlags)flags;
			callback_data.userData = user_data;
			callback_data.cursorPos = cursor;
			if (callback(&callback_data) != 0)
				return false;
			*p_char = callback_data.eventChar;
			if (!callback_data.eventChar)
				return false;
		}

		return true;
	}

	COREGUI_API bool InputText(const Rect& localRect, ID id, const String& hint, String& inputText, 
		const GuiStyleState& normalState, const GuiStyleState& activeState, const GuiStyleState& disabledState, 
		const Vec4& padding, TextInputFlags flags, TextInputCallback callback, void* callback_user_data)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		GuiStyleState& modNormalState = const_cast<GuiStyleState&>(normalState);
		modNormalState.textAlign = GUI::TextAlign_MiddleLeft;

		IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackHistory) && (flags & ImGuiInputTextFlags_Multiline)));        // Can't use both together (they both use up/down keys)
		IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackCompletion) && (flags & ImGuiInputTextFlags_AllowTabInput))); // Can't use both together (they both use tab key)

		ImGuiContext& g = *GImGui;
		ImGuiIO& io = g.IO;
		const ImGuiStyle& style = g.Style;

		Rect rect = ToWindowCoordinateSpace(localRect);

		flags |= TextInputFlags_CallbackResize;

		const bool RENDER_SELECTION_WHEN_INACTIVE = false;
		const bool is_multiline = (flags & ImGuiInputTextFlags_Multiline) != 0;
		const bool is_readonly = (flags & ImGuiInputTextFlags_ReadOnly) != 0;
		const bool is_password = (flags & ImGuiInputTextFlags_Password) != 0;
		const bool is_undoable = (flags & ImGuiInputTextFlags_NoUndoRedo) == 0;
		const bool is_resizable = true;//(flags & ImGuiInputTextFlags_CallbackResize) != 0;

		ImVec2 align{};
		if (!is_multiline)
		{
			if (normalState.textAlign == TextAlign_TopCenter)
			{
				align = ImVec2(0.5f, 0.0f);
			}
			else if (normalState.textAlign == TextAlign_TopRight)
			{
				align = ImVec2(1, 0.0f);
			}
			else if (normalState.textAlign == TextAlign_MiddleLeft)
			{
				align = ImVec2(0, 0.5f);
			}
			else if (normalState.textAlign == TextAlign_MiddleCenter)
			{
				align = ImVec2(0.5f, 0.5f);
			}
			else if (normalState.textAlign == TextAlign_MiddleRight)
			{
				align = ImVec2(1, 0.5f);
			}
		}

		ImGui::SetCursorPos(ImVec2(rect.min.x, rect.min.y));

		if (is_multiline) // Open group before calling GetID() because groups tracks id created within their scope (including the scrollbar)
			ImGui::BeginGroup();

		Vec2 size = Vec2(rect.max.x - rect.min.x, rect.max.y - rect.min.y);

		const ImVec2 label_size = ImGui::CalcTextSize("ignore", NULL, true);
		ImVec2 size_arg = ImVec2(size.x, size.y);

		const ImVec2 frame_size = size_arg;

		//const ImVec2 frame_size = ImGui::CalcItemSize(size_arg, ImGui::CalcItemWidth(),
		//	(is_multiline ? g.FontSize * 8.0f : label_size.y) + padding.top + padding.bottom); // Arbitrary default of 8 lines high for multi-line
		//const ImVec2 total_size = frame_size;

		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);
		//const ImRect total_bb(frame_bb.Min, frame_bb.Min + total_size);
		const ImRect frame_bb_inner(frame_bb.Min + ImVec2(padding.left, padding.top), frame_bb.Max - ImVec2(padding.right, padding.bottom));
		const ImVec2 frame_inner_size(frame_bb_inner.Max - frame_bb_inner.Min);
		
		ImGuiWindow* draw_window = window;
		ImVec2 inner_size = frame_size;
		ImGuiItemStatusFlags item_status_flags = 0;
		ImGuiLastItemData item_data_backup;

		if (is_multiline) // Multiline
		{
			ImVec2 backup_pos = window->DC.CursorPos;
			ImGui::ItemSize(frame_bb, padding.top);
			if (!ImGui::ItemAdd(frame_bb, id, &frame_bb, ImGuiItemFlags_Inputable))
			{
				ImGui::EndGroup();
				return false;
			}
			item_status_flags = g.LastItemData.StatusFlags;
			item_data_backup = g.LastItemData;
			window->DC.CursorPos = backup_pos;

			// We reproduce the contents of BeginChildFrame() in order to provide 'label' so our window internal data are easier to read/debug.
			// FIXME-NAV: Pressing NavActivate will trigger general child activation right before triggering our own below. Harmless but bizarre.
			ImGui::PushStyleColor(ImGuiCol_ChildBg, normalState.background.ToU32());
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, ImVec2(normalState.borderRadius.x, normalState.borderRadius.y));
			ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, normalState.borderThickness);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // Ensure no clip rect so mouse hover can reach FramePadding edges
			bool child_visible = ImGui::BeginChildEx(nullptr, id, frame_bb.GetSize(), true, ImGuiWindowFlags_NoMove);
			ImGui::PopStyleVar(3);
			ImGui::PopStyleColor();

			if (!child_visible)
			{
				ImGui::EndChild();
				ImGui::EndGroup();
				return false;
			}
			draw_window = g.CurrentWindow; // Child window
			draw_window->DC.NavLayersActiveMaskNext |= (1 << draw_window->DC.NavLayerCurrent); // This is to ensure that EndChild() will display a navigation highlight so we can "enter" into it.
			draw_window->DC.CursorPos += ImVec2(padding.left, padding.top);
			inner_size.x -= draw_window->ScrollbarSizes.x;
		}
		else
		{
			// Support for internal ImGuiInputTextFlags_MergedItem flag, which could be redesigned as an ItemFlags if needed (with test performed in ItemAdd)
			ImGui::ItemSize(frame_bb, padding.top);
			if (!(flags & ImGuiInputTextFlags_MergedItem))
				if (!ImGui::ItemAdd(frame_bb, id, &frame_bb, ImGuiItemFlags_Inputable))
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
		int buf_size = inputText.GetLength() + 1;

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
			const int buf_len = buf_size;

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
			ImGui::PushFont(password_font);
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
					const bool is_bol = (state->Stb.cursor == 0) || STB_TEXTEDIT_GETCHAR(state, state->Stb.cursor - 1) == '\n';
					if (STB_TEXT_HAS_SELECTION(&state->Stb) || !is_bol)
						state->OnKeyPressed(STB_TEXTEDIT_K_WORDLEFT);
					//state->OnKeyPressed(STB_TEXTEDIT_K_WORDRIGHT | STB_TEXTEDIT_K_SHIFT);
					if (!STB_TEXT_HAS_SELECTION(&state->Stb))
						stb_textedit_prep_selection_at_cursor(&state->Stb);
					state->Stb.cursor = STB_TEXTEDIT_MOVEWORDRIGHT_MAC(state, state->Stb.cursor);
					state->Stb.select_end = state->Stb.cursor;
					stb_textedit_clamp(state, &state->Stb);
				}
				else
				{
					// Triple-click: Select line
					const bool is_eol = STB_TEXTEDIT_GETCHAR(state, state->Stb.cursor) == '\n';
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
			if ((flags & ImGuiInputTextFlags_AllowTabInput) && ImGui::Shortcut(ImGuiKey_Tab, id) && !is_readonly)
			{
				unsigned int c = '\t'; // Insert TAB
				if (InputTextFilterCharacter(&c, flags, callback, callback_user_data, ImGuiInputSource_Keyboard, state->Stb.cursor))
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
						if (InputTextFilterCharacter(&c, flags, callback, callback_user_data, ImGuiInputSource_Keyboard, state->Stb.cursor))
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
			const bool is_cut = (ImGui::Shortcut(ImGuiMod_Shortcut | ImGuiKey_X, id, f_repeat) ||
				ImGui::Shortcut(ImGuiMod_Shift | ImGuiKey_Delete, id, f_repeat)) && !is_readonly && !is_password && (!is_multiline || state->HasSelection());
			const bool is_copy = (ImGui::Shortcut(ImGuiMod_Shortcut | ImGuiKey_C, id) ||
				ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Insert, id)) && !is_password && (!is_multiline || state->HasSelection());
			const bool is_paste = (ImGui::Shortcut(ImGuiMod_Shortcut | ImGuiKey_V, id, f_repeat) ||
				ImGui::Shortcut(ImGuiMod_Shift | ImGuiKey_Insert, id, f_repeat)) && !is_readonly;
			const bool is_undo = (ImGui::Shortcut(ImGuiMod_Shortcut | ImGuiKey_Z, id, f_repeat)) && !is_readonly && is_undoable;
			const bool is_redo = (ImGui::Shortcut(ImGuiMod_Shortcut | ImGuiKey_Y, id, f_repeat) || (is_osx && ImGui::Shortcut(ImGuiMod_Shortcut | ImGuiMod_Shift | ImGuiKey_Z, id, f_repeat))) && !is_readonly && is_undoable;
			const bool is_select_all = ImGui::Shortcut(ImGuiMod_Shortcut | ImGuiKey_A, id);

			// We allow validate/cancel with Nav source (gamepad) to makes it easier to undo an accidental NavInput press with no keyboard wired, but otherwise it isn't very useful.
			const bool nav_gamepad_active = (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) != 0 && (io.BackendFlags & ImGuiBackendFlags_HasGamepad) != 0;
			const bool is_enter_pressed = ImGui::IsKeyPressed(ImGuiKey_Enter, true) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, true);
			const bool is_gamepad_validate = nav_gamepad_active && (ImGui::IsKeyPressed(ImGuiKey_NavGamepadActivate, false) || ImGui::IsKeyPressed(ImGuiKey_NavGamepadInput, false));
			const bool is_cancel = ImGui::Shortcut(ImGuiKey_Escape, id, f_repeat) || (nav_gamepad_active && ImGui::Shortcut(ImGuiKey_NavGamepadCancel, id, f_repeat));

			// FIXME: Should use more Shortcut() and reduce IsKeyPressed()+SetKeyOwner(), but requires modifiers combination to be taken account of.
			if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) { state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_LINESTART : is_wordmove_key_down ? STB_TEXTEDIT_K_WORDLEFT : STB_TEXTEDIT_K_LEFT) | k_mask); }
			else if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) { state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_LINEEND : is_wordmove_key_down ? STB_TEXTEDIT_K_WORDRIGHT : STB_TEXTEDIT_K_RIGHT) | k_mask); }
			else if (ImGui::IsKeyPressed(ImGuiKey_UpArrow) && is_multiline) { if (io.KeyCtrl) ImGui::SetScrollY(draw_window, ImMax(draw_window->Scroll.y - g.FontSize, 0.0f)); else state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTSTART : STB_TEXTEDIT_K_UP) | k_mask); }
			else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow) && is_multiline) { if (io.KeyCtrl) ImGui::SetScrollY(draw_window, ImMin(draw_window->Scroll.y + g.FontSize, ImGui::GetScrollMaxY())); else state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTEND : STB_TEXTEDIT_K_DOWN) | k_mask); }
			else if (ImGui::IsKeyPressed(ImGuiKey_PageUp) && is_multiline) { state->OnKeyPressed(STB_TEXTEDIT_K_PGUP | k_mask); scroll_y -= row_count_per_page * g.FontSize; }
			else if (ImGui::IsKeyPressed(ImGuiKey_PageDown) && is_multiline) { state->OnKeyPressed(STB_TEXTEDIT_K_PGDOWN | k_mask); scroll_y += row_count_per_page * g.FontSize; }
			else if (ImGui::IsKeyPressed(ImGuiKey_Home)) { state->OnKeyPressed(io.KeyCtrl ? STB_TEXTEDIT_K_TEXTSTART | k_mask : STB_TEXTEDIT_K_LINESTART | k_mask); }
			else if (ImGui::IsKeyPressed(ImGuiKey_End)) { state->OnKeyPressed(io.KeyCtrl ? STB_TEXTEDIT_K_TEXTEND | k_mask : STB_TEXTEDIT_K_LINEEND | k_mask); }
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
					if (InputTextFilterCharacter(&c, flags, callback, callback_user_data, ImGuiInputSource_Keyboard, state->Stb.cursor))
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
					ImGui::SetClipboardText(clipboard_data);
					ImGui::MemFree(clipboard_data);
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
				if (const char* clipboard = ImGui::GetClipboardText())
				{
					// Filter pasted buffer
					const int clipboard_len = (int)strlen(clipboard);
					ImWchar* clipboard_filtered = (ImWchar*)IM_ALLOC((clipboard_len + 1) * sizeof(ImWchar));
					int clipboard_filtered_len = 0;
					for (const char* s = clipboard; *s != 0; )
					{
						unsigned int c;
						s += ImTextCharFromUtf8(&c, s, NULL);
						if (!InputTextFilterCharacter(&c, flags, callback, callback_user_data, ImGuiInputSource_Clipboard, state->Stb.cursor))
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

				if (state->Edited)
				{
					inputText = state->TextA.Data;
				}

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

					if (event_flag && callback != nullptr)
					{
						TextInputCallbackData callback_data;
						callback_data.ctx = &g;
						callback_data.eventFlag = (TextInputFlags)event_flag;
						callback_data.flags = flags;
						callback_data.userData = callback_user_data;

						char* callback_buf = is_readonly ? buf : state->TextA.Data;
						callback_data.eventKey = event_key;
						callback_data.buf = callback_buf;
						callback_data.bufTextLen = state->CurLenA;
						callback_data.bufSize = state->BufCapacityA;
						callback_data.bufDirty = false;

						// We have to convert from wchar-positions to UTF-8-positions, which can be pretty slow (an incentive to ditch the ImWchar buffer, see https://github.com/nothings/stb/issues/188)
						ImWchar* text = state->TextW.Data;
						const int utf8_cursor_pos = callback_data.cursorPos = ImTextCountUtf8BytesFromStr(text, text + state->Stb.cursor);
						const int utf8_selection_start = callback_data.selectionStart = ImTextCountUtf8BytesFromStr(text, text + state->Stb.select_start);
						const int utf8_selection_end = callback_data.selectionEnd = ImTextCountUtf8BytesFromStr(text, text + state->Stb.select_end);

						// Call user code
						callback(&callback_data);

						// Read back what user may have modified
						callback_buf = is_readonly ? buf : state->TextA.Data; // Pointer may have been invalidated by a resize callback
						IM_ASSERT(callback_data.buf == callback_buf);         // Invalid to modify those fields
						IM_ASSERT(callback_data.bufSize == state->BufCapacityA);
						IM_ASSERT(callback_data.flags == flags);
						const bool buf_dirty = callback_data.bufDirty;
						if (callback_data.cursorPos != utf8_cursor_pos || buf_dirty) { state->Stb.cursor = ImTextCountCharsFromUtf8(callback_data.buf, callback_data.buf + callback_data.cursorPos); state->CursorFollow = true; }
						if (callback_data.selectionStart != utf8_selection_start || buf_dirty) { state->Stb.select_start = (callback_data.selectionStart == callback_data.cursorPos) ? state->Stb.cursor : ImTextCountCharsFromUtf8(callback_data.buf, callback_data.buf + callback_data.selectionStart); }
						if (callback_data.selectionEnd != utf8_selection_end || buf_dirty) { state->Stb.select_end = (callback_data.selectionEnd == callback_data.selectionStart) ? state->Stb.select_start : ImTextCountCharsFromUtf8(callback_data.buf, callback_data.buf + callback_data.selectionEnd); }
						if (buf_dirty)
						{
							IM_ASSERT((flags & ImGuiInputTextFlags_ReadOnly) == 0);
							IM_ASSERT(callback_data.bufTextLen == (int)strlen(callback_data.buf)); // You need to maintain BufTextLen if you change the text!
							InputTextReconcileUndoStateAfterUserCallback(state, callback_data.buf, callback_data.bufTextLen); // FIXME: Move the rest of this block inside function and rename to InputTextReconcileStateAfterUserCallback() ?
							if (callback_data.bufTextLen > backup_current_text_length && is_resizable)
								state->TextW.resize(state->TextW.Size + (callback_data.bufTextLen - backup_current_text_length)); // Worse case scenario resize
							state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, state->TextW.Size, callback_data.buf, NULL);
							state->CurLenA = callback_data.bufTextLen;  // Assume correct length and valid UTF-8 from user, saves us an extra strlen()
							state->CursorAnimReset();
						}
					}
				}

				// Will copy result string if modified
				if (!is_readonly && (state->Edited || strcmp(state->TextA.Data, buf) != 0))
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
			if (g.ActiveId != id && ImGui::IsItemDeactivatedAfterEdit() && !is_readonly)
			{
				apply_new_text = g.InputTextDeactivatedState.TextA.Data;
				apply_new_text_length = g.InputTextDeactivatedState.TextA.Size - 1;
				value_changed |= (strcmp(g.InputTextDeactivatedState.TextA.Data, buf) != 0);
				//IMGUI_DEBUG_LOG("InputText(): apply Deactivated data for 0x%08X: \"%.*s\".\n", id, apply_new_text_length, apply_new_text);
			}
			g.InputTextDeactivatedState.ID = 0;
		}

		// Release active ID at the end of the function (so e.g. pressing Return still does a final application of the value)
		// Otherwise request text input ahead for next frame.
		if (g.ActiveId == id && clear_active_id)
			ImGui::ClearActiveID();
		else if (g.ActiveId == id)
			g.WantTextInputNextFrame = 1;

		// Render frame
		if (!is_multiline)
		{
			//ImGui::RenderNavHighlight(frame_bb, id);
			//ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
			Vec4 frameRect{ frame_bb.Min.x, frame_bb.Min.y, frame_bb.Max.x, frame_bb.Max.y };
			GUI::RenderNavHighlight(frameRect, id, normalState.borderThickness, normalState.borderRadius);
			GUI::RenderFrame(frameRect, normalState.background, normalState.borderColor, normalState.borderThickness, normalState.borderRadius);
		}

		const ImVec4 clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + inner_size.x, frame_bb.Min.y + inner_size.y); // Not using frame_bb.Max because we have adjusted size
		ImVec2 draw_pos = is_multiline ? draw_window->DC.CursorPos : frame_bb.Min + ImVec2(padding.left, padding.top);

		// Set upper limit of single-line InputTextEx() at 2 million characters strings. The current pathological worst case is a long line
		// without any carriage return, which would makes ImFont::RenderText() reserve too many vertices and probably crash. Avoid it altogether.
		// Note that we only use this limit on single-line InputText(), so a pathologically large line on a InputTextMultiline() would still crash.
		const int buf_display_max_length = 2 * 1024 * 1024;
		const char* buf_display = buf_display_from_state ? state->TextA.Data : buf; //-V595
		const char* buf_display_end = NULL; // We have specialized paths below for setting the length
		if (is_displaying_hint && !hint.IsEmpty())
		{
			buf_display = hint.GetCString();
			buf_display_end = hint.GetCString() + hint.GetLength();
		}

		ImVec2 text_size = ImGui::CalcTextSize(buf_display, buf_display_end);

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

				ImVec2 offset = {};
				if (!is_multiline)
				{
					offset.x = align.x * abs(clip_rect.z - clip_rect.x - (padding.left + padding.right)) - (g.FontSize - 2.0f) * align.x;
					offset.y = align.y * abs(clip_rect.w - clip_rect.y - (padding.top + padding.bottom)) - (g.FontSize - 2.0f) * align.y;
				}

				ImU32 bg_color = ImGui::GetColorU32(ImGuiCol_TextSelectedBg, render_cursor ? 1.0f : 0.6f); // FIXME: current code flow mandate that render_cursor is always true here, we are leaving the transparent one for tests.
				float bg_offy_up = is_multiline ? 0.0f : -1.0f;    // FIXME: those offsets should be part of the style? they don't play so well with multi-line selection.
				float bg_offy_dn = is_multiline ? 0.0f : 2.0f;
				ImVec2 rect_pos = draw_pos + select_start_offset - draw_scroll + offset;
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
				//ImU32 col = ImGui::GetColorU32(is_displaying_hint ? ImGuiCol_TextDisabled : ImGuiCol_Text);
				u32 col = is_displaying_hint ? disabledState.foreground.ToU32() : normalState.foreground.ToU32();
				ImVec2 offset = {};
				if (!is_multiline)
				{
					offset.x = align.x * abs(clip_rect.z - clip_rect.x - (padding.left + padding.right)) - text_size.x * align.x;
					offset.y = align.y * abs(clip_rect.w - clip_rect.y - (padding.top + padding.bottom)) - text_size.y * align.y;
				}
				draw_window->DrawList->AddText(g.Font, g.FontSize, draw_pos - draw_scroll + offset, col, buf_display, buf_display_end, 0.0f, is_multiline ? NULL : &clip_rect);
			}

			// Draw blinking cursor
			if (render_cursor)
			{
				ImVec2 offset = {};
				if (!is_multiline)
				{
					offset.x = align.x * abs(clip_rect.z - clip_rect.x - (padding.left + padding.right)) - (g.FontSize - 2.0f) * align.x;
					offset.y = align.y * abs(clip_rect.w - clip_rect.y - (padding.top + padding.bottom)) - (g.FontSize - 2.0f) * align.y;
				}
				state->CursorAnim += io.DeltaTime;
				bool cursor_is_visible = (!g.IO.ConfigInputTextCursorBlink) || (state->CursorAnim <= 0.0f) || ImFmod(state->CursorAnim, 1.20f) <= 0.80f;
				ImVec2 cursor_screen_pos = ImFloor(draw_pos + cursor_offset - draw_scroll) + offset;
				ImRect cursor_screen_rect(cursor_screen_pos.x, cursor_screen_pos.y - g.FontSize + 0.5f, cursor_screen_pos.x + 1.0f, cursor_screen_pos.y - 1.5f);
				if (cursor_is_visible && cursor_screen_rect.Overlaps(clip_rect))
					draw_window->DrawList->AddLine(cursor_screen_rect.Min, cursor_screen_rect.GetBL(), normalState.foreground.ToU32());//ImGui::GetColorU32(ImGuiCol_Text));

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
				text_size = ImVec2(inner_size.x, InputTextCalcTextLenAndLineCount(buf_display, &buf_display_end) * g.FontSize); // We don't need width
			else if (!is_displaying_hint && g.ActiveId == id)
				buf_display_end = buf_display + state->CurLenA;
			else if (!is_displaying_hint)
				buf_display_end = buf_display + strlen(buf_display);

			if (is_multiline || (buf_display_end - buf_display) < buf_display_max_length)
			{
				//ImU32 col = ImGui::GetColorU32(is_displaying_hint ? ImGuiCol_TextDisabled : ImGuiCol_Text);
				u32 col = is_displaying_hint ? disabledState.foreground.ToU32() : normalState.foreground.ToU32();
				ImVec2 offset = {};
				if (!is_multiline)
				{
					offset.x = align.x * abs(clip_rect.z - clip_rect.x - (padding.left + padding.right)) - text_size.x * align.x;
					offset.y = align.y * abs(clip_rect.w - clip_rect.y - (padding.top + padding.bottom)) - text_size.y * align.y;
				}
				draw_window->DrawList->AddText(g.Font, g.FontSize, draw_pos + offset, col, buf_display, buf_display_end, 0.0f, is_multiline ? NULL : &clip_rect);
			}
		}

		if (is_password && !is_displaying_hint)
			ImGui::PopFont();

		if (is_multiline)
		{
			// For focus requests to work on our multiline we need to ensure our child ItemAdd() call specifies the ImGuiItemFlags_Inputable (ref issue #4761)...
			ImGui::Dummy(ImVec2(text_size.x, text_size.y + style.FramePadding.y));
			ImGuiItemFlags backup_item_flags = g.CurrentItemFlags;
			g.CurrentItemFlags |= ImGuiItemFlags_Inputable | ImGuiItemFlags_NoTabStop;
			ImGui::EndChild();
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

		// Do NOT draw label
		//if (label_size.x > 0)
		//	ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label.GetCString());

		if (value_changed && !(flags & ImGuiInputTextFlags_NoMarkEdited))
			ImGui::MarkItemEdited(id);

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Inputable);
		if ((flags & ImGuiInputTextFlags_EnterReturnsTrue) != 0)
			return value_changed || validated;
		else
			return value_changed;
	}

	COREGUI_API bool InputTextEx(ID id, const String& hint, String& inputText, const Vec2& baseSize, 
		const Vec4& padding, const Vec4& rounding, f32 borderThickness, Vec2 minSize, Vec2 maxSize, 
		TextInputFlags flags, TextInputCallback callback, void* callback_user_data)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackHistory) && (flags & ImGuiInputTextFlags_Multiline)));        // Can't use both together (they both use up/down keys)
		IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackCompletion) && (flags & ImGuiInputTextFlags_AllowTabInput))); // Can't use both together (they both use tab key)

		ImGuiContext& g = *GImGui;
		ImGuiIO& io = g.IO;
		const ImGuiStyle& style = g.Style;

		Vec2 size = baseSize;
		if (minSize.x > 0 && size.x < minSize.x)
			size.x = minSize.x;
		if (minSize.y > 0 && size.y < minSize.y)
			size.y = minSize.y;
		if (maxSize.x > 0 && size.x > maxSize.x)
			size.x = maxSize.x;
		if (maxSize.y > 0 && size.y > maxSize.y)
			size.y = maxSize.y;

		flags |= TextInputFlags_CallbackResize;
        
        const bool RENDER_SELECTION_WHEN_INACTIVE = false;
        const bool is_multiline = (flags & ImGuiInputTextFlags_Multiline) != 0;
        const bool is_readonly = (flags & ImGuiInputTextFlags_ReadOnly) != 0;
        const bool is_password = (flags & ImGuiInputTextFlags_Password) != 0;
        const bool is_undoable = (flags & ImGuiInputTextFlags_NoUndoRedo) == 0;
		const bool is_resizable = true;//(flags & ImGuiInputTextFlags_CallbackResize) != 0;
        //if (is_resizable)
        //    IM_ASSERT(callback != NULL); // Must provide a callback if you set the ImGuiInputTextFlags_CallbackResize flag!

        if (is_multiline) // Open group before calling GetID() because groups tracks id created within their scope (including the scrollbar)
            ImGui::BeginGroup();
        
        const ImVec2 label_size = ImGui::CalcTextSize("ignore", NULL, true);
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
            ImGui::ItemSize(total_bb, padding.top);
            if (!ImGui::ItemAdd(total_bb, id, &frame_bb, ImGuiItemFlags_Inputable))
            {
				ImGui::EndGroup();
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
            ImGui::PopStyleVar(3);
			ImGui::PopStyleColor();
			
            if (!child_visible)
            {
                ImGui::EndChild();
				ImGui::EndGroup();
                return false;
            }
            draw_window = g.CurrentWindow; // Child window
            draw_window->DC.NavLayersActiveMaskNext |= (1 << draw_window->DC.NavLayerCurrent); // This is to ensure that EndChild() will display a navigation highlight so we can "enter" into it.
            draw_window->DC.CursorPos += ImVec2(padding.left, padding.top);
            inner_size.x -= draw_window->ScrollbarSizes.x;
        }
        else
        {
            // Support for internal ImGuiInputTextFlags_MergedItem flag, which could be redesigned as an ItemFlags if needed (with test performed in ItemAdd)
            ImGui::ItemSize(total_bb, padding.top);
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
        int buf_size = inputText.GetLength() + 1;

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
			const int buf_len = buf_size;
            
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
            ImGui::PushFont(password_font);
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
                    const bool is_bol = (state->Stb.cursor == 0) || STB_TEXTEDIT_GETCHAR(state, state->Stb.cursor - 1) == '\n';
                    if (STB_TEXT_HAS_SELECTION(&state->Stb) || !is_bol)
                        state->OnKeyPressed(STB_TEXTEDIT_K_WORDLEFT);
                    //state->OnKeyPressed(STB_TEXTEDIT_K_WORDRIGHT | STB_TEXTEDIT_K_SHIFT);
                    if (!STB_TEXT_HAS_SELECTION(&state->Stb))
                        stb_textedit_prep_selection_at_cursor(&state->Stb);
                    state->Stb.cursor = STB_TEXTEDIT_MOVEWORDRIGHT_MAC(state, state->Stb.cursor);
                    state->Stb.select_end = state->Stb.cursor;
                    stb_textedit_clamp(state, &state->Stb);
                }
                else
                {
                    // Triple-click: Select line
                    const bool is_eol = STB_TEXTEDIT_GETCHAR(state, state->Stb.cursor) == '\n';
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
            if ((flags & ImGuiInputTextFlags_AllowTabInput) && ImGui::Shortcut(ImGuiKey_Tab, id) && !is_readonly)
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
            else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow) && is_multiline)   { if (io.KeyCtrl) ImGui::SetScrollY(draw_window, ImMin(draw_window->Scroll.y + g.FontSize, ImGui::GetScrollMaxY())); else state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTEND : STB_TEXTEDIT_K_DOWN) | k_mask); }
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
					ImGui::SetClipboardText(clipboard_data);
					ImGui::MemFree(clipboard_data);
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
                if (const char* clipboard = ImGui::GetClipboardText())
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

				if (state->Edited)
				{
					inputText = state->TextA.Data;
				}

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

                    if (event_flag && callback != nullptr)
                    {
                        TextInputCallbackData callback_data;
                        callback_data.ctx = &g;
                        callback_data.eventFlag = (TextInputFlags)event_flag;
                        callback_data.flags = flags;
                        callback_data.userData = callback_user_data;

                        char* callback_buf = is_readonly ? buf : state->TextA.Data;
                        callback_data.eventKey = event_key;
                        callback_data.buf = callback_buf;
                        callback_data.bufTextLen = state->CurLenA;
                        callback_data.bufSize = state->BufCapacityA;
                        callback_data.bufDirty = false;

                        // We have to convert from wchar-positions to UTF-8-positions, which can be pretty slow (an incentive to ditch the ImWchar buffer, see https://github.com/nothings/stb/issues/188)
                        ImWchar* text = state->TextW.Data;
                        const int utf8_cursor_pos = callback_data.cursorPos = ImTextCountUtf8BytesFromStr(text, text + state->Stb.cursor);
                        const int utf8_selection_start = callback_data.selectionStart = ImTextCountUtf8BytesFromStr(text, text + state->Stb.select_start);
                        const int utf8_selection_end = callback_data.selectionEnd = ImTextCountUtf8BytesFromStr(text, text + state->Stb.select_end);

                        // Call user code
                        callback(&callback_data);

                        // Read back what user may have modified
                        callback_buf = is_readonly ? buf : state->TextA.Data; // Pointer may have been invalidated by a resize callback
                        IM_ASSERT(callback_data.buf == callback_buf);         // Invalid to modify those fields
                        IM_ASSERT(callback_data.bufSize == state->BufCapacityA);
                        IM_ASSERT(callback_data.flags == flags);
                        const bool buf_dirty = callback_data.bufDirty;
                        if (callback_data.cursorPos != utf8_cursor_pos || buf_dirty)            { state->Stb.cursor = ImTextCountCharsFromUtf8(callback_data.buf, callback_data.buf + callback_data.cursorPos); state->CursorFollow = true; }
                        if (callback_data.selectionStart != utf8_selection_start || buf_dirty)  { state->Stb.select_start = (callback_data.selectionStart == callback_data.cursorPos) ? state->Stb.cursor : ImTextCountCharsFromUtf8(callback_data.buf, callback_data.buf + callback_data.selectionStart); }
                        if (callback_data.selectionEnd != utf8_selection_end || buf_dirty)      { state->Stb.select_end = (callback_data.selectionEnd == callback_data.selectionStart) ? state->Stb.select_start : ImTextCountCharsFromUtf8(callback_data.buf, callback_data.buf + callback_data.selectionEnd); }
                        if (buf_dirty)
                        {
                            IM_ASSERT((flags & ImGuiInputTextFlags_ReadOnly) == 0);
                            IM_ASSERT(callback_data.bufTextLen == (int)strlen(callback_data.buf)); // You need to maintain BufTextLen if you change the text!
                            InputTextReconcileUndoStateAfterUserCallback(state, callback_data.buf, callback_data.bufTextLen); // FIXME: Move the rest of this block inside function and rename to InputTextReconcileStateAfterUserCallback() ?
                            if (callback_data.bufTextLen > backup_current_text_length && is_resizable)
                                state->TextW.resize(state->TextW.Size + (callback_data.bufTextLen - backup_current_text_length)); // Worse case scenario resize
                            state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, state->TextW.Size, callback_data.buf, NULL);
                            state->CurLenA = callback_data.bufTextLen;  // Assume correct length and valid UTF-8 from user, saves us an extra strlen()
                            state->CursorAnimReset();
                        }
                    }
                }

                // Will copy result string if modified
				if (!is_readonly && (state->Edited || strcmp(state->TextA.Data, buf) != 0))
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
            if (g.ActiveId != id && ImGui::IsItemDeactivatedAfterEdit() && !is_readonly)
            {
                apply_new_text = g.InputTextDeactivatedState.TextA.Data;
                apply_new_text_length = g.InputTextDeactivatedState.TextA.Size - 1;
                value_changed |= (strcmp(g.InputTextDeactivatedState.TextA.Data, buf) != 0);
                //IMGUI_DEBUG_LOG("InputText(): apply Deactivated data for 0x%08X: \"%.*s\".\n", id, apply_new_text_length, apply_new_text);
            }
            g.InputTextDeactivatedState.ID = 0;
        }

        // Release active ID at the end of the function (so e.g. pressing Return still does a final application of the value)
        // Otherwise request text input ahead for next frame.
        if (g.ActiveId == id && clear_active_id)
			ImGui::ClearActiveID();
        else if (g.ActiveId == id)
            g.WantTextInputNextFrame = 1;

        // Render frame
        if (!is_multiline)
        {
			//ImGui::RenderNavHighlight(frame_bb, id);
            //ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
			Vec4 frameRect{ frame_bb.Min.x, frame_bb.Min.y, frame_bb.Max.x, frame_bb.Max.y };
			GUI::RenderNavHighlight(frameRect, id, borderThickness, rounding);
			GUI::RenderFrame(frameRect, ImGui::GetColorU32(ImGuiCol_FrameBg), borderThickness, rounding);
        }

        const ImVec4 clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + inner_size.x, frame_bb.Min.y + inner_size.y); // Not using frame_bb.Max because we have adjusted size
        ImVec2 draw_pos = is_multiline ? draw_window->DC.CursorPos : frame_bb.Min + ImVec2(padding.left, padding.top);
        ImVec2 text_size(0.0f, 0.0f);

        // Set upper limit of single-line InputTextEx() at 2 million characters strings. The current pathological worst case is a long line
        // without any carriage return, which would makes ImFont::RenderText() reserve too many vertices and probably crash. Avoid it altogether.
        // Note that we only use this limit on single-line InputText(), so a pathologically large line on a InputTextMultiline() would still crash.
        const int buf_display_max_length = 2 * 1024 * 1024;
        const char* buf_display = buf_display_from_state ? state->TextA.Data : buf; //-V595
        const char* buf_display_end = NULL; // We have specialized paths below for setting the length
        if (is_displaying_hint && !hint.IsEmpty())
        {
            buf_display = hint.GetCString();
            buf_display_end = hint.GetCString() + hint.GetLength();
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

                ImU32 bg_color = ImGui::GetColorU32(ImGuiCol_TextSelectedBg, render_cursor ? 1.0f : 0.6f); // FIXME: current code flow mandate that render_cursor is always true here, we are leaving the transparent one for tests.
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
                ImU32 col = ImGui::GetColorU32(is_displaying_hint ? ImGuiCol_TextDisabled : ImGuiCol_Text);
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
                    draw_window->DrawList->AddLine(cursor_screen_rect.Min, cursor_screen_rect.GetBL(), ImGui::GetColorU32(ImGuiCol_Text));

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
                text_size = ImVec2(inner_size.x, InputTextCalcTextLenAndLineCount(buf_display, &buf_display_end) * g.FontSize); // We don't need width
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
			ImGui::PopFont();

        if (is_multiline)
        {
            // For focus requests to work on our multiline we need to ensure our child ItemAdd() call specifies the ImGuiItemFlags_Inputable (ref issue #4761)...
			ImGui::Dummy(ImVec2(text_size.x, text_size.y + style.FramePadding.y));
            ImGuiItemFlags backup_item_flags = g.CurrentItemFlags;
            g.CurrentItemFlags |= ImGuiItemFlags_Inputable | ImGuiItemFlags_NoTabStop;
			ImGui::EndChild();
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

		// Do NOT draw label
        //if (label_size.x > 0)
		//	ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label.GetCString());

        if (value_changed && !(flags & ImGuiInputTextFlags_NoMarkEdited))
			ImGui::MarkItemEdited(id);

        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Inputable);
        if ((flags & ImGuiInputTextFlags_EnterReturnsTrue) != 0)
            return value_changed || validated;
        else
            return value_changed;
	}

	static inline TextInputFlags InputScalar_DefaultCharsFilter(ImGuiDataType data_type, const char* format)
	{
		if (data_type == ImGuiDataType_Float || data_type == ImGuiDataType_Double)
			return TextInputFlags_CharsScientific;
		const char format_last_char = format[0] ? format[strlen(format) - 1] : 0;
		return (format_last_char == 'x' || format_last_char == 'X') ? TextInputFlags_CharsHexadecimal : TextInputFlags_CharsDecimal;
	}

	COREGUI_API bool TempInputText(const Vec4& rect, ID id, String& text, TextInputFlags flags,
		const Vec2& size, const Vec4& padding, const Vec4& rounding, f32 borderThickness)
	{
		// On the first frame, g.TempInputTextId == 0, then on subsequent frames it becomes == id.
		// We clear ActiveID on the first frame to allow the InputText() taking it back.
		ImGuiContext& g = *GImGui;
		const bool init = (g.TempInputId != id);
		if (init)
			ImGui::ClearActiveID();

		g.CurrentWindow->DC.CursorPos = ImVec2(rect.min.x, rect.min.y);
		//Vec2 size = rect.max - rect.min;

		bool value_changed = InputTextEx(id, "", text, size, padding, rounding, borderThickness, {}, {}, flags | TextInputFlags_MergedItem);
		if (init)
		{
			// First frame we started displaying the InputText widget, we expect it to take the active id.
			IM_ASSERT(g.ActiveId == id);
			g.TempInputId = g.ActiveId;
		}
		return value_changed;
	}

	COREGUI_API bool TempInputScalar(const Vec4& rect, ID id, DataType data_type, void* data, const char* format, const void* clampMin, const void* clampMax,
		const Vec2& size, const Vec4& padding, const Vec4& rounding, f32 borderThickness)
	{
		char fmt_buf[32];
		char data_buf[32];
		format = ImParseFormatTrimDecorations(format, fmt_buf, IM_ARRAYSIZE(fmt_buf));
		ImGui::DataTypeFormatString(data_buf, IM_ARRAYSIZE(data_buf), (ImGuiDataType)data_type, data, format);
		ImStrTrimBlanks(data_buf);
		String str = data_buf;

		TextInputFlags flags = TextInputFlags_AutoSelectAll | TextInputFlags_NoMarkEdited;
		flags |= InputScalar_DefaultCharsFilter((ImGuiDataType)data_type, format);

		bool value_changed = false;
		if (TempInputText(rect, id, str, flags, size, padding, rounding, borderThickness))
		{
			memcpy(data_buf, str.GetCString(), str.GetLength() + 1);

			// Backup old value
			size_t data_type_size = ImGui::DataTypeGetInfo((ImGuiDataType)data_type)->Size;
			ImGuiDataTypeTempStorage data_backup;
			memcpy(&data_backup, data, data_type_size);

			// Apply new value (or operations) then clamp
			ImGui::DataTypeApplyFromText(data_buf, (ImGuiDataType)data_type, data, format);
			if (clampMin || clampMax)
			{
				if (clampMin && clampMax && ImGui::DataTypeCompare((ImGuiDataType)data_type, clampMin, clampMax) > 0)
					ImSwap(clampMin, clampMax);
				ImGui::DataTypeClamp((ImGuiDataType)data_type, data, clampMin, clampMax);
			}

			// Only mark as edited if new value is different
			value_changed = memcmp(&data_backup, data, data_type_size) != 0;
			if (value_changed)
				ImGui::MarkItemEdited(id);
		}
		return value_changed;
	}

	COREGUI_API bool DragScalar(ID id, DataType dataType, void* data, float speed, const void* min, const void* max, const char* format, 
		const Vec2& size, const Vec4& padding, const Vec4& rounding, f32 borderThickness,
		SliderFlags flags)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		float w = ImGui::CalcItemWidth();
		if (size.x > 0)
			w = size.x;

		//const ImVec2 label_size = ImGui::CalcTextSize("label", NULL, true);
		const f32 labelHeight = ImGui::CalcTextSize("label", NULL, true).y;
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, ImMax(size.y, labelHeight) + padding.top + padding.bottom));
		const ImRect total_bb(frame_bb.Min, frame_bb.Max);

		const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
		ImGui::ItemSize(total_bb, padding.top);
		if (!ImGui::ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
			return false;

		// Default format string when passing NULL
		if (format == NULL)
			format = ImGui::DataTypeGetInfo((ImGuiDataType)dataType)->PrintFmt;

		const bool hovered = ImGui::ItemHoverable(frame_bb, id);
		bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive(id);
		if (!temp_input_is_active)
		{
			// Tabbing or CTRL-clicking on Drag turns it into an InputText
			const bool input_requested_by_tabbing = temp_input_allowed && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
			const bool clicked = hovered && ImGui::IsMouseClicked(0, id);
			const bool double_clicked = (hovered && g.IO.MouseClickedCount[0] == 2 && ImGui::TestKeyOwner(ImGuiKey_MouseLeft, id));
			const bool make_active = (input_requested_by_tabbing || clicked || double_clicked || g.NavActivateId == id);
			if (make_active && (clicked || double_clicked))
				ImGui::SetKeyOwner(ImGuiKey_MouseLeft, id);
			if (make_active && temp_input_allowed)
				if (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl) || double_clicked || (g.NavActivateId == id && (g.NavActivateFlags & ImGuiActivateFlags_PreferInput)))
					temp_input_is_active = true;

			// (Optional) simple click (without moving) turns Drag into an InputText
			if (g.IO.ConfigDragClickToInputText && temp_input_allowed && !temp_input_is_active)
				if (g.ActiveId == id && hovered && g.IO.MouseReleased[0] && !ImGui::IsMouseDragPastThreshold(0, g.IO.MouseDragThreshold * 0.5f))
				{
					g.NavActivateId = id;
					g.NavActivateFlags = ImGuiActivateFlags_PreferInput;
					temp_input_is_active = true;
				}

			if (make_active && !temp_input_is_active)
			{
				ImGui::SetActiveID(id, window);
				ImGui::SetFocusID(id, window);
				ImGui::FocusWindow(window);
				g.ActiveIdUsingNavDirMask = (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
			}
		}

		if (temp_input_is_active)
		{
			// Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
			const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0 && 
				(min == NULL || max == NULL || ImGui::DataTypeCompare((ImGuiDataType)dataType, min, max) < 0);
			return TempInputScalar(Vec4(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Max.x, frame_bb.Max.y),
				id, dataType, data, format, is_clamp_input ? min : NULL, is_clamp_input ? max : NULL,
				size, padding, rounding, borderThickness);
		}

		// Draw frame
		const ImU32 frame_col = ImGui::GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
		//ImGui::RenderNavHighlight(frame_bb, id);
		//ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, style.FrameRounding);

		Vec4 frameRect = Vec4(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Max.x, frame_bb.Max.y);
		RenderNavHighlight(frameRect + Vec4(1, 1, -1, -1), id, borderThickness, rounding);
		RenderFrame(frameRect, frame_col, borderThickness, rounding);

		// Drag behavior
		const bool value_changed = ImGui::DragBehavior(id, (ImGuiDataType)dataType, data, speed, min, max, format, flags);
		if (value_changed)
			ImGui::MarkItemEdited(id);

		// Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
		char value_buf[64];
		const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), (ImGuiDataType)dataType, data, format);
		if (g.LogEnabled)
			ImGui::LogSetNextTextDecoration("{", "}");
		ImGui::RenderTextClipped(frame_bb.Min + ImVec2(padding.left, 0), frame_bb.Max - ImVec2(padding.right, 0), 
			value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | (temp_input_allowed ? ImGuiItemStatusFlags_Inputable : 0));
		return value_changed;
	}

	COREGUI_API bool DragScalar(const Rect& rect, ID id, DataType dataType, void* data, float speed, const void* min, const void* max, const char* format, 
		GuiStyleState normalState, GuiStyleState activeState, SliderFlags flags)
	{
		
		return false;
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

	COREGUI_API bool TableTreeNode(ID id, TreeNodeFlags flags)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const bool display_frame = (flags & ImGuiTreeNodeFlags_Framed) != 0;
		const ImVec2 padding = (display_frame || (flags & ImGuiTreeNodeFlags_FramePadding)) ? style.FramePadding : ImVec2(style.FramePadding.x, ImMin(window->DC.CurrLineTextBaseOffset, style.FramePadding.y));

		//auto label_end = ImGui::FindRenderedTextEnd(label.GetCString());
		const ImVec2 label_size = ImGui::CalcTextSize("label");

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
		//if (!display_frame && (flags & (ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth)) == 0)
		//	interact_bb.Max.x = frame_bb.Min.x + text_width + style.ItemSpacing.x * 2.0f;

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
			//ImGui::RenderTextClipped(text_pos, frame_bb.Max, label.GetCString(), label_end, &label_size);
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
			//ImGui::RenderText(text_pos, label.GetCString(), label_end, false);
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

	COREGUI_API bool CollapsibleHeader(const Rect& localRect, const String& label, ID id, const GuiStyleState& styleState, bool& hovered, bool& held, const Vec4& headerPadding, const Vec4& contentPadding, TreeNodeFlags flags)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		Rect rect = ToWindowCoordinateSpace(localRect);
		auto rectSize = rect.max - rect.min;

		GUI::SetCursorPos(rect.min);

		flags |= TNF_CollapsingHeader;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const bool display_frame = (flags & ImGuiTreeNodeFlags_Framed) != 0;
		//const ImVec2 defaultPadding = (display_frame || (flags & ImGuiTreeNodeFlags_FramePadding)) ? style.FramePadding : ImVec2(style.FramePadding.x, ImMin(window->DC.CurrLineTextBaseOffset, style.FramePadding.y));

		ImVec4 padding = ImVec4(headerPadding.x, headerPadding.y, headerPadding.z, headerPadding.w);

		auto label_end = ImGui::FindRenderedTextEnd(label.GetCString());
		const ImVec2 label_size = ImGui::CalcTextSize(label.GetCString(), label_end, false);

		// We vertically grow up to current line height up the typical widget height.
		const float frame_height = ImMax(ImMin(window->DC.CurrLineSize.y, g.FontSize + headerPadding.top + headerPadding.bottom), label_size.y + padding.y + padding.w);
		ImRect frame_bb;
		frame_bb.Min.x = (flags & ImGuiTreeNodeFlags_SpanFullWidth) ? window->WorkRect.Min.x : window->DC.CursorPos.x;
		frame_bb.Min.y = window->DC.CursorPos.y;
		frame_bb.Max.x = frame_bb.Min.x + rectSize.width;
		frame_bb.Max.y = window->DC.CursorPos.y + frame_height;
		if (display_frame)
		{
			// Framed header expand a little outside the default padding, to the edge of InnerClipRect
			// (FIXME: May remove this at some point and make InnerClipRect align with WindowPadding.x instead of WindowPadding.x*0.5f)
			//frame_bb.Min.x -= IM_FLOOR(window->WindowPadding.x * 0.5f - 1.0f);
			//frame_bb.Max.x += IM_FLOOR(window->WindowPadding.x * 0.5f);
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
		const ImU32 text_col = styleState.foreground.ToU32();
		ImGui::PushStyleColor(ImGuiCol_Text, text_col);
		ImGuiNavHighlightFlags nav_highlight_flags = ImGuiNavHighlightFlags_TypeThin;
		if (display_frame)
		{
			// Framed type
			//ImGui::GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);

			Vec4 frameRect = Vec4(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Max.x, frame_bb.Max.y);
			GUI::RenderFrame(frameRect, styleState.background, styleState.borderColor,
				styleState.borderThickness,
				styleState.borderRadius);
			GUI::RenderNavHighlight(frameRect, id, nav_highlight_flags, styleState.borderRadius);

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
				//const ImU32 bg_col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
				
				Vec4 frameRect = Vec4(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Max.x, frame_bb.Max.y);

				GUI::RenderFrame(frameRect, styleState.background, styleState.borderColor, styleState.borderThickness, styleState.borderRadius);
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
		ImGui::PopStyleColor(1);

		if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
			ImGui::TreePushOverrideID(id);
		IMGUI_TEST_ENGINE_ITEM_INFO(id, label.GetCString(), g.LastItemData.StatusFlags | (is_leaf ? 0 : ImGuiItemStatusFlags_Openable) | (is_open ? ImGuiItemStatusFlags_Opened : 0));
		return is_open;
	}


	struct ImGuiTabBarSection
	{
		int                 TabCount;               // Number of tabs in this section.
		float               Width;                  // Sum of width of tabs in this section (after shrinking down)
		float               Spacing;                // Horizontal spacing at the end of the section.

		ImGuiTabBarSection() { memset(this, 0, sizeof(*this)); }
	};

	static void             TabBarLayout(ImGuiTabBar* tab_bar);
	static ImU32            TabBarCalcTabID(ImGuiTabBar* tab_bar, const char* label, ImGuiWindow* docked_window);
	static float            TabBarCalcMaxTabWidth();
	static float            TabBarScrollClamp(ImGuiTabBar* tab_bar, float scrolling);
	static void             TabBarScrollToTab(ImGuiTabBar* tab_bar, ImGuiID tab_id, ImGuiTabBarSection* sections);
	static ImGuiTabItem* TabBarScrollingButtons(ImGuiTabBar* tab_bar);
	static ImGuiTabItem* TabBarTabListPopupButton(ImGuiTabBar* tab_bar);
	static inline int TabItemGetSectionIdx(const ImGuiTabItem* tab);
	static int IMGUI_CDECL TabItemComparerBySection(const void* lhs, const void* rhs);
	static int IMGUI_CDECL TabItemComparerByBeginOrder(const void* lhs, const void* rhs);
	static float TabBarCalcScrollableWidth(ImGuiTabBar* tab_bar, ImGuiTabBarSection* sections);

	static void TabBarLayout(ImGuiTabBar* tab_bar)
	{
		ImGuiContext& g = *GImGui;
		tab_bar->WantLayout = false;

		// Garbage collect by compacting list
		// Detect if we need to sort out tab list (e.g. in rare case where a tab changed section)
		int tab_dst_n = 0;
		bool need_sort_by_section = false;
		ImGuiTabBarSection sections[3]; // Layout sections: Leading, Central, Trailing
		for (int tab_src_n = 0; tab_src_n < tab_bar->Tabs.Size; tab_src_n++)
		{
			ImGuiTabItem* tab = &tab_bar->Tabs[tab_src_n];
			if (tab->LastFrameVisible < tab_bar->PrevFrameVisible || tab->WantClose)
			{
				// Remove tab
				if (tab_bar->VisibleTabId == tab->ID) { tab_bar->VisibleTabId = 0; }
				if (tab_bar->SelectedTabId == tab->ID) { tab_bar->SelectedTabId = 0; }
				if (tab_bar->NextSelectedTabId == tab->ID) { tab_bar->NextSelectedTabId = 0; }
				continue;
			}
			if (tab_dst_n != tab_src_n)
				tab_bar->Tabs[tab_dst_n] = tab_bar->Tabs[tab_src_n];

			tab = &tab_bar->Tabs[tab_dst_n];
			tab->IndexDuringLayout = (ImS16)tab_dst_n;

			// We will need sorting if tabs have changed section (e.g. moved from one of Leading/Central/Trailing to another)
			int curr_tab_section_n = TabItemGetSectionIdx(tab);
			if (tab_dst_n > 0)
			{
				ImGuiTabItem* prev_tab = &tab_bar->Tabs[tab_dst_n - 1];
				int prev_tab_section_n = TabItemGetSectionIdx(prev_tab);
				if (curr_tab_section_n == 0 && prev_tab_section_n != 0)
					need_sort_by_section = true;
				if (prev_tab_section_n == 2 && curr_tab_section_n != 2)
					need_sort_by_section = true;
			}

			sections[curr_tab_section_n].TabCount++;
			tab_dst_n++;
		}
		if (tab_bar->Tabs.Size != tab_dst_n)
			tab_bar->Tabs.resize(tab_dst_n);

		if (need_sort_by_section)
			ImQsort(tab_bar->Tabs.Data, tab_bar->Tabs.Size, sizeof(ImGuiTabItem), TabItemComparerBySection);

		// Calculate spacing between sections
		sections[0].Spacing = sections[0].TabCount > 0 && (sections[1].TabCount + sections[2].TabCount) > 0 ? g.Style.ItemInnerSpacing.x : 0.0f;
		sections[1].Spacing = sections[1].TabCount > 0 && sections[2].TabCount > 0 ? g.Style.ItemInnerSpacing.x : 0.0f;

		// Setup next selected tab
		ImGuiID scroll_to_tab_id = 0;
		if (tab_bar->NextSelectedTabId)
		{
			tab_bar->SelectedTabId = tab_bar->NextSelectedTabId;
			tab_bar->NextSelectedTabId = 0;
			scroll_to_tab_id = tab_bar->SelectedTabId;
		}

		// Process order change request (we could probably process it when requested but it's just saner to do it in a single spot).
		if (tab_bar->ReorderRequestTabId != 0)
		{
			if (ImGui::TabBarProcessReorder(tab_bar))
				if (tab_bar->ReorderRequestTabId == tab_bar->SelectedTabId)
					scroll_to_tab_id = tab_bar->ReorderRequestTabId;
			tab_bar->ReorderRequestTabId = 0;
		}

		// Tab List Popup (will alter tab_bar->BarRect and therefore the available width!)
		const bool tab_list_popup_button = (tab_bar->Flags & ImGuiTabBarFlags_TabListPopupButton) != 0;
		if (tab_list_popup_button)
			if (ImGuiTabItem* tab_to_select = TabBarTabListPopupButton(tab_bar)) // NB: Will alter BarRect.Min.x!
				scroll_to_tab_id = tab_bar->SelectedTabId = tab_to_select->ID;

		// Leading/Trailing tabs will be shrink only if central one aren't visible anymore, so layout the shrink data as: leading, trailing, central
		// (whereas our tabs are stored as: leading, central, trailing)
		int shrink_buffer_indexes[3] = { 0, sections[0].TabCount + sections[2].TabCount, sections[0].TabCount };
		g.ShrinkWidthBuffer.resize(tab_bar->Tabs.Size);

		// Compute ideal tabs widths + store them into shrink buffer
		ImGuiTabItem* most_recently_selected_tab = NULL;
		int curr_section_n = -1;
		bool found_selected_tab_id = false;
		for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
		{
			ImGuiTabItem* tab = &tab_bar->Tabs[tab_n];
			IM_ASSERT(tab->LastFrameVisible >= tab_bar->PrevFrameVisible);

			if ((most_recently_selected_tab == NULL || most_recently_selected_tab->LastFrameSelected < tab->LastFrameSelected) && !(tab->Flags & ImGuiTabItemFlags_Button))
				most_recently_selected_tab = tab;
			if (tab->ID == tab_bar->SelectedTabId)
				found_selected_tab_id = true;
			if (scroll_to_tab_id == 0 && g.NavJustMovedToId == tab->ID)
				scroll_to_tab_id = tab->ID;

			// Refresh tab width immediately, otherwise changes of style e.g. style.FramePadding.x would noticeably lag in the tab bar.
			// Additionally, when using TabBarAddTab() to manipulate tab bar order we occasionally insert new tabs that don't have a width yet,
			// and we cannot wait for the next BeginTabItem() call. We cannot compute this width within TabBarAddTab() because font size depends on the active window.
			const char* tab_name = ImGui::TabBarGetTabName(tab_bar, tab);
			const bool has_close_button_or_unsaved_marker = (tab->Flags & ImGuiTabItemFlags_NoCloseButton) == 0 || (tab->Flags & ImGuiTabItemFlags_UnsavedDocument);
			tab->ContentWidth = (tab->RequestedWidth >= 0.0f) ? tab->RequestedWidth : ImGui::TabItemCalcSize(tab_name, has_close_button_or_unsaved_marker).x;

			int section_n = TabItemGetSectionIdx(tab);
			ImGuiTabBarSection* section = &sections[section_n];
			section->Width += tab->ContentWidth + (section_n == curr_section_n ? g.Style.ItemInnerSpacing.x : 0.0f);
			curr_section_n = section_n;

			// Store data so we can build an array sorted by width if we need to shrink tabs down
			IM_MSVC_WARNING_SUPPRESS(6385);
			ImGuiShrinkWidthItem* shrink_width_item = &g.ShrinkWidthBuffer[shrink_buffer_indexes[section_n]++];
			shrink_width_item->Index = tab_n;
			shrink_width_item->Width = shrink_width_item->InitialWidth = tab->ContentWidth;
			tab->Width = ImMax(tab->ContentWidth, 1.0f);
		}

		// Compute total ideal width (used for e.g. auto-resizing a window)
		tab_bar->WidthAllTabsIdeal = 0.0f;
		for (int section_n = 0; section_n < 3; section_n++)
			tab_bar->WidthAllTabsIdeal += sections[section_n].Width + sections[section_n].Spacing;

		// Horizontal scrolling buttons
		// (note that TabBarScrollButtons() will alter BarRect.Max.x)
		if ((tab_bar->WidthAllTabsIdeal > tab_bar->BarRect.GetWidth() && tab_bar->Tabs.Size > 1) && !(tab_bar->Flags & ImGuiTabBarFlags_NoTabListScrollingButtons) && (tab_bar->Flags & ImGuiTabBarFlags_FittingPolicyScroll))
			if (ImGuiTabItem* scroll_and_select_tab = TabBarScrollingButtons(tab_bar))
			{
				scroll_to_tab_id = scroll_and_select_tab->ID;
				if ((scroll_and_select_tab->Flags & ImGuiTabItemFlags_Button) == 0)
					tab_bar->SelectedTabId = scroll_to_tab_id;
			}

		// Shrink widths if full tabs don't fit in their allocated space
		float section_0_w = sections[0].Width + sections[0].Spacing;
		float section_1_w = sections[1].Width + sections[1].Spacing;
		float section_2_w = sections[2].Width + sections[2].Spacing;
		bool central_section_is_visible = (section_0_w + section_2_w) < tab_bar->BarRect.GetWidth();
		float width_excess;
		if (central_section_is_visible)
			width_excess = ImMax(section_1_w - (tab_bar->BarRect.GetWidth() - section_0_w - section_2_w), 0.0f); // Excess used to shrink central section
		else
			width_excess = (section_0_w + section_2_w) - tab_bar->BarRect.GetWidth(); // Excess used to shrink leading/trailing section

		// With ImGuiTabBarFlags_FittingPolicyScroll policy, we will only shrink leading/trailing if the central section is not visible anymore
		if (width_excess >= 1.0f && ((tab_bar->Flags & ImGuiTabBarFlags_FittingPolicyResizeDown) || !central_section_is_visible))
		{
			int shrink_data_count = (central_section_is_visible ? sections[1].TabCount : sections[0].TabCount + sections[2].TabCount);
			int shrink_data_offset = (central_section_is_visible ? sections[0].TabCount + sections[2].TabCount : 0);
			ImGui::ShrinkWidths(g.ShrinkWidthBuffer.Data + shrink_data_offset, shrink_data_count, width_excess);

			// Apply shrunk values into tabs and sections
			for (int tab_n = shrink_data_offset; tab_n < shrink_data_offset + shrink_data_count; tab_n++)
			{
				ImGuiTabItem* tab = &tab_bar->Tabs[g.ShrinkWidthBuffer[tab_n].Index];
				float shrinked_width = IM_FLOOR(g.ShrinkWidthBuffer[tab_n].Width);
				if (shrinked_width < 0.0f)
					continue;

				shrinked_width = ImMax(1.0f, shrinked_width);
				int section_n = TabItemGetSectionIdx(tab);
				sections[section_n].Width -= (tab->Width - shrinked_width);
				tab->Width = shrinked_width;
			}
		}

		// Layout all active tabs
		int section_tab_index = 0;
		float tab_offset = 0.0f;
		tab_bar->WidthAllTabs = 0.0f;
		for (int section_n = 0; section_n < 3; section_n++)
		{
			ImGuiTabBarSection* section = &sections[section_n];
			if (section_n == 2)
				tab_offset = ImMin(ImMax(0.0f, tab_bar->BarRect.GetWidth() - section->Width), tab_offset);

			for (int tab_n = 0; tab_n < section->TabCount; tab_n++)
			{
				ImGuiTabItem* tab = &tab_bar->Tabs[section_tab_index + tab_n];
				tab->Offset = tab_offset;
				tab->NameOffset = -1;
				tab_offset += tab->Width + (tab_n < section->TabCount - 1 ? g.Style.ItemInnerSpacing.x : 0.0f);
			}
			tab_bar->WidthAllTabs += ImMax(section->Width + section->Spacing, 0.0f);
			tab_offset += section->Spacing;
			section_tab_index += section->TabCount;
		}

		// Clear name buffers
		tab_bar->TabsNames.Buf.resize(0);

		// If we have lost the selected tab, select the next most recently active one
		if (found_selected_tab_id == false)
			tab_bar->SelectedTabId = 0;
		if (tab_bar->SelectedTabId == 0 && tab_bar->NextSelectedTabId == 0 && most_recently_selected_tab != NULL)
			scroll_to_tab_id = tab_bar->SelectedTabId = most_recently_selected_tab->ID;

		// Lock in visible tab
		tab_bar->VisibleTabId = tab_bar->SelectedTabId;
		tab_bar->VisibleTabWasSubmitted = false;

		// CTRL+TAB can override visible tab temporarily
		if (g.NavWindowingTarget != NULL && g.NavWindowingTarget->DockNode && g.NavWindowingTarget->DockNode->TabBar == tab_bar)
			tab_bar->VisibleTabId = scroll_to_tab_id = g.NavWindowingTarget->TabId;

		// Apply request requests
		if (scroll_to_tab_id != 0)
			TabBarScrollToTab(tab_bar, scroll_to_tab_id, sections);
		else if ((tab_bar->Flags & ImGuiTabBarFlags_FittingPolicyScroll) && 
			ImGui::IsMouseHoveringRect(tab_bar->BarRect.Min, tab_bar->BarRect.Max, true) && 
			ImGui::IsWindowContentHoverable(g.CurrentWindow))
		{
			const float wheel = g.IO.MouseWheelRequestAxisSwap ? g.IO.MouseWheel : g.IO.MouseWheelH;
			const ImGuiKey wheel_key = g.IO.MouseWheelRequestAxisSwap ? ImGuiKey_MouseWheelY : ImGuiKey_MouseWheelX;
			if (ImGui::TestKeyOwner(wheel_key, tab_bar->ID) && wheel != 0.0f)
			{
				const float scroll_step = wheel * TabBarCalcScrollableWidth(tab_bar, sections) / 3.0f;
				tab_bar->ScrollingTargetDistToVisibility = 0.0f;
				tab_bar->ScrollingTarget = TabBarScrollClamp(tab_bar, tab_bar->ScrollingTarget - scroll_step);
			}
			ImGui::SetKeyOwner(wheel_key, tab_bar->ID);
		}

		// Update scrolling
		tab_bar->ScrollingAnim = TabBarScrollClamp(tab_bar, tab_bar->ScrollingAnim);
		tab_bar->ScrollingTarget = TabBarScrollClamp(tab_bar, tab_bar->ScrollingTarget);
		if (tab_bar->ScrollingAnim != tab_bar->ScrollingTarget)
		{
			// Scrolling speed adjust itself so we can always reach our target in 1/3 seconds.
			// Teleport if we are aiming far off the visible line
			tab_bar->ScrollingSpeed = ImMax(tab_bar->ScrollingSpeed, 70.0f * g.FontSize);
			tab_bar->ScrollingSpeed = ImMax(tab_bar->ScrollingSpeed, ImFabs(tab_bar->ScrollingTarget - tab_bar->ScrollingAnim) / 0.3f);
			const bool teleport = (tab_bar->PrevFrameVisible + 1 < g.FrameCount) || (tab_bar->ScrollingTargetDistToVisibility > 10.0f * g.FontSize);
			tab_bar->ScrollingAnim = teleport ? tab_bar->ScrollingTarget : ImLinearSweep(tab_bar->ScrollingAnim, tab_bar->ScrollingTarget, g.IO.DeltaTime * tab_bar->ScrollingSpeed);
		}
		else
		{
			tab_bar->ScrollingSpeed = 0.0f;
		}
		tab_bar->ScrollingRectMinX = tab_bar->BarRect.Min.x + sections[0].Width + sections[0].Spacing;
		tab_bar->ScrollingRectMaxX = tab_bar->BarRect.Max.x - sections[2].Width - sections[1].Spacing;

		// Actual layout in host window (we don't do it in BeginTabBar() so as not to waste an extra frame)
		ImGuiWindow* window = g.CurrentWindow;
		window->DC.CursorPos = tab_bar->BarRect.Min;
		ImGui::ItemSize(ImVec2(tab_bar->WidthAllTabs, tab_bar->BarRect.GetHeight()), tab_bar->FramePadding.y);
		window->DC.IdealMaxPos.x = ImMax(window->DC.IdealMaxPos.x, tab_bar->BarRect.Min.x + tab_bar->WidthAllTabsIdeal);
	}

	static float TabBarCalcScrollableWidth(ImGuiTabBar* tab_bar, ImGuiTabBarSection* sections)
	{
		return tab_bar->BarRect.GetWidth() - sections[0].Width - sections[2].Width - sections[1].Spacing;
	}

	static ImU32 TabBarCalcTabID(ImGuiTabBar* tab_bar, const char* label, ImGuiWindow* docked_window)
	{
		if (docked_window != NULL)
		{
			IM_UNUSED(tab_bar);
			IM_ASSERT(tab_bar->Flags & ImGuiTabBarFlags_DockNode);
			ImGuiID id = docked_window->TabId;
			ImGui::KeepAliveID(id);
			return id;
		}
		else
		{
			ImGuiWindow* window = GImGui->CurrentWindow;
			return window->GetID(label);
		}
	}

	static float TabBarCalcMaxTabWidth()
	{
		ImGuiContext& g = *GImGui;
		return g.FontSize * 20.0f;
	}

	static float TabBarScrollClamp(ImGuiTabBar* tab_bar, float scrolling)
	{
		scrolling = ImMin(scrolling, tab_bar->WidthAllTabs - tab_bar->BarRect.GetWidth());
		return ImMax(scrolling, 0.0f);
	}

	static void TabBarScrollToTab(ImGuiTabBar* tab_bar, ImGuiID tab_id, ImGuiTabBarSection* sections)
	{
		ImGuiTabItem* tab = ImGui::TabBarFindTabByID(tab_bar, tab_id);
		if (tab == NULL)
			return;
		if (tab->Flags & ImGuiTabItemFlags_SectionMask_)
			return;

		ImGuiContext& g = *GImGui;
		float margin = g.FontSize * 1.0f; // When to scroll to make Tab N+1 visible always make a bit of N visible to suggest more scrolling area (since we don't have a scrollbar)
		int order = ImGui::TabBarGetTabOrder(tab_bar, tab);

		// Scrolling happens only in the central section (leading/trailing sections are not scrolling)
		float scrollable_width = TabBarCalcScrollableWidth(tab_bar, sections);

		// We make all tabs positions all relative Sections[0].Width to make code simpler
		float tab_x1 = tab->Offset - sections[0].Width + (order > sections[0].TabCount - 1 ? -margin : 0.0f);
		float tab_x2 = tab->Offset - sections[0].Width + tab->Width + (order + 1 < tab_bar->Tabs.Size - sections[2].TabCount ? margin : 1.0f);
		tab_bar->ScrollingTargetDistToVisibility = 0.0f;
		if (tab_bar->ScrollingTarget > tab_x1 || (tab_x2 - tab_x1 >= scrollable_width))
		{
			// Scroll to the left
			tab_bar->ScrollingTargetDistToVisibility = ImMax(tab_bar->ScrollingAnim - tab_x2, 0.0f);
			tab_bar->ScrollingTarget = tab_x1;
		}
		else if (tab_bar->ScrollingTarget < tab_x2 - scrollable_width)
		{
			// Scroll to the right
			tab_bar->ScrollingTargetDistToVisibility = ImMax((tab_x1 - scrollable_width) - tab_bar->ScrollingAnim, 0.0f);
			tab_bar->ScrollingTarget = tab_x2 - scrollable_width;
		}
	}

	static ImGuiTabItem* TabBarScrollingButtons(ImGuiTabBar* tab_bar)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		const ImVec2 arrow_button_size(g.FontSize - 2.0f, g.FontSize + g.Style.FramePadding.y * 2.0f);
		const float scrolling_buttons_width = arrow_button_size.x * 2.0f;

		const ImVec2 backup_cursor_pos = window->DC.CursorPos;
		//window->DrawList->AddRect(ImVec2(tab_bar->BarRect.Max.x - scrolling_buttons_width, tab_bar->BarRect.Min.y), ImVec2(tab_bar->BarRect.Max.x, tab_bar->BarRect.Max.y), IM_COL32(255,0,0,255));

		int select_dir = 0;
		ImVec4 arrow_col = g.Style.Colors[ImGuiCol_Text];
		arrow_col.w *= 0.5f;

		ImGui::PushStyleColor(ImGuiCol_Text, arrow_col);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		const float backup_repeat_delay = g.IO.KeyRepeatDelay;
		const float backup_repeat_rate = g.IO.KeyRepeatRate;
		g.IO.KeyRepeatDelay = 0.250f;
		g.IO.KeyRepeatRate = 0.200f;
		float x = ImMax(tab_bar->BarRect.Min.x, tab_bar->BarRect.Max.x - scrolling_buttons_width);
		window->DC.CursorPos = ImVec2(x, tab_bar->BarRect.Min.y);
		if (ImGui::ArrowButtonEx("##<", ImGuiDir_Left, arrow_button_size, ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_Repeat))
			select_dir = -1;
		window->DC.CursorPos = ImVec2(x + arrow_button_size.x, tab_bar->BarRect.Min.y);
		if (ImGui::ArrowButtonEx("##>", ImGuiDir_Right, arrow_button_size, ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_Repeat))
			select_dir = +1;
		PopStyleColor(2);
		g.IO.KeyRepeatRate = backup_repeat_rate;
		g.IO.KeyRepeatDelay = backup_repeat_delay;

		ImGuiTabItem* tab_to_scroll_to = NULL;
		if (select_dir != 0)
			if (ImGuiTabItem* tab_item = ImGui::TabBarFindTabByID(tab_bar, tab_bar->SelectedTabId))
			{
				int selected_order = ImGui::TabBarGetTabOrder(tab_bar, tab_item);
				int target_order = selected_order + select_dir;

				// Skip tab item buttons until another tab item is found or end is reached
				while (tab_to_scroll_to == NULL)
				{
					// If we are at the end of the list, still scroll to make our tab visible
					tab_to_scroll_to = &tab_bar->Tabs[(target_order >= 0 && target_order < tab_bar->Tabs.Size) ? target_order : selected_order];

					// Cross through buttons
					// (even if first/last item is a button, return it so we can update the scroll)
					if (tab_to_scroll_to->Flags & ImGuiTabItemFlags_Button)
					{
						target_order += select_dir;
						selected_order += select_dir;
						tab_to_scroll_to = (target_order < 0 || target_order >= tab_bar->Tabs.Size) ? tab_to_scroll_to : NULL;
					}
				}
			}
		window->DC.CursorPos = backup_cursor_pos;
		tab_bar->BarRect.Max.x -= scrolling_buttons_width + 1.0f;

		return tab_to_scroll_to;
	}

	static ImGuiTabItem* TabBarTabListPopupButton(ImGuiTabBar* tab_bar)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		// We use g.Style.FramePadding.y to match the square ArrowButton size
		const float tab_list_popup_button_width = g.FontSize + g.Style.FramePadding.y;
		const ImVec2 backup_cursor_pos = window->DC.CursorPos;
		window->DC.CursorPos = ImVec2(tab_bar->BarRect.Min.x - g.Style.FramePadding.y, tab_bar->BarRect.Min.y);
		tab_bar->BarRect.Min.x += tab_list_popup_button_width;

		ImVec4 arrow_col = g.Style.Colors[ImGuiCol_Text];
		arrow_col.w *= 0.5f;
		ImGui::PushStyleColor(ImGuiCol_Text, arrow_col);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		bool open = ImGui::BeginCombo("##v", NULL, ImGuiComboFlags_NoPreview | ImGuiComboFlags_HeightLargest);
		PopStyleColor(2);

		ImGuiTabItem* tab_to_select = NULL;
		if (open)
		{
			for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
			{
				ImGuiTabItem* tab = &tab_bar->Tabs[tab_n];
				if (tab->Flags & ImGuiTabItemFlags_Button)
					continue;

				const char* tab_name = ImGui::TabBarGetTabName(tab_bar, tab);
				if (ImGui::Selectable(tab_name, tab_bar->SelectedTabId == tab->ID))
					tab_to_select = tab;
			}
			ImGui::EndCombo();
		}

		window->DC.CursorPos = backup_cursor_pos;
		return tab_to_select;
	}

	static inline int TabItemGetSectionIdx(const ImGuiTabItem* tab)
	{
		return (tab->Flags & ImGuiTabItemFlags_Leading) ? 0 : (tab->Flags & ImGuiTabItemFlags_Trailing) ? 2 : 1;
	}

	static int IMGUI_CDECL TabItemComparerBySection(const void* lhs, const void* rhs)
	{
		const ImGuiTabItem* a = (const ImGuiTabItem*)lhs;
		const ImGuiTabItem* b = (const ImGuiTabItem*)rhs;
		const int a_section = TabItemGetSectionIdx(a);
		const int b_section = TabItemGetSectionIdx(b);
		if (a_section != b_section)
			return a_section - b_section;
		return (int)(a->IndexDuringLayout - b->IndexDuringLayout);
	}

	static int IMGUI_CDECL TabItemComparerByBeginOrder(const void* lhs, const void* rhs)
	{
		const ImGuiTabItem* a = (const ImGuiTabItem*)lhs;
		const ImGuiTabItem* b = (const ImGuiTabItem*)rhs;
		return (int)(a->BeginOrder - b->BeginOrder);
	}

	static ImGuiTabBar* GetTabBarFromTabBarRef(const ImGuiPtrOrIndex& ref)
	{
		ImGuiContext& g = *GImGui;
		return ref.Ptr ? (ImGuiTabBar*)ref.Ptr : g.TabBars.GetByIndex(ref.Index);
	}

	static ImGuiPtrOrIndex GetTabBarRefFromTabBar(ImGuiTabBar* tab_bar)
	{
		ImGuiContext& g = *GImGui;
		if (g.TabBars.Contains(tab_bar))
			return ImGuiPtrOrIndex(g.TabBars.GetIndex(tab_bar));
		return ImGuiPtrOrIndex(tab_bar);
	}

	COREGUI_API bool BeginTabBar(const String& id, TabBarFlags flags)
	{
		return ImGui::BeginTabBar(id.GetCString(), flags);
	}

	COREGUI_API bool BeginTabBar(const Rect& rect, ID id, const Vec4& tabBarPadding, TabBarFlags flags)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		if (window->SkipItems)
			return false;

		ImGui::SetCursorPos(ImVec2(rect.x, rect.y));

		Rect contentSpaceRect = rect;
		contentSpaceRect.top += 30 + ImGui::GetStyle().FramePadding.y;

		ImGuiTabBar* tab_bar = g.TabBars.GetOrAddByKey((ImGuiID)id);
		ImRect tab_bar_bb = ImRect(window->DC.CursorPos.x, window->DC.CursorPos.y, 
			window->DC.CursorPos.x + rect.max.x - rect.min.x, window->DC.CursorPos.y + g.FontSize + tabBarPadding.top + tabBarPadding.bottom);
		tab_bar->ID = id;

		flags |= TabBarFlags_IsFocused;

		if ((flags & TabBarFlags_DockNode) == 0)
			ImGui::PushOverrideID(tab_bar->ID);

		// Add to stack
		g.CurrentTabBarStack.push_back(GetTabBarRefFromTabBar(tab_bar));
		g.CurrentTabBar = tab_bar;

		// Append with multiple BeginTabBar()/EndTabBar() pairs.
		tab_bar->BackupCursorPos = window->DC.CursorPos;
		if (tab_bar->CurrFrameVisible == g.FrameCount)
		{
			window->DC.CursorPos = ImVec2(tab_bar->BarRect.Min.x, tab_bar->BarRect.Max.y + tab_bar->ItemSpacingY);
			tab_bar->BeginCount++;
			coordinateSpaceStack.Push(contentSpaceRect);
			return true;
		}

		// Ensure correct ordering when toggling TabBarFlags_Reorderable flag, or when a new tab was added while being not reorderable
		if ((flags & TabBarFlags_Reorderable) != (tab_bar->Flags & TabBarFlags_Reorderable) || (tab_bar->TabsAddedNew && !(flags & TabBarFlags_Reorderable)))
			if ((flags & TabBarFlags_DockNode) == 0) // FIXME: TabBar with DockNode can now be hybrid
				ImQsort(tab_bar->Tabs.Data, tab_bar->Tabs.Size, sizeof(ImGuiTabItem), TabItemComparerByBeginOrder);
		tab_bar->TabsAddedNew = false;

		// Flags
		if ((flags & TabBarFlags_FittingPolicyMask_) == 0)
			flags |= TabBarFlags_FittingPolicyDefault_;

		tab_bar->Flags = flags;
		tab_bar->BarRect = tab_bar_bb;
		tab_bar->WantLayout = true; // Layout will be done on the first call to ItemTab()
		tab_bar->PrevFrameVisible = tab_bar->CurrFrameVisible;
		tab_bar->CurrFrameVisible = g.FrameCount;
		tab_bar->PrevTabsContentsHeight = tab_bar->CurrTabsContentsHeight;
		tab_bar->CurrTabsContentsHeight = 0.0f;
		tab_bar->ItemSpacingY = g.Style.ItemSpacing.y;
		tab_bar->FramePadding = g.Style.FramePadding;
		tab_bar->TabsActiveCount = 0;
		tab_bar->LastTabItemIdx = -1;
		tab_bar->BeginCount = 1;

		// Set cursor pos in a way which only be used in the off-chance the user erroneously submits item before BeginTabItem(): items will overlap
		window->DC.CursorPos = ImVec2(tab_bar->BarRect.Min.x, tab_bar->BarRect.Max.y + tab_bar->ItemSpacingY);

		// Draw separator
		const ImU32 col = ImGui::GetColorU32((flags & TabBarFlags_IsFocused) ? ImGuiCol_TabActive : ImGuiCol_TabUnfocusedActive);
		const float y = tab_bar->BarRect.Max.y - 1.0f;
		{
			const float separator_min_x = tab_bar->BarRect.Min.x - IM_FLOOR(window->WindowPadding.x * 0.5f);
			const float separator_max_x = tab_bar->BarRect.Max.x + IM_FLOOR(window->WindowPadding.x * 0.5f);
			window->DrawList->AddLine(ImVec2(separator_min_x, y), ImVec2(separator_max_x, y), col, 1.0f);
		}
		coordinateSpaceStack.Push(contentSpaceRect);
		return true;
	}

	static bool TabItemEx(ImGuiTabBar* tab_bar, const char* label, bool* p_open, ImGuiTabItemFlags flags, ImGuiWindow* docked_window, const Vec4& padding = {})
	{
		// Layout whole tab bar if not already done
		ImGuiContext& g = *GImGui;
		if (tab_bar->WantLayout)
		{
			ImGuiNextItemData backup_next_item_data = g.NextItemData;
			TabBarLayout(tab_bar);
			g.NextItemData = backup_next_item_data;
		}
		ImGuiWindow* window = g.CurrentWindow;
		if (window->SkipItems)
			return false;

		const ImGuiStyle& style = g.Style;
		const ImGuiID id = TabBarCalcTabID(tab_bar, label, docked_window);

		// If the user called us with *p_open == false, we early out and don't render.
		// We make a call to ItemAdd() so that attempts to use a contextual popup menu with an implicit ID won't use an older ID.
		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
		if (p_open && !*p_open)
		{
			ImGui::ItemAdd(ImRect(), id, NULL, ImGuiItemFlags_NoNav);
			return false;
		}

		IM_ASSERT(!p_open || !(flags & ImGuiTabItemFlags_Button));
		IM_ASSERT((flags & (ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_Trailing)) != (ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_Trailing)); // Can't use both Leading and Trailing

		// Store into ImGuiTabItemFlags_NoCloseButton, also honor ImGuiTabItemFlags_NoCloseButton passed by user (although not documented)
		if (flags & ImGuiTabItemFlags_NoCloseButton)
			p_open = NULL;
		else if (p_open == NULL)
			flags |= ImGuiTabItemFlags_NoCloseButton;

		// Acquire tab data
		ImGuiTabItem* tab = ImGui::TabBarFindTabByID(tab_bar, id);
		bool tab_is_new = false;
		if (tab == NULL)
		{
			tab_bar->Tabs.push_back(ImGuiTabItem());
			tab = &tab_bar->Tabs.back();
			tab->ID = id;
			tab_bar->TabsAddedNew = tab_is_new = true;
		}
		tab_bar->LastTabItemIdx = (ImS16)tab_bar->Tabs.index_from_ptr(tab);

		// Calculate tab contents size
		ImVec2 size = ImGui::TabItemCalcSize(label, (p_open != NULL) || (flags & ImGuiTabItemFlags_UnsavedDocument));
		tab->RequestedWidth = -1.0f;
		if (g.NextItemData.Flags & ImGuiNextItemDataFlags_HasWidth)
			size.x = tab->RequestedWidth = g.NextItemData.Width;
		if (tab_is_new)
			tab->Width = ImMax(1.0f, size.x);
		tab->ContentWidth = size.x;
		tab->BeginOrder = tab_bar->TabsActiveCount++;

		const bool tab_bar_appearing = (tab_bar->PrevFrameVisible + 1 < g.FrameCount);
		const bool tab_bar_focused = (tab_bar->Flags & ImGuiTabBarFlags_IsFocused) != 0;
		const bool tab_appearing = (tab->LastFrameVisible + 1 < g.FrameCount);
		const bool tab_just_unsaved = (flags & ImGuiTabItemFlags_UnsavedDocument) && !(tab->Flags & ImGuiTabItemFlags_UnsavedDocument);
		const bool is_tab_button = (flags & ImGuiTabItemFlags_Button) != 0;
		tab->LastFrameVisible = g.FrameCount;
		tab->Flags = flags;
		tab->Window = docked_window;

		// Append name _WITH_ the zero-terminator
		// (regular tabs are permitted in a DockNode tab bar, but window tabs not permitted in a non-DockNode tab bar)
		if (docked_window != NULL)
		{
			IM_ASSERT(tab_bar->Flags & ImGuiTabBarFlags_DockNode);
			tab->NameOffset = -1;
		}
		else
		{
			tab->NameOffset = (ImS32)tab_bar->TabsNames.size();
			tab_bar->TabsNames.append(label, label + strlen(label) + 1);
		}

		// Update selected tab
		if (!is_tab_button)
		{
			if (tab_appearing && (tab_bar->Flags & ImGuiTabBarFlags_AutoSelectNewTabs) && tab_bar->NextSelectedTabId == 0)
				if (!tab_bar_appearing || tab_bar->SelectedTabId == 0)
					ImGui::TabBarQueueFocus(tab_bar, tab); // New tabs gets activated
			if ((flags & ImGuiTabItemFlags_SetSelected) && (tab_bar->SelectedTabId != id)) // _SetSelected can only be passed on explicit tab bar
				ImGui::TabBarQueueFocus(tab_bar, tab);
		}

		// Lock visibility
		// (Note: tab_contents_visible != tab_selected... because CTRL+TAB operations may preview some tabs without selecting them!)
		bool tab_contents_visible = (tab_bar->VisibleTabId == id);
		if (tab_contents_visible)
			tab_bar->VisibleTabWasSubmitted = true;

		// On the very first frame of a tab bar we let first tab contents be visible to minimize appearing glitches
		if (!tab_contents_visible && tab_bar->SelectedTabId == 0 && tab_bar_appearing && docked_window == NULL)
			if (tab_bar->Tabs.Size == 1 && !(tab_bar->Flags & ImGuiTabBarFlags_AutoSelectNewTabs))
				tab_contents_visible = true;

		// Note that tab_is_new is not necessarily the same as tab_appearing! When a tab bar stops being submitted
		// and then gets submitted again, the tabs will have 'tab_appearing=true' but 'tab_is_new=false'.
		if (tab_appearing && (!tab_bar_appearing || tab_is_new))
		{
			ImGui::ItemAdd(ImRect(), id, NULL, ImGuiItemFlags_NoNav);
			if (is_tab_button)
				return false;
			return tab_contents_visible;
		}

		if (tab_bar->SelectedTabId == id)
			tab->LastFrameSelected = g.FrameCount;

		// Backup current layout position
		const ImVec2 backup_main_cursor_pos = window->DC.CursorPos;

		// Layout
		const bool is_central_section = (tab->Flags & ImGuiTabItemFlags_SectionMask_) == 0;
		size.x = tab->Width;
		if (is_central_section)
			window->DC.CursorPos = tab_bar->BarRect.Min + ImVec2(IM_FLOOR(tab->Offset - tab_bar->ScrollingAnim), 0.0f);
		else
			window->DC.CursorPos = tab_bar->BarRect.Min + ImVec2(tab->Offset, 0.0f);
		ImVec2 pos = window->DC.CursorPos;
		ImRect bb(pos, pos + size);

		// We don't have CPU clipping primitives to clip the CloseButton (until it becomes a texture), so need to add an extra draw call (temporary in the case of vertical animation)
		const bool want_clip_rect = is_central_section && (bb.Min.x < tab_bar->ScrollingRectMinX || bb.Max.x > tab_bar->ScrollingRectMaxX);
		if (want_clip_rect)
			ImGui::PushClipRect(ImVec2(ImMax(bb.Min.x, tab_bar->ScrollingRectMinX), bb.Min.y - 1), ImVec2(tab_bar->ScrollingRectMaxX, bb.Max.y), true);

		ImVec2 backup_cursor_max_pos = window->DC.CursorMaxPos;
		ImGui::ItemSize(bb.GetSize(), style.FramePadding.y);
		window->DC.CursorMaxPos = backup_cursor_max_pos;

		if (!ImGui::ItemAdd(bb, id))
		{
			if (want_clip_rect)
				ImGui::PopClipRect();
			window->DC.CursorPos = backup_main_cursor_pos;
			return tab_contents_visible;
		}

		// Click to Select a tab
		ImGuiButtonFlags button_flags = ((is_tab_button ? ImGuiButtonFlags_PressedOnClickRelease : ImGuiButtonFlags_PressedOnClick) | ImGuiButtonFlags_AllowItemOverlap);
		if (g.DragDropActive && !g.DragDropPayload.IsDataType(IMGUI_PAYLOAD_TYPE_WINDOW)) // FIXME: May be an opt-in property of the payload to disable this
			button_flags |= ImGuiButtonFlags_PressedOnDragDropHold;
		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, button_flags);
		if (pressed && !is_tab_button)
			ImGui::TabBarQueueFocus(tab_bar, tab);

		// Transfer active id window so the active id is not owned by the dock host (as StartMouseMovingWindow()
		// will only do it on the drag). This allows FocusWindow() to be more conservative in how it clears active id.
		if (held && docked_window && g.ActiveId == id && g.ActiveIdIsJustActivated)
			g.ActiveIdWindow = docked_window;

		// Allow the close button to overlap unless we are dragging (in which case we don't want any overlapping tabs to be hovered)
		if (g.ActiveId != id)
			ImGui::SetItemAllowOverlap();

		// Drag and drop a single floating window node moves it
		ImGuiDockNode* node = docked_window ? docked_window->DockNode : NULL;
		const bool single_floating_window_node = node && node->IsFloatingNode() && (node->Windows.Size == 1);
		if (held && single_floating_window_node && ImGui::IsMouseDragging(0, 0.0f))
		{
			// Move
			ImGui::StartMouseMovingWindow(docked_window);
		}
		else if (held && !tab_appearing && ImGui::IsMouseDragging(0))
		{
			// Drag and drop: re-order tabs
			int drag_dir = 0;
			float drag_distance_from_edge_x = 0.0f;
			if (!g.DragDropActive && ((tab_bar->Flags & ImGuiTabBarFlags_Reorderable) || (docked_window != NULL)))
			{
				// While moving a tab it will jump on the other side of the mouse, so we also test for MouseDelta.x
				if (g.IO.MouseDelta.x < 0.0f && g.IO.MousePos.x < bb.Min.x)
				{
					drag_dir = -1;
					drag_distance_from_edge_x = bb.Min.x - g.IO.MousePos.x;
					ImGui::TabBarQueueReorderFromMousePos(tab_bar, tab, g.IO.MousePos);
				}
				else if (g.IO.MouseDelta.x > 0.0f && g.IO.MousePos.x > bb.Max.x)
				{
					drag_dir = +1;
					drag_distance_from_edge_x = g.IO.MousePos.x - bb.Max.x;
					ImGui::TabBarQueueReorderFromMousePos(tab_bar, tab, g.IO.MousePos);
				}
			}

			// Extract a Dockable window out of it's tab bar
			if (docked_window != NULL && !(docked_window->Flags & ImGuiWindowFlags_NoMove))
			{
				// We use a variable threshold to distinguish dragging tabs within a tab bar and extracting them out of the tab bar
				bool undocking_tab = (g.DragDropActive && g.DragDropPayload.SourceId == id);
				if (!undocking_tab) //&& (!g.IO.ConfigDockingWithShift || g.IO.KeyShift)
				{
					float threshold_base = g.FontSize;
					float threshold_x = (threshold_base * 2.2f);
					float threshold_y = (threshold_base * 1.5f) + ImClamp((ImFabs(g.IO.MouseDragMaxDistanceAbs[0].x) - threshold_base * 2.0f) * 0.20f, 0.0f, threshold_base * 4.0f);
					//GetForegroundDrawList()->AddRect(ImVec2(bb.Min.x - threshold_x, bb.Min.y - threshold_y), ImVec2(bb.Max.x + threshold_x, bb.Max.y + threshold_y), IM_COL32_WHITE); // [DEBUG]

					float distance_from_edge_y = ImMax(bb.Min.y - g.IO.MousePos.y, g.IO.MousePos.y - bb.Max.y);
					if (distance_from_edge_y >= threshold_y)
						undocking_tab = true;
					if (drag_distance_from_edge_x > threshold_x)
						if ((drag_dir < 0 && ImGui::TabBarGetTabOrder(tab_bar, tab) == 0) || (drag_dir > 0 && ImGui::TabBarGetTabOrder(tab_bar, tab) == tab_bar->Tabs.Size - 1))
							undocking_tab = true;
				}

				if (undocking_tab)
				{
					// Undock
					// FIXME: refactor to share more code with e.g. StartMouseMovingWindow
					ImGui::DockContextQueueUndockWindow(&g, docked_window);
					g.MovingWindow = docked_window;
					ImGui::SetActiveID(g.MovingWindow->MoveId, g.MovingWindow);
					g.ActiveIdClickOffset -= g.MovingWindow->Pos - bb.Min;
					g.ActiveIdNoClearOnFocusLoss = true;
					ImGui::SetActiveIdUsingAllKeyboardKeys();
				}
			}
		}

#if 0
		if (hovered && g.HoveredIdNotActiveTimer > TOOLTIP_DELAY && bb.GetWidth() < tab->ContentWidth)
		{
			// Enlarge tab display when hovering
			bb.Max.x = bb.Min.x + IM_FLOOR(ImLerp(bb.GetWidth(), tab->ContentWidth, ImSaturate((g.HoveredIdNotActiveTimer - 0.40f) * 6.0f)));
			display_draw_list = GetForegroundDrawList(window);
			TabItemBackground(display_draw_list, bb, flags, GetColorU32(ImGuiCol_TitleBgActive));
		}
#endif

		// Render tab shape
		ImDrawList* display_draw_list = window->DrawList;
		const ImU32 tab_col = ImGui::GetColorU32((held || hovered) ? ImGuiCol_TabHovered : tab_contents_visible ? (tab_bar_focused ? ImGuiCol_TabActive : ImGuiCol_TabUnfocusedActive) : (tab_bar_focused ? ImGuiCol_Tab : ImGuiCol_TabUnfocused));
		ImGui::TabItemBackground(display_draw_list, bb, flags, tab_col);
		ImGui::RenderNavHighlight(bb, id);

		// Select with right mouse button. This is so the common idiom for context menu automatically highlight the current widget.
		const bool hovered_unblocked = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);
		if (hovered_unblocked && (ImGui::IsMouseClicked(1) || ImGui::IsMouseReleased(1)) && !is_tab_button)
			ImGui::TabBarQueueFocus(tab_bar, tab);

		if (tab_bar->Flags & ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)
			flags |= ImGuiTabItemFlags_NoCloseWithMiddleMouseButton;

		// Render tab label, process close button
		const ImGuiID close_button_id = p_open ? ImGui::GetIDWithSeed("#CLOSE", NULL, docked_window ? docked_window->ID : id) : 0;
		bool just_closed;
		bool text_clipped;
		ImGui::TabItemLabelAndCloseButton(display_draw_list, bb, tab_just_unsaved ? (flags & ~ImGuiTabItemFlags_UnsavedDocument) : flags, tab_bar->FramePadding, label, id, close_button_id, tab_contents_visible, &just_closed, &text_clipped);
		if (just_closed && p_open != NULL)
		{
			*p_open = false;
			ImGui::TabBarCloseTab(tab_bar, tab);
		}

		// Forward Hovered state so IsItemHovered() after Begin() can work (even though we are technically hovering our parent)
		// That state is copied to window->DockTabItemStatusFlags by our caller.
		if (docked_window && (hovered || g.HoveredId == close_button_id))
			g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;

		// Restore main window position so user can draw there
		if (want_clip_rect)
			ImGui::PopClipRect();
		window->DC.CursorPos = backup_main_cursor_pos;

		// Tooltip
		// (Won't work over the close button because ItemOverlap systems messes up with HoveredIdTimer-> seems ok)
		// (We test IsItemHovered() to discard e.g. when another item is active or drag and drop over the tab bar, which g.HoveredId ignores)
		// FIXME: This is a mess.
		// FIXME: We may want disabled tab to still display the tooltip?
		if (text_clipped && g.HoveredId == id && !held)
			if (!(tab_bar->Flags & ImGuiTabBarFlags_NoTooltip) && !(tab->Flags & ImGuiTabItemFlags_NoTooltip))
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
					ImGui::SetTooltip("%.*s", (int)(ImGui::FindRenderedTextEnd(label) - label), label);

		IM_ASSERT(!is_tab_button || !(tab_bar->SelectedTabId == tab->ID && is_tab_button)); // TabItemButton should not be selected
		if (is_tab_button)
			return pressed;
		return tab_contents_visible;
	}

	COREGUI_API bool BeginTabItem(const String& labelString, const Vec4& padding, bool* open, TabItemFlags flags)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		if (window->SkipItems)
			return false;

		const char* label = labelString.GetCString();

		ImGuiTabBar* tab_bar = g.CurrentTabBar;
		if (tab_bar == NULL)
		{
			IM_ASSERT_USER_ERROR(tab_bar, "Needs to be called between BeginTabBar() and EndTabBar()!");
			return false;
		}
		IM_ASSERT((flags & ImGuiTabItemFlags_Button) == 0);             // BeginTabItem() Can't be used with button flags, use TabItemButton() instead!

		bool ret = TabItemEx(tab_bar, label, open, flags, NULL, padding);
		if (ret && !(flags & ImGuiTabItemFlags_NoPushId))
		{
			ImGuiTabItem* tab = &tab_bar->Tabs[tab_bar->LastTabItemIdx];
			ImGui::PushOverrideID(tab->ID); // We already hashed 'label' so push into the ID stack directly instead of doing another hash through PushID(label)
		}
		return ret;
	}

	COREGUI_API void EndTabItem()
	{
		ImGui::EndTabItem();
	}

	COREGUI_API void EndTabBar()
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		if (window->SkipItems)
			return;

		ImGuiTabBar* tab_bar = g.CurrentTabBar;
		if (tab_bar == NULL)
		{
			IM_ASSERT_USER_ERROR(tab_bar != NULL, "Mismatched BeginTabBar()/EndTabBar()!");
			return;
		}

		// Fallback in case no TabItem have been submitted
		if (tab_bar->WantLayout)
			TabBarLayout(tab_bar);

		// Restore the last visible height if no tab is visible, this reduce vertical flicker/movement when a tabs gets removed without calling SetTabItemClosed().
		const bool tab_bar_appearing = (tab_bar->PrevFrameVisible + 1 < g.FrameCount);
		if (tab_bar->VisibleTabWasSubmitted || tab_bar->VisibleTabId == 0 || tab_bar_appearing)
		{
			tab_bar->CurrTabsContentsHeight = ImMax(window->DC.CursorPos.y - tab_bar->BarRect.Max.y, tab_bar->CurrTabsContentsHeight);
			window->DC.CursorPos.y = tab_bar->BarRect.Max.y + tab_bar->CurrTabsContentsHeight;
		}
		else
		{
			window->DC.CursorPos.y = tab_bar->BarRect.Max.y + tab_bar->PrevTabsContentsHeight;
		}
		if (tab_bar->BeginCount > 1)
			window->DC.CursorPos = tab_bar->BackupCursorPos;

		tab_bar->LastTabItemIdx = -1;
		if ((tab_bar->Flags & ImGuiTabBarFlags_DockNode) == 0)
			ImGui::PopID();

		g.CurrentTabBarStack.pop_back();
		g.CurrentTabBar = g.CurrentTabBarStack.empty() ? NULL : GetTabBarFromTabBarRef(g.CurrentTabBarStack.back());

		coordinateSpaceStack.Pop();
	}

#pragma endregion

#pragma region Table

	static const int TABLE_DRAW_CHANNEL_BG0 = 0;
	static const int TABLE_DRAW_CHANNEL_BG2_FROZEN = 1;
	static const int TABLE_DRAW_CHANNEL_NOCLIP = 2;                     // When using ImGuiTableFlags_NoClip (this becomes the last visible channel)
	static const float TABLE_BORDER_SIZE = 1.0f;    // FIXME-TABLE: Currently hard-coded because of clipping assumptions with outer borders rendering.
	static const float TABLE_RESIZE_SEPARATOR_HALF_THICKNESS = 4.0f;    // Extend outside inner borders.
	static const float TABLE_RESIZE_SEPARATOR_FEEDBACK_TIMER = 0.06f;   // Delay/timer before making the hover feedback (color+cursor) visible because tables/columns tends to be more cramped.


	inline ImGuiTableFlags TableFixFlags(ImGuiTableFlags flags, ImGuiWindow* outer_window)
	{
		// Adjust flags: set default sizing policy
		if ((flags & ImGuiTableFlags_SizingMask_) == 0)
			flags |= ((flags & ImGuiTableFlags_ScrollX) || (outer_window->Flags & ImGuiWindowFlags_AlwaysAutoResize)) ? ImGuiTableFlags_SizingFixedFit : ImGuiTableFlags_SizingStretchSame;

		// Adjust flags: enable NoKeepColumnsVisible when using ImGuiTableFlags_SizingFixedSame
		if ((flags & ImGuiTableFlags_SizingMask_) == ImGuiTableFlags_SizingFixedSame)
			flags |= ImGuiTableFlags_NoKeepColumnsVisible;

		// Adjust flags: enforce borders when resizable
		if (flags & ImGuiTableFlags_Resizable)
			flags |= ImGuiTableFlags_BordersInnerV;

		// Adjust flags: disable NoHostExtendX/NoHostExtendY if we have any scrolling going on
		if (flags & (ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY))
			flags &= ~(ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_NoHostExtendY);

		// Adjust flags: NoBordersInBodyUntilResize takes priority over NoBordersInBody
		if (flags & ImGuiTableFlags_NoBordersInBodyUntilResize)
			flags &= ~ImGuiTableFlags_NoBordersInBody;

		// Adjust flags: disable saved settings if there's nothing to save
		if ((flags & (ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Sortable)) == 0)
			flags |= ImGuiTableFlags_NoSavedSettings;

		// Inherit _NoSavedSettings from top-level window (child windows always have _NoSavedSettings set)
		if (outer_window->RootWindow->Flags & ImGuiWindowFlags_NoSavedSettings)
			flags |= ImGuiTableFlags_NoSavedSettings;

		return flags;
	}

	COREGUI_API bool BeginTable(const Rect& localRect, ID id, const String& label, int columnCount, TableFlags tableFlags)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* outer_window = ImGui::GetCurrentWindow();
		if (outer_window->SkipItems) // Consistent with other tables + beneficial side effect that assert on miscalling EndTable() will be more visible.
			return false;

		Rect rect = ToWindowCoordinateSpace(localRect);
		Vec2 rectSize = rect.max - rect.min;
		ImVec2 outer_size = ImVec2(rectSize.x, rectSize.y);

		f32 inner_width = rect.right - rect.left;
		int columns_count = columnCount;
		const char* name = label.GetCString();

		ImGuiTableFlags flags = (ImGuiTabBarFlags)tableFlags;

		// Sanity checks
		IM_ASSERT(columns_count > 0 && columns_count < IMGUI_TABLE_MAX_COLUMNS);
		if (flags & ImGuiTableFlags_ScrollX)
			IM_ASSERT(inner_width >= 0.0f);

		GUI::SetCursorPos(rect.min);

		// If an outer size is specified ahead we will be able to early out when not visible. Exact clipping rules may evolve.
		const bool use_child_window = (flags & (ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY)) != 0;
		const ImVec2 avail_size = ImGui::GetContentRegionAvail();
		ImVec2 actual_outer_size = ImVec2(rectSize.x, rectSize.y);
		//ImVec2 actual_outer_size = ImGui::CalcItemSize(outer_size, ImMax(avail_size.x, 1.0f), use_child_window ? ImMax(avail_size.y, 1.0f) : 0.0f);
		ImRect outer_rect(outer_window->DC.CursorPos, outer_window->DC.CursorPos + actual_outer_size);
		if (use_child_window && ImGui::IsClippedEx(outer_rect, 0))
		{
			ImGui::ItemSize(outer_rect);
			return false;
		}

		// Acquire storage for the table
		ImGuiTable* table = g.Tables.GetOrAddByKey(id);
		const ImGuiTableFlags table_last_flags = table->Flags;

		// Acquire temporary buffers
		const int table_idx = g.Tables.GetIndex(table);
		if (++g.TablesTempDataStacked > g.TablesTempData.Size)
			g.TablesTempData.resize(g.TablesTempDataStacked, ImGuiTableTempData());
		ImGuiTableTempData* temp_data = table->TempData = &g.TablesTempData[g.TablesTempDataStacked - 1];
		temp_data->TableIndex = table_idx;
		table->DrawSplitter = &table->TempData->DrawSplitter;
		table->DrawSplitter->Clear();

		// Fix flags
		table->IsDefaultSizingPolicy = (flags & ImGuiTableFlags_SizingMask_) == 0;
		flags = TableFixFlags(flags, outer_window);

		// Initialize
		const int instance_no = (table->LastFrameActive != g.FrameCount) ? 0 : table->InstanceCurrent + 1;
		table->ID = id;
		table->Flags = flags;
		table->LastFrameActive = g.FrameCount;
		table->OuterWindow = table->InnerWindow = outer_window;
		table->ColumnsCount = columns_count;
		table->IsLayoutLocked = false;
		table->InnerWidth = inner_width;
		temp_data->UserOuterSize = outer_size;

		// Instance data (for instance 0, TableID == TableInstanceID)
		ImGuiID instance_id;
		table->InstanceCurrent = (ImS16)instance_no;
		if (instance_no > 0)
		{
			IM_ASSERT(table->ColumnsCount == columns_count && "BeginTable(): Cannot change columns count mid-frame while preserving same ID");
			if (table->InstanceDataExtra.Size < instance_no)
				table->InstanceDataExtra.push_back(ImGuiTableInstanceData());
			instance_id = ImGui::GetIDWithSeed(instance_no, ImGui::GetIDWithSeed("##Instances", NULL, id)); // Push "##Instance" followed by (int)instance_no in ID stack.
		}
		else
		{
			instance_id = id;
		}
		ImGuiTableInstanceData* table_instance = ImGui::TableGetInstanceData(table, table->InstanceCurrent);
		table_instance->TableInstanceID = instance_id;

		// When not using a child window, WorkRect.Max will grow as we append contents.
		if (use_child_window)
		{
			// Ensure no vertical scrollbar appears if we only want horizontal one, to make flag consistent
			// (we have no other way to disable vertical scrollbar of a window while keeping the horizontal one showing)
			ImVec2 override_content_size(FLT_MAX, FLT_MAX);
			if ((flags & ImGuiTableFlags_ScrollX) && !(flags & ImGuiTableFlags_ScrollY))
				override_content_size.y = FLT_MIN;

			// Ensure specified width (when not specified, Stretched columns will act as if the width == OuterWidth and
			// never lead to any scrolling). We don't handle inner_width < 0.0f, we could potentially use it to right-align
			// based on the right side of the child window work rect, which would require knowing ahead if we are going to
			// have decoration taking horizontal spaces (typically a vertical scrollbar).
			if ((flags & ImGuiTableFlags_ScrollX) && inner_width > 0.0f)
				override_content_size.x = inner_width;

			if (override_content_size.x != FLT_MAX || override_content_size.y != FLT_MAX)
				ImGui::SetNextWindowContentSize(ImVec2(override_content_size.x != FLT_MAX ? override_content_size.x : 0.0f, override_content_size.y != FLT_MAX ? override_content_size.y : 0.0f));

			// Reset scroll if we are reactivating it
			if ((table_last_flags & (ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY)) == 0)
				ImGui::SetNextWindowScroll(ImVec2(0.0f, 0.0f));

			// Create scrolling region (without border and zero window padding)
			ImGuiWindowFlags child_flags = (flags & ImGuiTableFlags_ScrollX) ? ImGuiWindowFlags_HorizontalScrollbar : ImGuiWindowFlags_None;
			ImGui::BeginChildEx(name, instance_id, outer_rect.GetSize(), false, child_flags);
			table->InnerWindow = g.CurrentWindow;
			table->WorkRect = table->InnerWindow->WorkRect;
			table->OuterRect = table->InnerWindow->Rect();
			table->InnerRect = table->InnerWindow->InnerRect;
			IM_ASSERT(table->InnerWindow->WindowPadding.x == 0.0f && table->InnerWindow->WindowPadding.y == 0.0f && table->InnerWindow->WindowBorderSize == 0.0f);

			// When using multiple instances, ensure they have the same amount of horizontal decorations (aka vertical scrollbar) so stretched columns can be aligned)
			if (instance_no == 0)
			{
				table->HasScrollbarYPrev = table->HasScrollbarYCurr;
				table->HasScrollbarYCurr = false;
			}
			table->HasScrollbarYCurr |= (table->InnerWindow->ScrollMax.y > 0.0f);
		}
		else
		{
			// For non-scrolling tables, WorkRect == OuterRect == InnerRect.
			// But at this point we do NOT have a correct value for .Max.y (unless a height has been explicitly passed in). It will only be updated in EndTable().
			table->WorkRect = table->OuterRect = table->InnerRect = outer_rect;
		}

		// Push a standardized ID for both child-using and not-child-using tables
		ImGui::PushOverrideID(id);
		if (instance_no > 0)
			ImGui::PushOverrideID(instance_id); // FIXME: Somehow this is not resolved by stack-tool, even tho GetIDWithSeed() submitted the symbol.

		// Backup a copy of host window members we will modify
		ImGuiWindow* inner_window = table->InnerWindow;
		table->HostIndentX = inner_window->DC.Indent.x;
		table->HostClipRect = inner_window->ClipRect;
		table->HostSkipItems = inner_window->SkipItems;
		temp_data->HostBackupWorkRect = inner_window->WorkRect;
		temp_data->HostBackupParentWorkRect = inner_window->ParentWorkRect;
		temp_data->HostBackupColumnsOffset = outer_window->DC.ColumnsOffset;
		temp_data->HostBackupPrevLineSize = inner_window->DC.PrevLineSize;
		temp_data->HostBackupCurrLineSize = inner_window->DC.CurrLineSize;
		temp_data->HostBackupCursorMaxPos = inner_window->DC.CursorMaxPos;
		temp_data->HostBackupItemWidth = outer_window->DC.ItemWidth;
		temp_data->HostBackupItemWidthStackSize = outer_window->DC.ItemWidthStack.Size;
		inner_window->DC.PrevLineSize = inner_window->DC.CurrLineSize = ImVec2(0.0f, 0.0f);

		// Padding and Spacing
		// - None               ........Content..... Pad .....Content........
		// - PadOuter           | Pad ..Content..... Pad .....Content.. Pad |
		// - PadInner           ........Content.. Pad | Pad ..Content........
		// - PadOuter+PadInner  | Pad ..Content.. Pad | Pad ..Content.. Pad |
		const bool pad_outer_x = (flags & ImGuiTableFlags_NoPadOuterX) ? false : (flags & ImGuiTableFlags_PadOuterX) ? true : (flags & ImGuiTableFlags_BordersOuterV) != 0;
		const bool pad_inner_x = (flags & ImGuiTableFlags_NoPadInnerX) ? false : true;
		const float inner_spacing_for_border = (flags & ImGuiTableFlags_BordersInnerV) ? TABLE_BORDER_SIZE : 0.0f;
		const float inner_spacing_explicit = (pad_inner_x && (flags & ImGuiTableFlags_BordersInnerV) == 0) ? g.Style.CellPadding.x : 0.0f;
		const float inner_padding_explicit = (pad_inner_x && (flags & ImGuiTableFlags_BordersInnerV) != 0) ? g.Style.CellPadding.x : 0.0f;
		table->CellSpacingX1 = inner_spacing_explicit + inner_spacing_for_border;
		table->CellSpacingX2 = inner_spacing_explicit;
		table->CellPaddingX = inner_padding_explicit;
		table->CellPaddingY = g.Style.CellPadding.y;

		const float outer_padding_for_border = (flags & ImGuiTableFlags_BordersOuterV) ? TABLE_BORDER_SIZE : 0.0f;
		const float outer_padding_explicit = pad_outer_x ? g.Style.CellPadding.x : 0.0f;
		table->OuterPaddingX = (outer_padding_for_border + outer_padding_explicit) - table->CellPaddingX;

		table->CurrentColumn = -1;
		table->CurrentRow = -1;
		table->RowBgColorCounter = 0;
		table->LastRowFlags = ImGuiTableRowFlags_None;
		table->InnerClipRect = (inner_window == outer_window) ? table->WorkRect : inner_window->ClipRect;
		table->InnerClipRect.ClipWith(table->WorkRect);     // We need this to honor inner_width
		table->InnerClipRect.ClipWithFull(table->HostClipRect);
		table->InnerClipRect.Max.y = (flags & ImGuiTableFlags_NoHostExtendY) ? ImMin(table->InnerClipRect.Max.y, inner_window->WorkRect.Max.y) : inner_window->ClipRect.Max.y;

		table->RowPosY1 = table->RowPosY2 = table->WorkRect.Min.y; // This is needed somehow
		table->RowTextBaseline = 0.0f; // This will be cleared again by TableBeginRow()
		table->FreezeRowsRequest = table->FreezeRowsCount = 0; // This will be setup by TableSetupScrollFreeze(), if any
		table->FreezeColumnsRequest = table->FreezeColumnsCount = 0;
		table->IsUnfrozenRows = true;
		table->DeclColumnsCount = 0;

		// Using opaque colors facilitate overlapping elements of the grid
		table->BorderColorStrong = ImGui::GetColorU32(ImGuiCol_TableBorderStrong);
		table->BorderColorLight = ImGui::GetColorU32(ImGuiCol_TableBorderLight);

		// Make table current
		g.CurrentTable = table;
		outer_window->DC.NavIsScrollPushableX = false; // Shortcut for NavUpdateCurrentWindowIsScrollPushableX();
		outer_window->DC.CurrentTableIdx = table_idx;
		if (inner_window != outer_window) // So EndChild() within the inner window can restore the table properly.
			inner_window->DC.CurrentTableIdx = table_idx;

		if ((table_last_flags & ImGuiTableFlags_Reorderable) && (flags & ImGuiTableFlags_Reorderable) == 0)
			table->IsResetDisplayOrderRequest = true;

		// Mark as used
		if (table_idx >= g.TablesLastTimeActive.Size)
			g.TablesLastTimeActive.resize(table_idx + 1, -1.0f);
		g.TablesLastTimeActive[table_idx] = (float)g.Time;
		temp_data->LastTimeActive = (float)g.Time;
		table->MemoryCompacted = false;

		// Setup memory buffer (clear data if columns count changed)
		ImGuiTableColumn* old_columns_to_preserve = NULL;
		void* old_columns_raw_data = NULL;
		const int old_columns_count = table->Columns.size();
		if (old_columns_count != 0 && old_columns_count != columns_count)
		{
			// Attempt to preserve width on column count change (#4046)
			old_columns_to_preserve = table->Columns.Data;
			old_columns_raw_data = table->RawData;
			table->RawData = NULL;
		}
		if (table->RawData == NULL)
		{
			ImGui::TableBeginInitMemory(table, columns_count);
			table->IsInitializing = table->IsSettingsRequestLoad = true;
		}
		if (table->IsResetAllRequest)
			ImGui::TableResetSettings(table);
		if (table->IsInitializing)
		{
			// Initialize
			table->SettingsOffset = -1;
			table->IsSortSpecsDirty = true;
			table->InstanceInteracted = -1;
			table->ContextPopupColumn = -1;
			table->ReorderColumn = table->ResizedColumn = table->LastResizedColumn = -1;
			table->AutoFitSingleColumn = -1;
			table->HoveredColumnBody = table->HoveredColumnBorder = -1;
			for (int n = 0; n < columns_count; n++)
			{
				ImGuiTableColumn* column = &table->Columns[n];
				if (old_columns_to_preserve && n < old_columns_count)
				{
					// FIXME: We don't attempt to preserve column order in this path.
					*column = old_columns_to_preserve[n];
				}
				else
				{
					float width_auto = column->WidthAuto;
					*column = ImGuiTableColumn();
					column->WidthAuto = width_auto;
					column->IsPreserveWidthAuto = true; // Preserve WidthAuto when reinitializing a live table: not technically necessary but remove a visible flicker
					column->IsEnabled = column->IsUserEnabled = column->IsUserEnabledNextFrame = true;
				}
				column->DisplayOrder = table->DisplayOrderToIndex[n] = (ImGuiTableColumnIdx)n;
			}
		}
		if (old_columns_raw_data)
			IM_FREE(old_columns_raw_data);

		// Load settings
		if (table->IsSettingsRequestLoad)
			ImGui::TableLoadSettings(table);

		// Handle DPI/font resize
		// This is designed to facilitate DPI changes with the assumption that e.g. style.CellPadding has been scaled as well.
		// It will also react to changing fonts with mixed results. It doesn't need to be perfect but merely provide a decent transition.
		// FIXME-DPI: Provide consistent standards for reference size. Perhaps using g.CurrentDpiScale would be more self explanatory.
		// This is will lead us to non-rounded WidthRequest in columns, which should work but is a poorly tested path.
		const float new_ref_scale_unit = g.FontSize; // g.Font->GetCharAdvance('A') ?
		if (table->RefScale != 0.0f && table->RefScale != new_ref_scale_unit)
		{
			const float scale_factor = new_ref_scale_unit / table->RefScale;
			//IMGUI_DEBUG_PRINT("[table] %08X RefScaleUnit %.3f -> %.3f, scaling width by %.3f\n", table->ID, table->RefScaleUnit, new_ref_scale_unit, scale_factor);
			for (int n = 0; n < columns_count; n++)
				table->Columns[n].WidthRequest = table->Columns[n].WidthRequest * scale_factor;
		}
		table->RefScale = new_ref_scale_unit;

		// Disable output until user calls TableNextRow() or TableNextColumn() leading to the TableUpdateLayout() call..
		// This is not strictly necessary but will reduce cases were "out of table" output will be misleading to the user.
		// Because we cannot safely assert in EndTable() when no rows have been created, this seems like our best option.
		inner_window->SkipItems = true;

		// Clear names
		// At this point the ->NameOffset field of each column will be invalid until TableUpdateLayout() or the first call to TableSetupColumn()
		if (table->ColumnsNames.Buf.Size > 0)
			table->ColumnsNames.Buf.resize(0);

		// Apply queued resizing/reordering/hiding requests
		ImGui::TableBeginApplyRequests(table);

		return true;
	}

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

	COREGUI_API void Columns(const Rect& localRect, int count, bool border)
	{
		auto rect = ToWindowCoordinateSpace(localRect);

		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		IM_ASSERT(count >= 1);

		ImGuiOldColumnFlags flags = (border ? 0 : ImGuiOldColumnFlags_NoBorder);
		//flags |= ImGuiOldColumnFlags_NoPreserveWidths; // NB: Legacy behavior
		ImGuiOldColumns* columns = window->DC.CurrentColumns;
		if (columns != NULL && columns->Count == count && columns->Flags == flags)
			return;

		if (columns != NULL)
		{
			ImGui::EndColumns();
		}

		GUI::SetCursorPos(rect.min);

		ImRect frame{ rect.x, rect.y, rect.w, rect.z };

		int columns_count = count;

		if (count != 1)
		{
			//ImGui::BeginColumns(NULL, count, flags);

			IM_ASSERT(columns_count >= 1);
			IM_ASSERT(window->DC.CurrentColumns == NULL);   // Nested columns are currently not supported

			// Acquire storage for the columns set
			ImGuiID id = ImGui::GetColumnsID(NULL, columns_count);
			ImGuiOldColumns* columns = ImGui::FindOrCreateColumns(window, id);
			IM_ASSERT(columns->ID == id);
			columns->Current = 0;
			columns->Count = columns_count;
			columns->Flags = flags;
			window->DC.CurrentColumns = columns;
			window->DC.NavIsScrollPushableX = false; // Shortcut for NavUpdateCurrentWindowIsScrollPushableX();

			columns->HostCursorPosY = window->DC.CursorPos.y;
			columns->HostCursorMaxPosX = window->DC.CursorMaxPos.x;
			columns->HostInitialClipRect = frame;//window->ClipRect;
			columns->HostBackupParentWorkRect = window->ParentWorkRect;
			window->ParentWorkRect = frame;//window->WorkRect;

			// Set state for first column
			// We aim so that the right-most column will have the same clipping width as other after being clipped by parent ClipRect
			const float column_padding = g.Style.ItemSpacing.x;
			const float half_clip_extend_x = ImFloor(ImMax(window->WindowPadding.x * 0.5f, window->WindowBorderSize));
			const float max_1 = window->WorkRect.Max.x + column_padding - ImMax(column_padding - window->WindowPadding.x, 0.0f);
			const float max_2 = window->WorkRect.Max.x + half_clip_extend_x;
			columns->OffMinX = window->DC.Indent.x - column_padding + ImMax(column_padding - window->WindowPadding.x, 0.0f);
			columns->OffMaxX = ImMax(ImMin(max_1, max_2) - window->Pos.x, columns->OffMinX + 1.0f);
			columns->LineMinY = columns->LineMaxY = window->DC.CursorPos.y;

			// Clear data if columns count changed
			if (columns->Columns.Size != 0 && columns->Columns.Size != columns_count + 1)
				columns->Columns.resize(0);

			// Initialize default widths
			columns->IsFirstFrame = (columns->Columns.Size == 0);
			if (columns->Columns.Size == 0)
			{
				columns->Columns.reserve(columns_count + 1);
				for (int n = 0; n < columns_count + 1; n++)
				{
					ImGuiOldColumnData column;
					column.OffsetNorm = n / (float)columns_count;
					columns->Columns.push_back(column);
				}
			}

			for (int n = 0; n < columns_count; n++)
			{
				// Compute clipping rectangle
				ImGuiOldColumnData* column = &columns->Columns[n];
				float clip_x1 = IM_ROUND(window->Pos.x + ImGui::GetColumnOffset(n));
				float clip_x2 = IM_ROUND(window->Pos.x + ImGui::GetColumnOffset(n + 1) - 1.0f);
				column->ClipRect = ImRect(clip_x1, -FLT_MAX, clip_x2, +FLT_MAX);
				column->ClipRect.ClipWithFull(window->ClipRect);
			}

			if (columns->Count > 1)
			{
				columns->Splitter.Split(window->DrawList, 1 + columns->Count);
				columns->Splitter.SetCurrentChannel(window->DrawList, 1);
				ImGui::PushColumnClipRect(0);
			}

			// We don't generally store Indent.x inside ColumnsOffset because it may be manipulated by the user.
			float offset_0 = ImGui::GetColumnOffset(columns->Current);
			float offset_1 = ImGui::GetColumnOffset(columns->Current + 1);
			float width = offset_1 - offset_0;
			ImGui::PushItemWidth(width * 0.65f);
			window->DC.ColumnsOffset.x = ImMax(column_padding - window->WindowPadding.x, 0.0f);
			window->DC.CursorPos.x = IM_FLOOR(window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x);
			window->WorkRect.Max.x = window->Pos.x + offset_1 - column_padding;

		}
	}

	COREGUI_API void ColumnsNext()
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems || window->DC.CurrentColumns == NULL)
			return;

		ImGuiContext& g = *GImGui;
		ImGuiOldColumns* columns = window->DC.CurrentColumns;

		if (columns->Count == 1)
		{
			window->DC.CursorPos.x = IM_FLOOR(window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x);
			IM_ASSERT(columns->Current == 0);
			return;
		}

		// Next column
		if (++columns->Current == columns->Count)
			columns->Current = 0;

		ImGui::PopItemWidth();

		// Optimization: avoid PopClipRect() + SetCurrentChannel() + PushClipRect()
		// (which would needlessly attempt to update commands in the wrong channel, then pop or overwrite them),
		ImGuiOldColumnData* column = &columns->Columns[columns->Current];
		ImGui::SetWindowClipRectBeforeSetChannel(window, column->ClipRect);
		columns->Splitter.SetCurrentChannel(window->DrawList, columns->Current + 1);

		const float column_padding = g.Style.ItemSpacing.x;
		columns->LineMaxY = ImMax(columns->LineMaxY, window->DC.CursorPos.y);
		if (columns->Current > 0)
		{
			// Columns 1+ ignore IndentX (by canceling it out)
			// FIXME-COLUMNS: Unnecessary, could be locked?
			window->DC.ColumnsOffset.x = ImGui::GetColumnOffset(columns->Current) - window->DC.Indent.x + column_padding;
		}
		else
		{
			// New row/line: column 0 honor IndentX.
			window->DC.ColumnsOffset.x = ImMax(column_padding - window->WindowPadding.x, 0.0f);
			window->DC.IsSameLine = false;
			columns->LineMinY = columns->LineMaxY;
		}
		window->DC.CursorPos.x = IM_FLOOR(window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x);
		window->DC.CursorPos.y = columns->LineMinY;
		window->DC.CurrLineSize = ImVec2(0.0f, 0.0f);
		window->DC.CurrLineTextBaseOffset = 0.0f;

		// FIXME-COLUMNS: Share code with BeginColumns() - move code on columns setup.
		float offset_0 = ImGui::GetColumnOffset(columns->Current);
		float offset_1 = ImGui::GetColumnOffset(columns->Current + 1);
		float width = offset_1 - offset_0;
		ImGui::PushItemWidth(width * 0.65f);
		window->WorkRect.Max.x = window->Pos.x + offset_1 - column_padding;
	}

#pragma endregion

#pragma region Layout

	static Rect ToParentCoordinateSpace(const Rect& localSpaceRect, const Rect& parentSpace)
	{
		auto originalSize = localSpaceRect.max - localSpaceRect.min;

		return Rect(parentSpace.min + localSpaceRect.min, parentSpace.min + localSpaceRect.min + originalSize);
	}

	COREGUI_API Rect ToWindowCoordinateSpace(const Rect& localSpaceRect)
	{
		if (coordinateSpaceStack.IsEmpty())
			return localSpaceRect;

		auto rect = localSpaceRect;

		for (int i = coordinateSpaceStack.GetSize() - 1; i >= 0; i--)
		{
			rect = ToParentCoordinateSpace(rect, coordinateSpaceStack[i]);
		}

		return rect;
	}

	COREGUI_API void PushChildCoordinateSpace(const Rect& rect, const Vec4& padding)
	{
		Rect contentSpaceRect = rect;
		//contentSpaceRect.min += padding.min;
		//contentSpaceRect.max -= padding.max;
		coordinateSpaceStack.Push(contentSpaceRect);
	}

	COREGUI_API void PushZeroingChildCoordinateSpace()
	{
		Rect rect = {};

		for (int i = coordinateSpaceStack.GetSize() - 1; i >= 0; i--)
		{
			rect -= coordinateSpaceStack[i];
		}

		coordinateSpaceStack.Push(rect);
	}

	COREGUI_API void PopChildCoordinateSpace()
	{
		coordinateSpaceStack.Pop();
	}

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
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		g.GroupStack.resize(g.GroupStack.Size + 1);
		ImGuiGroupData& group_data = g.GroupStack.back();
		group_data.WindowID = window->ID;
		group_data.BackupCursorPos = window->DC.CursorPos;
		group_data.BackupCursorMaxPos = window->DC.CursorMaxPos;
		group_data.BackupIndent = window->DC.Indent;
		group_data.BackupGroupOffset = window->DC.GroupOffset;
		group_data.BackupCurrLineSize = window->DC.CurrLineSize;
		group_data.BackupCurrLineTextBaseOffset = window->DC.CurrLineTextBaseOffset;
		group_data.BackupActiveIdIsAlive = g.ActiveIdIsAlive;
		group_data.BackupHoveredIdIsAlive = g.HoveredId != 0;
		group_data.BackupActiveIdPreviousFrameIsAlive = g.ActiveIdPreviousFrameIsAlive;
		group_data.EmitItem = true;

		window->DC.GroupOffset.x = window->DC.CursorPos.x - window->Pos.x - window->DC.ColumnsOffset.x;
		window->DC.Indent = window->DC.GroupOffset;
		window->DC.CursorMaxPos = window->DC.CursorPos;
		window->DC.CurrLineSize = ImVec2(0.0f, 0.0f);
		if (g.LogEnabled)
			g.LogLinePosY = -FLT_MAX; // To enforce a carriage return
	}

	COREGUI_API void EndGroup(const Vec4& padding)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		IM_ASSERT(g.GroupStack.Size > 0); // Mismatched BeginGroup()/EndGroup() calls

		ImGuiGroupData& group_data = g.GroupStack.back();
		IM_ASSERT(group_data.WindowID == window->ID); // EndGroup() in wrong window?

		if (window->DC.IsSetPos)
			ImGui::ErrorCheckUsingSetCursorPosToExtendParentBoundaries();

		ImRect group_bb(group_data.BackupCursorPos, 
			ImMax(window->DC.CursorMaxPos, group_data.BackupCursorPos) - ImVec2(padding.right, padding.bottom));

		window->DC.CursorPos = group_data.BackupCursorPos ;
		window->DC.CursorMaxPos = ImMax(group_data.BackupCursorMaxPos, window->DC.CursorMaxPos);
		window->DC.Indent = group_data.BackupIndent;
		window->DC.GroupOffset = group_data.BackupGroupOffset;
		window->DC.CurrLineSize = group_data.BackupCurrLineSize;
		window->DC.CurrLineTextBaseOffset = group_data.BackupCurrLineTextBaseOffset;
		if (g.LogEnabled)
			g.LogLinePosY = -FLT_MAX; // To enforce a carriage return

		if (!group_data.EmitItem)
		{
			g.GroupStack.pop_back();
			return;
		}

		window->DC.CurrLineTextBaseOffset = ImMax(window->DC.PrevLineTextBaseOffset, group_data.BackupCurrLineTextBaseOffset);      // FIXME: Incorrect, we should grab the base offset from the *first line* of the group but it is hard to obtain now.
		ImGui::ItemSize(group_bb.GetSize());
		ImGui::ItemAdd(group_bb, 0, NULL, ImGuiItemFlags_NoTabStop);

		// If the current ActiveId was declared within the boundary of our group, we copy it to LastItemId so IsItemActive(), IsItemDeactivated() etc. will be functional on the entire group.
		// It would be neater if we replaced window.DC.LastItemId by e.g. 'bool LastItemIsActive', but would put a little more burden on individual widgets.
		// Also if you grep for LastItemId you'll notice it is only used in that context.
		// (The two tests not the same because ActiveIdIsAlive is an ID itself, in order to be able to handle ActiveId being overwritten during the frame.)
		const bool group_contains_curr_active_id = (group_data.BackupActiveIdIsAlive != g.ActiveId) && (g.ActiveIdIsAlive == g.ActiveId) && g.ActiveId;
		const bool group_contains_prev_active_id = (group_data.BackupActiveIdPreviousFrameIsAlive == false) && (g.ActiveIdPreviousFrameIsAlive == true);
		if (group_contains_curr_active_id)
			g.LastItemData.ID = g.ActiveId;
		else if (group_contains_prev_active_id)
			g.LastItemData.ID = g.ActiveIdPreviousFrame;
		g.LastItemData.Rect = group_bb;

		// Forward Hovered flag
		const bool group_contains_curr_hovered_id = (group_data.BackupHoveredIdIsAlive == false) && g.HoveredId != 0;
		if (group_contains_curr_hovered_id)
			g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;

		// Forward Edited flag
		if (group_contains_curr_active_id && g.ActiveIdHasBeenEditedThisFrame)
			g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_Edited;

		// Forward Deactivated flag
		g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HasDeactivated;
		if (group_contains_prev_active_id && g.ActiveId != g.ActiveIdPreviousFrame)
			g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_Deactivated;

		g.GroupStack.pop_back();
	}

    COREGUI_API Vec2 CalculateTextSize(const char* text)
    {
        ImVec2 size = ImGui::CalcTextSize(text, 0, true);
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

	COREGUI_API Vec4 GetDefaultFramePadding()
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

	COREGUI_API bool IsWindowFocused(ID id, FocusFlags flags)
	{
		if (GImGui->WindowsFocusOrder.Size == 0)
			return ImGui::IsWindowFocused((int)flags);
		return id == GImGui->WindowsFocusOrder[GImGui->WindowsFocusOrder.Size - 1]->ID && ImGui::IsWindowFocused((int)flags);
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

	COREGUI_API bool PushClipRectInWindow(const Rect& rectInWindow, bool intersectWithCurrentClipRect)
	{
		if (isnan(rectInWindow.z) || isnan(rectInWindow.w))
			return false;
		auto rect = WindowRectToGlobalRect(rectInWindow);
		ImGui::PushClipRect(ImVec2(rect.min.x, rect.min.y), ImVec2(rect.max.x, rect.max.y), intersectWithCurrentClipRect);
		return true;
	}

	COREGUI_API void PopClipRect()
	{
		ImGui::PopClipRect();
	}

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
		drawList->PathLineTo(ImVec2(rect.x, rect.y + roundingTL));

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

	COREGUI_API void FillCheckMark(const Vec2& pos, const Color& color, f32 size)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		if (drawList == nullptr)
			return;
		
		ImGui::RenderCheckMark(drawList, ImVec2(pos.x, pos.y), color.ToU32(), size);
	}

	COREGUI_API void FillArrow(const Vec2& pos, const Color& color, GUI::Dir direction, f32 size)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		if (drawList == nullptr)
			return;

		ImGui::RenderArrow(drawList, ImVec2(pos.y, pos.y), color.ToU32(), (ImGuiDir)direction, size);
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

	COREGUI_API void RenderFrame(const Vec4& rect, const Color& color, const Color& borderColor, f32 borderSize, Vec4 rounding)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		FillRect(rect, color, rounding);

		ImVec2 p_min = ImVec2(rect.x, rect.y);
		ImVec2 p_max = ImVec2(rect.z, rect.w);

		if (borderSize > 0.1f)
		{
			DrawRect(rect + Vec4(1, 1, 1, 1), Color(borderColor.r, borderColor.g, borderColor.b, borderColor.a / 2.f), rounding, borderSize);
			DrawRect(rect, borderColor.ToU32(), rounding, borderSize);
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

