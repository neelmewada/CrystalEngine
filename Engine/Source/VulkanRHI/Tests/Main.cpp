
#include "Core.h"
#include "CoreMedia.h"
#include "CoreRHI.h"
#include "VulkanRHI.h"

#include "VulkanRHIPrivate.h"

#include "VulkanRHITest.h"

#include <gtest/gtest.h>

using namespace CE;

#define TEST_BEGIN TestBegin()
#define TEST_END TestEnd()

#define LOG(x) std::cout << "\033[1;32m[ INFO ]\033[0m " << x << std::endl;
#define ERROR(x) std::cerr << "\033[1;31m[ ERROR ]\033[0m " << x << std::endl;
#define WARN(x) std::cerr << "\033[1;33m[ WARNING ]\033[0m " << x << std::endl;

static bool gErrorFound = false;

void OnValidationMessage(RHI::ValidationMessageType messageType, const char* message);

static void TestBegin()
{
	gProjectName = MODULE_NAME;
	gProjectPath = PlatformDirectories::GetLaunchDir();

	ModuleManager::Get().LoadModule("Core");
	ModuleManager::Get().LoadModule("CoreMedia");
	ModuleManager::Get().LoadModule("CoreRHI");
	ModuleManager::Get().LoadModule("VulkanRHI");

	RHI::gDynamicRHI = new Vulkan::VulkanRHI();
	RHI::gDynamicRHI->AddValidationCallbackHandler(OnValidationMessage, RHI::ValidationMessageType::Warning);

	RHI::gDynamicRHI->Initialize();
	RHI::gDynamicRHI->PostInitialize();
}

static void TestEnd()
{
	RHI::gDynamicRHI->PreShutdown();
	RHI::gDynamicRHI->Shutdown();

	delete RHI::gDynamicRHI;
	RHI::gDynamicRHI = nullptr;

	ModuleManager::Get().LoadModule("VulkanRHI");
	ModuleManager::Get().LoadModule("CoreRHI");
	ModuleManager::Get().LoadModule("CoreMedia");
	ModuleManager::Get().LoadModule("Core");	
}

void OnValidationMessage(RHI::ValidationMessageType messageType, const char* message)
{
	switch (messageType)
	{
	case RHI::ValidationMessageType::Error:
		ERROR("Vulkan Error: " << message);
		FAIL();
		break;
	case RHI::ValidationMessageType::Warning:
		WARN("Vulkan Warning: " << message);
		break;
	}
}

TEST(VulkanRHI, BufferAliasing)
{
	TEST_BEGIN;
	
	struct Data1
	{
		Vec4 vec4{};
		Color color{};
		u32 number = 0;
		u32 number2 = 0;
	};

	struct Data2
	{
		u32 number{};
		Color color{};
		Vec4 vec4{};
	};

	Data1 srcData1[4] = {};
	for (int i = 0; i < COUNTOF(srcData1); i++)
	{
		srcData1[i].vec4 = Vec4(1, 2, 3, 4) * i;
		srcData1[i].number = i;
		srcData1[i].number2 = i * 2;
		srcData1[i].color = Color(0.1f, 0.1f, 0.1f, 0.1f) * i;
	}

	Data2 srcData2[4] = {};
	for (int i = 0; i < COUNTOF(srcData2); i++)
	{
		srcData2[i].vec4 = Vec4(4, 3, 2, 1) * i;
		srcData2[i].number = i * 10;
		srcData2[i].color = Color(0.1f, 0.1f, 0.1f, 0.1f) * i;
	}

	RHI::MemoryHeapType heapType = RHI::MemoryHeapType::Default;

	RHI::BufferDescriptor src1Desc{};
	src1Desc.bufferSize = sizeof(srcData1);
	src1Desc.structureByteStride = sizeof(Data1);
	src1Desc.bindFlags = RHI::BufferBindFlags::Undefined;
	// Even if the buffer won't self allocate memory upon creation,
	// you still need to specify which heap type you want to use this buffer with.
	src1Desc.defaultHeapType = heapType;

	RHI::ResourceMemoryRequirements src1Req{};
	RHI::gDynamicRHI->GetBufferMemoryRequirements(src1Desc, src1Req);

	RHI::BufferDescriptor src2Desc{};
	src2Desc.bufferSize = sizeof(srcData2);
	src2Desc.structureByteStride = sizeof(Data2);
	src2Desc.bindFlags = RHI::BufferBindFlags::Undefined;
	src2Desc.defaultHeapType = heapType;

	RHI::ResourceMemoryRequirements src2Req{};
	RHI::gDynamicRHI->GetBufferMemoryRequirements(src2Desc, src2Req);

	u64 src1Offset = 0;

	u64 src2Offset = src1Req.size;
	src2Offset = Memory::GetAlignedSize(src2Offset, src2Req.offsetAlignment);
	
	u64 totalSize = src2Offset + src2Req.size;

	RHI::MemoryHeapDescriptor heapDesc{};
	heapDesc.allocationSize = totalSize;
	heapDesc.heapType = heapType;
	heapDesc.flags = src1Req.flags & src2Req.flags;
	heapDesc.usageFlags = RHI::MemoryHeapUsageFlags::Buffer;

	RHI::MemoryHeap* heap = RHI::gDynamicRHI->AllocateMemoryHeap(heapDesc);

	RHI::ResourceMemoryDescriptor mem1Desc{};
	mem1Desc.memoryHeap = heap;
	mem1Desc.memoryOffset = src1Offset;

	RHI::Buffer* src1 = RHI::gDynamicRHI->CreateBuffer(src1Desc, mem1Desc);
	RHI::BufferData src1Upload{};
	src1Upload.data = &srcData1;
	src1Upload.dataSize = sizeof(srcData1);
	src1Upload.startOffsetInBuffer = 0;
	src1->UploadData(src1Upload);

	RHI::ResourceMemoryDescriptor mem2Desc{};
	mem2Desc.memoryHeap = heap;
	mem2Desc.memoryOffset = src2Offset;

	RHI::Buffer* src2 = RHI::gDynamicRHI->CreateBuffer(src2Desc, mem2Desc);
	RHI::BufferData src2Upload{};
	src2Upload.data = &srcData2;
	src2Upload.dataSize = sizeof(srcData2);
	src2Upload.startOffsetInBuffer = 0;
	src2->UploadData(src2Upload);

	Data1* dstData1 = new Data1[COUNTOF(srcData1)];
	Data2* dstData2 = new Data2[COUNTOF(srcData2)];

	src1->ReadData(dstData1);
	src2->ReadData(dstData2);

	for (int i = 0; i < COUNTOF(srcData1); i++)
	{
		EXPECT_EQ(srcData1[i].vec4, Vec4(1, 2, 3, 4) * i);
		EXPECT_EQ(srcData1[i].number, i);
		EXPECT_EQ(srcData1[i].number2, i * 2);
		EXPECT_EQ(srcData1[i].color.r, 0.1f * i);
	}

	for (int i = 0; i < COUNTOF(srcData2); i++)
	{
		EXPECT_EQ(srcData2[i].vec4, Vec4(4, 3, 2, 1) * i);
		EXPECT_EQ(srcData2[i].number, i * 10);
		EXPECT_EQ(srcData2[i].color.r, 0.1f * i);
	}

	delete[] dstData1;
	delete[] dstData2;

	RHI::gDynamicRHI->DestroyBuffer(src1);
	RHI::gDynamicRHI->DestroyBuffer(src2);

	RHI::gDynamicRHI->FreeMemoryHeap(heap);

	TEST_END;
}

