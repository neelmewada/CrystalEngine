#include "Core.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

namespace CE::Mono
{

	ScriptMethod::ScriptMethod(void* impl) : impl(impl)
	{
		
	}

	MonoObject* ScriptMethod::Invoke(void* obj, void** params)
	{
		if (!IsValid())
			return nullptr;
		return mono_runtime_invoke(Get<MonoMethod>(), obj, params, nullptr);
	}

	const char* ScriptMethod::GetMethodName() const
	{
		if (!IsValid())
			return nullptr;
		
		return mono_method_get_name(Get<MonoMethod>());
	}

} // namespace CE::Mono
