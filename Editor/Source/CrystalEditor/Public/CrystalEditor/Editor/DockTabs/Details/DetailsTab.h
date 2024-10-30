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

        void SetSelectedActor(Actor* actor);

    public: // - Fusion Properties -


    protected:

        FLabel* emptyLabel = nullptr;
        FVerticalStack* editorContainer = nullptr;
        ObjectEditor* editor = nullptr;

        FLabel* actorName = nullptr;
        ComponentTreeView* treeView = nullptr;

        FUSION_WIDGET;
    };
    
}

#include "DetailsTab.rtti.h"
