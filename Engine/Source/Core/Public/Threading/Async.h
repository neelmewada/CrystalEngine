#pragma once

#include <future>

namespace CE
{
	template<typename ReturnType>
	class Future
	{
	public:

		Future() : isValid(false)
		{
            
		}

		/// Internal use only! Please use AsyncXXX() functions instead.
		template<class Functor, typename... Args>
		explicit Future(Delegate<void(const ReturnType&)> callback, Functor func, Args... args) : isValid(true)
		{
			typedef FunctionTraits<Functor> Traits;
			typedef typename Traits::Tuple TupleType;
			typedef typename Traits::ReturnType RetType;

			auto run = [=]() -> void
			{
                RetType val = func(args...);

				mutex.Lock();
				this->returnValue = val;
				this->isFinished = true;
				callback.InvokeIfValid(this->returnValue);
				mutex.Unlock();
			};

			runner = Thread([=]
				{
					run();
				});
		}

		FORCE_INLINE bool IsFinished() const { return isFinished; }

		FORCE_INLINE bool IsJoined() const { return isJoined; }

		FORCE_INLINE bool IsValid() const { return isValid; }

		FORCE_INLINE bool operator==(const Future& rhs)
		{
			return runner.GetId() == rhs.runner.GetId();
		}

		template<typename T>
		FORCE_INLINE bool operator==(const Future<T>& rhs)
		{
			return runner.GetId() == rhs.runner.GetId();
		}

		void Wait()
		{
			if (!isJoined && runner.IsJoinable())
			{
				isJoined = true;
				runner.Join();
			}
		}

		ReturnType Get()
		{
			if (!isFinished)
				Wait();
			return returnValue;
		}

		FORCE_INLINE SIZE_T GetHash()
		{
			return runner.GetId();
		}

	private:

		Mutex mutex{};
		Thread runner;

		ReturnType returnValue{};

		b8 isJoined = false;
		b8 isFinished = false;
		b8 isValid = false;
	};

	template<class Functor, typename... Args>
	inline Future<typename FunctionTraits<Functor>::ReturnType> Async(Functor func, Args&&... args)
	{
		return Future<typename FunctionTraits<Functor>::ReturnType>(nullptr, func, args...);
	}

	template<class Functor, typename... Args>
	inline Future<typename FunctionTraits<Functor>::ReturnType> AsyncCallback(Functor func, 
		Delegate<void(const typename FunctionTraits<Functor>::ReturnType&)> callback, Args&&... args)
	{
		return Future<typename FunctionTraits<Functor>::ReturnType>(callback, func, args...);
	}

} // namespace CE
