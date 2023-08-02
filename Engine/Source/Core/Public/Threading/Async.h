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
		Future(Functor func, Delegate<void(const ReturnType&)> callback, Args... args) : isValid(true)
		{
			typedef FunctionTraits<Functor> Traits;
			typedef typename Traits::Tuple TupleType;
			typedef typename Traits::ReturnType ReturnType;

			auto run = [=]() -> void
			{
				ReturnType val = func(args...);

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

		template<typename ReturnType, typename ClassOrStruct, typename... Args>
		Future(ReturnType(ClassOrStruct::* func)(Args...), ClassOrStruct* instance, Delegate<void(const ReturnType&)> callback, Args&&... args) : isValid(true)
		{
			auto run = [=]() -> void
			{
				ReturnType val = (instance->*func)(args...);
				
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
		return Future<typename FunctionTraits<Functor>::ReturnType>(func, nullptr, args...);
	}

	template<class Functor, typename... Args>
	inline Future<typename FunctionTraits<Functor>::ReturnType> AsyncCallback(Functor func, 
		Delegate<void(const typename FunctionTraits<Functor>::ReturnType&)> callback, Args&&... args)
	{
		return Future<typename FunctionTraits<Functor>::ReturnType>(func, callback, args...);
	}

	template<typename ReturnType, typename ClassOrStruct, typename... Args>
	inline Future<ReturnType> AsyncMember(ReturnType(ClassOrStruct::* func)(Args...), ClassOrStruct* instance, Args&&... args)
	{
		return Future<ReturnType>(func, instance, nullptr, args...);
	}

	template<typename ReturnType, typename ClassOrStruct, typename... Args>
	inline Future<ReturnType> AsyncMemberCallback(ReturnType(ClassOrStruct::* func)(Args...), ClassOrStruct* instance,
		Delegate<void(const ReturnType&)> callback, Args&&... args)
	{
		return Future<ReturnType>(func, instance, callback, args...);
	}

} // namespace CE
