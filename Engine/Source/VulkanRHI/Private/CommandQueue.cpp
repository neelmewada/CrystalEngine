
#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{

	CommandQueue::CommandQueue(VulkanDevice* device, 
		u32 familyIndex, u32 queueIndex, 
		RHI::HardwareQueueClassMask queueMask, 
		VkQueue queue, bool presentSupported)
		: device(device)
		, familyIndex(familyIndex)
		, queueIndex(queueIndex)
		, queue(queue)
		, presentSupported(presentSupported)
	{
		this->queueMask = queueMask;

		thread = new SubmissionThread();

		thread->thread = Thread([this]()
			{
				ProcessSubmissionThread();
			});
	}

	void CommandQueue::ProcessSubmissionThread()
	{
		while (!thread->terminate)
		{
			thread->sleepEvent.acquire();

			submissionMutex.Lock();
			Array<SubmitBatch> submissions = this->submissions;
			this->submissions.Clear();

			submissionMutex.Unlock();

			for (int batchIdx = 0; batchIdx < submissions.GetSize(); batchIdx++)
			{
				List<VkSubmitInfo> submitInfos{};

				for (int i = 0; i < submissions[batchIdx].submitInfos.GetSize(); i++)
				{
					VkSubmitInfo submitInfo{};
					submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
					submitInfo.commandBufferCount = submissions[batchIdx].submitInfos[i].commandBuffers.GetSize();
					submitInfo.pCommandBuffers = submissions[batchIdx].submitInfos[i].commandBuffers.GetData();
					submitInfo.waitSemaphoreCount = submissions[batchIdx].submitInfos[i].waitSemaphores.GetSize();
					submitInfo.pWaitSemaphores = submissions[batchIdx].submitInfos[i].waitSemaphores.GetData();
					submitInfo.pWaitDstStageMask = submissions[batchIdx].submitInfos[i].waitDstStageMask.GetData();
					submitInfo.signalSemaphoreCount = submissions[batchIdx].submitInfos[i].signalSemaphores.GetSize();
					submitInfo.pSignalSemaphores = submissions[batchIdx].submitInfos[i].signalSemaphores.GetData();

					submitInfos.Add(submitInfo);
				}

				if (!submitInfos.IsEmpty())
				{
					vkQueueSubmit(queue, submitInfos.GetSize(), submitInfos.GetData(), submissions[batchIdx].fence);
				}
			}

			submitEvent.release();
		}
	}

	CommandQueue::~CommandQueue()
	{
		thread->sleepEvent.release();

		thread->terminate = true;
		if (thread->thread.IsJoinable())
			thread->thread.Join();
		
		delete thread;
	}

	bool CommandQueue::Execute(u32 count, RHI::CommandList** commandLists, RHI::Fence* fence)
	{
		if (count == 0)
			return true;

		SubmitInfo info{};
		info.commandBuffers.Resize(count);

		for (int i = 0; i < count; i++)
		{
			if (commandLists[i] == nullptr)
				return false;

			info.commandBuffers[i] = ((Vulkan::CommandList*)commandLists[i])->GetCommandBuffer();
		}

		submissionMutex.Lock();
		if (fence != nullptr)
		{
			auto submitFence = ((Vulkan::Fence*)fence)->GetHandle();
			submissions.Add(SubmitBatch{ { info }, submitFence});
		}
		else
		{
			submissions.Add(SubmitBatch{ { info }, VK_NULL_HANDLE });
		}
		submissionMutex.Unlock();
		
		thread->sleepEvent.release();
		submitEvent.acquire();

		return true;
	}

	bool CommandQueue::Submit(u32 count, VkSubmitInfo* submitInfos, VkFence fence)
	{
		if (count == 0)
			return true;

		Array<SubmitInfo> infos{};
		infos.Resize(count);

		for (int i = 0; i < count; i++)
		{
			infos[i].commandBuffers.Resize(submitInfos[i].commandBufferCount);
			
			for (int j = 0; j < submitInfos[i].commandBufferCount; j++)
			{
				infos[i].commandBuffers[j] = submitInfos[i].pCommandBuffers[j];
			}

			infos[i].waitDstStageMask.Resize(submitInfos[i].waitSemaphoreCount);
			infos[i].waitSemaphores.Resize(submitInfos[i].waitSemaphoreCount);
			
			for (int j = 0; j < submitInfos[i].waitSemaphoreCount; j++)
			{
				infos[i].waitSemaphores[j] = submitInfos[i].pWaitSemaphores[j];
				infos[i].waitDstStageMask[j] = submitInfos[i].pWaitDstStageMask[j];
			}

			infos[i].signalSemaphores.Resize(submitInfos[i].signalSemaphoreCount);
			
			for (int j = 0; j < submitInfos[i].signalSemaphoreCount; j++)
			{
				infos[i].signalSemaphores[j] = submitInfos[i].pSignalSemaphores[j];
			}
		}

		submissionMutex.Lock();
		submissions.Add(SubmitBatch{ infos, fence });
		submissionMutex.Unlock();

		thread->sleepEvent.release();
		submitEvent.acquire();

		return true;
	}

} // namespace CE
