
#include "Math/Vector.h"
#include "Math/Quaternion.h"


namespace CE
{

    Quat Quat::Multiply(const Quat& quat)
    {
        Vec4 a = Vec4(x, y, z, w);
        Vec4 b = Vec4(quat.x, quat.y, quat.z, quat.w);

        return Quat(
            a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
            a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
            a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
            a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
        );
    }

} // namespace CE
