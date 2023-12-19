#pragma once

namespace CE
{
	class TypeInfo;
	class JobContext;

	enum : u32
	{
		JOB_THREAD_UNDEFINED = 0,
		/// The default tag for all worker threads
		JOB_THREAD_WORKER,
		JOB_THREAD_PRIMARY,
		JOB_THREAD_GAME,
		JOB_THREAD_UPDATE,
		JOB_THREAD_RENDER,
		JOB_THREAD_ASSET,
		JOB_THREAD_PHYSICS,
		JOB_THREAD_AUDIO,
		JOB_THREAD_ANIMATION,
		JOB_THREAD_AI,

		// 16-bit LSB are built-in tags
		JOB_THREAD_BUILTIN_FLAGS = 0x0000ffff,
		// 16-bit MSB are custom tags
		JOB_THREAD_CUSTOM_FLAGS =  0xffff0000,
	};

	typedef u32 JobThreadTag;


	/*
	 * Copyright (c) Contributors to the Open 3D Engine Project.
	 *
	 * SPDX-License-Identifier: Apache-2.0 OR MIT
	 *
	 */

	class CORE_API Job
	{
	public:

		Job(bool isAutoDelete = true, JobContext* context = nullptr);
		virtual ~Job();

		inline JobContext* GetContext() const { return context; }

		/// Override to specify a name for this job
		virtual String GetName() const { return "Job"; }

		/// Allows the job to start executing. Auto-delete jobs should not be accessed after calling this function.
		void Start();

		/// Resets the job to be reused. Should not be used if Job is of auto-delete type.
		void Reset(bool clearDependent);

		void StartAsChild(Job* childJob);

		/// Suspend processing of this job until all children jobs are complete
		void WaitForChildren();

		/// Override this method to implement reset functionality
		virtual void OnReset() {}

		bool IsAutoDelete();
		bool IsFinished();
		bool IsCancelled();

		Job* GetDependent();

		FORCE_INLINE JobThreadTag GetThreadFilter() const { return threadFilter; }

		/// Must be called before calling Start() function.
		void SetThreadFilter(JobThreadTag threadTag);

		/// Sets the dependent job. The dependent job won't be allowed to run until `this` job has finished executing.
		/// Both jobs should be in un-started state when calling this function.
		void SetDependent(Job* dependent);

		/// Sets the child dependent job.
		void SetDependentChild(Job* dependent);

		void SetAutoDelete(bool set);

		/// Override this function to implement the job
		virtual void Process() = 0;

		/// Called when the job is finished
		virtual void Finish() {}

		/// Wait on this thread until job is finished. Start() has to be called before calling Complete().
		void Complete();

		u32 GetDependentCount();
		// Increment the number of jobs this job is waiting on before it's execution
		void IncrementDependentCount();

		void DecrementDependentCount();

		void IncrementDependentCountAndSetChildFlag();

		void StoreDependent(Job* dependent);
		u32 GetDependentCountAndFlags();
		void SetDependentCountAndFlags(u32 countAndFlags);

	private:

		bool IsInitialized();

		enum : u32
		{
			FLAGS_DEPENDENTCOUNT_MASK =	0x0000ffff, // 16-bit LSB
			FLAGS_FLAG_MASK =			0xffff0000, // 16-bit MSB

			FLAGS_NONE =				0x00000000,
			FLAGS_AUTO_DELETE =			0x00010000,
			FLAGS_CANCELLED =			0x00020000,
			FLAGS_CHILD_JOBS =			0x00040000,
			FLAGS_FINISHED =			0x00080000,
		};

		JobContext* volatile context = nullptr;

		JobThreadTag threadFilter = 0;

		// Number of jobs we are currently waiting on
		Atomic<u32> dependentCountAndFlags = 0;

		// Job which is dependent on us, and and will be notified once we are finished
		Atomic<Job*> dependent = nullptr;

		friend class WorkQueue;
		friend class JobManager;
	};
}

