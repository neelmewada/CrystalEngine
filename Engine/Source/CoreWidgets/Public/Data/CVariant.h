#pragma once

namespace CE::Widgets
{

	class COREWIDGETS_API CVariant
	{
	public:

		CVariant();
		~CVariant();

		template<typename T>
		CVariant(const T& value)
		{
			Clear();
			SetInternalValue(value);
		}

		CVariant(const CVariant& copy)
		{
			this->Copy(copy);
		}

		CVariant(CVariant&& move)
		{
			this->Copy(move);
			move.Clear();
		}

		CVariant& operator=(const CVariant& copy)
		{
			this->Copy(copy);
			return *this;
		}

		template<typename T>
		FORCE_INLINE bool IsOfType()
		{
			return TYPEID(T) == valueTypeId;
		}

	private:

		template<typename T>
		void SetInternalValue(const T& value)
		{
			valueTypeId = TYPEID(T);
			*(T*)this = value;
		}

		void Copy(const CVariant& copy);

		void Clear();

		union
		{
			u8 u8Value;
			u16 u16Value;
			u32 u32Value;
			u64 u64Value;
			s8 s8Value;
			s16 s16Value;
			s32 s32Value;
			s64 s64Value;

			f32 f32Value;
			f64 f64Value;

			String stringValue;
			Name nameValue;

			Vec2 vec2Value;
			Vec3 vec3Value;
			Vec4 vec4Value;

			Color colorValue;
		};

		TypeId valueTypeId = 0;
	};
    
} // namespace CE::Widgets

CE_RTTI_POD(COREWIDGETS_API, CE::Widgets, CVariant)
