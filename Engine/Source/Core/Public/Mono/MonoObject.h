#pragma once


namespace CE::Mono
{

	CORE_API MonoString* ObjectToString(MonoObject* object, MonoObject** exception = nullptr);

} // namespace CE::Mono
