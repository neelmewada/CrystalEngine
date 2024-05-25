#include "EditorSystem.h"

namespace CE::Editor
{
	static ColorPickerTool* colorPicker = nullptr;

	ColorPickerTool* ColorPickerTool::Open()
	{
		if (!colorPicker)
		{
			PlatformWindowInfo windowInfo{};
			windowInfo.fullscreen = windowInfo.hidden = windowInfo.resizable = windowInfo.maximised = false;
			windowInfo.resizable = false;
			windowInfo.windowFlags = PlatformWindowFlags::DestroyOnClose | PlatformWindowFlags::Utility;

			PlatformWindow* nativeWindow = PlatformApplication::Get()->CreatePlatformWindow("Color Picker", 450, 500, windowInfo);
			nativeWindow->SetMinimumSize(Vec2i(400, 500));
			nativeWindow->SetAlwaysOnTop(true);
			nativeWindow->SetBorderless(true);

			colorPicker = CreateWindow<ColorPickerTool>("ColorPicker", nativeWindow);
		}

		colorPicker->Show();

		return colorPicker;
	}

	void ColorPickerTool::EnableAlpha(bool enable)
	{
		this->enableAlpha = enable;
	}

	void ColorPickerTool::SetOriginalColor(const Color& original)
	{
		this->original = original;
		this->hsv = original.ToHSV();
	}

	void ColorPickerTool::SetColor(const Color& color)
	{
		this->value = color;
		this->hsv = color.ToHSV();

		SetNeedsPaint();
	}

	ColorPickerTool::ColorPickerTool()
	{
		title = "Color Picker";

		canBeClosed = true;
		canBeMaximized = canBeMinimized = false;
	}

	ColorPickerTool::~ColorPickerTool()
	{
		if (colorPicker == this)
		{
			colorPicker = nullptr;
		}
	}

	void ColorPickerTool::OnBeforeDestroy()
	{
		Super::OnBeforeDestroy();

		emit OnColorPickerClosed(this);
	}

	void ColorPickerTool::Construct()
	{
		Super::Construct();

		LoadStyleSheet(PlatformDirectories::GetLaunchDir() / "Editor/Styles/ColorPickerStyle.css");

		CWidget* topBox = CreateObject<CWidget>(this, "TopBox");
		topBox->AddStyleClass("HStack");
		{
			CWidget* topLeft = CreateObject<CWidget>(topBox, "TopLeftContainer");
			{
				CColorPicker* colorMap = CreateObject<CColorPicker>(topLeft, "ColorMap");
			}

			CWidget* topRight = CreateObject<CWidget>(topBox, "TopRightContainer");
			{
				ColorPickerPreview* previewOld = CreateObject<ColorPickerPreview>(topRight, "PreviewOld");
				previewOld->SetColor(Color::Cyan());

				CreateObject<CWidget>(topRight, "PreviewSeparator");

				ColorPickerPreview* previewNew = CreateObject<ColorPickerPreview>(topRight, "PreviewNew");
				previewNew->SetColor(Color::RGBA(128, 128, 128, 0));
			}
		}

		CWidget* bottomBox = CreateObject<CWidget>(this, "BottomBox");
		bottomBox->AddStyleClass("HStack");
		{
			CWidget* bottomLeft = CreateObject<CWidget>(bottomBox, "BottomLeftContainer");
			{
				constexpr const char* rgbaLabels[] = { "R", "G", "B", "A" };
				
				for (int i = 0; i < 4; ++i)
				{
					CWidget* hBox = CreateObject<CWidget>(bottomLeft, "ColorComponentField");
					hBox->AddStyleClass("HStack");
					{
						CLabel* label = CreateObject<CLabel>(hBox, "ColorLabel");
						label->SetText(rgbaLabels[i]);

						CTextInput* inputField = CreateObject<CTextInput>(hBox, "InputField");
						inputField->SetInputValidator(CFloatInputValidator);
						inputField->SetText(String::Format("{}", value.ToVec4().xyzw[i]));

						Bind(inputField, MEMBER_FUNCTION(CTextInput, OnTextChanged), [this, i](CTextInput* inputField)
							{
								f32 number = 0.0f;
								if (String::TryParse(inputField->GetText(), number))
								{
									number = Math::Clamp01(number);

									if (i == 0)
										value.r = number;
									else if (i == 1)
										value.g = number;
									else if (i == 2)
										value.b = number;
									else if (i == 3)
										value.a = number;

									hsv = value.ToHSV();
									SetNeedsPaint();
								}
							});
					}
				}
			}

			CWidget* bottomRight = CreateObject<CWidget>(bottomBox, "BottomRightContainer");
			{
				constexpr const char* hsvLabels[] = { "H", "S", "V" };

				for (int i = 0; i < 3; ++i)
				{
					CWidget* hBox = CreateObject<CWidget>(bottomRight, "ColorComponentField");
					hBox->AddStyleClass("HStack");
					{
						CLabel* label = CreateObject<CLabel>(hBox, "ColorLabel");
						label->SetText(hsvLabels[i]);

						CTextInput* inputField = CreateObject<CTextInput>(hBox, "InputField");
						inputField->SetInputValidator(CFloatInputValidator);
						inputField->SetText(String::Format("{}", value.ToVec4().xyzw[i]));

						Bind(inputField, MEMBER_FUNCTION(CTextInput, OnTextChanged), [this, i](CTextInput* inputField)
							{
								f32 number = 0.0f;
								if (String::TryParse(inputField->GetText(), number))
								{
									number = Math::Clamp01(number);

									if (i == 0)
										hsv.x = number;
									else if (i == 1)
										hsv.y = number;
									else if (i == 2)
										hsv.z = number;

									value = Color::HSV(hsv.x, hsv.y, hsv.z);
									SetNeedsPaint();
								}
							});
					}
				}

				CWidget* hbox = CreateObject<CWidget>(bottomRight, "HexFieldBox");
				hbox->AddStyleClass("HStack");
				{
					CLabel* hexLabel = CreateObject<CLabel>(hbox, "HexLabel");
					hexLabel->SetText("Hex");

					hexInput = CreateObject<CTextInput>(hbox, "HexInput");
					hexInput->SetText(String::Format("{:04X}", value.ToU32()));
				}

				CWidget* spacer = CreateObject<CWidget>(bottomRight, "Spacer");
				spacer->AddStyleClass("Spacer");

				CWidget* buttonBox = CreateObject<CWidget>(bottomRight, "ButtonBox");
				buttonBox->AddStyleClass("HStack");
				{
					CButton* okButton = CreateObject<CButton>(buttonBox, "OkButton");
					okButton->SetText("Ok");

					Bind(okButton, MEMBER_FUNCTION(CButton, OnMouseLeftClick),
						this, MEMBER_FUNCTION(Self, DoAccept));

					CButton* cancelButton = CreateObject<CButton>(buttonBox, "CancelButton");
					cancelButton->SetText("Cancel");

					Bind(cancelButton, MEMBER_FUNCTION(CButton, OnMouseLeftClick),
						this, MEMBER_FUNCTION(Self, DoCancel));
				}
			}
		}
	}

	void ColorPickerTool::DoAccept()
	{
		QueueDestroy();
	}

	void ColorPickerTool::DoCancel()
	{
		QueueDestroy();
	}

} // namespace CE::Editor
