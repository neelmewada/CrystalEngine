#pragma once

namespace CE::Editor
{
    DECLARE_SCRIPT_DELEGATE(ColorPickerDelegate, void, Color color);

    CLASS()
    class EDITORCORE_API ColorPickerTool : public FToolWindow
    {
        CE_CLASS(ColorPickerTool, FToolWindow)
    protected:

        ColorPickerTool();

        void Construct() override;

    public: // - Public API -

        static Ref<ColorPickerTool> Open();

        void SetOriginalColor(const Color& color);
        void SetColor(const Color& color);

    protected: // - Internal -

        void ParseHexInput();

        FUNCTION()
        void OnClickOk();

        FUNCTION()
        void OnClickCancel();

        FUNCTION()
        void OnHSVColorEdited(f32 h, f32 s, f32 v);

        void UpdateFields(ColorComponentField* excludeField = nullptr, bool excludeHexField = false, bool excludeColorPicker = false);

        FIELD()
        Color original;

        FIELD()
        Color value;

        FIELD()
        Vec3 hsv;

        Ref<FColorPicker> colorPicker;
        Ref<ColorPickerPreview> previewOld;
        Ref<ColorPickerPreview> previewNew;

        Ref<ColorComponentField> inputR;
        Ref<ColorComponentField> inputG;
        Ref<ColorComponentField> inputB;
        Ref<ColorComponentField> inputA;

        Ref<ColorComponentField> inputH;
        Ref<ColorComponentField> inputS;
        Ref<ColorComponentField> inputV;

        Ref<FTextInput> hexInput;

    public: // - Fusion Properties - 

        FUSION_EVENT(ColorPickerDelegate, OnColorChanged);

        FUSION_WIDGET;
    };
    
}

#include "ColorPickerTool.rtti.h"
