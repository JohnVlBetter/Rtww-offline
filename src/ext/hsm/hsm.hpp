#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#pragma once

#include <assert.h>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <random>

//#define USE_DOUBLE
#ifdef USE_DOUBLE
	typedef double Float;
#else
	typedef float Float;
#endif

#if defined(_MSC_VER)
#pragma warning(disable : 4305)
#pragma warning(disable : 4244)
#pragma warning(disable : 4843)
#pragma warning(disable : 4267)
#pragma warning(disable : 4838)
#endif

namespace hsm {

static constexpr Float Infinity = std::numeric_limits<Float>::infinity();
static constexpr Float Sqrt2    = 1.41421356237309504880;
static constexpr Float Pi       = 3.14159265358979323846;
static constexpr Float InvPi    = 0.31830988618379067154;

template<typename T>
inline T Random() {
	static std::uniform_real_distribution<T> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

inline int RandomInt() {
	return static_cast<int>(Random<Float>());
}

template<typename T>
inline T Random(T min, T max) {
	return min + (max - min) * Random<T>();
}

inline int RandomInt(int min, int max) {
	return static_cast<int>(Random<Float>(min, max));
}

template <typename T, typename S, typename R>
inline T Clamp(T val, S low, R high) {
	if (val < low)
		return low;
	else if (val > high)
		return high;
	else
		return val;
}

template<typename T>
inline bool isNaN(const T t) { return std::isnan(t); }
template<>
inline bool isNaN(const int t) { return false; }
template<typename T>
inline bool isNearZero(const T t) { return fabs(t) < 1e-8; }
template<>
inline bool isNearZero(const int t) { return false; }

inline Float Radians(Float degree) { return (Pi / 180.0f) * degree; }

inline Float Degrees(Float radian) { return (180.0f / Pi) * radian; }

template<typename T> class Vector2;
template<typename T> class Bounds2;
//two-dimensional point
template<typename T>
class Point2 {
public:
	//public methods
	Point2() :x(0), y(0) {}
	Point2(T xx, T yy) :x(xx), y(yy) {}

	template<typename S>
	explicit Point2(const Point2<S>& p) {
		x = (T)p.x;
		y = (T)p.y;
	}

	template<typename S>
	explicit Point2(const Vector2<S>& v) {
		x = (T)v.x;
		y = (T)v.y;
	}

	bool HasNaN() const {
		return isNaN(x) || isNaN(y);
	}

	Point2<T>& operator = (const Point2<T>& p) {
		x = p.x;
		y = p.y;
		return *this;
	}

	bool operator == (const Point2<T>& p) const {
		return x == p.x && y == p.y;
	}

	bool operator != (const Point2<T>& p) const {
		return x != p.x || y != p.y;
	}

	Point2<T> operator -() const {
		return Point2<T>(-x, -y);
	}

	T operator [](int i) const {
		assert(i == 0 || i == 1);
		if (i == 0) return x;
		return y;
	}

	T& operator [](int i) {
		assert(i == 0 || i == 1);
		if (i == 0) return x;
		return y;
	}

	Point2<T> operator + (const Vector2<T>& v) const {
		return Point2<T>(x + v.x, y + v.y);
	}

	Point2<T> operator + (const Point2<T>& p) const {
		return Point2<T>(x + p.x, y + p.y);
	}

	Point2<T>& operator += (const Vector2<T>& v) {
		x += v.x;
		y += v.y;
		return *this;
	}

	Point2<T> operator - (const Vector2<T>& v) const {
		return Point2<T>(x - v.x, y - v.y);
	}

	Point2<T>& operator -= (const Vector2<T>& v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}

	Vector2<T> operator - (const Point2<T>& p) const {
		return Vector2<T>(x - p.x, y - p.y);
	}

	template<typename S>
	Point2<T> operator *(S n) const {
		return Point2<T>(n * x, n * y);
	}

	template<typename S>
	Point2<T>& operator *= (S n) {
		x *= n;
		y *= n;
		return *this;
	}

	template<typename S>
	Point2<T> operator / (S n) const {
		assert(n != 0);
		Float inverse = (Float)1 / n;
		return Point2<T>(x * inverse, y * inverse);
	}

	template<typename S>
	Point2<T>& operator /= (S n) {
		assert(n != 0);
		Float inverse = (Float)1 / n;
		x *= inverse;
		y *= inverse;
		return *this;
	}

	inline Float DistanceSquared(const Point2<T>& p) const {
		return std::pow(x - p.x, 2) + std::pow(y - p.y, 2);
	}
	inline Float Distance(const Point2<T>& p) const { return std::sqrt(DistanceSquared(p)); }

	inline Point2<T> Floor() const { return Point2<T>(std::floor(x), std::floor(y)); }
	inline Point2<T> Ceil() const { return Point2<T>(std::ceil(x), std::ceil(y)); }
	inline Point2<T> Abs() const { return Point2<T>(std::abs(x), std::abs(y)); }

	inline Point2<T> Lerp(const Point2<T>& p, Float t) const { return (1 - t)*(*this) + t * p; }

	//public data
	T x, y;
};

template<typename T>
std::ostream & operator << (std::ostream &o, const Point2<T>& p) {
	o << '[' << p.x << ',' << p.y << ']';
	return o;
}

template<typename T, typename U>
inline Point2<T> operator * (U n, const Point2<T>& v) {
	return v * n;
}

template <typename T>
Point2<T> Lerp(Float t, const Point2<T> &p0, const Point2<T> &p1) {
	return (1 - t) * p0 + t * p1;
}

template <typename T>
bool Inside(const Point2<T> &p, const Bounds2<T> &b) {
	return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y && p.y <= b.pMax.y);
}

template<typename T> class Vector3;
template<typename T> class Bounds3;
//three-dimensional point
template<typename T>
class Point3 {
public:
	//public methods
	Point3() :x(0), y(0), z(0) {}
	Point3(T xx, T yy, T zz) :x(xx), y(yy), z(zz) {}

