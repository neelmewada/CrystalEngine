#pragma once

#include "GUIDefinitions.h"

namespace CE::GUI
{
	COREGUI_API ID GetID(const char* strId);

	COREGUI_API ID GetID(const String& strId);

	COREGUI_API bool AreSettingsLoaded();

    COREGUI_API Vec2 GetCursorPos();
    COREGUI_API void SetCursorPos(const Vec2& pos);
    COREGUI_API Vec2 GetCursorScreenPos();

	COREGUI_API Vec2 GetWindowPos();
	COREGUI_API Vec2 GetWindowSize();
	COREGUI_API Vec4 GetWindowRect();
	COREGUI_API Vec2 GetWindowScroll();

	COREGUI_API void SetWindowFontScale(f32 scale);
	COREGUI_API f32 GetWindowFontScale();

	COREGUI_API void PushFont(void* fontHandle);
	COREGUI_API void PopFont();
	COREGUI_API void SetCurrentFont(void* fontHandle);
	COREGUI_API f32 GetFontSize();

	COREGUI_API Viewport* GetCurrentViewport();
	COREGUI_API Viewport* GetMainViewport();

	COREGUI_API void SetWindowToolBarHeight(float height);
	COREGUI_API float GetWindowToolBarHeight();

	/// Converts a rect from window space to global space
	COREGUI_API Vec4 WindowRectToGlobalRect(const Vec4& rectInWindow);
	/// Converts a rect from global space to window space
	COREGUI_API Vec4 GlobalRectToWindowRect(const Vec4& globalRect);

	COREGUI_API void SetNextWindowPos(const Vec2& pos, Cond condition = Cond::None, const Vec2& pivot = Vec2(0, 0));

	COREGUI_API void SetNextWindowSize(const Vec2& size, Cond condition = Cond::None);

	COREGUI_API void SetNextWindowViewport(ID viewportId);

    COREGUI_API bool BeginWindow(const String& name, bool* isShown, WindowFlags flags = WF_None);

    COREGUI_API ID DockSpace(const String& id, Vec2 size = Vec2(0, 0), DockFlags dockFlags = DockFlags_None);

	COREGUI_API ID DockSpace(ID id, Vec2 size = Vec2(0, 0), DockFlags dockFlags = DockFlags_None);

    COREGUI_API void EndWindow();

    COREGUI_API bool BeginMenuBar();

    COREGUI_API void EndMenuBar();

    COREGUI_API bool BeginMenu(const String& label, bool enabled = true, const char* icon = nullptr);

    COREGUI_API bool MenuItem(const String& label, const String& shortcut = "", bool selected = false, bool enabled = true);

    COREGUI_API bool MenuItem(const String& label, const String& shortcut, bool* selected, bool enabled = true);

    COREGUI_API void EndMenu();

	COREGUI_API bool BeginChild(const String& name, ID id, const Vec2& size = {}, f32 borderThickness = 0, 
		const Vec2& minSize = {}, const Vec2& maxSize = {}, const Vec4& padding = {}, const Color& background = {}, WindowFlags flags = WF_None);

	COREGUI_API void EndChild();

	COREGUI_API bool BeginChild(const Rect& rect, ID id, const String& title, const Vec4& padding = {}, const GuiStyleState& styleState = {}, WindowFlags flags = WF_None);

	extern COREGUI_API Array<Vec2> gPaddingXStack;

#pragma region DockBuilder

	COREGUI_API void DockBuilderRemoveNode(ID nodeId);
	COREGUI_API void DockBuilderAddNode(ID nodeId);
	COREGUI_API void DockBuilderSetNodePos(ID nodeId, Vec2 pos);
	COREGUI_API void DockBuilderSetNodeSize(ID nodeId, Vec2 size);
	COREGUI_API ID   DockBuilderSplitNode(ID nodeId, GUI::Dir split_dir, float size_ratio_for_node_at_dir, ID* out_id_at_dir, ID* out_id_at_opposite_dir); // Create 2 child nodes in this parent node.
	COREGUI_API void DockBuilderDockWindow(const String& windowName, ID nodeId);
	COREGUI_API void DockBuilderFinish(ID nodeId);

#pragma endregion

#pragma region Style

	COREGUI_API GUI::Style& GetStyle();

    COREGUI_API bool IsStyleVarOfVectorType(StyleVar var);

    COREGUI_API void PushStyleVar(StyleVar var, f32 value);

