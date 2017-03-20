///////////////////////////////////////////////////////////////////////////////
// kxMatrice.h
// =========
// NxN Matrix Math classes
//
// The elements of the matrix are stored as row major order.
// | 0 1 |    | 0 1 2 |    |   0   1  2    3 |
// | 2 3 |    | 3 4 5 |    |   4   5  6    7 |
//             | 6 7 8 |    |   8   9 10  11 |
//                            |  12 13 14 15 |
//
// Dependencies: kxVector2, kxVector3, kxVector3 kxVector4
//
//  AUTHOR: kevin xing
// CREATED: 2017-03-12
// UPDATED: 2017-03-12
//
// Copyright (C) 2017 kevin.xing
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <iomanip>
#include "kxGlobal.h"
#include "kxVector.h"

KX_BEGIN
///////////////////////////////////////////////////////////////////////////
// 2x2 matrix
///////////////////////////////////////////////////////////////////////////
class kxMatrix22
{
public:
	kxMatrix22();
	kxMatrix22(const float src[4]);
	kxMatrix22(float m01, float m02, float m11, float m12);

	void set(const float src[4]);
	void set(float m01, float m02, float m11, float m12);
	void setRow(int index, const float row[2]);
	void setRow(int index, const kxVector2&v);
	void setColumn(int index, const float col[2]);
	void setColumn(int index, const kxVector2& v);

	const float* get() const;
	float getDeterminant() const;
	float getAngle() const;

	kxMatrix22& identity();
	kxMatrix22& transpose();
	kxMatrix22& invert();

	kxMatrix22    operator+  (const kxMatrix22& rhs)const; //add rhs
	kxMatrix22    operator-   (const kxMatrix22& rhs)const; //subtract rhs
	kxMatrix22& operator+=(const kxMatrix22& rhs);         
	kxMatrix22& operator -=(const kxMatrix22& rhs);
	//根据矩阵乘法的要求 第一个矩阵的列数等于第二个矩阵的行数 kxVector2可以被看做一个两行一列的特殊矩阵，按照矩阵乘法计算
	kxVector2      operator*   (const kxVector2&   rhs)const; 
	kxMatrix22    operator*   (const kxMatrix22& rhs)const;
	kxMatrix22& operator*= (const kxMatrix22& rhs);
	bool              operator==(const kxMatrix22& rhs)const;
	bool              operator !=(const kxMatrix22& rhs)const;
	float              operator[]  (int index)const;
	float&           operator[]  (int index);

	friend kxMatrix22 operator-(const kxMatrix22& m);
	friend kxMatrix22 operator*(float scalar, kxMatrix22& m);
	friend kxVector2  operator*(const kxVector2& vec, const kxMatrix22& m);
	friend std::ostream& operator<<(std::ostream& os, const kxMatrix22& m);

private:
	float m[4];
};
///////////////////////////////////////////////////////////////////////////
// 3x3 matrix
///////////////////////////////////////////////////////////////////////////
class kxMatrix33
{
public:
	kxMatrix33();
	kxMatrix33(const float src[9]);
	kxMatrix33(float m11, float m12, float m13,
		float m21, float m22, float m23,
		float m31, float m32, float m33);

	void set(const float src[9]);
	void set(float m11, float m12, float m13,
		float m21, float m22, float m23,
		float m31, float m32, float m33);
	void setRow(int index, const float row[3]);
	void setRow(int index, const kxVector3&v);
	void setColumn(int index, const float col[3]);
	void setColumn(int index, const kxVector3& v);

	const float* get() const;
	float getDeterminant() const;
	kxVector3 getAngle() const;

	kxMatrix33& identity();
	kxMatrix33& transpose();
	kxMatrix33& invert();