	template<typename S>
	explicit Point3(const Point3<S>& p) {
		x = (T)p.x;
		y = (T)p.y;
		z = (T)p.z;
	}

	template<typename S>
	explicit Point3(const Vector3<S>& v) {
		x = (T)v.x;
		y = (T)v.y;
		z = (T)v.z;
	}

	bool HasNaN() const {
		return isNaN(x) || isNaN(y) || isNaN(z);
	}

	Point3<T>& operator = (const Point3<T>& p) {
		x = p.x;
		y = p.y;
		z = p.z;
		return *this;
	}

	bool operator == (const Point3<T>& p) const {
		return x == p.x && y == p.y && z == p.z;
	}

	bool operator != (const Point3<T>& p) const {
		return x != p.x || y != p.y || z != p.z;
	}

	Point3<T> operator -() const {
		return Point3<T>(-x, -y, -z);
	}

	T operator [](int i) const {
		assert(i >= 0 && i <= 3);
		if (i == 0) return x;
		else if (i == 1) return y;
		return z;
	}

	T& operator [](int i) {
		assert(i >= 0 && i <= 2);
		if (i == 0) return x;
		else if (i == 1) return y;
		return z;
	}

	Point3<T> operator + (const Vector3<T>& v) const {
		return Point3<T>(x + v.x, y + v.y, z + v.z);
	}

	Point3<T> operator + (const Point3<T>& p) const {
		return Point3<T>(x + p.x, y + p.y, z + p.z);
	}

	Point3<T>& operator += (const Vector3<T>& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Point3<T> operator - (const Vector3<T>& v) const {
		return Point3<T>(x - v.x, y - v.y, z - v.z);
	}

	Point3<T>& operator -= (const Vector3<T>& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector3<T> operator - (const Point3<T>& p) const {
		return Vector3<T>(x - p.x, y - p.y, z - p.z);
	}

	template<typename S>
	Point3<T> operator *(S n) const {
		return Point3<T>(n * x, n * y, n * z);
	}

	template<typename S>
	Point3<T>& operator *= (S n) {
		x *= n;
		y *= n;
		z *= n;
		return *this;
	}

	template<typename S>
	Point3<T> operator / (S n) const {
		assert(n != 0);
		Float inverse = (Float)1 / n;
		return Point3<T>(x * inverse, y * inverse, z * inverse);
	}

	template<typename S>
	Point3<T>& operator /= (S n) {
		assert(n != 0);
		Float inverse = (Float)1 / n;
		x *= inverse;
		y *= inverse;
		z *= inverse;
		return *this;
	}

	inline Float DistanceSquared(const Point3<T>& p) const {
		return std::pow(x - p.x, 2) + std::pow(y - p.y, 2) + std::pow(z - p.z, 2);
	}
	inline Float Distance(const Point3<T>& p) const { return std::sqrt(DistanceSquared(p)); }

	inline Point3<T> Floor() const { return Point3<T>(std::floor(x), std::floor(y), std::floor(z)); }
	inline Point3<T> Ceil() const { return Point3<T>(std::ceil(x), std::ceil(y), std::ceil(z)); }
	inline Point3<T> Abs() const { return Point3<T>(std::abs(x), std::abs(y), std::abs(z)); }

	inline Point3<T> Lerp(const Point3<T>& p, Float t) const { return (1 - t)*(*this) + t * p; }

	//public data
	T x, y, z;
};

template<typename T>
std::ostream & operator << (std::ostream &o, const Point3<T>& p) {
	o << '[' << p.x << ',' << p.y << ',' << p.z << ']';
	return o;
}

template<typename T, typename U>
inline Point3<T> operator * (U n, const Point3<T>& v) {
	return v * n;
}

template <typename T>
Point3<T> Lerp(Float t, const Point3<T> &p0, const Point3<T> &p1) {
	return (1 - t) * p0 + t * p1;
}

template <typename T>
bool Inside(const Point3<T> &p, const Bounds3<T> &b) {
	return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y && 
		    p.y <= b.pMax.y && p.z >= b.pMin.z && p.z <= b.pMax.z);
}

//two-dimensional vector
template<typename T>
class Vector2 {
public:
	//public methods
	Vector2() :x(0), y(0) {}
	Vector2(T xx, T yy) :x(xx), y(yy) {}

	template<typename S>
	explicit Vector2(const Point2<T>& p) {
		x = (T)p.x;
		y = (T)p.y;
	}

	template<typename S>
	explicit Vector2(const Vector2<T>& v) {
		x = (T)v.x;
		y = (T)v.y;
	}

	bool HasNaN() const { return isNaN(x) || isNaN(y); }

	Vector2<T>& operator = (const Vector2<T>& v) {
		x = v.x;
		y = v.y;
		return *this;
	}

