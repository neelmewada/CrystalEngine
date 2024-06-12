#pragma once

namespace CE
{
    template<typename T, SIZE_T GrowthIncrement = 128, bool CallDestructor = true>
    class StableGrowthArray
    {
        CE_NO_COPY(StableGrowthArray);
        static_assert(GrowthIncrement > 0);
    public:

        StableGrowthArray()
        {}

        ~StableGrowthArray()
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

    struct FusionRendererInitInfo
    {
	    RPI::Shader* fusionShader = nullptr;
        RHI::MultisampleState multisampling{};
    };

    CLASS(Config = Engine)
    class FUSIONCORE_API FusionRenderer : public Object
    {
        CE_CLASS(FusionRenderer, Object)
    public:

        static constexpr u32 MaxImageCount = RHI::Limits::MaxSwapChainImageCount;
#if PAL_TRAIT_BUILD_EDITOR
        static constexpr u32 DrawItemGrowIncrement = 512;
#else
        static constexpr u32 DrawItemGrowIncrement = 128;
#endif


        FusionRenderer();

        void Init(const FusionRendererInitInfo& initInfo);

        void OnBeforeDestroy() override;

        void Begin();

        void End();

        const Array<RHI::DrawPacket*>& FlushDrawPackets(u32 imageIndex);

        void SetViewConstants(const RPI::PerViewConstants& viewConstants);

        void SetRootTransform(const Matrix4x4& transform);

        void SetTransformOverlay(const Matrix4x4& transform);

        void SetScreenSize(Vec2i screenSize);

        void SetDrawListTag(RHI::DrawListTag drawListTag);

        // - Draw API -


    protected:

        // - Data Structures -

        enum FDrawType : u32
        {
	        DRAW_Rect
        };

        struct alignas(16) FDrawItem2D
        {
            Matrix4x4 transform;
            FDrawType drawType = DRAW_Rect;
        };

        using FDrawItemList = StableGrowthArray<FDrawItem2D, DrawItemGrowIncrement, false>;

        struct FDrawBatch
        {
            u32 firstDrawItemIndex = 0;
            u32 drawItemCount = 0;
        };

        // - Internal Methods -

        FDrawItem2D& DrawCustomItem(FDrawType drawType, Vec2 pos, Vec2 size);

        // - Config -

        FIELD(Config)
        u32 initialDrawItemCapacity = 5000;

        FIELD(Config)
        f32 drawItemGrowRatio = 0.25f;

        // - State -

        u32 numFrames = 0;
        int curImageIndex = 0;
        Vec2i screenSize = Vec2i(0, 0);

        f32 angleDegrees = 0;
        Vec2 scaling = Vec2(1, 1);
        Matrix4x4 transformOverlay = Matrix4x4::Identity();

        Matrix4x4 rootTransform = Matrix4x4::Identity();
        RHI::MultisampleState multisampling{};

        // - Constants -

    	RPI::PerViewConstants viewConstants{};
        StaticArray<RHI::Buffer*, MaxImageCount> viewConstantsBuffer{};
        RHI::ShaderResourceGroup* perViewSrg = nullptr;

        // - Drawing -

        RPI::DynamicStructuredBuffer<FDrawItem2D> drawItemsBuffer;
        RHI::ShaderResourceGroup* drawItemSrg = nullptr;

        FDrawItemList drawItemList;

        Array<FDrawBatch> drawBatches{};
        bool createNewDrawBatch = false;

        // - Flush Draw Packets -

        Array<RHI::DrawPacket*> drawPackets{};
        StaticArray<bool, MaxImageCount> resubmitDrawItems{};
        StaticArray<bool, MaxImageCount> viewConstantsUpdateRequired{};
        RHI::DrawListTag drawListTag = RHI::DrawListTag::NullValue;

        // - Utils -

        RPI::Shader* fusionShader = nullptr;

        // - Cache -

        friend class FNativeContext;
    };
    
} // namespace CE

#include "FusionRenderer.rtti.h"
