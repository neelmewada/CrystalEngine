
#include "CoreShader.h"

#if PLATFORM_DESKTOP
#include "spirv_cross/spirv_reflect.hpp"
#include "spirv_cross/spirv_parser.hpp"
#include <spirv-tools/libspirv.hpp>
#endif

namespace CE
{   
    
    ShaderReflector::ShaderReflector()
    {
		
    }

    ShaderReflector::~ShaderReflector()
    {

    }


	ShaderReflector::ErrorCode ShaderReflector::ReflectSpirv(const void* byteCode, u32 byteSize, ShaderStage curStage, ShaderReflection& outReflection)
	{
#if PLATFORM_DESKTOP
		spirv_cross::CompilerReflection* reflection = new spirv_cross::CompilerReflection((const uint32_t*)byteCode, byteSize / 4);
		defer(
			delete reflection;
		);

		auto resources = reflection->get_shader_resources();
		
		if (curStage == ShaderStage::Vertex)
		{
			// Fetch vertex stage inputs
			for (const spirv_cross::Resource& input : resources.stage_inputs)
			{
				
			}
		}

		auto reflectStruct = [&](SRGVariable& variable, spirv_cross::SPIRType type, spirv_cross::TypeID baseTypeId)
			{
				int numMembers = type.member_types.size();
				for (int i = 0; i < numMembers; i++)
				{
					auto memberTypeId = type.member_types[i];
					String name = reflection->get_member_name(baseTypeId, i);
					auto memberType = reflection->get_type(memberTypeId);

					ShaderStructMember structMember{};
					structMember.name = name;

					if (memberType.basetype != spirv_cross::SPIRType::Float)
						continue;
					if (memberType.columns == 1)
					{
						if (memberType.vecsize == 1)
							structMember.dataType = ShaderStructMemberType::Float;
						else if (memberType.vecsize == 2)
							structMember.dataType = ShaderStructMemberType::Float2;
						else if (memberType.vecsize == 3)
							structMember.dataType = ShaderStructMemberType::Float3;
						else if (memberType.vecsize == 4)
							structMember.dataType = ShaderStructMemberType::Float4;
						else
							continue;
					}
					else if (memberType.columns == 4)
					{
						structMember.dataType = ShaderStructMemberType::Float4x4;
					}
					else
					{
						continue;
					}

					variable.structMembers.Add(structMember);
				}
			};
		
		int numUniformBuffers = resources.uniform_buffers.size();
		for (int i = 0; i < numUniformBuffers; i++) // ConstantBuffer (uniform buffer)
		{
			auto uniformBuffer = resources.uniform_buffers[i];
			
			String name = reflection->get_name(uniformBuffer.id);
			auto id = uniformBuffer.id;
			String internalName = reflection->get_name(uniformBuffer.base_type_id);
			auto type = reflection->get_type(uniformBuffer.type_id);
			int count = 1;
			if (type.array.size() > 0)
				count = type.array[0];

			u32 set = reflection->get_decoration(id, spv::DecorationDescriptorSet);
			u32 binding = reflection->get_decoration(id, spv::DecorationBinding);

			SRGVariable variable{};
			variable.bindingSlot = binding;
			variable.name = name;
			variable.internalName = internalName;
			variable.resourceType = ShaderResourceType::ConstantBuffer;
			variable.count = count;
			variable.shaderStages = curStage;

			if (type.basetype == spirv_cross::SPIRType::Struct)
			{
				reflectStruct(variable, type, uniformBuffer.base_type_id);
			}

			//auto& entry = outReflection.FindOrAdd(set);
			//entry.TryAdd(variable, curStage);
		}

		int numStorageBuffers = resources.storage_buffers.size();
		for (int i = 0; i < numStorageBuffers; i++) // StructuredBuffer (Storage Buffers)
		{
			auto storageBuffer = resources.storage_buffers[i];

			String name = reflection->get_name(storageBuffer.id);
			auto id = storageBuffer.id;
			String internalName = reflection->get_name(storageBuffer.base_type_id);

			auto type = reflection->get_type(storageBuffer.type_id);
			int count = 1;
			if (type.array.size() > 0)
				count = type.array[0];

			u32 set = reflection->get_decoration(id, spv::DecorationDescriptorSet);
			u32 binding = reflection->get_decoration(id, spv::DecorationBinding);

			SRGVariable variable{};
			variable.bindingSlot = binding;
			variable.name = name;
			variable.internalName = internalName;
			variable.resourceType = ShaderResourceType::StructuredBuffer;
			variable.count = count;
			variable.shaderStages = curStage;
			
			if (type.basetype == spirv_cross::SPIRType::Struct)
			{
				reflectStruct(variable, type, storageBuffer.base_type_id);
			}

			//auto& entry = outReflection.FindOrAdd(set);
			//entry.TryAdd(variable, curStage);
		}

		int numStorageImages = resources.storage_images.size();
		for (int i = 0; i < numStorageImages; i++) // RWTexture2D, etc
		{
			auto storageImage = resources.storage_images[i];

			String name = reflection->get_name(storageImage.id);
			auto id = storageImage.id;
			String internalName = reflection->get_name(storageImage.id);

			auto type = reflection->get_type(storageImage.type_id);
			int count = 1;
			if (type.array.size() > 0)
				count = type.array[0];

			u32 set = reflection->get_decoration(id, spv::DecorationDescriptorSet);
			u32 binding = reflection->get_decoration(id, spv::DecorationBinding);
			
			SRGVariable variable{};
			variable.bindingSlot = binding;
			variable.name = name;
			variable.internalName = internalName;
			if (type.image.dim == spv::Dim2D)
			{
				variable.resourceType = ShaderResourceType::RWTexture2D;
			}
			else
			{
				continue; // Invalid type
			}
			variable.resourceType = ShaderResourceType::RWTexture2D;
			variable.count = count;
			variable.shaderStages = curStage;

			//auto& entry = outReflection.FindOrAdd(set);
			//entry.TryAdd(variable, curStage);
		}
		
		int numTextures = resources.separate_images.size();
		for (int i = 0; i < numTextures; i++) // Texure1D/2D/3D/Cube (separate texture)
		{
			auto texture = resources.separate_images[i];
			
			String name = reflection->get_name(texture.id);
			auto id = texture.id;
			String internalName = reflection->get_name(texture.base_type_id);

			auto type = reflection->get_type(texture.type_id);
			int count = 1;
			if (type.array.size() > 0)
				count = type.array[0];

			u32 set = reflection->get_decoration(id, spv::DecorationDescriptorSet);
			u32 binding = reflection->get_decoration(id, spv::DecorationBinding);

			RHI::SRGVariableDescriptor variable{};
			variable.bindingSlot = binding;
			variable.name = name;
			variable.shaderStages = curStage;
			
			if (type.image.dim == spv::Dim2D)
			{
				variable.type = ShaderResourceType::Texture2D;
			}
			else if (type.image.dim == spv::Dim1D)
			{
				variable.type = ShaderResourceType::Texture1D;
			}
			else if (type.image.dim == spv::Dim3D)
			{
				variable.type = ShaderResourceType::Texture3D;
			}
			else if (type.image.dim == spv::DimCube)
			{
				variable.type = ShaderResourceType::TextureCube;
			}
			else
			{
				continue; // Invalid type
			}
			variable.arrayCount = count;

			auto& entry = outReflection.FindOrAdd((RHI::SRGType)set);
			entry.TryAdd(variable);
		}

		int numSamplers = resources.separate_samplers.size();
		for (int i = 0; i < numSamplers; i++) // SamplerState (separate sampler)
		{
			auto sampler = resources.separate_samplers[i];

			String name = reflection->get_name(sampler.id);
			auto id = sampler.id;
			String internalName = reflection->get_name(sampler.base_type_id);

			auto type = reflection->get_type(sampler.type_id);
			int count = 1;
			if (type.array.size() > 0)
				count = type.array[0];

			u32 set = reflection->get_decoration(id, spv::DecorationDescriptorSet);
			u32 binding = reflection->get_decoration(id, spv::DecorationBinding);

			SRGVariable variable{};
			variable.bindingSlot = binding;
			variable.name = name;
			variable.internalName = internalName;
			variable.resourceType = ShaderResourceType::SamplerState;
			variable.count = count;
			variable.shaderStages = curStage;

			//auto& entry = outReflection.FindOrAdd(set);
			//entry.TryAdd(variable, curStage);
		}
		return ERR_Success;
#else
		return ERR_UnsupportedPlatform;
#endif
	}

} // namespace CE

