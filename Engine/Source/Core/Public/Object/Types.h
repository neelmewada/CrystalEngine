#pragma once

#include "Misc/CoreDefines.h"
#include "Containers/String.h"
#include "Containers/HashMap.h"

#include <type_traits>
#include <any>

namespace CE
{

	enum class FieldBaseType
	{
		Undefined,
		u8, u16, u32, u64,
		s8, s16, s32, s64,
		Bool, f32, f64,
		String,
		Enum, Struct, Class, Object
	};

	enum class FieldType
	{
		Plain,
		Pointer,
		Array,
		ArrayPtr,
		Function,
	};

	// Forward Declarations

	class CORE_API TypeInfo;
	class CORE_API ClassType;
	class CORE_API StructType;
	class CORE_API PropertyType;
	class CORE_API FunctionType;
	class CORE_API EnumType;

	// **********************************************************
	// Any

	class Any
	{
	public:
		Any() : bHasValue(false)
		{}

		inline bool HasValue()
		{
			return bHasValue;
		}

	private:
		union
		{
			bool boolValue;
			String stringValue;
			s8 s8Value;
			s16 s16Value;
		};

		bool bHasValue = false;
	};

	// **********************************************************
	// Type Info

	class CORE_API TypeInfo
	{
	public:
		TypeInfo(const char* name, CE::FieldType type, CE::FieldBaseType underlyingType, String underlyingTypePath)
			: Name(name), Type(type), UnderlyingType(underlyingType), UnderlyingTypePath(underlyingTypePath)
		{}

		virtual ~TypeInfo()
		{}

		const char* GetName() { return Name; }

		CE::FieldType GetType() { return Type; }
		CE::FieldBaseType GetUnderlyingType() { return UnderlyingType; }
		const char* GetUnderlyingTypePath() { return UnderlyingTypePath.GetCString(); }

		virtual bool IsStructType() = 0;

		virtual bool IsClassType() = 0;

		virtual bool IsFunctionType() = 0;

		virtual bool IsPropertyType() = 0;

		virtual bool IsParameterType() = 0;

	private:
		const char* Name;
		CE::FieldType Type;
		CE::FieldBaseType UnderlyingType;
		String UnderlyingTypePath;
	};


	// **********************************************************
	// Property Type

	struct PropertyInitializer
	{
		const char* Name;
		const char* OwnerPath;
		u32 Offset;
		u32 Size;
		CE::FieldType Type;
		CE::FieldBaseType UnderlyingType;
		const char* UnderlyingTypePath;
		const char* Attributes = nullptr;
	};

	class CORE_API PropertyType : public TypeInfo
	{
	public:
		PropertyType(PropertyInitializer init) 
			: TypeInfo(init.Name, init.Type, init.UnderlyingType, init.UnderlyingTypePath)
			, Offset(init.Offset), Size(init.Size)
			, OwnerPath(init.OwnerPath)
			, Attributes(init.Attributes)
		{}

		CE_NO_COPY(PropertyType);

		bool IsStructType() override
		{
			return true;
		}

		bool IsClassType() override
		{
			return false;
		}

		bool IsFunctionType() override
		{
			return false;
		}

		bool IsPropertyType() override
		{
			return true;
		}

		bool IsParameterType() override
		{
			return false;
		}

		template<typename PropertyType>
		inline PropertyType& GetValue(const void* instance)
		{
			return *(PropertyType*)((SIZE_T)instance + Offset);
		}

		template<typename PropertyType>
		inline void SetValue(void* instance, const PropertyType& value)
		{
			*(PropertyType*)((SIZE_T)instance + Offset) = value;
		}

		template<>
		inline void SetValue<String>(void* instance, const String& value)
		{
			String* ptr = (String*)((SIZE_T)instance + Offset);
			*ptr = value;
		}

	private:
		const char* OwnerPath;
		u32 Offset;
		u32 Size;
		const char* Attributes = nullptr;

		Array<String> AttributeList{};
	};


	// **********************************************************
	// Struct Type

	struct StructInitializer
	{
		const char* Name;
		CE::FieldType Type; 
		CE::FieldBaseType UnderlyingType;
		const char* UnderlyingTypePath;
	};

	class CORE_API StructType : public TypeInfo
	{
	public:
		StructType(StructInitializer init)
			: TypeInfo(init.Name, init.Type, init.UnderlyingType, init.UnderlyingTypePath)
		{}

		virtual ~StructType();

		CE_NO_COPY(StructType);

		bool IsStructType() override
		{
			return true;
		}

		bool IsClassType() override
		{
			return false;
		}

		bool IsFunctionType() override
		{
			return false;
		}

		bool IsPropertyType() override
		{
			return false;
		}

		bool IsParameterType() override
		{
			return false;
		}

		u32 GetPropertyCount();
		PropertyType* GetPropertyAt(u32 index);

		u32 GetFunctionCount();
		FunctionType* GetFunctionAt(u32 index);

