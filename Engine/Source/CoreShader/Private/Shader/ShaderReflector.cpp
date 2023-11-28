
#include "CoreShader.h"

#if PLATFORM_DESKTOP
#include "spirv_cross/spirv_reflect.hpp"
#endif

namespace CE
{   
    
    ShaderReflector::ShaderReflector()
    {
		
    }

    ShaderReflector::~ShaderReflector()
    {

    }


	ShaderReflector::ErrorCode ShaderReflector::ReflectSpirv(const void* byteCode, u32 byteSize, ShaderReflection& outReflection)
	{
		spirv_cross::CompilerReflection* reflection = new spirv_cross::CompilerReflection((const uint32_t*)byteCode, byteSize / 4);
		defer(
			delete reflection;
		);

		outReflection = ShaderReflection();
		
		auto resources = reflection->get_shader_resources();

		auto reflectResource = [&](spirv_cross::SmallVector<spirv_cross::Resource>& resourceList, ShaderResourceType resourceType)
			{
				int numResources = resourceList.size();
				for (int i = 0; i < numResources; i++)
				{
					auto resource = resourceList[i];
					auto id = resource.id;

					String internalName = reflection->get_name(resource.base_type_id);
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
			variable.binding = binding;
			variable.name = name;
			variable.internalName = internalName;
			variable.resourceType = ShaderResourceType::ConstantBuffer;
			variable.count = count;

			auto& entry = outReflection.FindOrAdd(set);
			entry.variables.Add(variable);
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
			variable.binding = binding;
			variable.name = name;
			variable.internalName = internalName;
			variable.resourceType = ShaderResourceType::StructuredBuffer;
			variable.count = count;
			
			auto& entry = outReflection.FindOrAdd(set);
			entry.variables.Add(variable);
		}

		int numStorageImages = resources.storage_images.size();
		for (int i = 0; i < numStorageImages; i++)
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
			variable.binding = binding;
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

			auto& entry = outReflection.FindOrAdd(set);
			entry.variables.Add(variable);
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

			SRGVariable variable{};
			variable.binding = binding;
			variable.name = name;
			variable.internalName = internalName;
			
			if (type.image.dim == spv::Dim2D)
			{
				variable.resourceType = ShaderResourceType::Texture2D;
			}
			else if (type.image.dim == spv::Dim1D)
			{
				variable.resourceType = ShaderResourceType::Texture1D;
			}
			else if (type.image.dim == spv::Dim3D)
			{
				variable.resourceType = ShaderResourceType::Texture3D;
			}
			else if (type.image.dim == spv::DimCube)
			{
				variable.resourceType = ShaderResourceType::TextureCube;
			}
			else
			{
				continue; // Invalid type
			}
			variable.count = count;

			auto& entry = outReflection.FindOrAdd(set);
			entry.variables.Add(variable);
		}

		int numSamplers = resources.separate_samplers.size();
		for (int i = 0; i < numSamplers; i++) // SampelerState (separate sampler)
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
			variable.binding = binding;
			variable.name = name;
			variable.internalName = internalName;
			variable.resourceType = ShaderResourceType::SamplerState;
			variable.count = count;

			auto& entry = outReflection.FindOrAdd(set);
			entry.variables.Add(variable);
		}

		return ERR_Success;
	}

} // namespace CE

