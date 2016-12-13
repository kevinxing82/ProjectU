///////////////////////////////////////////////////////////////////////////////
// Vectors.h
// =========
// 2D/3D/4D vectors
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2007-02-14
// UPDATED: 2016-04-04
//
// Copyright (C) 2007-2013 Song Ho Ahn
///////////////////////////////////////////////////////////////////////////////


#ifndef VECTORS_H_DEF
#define VECTORS_H_DEF

#include <cmath>
#include <iostream>

namespace KevinX
{

	///////////////////////////////////////////////////////////////////////////////
	// 2D vector
	///////////////////////////////////////////////////////////////////////////////
	class kxVector2
	{
	public:
		float x;
		float y;

		// ctors
		kxVector2() : x(0), y(0) {};
		kxVector2(float x, float y) : x(x), y(y) {};

		// utils functions
		void        set(float x, float y);
		float       length() const;                         //
		float       distance(const kxVector2& vec) const;     // distance between two vectors
		kxVector2&    normalize();                            //
		float       dot(const kxVector2& vec) const;          // dot product
		bool        equal(const kxVector2& vec, float e) const; // compare with epsilon

															  // operators
		kxVector2     operator-() const;                      // unary operator (negate)
		kxVector2     operator+(const kxVector2& rhs) const;    // add rhs
		kxVector2     operator-(const kxVector2& rhs) const;    // subtract rhs
		kxVector2&    operator+=(const kxVector2& rhs);         // add rhs and update this object
		kxVector2&    operator-=(const kxVector2& rhs);         // subtract rhs and update this object
		kxVector2     operator*(const float scale) const;     // scale
		kxVector2     operator*(const kxVector2& rhs) const;    // multiply each element
		kxVector2&    operator*=(const float scale);          // scale and update this object
		kxVector2&    operator*=(const kxVector2& rhs);         // multiply each element and update this object
		kxVector2     operator/(const float scale) const;     // inverse scale
		kxVector2&    operator/=(const float scale);          // scale and update this object
		bool        operator==(const kxVector2& rhs) const;   // exact compare, no epsilon
		bool        operator!=(const kxVector2& rhs) const;   // exact compare, no epsilon
		bool        operator<(const kxVector2& rhs) const;    // comparison for sort
		float       operator[](int index) const;            // subscript operator v[0], v[1]
		float&      operator[](int index);                  // subscript operator v[0], v[1]

		friend kxVector2 operator*(const float a, const kxVector2 vec);
		friend std::ostream& operator<<(std::ostream& os, const kxVector2& vec);
	};



	///////////////////////////////////////////////////////////////////////////////
	// 3D vector
	///////////////////////////////////////////////////////////////////////////////
	class kxVector3
	{
	public:
		float x;
		float y;
		float z;

		// ctors
		kxVector3() : x(0), y(0), z(0) {};
		kxVector3(float x, float y, float z) : x(x), y(y), z(z) {};

		// utils functions
		void        set(float x, float y, float z);
		float       length() const;                         //
		float       distance(const kxVector3& vec) const;     // distance between two vectors
		float       angle(const kxVector3& vec) const;        // angle between two vectors
		kxVector3&    normalize();                            //
		float       dot(const kxVector3& vec) const;          // dot product
		kxVector3     cross(const kxVector3& vec) const;        // cross product
		bool        equal(const kxVector3& vec, float e) const; // compare with epsilon

															  // operators
		kxVector3     operator-() const;                      // unary operator (negate)
		kxVector3     operator+(const kxVector3& rhs) const;    // add rhs
		kxVector3     operator-(const kxVector3& rhs) const;    // subtract rhs
		kxVector3&    operator+=(const kxVector3& rhs);         // add rhs and update this object
		kxVector3&    operator-=(const kxVector3& rhs);         // subtract rhs and update this object
		kxVector3     operator*(const float scale) const;     // scale
		kxVector3     operator*(const kxVector3& rhs) const;    // multiplay each element
		kxVector3&    operator*=(const float scale);          // scale and update this object
		kxVector3&    operator*=(const kxVector3& rhs);         // product each element and update this object
		kxVector3     operator/(const float scale) const;     // inverse scale
		kxVector3&    operator/=(const float scale);          // scale and update this object
		bool        operator==(const kxVector3& rhs) const;   // exact compare, no epsilon
		bool        operator!=(const kxVector3& rhs) const;   // exact compare, no epsilon
		bool        operator<(const kxVector3& rhs) const;    // comparison for sort
		float       operator[](int index) const;            // subscript operator v[0], v[1]
		float&      operator[](int index);                  // subscript operator v[0], v[1]

		friend kxVector3 operator*(const float a, const kxVector3 vec);
		friend std::ostream& operator<<(std::ostream& os, const kxVector3& vec);
	};



