#ifndef SCOP_MATH_H
#define SCOP_MATH_H

float Abs (float x);

struct Vec2f
{
    float x = 0;
    float y = 0;
};

struct Vec3f
{
    float x = 0;
    float y = 0;
    float z = 0;
};

struct Vec4f
{
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 0;
};

struct Quatf
{
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1;
};

struct Mat3f
{
    float r0c0 = 1;
    float r0c1 = 0;
    float r0c2 = 0;
    float r1c0 = 0;
    float r1c1 = 1;
    float r1c2 = 0;
    float r2c0 = 0;
    float r2c1 = 0;
    float r2c2 = 1;
};

struct Mat4f
{
    float r0c0 = 1;
    float r0c1 = 0;
    float r0c2 = 0;
    float r0c3 = 0;
    float r1c0 = 0;
    float r1c1 = 1;
    float r1c2 = 0;
    float r1c3 = 0;
    float r2c0 = 0;
    float r2c1 = 0;
    float r2c2 = 1;
    float r2c3 = 0;
    float r3c0 = 0;
    float r3c1 = 0;
    float r3c2 = 0;
    float r3c3 = 1;
};

Vec2f Add (const Vec2f &a, const Vec2f &b);
Vec3f Add (const Vec3f &a, const Vec3f &b);
Vec4f Add (const Vec4f &a, const Vec4f &b);
Quatf Add (const Quatf &a, const Quatf &b);

Vec2f Sub (const Vec2f &a, const Vec2f &b);
Vec3f Sub (const Vec3f &a, const Vec3f &b);
Vec4f Sub (const Vec4f &a, const Vec4f &b);
Quatf Sub (const Quatf &a, const Quatf &b);

Vec2f Neg (const Vec2f &a);
Vec3f Neg (const Vec3f &a);
Vec4f Neg (const Vec4f &a);
Quatf Neg (const Quatf &a);

Vec2f Mul (const Vec2f &a, float b);
Vec3f Mul (const Vec3f &a, float b);
Vec4f Mul (const Vec4f &a, float b);
Quatf Mul (const Quatf &a, float b);

Vec2f Div (const Vec2f &a, float b);
Vec3f Div (const Vec3f &a, float b);
Vec4f Div (const Vec4f &a, float b);
Quatf Div (const Quatf &a, float b);

bool Equals (const Vec2f &a, const Vec2f &b);
bool Equals (const Vec3f &a, const Vec3f &b);
bool Equals (const Vec4f &a, const Vec4f &b);
bool Equals (const Quatf &a, const Quatf &b);

bool ApproxEquals (float a, float b, float epsilon);
bool ApproxEquals (const Vec2f &a, const Vec2f &b, float epsilon);
bool ApproxEquals (const Vec3f &a, const Vec3f &b, float epsilon);
bool ApproxEquals (const Vec4f &a, const Vec4f &b, float epsilon);
bool ApproxEquals (const Quatf &a, const Quatf &b, float epsilon);

bool ApproxZero (float a, float epsilon);
bool ApproxZero (const Vec2f &a, float epsilon);
bool ApproxZero (const Vec3f &a, float epsilon);
bool ApproxZero (const Vec4f &a, float epsilon);
bool ApproxZero (const Quatf &a, float epsilon);

inline Vec2f operator + (const Vec2f &a, const Vec2f &b) { return Add (a, b); }
inline Vec3f operator + (const Vec3f &a, const Vec3f &b) { return Add (a, b); }
inline Vec4f operator + (const Vec4f &a, const Vec4f &b) { return Add (a, b); }
inline Quatf operator + (const Quatf &a, const Quatf &b) { return Add (a, b); }

inline Vec2f operator - (const Vec2f &a, const Vec2f &b) { return Sub (a, b); }
inline Vec3f operator - (const Vec3f &a, const Vec3f &b) { return Sub (a, b); }
inline Vec4f operator - (const Vec4f &a, const Vec4f &b) { return Sub (a, b); }
inline Quatf operator - (const Quatf &a, const Quatf &b) { return Sub (a, b); }

inline Vec2f operator - (const Vec2f &a) { return Neg (a); }
inline Vec3f operator - (const Vec3f &a) { return Neg (a); }
inline Vec4f operator - (const Vec4f &a) { return Neg (a); }
inline Quatf operator - (const Quatf &a) { return Neg (a); }

inline Vec2f operator * (const Vec2f &a, float b) { return Mul (a, b); }
inline Vec3f operator * (const Vec3f &a, float b) { return Mul (a, b); }
inline Vec4f operator * (const Vec4f &a, float b) { return Mul (a, b); }
inline Quatf operator * (const Quatf &a, float b) { return Mul (a, b); }

inline Vec2f operator / (const Vec2f &a, float b) { return Div (a, b); }
inline Vec3f operator / (const Vec3f &a, float b) { return Div (a, b); }
inline Vec4f operator / (const Vec4f &a, float b) { return Div (a, b); }
inline Quatf operator / (const Quatf &a, float b) { return Div (a, b); }

inline bool operator == (const Vec2f &a, const Vec2f &b) { return Equals (a, b); }
inline bool operator == (const Vec3f &a, const Vec3f &b) { return Equals (a, b); }
inline bool operator == (const Vec4f &a, const Vec4f &b) { return Equals (a, b); }
inline bool operator == (const Quatf &a, const Quatf &b) { return Equals (a, b); }

float Dot (const Vec2f &a, const Vec2f &b);
float Dot (const Vec3f &a, const Vec3f &b);
float Dot (const Vec4f &a, const Vec4f &b);
float Dot (const Quatf &a, const Quatf &b);
Vec3f Cross (const Vec3f &a, const Vec3f &b);

#endif
