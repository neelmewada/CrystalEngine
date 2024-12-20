#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API EditorBase : public EditorDockTab
    {
        CE_CLASS(EditorBase, EditorDockTab)
    protected:

        EditorBase();

        void Construct() override;

        void HandleEvent(FEvent* event) override;

    public: // - Public API -

        const Ref<EditorHistory>& GetHistory() const { return history; }

        bool SupportsKeyboardEvents() const override { return true; }

    protected: // - Internal -

        Ref<EditorHistory> history = nullptr;

    public: // - Fusion Properties - 

        FUSION_WIDGET;
    };
    
}

#include "EditorBase.rtti.h"
