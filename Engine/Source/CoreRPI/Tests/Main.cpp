
// Allow access to internal members for tests
#define corerpi_internal public
#define corerpi_private_internal public
#define corerpi_protected_internal public

#include "Core.h"

#include "CoreRPI.h"
#include <gtest/gtest.h>

#include "JsonData.h"

using namespace CE;

#define TEST_BEGIN TestBegin()
#define TEST_END TestEnd()

static void TestBegin()
{
	ModuleManager::Get().LoadModule("Core");
	ModuleManager::Get().LoadModule("CoreMedia");
	ModuleManager::Get().LoadModule("CoreShader");
	ModuleManager::Get().LoadModule("CoreRHI");
	ModuleManager::Get().LoadModule("CoreRPI");
}

static void TestEnd()
{
	ModuleManager::Get().UnloadModule("CoreRPI");
	ModuleManager::Get().UnloadModule("CoreRHI");
	ModuleManager::Get().UnloadModule("CoreShader");
	ModuleManager::Get().UnloadModule("CoreMedia");
	ModuleManager::Get().UnloadModule("Core");
}

TEST(RenderPipeline, DescriptorParsing)
{
	TEST_BEGIN;

	RenderPipelineDescriptor desc{};

	JsonFieldDeserializer deserializer{ desc.GetStruct(), &desc };

	JValue root{};
	JsonSerializer::Deserialize2(DescriptorParsing_DefaultPipeline_JSON, root);

	MemoryStream stream = MemoryStream((void*)DescriptorParsing_DefaultPipeline_JSON, COUNTOF(DescriptorParsing_DefaultPipeline_JSON));
	
	deserializer.Deserialize(&stream);

	EXPECT_EQ(desc.name, "DefaultPipeline");
	EXPECT_EQ(desc.mainViewTag, "Camera");

	const RPI::PassRequest& rootPass = desc.rootPass;
	{
		EXPECT_EQ(rootPass.passName, "DefaultPipelineRoot");
		EXPECT_EQ(rootPass.passDefinition, "DefaultPipelineRoot");
		EXPECT_EQ(rootPass.childPasses.GetSize(), 3);

		{
			const RPI::PassRequest& childPass = rootPass.childPasses[0];
			EXPECT_EQ(childPass.passName, "DepthPrePass");
			EXPECT_EQ(childPass.passDefinition, "DepthPrePassDefinition");
			EXPECT_EQ(childPass.connections.GetSize(), 0);
			EXPECT_EQ(childPass.childPasses.GetSize(), 0);
		}
		{
			const RPI::PassRequest& childPass = rootPass.childPasses[1];
			EXPECT_EQ(childPass.passName, "OpaquePass");
			EXPECT_EQ(childPass.passDefinition, "OpaquePassDefinition");
			EXPECT_EQ(childPass.connections.GetSize(), 0);
			EXPECT_EQ(childPass.childPasses.GetSize(), 0);
		}
		{
			const RPI::PassRequest& childPass = rootPass.childPasses[2];
			EXPECT_EQ(childPass.passName, "TransparentPass");
			EXPECT_EQ(childPass.passDefinition, "TransparentPassDefinition");
			EXPECT_EQ(childPass.connections.GetSize(), 2);
			{
				const RPI::PassConnection& connection0 = childPass.connections[0];
				EXPECT_EQ(connection0.localSlot, "Color");
				EXPECT_EQ(connection0.attachmentRef.pass, "OpaquePass");
				EXPECT_EQ(connection0.attachmentRef.attachment, "Color");

				const RPI::PassConnection& connection1 = childPass.connections[1];
				EXPECT_EQ(connection1.localSlot, "Color");
				EXPECT_EQ(connection1.attachmentRef.pass, "$root");
				EXPECT_EQ(connection1.attachmentRef.attachment, "PipelineOutput");
			}
			EXPECT_EQ(childPass.childPasses.GetSize(), 0);
		}
	}

	EXPECT_EQ(desc.passDefinitions.GetSize(), 4);

	// Root Pass
	{
		const RPI::PassDefinition& passDesc = desc.passDefinitions[0];

		EXPECT_EQ(passDesc.name, "DefaultPipelineRoot");
		EXPECT_EQ(passDesc.passClass, "ParentPass");
		EXPECT_EQ(passDesc.slots.GetSize(), 1);
		EXPECT_EQ(passDesc.slots[0].name, "PipelineOutput");
		EXPECT_EQ(passDesc.slots[0].slotType, RPI::PassSlotType::InputOutput);
	}

	// Depth Pre-Pass Definition
	{
		const RPI::PassDefinition& passDesc = desc.passDefinitions[1];

		EXPECT_EQ(passDesc.name, "DepthPrePassDefinition");
		EXPECT_EQ(passDesc.passClass, "RasterPass");
		EXPECT_EQ(passDesc.slots.GetSize(), 1);
		EXPECT_EQ(passDesc.slots[0].name, "Output");
		EXPECT_EQ(passDesc.slots[0].slotType, RPI::PassSlotType::Output);

		EXPECT_EQ(passDesc.slots[0].loadStoreAction.clearValueDepth, 0);
		EXPECT_EQ(passDesc.slots[0].loadStoreAction.clearValueStencil, 0);
		EXPECT_EQ(passDesc.slots[0].loadStoreAction.loadAction, RHI::AttachmentLoadAction::Clear);
		EXPECT_EQ(passDesc.slots[0].loadStoreAction.storeAction, RHI::AttachmentStoreAction::Store);

		EXPECT_EQ(passDesc.imageAttachments.GetSize(), 1);
		EXPECT_EQ(passDesc.imageAttachments[0].name, "DepthStencil");
		EXPECT_EQ(passDesc.imageAttachments[0].lifetime, RHI::AttachmentLifetimeType::Transient);
		EXPECT_EQ(passDesc.imageAttachments[0].sizeSource.source.pass, "$root");
		EXPECT_EQ(passDesc.imageAttachments[0].sizeSource.source.attachment, "PipelineOutput");
		EXPECT_EQ(passDesc.imageAttachments[0].sizeSource.sizeMultipliers, Vec3(1, 1, 1));
		EXPECT_EQ(passDesc.imageAttachments[0].imageDescriptor.bindFlags, RHI::TextureBindFlags::DepthStencil);
		EXPECT_EQ(passDesc.imageAttachments[0].imageDescriptor.format, RHI::Format::D32_SFLOAT_S8_UINT);
		EXPECT_EQ(passDesc.imageAttachments[0].fallbackFormats[0], RHI::Format::D24_UNORM_S8_UINT);

		EXPECT_EQ(passDesc.connections.GetSize(), 1);
		EXPECT_EQ(passDesc.connections[0].localSlot, "Output");
		EXPECT_EQ(passDesc.connections[0].attachmentRef.pass, "$this");
		EXPECT_EQ(passDesc.connections[0].attachmentRef.attachment, "DepthStencil");

		EXPECT_EQ(passDesc.passData.drawListTag, "depth");
		EXPECT_EQ(passDesc.passData.viewTag, "Camera");
	}

	// OpaquePassDefinition
	{
		const RPI::PassDefinition& passDesc = desc.passDefinitions[2];

		EXPECT_EQ(passDesc.name, "OpaquePassDefinition");
		EXPECT_EQ(passDesc.passClass, "RasterPass");
		EXPECT_EQ(passDesc.slots.GetSize(), 1);
		EXPECT_EQ(passDesc.slots[0].name, "Color");
		EXPECT_EQ(passDesc.slots[0].slotType, RPI::PassSlotType::Output);
		EXPECT_EQ(passDesc.slots[0].loadStoreAction.clearValue, Vec4(0, 0, 0, 0));
		EXPECT_EQ(passDesc.slots[0].loadStoreAction.loadAction, RHI::AttachmentLoadAction::Clear);
		EXPECT_EQ(passDesc.slots[0].loadStoreAction.storeAction, RHI::AttachmentStoreAction::Store);

		EXPECT_EQ(passDesc.connections.GetSize(), 1);
		EXPECT_EQ(passDesc.connections[0].localSlot, "Color");
		EXPECT_EQ(passDesc.connections[0].attachmentRef.pass, "$root");
		EXPECT_EQ(passDesc.connections[0].attachmentRef.attachment, "PipelineOutput");

		EXPECT_EQ(passDesc.passData.drawListTag, "forward");
		EXPECT_EQ(passDesc.passData.viewTag, "Camera");
	}

	// TransparentPassDefinition
	{
		const RPI::PassDefinition& passDesc = desc.passDefinitions[3];

		EXPECT_EQ(passDesc.name, "TransparentPassDefinition");
		EXPECT_EQ(passDesc.passClass, "RasterPass");
		EXPECT_EQ(passDesc.slots.GetSize(), 1);
		EXPECT_EQ(passDesc.slots[0].name, "Color");
		EXPECT_EQ(passDesc.slots[0].slotType, RPI::PassSlotType::InputOutput);
		EXPECT_EQ(passDesc.slots[0].loadStoreAction.loadAction, RHI::AttachmentLoadAction::Load);
		EXPECT_EQ(passDesc.slots[0].loadStoreAction.storeAction, RHI::AttachmentStoreAction::Store);

		EXPECT_EQ(passDesc.passData.drawListTag, "forward");
		EXPECT_EQ(passDesc.passData.viewTag, "Camera");
	}

	TEST_END;
}


