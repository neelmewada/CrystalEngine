#pragma once

namespace CE::Editor
{

    CLASS()
    class CRYSTALEDITOR_API DetailsWindow final : public MinorEditorWindow
    {
        CE_CLASS(DetailsWindow, MinorEditorWindow)
    public:

        DetailsWindow();

        virtual ~DetailsWindow();

        FUNCTION()
        void SetupForActor(Actor* actor);

    private:

        void Construct() override;

        void HandleEvent(CEvent* event) override;

        FIELD()
        CTreeWidget* treeWidget = nullptr;

        FIELD()
        CSplitView* splitView = nullptr;

        FIELD()
        CLabel* noSelectionLabel = nullptr;

        friend class SceneEditorWindow;
    };
    
} // namespace CE::Editor

#include "DetailsWindow.rtti.h"