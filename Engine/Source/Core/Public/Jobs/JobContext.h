#pragma once

/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */


namespace CE
{
	class JobManager;

	class CORE_API JobContext
	{
	public:

		JobContext(JobManager* manager)
			: manager(manager)
		{

		}

		/// Ideally, you should only have one context pushed
		static void PushGlobalContext(JobContext* context);

		static void PopGlobalContext();

		static JobContext* GetGlobalContext();

		inline JobManager* GetJobManager() const
		{
			return manager;
		}

	private:

		JobManager* manager = nullptr;
	};
    
} // namespace CE
