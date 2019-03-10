// See LICENSE for license details.

#pragma once

template <size_t N, typename T> struct EGVecP;
template <size_t N, typename T> struct EGVecT;

#define NORMALIZE_USES_RECIPROCAL 1

/* EGVecP */

template <size_t N, typename T>
struct EGVecP
{
    typedef T value_type;
    
	T *vec;
    
	size_t size(void) const { return sizeof(T) * N; }
    size_t dim(void) const { return N; }
    
    EGVecP<N,T>(T v[N]);
    EGVecP<N,T>(const EGVecT<N,T> &o);
    EGVecP<N,T>(const EGVecP<N,T> &o);
    EGVecP<N,T>& operator=(const T v[N]);
    EGVecP<N,T>& operator=(const EGVecP<N,T> &o);
    
    T& operator[](const size_t index);
    operator std::string();
    
    T magnitude();
    void normalize();
    EGVecT<N,T> normalized();
    
    EGVecT<N,T> operator+(const EGVecP<N,T> &o);
    EGVecT<N,T> operator+(const T v[N]);
    EGVecT<N,T> operator+(T scalar);
    EGVecP<N,T>& operator+=(const EGVecP<N,T> &o);
    EGVecP<N,T>& operator+=(const T v[N]);
    EGVecP<N,T>& operator+=(T scalar);
    EGVecT<N,T> operator-(const EGVecP<N,T> &o);
    EGVecT<N,T> operator-(const T v[N]);
    EGVecT<N,T> operator-(T scalar);
    EGVecP<N,T>& operator-=(const EGVecP<N,T> &o);
    EGVecP<N,T>& operator-=(const T v[N]);
    EGVecP<N,T>& operator-=(T scalar);
    EGVecT<N,T> operator*(const EGVecP<N,T> &o);
    EGVecT<N,T> operator*(const T v[N]);
    EGVecT<N,T> operator*(T scalar);
    EGVecP<N,T>& operator*=(const EGVecP<N,T> &o);
    EGVecP<N,T>& operator*=(const T v[N]);
    EGVecP<N,T>& operator*=(T scalar);
    EGVecT<N,T> operator/(const EGVecP<N,T> &o);
    EGVecT<N,T> operator/(const T v[N]);
    EGVecT<N,T> operator/(T scalar);
    EGVecP<N,T>& operator/=(const EGVecP<N,T> &o);
    EGVecP<N,T>& operator/=(const T v[N]);
    EGVecP<N,T>& operator/=(T scalar);
    bool operator==(const EGVecT<N,T> &o);
    bool operator!=(const EGVecT<N,T> &o);
    bool operator<(const EGVecT<N,T> &o);
    bool operator>(const EGVecT<N,T> &o);
    bool operator<=(const EGVecT<N,T> &o);
    bool operator>=(const EGVecT<N,T> &o);
};


/* EGVecT */

template <size_t N, typename T>
struct EGVecT
{
    typedef T value_type;
    
	T vec[N];
    
	size_t size(void) const { return sizeof(T) * N; }
    size_t dim(void) const { return N; }
    
    EGVecT<N,T>();
    EGVecT<N,T>(T x);
    EGVecT<N,T>(T x, T y);
    EGVecT<N,T>(T x, T y, T z);
    EGVecT<N,T>(T x, T y, T z, T w);
    EGVecT<N,T>(const T v[N]);
    EGVecT<N,T>(const EGVecT<N,T> &o);
    EGVecT<N,T>(const EGVecP<N,T> &o);
    EGVecT<N,T>& operator=(const T v[N]);
    EGVecT<N,T>& operator=(const EGVecT<N,T> &o);
    
    T& operator[](const size_t index);
    operator std::string();
    
    T magnitude();
    void normalize();
    EGVecT<N,T> normalized();
    
