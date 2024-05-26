#include "EditorSystem.h"

namespace CE::Editor
{
	static ColorPickerTool* colorPicker = nullptr;

	static const Color hueValues[] = { Color(1, 0, 0), Color(1, 1, 0), Color(0, 1, 0),
										Color(0, 1, 1), Color(0, 0, 1), Color(1, 0, 1),
										Color(1, 0, 0)
	};

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

	void ColorPickerTool::SetAlphaEnabled(bool enable)
	{
		if (this->enableAlpha == enable)
			return;

		this->enableAlpha = enable;

		alphaBox->SetEnabled(enable);
	}

	void ColorPickerTool::SetOriginalColor(const Color& original)
	{
		this->original = original;
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

	void ColorPickerTool::UpdateFields(CTextInput* excludeField)
	{
		constexpr const char* format = "{:.6f}";

		if (inputH != excludeField)
			inputH->SetText(String::Format(format, hsv.x));
		if (inputS != excludeField)
			inputS->SetText(String::Format(format, hsv.y));
		if (inputV != excludeField)
			inputV->SetText(String::Format(format, hsv.z));
		
		if (inputR != excludeField)
			inputR->SetText(String::Format(format, value.r));
		if (inputG != excludeField)
			inputG->SetText(String::Format(format, value.g));
		if (inputB != excludeField)
			inputB->SetText(String::Format(format, value.b));
		if (inputA != excludeField)
			inputA->SetText(String::Format(format, value.a));

		gradientR->SetGradient(CGradient()
			.WithRotation(0)
			.WithType(CGradientType::LinearGradient)
			.AddKey(Color(0, value.g, value.b, 1), 0)
			.AddKey(Color(1, value.g, value.b, 1), 100)
		);

		gradientG->SetGradient(CGradient()
			.WithRotation(0)
			.WithType(CGradientType::LinearGradient)
			.AddKey(Color(value.r, 0, value.b, 1), 0)
			.AddKey(Color(value.r, 1, value.b, 1), 1000)
		);

		gradientB->SetGradient(CGradient()
			.WithType(CGradientType::LinearGradient)
			.AddKey(Color(value.r, value.g, 0, 1), 0)
			.AddKey(Color(value.r, value.g, 1, 1), 100)
		);

		gradientA->SetGradient(CGradient()
			.WithType(CGradientType::LinearGradient)
			.AddKey(Color(value.r, value.g, value.b, 0), 0)
			.AddKey(Color(value.r, value.g, value.b, 1), 100)
		);

		
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

				Bind(colorMap, MEMBER_FUNCTION(CColorPicker, OnHSVColorChanged), [this](f32 h, f32 s, f32 v)
					{
						hsv = Vec3(h, s, v);
						value = Color::HSV(h, s, v);

						UpdateFields();
						SetNeedsPaint();

						emit OnColorSelected(value);
					});
			}

			CWidget* topRight = CreateObject<CWidget>(topBox, "TopRightContainer");
			{
				CLabel* oldLabel = CreateObject<CLabel>(topRight, "OldLabel");
				oldLabel->SetText("Old");
				
				ColorPickerPreview* previewOld = CreateObject<ColorPickerPreview>(topRight, "PreviewOld");
				previewOld->SetColor(Color::Cyan());

				CreateObject<CWidget>(topRight, "PreviewSeparator");

				ColorPickerPreview* previewNew = CreateObject<ColorPickerPreview>(topRight, "PreviewNew");
				previewNew->SetColor(Color::RGBA(128, 128, 128, 0));

				CLabel* newLabel = CreateObject<CLabel>(topRight, "NewLabel");
				newLabel->SetText("New");

				CImageButton* eyeDropButton = CreateObject<CImageButton>(topRight, "EyeDropButton");
				eyeDropButton->SetText("");
				eyeDropButton->SetImage("/Editor/Assets/Icons/ColorPicker");

				Bind(eyeDropButton, MEMBER_FUNCTION(CImageButton, OnMouseLeftClick), [this]
					{
						
					});

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
					if (i == 4)
					{
						alphaBox = hBox;
					}
					hBox->AddStyleClass("HStack");
					{
						CLabel* label = CreateObject<CLabel>(hBox, "ColorLabel");
						label->SetText(rgbaLabels[i]);

						CWidget* vBox = CreateObject<CWidget>(hBox, "InputBox");
						vBox->AddStyleClass("VStack");
						{
							CTextInput* inputField = CreateObject<CTextInput>(vBox, "InputField");
							inputField->SetInputValidator(CFloatInputValidator);
							inputField->SetText(String::Format("{}", value.ToVec4().xyzw[i]));

							ColorPickerGradient* gradientPreview = CreateObject<ColorPickerGradient>(vBox, "GradientPreview");

							CGradient gradient{};
							gradient.gradientType = CGradientType::LinearGradient;

							CGradientKey start{}, end{};

							start.position = 0; end.position = 100.f;
							start.isPercent = end.isPercent = true;
							start.color = Color::Red();
							end.color = Color::Green();

							if (i == 0)
							{
								gradientR = gradientPreview;
								inputR = inputField;
							}
							else if (i == 1)
							{
								gradientG = gradientPreview;
								inputG = inputField;
							}
							else if (i == 2)
							{
								gradientB = gradientPreview;
								inputB = inputField;
							}
							else if (i == 3)
							{
								gradientA = gradientPreview;
								inputA = inputField;
								gradientPreview->AddStyleClass("Transparent");

								start.position = 0; end.position = 100.f;
								start.isPercent = end.isPercent = true;
								start.color = Color::Clear();
								end.color = Color::Black();
							}

							gradient.keys.AddRange({ start, end });

							gradientPreview->SetGradient(gradient);

							Bind(inputField, MEMBER_FUNCTION(CTextInput, OnTextEdited), [this, i](CTextInput* inputField)
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

										UpdateFields(inputField);
										SetNeedsPaint();

										emit OnColorSelected(value);
									}
								});
						}
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

						CWidget* vBox = CreateObject<CWidget>(hBox, "InputBox");
						vBox->AddStyleClass("VStack");
						{
							CTextInput* inputField = CreateObject<CTextInput>(vBox, "InputField");
							inputField->SetInputValidator(CFloatInputValidator);
							inputField->SetText(String::Format("{}", value.ToVec4().xyzw[i]));

							ColorPickerGradient* gradientPreview = CreateObject<ColorPickerGradient>(vBox, "GradientPreview");

							if (i == 0)
							{
								inputH = inputField;
								gradientH = gradientPreview;

								CGradient gradient{};
								gradient.gradientType = CGradientType::LinearGradient;

								for (int j = 0; j < 7; ++j)
								{
									CGradientKey key{};
									key.color = hueValues[j];
									key.position = 100.0f / 6.0f * (f32)j;
									gradient.keys.Add(key);
								}

								gradientH->SetGradient(gradient);
							}
							else if (i == 1)
							{
								inputS = inputField;
								gradientS = gradientPreview;
							}
							else if (i == 2)
							{
								inputV = inputField;
								gradientV = gradientPreview;
							}

							Bind(inputField, MEMBER_FUNCTION(CTextInput, OnTextEdited), [this, i](CTextInput* inputField)
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

										UpdateFields(inputField);
										SetNeedsPaint();

										emit OnColorSelected(value);
									}
								});
						}
					}
				}

				CWidget* hbox = CreateObject<CWidget>(bottomRight, "HexFieldBox");
				hbox->AddStyleClass("HStack");
				{
					CLabel* hexLabel = CreateObject<CLabel>(hbox, "HexLabel");
					hexLabel->SetText("Hex");

					hexInput = CreateObject<CTextInput>(hbox, "HexInput");
					hexInput->SetText(String::Format("{:08X}", value.ToU32()));
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

	void ColorPickerTool::OnClickClose()
	{
		Super::OnClickClose();

		emit OnColorSelected(original);
	}

	void ColorPickerTool::DoAccept()
	{
		emit OnColorSelected(value);

		QueueDestroy();
	}

	void ColorPickerTool::DoCancel()
	{
		emit OnColorSelected(original);

		QueueDestroy();
	}

} // namespace CE::Editor
