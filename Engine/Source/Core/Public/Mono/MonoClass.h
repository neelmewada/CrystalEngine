#pragma once

namespace CE::Mono
{
	enum MonoTypeNameFormat {
		MONO_TYPE_NAME_FORMAT_IL,
		MONO_TYPE_NAME_FORMAT_REFLECTION,
		MONO_TYPE_NAME_FORMAT_FULL_NAME,
		MONO_TYPE_NAME_FORMAT_ASSEMBLY_QUALIFIED
	};

	CORE_API char* TypeGetName(MonoType* monoType);
	CORE_API char* TypeGetFullName(MonoType* monoType, Mono::MonoTypeNameFormat nameFormat);

	class CORE_API ScriptClass final
	{
	public:

		ScriptClass(void* impl = nullptr);

		void IterateAllMethods(const std::function<void(ScriptMethod)>& callback, const bool& stop = false);

		bool IsValid() const { return impl != nullptr; }

		MonoType* GetType() const;

	private:

		template<typename T>
		T* Get() const { return (T*)impl; }

		void* impl = nullptr;

		friend class Assembly;
	};

} // namespace CE::Mono
