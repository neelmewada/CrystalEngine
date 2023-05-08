#pragma once

#include "Types/CoreTypeDefs.h"
#include "Misc/CoreDefines.h"
#include "Misc/CoreMacros.h"
#include "Containers/Array.h"
#include "Containers/HashMap.h"
#include "Containers/String.h"
#include "IO/Path.h"

#include "RTTIDefines.h"
#include "AutoRTTI.h"

#include "Types/Name.h"
#include "Variant.h"


namespace CE
{
	// Forward Decls

	template<typename ElementType>
	class Array;

	class Name;
	struct ModuleInfo;
	class CoreModule;

	namespace Internal
	{
		template<typename Struct>
		struct TypeInfoImpl;
	}

	template<typename Type>
	CE::Name GetTypeName()
	{
		if (TYPEID(Type) == TYPEID(Array<u8>))
		{
			return GetTypeName<Array<u8>>();
		}

		// Specialization contains the magical data
		return "";
	}

	// **********************************************************
	// Attribute

	struct AttributeTable;

	struct CORE_API Attribute
	{
	public:
		Attribute()
		{
			
		}

		Attribute(String str)
		{
			isString = true;
			stringValue = str;
		}

		Attribute(const HashMap<Name, Attribute>& tableRef)
		{
			isMap = true;
			tableValue = tableRef;
		}

		Attribute(const Attribute& copy)
		{
			isString = copy.isString;
			isMap = copy.isMap;
			
			if (isString)
			{
				stringValue = copy.stringValue;
			}
			else if (isMap)
			{
				tableValue = copy.tableValue;
			}
		}

		~Attribute()
		{

		}

		inline Attribute operator=(const Attribute& copy)
		{
			return Attribute(copy);
		}

		inline Attribute& operator[](const Name& key)
		{
			return tableValue[key];
		}

		bool IsString() const;
		bool IsMap() const;

		inline operator String() const
		{
			return GetStringValue();
		}

		String GetStringValue() const
		{
			if (!IsString())
				return "";
			return stringValue;
		}

		HashMap<Name, Attribute>& GetTableValue()
		{
			return tableValue;
		}

		const HashMap<Name, Attribute>& GetTableValue() const
		{
			return tableValue;
		}

		bool HasKey(const Name& key) const
		{
			return tableValue.KeyExists(key);
		}

		Attribute& GetKeyValue(const Name& key)
		{
			return tableValue[key];
		}

		static void Parse(String attributes, Attribute& outAttrib, bool cleanup = true);
		static String CleanupAttributeString(const String& inString);

	private:

		String stringValue{};
		HashMap<Name, Attribute> tableValue{};

		bool isString = false;
		bool isMap = false;

		friend class TypeInfo;
		friend class StructType;
		friend class ClassType;
	};

	// **********************************************************
	// Type Info

	class CORE_API TypeInfo
	{
	protected:
		TypeInfo(CE::Name name, CE::String attributes = "");
        
        template<typename Struct>
        friend struct TypeInfoImpl;

	public:
		const CE::Name& GetName() const { return name; }
		//const CE::Array<CE::Attribute>& GetLocalAttributes() const { return attributes; }

        virtual const Attribute& GetAttributes();

		virtual String GetDisplayName();

		//String GetLocalAttributeValue(const String& key) const;
        //bool HasLocalAttribute(const String& key) const;

        virtual Attribute GetAttribute(const String& key);
        virtual bool HasAttribute(const String& key);
        
		virtual bool IsClass() const { return false; }
		virtual bool IsStruct() const { return false; }
		virtual bool IsField() const { return false; }
		virtual bool IsFunction() const { return false; }
		virtual bool IsEnum() const { return false; }
		virtual bool IsEnumConstant() const { return false; }

		virtual bool IsPOD() const { return false; }

		virtual bool IsAssignableTo(TypeId typeId) const { return false; }

		virtual TypeId GetTypeId() const = 0;
		virtual u32 GetSize() const = 0;
        
        virtual CE::TypeId GetUnderlyingTypeId() const { return 0; }

		virtual bool IsArrayType() const { return this->GetTypeId() == TYPEID(Array<u8>); }
        virtual bool IsObjectStoreType() const { return this->GetTypeId() == TYPEID(ObjectStore); }
        