    EGVecT<N,T> operator+(const EGVecT<N,T> &o);
    EGVecT<N,T> operator+(const T v[N]);
    EGVecT<N,T> operator+(T scalar);
    EGVecT<N,T>& operator+=(const EGVecT<N,T> &o);
    EGVecT<N,T>& operator+=(const T v[N]);
    EGVecT<N,T>& operator+=(T scalar);
    EGVecT<N,T> operator-(const EGVecT<N,T> &o);
    EGVecT<N,T> operator-(const T v[N]);
    EGVecT<N,T> operator-(T scalar);
    EGVecT<N,T>& operator-=(const EGVecT<N,T> &o);
    EGVecT<N,T>& operator-=(const T v[N]);
    EGVecT<N,T>& operator-=(T scalar);
    EGVecT<N,T> operator*(const EGVecT<N,T> &o);
    EGVecT<N,T> operator*(const T v[N]);
    EGVecT<N,T> operator*(T scalar);
    EGVecT<N,T>& operator*=(const EGVecT<N,T> &o);
    EGVecT<N,T>& operator*=(const T v[N]);
    EGVecT<N,T>& operator*=(T scalar);
    EGVecT<N,T> operator/(const EGVecT<N,T> &o);
    EGVecT<N,T> operator/(const T v[N]);
    EGVecT<N,T> operator/(T scalar);
    EGVecT<N,T>& operator/=(const EGVecT<N,T> &o);
    EGVecT<N,T>& operator/=(const T v[N]);
    EGVecT<N,T>& operator/=(T scalar);
    bool operator==(const EGVecT<N,T> &o);
    bool operator!=(const EGVecT<N,T> &o);
    bool operator<(const EGVecT<N,T> &o);
    bool operator>(const EGVecT<N,T> &o);
    bool operator<=(const EGVecT<N,T> &o);
    bool operator>=(const EGVecT<N,T> &o);
};


/* EGVecP member definitions */

template <size_t N, typename T>
EGVecP<N,T>::EGVecP(T v[N])
{
    vec = v;
}

template <size_t N, typename T>
EGVecP<N,T>::EGVecP(const EGVecT<N,T> &o)
{
    vec = const_cast<T*>(o.vec);
}

template <size_t N, typename T>
EGVecP<N,T>::EGVecP(const EGVecP<N,T> &o)
{
    vec = o.vec;
}

template <size_t N, typename T>
EGVecP<N,T>& EGVecP<N,T>::operator=(const T v[N])
{
    vec = v;
    return (*this);
}

template <size_t N, typename T>
EGVecP<N,T>& EGVecP<N,T>::operator=(const EGVecP<N,T> &o)
{
    vec = o.vec;
    return (*this);
}

template <size_t N, typename T>
T& EGVecP<N,T>::operator[](const size_t index)
{
    return vec[index];
}

template <size_t N, typename T>
EGVecP<N,T>::operator std::string()
{
    std::stringstream ss;
    ss << "(";
    for (size_t i = 0; i < N; i++) {
        if (i != 0) ss << ",";
        ss << vec[i];
    }
    ss << ")";
    return ss.str();
}

template <size_t N, typename T>
T EGVecP<N,T>::magnitude()
{
    T sumsq = 0;
    for (size_t i = 0; i < N; i++) sumsq += (vec[i] * vec[i]);
    return sqrt(sumsq);
}

template <size_t N, typename T>
void EGVecP<N,T>::normalize()
{
#if NORMALIZE_USES_RECIPROCAL
    T rmag = 1.0f / magnitude();
    for (size_t i = 0; i < N; i++) vec[i] *= rmag;
#else
    T mag = magnitude();
    for (size_t i = 0; i < N; i++) vec[i] /= mag;
#endif
}

template <size_t N, typename T>
EGVecT<N,T> EGVecP<N,T>::normalized()
{
    EGVecT<N,T> n(vec);
    n.normalize();
    return n;
}

template <size_t N, typename T>
EGVecT<N,T> EGVecP<N,T>::operator+(const EGVecP<N,T> &o)
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] + o.vec[i];
    return n;
}

template <size_t N, typename T>
EGVecT<N,T> EGVecP<N,T>::operator+(const T v[N])
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] + v[i];
    return n;
}

template <size_t N, typename T>
EGVecT<N,T> EGVecP<N,T>::operator+(T scalar)
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] + scalar;
    return n;
}

