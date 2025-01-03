#include "CoreRPI.h"

namespace CE::RPI
{
    MaterialPropertyValue::MaterialPropertyValue(const MaterialPropertyValue& copy)
    {
        valueType = copy.valueType;

        switch (valueType)
        {
        case MaterialPropertyDataType::Int:
            copy.GetValue(u.intValue);
            break;
        case MaterialPropertyDataType::UInt:
            copy.GetValue(u.uintValue);
            break;
        case MaterialPropertyDataType::Float:
            copy.GetValue(u.floatValue);
            break;
        case MaterialPropertyDataType::Color:
            copy.GetValue(u.colorValue);
            break;
        case MaterialPropertyDataType::Vector2:
            copy.GetValue(u.vec2Value);
            break;
        case MaterialPropertyDataType::Vector3:
            copy.GetValue(u.vec3Value);
            break;
        case MaterialPropertyDataType::Vector4:
            copy.GetValue(u.vec4Value);
            break;
        case MaterialPropertyDataType::Texture:
            copy.GetValue(u.textureValue);
            break;
        case MaterialPropertyDataType::TextureView:
            copy.GetValue(u.textureViewValue);
            break;
        case MaterialPropertyDataType::Sampler:
            copy.GetValue(u.samplerValue);
            break;
        case MaterialPropertyDataType::Enum:
            u.enumValue = copy.u.enumValue;
            break;
        case MaterialPropertyDataType::Matrix4x4:
            u.matrixValue = copy.u.matrixValue;
            break;
        case MaterialPropertyDataType::Buffer:
            u.bufferValue = copy.u.bufferValue;
            break;
        default:
            throw std::runtime_error("Invalid or unimplemented MaterialPropertyDataType!");
        }
    }

    MaterialPropertyValue& MaterialPropertyValue::operator=(const MaterialPropertyValue& copy)
    {
        valueType = copy.valueType;

        switch (valueType)
        {
        case MaterialPropertyDataType::Int:
            copy.GetValue(u.intValue);
            break;
        case MaterialPropertyDataType::UInt:
            copy.GetValue(u.uintValue);
            break;
        case MaterialPropertyDataType::Float:
            copy.GetValue(u.floatValue);
            break;
        case MaterialPropertyDataType::Color:
            copy.GetValue(u.colorValue);
            break;
        case MaterialPropertyDataType::Vector2:
            copy.GetValue(u.vec2Value);
            break;
        case MaterialPropertyDataType::Vector3:
            copy.GetValue(u.vec3Value);
            break;
        case MaterialPropertyDataType::Vector4:
            copy.GetValue(u.vec4Value);
            break;
        case MaterialPropertyDataType::Texture:
            copy.GetValue(u.textureValue);
            break;
        case MaterialPropertyDataType::TextureView:
            copy.GetValue(u.textureViewValue);
            break;
        case MaterialPropertyDataType::Sampler:
            copy.GetValue(u.samplerValue);
            break;
        case MaterialPropertyDataType::Enum:
            u.enumValue = copy.u.enumValue;
            break;
        case MaterialPropertyDataType::Matrix4x4:
            u.matrixValue = copy.u.matrixValue;
            break;
        case MaterialPropertyDataType::Buffer:
            u.bufferValue = copy.u.bufferValue;
            break;
        default:
            throw std::runtime_error("Invalid or unimplemented MaterialPropertyDataType!");
        }

        return *this;
    }

    MaterialPropertyValue::MaterialPropertyValue(MaterialPropertyValue&& move) noexcept
    {
        valueType = move.valueType;

        switch (valueType)
        {
        case MaterialPropertyDataType::Int:
            move.GetValue(u.intValue);
            break;
        case MaterialPropertyDataType::UInt:
            move.GetValue(u.uintValue);
            break;
        case MaterialPropertyDataType::Float:
            move.GetValue(u.floatValue);
            break;
        case MaterialPropertyDataType::Color:
            move.GetValue(u.colorValue);
            break;
        case MaterialPropertyDataType::Vector2:
            move.GetValue(u.vec2Value);
            break;
        case MaterialPropertyDataType::Vector3:
            move.GetValue(u.vec3Value);
            break;
        case MaterialPropertyDataType::Vector4:
            move.GetValue(u.vec4Value);
            break;
        case MaterialPropertyDataType::Texture:
            move.GetValue(u.textureValue);
            break;
        case MaterialPropertyDataType::TextureView:
            move.GetValue(u.textureViewValue);
            break;
        case MaterialPropertyDataType::Sampler:
            move.GetValue(u.samplerValue);
            break;
        case MaterialPropertyDataType::Enum:
            u.enumValue = move.u.enumValue;
            break;
        case MaterialPropertyDataType::Matrix4x4:
            u.matrixValue = move.u.matrixValue;
            break;
        case MaterialPropertyDataType::Buffer:
            u.bufferValue = move.u.bufferValue;
            break;
        default:
            throw std::runtime_error("Invalid or unimplemented MaterialPropertyDataType!");
        }

        move.valueType = MaterialPropertyDataType::None;
        memset(&move.u, 0, sizeof(move.u));
    }

    MaterialPropertyValue::~MaterialPropertyValue()
    {
        valueType = MaterialPropertyDataType::None;
        memset(&u, 0, sizeof(u));
    }

    void MaterialPropertyValueArray::Add(const MaterialPropertyValue& value)
    {
        values.Add(value);
    }

    void MaterialPropertyValueArray::Insert(int index, const MaterialPropertyValue& value)
    {
        values.InsertAt(index, value);
    }

    void MaterialPropertyValueArray::RemoveAt(int index)
    {
        values.RemoveAt(index);
    }

} // namespace CE::RPI
