#ifndef VOLLEyBALL_MATH_H
#define VOLLEyBALL_MATH_H


union v2
{
    struct
    {
        r32 x, y;
    };
    struct
    {
        r32 u, v;
    };
    r32 E[2];
};


inline v2 
operator*(r32 Scalar, v2 A)
{
    v2 Result;

    Result.x = A.x * Scalar;
    Result.y = A.y * Scalar;

    return Result;
}


inline v2 
operator*(v2 A, r32 Scalar)
{
    return Scalar * A;
}


inline v2 &
operator*=(v2 &A, r32 Scalar)
{
    A = A * Scalar;

    return A;
}


inline v2
operator+(v2 A, v2 B)
{
    v2 Result;

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;

    return Result;
}


inline v2 &
operator+=(v2 &A, v2 B)
{
    A = A + B;

    return A;
}


inline v2
operator-(v2 A, v2 B)
{
    v2 Result;

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;

    return Result;
}


inline v2 &
operator-=(v2 &A, v2 B)
{
    A = A - B;

    return A;
}


// Unary
inline v2
operator-(v2 A)
{
    v2 Result;

    Result.x = -A.x;
    Result.y = -A.y;

    return Result;
}


#endif