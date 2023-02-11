#pragma once

#include "Containers/Array.h"
#include "RTTI.h"

#include <functional>

namespace CE
{
	class StructType;
	class ClassType;
	class Object;

	template<class _Fty>
	using Function = std::function<_Fty>;

	using FunctionDelegate = Function<void(CE::Object* instance, CE::Array<CE::Variant> params, CE::Variant& returnValue)>;

    class CORE_API FunctionType : public TypeInfo
    {
	protected:
		FunctionType(String name, TypeId returnType, std::initializer_list<TypeId> parameterTypes, 
			FunctionDelegate delegate, const TypeInfo* owner, String attributes)
			: TypeInfo(name)
			, returnType(returnType)
			, paramTypes(parameterTypes)
			, attributes(attributes)
			, delegateCallback(delegate)
			, owner(owner)
		{}

	public:

		virtual bool IsFunction() const override { return true; }

		inline TypeId GetReturnTypeId() const { return returnType; }
		
		inline u32 GetParameterCount() const { return paramTypes.GetSize(); }

		inline TypeId GetParameterTypeIdAt(u32 index) const { return paramTypes[index]; }

		virtual TypeId GetTypeId() const override
		{
			return GetReturnTypeId();
		}

		inline FunctionType* GetNext() const
		{
			return Next;
		}

		inline CE::Variant Invoke(CE::Object* instance, Array<CE::Variant> params) const
		{
			CE::Variant returnValue{};
			delegateCallback(instance, params, returnValue);
			return returnValue;
		}

		virtual u32 GetSize() const override { return 0; }

	protected:
		TypeId returnType = 0;
		Array<TypeId> paramTypes{};
		String attributes = "";
		FunctionDelegate delegateCallback;
		FunctionType* Next = nullptr;
		const TypeInfo* owner = nullptr;

		friend class StructType;
		friend class ClassType;
    };
    
} // namespace CE