        virtual bool IsObject() const { return IsClass() && IsAssignableTo(TYPEID(Object)); }
		virtual bool IsComponent() const;

		Name GenerateInstanceName(UUID uuid) const;

		Name GetOwnerModuleName() const;

	protected:
		CE::Name name;
		CE::String displayName{};
		//CE::Array<CE::Attribute> attributes{};
		Attribute attributes{};
        Name registeredModuleName{};

	public:
		// For internal use only!
        static HashMap<CE::Name, TypeInfo*> registeredTypesByName;
		// For internal use only!
		static HashMap<TypeId, TypeInfo*> registeredTypeById;

        // For internal use only!
        static void RegisterType(TypeInfo* type);
		// For internal use only!
		static void DeregisterType(TypeInfo* type);
		// For internal use only!
        CE_INLINE static u32 GetRegisteredCount()
        {
            return (u32)registeredTypesByName.GetSize();
        }

	private:
		friend class ModuleManager;
		friend class CE::CoreModule;

		static void DeregisterTypesForModule(ModuleInfo* moduleInfo);

		static Array<Name> currentlyLoadingModuleStack;
		static Array<Name> currentlyUnloadingModuleStack;

		static HashMap<Name, Array<TypeInfo*>> registeredTypesByModuleName;
	};

    template <typename T>
    struct TemplateType : TFalseType
    {
        typedef void DefaultArg;
        typedef T DefaultTemplate;
    };

    template <typename T, template <typename> class U>
    struct TemplateType<U<T>> : TTrueType
    {
        typedef T DefaultArg;
        typedef U<T> DefaultTemplate;
    };

	/// Default implementation always returns nullptr. Specialization will return the correct data.
	/// Returns the type info of the specified type at compile time.
	template<typename Type>
	TypeInfo* GetStaticType()
	{
        if constexpr (TemplateType<Type>::Value)
        {
            return GetStaticType<typename TemplateType<Type>::DefaultTemplate>();
        }

		return nullptr;
	}

	/// Returns dynamic type info of the type with specified name. Ex:  GetTypeInfo("ParentNamespace::ChildNamespace::SomeClass")
    CORE_API TypeInfo* GetTypeInfo(CE::Name name);

	/// Returns dynamic type info of the type with specified type id.
	CORE_API TypeInfo* GetTypeInfo(TypeId typeId);

	// Specialization will contain the magic data.
	template<typename T>
	struct StructTypeData
	{};

	template<typename... Args>
    CE_INLINE void RegisterTypes()
	{
		std::initializer_list<TypeInfo*> types = { GetStaticType<Args>()... };
		for (auto typeInfo : types)
		{
			TypeInfo::RegisterType(typeInfo);
		}
	}

	template<>
	CE_INLINE void RegisterTypes()
	{}

	template<typename... Args>
	CE_INLINE void DeregisterTypes()
	{
		std::initializer_list<TypeInfo*> types = { GetStaticType<Args>()... };
		for (auto typeInfo : types)
		{
			TypeInfo::DeregisterType(typeInfo);
		}
	}

	template<>
	CE_INLINE void DeregisterTypes()
	{}
	
} // namespace CE

// Simple data types

CE_RTTI_POD(CORE_API, , b8, TYPEID(s8), TYPEID(u8))

CE_RTTI_POD(CORE_API, , s8, TYPEID(u8))
CE_RTTI_POD(CORE_API, , s16, TYPEID(u16))
CE_RTTI_POD(CORE_API, , s32, TYPEID(u32))
CE_RTTI_POD(CORE_API, , s64, TYPEID(u64))

CE_RTTI_POD(CORE_API, , u8, TYPEID(s8))
CE_RTTI_POD(CORE_API, , u16, TYPEID(s16))
CE_RTTI_POD(CORE_API, , u32, TYPEID(s32))
CE_RTTI_POD(CORE_API, , u64, TYPEID(s64))
CE_RTTI_POD(CORE_API, CE, UUID, TYPEID(u64), TYPEID(s64))

CE_RTTI_POD(CORE_API, , f32)
CE_RTTI_POD(CORE_API, , f64)

CE_RTTI_POD(CORE_API, CE, String)
CE_RTTI_POD(CORE_API, CE, Name)
CE_RTTI_POD(CORE_API, CE::IO, Path)

CE_RTTI_POD_TEMPLATE(CORE_API, CE, Array, u8)


