#pragma once

namespace Vox
{

class IRenderContext
{
protected:
    IRenderContext() {}
public:
    virtual ~IRenderContext() {}

public: // Public API
    // - Setup
    virtual void SetClearColor(float clearColor[4]) = 0;

    // - Commands
    virtual void BeginRecording() = 0;
    virtual void EndRecording() = 0;
};

}