	kxMatrix33    operator+   (const kxMatrix33& rhs) const;
	kxMatrix33    operator-    (const kxMatrix33& rhs) const;
	kxMatrix33& operator+= (const kxMatrix33& rhs);
	kxMatrix33& operator -= (const kxMatrix33& rhs);
	kxVector3      operator*    (const kxVector3& rhs) const;
	kxMatrix33    operator*     (const kxMatrix33& rhs) const;
	kxMatrix33& operator*=   (const kxMatrix33& rhs);
	bool              operator==  (const kxMatrix33& rhs) const;
	bool              operator!=   (const kxMatrix33& rhs) const;
	float              operator[]    (int index)const;
	float&           operator[]     (int index);

	friend kxMatrix33 operator- (const kxMatrix33& m);
	friend kxMatrix33 operator* (float scalar, const kxMatrix33& m);
	friend kxVector3 operator*(const kxVector3& vec, const kxMatrix33& m);
	friend std::ostream& operator<<(std::ostream& os, const kxMatrix33& m);
private:
	float m[9];
};

///////////////////////////////////////////////////////////////////////////
// 4x4 matrix
///////////////////////////////////////////////////////////////////////////
class kxMatrix44
{
public:
	kxMatrix44();
	kxMatrix44(const float src[16]);
	kxMatrix44(float m11, float m12, float m13, float m14,
		float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34,
		float m41, float m42, float m43, float m44);
	kxMatrix44(const kxMatrix44& m);

	void set(const float src[16]);
	void set(float m11, float m12, float m13, float m14,
		float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34,
		float m41, float m42, float m43, float m44);
	void setRow(int index, const float row[4]);
	void setRow(int index, const kxVector4& v);
	void setRow(int index, const kxVector3& v);
	void setColumn(int index, const float col[4]);
	void setColumn(int index, const kxVector4& v);
	void setColumn(int index, const kxVector3&  v);

	const float* get() const;
	const float* getTranspose();
	float getDeterminant()const;
	kxMatrix33 getRotationMatrix() const;
	kxVector3 getAngle() const;

	kxMatrix44& identity();
	kxMatrix44& transpose();
	kxMatrix44& invert();
	//need test
	kxMatrix44& invertEuclidean();
	kxMatrix44& invertAffine();
	kxMatrix44& invertProjective();
	kxMatrix44& invertGeneral();

	kxMatrix44& translate(float x, float y, float z);
	kxMatrix44& translate(const kxVector3& v);
	kxMatrix44& rotate(float angle, const kxVector3& axis);
	kxMatrix44& rotate(float angle, float x, float y, float z);
	kxMatrix44& rotateX(float angle);
	kxMatrix44& rotateY(float angle);
	kxMatrix44& rotateZ(float angle);
	kxMatrix44& scale(float scale);
	kxMatrix44& scale(float sx, float sy, float sz);
	kxMatrix44& lookAt(float tx, float ty, float tz);
	kxMatrix44& lookAt(float tx, float ty, float tz, float ux, float uy, float uz);
	kxMatrix44& lookAt(const kxVector3& target);
	kxMatrix44& lookAt(const kxVector3& target, const kxVector3& up);

	kxMatrix44    operator+  (const kxMatrix44& rhs) const;
	kxMatrix44    operator-   (const kxMatrix44& rhs) const;
	kxMatrix44& operator+=(const kxMatrix44& rhs);
	kxMatrix44& operator-= (const kxMatrix44& rhs);
	kxVector4      operator*   (const kxVector4& rhs) const;
	kxVector3      operator*   (const kxVector3& rhs) const;
	kxMatrix44    operator*    (const kxMatrix44& rhs) const;
	kxMatrix44    operator*=(const kxMatrix44& rhs);
	bool              operator== (const kxMatrix44& rhs) const;
	bool              operator!=  (const kxMatrix44& rhs) const;
	float             operator[]    (int index) const;
	float&          operator[]    (int index);

