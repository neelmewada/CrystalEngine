#include "EditorCore.h"

namespace CE::Editor
{
	Array<AssetImportJob*> FontAssetImporter::CreateImportJobs(const Array<IO::Path>& sourceAssets, const Array<IO::Path>& productAssets)
	{
		Array<AssetImportJob*> jobs{};

		for (int i = 0; i < sourceAssets.GetSize(); i++)
		{
			FontAssetImportJob* job = new FontAssetImportJob(this, sourceAssets[i], productAssets[i]);
			job->genInfo.padding = 16;
			job->genInfo.fontSize = 64;

			job->genInfo.charSetRanges.Add(CharRange('a', 'z'));
			job->genInfo.charSetRanges.Add(CharRange('A', 'Z'));
			job->genInfo.charSetRanges.Add(CharRange('0', '9'));
			job->genInfo.charSetRanges.Add(CharRange(32, 47));
			job->genInfo.charSetRanges.Add(CharRange(58, 64));
			job->genInfo.charSetRanges.Add(CharRange(91, 96));
			job->genInfo.charSetRanges.Add(CharRange(123, 126));

			jobs.Add(job);
		}

		return jobs;
	}

	Array<Name> FontAssetImporter::GetProductAssetDependencies()
	{
		static const Array<Name> dependencies{
			"/Engine/Assets/Shaders/UI/SDFTextGen"
		};

		return dependencies;
	}

