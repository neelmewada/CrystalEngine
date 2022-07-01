#pragma once

#include "EngineDefs.h"

namespace Vox
{

class ENGINE_API IRenderContext
{
protected:
    IRenderContext() {}
public:
    virtual ~IRenderContext() {}

public: // Public API
    // - Setup
    virtual void SetClearColor(float clearColor[4]) = 0;

    // - Commands
    virtual void ReRecordCommands() = 0;
    virtual void BeginRecording() = 0;
    virtual void EndRecording() = 0;
};

}
