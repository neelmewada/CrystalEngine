#pragma once

namespace CE
{
	class Object;
	namespace Internal { class RefCountControl; }

	template<typename TObject> requires TIsBaseClassOf<Object, TObject>::Value
	class Ref final
	{
	public:


	private:

		Internal::RefCountControl* control = nullptr;
	};

	template<typename TObject> requires TIsBaseClassOf<Object, TObject>::Value
	class WeakRef final
	{
	public:


	private:

		Internal::RefCountControl* control = nullptr;
	};

	namespace Internal
	{
		class CORE_API RefCountControl
		{
		public:

			int AddStrongRef()
			{
				return strongReferences.fetch_add(1) + 1;
			}

			int AddWeakRef()
			{
				return weakReferences.fetch_add(1) + 1;
			}

			int GetNumStrongRefs()
			{
				return strongReferences;
			}

			int GetNumWeakRefs()
			{
				return weakReferences;
			}

			int ReleaseStrongRef();

			int ReleaseWeakRef();

			Object* GetObject();

		private:

			void SelfDestroy();

			enum class ObjectState : int
			{
				NotInitialized,
				Alive,
				Destroyed
			};

			std::atomic<int> strongReferences = 0;
			std::atomic<int> weakReferences = 0;
			Object* object = nullptr;
			SharedMutex lock{};

			ObjectState objectState = ObjectState::NotInitialized;
		};
	}

} // namespace CE