    COREGUI_API void PushStyleVar(StyleVar var, Vec2 value);

    COREGUI_API void PopStyleVar(int count = 1);

    COREGUI_API void PushStyleColor(StyleCol color, const Color& value);

    COREGUI_API void PopStyleColor(int count = 1);

	COREGUI_API void BeginDisabled(bool disabled = true);
	COREGUI_API void EndDisabled();

#pragma endregion


#pragma region Widgets

	COREGUI_API void ShowDemoWindow(bool* open = nullptr);

	COREGUI_API void Image(const Rect& localRect, GuiTextureID textureId, const GuiStyleState& style,
		const Vec2& uvMin = Vec2(0, 0), const Vec2& uvMax = Vec2(1, 1));

	COREGUI_API void Text(const Rect& localRect, const String& text, const GuiStyleState& style);
	COREGUI_API void Text(const Rect& localRect, const char* text, const GuiStyleState& style);
	COREGUI_API void TextWrapped(const Rect& localRect, const String& text, const GuiStyleState& style);
	COREGUI_API void TextWrapped(const Rect& localRect, const char* text, const GuiStyleState& style);

	COREGUI_API void Text(const char* text, const Vec2& size = {}, TextAlign align = TextAlign_MiddleCenter);
	COREGUI_API void Text(const String& text, const Vec2& size = {}, TextAlign align = TextAlign_MiddleCenter);

	COREGUI_API void TextColored(const char* text, const Color& color);
	COREGUI_API void TextColored(const String& text, const Color& color);

	COREGUI_API bool Button(const Rect& localRect, const String& label,
		const GuiStyleState& style, bool& isHovered, bool& isHeld, const Vec4& padding = {}, ButtonFlags flags = ButtonFlags::None);

    COREGUI_API bool Button(const String& label, const Vec2& size = Vec2(0, 0), ButtonFlags flags = ButtonFlags::None);

	COREGUI_API bool ButtonEx(const String& label, const Vec4& padding = {}, const Vec2& size = Vec2(0, 0), const Vec4& rounding = {},
		TextAlign textAlign = TextAlign_Inherited, Vec2 minSize = {}, Vec2 maxSize = {}, ButtonFlags flags = ButtonFlags::None);

	COREGUI_API bool ButtonEx(const String& label, const StyleColor& normal, const StyleColor& hovered, const StyleColor& pressed, 
		const Vec4& padding = {}, const Vec2& size = Vec2(0, 0), const Vec4& rounding = {},
		TextAlign textAlign = TextAlign_Inherited, Vec2 minSize = {}, Vec2 maxSize = {}, ButtonFlags flags = ButtonFlags::None);

	COREGUI_API bool SelectableEx(ID id, bool selected, SelectableFlags flags, const Vec2& size = {},
		f32 borderThickness = 0, const Color& hoveredCol = {}, const Color& pressedCol = {}, const Color& activeCol = {},
		const Color& hoveredBorderCol = {}, const Color& pressedBorderCol = {}, const Color& activeBorderCol = {});

	COREGUI_API bool Selectable(const Rect& localRect, ID id, bool selected, 
		const GuiStyleState& hoveredState, const GuiStyleState& pressedState, const GuiStyleState& selectedState,
		SelectableFlags flags = SelectableFlags_None);

    COREGUI_API void InvisibleButton(const String& id, const Vec2& size);
	COREGUI_API bool InvisibleButton(const Rect& rect, ID id, GUI::ButtonFlags flags = GUI::ButtonFlags::None);

	COREGUI_API bool Checkbox(const String& label, bool* value);

	COREGUI_API bool CheckboxTriState(ID id, s8* value, const Vec4& padding = {}, const Vec4& rounding = {}, f32 borderThickness = 0);

	COREGUI_API bool InputText(const Rect& rect, ID id, const String& hint, String& inputText, 
		const GuiStyleState& normalState, const Color& hintColor, const Vec4& padding = {},
		TextInputFlags flags = TextInputFlags_None, TextInputCallback callback = nullptr, void* callback_user_data = nullptr);

	COREGUI_API bool InputTextEx(ID id, const String& hint, String& inputText,
		const Vec2& size = {}, const Vec4& padding = {}, const Vec4& rounding = {}, f32 borderThickness = 0, Vec2 minSize = {}, Vec2 maxSize = {},
		TextInputFlags flags = TextInputFlags_None, TextInputCallback callback = nullptr, void* callback_user_data = nullptr);

