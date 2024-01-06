#pragma once

#include "CoreMinimal.h"
#include "CoreApplication.h"
#include "CoreMedia.h"

#include "RHI/ThreadLocalContext.h"
#include "RHI/Limits.h"
#include "RHI/Handle.h"

#include "RHI/TagRegistry.h"

#include "RHI/RHIDefinitions.h"
#include "RHI/ClearValue.h"
#include "RHI/ShaderResourceGroupLayout.h"
#include "RHI/Resources.h"
#include "RHI/CommandQueue.h"
#include "RHI/MemoryHeap.h"
#include "RHI/BufferView.h"
#include "RHI/Buffer.h"
#include "RHI/Texture.h"
#include "RHI/SwapChain.h"
#include "RHI/CommandList.h"
#include "RHI/Sampler.h"
#include "RHI/PipelineState.h"
#include "RHI/Shader.h"
#include "RHI/ShaderResourceGroup.h"
#include "RHI/IndexBufferView.h"
#include "RHI/VertexBufferView.h"

// Draw Data
#include "RHI/DrawItem.h"
#include "RHI/DrawList.h"
#include "RHI/DrawPacket.h"
#include "RHI/DrawPacketBuilder.h"
#include "RHI/DrawListContext.h"

// Frame
#include "RHI/AttachmentDefines.h"
#include "RHI/FrameAttachment.h"
#include "RHI/BufferFrameAttachment.h"
#include "RHI/ImageFrameAttachment.h"
#include "RHI/SwapChainFrameAttachment.h"
#include "RHI/FrameAttachmentDatabase.h"
#include "RHI/ScopeAttachment.h"
#include "RHI/BufferScopeAttachment.h"
#include "RHI/ImageScopeAttachment.h"
#include "RHI/Scope.h"
#include "RHI/FrameGraph.h"
#include "RHI/FrameGraphBuilder.h"

#include "RHI/RHI.h"
#include "RHI/RHISystem.h"

namespace CE
{
    
}
