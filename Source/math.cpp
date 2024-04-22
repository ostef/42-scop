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

float Length (const Vec2f &v)
{
    return sqrtf (Dot (v, v));
}

float Length (const Vec3f &v)
{
    return sqrtf (Dot (v, v));
}

float Length (const Vec4f &v)
{
    return sqrtf (Dot (v, v));
}

float Length (const Quatf &q)
{
    return sqrtf (Dot (q, q));
}

Vec2f Normalized (const Vec2f &v, const Vec2f &fallback)
{
    float length = Length (v);
    if (ApproxZero (length, 0.00001))
        return fallback;

    return v / length;
}

Vec3f Normalized (const Vec3f &v, const Vec3f &fallback)
{
    float length = Length (v);
    if (ApproxZero (length, 0.00001))
        return fallback;

    return v / length;
}

Vec4f Normalized (const Vec4f &v, const Vec4f &fallback)
{
    float length = Length (v);
    if (ApproxZero (length, 0.00001))
        return fallback;

    return v / length;
}


Vec3f Cross (const Vec3f &a, const Vec3f &b)
{
    return Vec3f{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

Vec3f Reject (const Vec3f &a, const Vec3f &b)
{
    return a - b * (Dot (a, b) / Dot (b, b));
}

Vec2f &Vec2f::operator += (const Vec2f &b) { *this = *this + b; return *this; }
Vec2f &Vec2f::operator -= (const Vec2f &b) { *this = *this - b; return *this; }
Vec2f &Vec2f::operator *= (float b) { *this = *this * b; return *this; }
Vec2f &Vec2f::operator /= (float b) { *this = *this / b; return *this; }

Vec3f &Vec3f::operator += (const Vec3f &b) { *this = *this + b; return *this; }
Vec3f &Vec3f::operator -= (const Vec3f &b) { *this = *this - b; return *this; }
Vec3f &Vec3f::operator *= (float b) { *this = *this * b; return *this; }
Vec3f &Vec3f::operator /= (float b) { *this = *this / b; return *this; }

Vec4f &Vec4f::operator += (const Vec4f &b) { *this = *this + b; return *this; }
Vec4f &Vec4f::operator -= (const Vec4f &b) { *this = *this - b; return *this; }
Vec4f &Vec4f::operator *= (float b) { *this = *this * b; return *this; }
Vec4f &Vec4f::operator /= (float b) { *this = *this / b; return *this; }

Quatf &Quatf::operator += (const Quatf &b) { *this = *this + b; return *this; }
Quatf &Quatf::operator -= (const Quatf &b) { *this = *this - b; return *this; }
Quatf &Quatf::operator *= (float b) { *this = *this * b; return *this; }
Quatf &Quatf::operator /= (float b) { *this = *this / b; return *this; }

Mat4f Inverted (const Mat4f &m)
{
    Vec3f a = {m.r0c0, m.r1c0, m.r2c0};
    Vec3f b = {m.r0c1, m.r1c1, m.r2c1};
    Vec3f c = {m.r0c2, m.r1c2, m.r2c2};
    Vec3f d = {m.r0c3, m.r1c3, m.r2c3};

    float x = m.r3c0;
    float y = m.r3c1;
    float z = m.r3c2;
    float w = m.r3c3;

    Vec3f s = Cross (a, b);
    Vec3f t = Cross (c, d);
    Vec3f u = a * y - b * x;
    Vec3f v = c * w - d * z;

    float det = Dot (s, v) + Dot (t, u);
    s /= det;
    t /= det;
    u /= det;
    v /= det;

    Vec3f r0 = Cross (b, v) + t * y;
    Vec3f r1 = Cross (v, a) - t * x;
    Vec3f r2 = Cross (d, u) + s * w;
    Vec3f r3 = Cross (u, c) - s * z;

    return Mat4f{
        r0.x, r0.y, r0.z, -Dot (b, t),
        r1.x, r1.y, r1.z,  Dot (a, t),
        r2.x, r2.y, r2.z, -Dot (d, s),
        r3.x, r3.y, r3.z,  Dot (c, s)
    };
}

Vec3f RightVector (const Mat4f &m)
{
    return Normalized (Vec3f{m.r0c0, m.r1c0, m.r2c0});
}

Vec3f UpVector (const Mat4f &m)
{
    return Normalized (Vec3f{m.r0c1, m.r1c1, m.r2c1});
}

Vec3f ForwardVector (const Mat4f &m)
{
    return Normalized (Vec3f{m.r0c2, m.r1c2, m.r2c2});
}

Mat4f Mat4fTranslate (const Vec3f &translation)
{
    return Mat4f{
        1, 0, 0, translation.x,
        0, 1, 0, translation.y,
        0, 0, 1, translation.z,
        0, 0, 0,             1
    };
}

Mat4f Mat4fRotate (const Vec3f &axis, float angle)
{
    Vec3f n = Normalized (axis);
    float s = sinf (angle);
    float c = cosf (angle);
    float c2 = 1 - c;

    Mat4f result;
    result.r0c0 = (n.x * n.x * c2) + c;
    result.r0c1 = (n.y * n.x * c2) - (n.z * s);
    result.r0c2 = (n.z * n.x * c2) + (n.y * s);
    result.r0c3 = 0;

    result.r1c0 = (n.x * n.y * c2) + (n.z * s);
    result.r1c1 = (n.y * n.y * c2) + c;
    result.r1c2 = (n.z * n.y * c2) - (n.x * s);
    result.r1c3 = 0;

    result.r2c0 = (n.x * n.z * c2) - (n.y * s);
    result.r2c1 = (n.y * n.z * c2) + (n.x * s);
    result.r2c2 = (n.z * n.z * c2) + c;
    result.r2c3 = 0;

    result.r3c0 = 0;
    result.r3c1 = 0;
    result.r3c2 = 0;
    result.r3c3 = 1;

    return result;
}

static Mat4f AlignZ (Vec3f axis, Vec3f up)
{
    Vec3f y = up - axis * Dot (up, axis);
    float ylen = Length (y);

    if (ApproxZero (ylen, 0.00001))
    {
        Vec3f k = {1,0,0};
        y = k - axis * Dot (k, axis);
        ylen = Length (y);

        if (ApproxZero (ylen, 0.00001))
        {
            Vec3f k = {0,0,1};
            y = k - axis * Dot (k, axis);
            ylen = Length (y);
        }
    }

    y /= ylen;
    Vec3f x = Cross (y, axis);

    return Mat4f{
        x.x, y.x, axis.x, 0,
        x.y, y.y, axis.y, 0,
        x.z, y.z, axis.z, 0,
          0,   0,      0, 1
    };
}

Mat4f Mat4fLookAt (const Vec3f &position, const Vec3f &target, const Vec3f &up)
{
    auto direction = Normalized (target - position, {0,0,1});
    auto up_vector = Normalized (up, {0,1,0});
    auto result = AlignZ (direction, up_vector);
    result.r0c3 = position.x;
    result.r1c3 = position.y;
    result.r2c3 = position.z;

    return result;
}

Mat4f Mat4fPerspectiveProjection (float fovy, float aspect, float znear, float zfar)
{
    float t = tanf (ToRads (fovy) * 0.5f) * znear;
    float b = -t;
    float r = aspect * t;
    float l = -r;
    float n = znear;
    float f = zfar;

    return Mat4f{
        2 * n / (r - l),               0, -(r + l) / (r - l),      0,
                      0, 2 * n / (t - b), -(t + b) / (t - b),      0,
                      0,               0,                 -1, -2 * n,
                      0,               0,                 -1,      0
    };
}

Mat4f Mul (const Mat4f &a, const Mat4f &b)
{
    Mat4f result = {};

    result.r0c0 = a.r0c0 * b.r0c0 + a.r0c1 * b.r1c0 + a.r0c2 * b.r2c0 + a.r0c3 * b.r3c0;
    result.r0c1 = a.r0c0 * b.r0c1 + a.r0c1 * b.r1c1 + a.r0c2 * b.r2c1 + a.r0c3 * b.r3c1;
    result.r0c2 = a.r0c0 * b.r0c2 + a.r0c1 * b.r1c2 + a.r0c2 * b.r2c2 + a.r0c3 * b.r3c2;
    result.r0c3 = a.r0c0 * b.r0c3 + a.r0c1 * b.r1c3 + a.r0c2 * b.r2c3 + a.r0c3 * b.r3c3;

    result.r1c0 = a.r1c0 * b.r0c0 + a.r1c1 * b.r1c0 + a.r1c2 * b.r2c0 + a.r1c3 * b.r3c0;
    result.r1c1 = a.r1c0 * b.r0c1 + a.r1c1 * b.r1c1 + a.r1c2 * b.r2c1 + a.r1c3 * b.r3c1;
    result.r1c2 = a.r1c0 * b.r0c2 + a.r1c1 * b.r1c2 + a.r1c2 * b.r2c2 + a.r1c3 * b.r3c2;
    result.r1c3 = a.r1c0 * b.r0c3 + a.r1c1 * b.r1c3 + a.r1c2 * b.r2c3 + a.r1c3 * b.r3c3;

    result.r2c0 = a.r2c0 * b.r0c0 + a.r2c1 * b.r1c0 + a.r2c2 * b.r2c0 + a.r2c3 * b.r3c0;
    result.r2c1 = a.r2c0 * b.r0c1 + a.r2c1 * b.r1c1 + a.r2c2 * b.r2c1 + a.r2c3 * b.r3c1;
    result.r2c2 = a.r2c0 * b.r0c2 + a.r2c1 * b.r1c2 + a.r2c2 * b.r2c2 + a.r2c3 * b.r3c2;
    result.r2c3 = a.r2c0 * b.r0c3 + a.r2c1 * b.r1c3 + a.r2c2 * b.r2c3 + a.r2c3 * b.r3c3;

    result.r3c0 = a.r3c0 * b.r0c0 + a.r3c1 * b.r1c0 + a.r3c2 * b.r2c0 + a.r3c3 * b.r3c0;
    result.r3c1 = a.r3c0 * b.r0c1 + a.r3c1 * b.r1c1 + a.r3c2 * b.r2c1 + a.r3c3 * b.r3c1;
    result.r3c2 = a.r3c0 * b.r0c2 + a.r3c1 * b.r1c2 + a.r3c2 * b.r2c2 + a.r3c3 * b.r3c2;
    result.r3c3 = a.r3c0 * b.r0c3 + a.r3c1 * b.r1c3 + a.r3c2 * b.r2c3 + a.r3c3 * b.r3c3;

    return result;
}