	friend kxMatrix44 operator-(const kxMatrix44& m);
	friend kxMatrix44 operator*(float scalar, const kxMatrix44& m);
	friend kxVector3   operator*(const kxVector3& vec, const kxMatrix44& m);
	friend kxVector4   operator*(const kxVector4& vec, const kxMatrix44& m);
	friend std::ostream& operator<<(std::ostream& os, const kxMatrix44& m);
private:
	float getCofactor(float m11, float m12, float m13,
		float m21, float m22, float m23,
		float m31, float m32, float m33);
	float m[16];
	float tm[16];
};

///////////////////////////////////////////////////////////////////////////
// inline functions for kxMatrix2
///////////////////////////////////////////////////////////////////////////
inline kxMatrix22::kxMatrix22()
{
	identity();
}

inline kxMatrix22::kxMatrix22(const float src[4])
{
	set(src);
}
inline kxMatrix22::kxMatrix22(float m01, float m02, float m11, float m12)
{
	set(m01, m02, m11, m12);
}

inline void kxMatrix22::set(const float src[4])
{
	m[0] = src[0]; m[1] = src[1]; m[2] = src[2]; m[3] = src[3];
}
inline void kxMatrix22::set(float m01, float m02, float m11, float m12)
{
	m[0] = m01; m[1] = m02; m[2] = m11; m[3] = m12;
}
inline void kxMatrix22::setRow(int index, const float row[2])
{
	m[index*2] = row[0]; m[index*2 + 1] = row[1];
}
inline void kxMatrix22::setRow(int index, const kxVector2 & v)
{
	m[index*2] = v.x; m[index*2 + 1] = v.y;
}
inline void kxMatrix22::setColumn(int index, const float col[2])
{
	m[index] = col[0]; m[index + 2] = col[1];
}
inline void kxMatrix22::setColumn(int index, const kxVector2 & v)
{
	m[index] = v.x; m[index + 2] = v.y;
}
inline const float * kxMatrix22::get() const
{
	return m;
}
inline kxMatrix22 & kxMatrix22::identity()
{
	m[0] = m[3] = 1.0f;
	m[1] = m[2] = 0.0f;
	return *this;
}

inline kxMatrix22 kxMatrix22::operator+(const kxMatrix22 & rhs) const
{
	return kxMatrix22(m[0]+rhs[0],m[1]+rhs[1],m[2]+rhs[2],m[3]+rhs[3]);
}
inline kxMatrix22 kxMatrix22::operator-(const kxMatrix22 & rhs) const
{
	return kxMatrix22(m[0]-rhs[0],m[1]-rhs[1],m[2]-rhs[2],m[3]-rhs[3]);
}
inline kxMatrix22 & kxMatrix22::operator+=(const kxMatrix22 & rhs)
{
	m[0] += rhs[0]; m[1] += rhs[1]; m[2] += rhs[2]; m[3] += rhs[3];
	return *this;
}
inline kxMatrix22 & kxMatrix22::operator-=(const kxMatrix22 & rhs)
{
	m[0] -= rhs[0]; m[1] -= rhs[1]; m[2] -= rhs[2]; m[3] -= rhs[3];
	return *this;
}

inline kxVector2 kxMatrix22::operator*(const kxVector2 & rhs) const
{
	return kxVector2(m[0]*rhs.x+m[1]*rhs.y,m[2]*rhs.x+m[3]*rhs.y);
}

