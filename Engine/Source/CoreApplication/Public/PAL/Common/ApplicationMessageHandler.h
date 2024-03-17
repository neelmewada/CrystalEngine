#pragma once

namespace CE
{

    class ApplicationMessageHandler
    {
    public:

        virtual ~ApplicationMessageHandler() = default;

        virtual void OnMainWindowDrawableSizeChanged(u32 width, u32 height)
        {

        }

        virtual void ProcessNativeEvents(void* nativeEvent)
        {

        }

        virtual void ProcessInputEvents(void* nativeEvent)
        {

        }
    };
    
} // namespace CE

