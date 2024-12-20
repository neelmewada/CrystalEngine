#include "EditorCore.h"

namespace CE::Editor
{

    EditorBase::EditorBase()
    {

    }

    void EditorBase::Construct()
    {
	    Super::Construct();

        history = CreateObject<EditorHistory>(this, "EditorHistory");

    }

    void EditorBase::HandleEvent(FEvent* event)
    {
        if (event->IsKeyEvent() && !event->isConsumed)
        {
            FKeyEvent* keyEvent = static_cast<FKeyEvent*>(event);

            if (keyEvent->type == FEventType::KeyPress)
            {
                KeyModifier ctrlMod = KeyModifier::Ctrl;
#if PLATFORM_MAC
                ctrlMod = KeyModifier::Gui;
#endif

                if (EnumHasFlag(keyEvent->modifiers, ctrlMod))
                {
                    if (keyEvent->key == KeyCode::Z)
                    {
                        if (history.IsValid())
                        {
                            history->Undo();
                        }

                        event->Consume(this);
                    }
                    else if (keyEvent->key == KeyCode::Y)
                    {
                        if (history.IsValid())
                        {
                            history->Redo();
                        }
                        
                        event->Consume(this);
                    }
                }
            }
        }

	    Super::HandleEvent(event);
    }

}

