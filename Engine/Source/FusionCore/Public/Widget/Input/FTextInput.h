#pragma once

namespace CE
{
    class FTextInput;


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

        void CalculateIntrinsicSize() override;

        bool SupportsMouseEvents() const override { return true; }

        bool SupportsKeyboardEvents() const override { return true; }

        bool SupportsFocusEvents() const override { return true; }

        bool SupportsDragEvents() const override { return true; }

        bool IsTextSelected() const { return isSelectionActive; }

    protected:

        void OnFusionPropertyModified(const CE::Name& propertyName) override;

        void OnPaint(FPainter* painter) override;

        void HandleEvent(FEvent* event) override;

        FUNCTION()
        void RecalculateCharacterOffsets();

        FUNCTION()
        void OnTimeOut();

        // - Internal API -

        void OnMouseClick(Vec2 localPos, bool isDoubleClick);

        Range GetCharacterMinMax(int characterIndex);

        bool IsEditing();

        void StartEditing();
        void StopEditing(bool restoreOriginal = false);

        void SetCursorPos(int newCursorPos);

        void ScrollTo(int charIndex);

        void InsertAt(const String& string, int insertPos);
        void InsertAt(char character, int insertPos);

        void RemoveRange(int startIndex, int count);
        void RemoveSelectedRange();

        void SelectAll();
        void SelectRange(int startIndex, int endIndex);
        void DeselectAll();

    protected: // - Fusion Fields -

        FIELD(FusionProperty)
        Color m_SelectionColor = Color::RGBA(0, 112, 224);

    public: // - Fusion Properties -

        FUSION_PROPERTY(SelectionColor);

    private:

        FIELD()
        FTimer* cursorTimer = nullptr;

        FTextInput* textInput = nullptr;

        String originalText;
        Array<Rect> characterOffsets;
        Vec2 textSize;
        bool cursorState : 1 = false;
        bool isSelectionActive : 1 = false;
        int cursorPos = 0;
        int selectionStart = -1;
        int selectionEnd = -1;
        f32 textScrollOffset = 0;
        f32 selectionDistance = 0;

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

        bool SupportsMouseEvents() const override { return true; }

        bool SupportsKeyboardEvents() const override { return true; }

        bool SupportsFocusEvents() const override { return true; }

        bool SupportsDragEvents() const override { return true; }

    protected:

        void OnLostFocus() override;

        void Construct() override final;

        void HandleEvent(FEvent* event) override;

        void SetEditingInternal(bool editing);

        FIELD()
        FTextInputState state = FTextInputState::None;

        FIELD()
        FCompoundWidget* leftSlot = nullptr;

        FIELD()
        FTextInputLabel* inputLabel = nullptr;

    protected: // - Fusion Fields -


    public: // - Fusion Properties -

        FUSION_PROPERTY_WRAPPER(SelectionColor, inputLabel);

        FUSION_PROPERTY_WRAPPER(Text, inputLabel);

        //Self& Text(const String& value);
        Self& Foreground(const Color& value);

        Self& LeftSlot(FWidget& content);

    private: // - Internal Data -

        friend class FTextInputLabel;
        FUSION_FRIENDS;
        FUSION_WIDGET;
    };

} // namespace CE

#include "FTextInput.rtti.h"