inline kxMatrix22 & kxMatrix22::operator*=(const kxMatrix22 & rhs)
{
	*this = *this*rhs;
	return *this;
}
inline bool kxMatrix22::operator==(const kxMatrix22 & rhs) const
{
	return (m[0] == rhs[0]) && (m[1] == rhs[1]) && (m[2] == rhs[2]) && (m[3] == rhs[3]);
}
inline bool kxMatrix22::operator!=(const kxMatrix22 & rhs) const
{
	return (m[0]!=rhs[0]) || (m[1]!=rhs[0]) || (m[2]!=rhs[2]) || (m[3]!=rhs[3]);
}
inline float kxMatrix22::operator[](int index) const
{
	return m[index];
}
inline float & kxMatrix22::operator[](int index)
{
	return m[index];
}
inline kxMatrix22 operator-(const kxMatrix22& rhs)
{
	return kxMatrix22(-rhs[0], -rhs[1], -rhs[2], -rhs[3]);
}
inline kxMatrix22 operator*(float scalar, kxMatrix22 & m)
{
	return kxMatrix22(scalar*m[0], scalar*m[1], scalar*m[2], scalar*m[3]);
}
inline kxVector2 operator*(const kxVector2 & vec, const kxMatrix22 & m)
{
	return kxVector2(vec.x*m[0]+vec.y*m[2],vec.x*m[1]+vec.y*m[3]);
}
inline std::ostream & operator<<(std::ostream & os, const kxMatrix22 & m)
{
	os << std::fixed << std::setprecision(5);
	os << "[" << std::setw(10) << m[0] << " " << std::setw(10) << m[1] << "]\n"
		<< "[" << std::setw(10) << m[2] << " " << std::setw(10) << m[3] << "]\n";
	os << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
	return os;
}

inline kxMatrix33 operator-(const kxMatrix33 & m)
{
	return kxMatrix33(-m[0], -m[1], -m[2], -m[3], -m[4], -m[5], -m[6], -m[7], -m[8]);
}

inline kxMatrix33 operator*(float scalar, const kxMatrix33 & m)
{
	return kxMatrix33(scalar*m[0], scalar*m[1], scalar*m[2], scalar*m[3], scalar*m[4], scalar*m[5], scalar*m[6], scalar*m[7], scalar*m[8]);
}

inline kxVector3 operator*(const kxVector3 & vec, const kxMatrix33 & m)
{
	return kxVector3(vec.x*m[0]+vec.y*m[3]+vec.z*m[6],vec.x*m[1]+vec.y*m[4]+vec.z*m[7],vec.x*m[2]+vec.y*m[5]+vec.z*m[8]);
}

