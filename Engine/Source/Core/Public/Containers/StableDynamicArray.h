#pragma once

namespace CE
{
    
    template<typename T, SIZE_T GrowthIncrement = 128, bool CallDestructor = true>
    class StableDynamicArray
    {
        CE_NO_COPY(StableDynamicArray);
        static_assert(GrowthIncrement > 0);
    public:

        StableDynamicArray()
        {}

        ~StableDynamicArray()
        {
            Free();
        }

        const T& operator[](SIZE_T index) const
        {
            return data[index];
        }

        T& operator[](SIZE_T index)
        {
            return data[index];
        }

        const T& First() const
        {
            return data[0];
        }

        T& First()
        {
            return data[0];
        }

        const T& Last() const
        {
            return data[count - 1];
        }

        T& Last()
        {
            return data[count - 1];
        }

        void Reserve(SIZE_T totalElementCapacity)
        {
	        if (capacity < totalElementCapacity)
	        {
                T* newData = new T[totalElementCapacity];
                if (data != nullptr)
                {
                    memcpy(newData, data, sizeof(T) * count);
                    delete[] data;
                }
                data = newData;
                
                capacity = totalElementCapacity;
	        }
        }

        void Free()
        {
	        if (data != nullptr)
	        {
                if constexpr (CallDestructor)
		        {
			        for (int i = 0; i < count; ++i)
			        {
			        	data[i].~T();
			        }
		        }
                delete[] data;
	        }

            data = nullptr;
            capacity = count = 0;
        }

        void Grow()
        {
            Reserve(capacity + GrowthIncrement);
        }

        void Insert(const T& item)
        {
	        if (data == nullptr || count >= capacity)
	        {
                Grow();
	        }

            data[count++] = item;
        }

        void RemoveAll()
        {
            if constexpr (CallDestructor)
	        {
		        for (int i = 0; i < count; ++i)
		        {
		        	data[i].~T();
		        }
	        }
            count = 0;
        }

        void RemoveAt(SIZE_T index)
        {
            if (count == 0 || index >= count)
                return;

            for (int i = index; i < count; ++i)
            {
                if (i == count - 1)
                {
	                data[i].~T();
                }
                else
                {
	                data[i] = data[i + 1];
                }
            }

            count--;
        }

        void RemoveLast()
        {
            RemoveAt(count - 1);
        }

        bool IsEmpty() const
        {
	        return count == 0 || data == nullptr;
        }

        SIZE_T GetCapacity() const
        {
            return capacity;
        }

        SIZE_T GetCount() const
        {
            return count;
        }

        T* GetData() const
        {
            return data;
        }
        
    private:

        T* data = nullptr;
        SIZE_T capacity = 0;
        SIZE_T count = 0;
    };

} // namespace CE
