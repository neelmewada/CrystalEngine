#include "CoreMinimal.h"

/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */


namespace CE
{
	static Array<JobContext*> gJobContext = {};

	void JobContext::PushGlobalContext(JobContext* context)
	{
		gJobContext.Push(context);
	}

	void JobContext::PopGlobalContext()
	{
		gJobContext.Pop();
	}

	JobContext* JobContext::GetGlobalContext()
    {
		if (gJobContext.IsEmpty())
			return nullptr;
        return gJobContext.Top();
    }

} // namespace CE
