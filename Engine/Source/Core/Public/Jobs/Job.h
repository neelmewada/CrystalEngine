#pragma once

namespace CE
{
	class TypeInfo;
	class JobContext;

	class CORE_API Job
	{
	public:

		Job(bool isAutoDelete = true, JobContext* context = nullptr);
		virtual ~Job();

		inline JobContext* GetContext() const { return context; }

		/// Allows the job to start executing. Auto-delete jobs should not be accessed after calling this function.
		void Start();

		bool IsAutoDelete();

		Job* GetDependent();

		/// Sets the dependent job. The dependent job won't be allowed to run until `this` job has finished executing.
		/// Both jobs should be in un-started state when calling this function.
		void SetDependent(Job* dependent);

		/// Override this function to implement the job
		virtual void Process() = 0;

		/// Called when the job is finished
		virtual void Finish() {}

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
			FLAGS_DEPENDENTCOUNT_MASK =	0x00ffff,
			FLAGS_FLAG_MASK =			0xff0000,

			FLAGS_AUTO_DELETE =			0x010000,
			FLAGS_CANCELLED =			0x020000,
			FLAGS_CHILD_JOBS =			0x040000,
		};

		JobContext* volatile context = nullptr;

		// Number of jobs we are currently waiting on
		Atomic<u32> dependentCountAndFlags = 0;

		// Job which is dependent on us, and and will be notified once we are finished
		Atomic<Job*> dependent = nullptr;

		friend class JobQueue;
		friend class JobManager;
	};
}

