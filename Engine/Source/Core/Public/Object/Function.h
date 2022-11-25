#pragma once

#include "RTTI.h"

#include <functional>

namespace CE
{
	class StructType;
	class ClassType;

	template<class _Fty>
	using Function = std::function<_Fty>;

	using FunctionDelegate = std::function<void(void* instance, std::initializer_list<CE::Variant> params, CE::Variant& returnValue)>;

    class CORE_API FunctionType : public TypeInfo
    {
	protected:
		FunctionType(String name, TypeId returnType, std::initializer_list<TypeId> parameterTypes, 
			FunctionDelegate delegate, String attributes)
			: TypeInfo(name)
			, ReturnType(returnType)
			, ParamTypes(parameterTypes)
			, Attributes(attributes)
			, Delegate(delegate)
		{}

	public:

		virtual bool IsFunction() const override { return true; }

		inline TypeId GetReturnTypeId() const { return ReturnType; }
		
		inline u32 GetParameterCount() const { return ParamTypes.GetSize(); }

		inline TypeId GetParameterTypeIdAt(u32 index) const { return ParamTypes[index]; }

		virtual TypeId GetTypeId() const override
		{
			return GetReturnTypeId();
		}

	protected:
		TypeId ReturnType = 0;
		Array<TypeId> ParamTypes{};
		String Attributes = "";
		FunctionDelegate Delegate;

		friend class StructType;
		friend class ClassType;
    };
    
} // namespace CE