inline std::ostream & operator<<(std::ostream & os, const kxMatrix33 & m)
{
	os << std::fixed << std::setprecision(5);
	os << "[" << std::setw(10) << m[0] << " " << std::setw(10) << m[1] << " " << std::setw(10) << m[2] << "]\n"
		<< "[" << std::setw(10) << m[3] << " " << std::setw(10) << m[4] << " " << std::setw(10) << m[5] << "]\n"
		<< "[" << std::setw(10) << m[6] << " " << std::setw(10) << m[7] << " " << std::setw(10) << m[8] << "]\n";
	os << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
	return os;
}
inline kxMatrix44 operator-(const kxMatrix44 & m)
{
	return kxMatrix44(-m[0], -m[1], -m[2], -m[3], -m[4], -m[5], -m[6], -m[7], -m[8], -m[9], -m[10], -m[11], -m[12], -m[13], -m[14], -m[15]);
}
inline kxMatrix44 operator*(float scalar, const kxMatrix44 & m)
{
	return kxMatrix44(scalar*m[0], scalar*m[1], scalar*m[2], scalar*m[3], scalar*m[4], scalar*m[5], scalar*m[6], scalar*m[7], scalar*m[8], scalar*m[9], scalar*m[10], scalar*m[11], scalar*m[12], scalar*m[13], scalar*m[14], scalar*m[15]);
}
inline kxVector3 operator*(const kxVector3 & vec, const kxMatrix44 & m)
{
	return kxVector3(vec.x*m[0]+vec.y*m[4]+vec.z*m[8],vec.x*m[1]+vec.y*m[5]+vec.z*m[9],vec.x*m[2]+vec.y*m[6]+vec.z*m[10]);
}
inline kxVector4 operator*(const kxVector4 & vec, const kxMatrix44 & m)
{
	return kxVector4(vec.x*m[0]+vec.y*m[4]+vec.z*m[8]+vec.w*m[12],vec.x*m[1]+vec.y*m[5]+vec.z*m[9]+vec.w*m[13],vec.x*m[2]+vec.y*m[6]+vec.z*m[10]+vec.w*m[14],vec.x*m[3]+vec.y*m[7]+vec.z*m[11]+vec.w*m[15]);
}
inline std::ostream & operator<<(std::ostream & os, const kxMatrix44 & m)
{
	os << std::fixed << std::setprecision(5);
	os << "[" << std::setw(10) << m[0] << " " << std::setw(10) << m[1] << " " << std::setw(10) << m[2] << " " << std::setw(10) << m[3] << "]\n"
		<< "[" << std::setw(10) << m[4] << " " << std::setw(10) << m[5] << " " << std::setw(10) << m[8] << " " << std::setw(10) << m[9] << "]\n"
		<< "[" << std::setw(10) << m[8] << " " << std::setw(10) << m[9] << " " << std::setw(10) << m[10] << " " << std::setw(10) << m[11] << "]\n"
		<< "[" << std::setw(10) << m[12] << " " << std::setw(10) << m[13] << " " << std::setw(10) << m[14] << " " << std::setw(10) << m[15] << "]\n";
	os << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
	return os;
}
inline kxMatrix33::kxMatrix33()
{
	identity();
}
inline kxMatrix33::kxMatrix33(const float src[9])
{
	set(src);
}
inline kxMatrix33::kxMatrix33(float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33)
{
	set(m11, m12, m13, m21, m22, m23, m31, m32, m33);
}
inline void kxMatrix33::set(const float src[9])
{
	m[0] = src[0]; m[1] = src[1]; m[2] = src[2];
	m[3] = src[3]; m[4] = src[4]; m[5] = src[5];
	m[6] = src[6]; m[7] = src[7]; m[8] = src[8];
}
inline void kxMatrix33::set(float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33)
{
	m[0] = m11; m[1] = m12; m[2] = m13;
	m[3] = m21; m[4] = m22; m[5] = m23;
	m[6] = m31; m[7] = m32; m[8] = m33;
}
inline void kxMatrix33::setRow(int index, const float row[3])
{
	m[index * 3] = row[0]; m[index * 3 + 1] = row[1]; m[index * 3 + 2] = row[2];
}
inline void kxMatrix33::setRow(int index, const kxVector3 & v)
{
	m[index * 3] = v.x; m[index * 3 + 1] = v.y; m[index * 3 + 2] = v.z;
}
inline void kxMatrix33::setColumn(int index, const float col[3])
{
	m[index] = col[0]; m[index + 3] = col[1]; m[index + 6] = col[2];
}
inline void kxMatrix33::setColumn(int index, const kxVector3 & v)
{
	m[index] = v.x; m[index + 3] = v.y; m[index + 6] = v.z;
}
inline const float * kxMatrix33::get() const
{
	return m;
}

inline kxMatrix33 & kxMatrix33::identity()
{
	m[0] = m[4] = m[8] = 1;
	m[1] = m[2] = m[3] = m[5] = m[6] = m[7] = 0;
	return *this;
}

