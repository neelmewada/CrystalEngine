
#include "CoreMesh.h"

#include "CoreMesh.private.h"


namespace CE
{
    class CoreMeshModule : public CE::Module
    {
    public:
        virtual void StartupModule() override
        {

        }

        virtual void ShutdownModule() override
        {

        }

        virtual void RegisterTypes() override
        {

        }
    };
}

CE_IMPLEMENT_MODULE(CoreMesh, CE::CoreMeshModule)