template <size_t N, typename T>
EGVecP<N,T>& EGVecP<N,T>::operator+=(const EGVecP<N,T> &o)
{
    for (size_t i = 0; i < N; i++) vec[i] += o.vec[i];
    return (*this);
}

template <size_t N, typename T>
EGVecP<N,T>& EGVecP<N,T>::operator+=(const T v[N])
{
    for (size_t i = 0; i < N; i++) vec[i] += v[i];
    return (*this);
}

template <size_t N, typename T>
EGVecP<N,T>& EGVecP<N,T>::operator+=(T scalar)
{
    for (size_t i = 0; i < N; i++) vec[i] += scalar;
    return (*this);
}

template <size_t N, typename T>
EGVecT<N,T> EGVecP<N,T>::operator-(const EGVecP<N,T> &o)
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] - o.vec[i];
    return n;
}

template <size_t N, typename T>
EGVecT<N,T> EGVecP<N,T>::operator-(const T v[N])
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] - v[i];
    return n;
}

template <size_t N, typename T>
EGVecT<N,T> EGVecP<N,T>::operator-(T scalar)
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] - scalar;
    return n;
}

template <size_t N, typename T>
EGVecP<N,T>& EGVecP<N,T>::operator-=(const EGVecP<N,T> &o)
{
    for (size_t i = 0; i < N; i++) vec[i] -= o.vec[i];
    return (*this);
}

template <size_t N, typename T>
EGVecP<N,T>& EGVecP<N,T>::operator-=(const T v[N])
{
    for (size_t i = 0; i < N; i++) vec[i] -= v[i];
    return (*this);
}

template <size_t N, typename T>
EGVecP<N,T>& EGVecP<N,T>::operator-=(T scalar)
{
    for (size_t i = 0; i < N; i++) vec[i] -= scalar;
    return (*this);
}

template <size_t N, typename T>
EGVecT<N,T> EGVecP<N,T>::operator*(const EGVecP<N,T> &o)
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] * o.vec[i];
    return n;
}

template <size_t N, typename T>
EGVecT<N,T> EGVecP<N,T>::operator*(const T v[N])
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] * v[i];
    return n;
}

template <size_t N, typename T>
EGVecT<N,T> EGVecP<N,T>::operator*(T scalar)
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] * scalar;
    return n;
}

template <size_t N, typename T>
EGVecP<N,T>& EGVecP<N,T>::operator*=(const EGVecP<N,T> &o)
{
    for (size_t i = 0; i < N; i++) vec[i] *= o.vec[i];
    return (*this);
}

template <size_t N, typename T>
EGVecP<N,T>& EGVecP<N,T>::operator*=(const T v[N])
{
    for (size_t i = 0; i < N; i++) vec[i] *= v[i];
    return (*this);
}

template <size_t N, typename T>
EGVecP<N,T>& EGVecP<N,T>::operator*=(T scalar)
{
    for (size_t i = 0; i < N; i++) vec[i] *= scalar;
    return (*this);
}

template <size_t N, typename T>
EGVecT<N,T> EGVecP<N,T>::operator/(const EGVecP<N,T> &o)
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] / o.vec[i];
    return n;
}

template <size_t N, typename T>
EGVecT<N,T> EGVecP<N,T>::operator/(const T v[N])
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] / v[i];
    return n;
}

template <size_t N, typename T>
EGVecT<N,T> EGVecP<N,T>::operator/(T scalar)
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] / scalar;
    return n;
}

template <size_t N, typename T>
EGVecP<N,T>& EGVecP<N,T>::operator/=(const EGVecP<N,T> &o)
{
    for (size_t i = 0; i < N; i++) vec[i] /= o.vec[i];
    return (*this);
}

template <size_t N, typename T>
EGVecP<N,T>& EGVecP<N,T>::operator/=(const T v[N])
{
    for (size_t i = 0; i < N; i++) vec[i] /= v[i];
    return (*this);
}

template <size_t N, typename T>
EGVecP<N,T>& EGVecP<N,T>::operator/=(T scalar)
{
    for (size_t i = 0; i < N; i++) vec[i] /= scalar;
    return (*this);
}

