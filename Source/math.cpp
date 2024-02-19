#include "Scop_Core.h"
#include "Scop_Math.h"

Vec2f Add (const Vec2f &a, const Vec2f &b)
{
    return Vec2f{
        a.x + b.x,
        a.y + b.y,
    };
}

Vec3f Add (const Vec3f &a, const Vec3f &b)
{
    return Vec3f{
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
    };
}

Vec4f Add (const Vec4f &a, const Vec4f &b)
{
    return Vec4f{
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
        a.w + b.w,
    };
}

Quatf Add (const Quatf &a, const Quatf &b)
{
    return Quatf{
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
        a.w + b.w,
    };
}

Vec2f Sub (const Vec2f &a, const Vec2f &b)
{
    return Vec2f{
        a.x - b.x,
        a.y - b.y,
    };
}

Vec3f Sub (const Vec3f &a, const Vec3f &b)
{
    return Vec3f{
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
    };
}

Vec4f Sub (const Vec4f &a, const Vec4f &b)
{
    return Vec4f{
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
        a.w - b.w,
    };
}

Quatf Sub (const Quatf &a, const Quatf &b)
{
    return Quatf{
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
        a.w - b.w,
    };
}

Vec2f Neg (const Vec2f &a)
{
    return Vec2f{
        -a.x,
        -a.y,
    };
}

Vec3f Neg (const Vec3f &a)
{
    return Vec3f{
        -a.x,
        -a.y,
        -a.z,
    };
}

Vec4f Neg (const Vec4f &a)
{
    return Vec4f{
        -a.x,
        -a.y,
        -a.z,
        -a.w,
    };
}

Quatf Neg (const Quatf &a)
{
    return Quatf{
        -a.x,
        -a.y,
        -a.z,
        -a.w,
    };
}

Vec2f Mul (const Vec2f &a, float b)
{
    return Vec2f{
        a.x * b,
        a.y * b,
    };
}

Vec3f Mul (const Vec3f &a, float b)
{
    return Vec3f{
        a.x * b,
        a.y * b,
        a.z * b,
    };
}

Vec4f Mul (const Vec4f &a, float b)
{
    return Vec4f{
        a.x * b,
        a.y * b,
        a.z * b,
        a.w * b,
    };
}

Quatf Mul (const Quatf &a, float b)
{
    return Quatf{
        a.x * b,
        a.y * b,
        a.z * b,
        a.w * b,
    };
}

Vec2f Div (const Vec2f &a, float b)
{
    return Vec2f{
        a.x / b,
        a.y / b,
    };
}

Vec3f Div (const Vec3f &a, float b)
{
    return Vec3f{
        a.x / b,
        a.y / b,
        a.z / b,
    };
}

Vec4f Div (const Vec4f &a, float b)
{
    return Vec4f{
        a.x / b,
        a.y / b,
        a.z / b,
        a.w / b,
    };
}

Quatf Div (const Quatf &a, float b)
{
    return Quatf{
        a.x / b,
        a.y / b,
        a.z / b,
        a.w / b,
    };
}

bool Equals (const Vec2f &a, const Vec2f &b)
{
    return a.x == b.x && a.y == b.y;
}

bool Equals (const Vec3f &a, const Vec3f &b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool Equals (const Vec4f &a, const Vec4f &b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

bool Equals (const Quatf &a, const Quatf &b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

bool ApproxEquals (float a, float b, float epsilon)
{
    return Abs (a - b) <= epsilon;
}

bool ApproxEquals (const Vec2f &a, const Vec2f &b, float epsilon)
{
    return ApproxEquals (a.x, b.x, epsilon)
        && ApproxEquals (a.y, b.y, epsilon);
}

bool ApproxEquals (const Vec3f &a, const Vec3f &b, float epsilon)
{
    return ApproxEquals (a.x, b.x, epsilon)
        && ApproxEquals (a.y, b.y, epsilon)
        && ApproxEquals (a.z, b.z, epsilon);
}

bool ApproxEquals (const Vec4f &a, const Vec4f &b, float epsilon)
{
    return ApproxEquals (a.x, b.x, epsilon)
        && ApproxEquals (a.y, b.y, epsilon)
        && ApproxEquals (a.z, b.z, epsilon)
        && ApproxEquals (a.w, b.w, epsilon);
}

bool ApproxEquals (const Quatf &a, const Quatf &b, float epsilon)
{
    return ApproxEquals (a.x, b.x, epsilon)
        && ApproxEquals (a.y, b.y, epsilon)
        && ApproxEquals (a.z, b.z, epsilon)
        && ApproxEquals (a.w, b.w, epsilon);
}

bool ApproxZero (float a, float epsilon)
{
    return Abs (a) <= epsilon;
}

bool ApproxZero (const Vec2f &a, float epsilon)
{
    return ApproxZero (a.x, epsilon)
        && ApproxZero (a.y, epsilon);
}

bool ApproxZero (const Vec3f &a, float epsilon)
{
    return ApproxZero (a.x, epsilon)
        && ApproxZero (a.y, epsilon)
        && ApproxZero (a.z, epsilon);
}

bool ApproxZero (const Vec4f &a, float epsilon)
{
    return ApproxZero (a.x, epsilon)
        && ApproxZero (a.y, epsilon)
        && ApproxZero (a.z, epsilon)
        && ApproxZero (a.w, epsilon);
}

bool ApproxZero (const Quatf &a, float epsilon)
{
    return ApproxZero (a.x, epsilon)
        && ApproxZero (a.y, epsilon)
        && ApproxZero (a.z, epsilon)
        && ApproxZero (a.w, epsilon);
}

float Dot (const Vec2f &a, const Vec2f &b)
{
    return a.x * b.x + a.y * b.y;
}

float Dot (const Vec3f &a, const Vec3f &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float Dot (const Vec4f &a, const Vec4f &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float Dot (const Quatf &a, const Quatf &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

Vec3f Cross (const Vec3f &a, const Vec3f &b)
{
    return Vec3f{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}
