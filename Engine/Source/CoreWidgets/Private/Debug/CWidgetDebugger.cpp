#include "CoreWidgets.h"

#include "imgui.h"

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

	void CWidgetDebugger::DrawLayoutRectAtCenter(const String& title, const Color& color, const Vec4& values, const Vec2& rectSize)
	{

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

		widget->debugDraw = ImGui::IsItemHovered();

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
