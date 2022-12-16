#pragma once

#include "RTTI.h"
#include "IO/Stream/MemoryStream.h"

#define YAML_CPP_API
#include "yaml-cpp/yaml.h"


namespace CE
{

    class CORE_API SerializedObject
    {
    public:
        SerializedObject(const TypeInfo* type);
        virtual ~SerializedObject();

        void Serialize(void* instance, IO::MemoryStream* outStream);
        void Deserialize(void* instance, IO::GenericStream* inStream);

    protected:
        void SerializeProperty(void* instance, FieldType* fieldType, YAML::Emitter& out);

        const TypeInfo* type = nullptr;
        void* instance = nullptr;

    };

    
    
} // namespace CE
