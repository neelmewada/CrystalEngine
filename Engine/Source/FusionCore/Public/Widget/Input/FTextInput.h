#pragma once

namespace CE
{
    ENUM(Flags)
    enum class FTextInputState : u8
    {
        None = 0,
        Hovered = BIT(0),
        Editing = BIT(1),
        InteractionDisabled = BIT(2)
    };
    ENUM_CLASS_FLAGS(FTextInputState);

    CLASS()
    class FUSIONCORE_API FTextInputLabel : public FLabel
    {
        CE_CLASS(FTextInputLabel, FLabel)
    public:

        FTextInputLabel();

    private:

        friend class FTextInput;
        FUSION_FRIENDS;
        FUSION_WIDGET;
    };

    CLASS()
    class FUSIONCORE_API FTextInput : public FStyledWidget
    {
        CE_CLASS(FTextInput, FStyledWidget)
    public:

        FTextInput();

        bool IsHovered() const { return EnumHasFlag(state, FTextInputState::Hovered); }
        bool IsEditing() const { return EnumHasFlag(state, FTextInputState::Editing); }
        bool IsInteractionDisabled() const { return EnumHasFlag(state, FTextInputState::InteractionDisabled); }

        bool CanReceiveMouseEvents() const override { return true; }

        bool CanReceiveKeyboardEvents() const override { return true; }

    protected:

        void OnPaintContent(FPainter* painter) override;

        void HandleEvent(FEvent* event) override;

        void Construct() override final;

        void OnFusionPropertyModified(const CE::Name& propertyName) override;

        void SetEditing(bool edit);

        FIELD()
        FTextInputState state = FTextInputState::None;

        FIELD()
        FCompoundWidget* leftSlot = nullptr;

        FIELD()
        FTextInputLabel* inputLabel = nullptr;

    protected: // - Fusion Fields -

        FIELD(FusionLayoutProperty)
        FFont m_Font;


    public: // - Fusion Properties -

        FUSION_LAYOUT_PROPERTY(Font);

        Self& Text(const String& value);
        Self& Foreground(const Color& value);

        Self& FontFamily(const CE::Name& fontFamily);
        Self& FontSize(int fontSize);
        Self& Bold(bool bold);
        Self& Italic(bool italic);

        Self& LeftSlot(FWidget& content);

    private: // - Internal Data -

        int cursorPos = 0;

        FUSION_FRIENDS;
        FUSION_WIDGET;
    };

} // namespace CE

#include "FTextInput.rtti.h"