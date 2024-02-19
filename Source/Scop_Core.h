#ifndef SCOP_CORE_H
#define SCOP_CORE_H

#include <stdlib.h>
#include <stdint.h>

typedef uint8_t  u8;
typedef  int8_t  s8;
typedef uint16_t u16;
typedef  int16_t s16;
typedef uint32_t u32;
typedef  int32_t s32;
typedef uint64_t u64;
typedef  int64_t s64;

typedef float  f32;
typedef double f64;

#define null nullptr

#define DebugBreak() __debugbreak ()
#define Panic(...) { printf ("Panic! " __VA_ARGS__); DebugBreak (); }
#define Assert(x, ...) { if (!(x)) { printf ("Assertion failed! " __VA_ARGS__); DebugBreak (); } }

struct String
{
    s64 length = 0;
    char *data = null;
};

template<typename T, typename Status = bool>
struct Result
{
    T value = {};
    union
    {
        Status status = {};
        Status ok;
    };

    static Result<T, Status> Good (const T &value, Status status)
    {
        Result<T, Status> res = {};
        res.value = value;
        res.status = status;

        return res;
    }

    static Result<T, Status> Bad (Status status)
    {
        Result<T, Status> res = {};
        res.status = status;

        return res;
    }
};

// Defer

template<typename T> struct scop_RemoveRef { using type = T; };
template<typename T> struct scop_RemoveRef<T &> { using type = T; };
template<typename T> struct scop_RemoveRef<T &&> { using type = T; };
template<typename T> inline T &&scop_forward (typename scop_RemoveRef<T>::type &t) { return static_cast<T &&> (t); }
template<typename T> inline T &&scop_forward (typename scop_RemoveRef<T>::type &&t) { return static_cast<T &&> (t); }

template<typename Tproc>
struct Defer
{
	Tproc proc;

	Defer (Tproc &&proc) : proc (scop_forward<Tproc> (proc)) {}
	~Defer () { proc (); }
};

template<typename Tproc> Defer<Tproc> DeferProcedureCall (Tproc proc) { return Defer<Tproc> (scop_forward<Tproc> (proc)); }

#define _defer1(x, y) x##y
#define _defer2(x, y) _defer1 (x, y)
#define _defer3(x)    _defer2 (x, __COUNTER__)
#define defer(code)   auto _defer3 (_defer_) = DeferProcedureCall ([&]() { code; })

#define StaticArraySize(arr) (sizeof (arr) / sizeof (*arr))

template<typename T>
struct Array
{
    s64 count = 0;
    T *data = null;
    s64 allocated = 0;

    T &operator [] (s64 index)
    {
        Assert (index >= 0 && index < count, "Array bounds check failed");

        return data[index];
    }

    const T &operator [] (s64 index) const
    {
        Assert (index >= 0 && index < count, "Array bounds check failed");

        return data[index];
    }
};

template<typename T>
void ArrayReserve (Array<T> *arr, s64 capacity)
{
    if (arr->allocated >= capacity)
        return;

    T *new_data = (T *)realloc (arr->data, capacity * sizeof (T));
    Assert (new_data != null);

    arr->data = new_data;
    arr->allocated = capacity;
}

template<typename T>
void ArrayFree (Array<T> *arr)
{
    free (arr->data);
    arr->data = null;
    arr->count = 0;
    arr->allocated = 0;
}

template<typename T>
T *ArrayPush (Array<T> *arr)
{
    if (arr->count >= arr->allocated)
        ArrayReserve (arr, arr->allocated * 2 + 8);

    T *ptr = &arr->data[arr->count];
    *ptr = T{};

    arr->count += 1;

    return ptr;
}

template<typename T>
T *ArrayPush (Array<T> *arr, const T item)
{
    T *ptr = ArrayPush (arr);
    *ptr = item;

    return ptr;
}

template<typename T>
void ArrayPop (Array<T> *arr)
{
    Assert (arr->count > 0);

    arr->count -= 1;
}

template<typename T>
void ArrayClear (Array<T> *arr)
{
    arr->count = 0;
}

Result<String> ReadEntireFile (const char *filename);

#endif
