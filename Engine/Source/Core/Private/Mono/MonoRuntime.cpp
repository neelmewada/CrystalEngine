#include "Core.h"

#include <mono/jit/jit.h>
#include <mono/metadata/tokentype.h>
#include <mono/metadata/metadata.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/class.h>
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

        if (domain == nullptr)
        {
            domain = mono_jit_init("MonoJitDomain");
        }
        
        char appDomainName[] = "CrystalScriptDomain";
        appDomain = mono_domain_create_appdomain(appDomainName, nullptr);
        mono_domain_set(appDomain, true);

        scriptCoreAssembly = new Assembly((PlatformDirectories::GetLaunchDir() / "ScriptCore.dll").GetString());
    }

    void ScriptRuntime::Shutdown()
    {
        //delete scriptCoreAssembly; scriptCoreAssembly = nullptr;

        // TODO: cleanup is causing a crash
        mono_domain_free(appDomain, true); appDomain = nullptr;

        //mono_jit_cleanup(domain); domain = nullptr;
    }

    // ----------------------------------
    // - Assembly

    Assembly::Assembly(const String& path)
    {
        impl = mono_domain_assembly_open(domain, path.GetCString());
        MonoAssemblyName* monoName = mono_assembly_get_name(Get<MonoAssembly>());
        const char* assemblyName = mono_assembly_name_get_name(monoName);

        MonoImage* image = mono_assembly_get_image(Get<MonoAssembly>());
        const MonoTableInfo* tableInfo = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);

        int numRows = mono_table_info_get_rows(tableInfo);
        MonoClass* rttiHelper = mono_class_from_name(image, "CE", "RttiHelper");

        for (int i = 0; i < numRows; ++i)
        {
            MonoClass* clazz = mono_class_get(image, MONO_TOKEN_TYPE_DEF | (i + 1));
            String className = mono_class_get_name(clazz);
            int numFields = mono_class_num_fields(clazz);
            MonoType* classType = mono_class_get_type(clazz);
            char* assemblyQualifiedName = mono_type_get_name_full(classType, ::MONO_TYPE_NAME_FORMAT_ASSEMBLY_QUALIFIED);
            MonoString* assemblyQualifiedNameStr = mono_string_new(mono_domain_get(), assemblyQualifiedName);

            if (className == "CustomObject" && rttiHelper)
            {
                MonoMethod* getFields = mono_class_get_method_from_name(rttiHelper, "GetFields", 1);

                void* params[1] = { assemblyQualifiedNameStr };

                MonoObject* exc = nullptr;
                MonoArray* retVal = (MonoArray*)mono_runtime_invoke(getFields, nullptr, params, &exc);

                if (retVal != nullptr)
                {
                    uintptr_t length = mono_array_length(retVal);

                    for (int j = 0; j < length; ++j)
                    {
                        MonoObject* val = mono_array_get(retVal, MonoObject*, j);
                        if (val)
                        {
                            MonoClass* objClass = mono_object_get_class(val);
                            const char* name = mono_class_get_name(objClass);
                            
                            String::IsAlphabet('a');
                        }
                    }
                }
            }

            mono_free(assemblyQualifiedName);
        }
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