	bool operator == (const Vector2<T>& v) const {
		return x == v.x && y == v.y;
	}

	bool operator != (const Vector2<T>& v) const {
		return x != v.x || y != v.y;
	}

	Vector2<T> operator -() const {
		return Vector2<T>(-x, -y);
	}

	T operator [](int i) const {
		assert(i >= 0 && i <= 1);
		if (i == 0) return x;
		return y;
	}

	T& operator [](int i) {
		assert(i >= 0 && i <= 1);
		if (i == 0) return x;
		return y;
	}

	Vector2<T> operator + (const Vector2<T>& v) const {
		return Point3<T>(x + v.x, y + v.y);
	}

	Vector2<T>& operator += (const Vector2<T>& v) {
		x += v.x;
		y += v.y;
		return *this;
	}

	Vector2<T> operator - (const Vector2<T>& v) const {
		return Point3<T>(x - v.x, y - v.y);
	}

	Vector2<T>& operator -= (const Vector2<T>& v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}

	template<typename S>
	Vector2<T> operator *(S n) const {
		return Vector2<T>(n * x, n * y);
	}

	template<typename S>
	Vector2<T>& operator *= (S n) {
		x *= n;
		y *= n;
		return *this;
	}

	template<typename S>
	Vector2<T> operator / (S n) const {
		assert(n != 0);
		Float inverse = (Float)1 / n;
		return Vector2<T>(x * inverse, y * inverse);
	}

	template<typename S>
	Vector2<T>& operator /= (S n) {
		assert(n != 0);
		Float inverse = (Float)1 / n;
		x *= inverse;
		y *= inverse;
		return *this;
	}

	inline Float LengthSquared()const { return x * x + y * y; }
	inline Float Length()const { return std::sqrt(LengthSquared()); }

	inline Vector2<T> Abs() const { return Vector2<T>(std::abs(x), std::abs(y)); }
	inline Vector2<T> Normalize() const { return *this / Length(); }

