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

				ImGui::PushStyleColor(ImGuiCol_Tab, Color::RGBA(41, 42, 45).ToU32());
				ImGui::PushStyleColor(ImGuiCol_TabActive, Color::RGBA(0, 0, 0).ToU32());
				ImGui::PushStyleColor(ImGuiCol_TabHovered, Color::RGBA(53, 54, 58).ToU32());
				ImGui::PushStyleColor(ImGuiCol_TabUnfocused, Color::RGBA(41, 42, 45).ToU32());
				ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, Color::RGBA(0, 0, 0).ToU32());
				ImGui::PushStyleColor(ImGuiCol_Separator, Color::RGBA(73, 76, 80).ToU32());
				ImGui::PushStyleColor(ImGuiCol_SeparatorActive, Color::RGBA(73, 76, 80).ToU32());
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
				ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 0);

				if (ImGui::BeginTabBar("DebugTabBar"))
				{
					if (ImGui::BeginTabItem("Computed"))
					{
						constexpr float minWidth = 90;
						constexpr float minHeight = 20;
						constexpr float maxHeight = minHeight * 7;
						constexpr float maxWidth = minWidth + minHeight * 6;

						auto origCursorPos = ImGui::GetCursorPos();
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

						// Fields
						ImGui::SetCursorPos(origCursorPos + ImVec2(0, 200));
						//ImGui::Separator();

						CFontManager::Get().PushFont(16);

						DrawComputedFieldsTable();

						CFontManager::Get().PopFont();

						ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem("Properties"))
					{
						DrawPropertiesTable();

						ImGui::EndTabItem();
					}

					ImGui::EndTabBar();
				}

				ImGui::PopStyleVar(2);
				ImGui::PopStyleColor(7);
			}
		}

		GUI::EndWindow();

		GUI::PopStyleColor();
	}

	void CWidgetDebugger::DrawPropertiesTable()
	{
		if (debugWidget == nullptr)
			return;

		ClassType* widgetClass = debugWidget->GetClass();

		static ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable;
		if (ImGui::BeginTable("PropertiesTable##CWidgetDebugger", 2, tableFlags))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TableNextColumn();

			auto drawField = [&](FieldType* field)
				{
					if (field == nullptr)
						return;

					String display = "";
					auto fieldTypeId = field->GetDeclarationTypeId();
					auto fieldDeclType = field->GetDeclarationType();

					if (fieldTypeId == TYPEID(UUID))
					{
						display = String::Format("{:x}", field->GetFieldValue<UUID>(debugWidget));
					}
					else if (fieldTypeId == TYPEID(String))
					{
						display = field->GetFieldValue<String>(debugWidget);
					}
					else if (fieldTypeId == TYPEID(Name))
					{
						display = field->GetFieldValue<Name>(debugWidget).GetString();
					}
					else if (fieldTypeId == TYPEID(u32))
					{
						display = String::Format("{}", field->GetFieldValue<u32>(debugWidget));
					}
					else if (fieldTypeId == TYPEID(s32))
					{
						display = String::Format("{}", field->GetFieldValue<s32>(debugWidget));
					}
					else if (fieldTypeId == TYPEID(u8))
					{
						display = String::Format("{}", field->GetFieldValue<u8>(debugWidget));
					}
					else if (fieldTypeId == TYPEID(bool))
					{
						display = String::Format("{}", field->GetFieldValue<bool>(debugWidget) ? "true" : "false");
					}
					else if (fieldTypeId == TYPEID(f32))
					{
						display = String::Format("{}", field->GetFieldValue<f32>(debugWidget));
					}
					else if (fieldDeclType != nullptr && fieldDeclType->IsAssignableTo(TYPEID(Object)))
					{
						Object* object = field->GetFieldValue<Object*>(debugWidget);
						if (object != nullptr)
						{
							display = String::Format("{} ({})", object->GetName(), object->GetClass()->GetName());
						}
					}
					else if (field->IsEnumField())
					{
						EnumType* enumType = (EnumType*)field->GetDeclarationType();
						s64 enumValue = field->GetFieldEnumValue(debugWidget);
						if (enumType->HasAnyEnumFlags(ENUM_FlagsEnum))
						{
							int numConstants = enumType->GetConstantsCount();
							for (int i = 0; i < numConstants; i++)
							{
								auto constant = enumType->GetConstant(i);
								if (constant != nullptr && 
									((constant->GetValue() & enumValue) != 0 || (enumValue == 0 && constant->GetValue() == 0)))
								{
									if (display.NonEmpty())
										display += " | ";
									display += constant->GetName().GetString();
								}
							}
						}
						else
						{
							auto constant = enumType->FindConstantWithValue(enumValue);
							if (constant == nullptr)
								return;
							display = constant->GetName().GetString();
						}
					}
					else if (field->GetName() == "styleClasses" && fieldTypeId == TYPEID(Array<u8>) && field->GetUnderlyingTypeId() == TYPEID(String))
					{
						const Array<String>& array = field->GetFieldValue<Array<String>>(debugWidget);
						for (const auto& string : array)
						{
							if (display.NonEmpty())
								display += " ";
							display += "." + string;
						}
					}
					else
					{
						//return;
					}

					ImGui::TableNextRow();

					ImGui::TableNextColumn();
					GUI::TextColored(field->GetName().GetString(), Color::RGBA(53, 212, 188));

					ImGui::TableNextColumn();
					GUI::Text(display);
				};
			
			for (auto field = widgetClass->GetFirstField(); field != nullptr; field = field->GetNext())
			{
				drawField(field);
			}

			ImGui::EndTable();
		}
	}

	void CWidgetDebugger::DrawComputedFieldsTable()
	{
		if (debugWidget == nullptr)
			return;

		static ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable;

		if (ImGui::BeginTable("ComputedFieldsTable##CWidgetDebugger", 2, tableFlags))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TableNextColumn();

			auto drawSingleEntry = [](const String& name, f32 value)
				{
					ImGui::TableNextRow();

					ImGui::TableNextColumn();
					GUI::TextColored(name, Color::RGBA(53, 212, 188));

					ImGui::TableNextColumn();
					String text = String::Format("{}", value);
					ImGui::Text(text.GetCString());
				};

			auto drawEnumEntry = [](const String& name, EnumType* enumType, const CStyleValue& value)
				{
					ImGui::TableNextRow();

					ImGui::TableNextColumn();
					GUI::TextColored(name, Color::RGBA(53, 212, 188));

					ImGui::TableNextColumn();
					auto constant = value.IsEnum() ? enumType->FindConstantWithValue(value.enumValue.x) : nullptr;
					if (constant != nullptr)
					{
						String text = constant->GetName().GetString().ToKebabCase();
						ImGui::Text(text.GetCString());
					}
					else
					{
						String text = "";
						if (value.IsEnum())
							text = String::Format("{}", value.enumValue.x);
						else if (value.IsSingle())
							text = String::Format("{}", value.single);
						else if (value.IsVector())
							text = String::Format("{}px {}px {}px {}px", value.vector.left, value.vector.top, value.vector.right, value.vector.bottom);
						ImGui::Text(text.GetCString());
					}
				};

			auto drawVec4Entry = [](const String& name, const Vec4& value)
				{
					ImGui::TableNextRow();

					ImGui::TableNextColumn();
					GUI::TextColored(name, Color::RGBA(53, 212, 188));

					ImGui::TableNextColumn();
					String text = String::Format("{}px {}px {}px {}px", value.left, value.top, value.right, value.bottom);
					ImGui::Text(text.GetCString());
				};

			auto drawComputedEntry = [](const String& name, const CStyleValue& value)
				{
					ImGui::TableNextRow();

					ImGui::TableNextColumn();
					GUI::TextColored(name, Color::RGBA(53, 212, 188));

					ImGui::TableNextColumn();

					if (value.IsVector())
					{
						String text = String::Format("{}px {}px {}px {}px", value.vector.left, value.vector.top, value.vector.right, value.vector.bottom);
						ImGui::Text(text.GetCString());
					}
					else if (value.IsColor())
					{
						u32 color = value.color.ToU32();
						String text = String::Format("rgba({}, {}, {}, {})", (int)(u8)(color), (int)(u8)(color >> 8), (int)(u8)(color >> 16), (int)(u8)(color >> 24));
						ImGui::Text(text.GetCString());
					}
					else if (value.IsString())
					{
						ImGui::Text(value.string.GetCString());
					}
					else if (value.IsSingle())
					{
						String text = String::Format("{}", value.single);
						if (name == "font-size")
							text = String::Format("{}", (u32)value.single);
						ImGui::Text(text.GetCString());
					}
				};

			drawVec4Entry("margin", debugWidget->GetComputedLayoutMargin());
			drawVec4Entry("padding", debugWidget->GetComputedLayoutPadding());
			drawSingleEntry("width", debugWidget->GetComputedLayoutSize().width);
			drawSingleEntry("height", debugWidget->GetComputedLayoutSize().height);
			drawVec4Entry("border-width", debugWidget->GetComputedLayoutBorder());
			drawComputedEntry("border-radius", debugWidget->GetComputedPropertyValue(CStylePropertyType::BorderRadius));
			drawComputedEntry("border-color", debugWidget->GetComputedPropertyValue(CStylePropertyType::BorderColor));
			drawEnumEntry("text-align", GetStaticEnum<CTextAlign>(), debugWidget->GetComputedPropertyValue(CStylePropertyType::TextAlign));
			drawComputedEntry("background", debugWidget->GetComputedPropertyValue(CStylePropertyType::Background));
			drawComputedEntry("foreground", debugWidget->GetComputedPropertyValue(CStylePropertyType::Foreground));
			//drawComputedEntry("opacity", debugWidget->GetComputedPropertyValue(CStylePropertyType::Opacity));
			drawComputedEntry("shadow-color", debugWidget->GetComputedPropertyValue(CStylePropertyType::ShadowColor));
			drawComputedEntry("shadow-offset", debugWidget->GetComputedPropertyValue(CStylePropertyType::ShadowOffset));
			drawComputedEntry("font", debugWidget->GetComputedPropertyValue(CStylePropertyType::FontName));
			drawComputedEntry("font-size", debugWidget->GetComputedPropertyValue(CStylePropertyType::FontSize));

			ImGui::EndTable();
		}
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

			String leftValue = values.left == 0 ? "-" : String::Format("{}", values.left);
			ImGui::RenderTextClipped(leftRect.Min, leftRect.Max, leftValue.GetCString(), NULL, nullptr, ImVec2(0.5f, 0.5f));

			String rightValue = values.right == 0 ? "-" : String::Format("{}", values.right);
			ImGui::RenderTextClipped(rightRect.Min, rightRect.Max, rightValue.GetCString(), NULL, nullptr, ImVec2(0.5f, 0.5f));

			String bottomValue = values.bottom == 0 ? "-" : String::Format("{}", values.bottom);
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