	bool FontAssetImportJob::ProcessAsset(Package* package)
	{
		if (package == nullptr)
			return false; // Should never happen
		if (!sourcePath.Exists())
			return false;

		// Clear the package of any subobjects/assets, we will build the asset from scratch
		package->DestroyAllSubobjects();

		String fileName = sourcePath.GetFilename().RemoveExtension().GetString();
		// Make sure we can use the fileName as name of an object
		fileName = FixObjectName(fileName);

		///////////////////////////////////////////////////
		// - Create rasterized atlas -

		CMFontAtlas* cmFontAtlas = CMFontAtlas::GenerateFromFontFile(sourcePath, genInfo);
		if (cmFontAtlas == nullptr)
		{
			errorMessage = "Failed to parse TTF Font!";
			return false;
		}

		defer(
			delete cmFontAtlas;
		);

		const CMImage& fontAtlasImage = cmFontAtlas->GetAtlas();
		if (!fontAtlasImage.IsValid())
		{
			errorMessage = "Invalid font atlas image!";
			return false;
		}
		
		///////////////////////////////////////////////////
		// - Setup objects -

		Font* font = CreateObject<Font>(package, fileName);
		font->fontAtlas = CreateObject<Texture2D>(font, "FontAtlas");

		const auto& glyphInfos = cmFontAtlas->GetGlyphInfos();
		font->glyphLayouts.Resize(glyphInfos.GetSize());

		for (int i = 0; i < glyphInfos.GetSize(); i++)
		{
			font->glyphLayouts[i].unicode = glyphInfos[i].charCode;
			font->glyphLayouts[i].x0 = glyphInfos[i].x0;
			font->glyphLayouts[i].x1 = glyphInfos[i].x1;
			font->glyphLayouts[i].y0 = glyphInfos[i].y0;
			font->glyphLayouts[i].y1 = glyphInfos[i].y1;

			font->glyphLayouts[i].xOffset = glyphInfos[i].xOffset;
			font->glyphLayouts[i].yOffset = glyphInfos[i].yOffset;

			font->glyphLayouts[i].advance = glyphInfos[i].advance;
		}

		////////////////////////////////////////////////////
		// - Create resources -

		RHI::SamplerDescriptor samplerDesc{};
		samplerDesc.addressModeU = samplerDesc.addressModeV = samplerDesc.addressModeW = SamplerAddressMode::ClampToBorder;
		samplerDesc.borderColor = SamplerBorderColor::FloatOpaqueBlack;
		samplerDesc.enableAnisotropy = false;
		samplerDesc.samplerFilterMode = FilterMode::Linear;

		RHI::Texture* rasterizedAtlas = nullptr;
		RPI::Texture* rasterizedAtlasRpi = nullptr;
		{
			RHI::TextureDescriptor textureDesc{};
			textureDesc.name = "Raster Atlas";
			textureDesc.bindFlags = TextureBindFlags::Color | TextureBindFlags::ShaderRead;
			textureDesc.defaultHeapType = MemoryHeapType::Default;
			textureDesc.width = fontAtlasImage.GetWidth();
			textureDesc.height = fontAtlasImage.GetHeight();
			textureDesc.depth = 1;
			textureDesc.arrayLayers = 1;
			textureDesc.mipLevels = 1;
			textureDesc.format = RHI::Format::R8_UNORM;
			textureDesc.dimension = RHI::Dimension::Dim2D;
			textureDesc.sampleCount = 1;

			rasterizedAtlas = RHI::gDynamicRHI->CreateTexture(textureDesc);
			rasterizedAtlasRpi = new RPI::Texture(rasterizedAtlas, samplerDesc);
		}

		RHI::Texture* sdfFontAtlas = nullptr;
		RPI::Texture* sdfFontAtlasRpi = nullptr;
		{
			RHI::TextureDescriptor desc{};
			desc.name = "Texture";
			desc.arrayLayers = 1;
			desc.bindFlags = TextureBindFlags::Color | TextureBindFlags::ShaderRead;
			desc.width = rasterizedAtlas->GetWidth();
			desc.height = rasterizedAtlas->GetHeight();
			desc.depth = 1;
			desc.defaultHeapType = MemoryHeapType::Default;
			desc.format = RHI::Format::R8_UNORM;
			desc.dimension = Dimension::Dim2D;
			desc.mipLevels = 1;

			sdfFontAtlas = RHI::gDynamicRHI->CreateTexture(desc);
			sdfFontAtlasRpi = new RPI::Texture(sdfFontAtlas);
		}

		// - Intermediate resources -

		RHI::Buffer* inputBuffer = nullptr;
		{
			RHI::BufferDescriptor desc{};
			desc.name = "Input Buffer";
			desc.bindFlags = BufferBindFlags::StagingBuffer;
			desc.defaultHeapType = MemoryHeapType::Upload;
			desc.bufferSize = rasterizedAtlas->GetWidth() * rasterizedAtlas->GetHeight();

			inputBuffer = RHI::gDynamicRHI->CreateBuffer(desc);

			// Upload rasterized image data
			void* inData;
			inputBuffer->Map(0, inputBuffer->GetBufferSize(), &inData);
			{
				memcpy(inData, fontAtlasImage.GetDataPtr(), inputBuffer->GetBufferSize());
			}
			inputBuffer->Unmap();
		}

		RHI::Buffer* outputBuffer = nullptr;
		{
			RHI::BufferDescriptor desc{};
			desc.name = "Output Image";
			desc.bindFlags = BufferBindFlags::StagingBuffer;
			desc.defaultHeapType = MemoryHeapType::ReadBack;
			desc.bufferSize = rasterizedAtlas->GetWidth() * rasterizedAtlas->GetHeight();

			outputBuffer = RHI::gDynamicRHI->CreateBuffer(desc);
		}

		defer(
			delete rasterizedAtlasRpi;
			delete sdfFontAtlasRpi;
			delete inputBuffer;
			delete outputBuffer;
		);

		/////////////////////////////////////////
		// - Setup shaders & materials -

		CE::Shader* sdfGenShader = gEngine->GetAssetManager()->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/UI/SDFTextGen");
		RPI::Material* sdfGenMaterial = new RPI::Material(sdfGenShader->GetOrCreateRPIShader(0));
		sdfGenMaterial->SetPropertyValue("_FontAtlas", rasterizedAtlasRpi);
		sdfGenMaterial->FlushProperties();

		defer(
			delete sdfGenMaterial;
		);

		////////////////////////////////////////
		// - Setup RHI -

		auto queue = RHI::gDynamicRHI->GetPrimaryGraphicsQueue();
		auto cmdList = RHI::gDynamicRHI->AllocateCommandList(queue);
		auto fence = RHI::gDynamicRHI->CreateFence(false);

		const auto& fullScreenQuad = RPISystem::Get().GetFullScreenQuad();
		auto drawArgs = RPISystem::Get().GetFullScreenQuadDrawArgs();

		RHI::RenderTarget* renderTarget = nullptr;
		RHI::RenderTargetBuffer* renderTargetBuffer = nullptr;
		{
			RHI::RenderTargetLayout rtLayout{};
			RHI::RenderAttachmentLayout colorAttachment{};
			colorAttachment.attachmentId = "Color";
			colorAttachment.attachmentUsage = ScopeAttachmentUsage::Color;
			colorAttachment.format = RHI::Format::R8_UNORM;
			colorAttachment.multisampleState.sampleCount = 1;
			colorAttachment.loadAction = AttachmentLoadAction::Clear;
			colorAttachment.storeAction = AttachmentStoreAction::Store;
			rtLayout.attachmentLayouts.Add(colorAttachment);

			renderTarget = RHI::gDynamicRHI->CreateRenderTarget(rtLayout);
			renderTargetBuffer = RHI::gDynamicRHI->CreateRenderTargetBuffer(renderTarget, { sdfFontAtlas });
		}

		// - Record Commands -

		cmdList->Begin();
		{
			RHI::ResourceBarrierDescriptor barrier{};

			barrier.resource = inputBuffer;
			barrier.fromState = ResourceState::Undefined;
			barrier.toState = ResourceState::CopySource;
			cmdList->ResourceBarrier(1, &barrier);

			barrier.resource = outputBuffer;
			barrier.fromState = ResourceState::Undefined;
			barrier.toState = ResourceState::CopyDestination;
			cmdList->ResourceBarrier(1, &barrier);

			barrier.resource = rasterizedAtlas;
			barrier.fromState = ResourceState::Undefined;
			barrier.toState = ResourceState::CopyDestination;
			cmdList->ResourceBarrier(1, &barrier);

			// Transfer: Input Buffer -> Rasterized Atlas Texture
			{
				RHI::BufferToTextureCopy copy{};
				copy.srcBuffer = inputBuffer;
				copy.bufferOffset = 0;
				copy.dstTexture = rasterizedAtlas;
				copy.baseArrayLayer = 0;
				copy.layerCount = 1;
				copy.mipSlice = 0;
				
				cmdList->CopyTextureRegion(copy);
			}

			barrier.resource = rasterizedAtlas;
			barrier.fromState = ResourceState::CopyDestination;
			barrier.toState = ResourceState::FragmentShaderResource;
			cmdList->ResourceBarrier(1, &barrier);

			barrier.resource = sdfFontAtlas;
			barrier.fromState = ResourceState::Undefined;
			barrier.toState = ResourceState::ColorOutput;
			cmdList->ResourceBarrier(1, &barrier);

			RHI::AttachmentClearValue clear{};
			clear.clearValue = Vec4(0, 0, 0, 0);

			cmdList->BeginRenderTarget(renderTarget, renderTargetBuffer, &clear);
			{
				RHI::ViewportState viewport{};
				viewport.x = viewport.y = 0;
				viewport.width = sdfFontAtlas->GetWidth();
				viewport.height = sdfFontAtlas->GetHeight();
				viewport.minDepth = 0.0f; viewport.maxDepth = 1.0f;
				cmdList->SetViewports(1, &viewport);

				RHI::ScissorState scissor{};
				scissor.x = scissor.y = 0;
				scissor.width = viewport.width;
				scissor.height = viewport.height;
				cmdList->SetScissors(1, &scissor);

				RHI::PipelineState* pipeline = sdfGenMaterial->GetCurrentShader()->GetPipeline();
				cmdList->BindPipelineState(pipeline);

				cmdList->SetShaderResourceGroup(sdfGenMaterial->GetShaderResourceGroup());
				cmdList->CommitShaderResources();

				cmdList->BindVertexBuffers(0, fullScreenQuad.GetSize(), fullScreenQuad.GetData());

				cmdList->DrawLinear(drawArgs);
			}
			cmdList->EndRenderTarget();

			// Transfer: SDF Atlas -> Output buffer
			{
				RHI::TextureToBufferCopy copy{};
				copy.srcTexture = sdfFontAtlas;
				copy.baseArrayLayer = 0;
				copy.layerCount = 1;
				copy.mipSlice = 0;
				copy.dstBuffer = outputBuffer;
				copy.bufferOffset = 0;

				cmdList->CopyTextureRegion(copy);
			}

			barrier.resource = outputBuffer;
			barrier.fromState = ResourceState::CopyDestination;
			barrier.toState = ResourceState::General;
			cmdList->ResourceBarrier(1, &barrier);
		}
		cmdList->End();

		queue->Execute(1, &cmdList, fence);
		fence->WaitForFence();

		// - Cleanup -

		delete cmdList;
		delete fence;
		delete renderTarget;
		delete renderTargetBuffer;

		return true;
	}

} // namespace CE::Editor
