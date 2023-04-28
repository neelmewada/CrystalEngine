#pragma once

#include "Misc/CoreDefines.h"
#include "Misc/CoreMacros.h"
#include "Containers/Array.h"
#include "Containers/HashMap.h"
#include "Containers/String.h"
#include "IO/Path.h"

#include "Types/CoreTypeDefs.h"

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
			Clear();
		}

		Attribute(String str)
		{
			Clear();
			isString = true;
			stringValue = str;
		}

		Attribute(const HashMap<Name, Attribute>& tableRef)
		{
			Clear();
			isAttributeTable = true;
			subTable = tableRef;
		}

		~Attribute()
		{

		}

		bool IsString() const;
		bool IsAttributeTable() const;

		String GetStringValue() const
		{
			if (!isString)
				return "";
			return stringValue;
		}

	private:

		void Clear()
		{
			memset(this, 0, sizeof(Attribute));
		}

		union {
			String stringValue{};
			HashMap<Name, Attribute> subTable;
		};

		bool isString = false;
		bool isAttributeTable = false;

		friend class TypeInfo;
		friend class StructType;
		friend class ClassType;
	};

	struct CORE_API AttributeTable
	{
	public:
		AttributeTable()
		{}

		static void Parse(String attributeString, AttributeTable& outTable);
		static String CleanupAttributeString(const String& inString);

		void Clear()
		{
			map.Clear();
			array.Clear();
		}

	private:
		HashMap<Name, Attribute> map{};
		Array<Attribute> array{};
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

        virtual const CE::Array<CE::Attribute>& GetAttributes();

		virtual String GetDisplayName();

		//String GetLocalAttributeValue(const String& key) const;
        //bool HasLocalAttribute(const String& key) const;

        virtual String GetAttributeValue(const String& key);
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

		virtual bool IsArrayType() const { return this->GetTypeId() == TYPEID(Array<u8>); }
        virtual bool IsObjectStoreType() const { return this->GetTypeId() == TYPEID(ObjectStore); }
        
        virtual bool IsObject() const { return IsClass() && IsAssignableTo(TYPEID(Object)); }
		virtual bool IsComponent() const;

		Name GenerateInstanceName(UUID uuid) const;

	protected:
		CE::Name name;
		CE::String displayName{};
		//CE::Array<CE::Attribute> attributes{};
		AttributeTable attributeTable{};

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

		static Name currentlyLoadingModule;
		static HashMap<Name, Array<TypeInfo*>> registeredTypesByModuleName;
	};

	/// Default implementation always returns nullptr. Specialization will return the correct data.
	/// Returns the type info of the specified type at compile time.
	template<typename Type>
	TypeInfo* GetStaticType()
	{
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

CE_RTTI_POD2(bool, TYPEID(CE::s8), TYPEID(CE::u8))

CE_RTTI_POD(CE, s8,  TYPEID(u8))
CE_RTTI_POD(CE, s16, TYPEID(u16))
CE_RTTI_POD(CE, s32, TYPEID(u32))
CE_RTTI_POD(CE, s64, TYPEID(u64))

CE_RTTI_POD(CE, u8,  TYPEID(s8), TYPEID(s16), TYPEID(s32), TYPEID(s64), TYPEID(u16), TYPEID(u32), TYPEID(u64))
CE_RTTI_POD(CE, u16, TYPEID(s8), TYPEID(s16), TYPEID(s32), TYPEID(s64), TYPEID(u8),  TYPEID(u32), TYPEID(u64))
CE_RTTI_POD(CE, u32, TYPEID(s8), TYPEID(s16), TYPEID(s32), TYPEID(s64), TYPEID(u8),  TYPEID(u16), TYPEID(u64))
CE_RTTI_POD(CE, u64, TYPEID(s8), TYPEID(s16), TYPEID(s32), TYPEID(s64), TYPEID(u8),  TYPEID(u16), TYPEID(u32))
CE_RTTI_POD(CE, UUID, TYPEID(u64), TYPEID(s64))

CE_RTTI_POD(CE, f32, TYPEID(s8), TYPEID(s16), TYPEID(s32), TYPEID(s64), TYPEID(u8),  TYPEID(u16), TYPEID(u32), TYPEID(u64), TYPEID(f64))
CE_RTTI_POD(CE, f64, TYPEID(s8), TYPEID(s16), TYPEID(s32), TYPEID(s64), TYPEID(u8),  TYPEID(u16), TYPEID(u32), TYPEID(u64), TYPEID(f32))

CE_RTTI_POD(CE, String, TYPEID(const char*), TYPEID(char*))
CE_RTTI_POD(CE, Name)
CE_RTTI_POD(CE::IO, Path)

CE_RTTI_POD3(CE, Array, Array<u8>)