	protected:
		CE::Array<CE::PropertyType*> Properties{};
		CE::Array<CE::FunctionType*> Functions{};
	};


	// **********************************************************
	// Parameter Type

	struct ParameterInitializer
	{
		const char* ParameterName;
		bool bIsReturnParam;
		u32 Size;
		CE::FieldType Type;
		CE::FieldBaseType UnderlyingType;
		const char* UnderlyingTypePath;
		const char* Attributes = "";
	};

	class CORE_API ParameterType : public TypeInfo
	{
	public:
		ParameterType(ParameterInitializer init)
			: Size(init.Size)
			, Attributes(init.Attributes)
			, bIsReturnParam(init.bIsReturnParam)
			, TypeInfo(init.ParameterName, init.Type, init.UnderlyingType, init.UnderlyingTypePath)
		{

		}

		bool IsStructType() override
		{
			return false;
		}

		bool IsClassType() override
		{
			return false;
		}

		bool IsFunctionType() override
		{
			return false;
		}

		bool IsPropertyType() override
		{
			return true;
		}

		bool IsParameterType() override
		{
			return true;
		}

		inline bool IsReturnParameter()
		{
			return bIsReturnParam;
		}

		inline u32 GetPosition()
		{
			return Position;
		}

	private:
		bool bIsReturnParam;
		u32 Position = 0;
		u32 Size;
		const char* Attributes;

		friend class FunctionType;
	};


	// **********************************************************
	// Function Type

	typedef std::function<void(void* instance, std::initializer_list<std::any> params, std::any& result)> MetaFunction;

	struct FunctionInitializer
	{
		const char* Name;
		// Full type path with namespace and owner class
		String FunctionPath;
		bool bIsStatic;
		MetaFunction Function = nullptr;
		std::initializer_list<CE::ParameterType*> ParameterTypes;
		CE::ParameterType* ReturnType = nullptr;
		const char* Attributes = "";
	};

	class CORE_API FunctionType : public TypeInfo
	{
	public:
		FunctionType(FunctionInitializer init) 
			: TypeInfo(init.Name, FieldType::Function, FieldBaseType::Undefined, init.FunctionPath)
			, Name(init.Name), Function(init.Function), bIsStatic(init.bIsStatic)
			, ParameterTypes(init.ParameterTypes), ReturnType(init.ReturnType)
		{
			s32 idx = 0;
			for (auto paramType : ParameterTypes)
			{
				if (paramType != nullptr)
				{
					paramType->Position = idx;
					idx++;
				}
			}
		}

		CE_NO_COPY(FunctionType);

		bool IsStructType() override
		{
			return false;
		}

		bool IsClassType() override
		{
			return false;
		}

		bool IsFunctionType() override
		{
			return true;
		}

		bool IsPropertyType() override
		{
			return false;
		}

		bool IsParameterType() override
		{
			return false;
		}

		bool IsStatic()
		{
			return bIsStatic;
		}

		std::any Invoke(void* instance, std::initializer_list<std::any> params)
		{
			std::any result{};
			Function(instance, params, result);
			return result;
		}

		template<typename ReturnType, typename... Args>
		ReturnType Invoke(void* instance, Args... args)
		{
			return std::any_cast<ReturnType>(Invoke(instance, { args... }));
		}

	private:
		const char* Name;
		bool bIsStatic;
		MetaFunction Function = nullptr;
		CE::Array<CE::ParameterType*> ParameterTypes{};
		CE::ParameterType* ReturnType = nullptr;
	};


	// **********************************************************
	// Class Type

	struct ClassInitializer
	{
		const char* Name;
		u32 Size;
		CE::FieldType Type;
		CE::FieldBaseType UnderlyingType;
		const char* ClassPath; // ex: SomeNamespace::ChildNamespace::SomeClass
		const char* Attributes = nullptr;
	};

	class CORE_API ClassType : public StructType
	{
	public:
		ClassType(ClassInitializer init)
			: StructType(StructInitializer{ init.Name, init.Type, init.UnderlyingType, init.ClassPath })
		{}

		bool IsStructType() override
		{
			return false;
		}

		bool IsClassType() override
		{
			return true;
		}

		static ClassType* FindClass(String classPath);

		static void RegisterClassType(ClassType* classType);
		static void UnregisterClassType(ClassType* classType);

	private:
		static HashMap<String, ClassType*> RegisteredClasses;
	};


	// **********************************************************
	// Enum Type

	class CORE_API EnumConstant
	{
	public:

	private:

	};

	class CORE_API EnumType : TypeInfo
	{
	public:
		EnumType(const char* name, const char* enumTypePath) : TypeInfo(name, FieldType::Plain, FieldBaseType::Enum, enumTypePath)
		{}

		virtual CE::FieldBaseType GetEnumUnderlyingType() = 0;

	private:
		
	};

} // namespace CE
