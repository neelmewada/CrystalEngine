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

        void SetEditTarget(Object* target);

        void Construct() override;

        void HandleEvent(CEvent* event) override;

        FIELD()
        Actor* selectedActor = nullptr;

        FIELD()
        CTreeWidget* treeWidget = nullptr;

        FIELD()
        CSplitView* splitView = nullptr;

        FIELD()
        CLabel* noSelectionLabel = nullptr;

        FIELD()
        Object* targetObject = nullptr;

        FIELD()
        ObjectEditor* objectEditor = nullptr;

        FIELD()
        CLabel* titleLabel = nullptr;

        friend class SceneEditorWindow;
    };
    
} // namespace CE::Editor

#include "DetailsWindow.rtti.h"