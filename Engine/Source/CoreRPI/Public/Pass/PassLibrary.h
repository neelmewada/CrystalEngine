#pragma once

namespace CE::RPI
{
    
    /// @brief PassLibrary stores and maintains a registry of PassTemplates and Passes
    class CORERPI_API PassLibrary final
    {
    public:
        
        struct TemplateEntry
        {
            Ptr<PassTemplate> templatePtr = nullptr;
            
            /// @brief List of passes instantiated from this template.
            Array<Pass*> passInstances{};
        };
        
        typedef HashMap<Name, TemplateEntry> TemplateEntriesByName;
        
        void RegisterTemplate(const Name& name, const Ptr<PassTemplate>& passTemplate);
        void DeregisterTemplate(const Name& name);
        
    private:
        
        TemplateEntriesByName templateRegistry{};
        
    };
    
} // namespace CE::RPI
