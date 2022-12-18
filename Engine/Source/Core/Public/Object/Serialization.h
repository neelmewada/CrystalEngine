#pragma once

#include "RTTI.h"
#include "IO/Stream/MemoryStream.h"

#define YAML_CPP_API
#include "yaml-cpp/yaml.h"


namespace CE
{

    class CORE_API SerializedObject
    {
        CE_CLASS(SerializedObject)
    private:
        SerializedObject(const TypeInfo* type, void* instance, YAML::Emitter* emitter);

    public:
        SerializedObject(const TypeInfo* type);
        virtual ~SerializedObject();

        void Serialize(void* instance, IO::MemoryStream* outStream);
        void Deserialize(void* instance, IO::GenericStream* inStream);

        virtual bool IsContext() const { return false; }

    protected:
        void SerializeProperty(void* instance, FieldType* fieldType, YAML::Emitter& out);

        const TypeInfo* type = nullptr;
        void* instance = nullptr;

        SerializedObject* context = nullptr;

        YAML::Emitter* emitter = nullptr;
    };

} // namespace CE

CE_RTTI_CLASS(CORE_API, CE, SerializedObject,
    CE_SUPER(),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(context)
    ),
    CE_FUNCTION_LIST()
)