template <size_t N, typename T>
bool EGVecP<N,T>::operator==(const EGVecT<N,T> &o)
{
    for (size_t i = 0; i < N; i++) if (vec[i] != o.vec[i]) return false;
    return true;
}

template <size_t N, typename T>
bool EGVecP<N,T>::operator!=(const EGVecT<N,T> &o)
{
    return !(*this == o);
}

template <size_t N, typename T>
bool EGVecP<N,T>::operator<(const EGVecT<N,T> &o)
{
    for (size_t i = 0; i < N; i++) if (vec[i] < o.vec[i]) return true;
    return false;
}

template <size_t N, typename T>
bool EGVecP<N,T>::operator>(const EGVecT<N,T> &o)
{
    for (size_t i = 0; i < N; i++) if (vec[i] > o.vec[i]) return true;
    return false;
}

template <size_t N, typename T>
bool EGVecP<N,T>::operator<=(const EGVecT<N,T> &o)
{
    return !(*this > o);
}

template <size_t N, typename T>
bool EGVecP<N,T>::operator>=(const EGVecT<N,T> &o)
{
    return !(*this < o);
}


/* EGVecT member definitions */

template <size_t N, typename T>
EGVecT<N,T>::EGVecT() : vec()
{
    for (size_t i = 0; i < N; i++) vec[i] = 0;
}

template <>
inline EGVecT<1,EGfloat>::EGVecT()
{
    vec[0] = 0;
}

template <>
inline EGVecT<2,EGfloat>::EGVecT()
{
    vec[0] = vec[1] = 0;
}

template <>
inline EGVecT<3,EGfloat>::EGVecT()
{
    vec[0] = vec[1] = vec[2] = 0;
}

template <>
inline EGVecT<4,EGfloat>::EGVecT()
{
    vec[0] = vec[1] = vec[2] = vec[3] = 0;
}

template <size_t N, typename T>
EGVecT<N,T>::EGVecT(T x)
{
    for (size_t i = 0; i < N; i++) vec[i] = x;
}

template <>
inline EGVecT<1,EGfloat>::EGVecT(EGfloat x)
{
    vec[0] = x;
}

template <>
inline EGVecT<2,EGfloat>::EGVecT(EGfloat x)
{
    vec[0] = vec[1] = x;
}

template <>
inline EGVecT<3,EGfloat>::EGVecT(EGfloat x)
{
    vec[0] = vec[1] = vec[2] = x;
}

template <>
inline EGVecT<4,EGfloat>::EGVecT(EGfloat x)
{
    vec[0] = vec[1] = vec[2] = vec[3] = x;
}

template <size_t N, typename T>
EGVecT<N,T>::EGVecT(T x, T y)
{
    if (N >= 1) vec[0] = x;
    if (N >= 2) vec[1] = y;
    for (size_t i = 2; i < N; i++) vec[i] = 0;
}

template <>
inline EGVecT<2,EGfloat>::EGVecT(EGfloat x, EGfloat y)
{
    vec[0] = x;
    vec[1] = y;
}

template <>
inline EGVecT<3,EGfloat>::EGVecT(EGfloat x, EGfloat y)
{
    vec[0] = x;
    vec[1] = y;
    vec[2] = 0;
}

template <>
inline EGVecT<4,EGfloat>::EGVecT(EGfloat x, EGfloat y)
{
    vec[0] = x;
    vec[1] = y;
    vec[2] = 0;
    vec[3] = 0;
}

template <size_t N, typename T>
EGVecT<N,T>::EGVecT(T x, T y, T z)
{
    if (N >= 1) vec[0] = x;
    if (N >= 2) vec[1] = y;
    if (N >= 3) vec[2] = z;
    for (size_t i = 3; i < N; i++) vec[i] = 0;
}

template <>
inline EGVecT<3,EGfloat>::EGVecT(EGfloat x, EGfloat y, EGfloat z)
{
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
}

template <>
inline EGVecT<4,EGfloat>::EGVecT(EGfloat x, EGfloat y, EGfloat z)
{
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
    vec[3] = 0;
}

