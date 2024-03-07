#pragma once

#include <variant>

namespace CE::RPI
{
    class Texture;

    enum class MaterialPropertyDataType
    {
        None = 0,
        Int, UInt,
        Float,
        Vector2, Vector3, Vector4,
        Color,
        Texture,
        TextureView,
        Sampler,
        Matrix4x4,
        Enum,
        COUNT
    };
    ENUM_CLASS(MaterialPropertyDataType);

    using MaterialSlotId = u32;

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

        MaterialPropertyValue(RPI::Texture* textureValue) : valueType(MaterialPropertyDataType::Texture)
        {
            u.textureValue = textureValue;
        }

        MaterialPropertyValue(RHI::TextureView* textureViewValue) : valueType(MaterialPropertyDataType::TextureView)
        {
            u.textureViewValue = textureViewValue;
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

        inline bool operator==(const MaterialPropertyValue& other) const
        {
            if (valueType != other.valueType)
                return false;

            switch (valueType)
            {
            case MaterialPropertyDataType::Float:
                return GetValue<float>() == other.GetValue<float>();
            case MaterialPropertyDataType::UInt:
                return GetValue<u32>() == other.GetValue<u32>();
            case MaterialPropertyDataType::Int:
                return GetValue<s32>() == other.GetValue<s32>();
            case MaterialPropertyDataType::Color:
                return u.colorValue.r == other.u.colorValue.r && u.colorValue.g == other.u.colorValue.g && 
                    u.colorValue.b == other.u.colorValue.b && u.colorValue.a == other.u.colorValue.a;
            case MaterialPropertyDataType::Vector2:
                return u.vec2Value.x == other.u.vec2Value.x && u.vec2Value.y == other.u.vec2Value.y;
            case MaterialPropertyDataType::Vector3:
                return u.vec3Value.x == other.u.vec3Value.x && u.vec3Value.y == u.vec3Value.y && u.vec3Value.z == u.vec3Value.z;
            case MaterialPropertyDataType::Vector4:
                return u.vec4Value.x == other.u.vec4Value.x && u.vec4Value.y == other.u.vec4Value.y &&
                    u.vec4Value.z == other.u.vec4Value.z && u.vec4Value.w == other.u.vec4Value.w;
            case MaterialPropertyDataType::Texture:
                return u.textureValue == other.u.textureValue;
            case MaterialPropertyDataType::TextureView:
                return u.textureViewValue == other.u.textureViewValue;
            case MaterialPropertyDataType::Sampler:
                return u.samplerValue == other.u.samplerValue;
            case MaterialPropertyDataType::Matrix4x4:
                return u.matrixValue == other.u.matrixValue;
            }

            return false;
        }

        inline bool operator!=(const MaterialPropertyValue& other) const
        {
            return !(*this == other);
        }

        inline MaterialPropertyDataType GetValueType() const { return valueType; }

        template<typename T> requires TContainsType<T, s32, u32, f32, Color, Vec2, Vec3, Vec4, RPI::Texture*, RHI::TextureView*, RHI::Sampler*, Matrix4x4>::Value or TIsEnum<T>::Value
        inline T GetValue() const
        {
            if constexpr (TIsEnum<T>::Value)
            {
                return (T)u.enumValue;
            }
            else if constexpr (TIsSameType<T, s32>::Value)
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
            else if constexpr (TIsSameType<T, RPI::Texture*>::Value)
            {
                return u.textureValue;
            }
            else if constexpr (TIsSameType<T, RHI::TextureView*>::Value)
            {
                return u.textureViewValue;
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

        template<typename T> requires TContainsType<T, s32, u32, f32, Color, Vec2, Vec3, Vec4, RPI::Texture*, RHI::TextureView*, RHI::Sampler*, Matrix4x4>::Value or TIsEnum<T>::Value
        inline void GetValue(T& outValue) const
        {
            outValue = GetValue<T>();
        }
        
        template<typename T> requires TContainsType<T, s32, u32, f32, Color, Vec2, Vec3, Vec4, RPI::Texture*, RHI::TextureView*, RHI::Sampler*, Matrix4x4>::Value or TIsEnum<T>::Value
        inline bool IsOfType() const
        {
            if constexpr (TIsEnum<T>::Value)
            {
                return valueType == MaterialPropertyDataType::Enum;
            }
            else if constexpr (TIsSameType<T, s32>::Value)
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
            else if constexpr (TIsSameType<T, RPI::Texture*>::Value)
            {
                return valueType == MaterialPropertyDataType::Texture;
            }
            else if constexpr (TIsSameType<T, RHI::TextureView*>::Value)
            {
                return valueType == MaterialPropertyDataType::TextureView;
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
                memset(this, 0, sizeof(*this));
            }
            
            s32 intValue;
            u32 uintValue;
            f32 floatValue;
            Color colorValue;
            Vec2 vec2Value;
            Vec3 vec3Value;
            Vec4 vec4Value;
            Matrix4x4 matrixValue = {};
            RPI::Texture* textureValue;
            RHI::TextureView* textureViewValue;
            RHI::Sampler* samplerValue;
            s64 enumValue;
        } u;

        MaterialPropertyDataType valueType = MaterialPropertyDataType::None;
    };
    
} // namespace CE::RPI
