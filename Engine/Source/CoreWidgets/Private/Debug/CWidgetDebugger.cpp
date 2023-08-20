#include "CoreWidgets.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

namespace CE::Widgets
{

	void CWidgetDebugger::RenderGUI()
	{
		if (!isShown)
			return;

		GUI::PushStyleColor(GUI::StyleCol_WindowBg, Color::FromRGBA32(32, 33, 36));

		auto shown = GUI::BeginWindow("Widget Debugger", &isShown);

		if (shown)
		{
			GUI::Checkbox("Widget Selection Mode", &enableLayoutDebugMode);

			static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody;

			if (ImGui::BeginTable("Tree view", 1, flags, ImVec2(0, 250)))
			{
				for (auto window : registeredWindows)
				{
					if (window == nullptr)
						continue;

					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					String windowName = String::Format("{} | {}", window->GetTitle().Split('#').GetFirst(), window->GetName());
					if (!foundWidgetInHierarchy && debugWidget != window)
					{
						ImGui::SetNextItemOpen(true);
					}

					bool open = ImGui::TreeNodeEx(windowName.GetCString(), 
						ImGuiTreeNodeFlags_SpanFullWidth | (debugWidget == window ? ImGuiTreeNodeFlags_Selected : 0));
					if (ImGui::IsItemClicked())
					{
						SetDebugWidget(window);
					}

					if (open)
					{
						for (auto widget : window->attachedWidgets)
						{
							DrawWidgetTreeEntry(widget);
						}

						ImGui::TreePop();
					}
				}

				ImGui::EndTable();
			}

			if (debugWidget != nullptr)
			{
				if (debugWidget->IsWindow())
				{
					CWindow* debugWindow = (CWindow*)debugWidget;
				}
				else
				{
					
				}

				constexpr float minWidth = 90;
				constexpr float minHeight = 20;
				constexpr float maxHeight = minHeight * 7;
				constexpr float maxWidth = minWidth + minHeight * 6;

				auto cursorScreenPos = ImGui::GetCursorScreenPos();
				auto availSpace = ImGui::GetContentRegionAvail();
				float centerPosX = cursorScreenPos.x + availSpace.x / 2;
				float centerPosY = cursorScreenPos.y + 20 + maxHeight / 2;
				
				auto marginValue = debugWidget->GetComputedLayoutMargin();
				auto borderValue = debugWidget->GetComputedLayoutBorder();
				auto paddingValue = debugWidget->GetComputedLayoutPadding();
				auto intrinsicSize = debugWidget->GetComputedLayoutIntrinsicSize();
				
				Color clearColor = Color::RGBA(32, 33, 36);

				ImGui::SetCursorPosX(ImGui::GetCursorStartPos().x + availSpace.x / 2 - maxWidth / 2);
				CDebugBackgroundFilter newHoveredLayoutItem = CDebugBackgroundFilter::All;

				auto calculateRect = [&](float width, float height)
					{
						return Rect(centerPosX - width / 2, centerPosY - height / 2, centerPosX + width / 2, centerPosY + height / 2);
					};
				auto rectContains = [](const Rect& screenRect, const Vec2& screenPoint) -> bool
					{
						if (screenPoint.x > screenRect.left &&
							screenPoint.y > screenRect.top &&
							screenPoint.x < screenRect.right &&
							screenPoint.y < screenRect.bottom)
							return true;
						return false;
					};

				GUI::GuiStyleState textStyle{};
				Color foregroundDark = Color::RGBA(36, 36, 36);

				CFontManager::Get().PushFont(15);
				
				auto marginRect = calculateRect(minWidth + minHeight * 6, minHeight * 7);
				DrawLayoutRectAtCenter("margin",
					EnumHasFlag(hoveredLayoutItem, CDebugBackgroundFilter::Margin), Color::RGBA(173, 128, 82), clearColor,
					marginValue, marginRect);

				auto borderRect = calculateRect(minWidth + minHeight * 4, minHeight * 5);
				DrawLayoutRectAtCenter("border",
					EnumHasFlag(hoveredLayoutItem, CDebugBackgroundFilter::Border), Color::RGBA(227, 195, 129), clearColor,
					borderValue, borderRect);

				auto paddingRect = calculateRect(minWidth + minHeight * 2, minHeight * 3);
				DrawLayoutRectAtCenter("padding",
					EnumHasFlag(hoveredLayoutItem, CDebugBackgroundFilter::Padding), Color::RGBA(183, 196, 127), clearColor,
					paddingValue, paddingRect);

				auto sizeRect = calculateRect(minWidth, minHeight);
				DrawLayoutRectAtCenter("",
					EnumHasFlag(hoveredLayoutItem, CDebugBackgroundFilter::IntrinsicSize), Color::RGBA(135, 178, 188), clearColor,
					intrinsicSize, sizeRect);

				bool hovered = false, held = false;
				GUI::InvisibleButton(marginRect, GUI::COORD_Screen, GUI::GetID("layout_size_debug_button"), hovered, held);
				if (hovered || held)
				{
					newHoveredLayoutItem = CDebugBackgroundFilter::All;
					if (rectContains(sizeRect, GUI::GetMousePos()))
					{
						newHoveredLayoutItem = CDebugBackgroundFilter::IntrinsicSize;
					}
					else if (rectContains(paddingRect, GUI::GetMousePos()))
					{
						newHoveredLayoutItem = CDebugBackgroundFilter::Padding;
					}
					else if (rectContains(borderRect, GUI::GetMousePos()))
					{
						newHoveredLayoutItem = CDebugBackgroundFilter::Border;
					}
					else if (rectContains(marginRect, GUI::GetMousePos()))
					{
						newHoveredLayoutItem = CDebugBackgroundFilter::Margin;
					}
				}

				hoveredLayoutItem = newHoveredLayoutItem;

				debugWidget->debugDraw = hoveredLayoutItem != CDebugBackgroundFilter::All;
				debugWidget->filter = hoveredLayoutItem;

				textStyle.foreground = EnumHasFlag(hoveredLayoutItem, CDebugBackgroundFilter::IntrinsicSize) ? foregroundDark : Color::White();
				GUI::Text(GUI::ScreenSpaceToWidgetSpace(sizeRect), String::Format("{}x{}", intrinsicSize.x, intrinsicSize.y), textStyle);

				CFontManager::Get().PopFont();
			}
		}

		GUI::EndWindow();

		GUI::PopStyleColor();
	}