template <size_t N, typename T>
EGVecT<N,T>::EGVecT(T x, T y, T z, T w)
{
    if (N >= 1) vec[0] = x;
    if (N >= 2) vec[1] = y;
    if (N >= 3) vec[2] = z;
    if (N >= 4) vec[3] = w;
    for (size_t i = 4; i < N; i++) vec[i] = 0;
}

template <>
inline EGVecT<4,EGfloat>::EGVecT(EGfloat x, EGfloat y, EGfloat z, EGfloat w)
{
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
    vec[3] = w;
}

template <size_t N, typename T>
EGVecT<N,T>::EGVecT(const T v[N])
{
    memcpy(vec, v, sizeof(T) * N);
}

template <size_t N, typename T>
EGVecT<N,T>::EGVecT(const EGVecT<N,T> &o)
{
    memcpy(vec, o.vec, sizeof(T) * N);
}

template <size_t N, typename T>
EGVecT<N,T>::EGVecT(const EGVecP<N,T> &o)
{
    memcpy(vec, o.vec, sizeof(T) * N);
}

template <size_t N, typename T>
EGVecT<N,T>& EGVecT<N,T>::operator=(const T v[N])
{
    memcpy(vec, v, sizeof(T) * N);
    return (*this);
}

template <size_t N, typename T>
EGVecT<N,T>& EGVecT<N,T>::operator=(const EGVecT<N,T> &o)
{
    memcpy(vec, o.vec, sizeof(T) * N);
    return (*this);
}

template <size_t N, typename T>
T& EGVecT<N,T>::operator[](const size_t index)
{
    return vec[index];
}

template <size_t N, typename T>
EGVecT<N,T>::operator std::string()
{
    std::stringstream ss;
    ss << "(";
    for (size_t i = 0; i < N; i++) {
        if (i != 0) ss << ",";
        ss << vec[i];
    }
    ss << ")";
    return ss.str();
}

template <size_t N, typename T>
T EGVecT<N,T>::magnitude()
{
    T sumsq = 0;
    for (size_t i = 0; i < N; i++) sumsq += (vec[i] * vec[i]);
    return sqrt(sumsq);
}

template <>
inline EGfloat EGVecT<1,EGfloat>::magnitude()
{
    return vec[0];
}

template <>
inline EGfloat EGVecT<2,EGfloat>::magnitude()
{
    return sqrtf(vec[0] * vec[0] + vec[1] * vec[1]);
}

template <>
inline EGfloat EGVecT<3,EGfloat>::magnitude()
{
    return sqrtf(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
}

template <size_t N, typename T>
void EGVecT<N,T>::normalize()
{
#if NORMALIZE_USES_RECIPROCAL
    T rmag = 1.0f / magnitude();
    for (size_t i = 0; i < N; i++) vec[i] *= rmag;
#else
    T mag = magnitude();
    for (size_t i = 0; i < N; i++) vec[i] /= mag;
#endif
}

template <>
inline void EGVecT<1,EGfloat>::normalize()
{
    vec[0] = 1.0f;
}

template <>
inline void EGVecT<2,EGfloat>::normalize()
{
#if NORMALIZE_USES_RECIPROCAL
    EGfloat rmag = 1.0f / magnitude();
    vec[0] *= rmag;
    vec[1] *= rmag;
#else
    EGfloat mag = magnitude();
    vec[0] /= mag;
    vec[1] /= mag;
#endif
}

template <>
inline void EGVecT<3,EGfloat>::normalize()
{
#if NORMALIZE_USES_RECIPROCAL
    EGfloat rmag = 1.0f / magnitude();
    vec[0] *= rmag;
    vec[1] *= rmag;
    vec[2] *= rmag;
#else
    EGfloat mag = magnitude();
    vec[0] /= mag;
    vec[1] /= mag;
    vec[2] /= mag;
#endif
}

template <size_t N, typename T>
EGVecT<N,T> EGVecT<N,T>::normalized()
{
    EGVecT<N,T> n(vec);
    n.normalize();
    return n;
}

template <size_t N, typename T>
EGVecT<N,T> EGVecT<N,T>::operator+(const EGVecT<N,T> &o)
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] + o.vec[i];
    return n;
}