	///////////////////////////////////////////////////////////////////////////////
	// 4D vector
	///////////////////////////////////////////////////////////////////////////////
	class kxVector4
	{
	public:
		float x;
		float y;
		float z;
		float w;

		// ctors
		kxVector4() : x(0), y(0), z(0), w(0) {};
		kxVector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};

		// utils functions
		void        set(float x, float y, float z, float w);
		float       length() const;                         //
		float       distance(const kxVector4& vec) const;     // distance between two vectors
		kxVector4&    normalize();                            //
		float       dot(const kxVector4& vec) const;          // dot product
		bool        equal(const kxVector4& vec, float e) const; // compare with epsilon

															  // operators
		kxVector4     operator-() const;                      // unary operator (negate)
		kxVector4     operator+(const kxVector4& rhs) const;    // add rhs
		kxVector4     operator-(const kxVector4& rhs) const;    // subtract rhs
		kxVector4&    operator+=(const kxVector4& rhs);         // add rhs and update this object
		kxVector4&    operator-=(const kxVector4& rhs);         // subtract rhs and update this object
		kxVector4     operator*(const float scale) const;     // scale
		kxVector4     operator*(const kxVector4& rhs) const;    // multiply each element
		kxVector4&    operator*=(const float scale);          // scale and update this object
		kxVector4&    operator*=(const kxVector4& rhs);         // multiply each element and update this object
		kxVector4     operator/(const float scale) const;     // inverse scale
		kxVector4&    operator/=(const float scale);          // scale and update this object
		bool        operator==(const kxVector4& rhs) const;   // exact compare, no epsilon
		bool        operator!=(const kxVector4& rhs) const;   // exact compare, no epsilon
		bool        operator<(const kxVector4& rhs) const;    // comparison for sort
		float       operator[](int index) const;            // subscript operator v[0], v[1]
		float&      operator[](int index);                  // subscript operator v[0], v[1]

