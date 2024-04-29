#pragma once

namespace CE
{

	/*
	 * Copyright (c) Contributors to the Open 3D Engine Project.
	 *
	 * SPDX-License-Identifier: Apache-2.0 OR MIT
	 *
	 */

	/// @brief A job that allows caller to wait until it's completed. Usually added to the end of jobs to wait on all of them.
	class CORE_API JobCompletion : public Job
	{
	public:
		JobCompletion(JobContext* context = nullptr) : Job(context)
		{

		}

		void StartAndWaitForCompletion()
		{
			Start();
			
			waitSemaphore.acquire();
		}

	protected:

		void Process() override
		{
			waitSemaphore.release();
		}

		std::binary_semaphore waitSemaphore{ 0 };
	};
    
} // namespace CE
