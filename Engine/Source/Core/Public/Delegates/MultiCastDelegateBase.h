#pragma once

#include "Delegate.h"

#include "Containers/Array.h"

namespace CE
{

    namespace Internal
    {
        template<class T>
        class MultiCastDelegateBase
        {
            static_assert(AlwaysFalse<T>, "Delegate should always be passed with function template arguments.");
        };

        template<class TRetType, class... TArgs>
        class MultiCastDelegateBase<TRetType(TArgs...)>
        {
        public:
            using RetType = TRetType;

            using InternalDelegate = Delegate<TRetType(TArgs...)>;
            using InvocationListType = Array<InternalDelegate>;

            MultiCastDelegateBase() : list({})
            {}

            MultiCastDelegateBase(const MultiCastDelegateBase& copy)
            {
                list.Clear();

                for (int i = 0; i < copy.list.GetSize(); i++)
                {
                    list.Add(copy.list[i]);
                }
            }

            DelegateHandle AddDelegateInstance(const InternalDelegate& delegate)
            {
                list.Add(delegate);
                return list.GetLast().GetHandle();
            }

            DelegateHandle operator+(const InternalDelegate& delegate)
            {
                return AddDelegateInstance(delegate);
            }

            MultiCastDelegateBase& operator-=(DelegateHandle handle)
            {
                RemoveDelegateInstance(handle);
                return *this;
            }

            void RemoveDelegateInstance(DelegateHandle handle)
            {
                for (int i = 0; i < list.GetSize(); i++)
                {
                    if (list[i].GetHandle() == handle)
                    {
                        list.RemoveAt(i);
                        return;
                    }
                }
            }

            void ClearAll()
            {
                list.Clear();
            }

            void Broadcast(TArgs... args) const
            {
                for (int i = 0; i < list.GetSize(); i++)
                {
                    if (list.At(i).IsValid())
                    {
                        list.At(i)(args...);
                    }
                }
            }

            u32 GetInvocationListCount() const
            {
                return (u32)list.GetSize();
            }

            const InvocationListType& GetInvocationList() const
            {
                return list;
            }

        protected:
            InvocationListType list{};
        };
    }

} // namespace CE
