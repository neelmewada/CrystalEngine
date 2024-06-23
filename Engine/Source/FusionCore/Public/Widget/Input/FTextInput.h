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

        void CalculateIntrinsicSize() override;

    protected:

        void OnFusionPropertyModified(const CE::Name& propertyName) override;

    protected: // - Fusion Fields -

        FIELD()
        FVoidEvent m_OnTextPropertyUpdated;

    public: // - Fusion Properties -

        FUSION_EVENT(OnTextPropertyUpdated);

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

        bool SupportsMouseEvents() const override { return true; }

        bool SupportsKeyboardEvents() const override { return true; }

        bool SupportsFocusEvents() const override { return true; }

        bool SupportsDragEvents() const override { return true; }

        bool IsSelectionActive() const { return isSelectionActive; }

        const String& GetText() const;

    protected:

        void OnLostFocus() override;

        void OnPaintContent(FPainter* painter) override;

        void HandleEvent(FEvent* event) override;

        void Construct() override final;

        void OnFusionPropertyModified(const CE::Name& propertyName) override;

        // - Internal API -

        FUNCTION()
        void RecalculateCharacterOffsets();

        void TryRecalculateCharacterOffsets();

        FUNCTION()
        void OnTimeOut();

        void SetEditing(bool edit);

        void SetCursorPos(int cursorPos);

        void ScrollTo(int cursorPos);

        void InsertAt(const String& string, int insertPos);

        void RemoveRange(int startIndex, int count);

        void SelectRange(int startIndex, int endIndex);
        void DeselectAll();

        FIELD()
        FTextInputState state = FTextInputState::None;

        FIELD()
        FCompoundWidget* leftSlot = nullptr;

        FIELD()
        FTextInputLabel* inputLabel = nullptr;

        FIELD()
        FTimer* cursorTimer = nullptr;

    protected: // - Fusion Fields -

        FIELD(FusionProperty)
        Color m_SelectionColor = Color::RGBA(0, 112, 224);

    public: // - Fusion Properties -

        FUSION_PROPERTY(SelectionColor);

        Self& Text(const String& value);
        Self& Foreground(const Color& value);

        Self& LeftSlot(FWidget& content);

    private: // - Internal Data -

        Array<Rect> characterOffsets;
        Vec2 textSize;
        bool cursorState : 1 = false;
        bool isSelectionActive : 1 = false;
        int cursorPos = 0;
        int selectionStart = -1;
        int selectionEnd = -1;
        f32 scrollOffset = 0;
        f32 selectionDistance = 0;

        FUSION_FRIENDS;
        FUSION_WIDGET;
    };

} // namespace CE

#include "FTextInput.rtti.h"