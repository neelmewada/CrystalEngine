#pragma once

namespace CE
{
    ENUM(Flags)
    enum class FTextInputState : u8
    {
        None = 0,
        Hovered = BIT(0),
        Editing = BIT(1),
        Disabled = BIT(2)
    };
    ENUM_CLASS_FLAGS(FTextInputState);

    CLASS()
    class FUSIONCORE_API FTextInput : public FStyledWidget
    {
        CE_CLASS(FTextInput, FStyledWidget)
    public:

        FTextInput();

        void CalculateIntrinsicSize() override;

        bool IsHovered() const { return EnumHasFlag(state, FTextInputState::Hovered); }
        bool IsEditing() const { return EnumHasFlag(state, FTextInputState::Editing); }

    protected:

        void OnPaintContent(FPainter* painter) override;

        void OnFusionPropertyModified(const CE::Name& propertyName) override;

        FIELD()
        FTextInputState state = FTextInputState::None;

    protected: // - Fusion Fields -

        FIELD()
        String m_Text;

        FIELD()
        FFont m_Font;

        FIELD()
        Color m_Foreground = Color::White();


    public: // - Fusion Properties -

        FUSION_LAYOUT_PROPERTY(Text);
        FUSION_LAYOUT_PROPERTY(Font);

        FUSION_PROPERTY(Foreground);

        Self& FontFamily(const CE::Name& fontFamily);
        Self& FontSize(int fontSize);
        Self& Bold(bool bold);
        Self& Italic(bool italic);


        FUSION_FRIENDS;
        FUSION_WIDGET;
    };

} // namespace CE

#include "FTextInput.rtti.h"