#pragma once

#include <variant>

namespace CE::RPI
{
    enum class MaterialPropertyDataType
    {
        None = 0,
        Int, UInt,
        Float,
        Color,
        Vector2, Vector3, Vector4,
        Texture,
        Enum,
        COUNT
    };
    ENUM_CLASS(MaterialPropertyDataType);

    struct CORERPI_API MaterialPropertyValue final
    {
    public:

        MaterialPropertyValue() {}

        MaterialPropertyValue(s32 intValue) : valueType(MaterialPropertyDataType::Int)
        {
            u.intValue = intValue;
        }

        MaterialPropertyValue(u32 uintValue) : valueType(MaterialPropertyDataType::UInt)
        {
            u.uintValue = uintValue;
        }

        MaterialPropertyValue(f32 floatValue) : valueType(MaterialPropertyDataType::Float)
        {
            u.floatValue = floatValue;
        }

        MaterialPropertyValue(Color colorValue) : valueType(MaterialPropertyDataType::Color)
        {
            u.colorValue = colorValue;
        }

        MaterialPropertyValue(Vec2 vec2Value) : valueType(MaterialPropertyDataType::Vector2)
        {
            u.vec2Value = vec2Value;
        }

        MaterialPropertyValue(Vec3 vec3Value) : valueType(MaterialPropertyDataType::Vector3)
        {
            u.vec3Value = vec3Value;
        }

        MaterialPropertyValue(Vec4 vec4Value) : valueType(MaterialPropertyDataType::Vector4)
        {
            u.vec4Value = vec4Value;
        }

        MaterialPropertyValue(RHI::Texture* textureValue) : valueType(MaterialPropertyDataType::Texture)
        {
            u.textureValue = textureValue;
        }

        template<typename TEnum> requires TIsEnum<TEnum>::Value
        MaterialPropertyValue(TEnum enumValue) : valueType(MaterialPropertyDataType::Enum)
        {
            u.enumValue = (s64)enumValue;
        }

        MaterialPropertyValue(const MaterialPropertyValue& copy);

        MaterialPropertyValue& operator=(const MaterialPropertyValue& copy);

        MaterialPropertyValue(MaterialPropertyValue&& move) noexcept;

        ~MaterialPropertyValue();

        inline MaterialPropertyDataType GetValueType() const { return valueType; }

        template<typename T> requires TContainsType<T, s32, u32, f32, Color, Vec2, Vec3, Vec4, RHI::Texture*>::Value
        inline T GetValue() const
        {
            if constexpr (TIsSameType<T, s32>::Value)
            {
                return u.intValue;
            }
            else if constexpr (TIsSameType<T, u32>::Value)
            {
                return u.uintValue;
            }
            else if constexpr (TIsSameType<T, f32>::Value)
            {
                return u.floatValue;
            }
            else if constexpr (TIsSameType<T, Color>::Value)
            {
                return u.colorValue;
            }
            else if constexpr (TIsSameType<T, Vec2>::Value)
            {
                return u.vec2Value;
            }
            else if constexpr (TIsSameType<T, Vec3>::Value)
            {
                return u.vec3Value;
            }
            else if constexpr (TIsSameType<T, Vec4>::Value)
            {
                return u.vec4Value;
            }
            else if constexpr (TIsSameType<T, RHI::Texture*>::Value)
            {
                return u.textureValue;
            }
            return {};
        }

        template<typename T> requires TContainsType<T, s32, u32, f32, Color, Vec2, Vec3, Vec4, RHI::Texture*>::Value
        inline void GetValue(T& outValue) const
        {
            outValue = GetValue<T>();
        }

        template<typename T> requires TIsEnum<T>::Value
        inline T GetEnumValue() const
        {
            return (T)u.enumValue;
        }
        
    private:

        union U {
            U() {
                vec4Value = Vec4();
            }
            
            s32 intValue;
            u32 uintValue;
            f32 floatValue;
            Color colorValue;
            Vec2 vec2Value;
            Vec3 vec3Value;
            Vec4 vec4Value = Vec4();
            RHI::Texture* textureValue;
            s64 enumValue;
        } u;

        MaterialPropertyDataType valueType = MaterialPropertyDataType::None;
    };
    
} // namespace CE::RPI