inline kxMatrix33 kxMatrix33::operator+(const kxMatrix33 & rhs) const
{
	return kxMatrix33(m[0] + rhs[0], m[1] + rhs[1], m[2] + rhs[2],
		m[3] + rhs[3], m[4] + rhs[4], m[5] + rhs[5],
		m[6] + rhs[6], m[7] + rhs[7], m[8] + rhs[8]);
}
inline kxMatrix33 kxMatrix33::operator-(const kxMatrix33 & rhs) const
{
	return kxMatrix33(m[0] - rhs[0], m[1] - rhs[1], m[2] - rhs[2],
		m[3] - rhs[3], m[4] - rhs[4], m[5] - rhs[5],
		m[6] - rhs[6], m[7] - rhs[7], m[8] - rhs[8]);
}
inline kxMatrix33 & kxMatrix33::operator+=(const kxMatrix33 & rhs)
{
	m[0] += rhs[0];  m[1] += rhs[1];  m[2] += rhs[2];
	m[3] += rhs[3];  m[4] += rhs[4];  m[5] += rhs[5];
	m[6] += rhs[6];  m[7] += rhs[7];  m[8] += rhs[8];
	return *this;
}
inline kxMatrix33 & kxMatrix33::operator-=(const kxMatrix33 & rhs)
{
	m[0] -= rhs[0];  m[1] -= rhs[1];  m[2] -= rhs[2];
	m[3] -= rhs[3];  m[4] -= rhs[4];  m[5] -= rhs[5];
	m[6] -= rhs[6];  m[7] -= rhs[7];  m[8] -= rhs[8];
	return *this;
}
inline kxVector3 kxMatrix33::operator*(const kxVector3 & rhs) const
{
	return kxVector3(m[0] * rhs.x + m[1] * rhs.y + m[2] * rhs.z,
		m[3] * rhs.x + m[4] * rhs.y + m[5] * rhs.z,
		m[6] * rhs.x + m[7] * rhs.y + m[8] * rhs.z);
}

inline kxMatrix33 kxMatrix33::operator*(const kxMatrix33 & rhs) const
{
	return kxMatrix33(m[0]*rhs[0]+m[1]*rhs[3]+m[2]*rhs[6],m[0]*rhs[1]+m[1]*rhs[4]+m[2]*rhs[7],m[0]*rhs[2]+m[1]*rhs[5]+m[2]*rhs[8],
		                        m[3]*rhs[0]+m[4]*rhs[3]+m[5]*rhs[6],m[3]*rhs[1]+m[4]*rhs[4]+m[5]*rhs[7],m[3]*rhs[2]+m[4]*rhs[5]+m[5]*rhs[8],
		                        m[6]*rhs[0]+m[7]*rhs[3]+m[8]*rhs[6],m[6]*rhs[1]+m[7]*rhs[4]+m[8]*rhs[7],m[6]*rhs[2]+m[7]*rhs[5]+m[8]*rhs[8]);
}

inline kxMatrix33 & kxMatrix33::operator*=(const kxMatrix33 & rhs)
{
	*this = *this*rhs;
	return *this;
}

inline bool kxMatrix33::operator==(const kxMatrix33 & rhs) const
{
	return (m[0] == rhs[0]) && (m[1] == rhs[1]) && (m[2] == rhs[2]) &&
		(m[3] == rhs[3]) && (m[4] == rhs[4]) && (m[5] == rhs[5]) &&
		(m[6] == rhs[6]) && (m[7] == rhs[7]) && (m[8] == rhs[8]);
}

inline bool kxMatrix33::operator!=(const kxMatrix33 & rhs) const
{
	return (m[0] != rhs[0]) || (m[1] != rhs[1]) || (m[2] != rhs[2]) ||
		(m[3] != rhs[3]) || (m[4] != rhs[4]) || (m[5] != rhs[5]) ||
		(m[6] != rhs[6]) || (m[7] != rhs[7]) || (m[8] != rhs[8]);
}

inline float kxMatrix33::operator[](int index) const
{
	return m[index];
}

inline float & kxMatrix33::operator[](int index)
{
	return m[index];
}

inline kxMatrix44::kxMatrix44()
{
	identity();
}

inline kxMatrix44::kxMatrix44(const float src[16])
{
	set(src);
}

inline kxMatrix44::kxMatrix44(float m11, float m12, float m13, float m14,
	float m21, float m22, float m23, float m24,
	float m31, float m32, float m33, float m34,
	float m41, float m42, float m43, float m44)
{
	set(m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44);
}

inline kxMatrix44::kxMatrix44(const kxMatrix44& m)
{
	set(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);
}