	COREGUI_API bool TempInputText(const Vec4& rect, ID id, String& text, TextInputFlags flags,
		const Vec2& size = {}, const Vec4& padding = {}, const Vec4& rounding = {}, f32 borderThickness = 0);

	COREGUI_API bool TempInputScalar(const Vec4& rect, ID id, DataType data_type, void* data, const char* format, const void* clampMin, const void* clampMax,
		const Vec2& size = {}, const Vec4& padding = {}, const Vec4& rounding = {}, f32 borderThickness = 0);

	COREGUI_API bool DragScalar(ID id, DataType dataType, void* data, float speed = 1.0f, 
		const void* min = nullptr, const void* max = nullptr, const char* format = nullptr, 
		const Vec2& size = {}, const Vec4& padding = {}, const Vec4& rounding = {}, f32 borderThickness = 0,
		SliderFlags flags = SliderFlags_None);

	COREGUI_API bool DragScalar(const Rect& rect, ID id, DataType dataType, void* data, float speed = 1.0f,
		const void* min = nullptr, const void* max = nullptr, const char* format = nullptr,
		GuiStyleState normalState = {}, GuiStyleState activeState = {},
		SliderFlags flags = SliderFlags_None);

	// Tree Node

	COREGUI_API bool TreeNode(const String& label, TreeNodeFlags flags = TNF_None);
	COREGUI_API bool TableTreeNode(ID id, TreeNodeFlags flags = TNF_None);

	COREGUI_API void TreePop();

    COREGUI_API bool CollapsibleHeader(const String& label, Vec4* padding = nullptr, Vec4* borderRadius = nullptr, f32* borderSize = nullptr, TreeNodeFlags flags = TNF_None);

	COREGUI_API bool CollapsibleHeader(const Rect& localRect, const String& label, ID id, const GuiStyleState& style, bool& hovered, bool& held,
		const Vec4& headerPadding = {}, const Vec4& contentPadding = {}, TreeNodeFlags flags = TNF_None);

	// Tab Bar

	COREGUI_API bool BeginTabBar(const Rect& rect, ID id, const Vec4& tabBarPadding = {}, TabBarFlags flags = TabBarFlags_None);

	COREGUI_API bool BeginTabBar(const String& id, TabBarFlags flags = TabBarFlags_None);

	COREGUI_API bool BeginTabItem(const String& label, const Vec4& padding = {}, bool* open = nullptr, TabItemFlags flags = TabItemFlags_None);

	COREGUI_API void EndTabItem();

	COREGUI_API void EndTabBar();

#pragma endregion

#pragma region Tree View

	COREGUI_API bool BeginTreeView(const Rect& localRect, ID id, const String& label, TreeViewFlags flags = TVF_None);
	COREGUI_API void EndTreeView();

	COREGUI_API void TreeViewNodeSetOpen(ID id, bool open);

	COREGUI_API bool TreeViewNode(const Vec2& size, ID id, f32 indentX, const Vec4& padding = {}, TreeNodeFlags flags = TNF_None);
	COREGUI_API bool TreeViewNodeSelectable(const Vec2& size, ID id, f32 indentX, bool* isSelected, bool* isHovered = nullptr, bool* isHeld = nullptr,
		const Vec4& padding = {}, TreeNodeFlags flags = TNF_None);

	COREGUI_API void TreeViewNodePop();

#pragma endregion

#pragma region Table

	COREGUI_API bool BeginTable(const Rect& localRect, ID id, const String& label, int columnCount, TableFlags flags = TableFlags_None);
	COREGUI_API bool BeginTable(const String& label, int columnCount, TableFlags flags = TableFlags_None, const Vec2& outerSize = {0, 0}, f32 innerWidth = 0);

	COREGUI_API void EndTable();

	COREGUI_API void TableSetupColumn(const String& label, TableColumnFlags flags = TableColumnFlags_None);
	COREGUI_API void TableHeadersRow();

	COREGUI_API void TableNextRow(TableRowFlags flags = TableRowFlags_None, f32 minHeight = 0);
	COREGUI_API bool TableNextColumn();

	COREGUI_API Vec4 GetTableClipRect();

	COREGUI_API void Columns(const Rect& localRect, int count, bool border = true);
	COREGUI_API void ColumnsNext();

#pragma endregion

#pragma region Popup / Menu

	COREGUI_API void OpenPopup(ID id, PopupFlags flags = PopupFlags_None);
	COREGUI_API bool IsPopupOpen(ID id, PopupFlags flags = PopupFlags_None);
	COREGUI_API void CloseCurrentPopup();

