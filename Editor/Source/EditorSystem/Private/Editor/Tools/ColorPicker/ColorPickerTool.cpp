#include "EditorSystem.h"

namespace CE::Editor
{
	static ColorPickerTool* colorPicker = nullptr;

	static const Color hueValues[] = { Color(1, 0, 0), Color(1, 1, 0), Color(0, 1, 0),
										Color(0, 1, 1), Color(0, 0, 1), Color(1, 0, 1),
										Color(1, 0, 0)
	};

	static u8 ParseByte(const char* str)
	{
		char msb = str[0];
		char lsb = str[1];

		u8 value = 0;

		if (lsb >= '0' && lsb <= '9')
		{
			value = (u8)lsb - '0';
		}
		else if (lsb >= 'A' && lsb <= 'F')
		{
			value = 10 + (u8)lsb - 'A';
		}

		if (msb >= '0' && msb <= '9')
		{
			value += ((u8)msb - '0') * 16;
		}
		else if (msb >= 'A' && msb <= 'F')
		{
			value += (10 + (u8)msb - 'A') * 16;
		}

		return value;
	}

	ColorPickerTool* ColorPickerTool::Open()
	{
		if (!colorPicker)
		{
			PlatformWindowInfo windowInfo{};
			windowInfo.fullscreen = windowInfo.hidden = windowInfo.resizable = windowInfo.maximised = false;
			windowInfo.resizable = false;
			windowInfo.windowFlags = PlatformWindowFlags::DestroyOnClose | PlatformWindowFlags::Utility;

			PlatformWindow* nativeWindow = PlatformApplication::Get()->CreatePlatformWindow("Color Picker", 450, 520, windowInfo);
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

		previewOld->SetColor(original);
	}

	void ColorPickerTool::SetColor(const Color& color)
	{
		this->value = color;
		this->hsv = color.ToHSV();

		UpdateFields();

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

	static String FormatHex(u32 hex)
	{
		String result = String::Format("{:08X}", hex);

		for (int i = 0; i < 2; ++i)
		{
			int pos = i * 2;
			int otherPos = 6 - i * 2;

			std::swap(result[pos], result[otherPos]);
			std::swap(result[pos + 1], result[otherPos + 1]);
		}

		return result;
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

		if (hexInput != excludeField)
		{
			hexInput->SetText(FormatHex(value.ToU32()));
		}

		previewNew->SetColor(value);

		if (excludeField != nullptr)
		{

		}

		inputR->SetGradient(CGradient()
			.WithRotation(0)
			.WithType(CGradientType::LinearGradient)
			.AddKey(Color(0, value.g, value.b, 1), 0)
			.AddKey(Color(1, value.g, value.b, 1), 100)
		);

		inputG->SetGradient(CGradient()
			.WithRotation(0)
			.WithType(CGradientType::LinearGradient)
			.AddKey(Color(value.r, 0, value.b, 1), 0)
			.AddKey(Color(value.r, 1, value.b, 1), 100)
		);

		inputB->SetGradient(CGradient()
			.WithType(CGradientType::LinearGradient)
			.AddKey(Color(value.r, value.g, 0, 1), 0)
			.AddKey(Color(value.r, value.g, 1, 1), 100)
		);

		inputA->SetGradient(CGradient()
			.WithType(CGradientType::LinearGradient)
			.AddKey(Color(value.r, value.g, value.b, 0), 0)
			.AddKey(Color(value.r, value.g, value.b, 1), 100)
		);

		inputS->SetGradient(CGradient()
			.WithType(CGradientType::LinearGradient)
			.AddKey(Color::HSV(hsv.x, 0, hsv.z), 0)
			.AddKey(Color::HSV(hsv.x, 1, hsv.z), 100)
		);

		inputV->SetGradient(CGradient()
			.WithType(CGradientType::LinearGradient)
			.AddKey(Color::HSV(hsv.x, hsv.y, 0), 0)
			.AddKey(Color::HSV(hsv.x, hsv.y, 1), 100)
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
				colorMap = CreateObject<CColorPicker>(topLeft, "ColorMap");

				Bind(colorMap, MEMBER_FUNCTION(CColorPicker, OnHSVColorChanged), [this](f32 h, f32 s, f32 v)
					{
						hsv = Vec3(h, s, v);
						f32 a = value.a;
						value = Color::HSV(h, s, v);
						value.a = a;

						UpdateFields();
						SetNeedsPaint();

						emit OnColorSelected(value);
					});
			}

			CWidget* topRight = CreateObject<CWidget>(topBox, "TopRightContainer");
			{
				CLabel* oldLabel = CreateObject<CLabel>(topRight, "OldLabel");
				oldLabel->SetText("Old");
				
				previewOld = CreateObject<ColorPickerPreview>(topRight, "PreviewOld");
				previewOld->SetColor(Color::Cyan());

				CreateObject<CWidget>(topRight, "PreviewSeparator");

				previewNew = CreateObject<ColorPickerPreview>(topRight, "PreviewNew");
				previewNew->SetColor(value);

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
							ColorComponentInput* inputField = CreateObject<ColorComponentInput>(vBox, "InputField");
							inputField->SetFieldType(NumericFieldType::Float32);
							inputField->SetText(String::Format("{}", value.ToVec4().xyzw[i]));
							inputField->SetRange(0, 1);
							inputField->SetFloatSensitivity(0.005f);

							CGradient gradient{};
							gradient.gradientType = CGradientType::LinearGradient;

							CGradientKey start{}, end{};

							start.position = 0; end.position = 100.f;
							start.isPercent = end.isPercent = true;
							start.color = Color::Red();
							end.color = Color::Green();

							if (i == 0)
							{
								inputR = inputField;
							}
							else if (i == 1)
							{
								inputG = inputField;
							}
							else if (i == 2)
							{
								inputB = inputField;
							}
							else if (i == 3)
							{
								inputA = inputField;
								inputA->ShowTransparency(true);

								start.position = 0; end.position = 100.f;
								start.isPercent = end.isPercent = true;
								start.color = Color::Clear();
								end.color = Color::Black();
							}

							gradient.keys.AddRange({ start, end });

							inputField->SetGradient(gradient);

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
							ColorComponentInput* inputField = CreateObject<ColorComponentInput>(vBox, "InputField");
							inputField->SetFieldType(NumericFieldType::Float32);
							inputField->SetText(String::Format("{}", value.ToVec4().xyzw[i]));
							inputField->SetFloatSensitivity(0.01f);

							if (i == 0)
							{
								inputH = inputField;
								inputField->SetRange(0, 360);
								inputField->SetFloatSensitivity(4.0f);

								CGradient gradient{};
								gradient.gradientType = CGradientType::LinearGradient;

								for (int j = 0; j < 7; ++j)
								{
									CGradientKey key{};
									key.color = hueValues[j];
									key.position = 100.0f / 6.0f * (f32)j;
									gradient.keys.Add(key);
								}

								inputField->SetGradient(gradient);
							}
							else if (i == 1)
							{
								inputS = inputField;
								inputField->SetRange(0, 1);
							}
							else if (i == 2)
							{
								inputV = inputField;
								inputField->SetRange(0, 1);
							}

							Bind(inputField, MEMBER_FUNCTION(CTextInput, OnTextEdited), [this, i](CTextInput* inputField)
								{
									f32 number = 0.0f;
									if (String::TryParse(inputField->GetText(), number))
									{
										if (i == 0)
											number = Math::Clamp<f32>(number, 0, 360);
										else
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
					hexInput->SetText(FormatHex(value.ToU32()));

					Bind(hexInput, MEMBER_FUNCTION(CTextInput, OnEditingFinished), [this](CTextInput*)
						{
							String text = hexInput->GetText().ToUpper();
							if (text.GetLength() == 6)
							{
								f32 r = ParseByte(text.GetCString());
								f32 g = ParseByte(text.GetCString() + 2);
								f32 b = ParseByte(text.GetCString() + 4);

								value.r = r / 255.0f;
								value.g = g / 255.0f;
								value.b = b / 255.0f;
							}
							else if (text.GetLength() >= 8)
							{
								f32 r = ParseByte(text.GetCString());
								f32 g = ParseByte(text.GetCString() + 2);
								f32 b = ParseByte(text.GetCString() + 4);
								f32 a = ParseByte(text.GetCString() + 6);

								value.r = r / 255.0f;
								value.g = g / 255.0f;
								value.b = b / 255.0f;
								value.a = a / 255.0f;
							}
							else if (text.GetLength() == 1)
							{
								char c = text[0];
								u8 halfByte = 0;

								if (c >= '0' && c <= '9')
									halfByte = c - '0';
								else if (c >= 'A' && c <= 'F')
									halfByte = 10 + c - 'A';

								value.r = value.g = value.b = (f32)halfByte / 15.0f;
							}
							else if (text.GetLength() == 2)
							{
								f32 byte = ParseByte(text.GetCString());

								value.r = value.g = value.b = byte / 255.0f;
							}
							else if (text.GetLength() >= 3 && text.GetLength() <= 5)
							{
								f32 components[3] = { 0, 0, 0 };

								for (int i = 0; i < 3; ++i)
								{
									char c = text[i];
									u8 halfByte = 0;

									if (c >= '0' && c <= '9')
										halfByte = c - '0';
									else if (c >= 'A' && c <= 'F')
										halfByte = 10 + c - 'A';

									components[i] = (f32)halfByte / 15.0f;
								}

								value.r = components[0];
								value.g = components[1];
								value.b = components[2];
							}

							SetColor(value);
						});
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
