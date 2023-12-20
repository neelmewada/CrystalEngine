#include "CoreRPI.h"

namespace CE::RPI
{

    void PassLibrary::RegisterTemplate(const Name& name, const Ptr<PassTemplate>& passTemplate)
    {
        if (templateRegistry.KeyExists(name))
            return;
        
        TemplateEntry entry{};
        entry.templatePtr = passTemplate;
        
        templateRegistry[name] = entry;
    }

    void PassLibrary::DeregisterTemplate(const Name& name)
    {
        templateRegistry.Remove(name);
    }
    
} // namespace CE::RPI