	COREGUI_API bool BeginPopup(const String& title, ID id, WindowFlags flags = WF_None);
	COREGUI_API void EndPopup();

	COREGUI_API bool BeginModalPopup(const String& title, ID id, bool* isShown, WindowFlags flags = WF_None);
	COREGUI_API void EndModalPopup();

#pragma endregion

#pragma region Layout

	COREGUI_API Rect ToWindowCoordinateSpace(const Rect& localRect);

	COREGUI_API void PushChildCoordinateSpace(const Rect& rect, const Vec4& padding = {});
	COREGUI_API void PushZeroingChildCoordinateSpace();

	COREGUI_API void PopChildCoordinateSpace();

	COREGUI_API void Spacing();

	COREGUI_API void SameLine(f32 offsetFromStart = 0, f32 spacing = -1.0f);

	COREGUI_API void BeginGroup();
	COREGUI_API void EndGroup(const Vec4& padding = {});

    COREGUI_API Vec2 CalculateTextSize(const char* text, float wrapWidth = -1.0f);
    COREGUI_API Vec2 CalculateTextSize(const String& text, float wrapWidth = -1.0f);

    COREGUI_API Vec2 GetItemRectSize();

	COREGUI_API Vec2 GetContentRegionAvailableSpace();

	COREGUI_API f32 GetTextLineHeight();

	COREGUI_API f32 GetTextLineHeightWithSpacing();

    COREGUI_API void Indent(f32 indent = 0);
    COREGUI_API void Unindent(f32 indent = 0);

	COREGUI_API Vec4 GetDefaultFramePadding();

#pragma endregion

#pragma region Events/States

	COREGUI_API Vec4 GetItemRect();

	COREGUI_API bool IsWindowHovered(HoveredFlags flags = Hovered_None);

	COREGUI_API bool IsWindowHovered(const String& windowId, HoveredFlags flags = Hovered_None);

	COREGUI_API bool IsWindowFocused(FocusFlags flags = FOCUS_None);
	COREGUI_API bool IsWindowFocused(const String& windowId, FocusFlags flags = FOCUS_None);
	COREGUI_API bool IsWindowFocused(ID id, FocusFlags flags = FOCUS_None);

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

	COREGUI_API bool PushClipRectInWindow(const Rect& rectInWindow, bool intersectWithCurrentClipRect = true);
	COREGUI_API void PopClipRect();

    COREGUI_API void DrawRect(const Vec4& rect, const Color& color, Vec4 rounding = { 0, 0, 0, 0 }, f32 thickness = 1.0f);
	COREGUI_API void DrawRect(const Vec4& rect, u32 color, Vec4 rounding = { 0, 0, 0, 0 }, f32 thickness = 1.0f);

    COREGUI_API void FillRect(const Vec4& rect, const Color& color, Vec4 rounding = { 0, 0, 0, 0 });
	COREGUI_API void FillRect(const Vec4& rect, u32 color, Vec4 rounding = { 0, 0, 0, 0 });

	COREGUI_API void FillCircle(const Rect& rect, const Color& color);
	COREGUI_API void DrawCircle(const Rect& rect, const Color& color, f32 thickness = 1.0f);

	COREGUI_API void FillRect(const Vec4& rect, const Gradient& gradient, Vec4 rounding = { 0, 0, 0, 0 });

	COREGUI_API void FillCheckMark(const Vec2& pos, const Color& color, f32 size);
	COREGUI_API void FillArrow(const Vec2& pos, const Color& color, GUI::Dir direction, f32 size);

	COREGUI_API void RenderFrame(const Vec4& rect, const Color& color, f32 borderSize = 0.0f, Vec4 rounding = { 0, 0, 0, 0 });
	COREGUI_API void RenderFrame(const Vec4& rect, u32 color, f32 borderSize = 0.0f, Vec4 rounding = { 0, 0, 0, 0 });

	COREGUI_API void RenderFrame(const Vec4& rect, const Color& color, const Color& borderColor, f32 borderSize = 0.0f, Vec4 rounding = { 0, 0, 0, 0 });

	COREGUI_API void RenderNavHighlight(const Vec4& rect, ID id, f32 thickness = 1.0f, Vec4 rounding = { 0, 0, 0, 0 }, NavHighlightFlags flags = NavHighlightFlags_TypeDefault);

#pragma endregion
    
} // namespace CE

