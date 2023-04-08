#pragma once

#include "EditorCore.h"

namespace CE::Editor
{
    
    class ASSETPROCESSOR_API AssetProcessorApplication : public Qt::CEQtApplication
    {
    public:
        AssetProcessorApplication(int argc, char** argv);
        virtual ~AssetProcessorApplication();


    };

} // namespace CE::Editor
