#pragma once

namespace CE::Widgets
{
    class CPopup;

    CLASS()
    class CRYSTALWIDGETS_API CComboBoxItem : public CWidget
    {
        CE_CLASS(CComboBoxItem, CWidget)
    public:

        CComboBoxItem();

        const String& GetText() const { return label->GetText(); }

        void SetText(const String& text) { label->SetText(text); }

    protected:

        void Construct() override;

        void HandleEvent(CEvent* event) override;

        FIELD()
        CLabel* label = nullptr;

        FIELD()
        CComboBox* comboBox = nullptr;

        friend class CComboBox;
    };

    CLASS()
    class CRYSTALWIDGETS_API CComboBox : public CWidget
    {
        CE_CLASS(CComboBox, CWidget)
    public:

        CComboBox();

        virtual ~CComboBox();

        bool IsSubWidgetAllowed(Class* subWidgetClass) override;

        u32 GetItemCount() const { return items.GetSize(); }

        CComboBoxItem* GetItem(u32 index) { return items[index]; }

        CPopup* GetComboPopup() const { return comboPopup; }

        int GetSelectedItemIndex() const { return selectedItemIndex; }

        void AddItem(CComboBoxItem* item);
        void RemoveItem(CComboBoxItem* item);

        void OpenPopup();
        void ClosePopup();

        void SelectItem(int index);
        void SelectItem(CComboBoxItem* item);

        // - Signals -

        // Params: int index
        CE_SIGNAL(OnSelectionChanged, int);

    protected:

        void Construct() override;

        void HandleEvent(CEvent* event) override;

        void OnPaint(CPaintEvent* paintEvent) override;

        void OnSubobjectAttached(Object* subobject) override;
        void OnSubobjectDetached(Object* subobject) override;

        FIELD()
        CPopup* comboPopup = nullptr;

        FIELD()
        Array<CComboBoxItem*> items{};

        FIELD()
        CLabel* displayLabel = nullptr;

        FIELD()
        CImage* downardArrow = nullptr;

        FIELD()
        int selectedItemIndex = -1;

        
    };
    
}

#include "CComboBox.rtti.h"
