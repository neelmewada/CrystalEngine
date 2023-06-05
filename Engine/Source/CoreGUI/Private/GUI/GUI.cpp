
#include "CoreGUI.h"

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

	COREGUI_API bool IsWindowFocused(FocusFlags flags)
	{
		return ImGui::IsWindowFocused((int)flags);
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

#pragma endregion

} // namespace CE