TEST(RenderPipeline, DefaultPipelineTree)
{
	TEST_BEGIN;

	Resource* pipelineJson = GetResourceManager()->LoadResource("/CoreRPI/Resources/Pipeline/Test.pipeline", nullptr);
	{
		RPI::Ptr<RPI::Scene> scene = new RPI::Scene();
		MemoryStream stream = MemoryStream(pipelineJson->GetData(), pipelineJson->GetDataSize());

		RenderPipeline* renderPipeline = RenderPipeline::CreateFromJson(&stream, scene);
		
		PassTree* tree = renderPipeline->passTree;
		ParentPass* rootPass = tree->rootPass;

		EXPECT_EQ(rootPass->GetName(), "DefaultPipelineRoot");
		EXPECT_EQ(rootPass->passes.GetSize(), 3);
		EXPECT_EQ(rootPass->passAttachments.GetSize(), 1);
		{
			EXPECT_EQ(rootPass->passAttachments[0]->name, "PipelineOutput");
			EXPECT_EQ(rootPass->passAttachments[0]->lifetime, RHI::AttachmentLifetimeType::External);
			EXPECT_EQ(rootPass->passAttachments[0]->attachmentDescriptor.type, RHI::AttachmentType::Image);
		}

		// Depth PrePass
		{
			Pass* pass = rootPass->passes[0];
			EXPECT_EQ(pass->GetName(), "DepthPrePass");
			EXPECT_EQ(pass->passAttachments.GetSize(), 1);
			EXPECT_EQ(pass->passAttachments[0]->name, "DepthStencil");
			EXPECT_EQ(pass->passAttachments[0]->lifetime, RHI::AttachmentLifetimeType::Transient);
			EXPECT_EQ(pass->passAttachments[0]->attachmentDescriptor.imageDesc.bindFlags, RHI::TextureBindFlags::DepthStencil);
			EXPECT_EQ(pass->passAttachments[0]->attachmentDescriptor.imageDesc.format, RHI::Format::D32_SFLOAT_S8_UINT);

			EXPECT_EQ(pass->inputBindings.GetSize(), 0);
			EXPECT_EQ(pass->inputOutputBindings.GetSize(), 0);
			EXPECT_EQ(pass->outputBindings.GetSize(), 1);

			EXPECT_EQ(pass->outputBindings[0].attachment, pass->passAttachments[0]);
		}

		// Opaque Pass
		{
			Pass* pass = rootPass->passes[1];
			EXPECT_EQ(pass->GetName(), "OpaquePass");

		}

		// Transparent Pass
		{
			Pass* pass = rootPass->passes[2];
			EXPECT_EQ(pass->GetName(), "TransparentPass");

		}

		delete renderPipeline;
		scene = nullptr;
	}
	pipelineJson->Destroy();

	TEST_END;
}