	void CWidgetDebugger::SetDebugWidget(CWidget* target)
	{
		debugWidget = target;
		foundWidgetInHierarchy = false;
	}

	void CWidgetDebugger::DrawLayoutRectAtCenter(const String& title, bool isEnabled, const Color& color, const Color& clearColor, const Vec4& values, const Rect& screenRect)
	{
		GUI::FillRect(screenRect, isEnabled ? color : clearColor);
		GUI::DrawRect(screenRect + Rect(-0.5f, -0.5f, 0.5f, 0.5f), Color::Black(), {}, 0.5f);

		if (!title.IsEmpty())
		{
			constexpr float minHeight = 20;

			u32 textColor = isEnabled ? Color::RGBA(36, 36, 36).ToU32() : Color(1, 1, 1).ToU32();
			ImGui::PushStyleColor(ImGuiCol_Text, textColor);

			ImGui::GetWindowDrawList()->AddText(ImVec2(screenRect.x + 2.5f, screenRect.y + 2.5f), 
				textColor, title.GetCString());

			ImRect topRect = ImRect(screenRect.left, screenRect.top, screenRect.right, screenRect.top + minHeight);
			ImRect leftRect = ImRect(screenRect.left, screenRect.top, screenRect.left + minHeight, screenRect.bottom);
			ImRect rightRect = ImRect(screenRect.right - minHeight, screenRect.top, screenRect.right, screenRect.bottom);
			ImRect bottomRect = ImRect(screenRect.left, screenRect.bottom - minHeight, screenRect.right, screenRect.bottom);

			String topValue = values.top == 0 ? "-" : String::Format("{}", values.top);
			ImGui::RenderTextClipped(topRect.Min, topRect.Max, topValue.GetCString(), NULL, nullptr, ImVec2(0.5f, 0.5f));

			String leftValue = values.top == 0 ? "-" : String::Format("{}", values.left);
			ImGui::RenderTextClipped(leftRect.Min, leftRect.Max, leftValue.GetCString(), NULL, nullptr, ImVec2(0.5f, 0.5f));

			String rightValue = values.top == 0 ? "-" : String::Format("{}", values.right);
			ImGui::RenderTextClipped(rightRect.Min, rightRect.Max, rightValue.GetCString(), NULL, nullptr, ImVec2(0.5f, 0.5f));

			String bottomValue = values.top == 0 ? "-" : String::Format("{}", values.bottom);
			ImGui::RenderTextClipped(bottomRect.Min, bottomRect.Max, bottomValue.GetCString(), NULL, nullptr, ImVec2(0.5f, 0.5f));

			ImGui::PopStyleColor();
		}
	}

	void CWidgetDebugger::DrawWidgetTreeEntry(CWidget* widget)
	{
		if (!widget)
			return;

		bool isLeaf = widget->attachedWidgets.IsEmpty();

		String widgetName = String::Format("{} ({})", widget->GetName(), widget->GetClass()->GetName().GetLastComponent());
		if (!foundWidgetInHierarchy && debugWidget != nullptr && debugWidget->IsWidgetPresentInParentHierarchy(widget))
			ImGui::SetNextItemOpen(true);

		if (widget == debugWidget)
		{
			foundWidgetInHierarchy = true;
		}

		bool isOpen = ImGui::TreeNodeEx(widgetName.GetCString(),
			(isLeaf ? (ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen) : 0) |
			ImGuiTreeNodeFlags_SpanFullWidth |
			(debugWidget == widget ? ImGuiTreeNodeFlags_Selected : 0));

		if (ImGui::IsItemClicked())
		{
			SetDebugWidget(widget);
		}

		widget->forceDebugDrawMode = ImGui::IsItemHovered() ? CDebugBackgroundFilter::All : CDebugBackgroundFilter::None;

		if (isOpen && !isLeaf)
		{
			for (auto widget : widget->attachedWidgets)
			{
				DrawWidgetTreeEntry(widget);
			}

			ImGui::TreePop();
		}
	}

} // namespace CE::Widgets