template <size_t N, typename T>
EGVecT<N,T> EGVecT<N,T>::operator+(const T v[N])
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] + v[i];
    return n;
}

template <size_t N, typename T>
EGVecT<N,T> EGVecT<N,T>::operator+(T scalar)
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] + scalar;
    return n;
}

template <size_t N, typename T>
EGVecT<N,T>& EGVecT<N,T>::operator+=(const EGVecT<N,T> &o)
{
    for (size_t i = 0; i < N; i++) vec[i] += o.vec[i];
    return (*this);
}

template <size_t N, typename T>
EGVecT<N,T>& EGVecT<N,T>::operator+=(const T v[N])
{
    for (size_t i = 0; i < N; i++) vec[i] += v[i];
    return (*this);
}

template <size_t N, typename T>
EGVecT<N,T>& EGVecT<N,T>::operator+=(T scalar)
{
    for (size_t i = 0; i < N; i++) vec[i] += scalar;
    return (*this);
}

template <size_t N, typename T>
EGVecT<N,T> EGVecT<N,T>::operator-(const EGVecT<N,T> &o)
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] - o.vec[i];
    return n;
}

template <size_t N, typename T>
EGVecT<N,T> EGVecT<N,T>::operator-(const T v[N])
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] - v[i];
    return n;
}

template <size_t N, typename T>
EGVecT<N,T> EGVecT<N,T>::operator-(T scalar)
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] - scalar;
    return n;
}

template <size_t N, typename T>
EGVecT<N,T>& EGVecT<N,T>::operator-=(const EGVecT<N,T> &o)
{
    for (size_t i = 0; i < N; i++) vec[i] -= o.vec[i];
    return (*this);
}

template <size_t N, typename T>
EGVecT<N,T>& EGVecT<N,T>::operator-=(const T v[N])
{
    for (size_t i = 0; i < N; i++) vec[i] -= v[i];
    return (*this);
}

template <size_t N, typename T>
EGVecT<N,T>& EGVecT<N,T>::operator-=(T scalar)
{
    for (size_t i = 0; i < N; i++) vec[i] -= scalar;
    return (*this);
}

template <size_t N, typename T>
EGVecT<N,T> EGVecT<N,T>::operator*(const EGVecT<N,T> &o)
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] * o.vec[i];
    return n;
}

template <size_t N, typename T>
EGVecT<N,T> EGVecT<N,T>::operator*(const T v[N])
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] * v[i];
    return n;
}

template <size_t N, typename T>
EGVecT<N,T> EGVecT<N,T>::operator*(T scalar)
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] * scalar;
    return n;
}

template <size_t N, typename T>
EGVecT<N,T>& EGVecT<N,T>::operator*=(const EGVecT<N,T> &o)
{
    for (size_t i = 0; i < N; i++) vec[i] *= o.vec[i];
    return (*this);
}

template <size_t N, typename T>
EGVecT<N,T>& EGVecT<N,T>::operator*=(const T v[N])
{
    for (size_t i = 0; i < N; i++) vec[i] *= v[i];
    return (*this);
}

template <size_t N, typename T>
EGVecT<N,T>& EGVecT<N,T>::operator*=(T scalar)
{
    for (size_t i = 0; i < N; i++) vec[i] *= scalar;
    return (*this);
}

template <size_t N, typename T>
EGVecT<N,T> EGVecT<N,T>::operator/(const EGVecT<N,T> &o)
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] / o.vec[i];
    return n;
}

template <size_t N, typename T>
EGVecT<N,T> EGVecT<N,T>::operator/(const T v[N])
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] / v[i];
    return n;
}

template <size_t N, typename T>
EGVecT<N,T> EGVecT<N,T>::operator/(T scalar)
{
    EGVecT<N,T> n;
    for (size_t i = 0; i < N; i++) n.vec[i] = vec[i] / scalar;
    return n;
}

template <size_t N, typename T>
EGVecT<N,T>& EGVecT<N,T>::operator/=(const EGVecT<N,T> &o)
{
    for (size_t i = 0; i < N; i++) vec[i] /= o.vec[i];
    return (*this);
}

