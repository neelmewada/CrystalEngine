
#include "Application/Application.h"

namespace CE
{

    Application::Application(int argc, char** argv)
    {
        ApplicationBus::BusConnect(this);
    }

    Application::~Application()
    {
        ApplicationBus::BusDisconnect(this);
    }

    void Application::RunMainLoop()
    {

    }

    void Application::ExitMainLoop()
    {

    }

} // namespace CE