		friend kxVector4 operator*(const float a, const kxVector4 vec);
		friend std::ostream& operator<<(std::ostream& os, const kxVector4& vec);
	};



	// fast math routines from Doom3 SDK
	inline float invSqrt(float x)
	{
		float xhalf = 0.5f * x;
		int i = *(int*)&x;          // get bits for floating value
		i = 0x5f3759df - (i >> 1);    // gives initial guess
		x = *(float*)&i;            // convert bits back to float
		x = x * (1.5f - xhalf*x*x); // Newton step
		return x;
	}



	///////////////////////////////////////////////////////////////////////////////
	// inline functions for Vector2
	///////////////////////////////////////////////////////////////////////////////
	inline kxVector2 kxVector2::operator-() const {
		return kxVector2(-x, -y);
	}

	inline kxVector2 kxVector2::operator+(const kxVector2& rhs) const {
		return kxVector2(x + rhs.x, y + rhs.y);
	}

	inline kxVector2 kxVector2::operator-(const kxVector2& rhs) const {
		return kxVector2(x - rhs.x, y - rhs.y);
	}

	inline kxVector2& kxVector2::operator+=(const kxVector2& rhs) {
		x += rhs.x; y += rhs.y; return *this;
	}

	inline kxVector2& kxVector2::operator-=(const kxVector2& rhs) {
		x -= rhs.x; y -= rhs.y; return *this;
	}

	inline kxVector2 kxVector2::operator*(const float a) const {
		return kxVector2(x*a, y*a);
	}

	inline kxVector2 kxVector2::operator*(const kxVector2& rhs) const {
		return kxVector2(x*rhs.x, y*rhs.y);
	}

	inline kxVector2& kxVector2::operator*=(const float a) {
		x *= a; y *= a; return *this;
	}

	inline kxVector2& kxVector2::operator*=(const kxVector2& rhs) {
		x *= rhs.x; y *= rhs.y; return *this;
	}

	inline kxVector2 kxVector2::operator/(const float a) const {
		return kxVector2(x / a, y / a);
	}

	inline kxVector2& kxVector2::operator/=(const float a) {
		x /= a; y /= a; return *this;
	}

	inline bool kxVector2::operator==(const kxVector2& rhs) const {
		return (x == rhs.x) && (y == rhs.y);
	}

	inline bool kxVector2::operator!=(const kxVector2& rhs) const {
		return (x != rhs.x) || (y != rhs.y);
	}

	inline bool kxVector2::operator<(const kxVector2& rhs) const {
		if (x < rhs.x) return true;
		if (x > rhs.x) return false;
		if (y < rhs.y) return true;
		if (y > rhs.y) return false;
		return false;
	}

	inline float kxVector2::operator[](int index) const {
		return (&x)[index];
	}

	inline float& kxVector2::operator[](int index) {
		return (&x)[index];
	}

	inline void kxVector2::set(float x, float y) {
		this->x = x; this->y = y;
	}

	inline float kxVector2::length() const {
		return sqrtf(x*x + y*y);
	}

	inline float kxVector2::distance(const kxVector2& vec) const {
		return sqrtf((vec.x - x)*(vec.x - x) + (vec.y - y)*(vec.y - y));
	}

	inline kxVector2& kxVector2::normalize() {
		//@@const float EPSILON = 0.000001f;
		float xxyy = x*x + y*y;
		//@@if(xxyy < EPSILON)
		//@@    return *this;

		//float invLength = invSqrt(xxyy);
		float invLength = 1.0f / sqrtf(xxyy);
		x *= invLength;
		y *= invLength;
		return *this;
	}

	inline float kxVector2::dot(const kxVector2& rhs) const {
		return (x*rhs.x + y*rhs.y);
	}

	inline bool kxVector2::equal(const kxVector2& rhs, float epsilon) const {
		return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon;
	}

	inline kxVector2 operator*(const float a, const kxVector2 vec) {
		return kxVector2(a*vec.x, a*vec.y);
	}

	inline std::ostream& operator<<(std::ostream& os, const kxVector2& vec) {
		os << "(" << vec.x << ", " << vec.y << ")";
		return os;
	}
	// END OF VECTOR2 /////////////////////////////////////////////////////////////




	///////////////////////////////////////////////////////////////////////////////
	// inline functions for Vector3
	///////////////////////////////////////////////////////////////////////////////
	inline kxVector3 kxVector3::operator-() const {
		return kxVector3(-x, -y, -z);
	}

	inline kxVector3 kxVector3::operator+(const kxVector3& rhs) const {
		return kxVector3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	inline kxVector3 kxVector3::operator-(const kxVector3& rhs) const {
		return kxVector3(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	inline kxVector3& kxVector3::operator+=(const kxVector3& rhs) {
		x += rhs.x; y += rhs.y; z += rhs.z; return *this;
	}

	inline kxVector3& kxVector3::operator-=(const kxVector3& rhs) {
		x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this;
	}

	inline kxVector3 kxVector3::operator*(const float a) const {
		return kxVector3(x*a, y*a, z*a);
	}

	inline kxVector3 kxVector3::operator*(const kxVector3& rhs) const {
		return kxVector3(x*rhs.x, y*rhs.y, z*rhs.z);
	}

	inline kxVector3& kxVector3::operator*=(const float a) {
		x *= a; y *= a; z *= a; return *this;
	}

	inline kxVector3& kxVector3::operator*=(const kxVector3& rhs) {
		x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this;
	}

	inline kxVector3 kxVector3::operator/(const float a) const {
		return kxVector3(x / a, y / a, z / a);
	}

	inline kxVector3& kxVector3::operator/=(const float a) {
		x /= a; y /= a; z /= a; return *this;
	}

	inline bool kxVector3::operator==(const kxVector3& rhs) const {
		return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
	}

	inline bool kxVector3::operator!=(const kxVector3& rhs) const {
		return (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
	}

	inline bool kxVector3::operator<(const kxVector3& rhs) const {
		if (x < rhs.x) return true;
		if (x > rhs.x) return false;
		if (y < rhs.y) return true;
		if (y > rhs.y) return false;
		if (z < rhs.z) return true;
		if (z > rhs.z) return false;
		return false;
	}

	inline float kxVector3::operator[](int index) const {
		return (&x)[index];
	}

	inline float& kxVector3::operator[](int index) {
		return (&x)[index];
	}

	inline void kxVector3::set(float x, float y, float z) {
		this->x = x; this->y = y; this->z = z;
	}

	inline float kxVector3::length() const {
		return sqrtf(x*x + y*y + z*z);
	}

	inline float kxVector3::distance(const kxVector3& vec) const {
		return sqrtf((vec.x - x)*(vec.x - x) + (vec.y - y)*(vec.y - y) + (vec.z - z)*(vec.z - z));
	}

	inline float kxVector3::angle(const kxVector3& vec) const {
		// return angle between [0, 180]
		float l1 = this->length();
		float l2 = vec.length();
		float d = this->dot(vec);
		float angle = acosf(d / (l1 * l2)) / 3.141592f * 180.0f;
		return angle;
	}

	inline kxVector3& kxVector3::normalize() {
		//@@const float EPSILON = 0.000001f;
		float xxyyzz = x*x + y*y + z*z;
		//@@if(xxyyzz < EPSILON)
		//@@    return *this; // do nothing if it is ~zero vector

		//float invLength = invSqrt(xxyyzz);
		float invLength = 1.0f / sqrtf(xxyyzz);
		x *= invLength;
		y *= invLength;
		z *= invLength;
		return *this;
	}

	inline float kxVector3::dot(const kxVector3& rhs) const {
		return (x*rhs.x + y*rhs.y + z*rhs.z);
	}

	inline kxVector3 kxVector3::cross(const kxVector3& rhs) const {
		return kxVector3(y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x);
	}

	inline bool kxVector3::equal(const kxVector3& rhs, float epsilon) const {
		return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon && fabs(z - rhs.z) < epsilon;
	}

	inline kxVector3 operator*(const float a, const kxVector3 vec) {
		return kxVector3(a*vec.x, a*vec.y, a*vec.z);
	}

	inline std::ostream& operator<<(std::ostream& os, const kxVector3& vec) {
		os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
		return os;
	}
	// END OF VECTOR3 /////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////
	// inline functions for Vector4
	///////////////////////////////////////////////////////////////////////////////
	inline kxVector4 kxVector4::operator-() const {
		return kxVector4(-x, -y, -z, -w);
	}

	inline kxVector4 kxVector4::operator+(const kxVector4& rhs) const {
		return kxVector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	}

	inline kxVector4 kxVector4::operator-(const kxVector4& rhs) const {
		return kxVector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	}

	inline kxVector4& kxVector4::operator+=(const kxVector4& rhs) {
		x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this;
	}

	inline kxVector4& kxVector4::operator-=(const kxVector4& rhs) {
		x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this;
	}

	inline kxVector4 kxVector4::operator*(const float a) const {
		return kxVector4(x*a, y*a, z*a, w*a);
	}

	inline kxVector4 kxVector4::operator*(const kxVector4& rhs) const {
		return kxVector4(x*rhs.x, y*rhs.y, z*rhs.z, w*rhs.w);
	}

	inline kxVector4& kxVector4::operator*=(const float a) {
		x *= a; y *= a; z *= a; w *= a; return *this;
	}

	inline kxVector4& kxVector4::operator*=(const kxVector4& rhs) {
		x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this;
	}

	inline kxVector4 kxVector4::operator/(const float a) const {
		return kxVector4(x / a, y / a, z / a, w / a);
	}

	inline kxVector4& kxVector4::operator/=(const float a) {
		x /= a; y /= a; z /= a; w /= a; return *this;
	}

	inline bool kxVector4::operator==(const kxVector4& rhs) const {
		return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
	}

	inline bool kxVector4::operator!=(const kxVector4& rhs) const {
		return (x != rhs.x) || (y != rhs.y) || (z != rhs.z) || (w != rhs.w);
	}

	inline bool kxVector4::operator<(const kxVector4& rhs) const {
		if (x < rhs.x) return true;
		if (x > rhs.x) return false;
		if (y < rhs.y) return true;
		if (y > rhs.y) return false;
		if (z < rhs.z) return true;
		if (z > rhs.z) return false;
		if (w < rhs.w) return true;
		if (w > rhs.w) return false;
		return false;
	}

	inline float kxVector4::operator[](int index) const {
		return (&x)[index];
	}

	inline float& kxVector4::operator[](int index) {
		return (&x)[index];
	}

	inline void kxVector4::set(float x, float y, float z, float w) {
		this->x = x; this->y = y; this->z = z; this->w = w;
	}

	inline float kxVector4::length() const {
		return sqrtf(x*x + y*y + z*z + w*w);
	}

	inline float kxVector4::distance(const kxVector4& vec) const {
		return sqrtf((vec.x - x)*(vec.x - x) + (vec.y - y)*(vec.y - y) + (vec.z - z)*(vec.z - z) + (vec.w - w)*(vec.w - w));
	}

	inline kxVector4& kxVector4::normalize() {
		//NOTE: leave w-component untouched
		//@@const float EPSILON = 0.000001f;
		float xxyyzz = x*x + y*y + z*z;
		//@@if(xxyyzz < EPSILON)
		//@@    return *this; // do nothing if it is zero vector

		//float invLength = invSqrt(xxyyzz);
		float invLength = 1.0f / sqrtf(xxyyzz);
		x *= invLength;
		y *= invLength;
		z *= invLength;
		return *this;
	}

	inline float kxVector4::dot(const kxVector4& rhs) const {
		return (x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w);
	}

	inline bool kxVector4::equal(const kxVector4& rhs, float epsilon) const {
		return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon &&
			fabs(z - rhs.z) < epsilon && fabs(w - rhs.w) < epsilon;
	}

	inline kxVector4 operator*(const float a, const kxVector4 vec) {
		return kxVector4(a*vec.x, a*vec.y, a*vec.z, a*vec.w);
	}

	inline std::ostream& operator<<(std::ostream& os, const kxVector4& vec) {
		os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
		return os;
	}
	// END OF VECTOR4 /////////////////////////////////////////////////////////////
}
#endif
