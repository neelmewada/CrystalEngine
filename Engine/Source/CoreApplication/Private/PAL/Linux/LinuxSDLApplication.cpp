
#include "CoreApplication.h"

#include <X11/Xlib.h>

namespace CE
{
    LinuxSDLApplication* LinuxSDLApplication::Create()
    {
        return new LinuxSDLApplication();
    }

    void LinuxSDLApplication::Initialize()
    {
        Super::Initialize();

        CalculateDPI();
    }

    LinuxSDLApplication::LinuxSDLApplication()
    {

    }

    void LinuxSDLApplication::CalculateDPI()
    {
        double xres, yres;
        Display *dpy;
        char *displayname = NULL;
        int scr = 0; /* Screen number */

        dpy = XOpenDisplay (displayname);
        /*
         * there are 2.54 centimeters to an inch; so there are 25.4 millimeters.
         *
         *     dpi = N pixels / (M millimeters / (25.4 millimeters / 1 inch))
         *         = N pixels / (M inch / 25.4)
         *         = N * 25.4 pixels / M inch
         */
        xres = ((((double) DisplayWidth(dpy,scr)) * 25.4) /
            ((double) DisplayWidthMM(dpy,scr)));
        yres = ((((double) DisplayHeight(dpy,scr)) * 25.4) /
            ((double) DisplayHeightMM(dpy,scr)));

        int x = (int) (xres + 0.5);
        int y = (int) (yres + 0.5);

        displayScaling = (float)y / 96.0f;

        XCloseDisplay (dpy);
    }
}