template <size_t N, typename T>
EGVecT<N,T>& EGVecT<N,T>::operator/=(const T v[N])
{
    for (size_t i = 0; i < N; i++) vec[i] /= v[i];
    return (*this);
}

template <size_t N, typename T>
EGVecT<N,T>& EGVecT<N,T>::operator/=(T scalar)
{
    for (size_t i = 0; i < N; i++) vec[i] /= scalar;
    return (*this);
}

template <size_t N, typename T>
bool EGVecT<N,T>::operator==(const EGVecT<N,T> &o)
{
    for (size_t i = 0; i < N; i++) if (vec[i] != o.vec[i]) return false;
    return true;
}

template <>
inline bool EGVecT<1,EGfloat>::operator==(const EGVecT<1,EGfloat> &o)
{
    return vec[0] == o.vec[0];
}

template <>
inline bool EGVecT<2,EGfloat>::operator==(const EGVecT<2,EGfloat> &o)
{
    return vec[0] == o.vec[0] && vec[1] == o.vec[1];
}

template <>
inline bool EGVecT<3,EGfloat>::operator==(const EGVecT<3,EGfloat> &o)
{
    return vec[0] == o.vec[0] && vec[1] == o.vec[1] && vec[2] == o.vec[2];
}

template <>
inline bool EGVecT<4,EGfloat>::operator==(const EGVecT<4,EGfloat> &o)
{
    return vec[0] == o.vec[0] && vec[1] == o.vec[1] && vec[2] == o.vec[2] && vec[3] == o.vec[3];
}

template <size_t N, typename T>
bool EGVecT<N,T>::operator!=(const EGVecT<N,T> &o)
{
    return !(*this == o);
}

template <size_t N, typename T>
bool EGVecT<N,T>::operator<(const EGVecT<N,T> &o)
{
    for (size_t i = 0; i < N; i++) if (vec[i] < o.vec[i]) return true;
    return false;
}

template <>
inline bool EGVecT<1,EGfloat>::operator<(const EGVecT<1,EGfloat> &o)
{
    return vec[0] < o.vec[0];
}

template <>
inline bool EGVecT<2,EGfloat>::operator<(const EGVecT<2,EGfloat> &o)
{
    return vec[0] < o.vec[0] || vec[1] < o.vec[1];
}

template <>
inline bool EGVecT<3,EGfloat>::operator<(const EGVecT<3,EGfloat> &o)
{
    return vec[0] < o.vec[0] || vec[1] < o.vec[1] || vec[2] < o.vec[2];
}

template <>
inline bool EGVecT<4,EGfloat>::operator<(const EGVecT<4,EGfloat> &o)
{
    return vec[0] < o.vec[0] || vec[1] < o.vec[1] || vec[2] < o.vec[2] || vec[3] < o.vec[3];
}

template <size_t N, typename T>
bool EGVecT<N,T>::operator>(const EGVecT<N,T> &o)
{
    for (size_t i = 0; i < N; i++) if (vec[i] > o.vec[i]) return true;
    return false;
}

template <>
inline bool EGVecT<1,EGfloat>::operator>(const EGVecT<1,EGfloat> &o)
{
    return vec[0] > o.vec[0];
}

template <>
inline bool EGVecT<2,EGfloat>::operator>(const EGVecT<2,EGfloat> &o)
{
    return vec[0] > o.vec[0] || vec[1] > o.vec[1];
}

template <>
inline bool EGVecT<3,EGfloat>::operator>(const EGVecT<3,EGfloat> &o)
{
    return vec[0] > o.vec[0] || vec[1] > o.vec[1] || vec[2] > o.vec[2];
}

template <>
inline bool EGVecT<4,EGfloat>::operator>(const EGVecT<4,EGfloat> &o)
{
    return vec[0] > o.vec[0] || vec[1] > o.vec[1] || vec[2] > o.vec[2] || vec[3] > o.vec[3];
}

template <size_t N, typename T>
bool EGVecT<N,T>::operator<=(const EGVecT<N,T> &o)
{
    return !(*this > o);
}