inline void kxMatrix44::set(const float src[16])
{
	m[0] = src[0];  m[1] = src[1];  m[2] = src[2];  m[3] = src[3];
	m[4] = src[4];  m[5] = src[5];  m[6] = src[6];  m[7] = src[7];
	m[8] = src[8];  m[9] = src[9];  m[10] = src[10]; m[11] = src[11];
	m[12] = src[12]; m[13] = src[13]; m[14] = src[14]; m[15] = src[15];
}
inline void kxMatrix44::set(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
{
	m[0] = m11; m[1] = m12; m[2] = m13; m[3] = m14;
	m[4] = m21; m[5] = m22; m[6] = m23; m[7] = m24;
	m[8] = m31; m[9] = m32; m[10] = m33; m[11] = m34;
	m[12] = m41; m[13] = m42; m[14] = m43; m[15] = m44;
}

inline void kxMatrix44::setRow(int index, const float row[4])
{
	m[index * 4] = row[0]; m[index * 4 + 1] = row[1]; m[index*+2] = row[2]; m[index * 3 + 3] = row[3];
}

inline void kxMatrix44::setRow(int index, const kxVector4 & v)
{
	m[index * 4] = v.x; m[index * 4 + 1] = v.y; m[index * 4 + 2] = v.z; m[index * 4 + 3] = v.w;
}

inline void kxMatrix44::setRow(int index, const kxVector3 & v)
{
	m[index * 4] = v.x; m[index * 4 + 1] = v.y; m[index * 4 + 2] = v.z;
}

inline void kxMatrix44::setColumn(int index, const float col[4])
{
	m[index] = col[0]; m[index + 4] = col[1]; m[index + 8] = col[2]; m[index + 12] = col[3];
}

inline void kxMatrix44::setColumn(int index, const kxVector4 & v)
{
	m[index] = v.x; m[index + 4] = v.y; m[index + 8] = v.z; m[index + 12] = v.w;
}

inline void kxMatrix44::setColumn(int index, const kxVector3 & v)
{
	m[index] = v.x; m[index + 4] = v.y; m[index + 8] = v.z;
}

inline const float * kxMatrix44::get() const
{
	return m;
}

inline const float * kxMatrix44::getTranspose()
{
	tm[0] = m[0];   tm[1] = m[4];   tm[2] = m[8];   tm[3] = m[12];
	tm[4] = m[1];   tm[5] = m[5];   tm[6] = m[9];   tm[7] = m[13];
	tm[8] = m[2];   tm[9] = m[6];   tm[10] = m[10];  tm[11] = m[14];
	tm[12] = m[3];   tm[13] = m[7];   tm[14] = m[11];  tm[15] = m[15];
	return tm;
}

inline kxMatrix44 & kxMatrix44::identity()
{
	m[0] = m[5] = m[10] = m[15] = 1.0f;
	m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0f;
	return *this;
}

inline kxMatrix44  kxMatrix44::operator+(const kxMatrix44& rhs) const
{
	return kxMatrix44(m[0] + rhs[0], m[1] + rhs[1], m[2] + rhs[2], m[3] + rhs[3],
		m[4] + rhs[4], m[5] + rhs[5], m[6] + rhs[6], m[7] + rhs[7],
		m[8] + rhs[8], m[9] + rhs[9], m[10] + rhs[10], m[11] + rhs[11],
		m[12] + rhs[12], m[13] + rhs[13], m[14] + rhs[14], m[15] + rhs[15]);
}

inline kxMatrix44 kxMatrix44::operator-(const kxMatrix44 & rhs) const
{
	return kxMatrix44(m[0] - rhs[0], m[1] - rhs[1], m[2] - rhs[2], m[3] - rhs[3],
		m[4] - rhs[4], m[5] - rhs[5], m[6] - rhs[6], m[7] - rhs[7],
		m[8] - rhs[8], m[9] - rhs[9], m[10] - rhs[10], m[11] - rhs[11],
		m[12] - rhs[12], m[13] - rhs[13], m[14] - rhs[14], m[15] - rhs[15]);
}

inline kxMatrix44 & kxMatrix44::operator+=(const kxMatrix44 & rhs)
{
	m[0] += rhs[0];   m[1] += rhs[1];   m[2] += rhs[2];   m[3] += rhs[3];
	m[4] += rhs[4];   m[5] += rhs[5];   m[6] += rhs[6];   m[7] += rhs[7];
	m[8] += rhs[8];   m[9] += rhs[9];   m[10] += rhs[10];  m[11] += rhs[11];
	m[12] += rhs[12];  m[13] += rhs[13];  m[14] += rhs[14];  m[15] += rhs[15];
	return *this;
}

inline kxMatrix44 & kxMatrix44::operator-=(const kxMatrix44 & rhs)
{
	m[0] -= rhs[0];   m[1] -= rhs[1];   m[2] -= rhs[2];   m[3] -= rhs[3];
	m[4] -= rhs[4];   m[5] -= rhs[5];   m[6] -= rhs[6];   m[7] -= rhs[7];
	m[8] -= rhs[8];   m[9] -= rhs[9];   m[10] -= rhs[10];  m[11] -= rhs[11];
	m[12] -= rhs[12];  m[13] -= rhs[13];  m[14] -= rhs[14];  m[15] -= rhs[15];
	return *this;
}

inline kxVector4 kxMatrix44::operator*(const kxVector4 & rhs) const
{
	return kxVector4(m[0]*rhs[0]+m[1]*rhs[1]+m[2]*rhs[2]+m[3]*rhs[3],m[4]*rhs[0]+m[5]*rhs[1]+m[6]*rhs[2]+m[7]*rhs[3],m[8]*rhs[0]+m[9]*rhs[1]+m[10]*rhs[2]+m[11]*rhs[3],m[12]*rhs[0]+m[13]*rhs[1]+m[14]*rhs[2]+m[15]*rhs[3]);
}

inline kxVector3 kxMatrix44::operator*(const kxVector3 & rhs) const
{
	return kxVector3(m[0] * rhs[0] + m[1] * rhs[1] + m[2] * rhs[2], m[4] * rhs[0] + m[5] * rhs[1] + m[6] * rhs[2],m[8] * rhs[0] + m[9] * rhs[1] + m[10] * rhs[2]);
}

inline kxMatrix44 kxMatrix44::operator*=(const kxMatrix44 & rhs)
{
	*this = *this*rhs;
	return *this;
}

inline bool kxMatrix44::operator==(const kxMatrix44 & rhs) const
{
	return (m[0] == rhs[0]) && (m[1] == rhs[1]) && (m[2] == rhs[2]) && (m[3] == rhs[3]) &&
		(m[4] == rhs[4]) && (m[5] == rhs[5]) && (m[6] == rhs[6]) && (m[7] == rhs[7]) &&
		(m[8] == rhs[8]) && (m[9] == rhs[9]) && (m[10] == rhs[10]) && (m[11] == rhs[11]) &&
		(m[12] == rhs[12]) && (m[13] == rhs[13]) && (m[14] == rhs[14]) && (m[15] == rhs[15]);
}

inline bool kxMatrix44::operator!=(const kxMatrix44 & rhs) const
{
	return (m[0] != rhs[0]) || (m[1] != rhs[1]) || (m[2] != rhs[2]) || (m[3] != rhs[3]) ||
		(m[4] != rhs[4]) || (m[5] != rhs[5]) || (m[6] != rhs[6]) || (m[7] != rhs[7]) ||
		(m[8] != rhs[8]) || (m[9] != rhs[9]) || (m[10] != rhs[10]) || (m[11] != rhs[11]) ||
		(m[12] != rhs[12]) || (m[13] != rhs[13]) || (m[14] != rhs[14]) || (m[15] != rhs[15]);
}

inline float kxMatrix44::operator[](int index) const
{
	return m[index];
}

inline float & kxMatrix44::operator[](int index)
{
	m[index];
}

KX_END

