#pragma once

#include <variant>

namespace CE::RPI
{
    enum class MaterialPropertyDataType
    {
        None = 0,
        Int, UInt,
        Float,
        Vector2, Vector3, Vector4,
        Color,
        Texture,
        Sampler,
        Matrix4x4,
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
        
        MaterialPropertyValue(RHI::Sampler* samplerValue) : valueType(MaterialPropertyDataType::Sampler)
        {
            u.samplerValue = samplerValue;
        }
        
        MaterialPropertyValue(const Matrix4x4& matrixValue) : valueType(MaterialPropertyDataType::Matrix4x4)
        {
            u.matrixValue = matrixValue;
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

        template<typename T> requires TContainsType<T, s32, u32, f32, Color, Vec2, Vec3, Vec4, RHI::Texture*, RHI::Sampler*, Matrix4x4>::Value
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
            else if constexpr (TIsSameType<T, RHI::Sampler*>::Value)
            {
                return u.samplerValue;
            }
            else if constexpr (TIsSameType<T, Matrix4x4>::Value)
            {
                return u.matrixValue;
            }
            return {};
        }

        template<typename T> requires TContainsType<T, s32, u32, f32, Color, Vec2, Vec3, Vec4, RHI::Texture*, RHI::Sampler*, Matrix4x4>::Value
        inline void GetValue(T& outValue) const
        {
            outValue = GetValue<T>();
        }

        template<typename T> requires TIsEnum<T>::Value
        inline T GetEnumValue() const
        {
            return (T)u.enumValue;
        }
        
        template<typename T> requires TContainsType<T, s32, u32, f32, Color, Vec2, Vec3, Vec4, RHI::Texture*, RHI::Sampler*, Matrix4x4>::Value
        inline bool IsOfType() const
        {
            if constexpr (TIsSameType<T, s32>::Value)
            {
                return valueType == MaterialPropertyDataType::Int;
            }
            else if constexpr (TIsSameType<T, u32>::Value)
            {
                return valueType == MaterialPropertyDataType::UInt;
            }
            else if constexpr (TIsSameType<T, f32>::Value)
            {
                return valueType == MaterialPropertyDataType::Float;
            }
            else if constexpr (TIsSameType<T, Color>::Value)
            {
                return valueType == MaterialPropertyDataType::Color;
            }
            else if constexpr (TIsSameType<T, Vec2>::Value)
            {
                return valueType == MaterialPropertyDataType::Vector2;
            }
            else if constexpr (TIsSameType<T, Vec3>::Value)
            {
                return valueType == MaterialPropertyDataType::Vector3;
            }
            else if constexpr (TIsSameType<T, Vec4>::Value)
            {
                return valueType == MaterialPropertyDataType::Vector4;
            }
            else if constexpr (TIsSameType<T, RHI::Texture*>::Value)
            {
                return valueType == MaterialPropertyDataType::Texture;
            }
            else if constexpr (TIsSameType<T, RHI::Sampler*>::Value)
            {
                return valueType == MaterialPropertyDataType::Sampler;
            }
            else if constexpr (TIsSameType<T, Matrix4x4>::Value)
            {
                return valueType == MaterialPropertyDataType::Matrix4x4;
            }
            return false;
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
            Vec4 vec4Value;
            Matrix4x4 matrixValue = {};
            RHI::Texture* textureValue;
            RHI::Sampler* samplerValue;
            s64 enumValue;
        } u;

        MaterialPropertyDataType valueType = MaterialPropertyDataType::None;
    };
    
} // namespace CE::RPI
