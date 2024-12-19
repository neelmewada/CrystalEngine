#include "EditorCore.h"

namespace CE::Editor
{

    EditorOperation::EditorOperation()
    {

    }

    void Example()
    {
        // Operation scenarios:
        // - Delete component from an actor
        // - Delete actor
        // - Delete asset
        // - Create a component and add it to an actor
        // - Create an actor and attach it as a child to another actor
        // - Modify field of a class/struct

        Name operation = "Delete_Object";

        [[maybe_unused]]
        auto execute = [operation](const Ref<EditorOperation>& self)
            {
                if (Ref<EditorHistory> history = self->history.Lock())
                {
                    // Serialize the object to disk before deleting

                    String bundleName = String::Format("{}-{}", operation, self->GetUuid());
                    self->bundleSavePath = "/Temp/EditorHistory/" + history->GetUuid().ToString() + "/" + bundleName;

                    Ref<Bundle> bundle = CreateObject<Bundle>(nullptr, bundleName);

                    for (const auto& objectWeakRef : self->targets)
                    {
                        if (auto lock = objectWeakRef.Lock())
                        {
                            bundle->AttachSubobject(lock.Get());
                        }
                    }

                    Bundle::SaveToDisk(bundle, nullptr, self->bundleSavePath);

                    for (const auto& objectWeakRef : self->targets)
                    {
                        if (auto lock = objectWeakRef.Lock())
                        {
                            lock->BeginDestroy();
                        }
                    }
                }
            };

        [[maybe_unused]]
        auto unexecute = [](const Ref<EditorOperation>& self)
            {
                // Deserialize the object back from disk

                if (Ref<EditorHistory> history = self->history.Lock())
                {
                    Ref<Bundle> bundle = Bundle::LoadBundle(nullptr, self->bundleSavePath,
                        { .loadFully = true, .forceReload = true, .destroyOutdatedObjects = true});

                    for (int i = 0; i < bundle->GetSubObjectCount(); ++i)
                    {
                        self->targets.Add(bundle->GetSubObject(i));
                        if (i == 0)
                        {
                            self->target = bundle->GetSubObject(i);
                        }
                    }
                }
            };
    }
    
} // namespace CE::Editor