	//public data
	T x, y;
};

template<typename T>
inline std::ostream& operator << (std::ostream& o, const Vector2<T>& v) {
	o << '[' << v.x << ',' << v.y << ']';
	return o;
}

template<typename T, typename U>
inline Vector2<T> operator * (U n, const Vector2<T>& v) {
	return v * n;
}

template<typename T>
inline Float Dot(const Vector2<T>& v1, const Vector2<T>& v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

//three-dimensional vector
template<typename T>
class Vector3 {
public:
	//public methods
	Vector3() :x(0), y(0), z(0) {}
	Vector3(T xx, T yy, T zz) :x(xx), y(yy), z(zz) {} 

	template<typename S>
	explicit Vector3(const Point3<T>& p) {
		x = (T)p.x;
		y = (T)p.y;
		z = (T)p.z;
	}

	template<typename S>
	explicit Vector3(const Vector3<T>& v) {
		x = (T)v.x;
		y = (T)v.y;
		z = (T)v.z;
	}

	bool HasNaN() const { return isNaN(x) || isNaN(y) || isNaN(z); }

	Vector3<T>& operator = (const Vector3<T>& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	bool operator == (const Vector3<T>& v) const {
		return x == v.x && y == v.y && z == v.z;
	}

	bool operator != (const Vector3<T>& v) const {
		return x != v.x || y != v.y || z != v.z;
	}

	Vector3<T> operator -() const {
		return Vector3<T>(-x, -y, -z);
	}

	T operator [](int i) const {
		assert(i >= 0 && i <= 2);
		if (i == 0) return x;
		if (i == 1) return y;
		return z;
	}

	T& operator [](int i) {
		assert(i >= 0 && i <= 2);
		if (i == 0) return x;
		if (i == 1) return y;
		return z;
	}

	Vector3<T> operator + (const Vector3<T>& v) const {
		return Vector3<T>(x + v.x, y + v.y, z + v.z);
	}

	Vector3<T>& operator += (const Vector3<T>& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3<T> operator - (const Vector3<T>& v) const {
		return Vector3<T>(x - v.x, y - v.y, z - v.z);
	}

	Vector3<T>& operator -= (const Vector3<T>& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	template<typename S>
	Vector3<T> operator *(S n) const {
		return Vector3<T>(n * x, n * y, n * z);
	}

	template<typename S>
	Vector3<T>& operator *= (S n) {
		x *= n;
		y *= n;
		z *= n;
		return *this;
	}

	template<typename S>
	Vector3<T> operator / (S n) const {
		assert(n != 0);
		Float inverse = (Float)1 / n;
		return Vector3<T>(x * inverse, y * inverse, z * inverse);
	}

	template<typename S>
	Vector3<T>& operator /= (S n) {
		assert(n != 0);
		Float inverse = (Float)1 / n;
		x *= inverse;
		y *= inverse;
		z *= inverse;
		return *this;
	}

	inline bool IsNearZero() const { return isNearZero<T>(x) && isNearZero<T>(y) && isNearZero<T>(z); }
	inline Float LengthSquared()const { return x * x + y * y + z * z; }
	inline Float Length()const { return std::sqrt(LengthSquared()); }

	inline Vector3<T> Abs() const { return Vector3<T>(std::abs(x), std::abs(y), std::abs(z)); }
	inline Vector3<T> Normalize() const { return *this / Length(); }
	inline Vector3<T> Reflect(const Vector3<T>& normal) { return *this - 2 * Dot(*this, normal)*normal; }

	Vector3<T> Refract(const Vector3<T>& normal, Float etaiOverEtat) {
		Float cosTheta = fmin(Dot(-*this, normal), 1.0f);
		//R perpendicular
		Vector3<T> rOutPerp = etaiOverEtat * (*this + cosTheta * normal);
		//R parallel
		Vector3<T> rOutParallel = -sqrt(fabs(1.0f - rOutPerp.LengthSquared())) * normal;
		return rOutPerp + rOutParallel;
	}

	//public data
	T x, y, z;
};

template<typename T>
inline std::ostream& operator << (std::ostream& o, const Vector3<T>& v) {
	o << '[' << v.x << ',' << v.y << ',' << v.z << ']';
	return o;
}

template<typename T, typename U>
inline Vector3<T> operator * (U n, const Vector3<T>& v) {
	return v * n;
}

template<typename T>
inline Float Dot(const Vector3<T>& v1, const Vector3<T>& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template<typename T>
inline Vector3<T> Cross(const Vector3<T>& v1, const Vector3<T>& v2) {
	return Vector3<T>((v1.y * v2.z) - (v1.z * v2.y), 
		(v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x));
}

typedef Vector3<Float> Vector3f;
typedef Vector3<int> Vector3i;
typedef Vector2<Float> Vector2f;
typedef Vector2<int> Vector2i;
typedef Point2<Float> Point2f;
typedef Point2<int> Point2i;
typedef Point3<Float> Point3f;
typedef Point3<int> Point3i;

inline Vector3f Convert(const Point3f& p) {
	return Vector3f(p.x, p.y, p.z);
}

Vector3f RandomInUnitSphere() {
	while (true) {
		Vector3f v(Random<Float>(-1.0f, 1.0f), Random<Float>(-1.0f, 1.0f), Random<Float>(-1.0f, 1.0f));
		if (v.LengthSquared() >= 1.0f) continue;
		return v;
	}
}

Vector2f RandomInUnitDisk() {
	while (true) {
		Vector2f v(Random<Float>(-1.0f, 1.0f), Random<Float>(-1.0f, 1.0f));
		if (v.LengthSquared() >= 1.0f) continue;
		return v;
	}
}

Vector3f RandomInHemisphere(const Vector3f& normal) {
	Vector3f v = RandomInUnitSphere();
	if (Dot(v, normal) > 0.0f) return v;
	return -v;
}

inline Vector3f RandomUnitVec() {
	return RandomInUnitSphere().Normalize();
}

inline Vector3f RandomVec(Float min, Float max) {
	return Vector3f(Random<Float>(min, max), Random<Float>(min, max), Random<Float>(min, max));
}

inline Vector3f RandomVec() {
	return Vector3f(Random<Float>(), Random<Float>(), Random<Float>());
}

inline Point3f RandomPoint(Float min, Float max) {
	return Point3f(Random<Float>(min, max), Random<Float>(min, max), Random<Float>(min, max));
}

inline Point3f RandomPoint() {
	return Point3f(Random<Float>(), Random<Float>(), Random<Float>());
}

inline Vector3f RandomCosineDirection() {
	auto r1 = Random<Float>();
	auto r2 = Random<Float>();
	auto z = sqrt(1 - r2);
	auto phi = 2 * Pi*r1;
	auto x = cos(phi)*sqrt(r2);
	auto y = sin(phi)*sqrt(r2);

	return Vector3f(x, y, z);
}

inline Vector3f Random2Sphere(double radius, double distanceSquared) {
	auto r1 = Random<Float>();
	auto r2 = Random<Float>();
	auto z = 1 + r2 * (sqrt(1 - radius * radius / distanceSquared) - 1);

	auto phi = 2 * Pi*r1;
	auto x = cos(phi)*sqrt(1 - z * z);
	auto y = sin(phi)*sqrt(1 - z * z);

	return Vector3f(x, y, z);
}

typedef Vector3<Float> Color;

inline Color operator * (const Color& c1, const Color& c2) {
	return Color(c1.x * c2.x, c1.y * c2.y, c1.z * c2.z);
}

template <typename T>
int MaxDimension(const Vector3<T> &v) {
	return (v.x > v.y) ? ((v.x > v.z) ? 0 : 2) : ((v.y > v.z) ? 1 : 2);
}

template <typename T>
Vector3<T> Permute(const Vector3<T> &v, int x, int y, int z) {
	return Vector3<T>(v[x], v[y], v[z]);
}

//Bounds2
template <typename T>
class Bounds2 {
public:
	//public methods
	Bounds2() {
		T minNum = std::numeric_limits<T>::lowest();
		T maxNum = std::numeric_limits<T>::max();
		pMin = Point2<T>(minNum, minNum);
		pMax = Point2<T>(maxNum, maxNum);
	}

	Bounds2(const Point2<T> &p1, const Point2<T> &p2) :
		pMax(std::max(p1.x, p2.x), std::max(p1.y, p2.y)),
		pMin(std::min(p1.x, p2.x), std::min(p1.y, p2.y)) {}

	inline Vector2<T> Diagonal() const { return pMax - pMin; }

	inline T Area() const { return (pMax.x - pMin.x) * (pMax.y - pMin.y); }

	Point2<T>& operator [] (int i) {
		assert(i == 0 || i == 1);
		if (i == 0) return pMin;
		return pMax;
	}

	const Point2<T>& operator [] (int i) const {
		assert(i == 0 || i == 1);
		if (i == 0) return pMin;
		return pMax;
	}

	bool operator == (const Bounds2<T>& b) const {
		return pMin == b.pMin && pMax == b.pMax;
	}

	bool operator != (const Bounds2<T>& b) const {
		return pMin != b.pMin || pMax != b.pMax;
	}

	void BoundingSphere(Point2<T>& center, T& radius) const {
		Point2<T> c = (pMin + pMax) * 0.5f;
		center.x = c.x;
		center.y = c.y;
		radius = Inside(center, *this) ? center.Distance(pMax) : 0;
	}

	//public data
	Point2<T> pMin, pMax;
};

template <typename T>
inline std::ostream& operator << (std::ostream& o, const Bounds2<T>& b) {
	o << "[ " << b.pMin << " , " << b.pMax << " ]";
	return o;
}

//Bounds3
template <typename T>
class Bounds3 {
public:
	//public methods
	Bounds3() {
		T minNum = std::numeric_limits<T>::lowest();
		T maxNum = std::numeric_limits<T>::max();
		pMin = Point3<T>(minNum, minNum, minNum);
		pMax = Point3<T>(maxNum, maxNum, maxNum);
	}

	Bounds3(const Point3<T> &p1, const Point3<T> &p2) :
		pMax(std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z)),
		pMin(std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z)) {}

	inline Vector3<T> Diagonal() const { return pMax - pMin; }

	inline T SurfaceArea() const {
		Vector3<T> v = Diagonal();
		return 2 * (v.x * v.y + v.y * v.z + v.x * v.z);
	}

	inline T Volume() const {
		Vector3<T> v = Diagonal();
		return v.x * v.y * v.z;
	}

	Point3<T>& operator [] (int i) {
		assert(i == 0 || i == 1);
		if (i == 0) return pMin;
		return pMax;
	}

	const Point3<T>& operator [] (int i) const {
		assert(i == 0 || i == 1);
		if (i == 0) return pMin;
		return pMax;
	}

	bool operator == (const Bounds3<T>& b) const {
		return pMin == b.pMin && pMax == b.pMax;
	}

	bool operator != (const Bounds3<T>& b) const {
		return pMin != b.pMin || pMax != b.pMax;
	}

	void BoundingSphere(Point3<T>& center, T& radius) const {
		Point3<T> c = (pMin + pMax) * 0.5f;
		center.x = c.x;
		center.y = c.y;
		center.z = c.z;
		radius = Inside(center, *this) ? center.Distance(pMax) : 0;
	}

	//public data
	Point3<T> pMin, pMax;
};

template <typename T>
inline std::ostream& operator << (std::ostream& o, const Bounds3<T>& b) {
	o << "[ " << b.pMin << " , " << b.pMax << " ]";
	return o;
}

typedef Bounds2<int> Bounds2i;
typedef Bounds2<Float> Bounds2f;
typedef Bounds3<int> Bounds3i;
typedef Bounds3<Float> Bounds3f;

//Ray
class Ray {
public:
	//public methods
	Ray() {}
	Ray(const Point3f& o, const Vector3f& d, Float t = 0.0f) : origin(o), direction(d), time(t) {}

	Point3f At(Float t) const { return origin + t * direction; }

	bool HasNaN() const {
		return (origin.HasNaN() || direction.HasNaN());
	}

	//public data
	Point3f origin;
	Vector3f direction;
	Float time;
};

inline std::ostream &operator<<(std::ostream &o, const Ray &r) {
	o << "[ origin:" << r.origin << ", direction:" << r.direction << ", time:" << r.time << "]";
	return o;
}

//Matrix 4x4
class Matrix4x4 {
public:
	//public methods
	Matrix4x4() {
		data[0][0] = data[1][1] = data[2][2] = data[3][3] = 1.0f;
		data[0][1] = data[0][2] = data[0][3] = data[1][0] =
		data[1][2] = data[1][3] = data[2][0] = data[2][1] =
		data[2][3] = data[3][1] = data[3][2] = data[3][0] = 0.0f;
	}

	Matrix4x4(Float d00, Float d01, Float d02, Float d03, Float d10, Float d11, Float d12, Float d13,
		Float d20, Float d21, Float d22, Float d23, Float d30, Float d31, Float d32, Float d33) {
		data[0][0] = d00;
		data[0][1] = d01;
		data[0][2] = d02;
		data[0][3] = d03;
		data[1][0] = d10;
		data[1][1] = d11;
		data[1][2] = d12;
		data[1][3] = d13;
		data[2][0] = d20;
		data[2][1] = d21;
		data[2][2] = d22;
		data[2][3] = d23;
		data[3][0] = d30;
		data[3][1] = d31;
		data[3][2] = d32;
		data[3][3] = d33;
	}

	Matrix4x4(Float mat[4][4]) {
		memcpy(data, mat, sizeof(Float) * 16);
	}

	bool operator == (const Matrix4x4& mat) const {
		for (int i = 0; i <= 3; ++i)
			for (int j = 0; j <= 3; ++j)
				if (std::abs(data[i][j] - mat.data[i][j]) > 1e-6)
					return false;
		return true;
	}

	bool operator != (const Matrix4x4& mat) const {
		for (int i = 0; i <= 3; ++i)
			for (int j = 0; j <= 3; ++j)
				if (std::abs(data[i][j] - mat.data[i][j]) > 1e-6) 
					return true;
		return false;
	}

	Matrix4x4 operator * (const Matrix4x4& mat) const {
		Matrix4x4 result;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				result.data[i][j] = data[i][0] * mat.data[0][j] + data[i][1] * mat.data[1][j] +
					                data[i][2] * mat.data[2][j] + data[i][3] * mat.data[3][j];
		return result;
	}

	bool HasScale() const {
#define NOT_ONE(x) ((x) < .999f || (x) > 1.001f)
		return (NOT_ONE(data[0][0]) || NOT_ONE(data[1][1]) || NOT_ONE(data[2][2]));
#undef NOT_ONE
	}

	Matrix4x4 Transpose() const {
		return Matrix4x4(data[0][0], data[1][0], data[2][0], data[3][0],
			             data[0][1], data[1][1], data[2][1], data[3][1],
			             data[0][2], data[1][2], data[2][2], data[3][2],
			             data[0][3], data[1][3], data[2][3], data[3][3]);
	}

	Matrix4x4 Inverse() const {
		int indexC[4], indexR[4];
		int ipiv[4] = { 0, 0, 0, 0 };
		Float inv[4][4];
		memcpy(inv, data, 4 * 4 * sizeof(Float));
		for (int i = 0; i < 4; i++) {
			int irow = 0, icol = 0;
			Float big = 0.f;
			for (int j = 0; j < 4; j++) {
				if (ipiv[j] != 1) {
					for (int k = 0; k < 4; k++) {
						if (ipiv[k] == 0) {
							if (std::abs(inv[j][k]) >= big) {
								big = Float(std::abs(inv[j][k]));
								irow = j;
								icol = k;
							}
						}
						else if (ipiv[k] > 1)
							std::cout << "Singular matrix!" << std::endl;
					}
				}
			}
			++ipiv[icol];
			if (irow != icol) {
				for (int k = 0; k < 4; ++k) std::swap(inv[irow][k], inv[icol][k]);
			}
			indexR[i] = irow;
			indexC[i] = icol;
			if (inv[icol][icol] == 0.f) std::cout << "Singular matrix!" << std::endl;

			Float pivinv = 1. / inv[icol][icol];
			inv[icol][icol] = 1.;
			for (int j = 0; j < 4; j++) inv[icol][j] *= pivinv;

			for (int j = 0; j < 4; j++) {
				if (j != icol) {
					Float save = inv[j][icol];
					inv[j][icol] = 0;
					for (int k = 0; k < 4; k++) inv[j][k] -= inv[icol][k] * save;
				}
			}
		}

		for (int j = 3; j >= 0; j--) {
			if (indexR[j] != indexC[j]) {
				for (int k = 0; k < 4; k++)
					std::swap(inv[k][indexR[j]], inv[k][indexC[j]]);
			}
		}
		return Matrix4x4(inv);
	}

	bool SwapsHandedness() const {
		Float det = data[0][0] * (data[1][1] * data[2][2] - data[1][2] * data[2][1]) -
					data[0][1] * (data[1][0] * data[2][2] - data[1][2] * data[2][0]) +
					data[0][2] * (data[1][0] * data[2][1] - data[1][1] * data[2][0]);
		return det < 0;
	}

	bool IsIdentity() const {
		return (data[0][0] == 1.f && data[0][1] == 0.f && data[0][2] == 0.f && data[0][3] == 0.f && 
				data[1][0] == 0.f && data[1][1] == 1.f && data[1][2] == 0.f && data[1][3] == 0.f && 
				data[2][0] == 0.f && data[2][1] == 0.f && data[2][2] == 1.f && data[2][3] == 0.f &&
				data[3][0] == 0.f && data[3][1] == 0.f && data[3][2] == 0.f && data[3][3] == 1.f);
	}

	template <typename T>
	inline Point3<T> operator()(const Point3<T> &p) const;
	template <typename T>
	inline Vector3<T> operator()(const Vector3<T> &v) const;
	inline Ray operator()(const Ray &r) const;

	//public data
	Float data[4][4];
};

inline std::ostream& operator << (std::ostream& o, const Matrix4x4& mat) {
	o << "[ " << mat.data[0][0] << " , " << mat.data[0][1] << " , " << mat.data[0][2] << " , " << mat.data[0][3] << " ]\n" <<
		 "[ " << mat.data[1][0] << " , " << mat.data[1][1] << " , " << mat.data[1][2] << " , " << mat.data[1][3] << " ]\n" <<
		 "[ " << mat.data[2][0] << " , " << mat.data[2][1] << " , " << mat.data[2][2] << " , " << mat.data[2][3] << " ]\n" <<
		 "[ " << mat.data[3][0] << " , " << mat.data[3][1] << " , " << mat.data[3][2] << " , " << mat.data[3][3] << " ]"   <<
		 std::endl;
	return o;
}

Matrix4x4 Translate(const Vector3f delta) {
	return Matrix4x4(1.0f, 0.0f, 0.0f, delta.x, 0.0f, 1.0f, 0.0f, delta.y,
		             0.0f, 0.0f, 1.0f, delta.z, 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 RotateX(Float degree) {
	Float sinTheta = std::sin(Radians(degree));
	Float cosTheta = std::cos(Radians(degree));
	return Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cosTheta, -sinTheta, 0.0f,
		             0.0f, sinTheta, cosTheta, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 RotateY(Float degree) {
	Float sinTheta = std::sin(Radians(degree));
	Float cosTheta = std::cos(Radians(degree));
	return Matrix4x4(cosTheta, 0.0f, sinTheta, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		             -sinTheta, 0.0f, cosTheta, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 RotateZ(Float degree) {
	Float sinTheta = std::sin(Radians(degree));
	Float cosTheta = std::cos(Radians(degree));
	return Matrix4x4(cosTheta, -sinTheta, 0.0f, 0.0f, sinTheta, cosTheta, 0.0f, 0.0f,
		             0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 Rotate(const Vector3f& axis, Float degree) {
	Vector3f normalizedAxis = axis.Normalize();
	Float sinTheta = std::sin(Radians(degree));
	Float cosTheta = std::cos(Radians(degree));
	return Matrix4x4(normalizedAxis.x * normalizedAxis.x + (1 - normalizedAxis.x * normalizedAxis.x) * cosTheta,
		normalizedAxis.x * normalizedAxis.y * (1 - cosTheta) - normalizedAxis.z * sinTheta,
		normalizedAxis.x * normalizedAxis.z * (1 - cosTheta) + normalizedAxis.y * sinTheta,
		0,
		normalizedAxis.x * normalizedAxis.y * (1 - cosTheta) + normalizedAxis.z * sinTheta,
		normalizedAxis.y * normalizedAxis.y + (1 - normalizedAxis.y * normalizedAxis.y) * cosTheta,
		normalizedAxis.y * normalizedAxis.z * (1 - cosTheta) - normalizedAxis.x * sinTheta,
		0,
		normalizedAxis.x * normalizedAxis.z * (1 - cosTheta) - normalizedAxis.y * sinTheta,
		normalizedAxis.y * normalizedAxis.z * (1 - cosTheta) + normalizedAxis.x * sinTheta,
		normalizedAxis.z * normalizedAxis.z + (1 - normalizedAxis.z * normalizedAxis.z) * cosTheta,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 Scale(const Vector3f& scale) {
	return Matrix4x4(scale.x, 0.0f, 0.0f, 0.0f, 0.0f, scale.y, 0.0f, 0.0f,
		             0.0f, 0.0f, scale.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 GetViewMatrix(const Point3f& pos, const Point3f& target, const Vector3f& viewUp) {
	Vector3f forward = (target - pos).Normalize();
	if (Cross(viewUp.Normalize(), forward).Length() == 0) {
		std::cout << "hsm error : The up direction and the view direction is the same direction!" << std::endl;
		return Matrix4x4();
	}
	Vector3f right = Cross(viewUp.Normalize(), forward);
	Vector3f up = Cross(forward, right);
	Matrix4x4 rotateMat(right.x, right.y, right.z, 0.0f, up.x, up.y, up.z, 0.0f,
		                forward.x, forward.y, forward.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	Matrix4x4 translateMat(1.0f, 0.0f, 0.0f, -pos.x, 0.0f, 1.0f, 0.0f, -pos.y,
		                   0.0f, 0.0f, 1.0f, -pos.z, 0.0f, 0.0f, 0.0f, 1.0f);
	return rotateMat * translateMat;
}

Matrix4x4 GetPerspectiveMatrix(Float aspect, Float fov, Float near, Float far) {
	Float tanHalfFov = std::tan(Radians(fov) * 0.5f);
	return Matrix4x4(1 / (aspect * tanHalfFov), 0.0f, 0.0f, 0.0f, 0.0f, 1 / tanHalfFov, 0.0f, 0.0f,
		             0.0f, 0.0f, (near + far) / (far - near), 1.0f, 0.0f, 0.0f, (2 * far * near)/(near - far), 0.0f);
}

template <typename T>
inline Point3<T> Matrix4x4::operator()(const Point3<T> &p) const {
	T x = p.x, y = p.y, z = p.z;
	T px = data[0][0] * x + data[0][1] * y + data[0][2] * z + data[0][3];
	T py = data[1][0] * x + data[1][1] * y + data[1][2] * z + data[1][3];
	T pz = data[2][0] * x + data[2][1] * y + data[2][2] * z + data[2][3];
	T pw = data[3][0] * x + data[3][1] * y + data[3][2] * z + data[3][3];
	assert(pw != 0);
	if (pw == 1) return Point3<T>(px, py, pz);
	else return Point3<T>(px, py, pz) / pw;
}

template <typename T>
inline Vector3<T> Matrix4x4::operator()(const Vector3<T> &v) const {
	T x = v.x, y = v.y, z = v.z;
	return Vector3<T>(data[0][0] * x + data[0][1] * y + data[0][2] * z,
					  data[1][0] * x + data[1][1] * y + data[1][2] * z,
					  data[2][0] * x + data[2][1] * y + data[2][2] * z);
}

inline Ray Matrix4x4::operator()(const Ray &r) const {
	Point3f o = (*this)(r.origin);
	Vector3f d = (*this)(r.direction);
	return Ray(o, d, r.time);
}

//Quaternion
class Quaternion {
public:
	//public methods
	Quaternion():w(1), x(0), y(0), z(0) {}
	Quaternion(Float ww, Float xx, Float yy, Float zz):w(ww), x(xx), y(yy), z(zz) {}
	Quaternion(const Matrix4x4& rotMat) {
		//trace = 4w^2 - 1 = mat[0][0] + mat[1][1] + mat[2][2]
		Float trace = rotMat.data[0][0] + rotMat.data[1][1] + rotMat.data[2][2];
		if (trace > 0.0f) {
			Float sqrtT = std::sqrt(trace + 1.0f);
			w = sqrtT * 0.5f;
			sqrtT = 0.5f / sqrtT;
			x = (rotMat.data[2][1] - rotMat.data[1][2]) * sqrtT;
			y = (rotMat.data[0][2] - rotMat.data[2][0]) * sqrtT;
			z = (rotMat.data[1][0] - rotMat.data[0][1]) * sqrtT;
		}
		else {
			const int indices[3] = { 1, 2 ,0 };
			Float QuaternionXYZ[3];
			int i = 0;
			if (rotMat.data[1][1] > rotMat.data[0][0]) i = 1;
			if (rotMat.data[2][2] > rotMat.data[i][i]) i = 2;
			int j = indices[i];
			int k = indices[j];
			Float sqrtT = std::sqrt((rotMat.data[i][i] - (rotMat.data[j][j] + rotMat.data[k][k])) + 1.0f);
			QuaternionXYZ[i] = sqrtT * 0.5f;
			if (sqrtT != 0.f) sqrtT = 0.5f / sqrtT;
			w = (rotMat.data[k][j] - rotMat.data[j][k]) * sqrtT;
			QuaternionXYZ[j] = (rotMat.data[j][i] + rotMat.data[i][j]) * sqrtT;
			QuaternionXYZ[k] = (rotMat.data[k][i] + rotMat.data[i][k]) * sqrtT;
			x = QuaternionXYZ[0];
			y = QuaternionXYZ[1];
			z = QuaternionXYZ[2];
		}
	}

	Matrix4x4 ToMatrix4x4() const {
		Float xx = x * x, yy = y * y, zz = z * z ,xy = x * y, xz = x * z, 
			  yz = y * z ,wx = x * w, wy = y * w, wz = z * w;

		//We use the left-hand coordinate system, so we need to return the inverse matrix of this matrix.
		//And the transpose matrix of the rotation matrix is ​​equal to its inverse matrix.
		return Matrix4x4(1 - 2 * (yy + zz), 2 * (xy + wz), 2 * (xz - wy), 0.0f,
						 2 * (xy - wz), 1 - 2 * (xx + zz), 2 * (yz + wx), 0.0f,
						 2 * (xz + wy), 2 * (yz - wx), 1 - 2 * (xx + yy), 0.0f,
						 0.0f, 0.0f, 0.0f, 1.0f).Transpose();
	}

	Quaternion operator - () const {
		return Quaternion(-w, -x, -y, -z);
	}

	Quaternion operator + (const Quaternion& q) const {
		return Quaternion(w + q.w, x + q.x, y + q.y, z + q.z);
	}

	Quaternion& operator += (const Quaternion& q) {
		w += q.w;
		x += q.x;
		y += q.y;
		z += q.z;
		return *this;
	}

	Quaternion operator - (const Quaternion& q) const {
		return Quaternion(w - q.w, x - q.x, y - q.y, z - q.z);
	}

	Quaternion& operator -= (const Quaternion& q) {
		w -= q.w;
		x -= q.x;
		y -= q.y;
		z -= q.z;
		return *this;
	}

	Quaternion &operator*=(Float n) {
		w *= n;
		x *= n;
		y *= n;
		z *= n;
		return *this;
	}
	Quaternion operator*(Float n) const {
		return Quaternion(w * n, x * n, y * n, z * n);
	}
	Quaternion &operator/=(Float n) {
		w *= n;
		x *= n;
		y *= n;
		z *= n;
		return *this;
	}
	Quaternion operator/(Float n) const {
		assert(n != 0);
		Float inverse = (Float)1 / n;
		return Quaternion(w * inverse, x * inverse, y * inverse, z * inverse);
	}

	inline Quaternion Normalize() const { return *this / std::sqrt(w * w + x * x + y * y + z * z); }

	//public data
	Float w, x, y, z;
};

Quaternion CreateQuaternionByVec3(const Vector3f& rotation) {
	auto rotMat = Matrix4x4();
	if (rotation.x != 0)rotMat = rotMat * RotateX(rotation.x);
	if (rotation.y != 0)rotMat = rotMat * RotateY(rotation.y);
	if (rotation.z != 0)rotMat = rotMat * RotateZ(rotation.z);
	return Quaternion(rotMat);
}

inline Quaternion operator * (Float n, const Quaternion &q) { 
	return q * n; 
}

inline std::ostream& operator << (std::ostream &o, const Quaternion &q) {
	o << '[' << q.w << ',' << q.x << ',' << q.y << ',' << q.z << ']';
	return o;
}

inline Float Dot(const Quaternion& q1, const Quaternion& q2) {
	return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
}

Quaternion Slerp(Float n, const Quaternion& q1, const Quaternion& q2) {
	Float cosTheta = Dot(q1, q2);
	//If the dot product is negative, slerp won't take the shorter path.
	if (cosTheta > .9995f)
		return ((1 - n) * q1 + n * q2).Normalize();
	else {
		Float theta = std::acos(Clamp(cosTheta, -1, 1));
		Float thetaN = theta * n;
		Quaternion q3 = (q2 - q1 * cosTheta).Normalize();
		return q1 * std::cos(thetaN) + q3 * std::sin(thetaN);
	}
}
}