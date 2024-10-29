#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API DetailsTab : public EditorMinorDockTab
    {
        CE_CLASS(DetailsTab, EditorMinorDockTab)
    protected:

        DetailsTab();

        void Construct() override;

    public: // - Public API -

        void SetObjectEditor(ObjectEditor* editor);

    public: // - Fusion Properties -


    protected:

        FLabel* emptyLabel = nullptr;
        //FStyledWidget* editorContainer = nullptr;
        ObjectEditor* editor = nullptr;

        FLabel* actorName = nullptr;
        ComponentTreeView* treeView = nullptr;

        FUSION_WIDGET;
    };
    
}

#include "DetailsTab.rtti.h"
