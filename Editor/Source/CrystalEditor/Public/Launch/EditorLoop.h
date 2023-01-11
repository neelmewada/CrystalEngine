#pragma once

namespace CE::Editor
{

    class CRYSTALEDITOR_API EditorLoop
    {
    public:
        EditorLoop() = default;
        ~EditorLoop() = default;

        int RunLoop(int argc, char** argv);
    };
    
} // namespace CE::Editor
