#include "Core.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

namespace CE::Mono
{
	

	CORE_API char* TypeGetName(MonoType* monoType)
	{
		return mono_type_get_name(monoType);
	}

	CORE_API char* TypeGetFullName(MonoType* monoType, Mono::MonoTypeNameFormat nameFormat)
	{
		return mono_type_get_name_full(monoType, (::MonoTypeNameFormat)nameFormat);
	}

	ScriptClass::ScriptClass(void* impl) : impl(impl)
	{
		
	}


	void ScriptClass::IterateAllMethods(const std::function<void(ScriptMethod)>& callback, const bool& stop)
	{
		if (impl == nullptr)
			return;

		void* iter = nullptr;
		MonoMethod* method;

		while ((method = mono_class_get_methods(Get<MonoClass>(), &iter)) != nullptr)
		{
			if (stop)
				return;

			callback(ScriptMethod(method));
		}
	}

	MonoType* ScriptClass::GetType() const
	{
		return mono_class_get_type(Get<MonoClass>());
	}

} // namespace CE::Mono
