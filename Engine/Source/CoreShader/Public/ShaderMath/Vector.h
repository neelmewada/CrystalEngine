#pragma once

namespace CE
{
	struct float1
	{
		constexpr static u8 vk_align = 4;

		inline operator float() const
		{
			return x;
		}

		float x = 0;
	};
    
	struct float2
	{
		constexpr static u8 vk_align = 8;
		
		union {
			struct {
				float x, y;
			};

			float xy[2] = { 0, 0 };
		};
	};

	struct float3
	{
		constexpr static u8 vk_align = 16;

		union {
			struct {
				float x, y, z;
			};

			float xyz[3] = { 0, 0, 0 };
		};
	};

	struct alignas(16) float4
	{
		constexpr static u8 vk_align = 16;

		union {
			struct {
				float x, y, z, w;
			};

			float xyzw[4] = { 0, 0, 0, 0 };

			struct {
				float xy[2];
				float yz[2];
			};

			struct {
				float xyz[3];
				float z;
			};
		};
	};

} // namespace CE
