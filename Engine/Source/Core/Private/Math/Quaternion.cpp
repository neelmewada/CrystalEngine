
#include "Math/Vector.h"
#include "Math/Quaternion.h"


namespace CE
{

    Quat Quat::Multiply(const Quat& quat)
    {
        Vec4 a = Vec4(X, Y, Z, W);
        Vec4 b = Vec4(quat.X, quat.Y, quat.Z, quat.W);

        return Quat(
            a.W * b.X + a.X * b.W + a.Y * b.Z - a.Z * b.Y,
            a.W * b.Y - a.X * b.Z + a.Y * b.W + a.Z * b.X,
            a.W * b.Z + a.X * b.Y - a.Y * b.X + a.Z * b.W,
            a.W * b.W - a.X * b.X - a.Y * b.Y - a.Z * b.Z
        );
    }

} // namespace CE
