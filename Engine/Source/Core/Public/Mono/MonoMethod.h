#pragma once

namespace CE::Mono
{

	class CORE_API ScriptMethod
	{
	public:

		ScriptMethod(void* impl = nullptr);

		MonoObject* Invoke(void* obj, void** params);

		const char* GetMethodName() const;

		bool IsValid() const { return impl != nullptr; }

	private:

		template<typename T>
		T* Get() const { return (T*)impl; }

		void* impl = nullptr;

	};

} // namespace CE::Mono
