#include "Core.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

namespace CE::Mono
{

	CORE_API MonoString* ObjectToString(MonoObject* object, MonoObject** exception)
	{
		return mono_object_to_string(object, exception);
	}


} // namespace CE::Mono