template <size_t N, typename T>
bool EGVecT<N,T>::operator>=(const EGVecT<N,T> &o)
{
    return !(*this < o);
}


/* Predefined vector types */

/* EGbyte */
    
#define EGVec1b EGVecT<1,EGbyte>
#define EGVec2b EGVecT<2,EGbyte>
#define EGVec3b EGVecT<3,EGbyte>
#define EGVec4b EGVecT<4,EGbyte>
#define EGVec1bv EGVecP<1,EGbyte>
#define EGVec2bv EGVecP<2,EGbyte>
#define EGVec3bv EGVecP<3,EGbyte>
#define EGVec4bv EGVecP<4,EGbyte>

/* EGubyte */

#define EGVec1ub EGVecT<1,EGubyte>
#define EGVec2ub EGVecT<2,EGubyte>
#define EGVec3ub EGVecT<3,EGubyte>
#define EGVec4ub EGVecT<4,EGubyte>
#define EGVec1ubv EGVecP<1,EGubyte>
#define EGVec2ubv EGVecP<2,EGubyte>
#define EGVec3ubv EGVecP<3,EGubyte>
#define EGVec4ubv EGVecP<4,EGubyte>

/* EGshort */
    
#define EGVec1s EGVecT<1,EGshort>
#define EGVec2s EGVecT<2,EGshort>
#define EGVec3s EGVecT<3,EGshort>
#define EGVec4s EGVecT<4,EGshort>
#define EGVec1sv EGVecP<1,EGshort>
#define EGVec2sv EGVecP<2,EGshort>
#define EGVec3sv EGVecP<3,EGshort>
#define EGVec4sv EGVecP<4,EGshort>

/* EGushort */
    
#define EGVec1us EGVecT<1,EGushort>
#define EGVec2us EGVecT<2,EGushort>
#define EGVec3us EGVecT<3,EGushort>
#define EGVec4us EGVecT<4,EGushort>
#define EGVec1usv EGVecP<1,EGushort>
#define EGVec2usv EGVecP<2,EGushort>
#define EGVec3usv EGVecP<3,EGushort>
#define EGVec4usv EGVecP<4,EGushort>

/* EGint */
    
#define EGVec1i EGVecT<1,EGint>
#define EGVec2i EGVecT<2,EGint>
#define EGVec3i EGVecT<3,EGint>
#define EGVec4i EGVecT<4,EGint>
#define EGVec1iv EGVecP<1,EGint>
#define EGVec2iv EGVecP<2,EGint>
#define EGVec3iv EGVecP<3,EGint>
#define EGVec4iv EGVecP<4,EGint>

/* EGuint */
    
#define EGVec1ui EGVecT<1,EGuint>
#define EGVec2ui EGVecT<2,EGuint>
#define EGVec3ui EGVecT<3,EGuint>
#define EGVec4ui EGVecT<4,EGuint>
#define EGVec1uiv EGVecP<1,EGuint>
#define EGVec2uiv EGVecP<2,EGuint>
#define EGVec3uiv EGVecP<3,EGuint>
#define EGVec4uiv EGVecP<4,EGuint>

/* EGfloat */
    
#define EGVec1f EGVecT<1,EGfloat>
#define EGVec2f EGVecT<2,EGfloat>
#define EGVec3f EGVecT<3,EGfloat>
#define EGVec4f EGVecT<4,EGfloat>
#define EGVec1fv EGVecP<1,EGfloat>
#define EGVec2fv EGVecP<2,EGfloat>
#define EGVec3fv EGVecP<3,EGfloat>
#define EGVec4fv EGVecP<4,EGfloat>

/* EGdouble */
    
#define EGVec1d EGVecT<1,EGdouble>
#define EGVec2d EGVecT<2,EGdouble>
#define EGVec3d EGVecT<3,EGdouble>
#define EGVec4d EGVecT<4,EGdouble>
#define EGVec1dv EGVecP<1,EGdouble>
#define EGVec2dv EGVecP<2,EGdouble>
#define EGVec3dv EGVecP<3,EGdouble>
#define EGVec4dv EGVecP<4,EGdouble>
