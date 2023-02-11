#pragma once

namespace CE::Editor
{

    class CRYSTALEDITOR_API EditorLoop
    {
    public:
        EditorLoop() = default;
        ~EditorLoop() = default;

        int RunLoop(int argc, char** argv);

    private:
        void PreInit();
        void PostInit();
        void PreShutdown();
        void Shutdown();
    };
    
} // namespace CE::Editor
