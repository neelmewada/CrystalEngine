
#include "FusionCore.h"

#include "FusionCore.private.h"
#include "Resource.h"

namespace CE
{
	RawData GetFusionShaderVert()
    {
        return RawData{ (u8*)Shaders_FusionShader_vert_spv_Data, Shaders_FusionShader_vert_spv_Length };
    }

    RawData GetFusionShaderFrag()
    {
        return RawData{ (u8*)Shaders_FusionShader_frag_spv_Data, Shaders_FusionShader_frag_spv_Length };
    }

    RawData GetFusionShaderVertJson()
    {
        return RawData{ (u8*)Shaders_FusionShader_vert_json_Data, Shaders_FusionShader_vert_json_Length };
    }

    RawData GetFusionShaderFragJson()
    {
        return RawData{ (u8*)Shaders_FusionShader_frag_json_Data, Shaders_FusionShader_frag_json_Length };
    }

    RawData GetRobotoFont()
	{
        return RawData{ (u8*)Fonts_Roboto_ttf_Data, Fonts_Roboto_ttf_Length };
	}

    class FusionCoreModule : public CE::Module
    {
    public:
        virtual void StartupModule() override
        {

        }

        virtual void ShutdownModule() override
        {

        }

        virtual void RegisterTypes() override
        {

        }
    };
}

CE_IMPLEMENT_MODULE(FusionCore, CE::FusionCoreModule)
