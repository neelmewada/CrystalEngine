#pragma once

namespace CE::Mono
{
	CORE_API MonoString* NewString(const char* text);
	CORE_API int GetStringLength(MonoString* string);
	CORE_API char* StringToUTF8(MonoString* string);

	class CORE_API Assembly final
	{
	public:

		~Assembly();

		void AddInternalCall(const char* name, const void* method);

		ScriptClass FindClass(const char* nameSpace, const char* className);

	private:

		Assembly(const String& path);

		template<typename T>
		T* Get() const { return (T*)impl; }

		void* impl = nullptr;

		friend class ScriptRuntime;
	};

	class CORE_API ScriptRuntime
	{
		CE_NO_COPY(ScriptRuntime)
		CE_NO_MOVE(ScriptRuntime)
	private:
		ScriptRuntime() = delete;

	public:

		static void Initialize();

		static void Shutdown();

		static Assembly* GetScriptCoreAssembly() {
			return scriptCoreAssembly;
		}

	private:

		static Assembly* scriptCoreAssembly;
	};

} // namespace CE
