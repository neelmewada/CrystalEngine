
//#import <Foundation/Foundation.h>
//#import <CoreFoundation/CoreFoundation.h>
//#import <Cocoa/Cocoa.h>

#include "EditorCore.h"

#include <string>

namespace CE::Editor
{

    IO::Path MacEditorPlatform::ShowSelectDirectoryDialog(const IO::Path& defaultPath)
    {
        IO::Path result{};
        /*NSOpenPanel* panel = [NSOpenPanel openPanel];
        [panel setCanChooseDirectories:YES];
        [panel setCanChooseFiles:NO];
        [panel setCanCreateDirectories:YES];
        [panel setAllowsMultipleSelection:NO];
        [panel setTitle:@"Select folder"];
        
        NSInteger ret = [panel runModal];
        
        if (ret == NSModalResponseOK)
        {
            NSURL* url = [panel URL];
            result = std::string([[url path] UTF8String]);
        }*/
        
        return result;
    }

    IO::Path MacEditorPlatform::ShowFileSelectionDialog(const IO::Path& defaultPath, const Array<FileType>& inFileTypes)
    {
        IO::Path result{};
        return result;
    }

} // namespace CE::Editor
