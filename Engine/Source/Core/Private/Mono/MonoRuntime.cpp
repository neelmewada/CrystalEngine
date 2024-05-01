#include "Core.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

#if PLATFORM_WINDOWS
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "version.lib")
#pragma comment(lib, "bcrypt.lib")
#endif

namespace CE::Mono
{
    static MonoDomain* domain = nullptr;
    static MonoDomain* appDomain = nullptr;

    // ----------------------------------
    // - ScriptRuntime

    Assembly* ScriptRuntime::scriptCoreAssembly = nullptr;

    void ScriptRuntime::Initialize()
    {
        String libDir = (PlatformDirectories::GetLaunchDir() / "Mono/lib").GetString();
        String etcDir = (PlatformDirectories::GetLaunchDir() / "Mono/etc").GetString();
        
        mono_set_dirs(libDir.GetCString(), etcDir.GetCString());

        domain = mono_jit_init("MonoJitDomain");
        
        char appDomainName[] = "CrystalScriptDomain";
        appDomain = mono_domain_create_appdomain(appDomainName, nullptr);
        mono_domain_set(appDomain, true);

        scriptCoreAssembly = new Assembly((PlatformDirectories::GetLaunchDir() / "ScriptCore.dll").GetString());
    }

    void ScriptRuntime::Shutdown()
    {
        //delete scriptCoreAssembly; scriptCoreAssembly = nullptr;

        // TODO: cleanup is causing a crash
        //mono_domain_free(appDomain, true); appDomain = nullptr;

        // TODO: cleanup is causing a crash
        mono_jit_cleanup(domain); domain = nullptr;
    }

    // ----------------------------------
    // - Assembly

    Assembly::Assembly(const String& path)
    {
        impl = mono_domain_assembly_open(domain, path.GetCString());
    }

    Assembly::~Assembly()
    {
        mono_assembly_close(Get<MonoAssembly>());
        impl = nullptr;
    }

    void Assembly::AddInternalCall(const char* name, const void* method)
    {
        mono_add_internal_call(name, method);
    }

    ScriptClass Assembly::FindClass(const char* nameSpace, const char* className)
    {
        MonoImage* image = mono_assembly_get_image(Get<MonoAssembly>());

        ScriptClass result{};
        result.impl = mono_class_from_name(image, "CE", "SampleClass");
        return result;
    }

    //---------------------------------------
    // - String

    CORE_API MonoString* NewString(const char* text)
    {
        return mono_string_new(mono_domain_get(), text);
    }

    CORE_API int GetStringLength(MonoString* string)
    {
        return mono_string_length(string);
    }

    CORE_API char* StringToUTF8(MonoString* string)
    {
        return mono_string_to_utf8(string);
    }

} // namespace CE::Mono
