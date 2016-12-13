///////////////////////////////////////////////////////////////////////////////
// Matrice.h
// =========
// NxN Matrix Math classes
//
// The elements of the matrix are stored as column major order.
// | 0 2 |    | 0 3 6 |    |  0  4  8 12 |
// | 1 3 |    | 1 4 7 |    |  1  5  9 13 |
//            | 2 5 8 |    |  2  6 10 14 |
//                         |  3  7 11 15 |
//
// Dependencies: kxVector2, kxVector3, kxVector3
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2005-06-24
// UPDATED: 2016-07-07
//
// Copyright (C) 2005 Song Ho Ahn
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_MATRICES_H
#define MATH_MATRICES_H

#include <iostream>
#include <iomanip>
#include "kxVector.h"

namespace KevinX
{

	///////////////////////////////////////////////////////////////////////////
	// 2x2 matrix
	///////////////////////////////////////////////////////////////////////////
	class kxMatrix2
	{
	public:
		// constructors
		kxMatrix2();  // init with identity
		kxMatrix2(const float src[4]);
		kxMatrix2(float m0, float m1, float m2, float m3);

		void        set(const float src[4]);
		void        set(float m0, float m1, float m2, float m3);
		void        setRow(int index, const float row[2]);
		void        setRow(int index, const kxVector2& v);
		void        setColumn(int index, const float col[2]);
		void        setColumn(int index, const kxVector2& v);

		const float* get() const;
		float       getDeterminant() const;
		float       getAngle() const;                       // retrieve angle (degree) from matrix

		kxMatrix2&    identity();
		kxMatrix2&    transpose();                            // transpose itself and return reference
		kxMatrix2&    invert();

		// operators
		kxMatrix2     operator+(const kxMatrix2& rhs) const;    // add rhs
		kxMatrix2     operator-(const kxMatrix2& rhs) const;    // subtract rhs
		kxMatrix2&    operator+=(const kxMatrix2& rhs);         // add rhs and update this object
		kxMatrix2&    operator-=(const kxMatrix2& rhs);         // subtract rhs and update this object
		kxVector2     operator*(const kxVector2& rhs) const;    // multiplication: v' = M * v
		kxMatrix2     operator*(const kxMatrix2& rhs) const;    // multiplication: M3 = M1 * M2
		kxMatrix2&    operator*=(const kxMatrix2& rhs);         // multiplication: M1' = M1 * M2
		bool        operator==(const kxMatrix2& rhs) const;   // exact compare, no epsilon
		bool        operator!=(const kxMatrix2& rhs) const;   // exact compare, no epsilon
		float       operator[](int index) const;            // subscript operator v[0], v[1]
		float&      operator[](int index);                  // subscript operator v[0], v[1]

															// friends functions
		friend kxMatrix2 operator-(const kxMatrix2& m);                     // unary operator (-)
		friend kxMatrix2 operator*(float scalar, const kxMatrix2& m);       // pre-multiplication
		friend kxVector2 operator*(const kxVector2& vec, const kxMatrix2& m); // pre-multiplication
		friend std::ostream& operator<<(std::ostream& os, const kxMatrix2& m);

		// static functions

	protected:

	private:
		float m[4];

	};



	///////////////////////////////////////////////////////////////////////////
	// 3x3 matrix
	///////////////////////////////////////////////////////////////////////////
	class kxMatrix3
	{
	public:
		// constructors
		kxMatrix3();  // init with identity
		kxMatrix3(const float src[9]);
		kxMatrix3(float m0, float m1, float m2,           // 1st column
			float m3, float m4, float m5,           // 2nd column
			float m6, float m7, float m8);          // 3rd column

		void        set(const float src[9]);
		void        set(float m0, float m1, float m2,   // 1st column
			float m3, float m4, float m5,   // 2nd column
			float m6, float m7, float m8);  // 3rd column
		void        setRow(int index, const float row[3]);
		void        setRow(int index, const kxVector3& v);
		void        setColumn(int index, const float col[3]);
		void        setColumn(int index, const kxVector3& v);

		const float* get() const;
		float       getDeterminant() const;
		kxVector3     getAngle() const;                       // return (pitch, yaw, roll)

		kxMatrix3&    identity();
		kxMatrix3&    transpose();                            // transpose itself and return reference
		kxMatrix3&    invert();

		// operators
		kxMatrix3     operator+(const kxMatrix3& rhs) const;    // add rhs
		kxMatrix3     operator-(const kxMatrix3& rhs) const;    // subtract rhs
		kxMatrix3&    operator+=(const kxMatrix3& rhs);         // add rhs and update this object
		kxMatrix3&    operator-=(const kxMatrix3& rhs);         // subtract rhs and update this object
		kxVector3     operator*(const kxVector3& rhs) const;    // multiplication: v' = M * v
		kxMatrix3     operator*(const kxMatrix3& rhs) const;    // multiplication: M3 = M1 * M2
		kxMatrix3&    operator*=(const kxMatrix3& rhs);         // multiplication: M1' = M1 * M2
		bool        operator==(const kxMatrix3& rhs) const;   // exact compare, no epsilon
		bool        operator!=(const kxMatrix3& rhs) const;   // exact compare, no epsilon
		float       operator[](int index) const;            // subscript operator v[0], v[1]
		float&      operator[](int index);                  // subscript operator v[0], v[1]

															// friends functions
		friend kxMatrix3 operator-(const kxMatrix3& m);                     // unary operator (-)
		friend kxMatrix3 operator*(float scalar, const kxMatrix3& m);       // pre-multiplication
		friend kxVector3 operator*(const kxVector3& vec, const kxMatrix3& m); // pre-multiplication
		friend std::ostream& operator<<(std::ostream& os, const kxMatrix3& m);

	protected:

	private:
		float m[9];

	};



	///////////////////////////////////////////////////////////////////////////
	// 4x4 matrix
	///////////////////////////////////////////////////////////////////////////
	class kxMatrix4
	{
	public:
		// constructors
		kxMatrix4();  // init with identity
		kxMatrix4(const float src[16]);
		kxMatrix4(float m00, float m01, float m02, float m03, // 1st column
			float m04, float m05, float m06, float m07, // 2nd column
			float m08, float m09, float m10, float m11, // 3rd column
			float m12, float m13, float m14, float m15);// 4th column

		void        set(const float src[16]);
		void        set(float m00, float m01, float m02, float m03, // 1st column
			float m04, float m05, float m06, float m07, // 2nd column
			float m08, float m09, float m10, float m11, // 3rd column
			float m12, float m13, float m14, float m15);// 4th column
		void        setRow(int index, const float row[4]);
		void        setRow(int index, const kxVector4& v);
		void        setRow(int index, const kxVector3& v);
		void        setColumn(int index, const float col[4]);
		void        setColumn(int index, const kxVector4& v);
		void        setColumn(int index, const kxVector3& v);

		const float* get() const;
		const float* getTranspose();                        // return transposed matrix
		float       getDeterminant() const;
		kxMatrix3     getRotationMatrix() const;              // return 3x3 rotation part
		kxVector3     getAngle() const;                       // return (pitch, yaw, roll)

		kxMatrix4&    identity();
		kxMatrix4&    transpose();                            // transpose itself and return reference
		kxMatrix4&    invert();                               // check best inverse method before inverse
		kxMatrix4&    invertEuclidean();                      // inverse of Euclidean transform matrix
		kxMatrix4&    invertAffine();                         // inverse of affine transform matrix
		kxMatrix4&    invertProjective();                     // inverse of projective matrix using partitioning
		kxMatrix4&    invertGeneral();                        // inverse of generic matrix

															// transform matrix
		kxMatrix4&    translate(float x, float y, float z);   // translation by (x,y,z)
		kxMatrix4&    translate(const kxVector3& v);            //
		kxMatrix4&    rotate(float angle, const kxVector3& axis); // rotate angle(degree) along the given axix
		kxMatrix4&    rotate(float angle, float x, float y, float z);
		kxMatrix4&    rotateX(float angle);                   // rotate on X-axis with degree
		kxMatrix4&    rotateY(float angle);                   // rotate on Y-axis with degree
		kxMatrix4&    rotateZ(float angle);                   // rotate on Z-axis with degree
		kxMatrix4&    scale(float scale);                     // uniform scale
		kxMatrix4&    scale(float sx, float sy, float sz);    // scale by (sx, sy, sz) on each axis
		kxMatrix4&    lookAt(float tx, float ty, float tz);   // face object to the target direction
		kxMatrix4&    lookAt(float tx, float ty, float tz, float ux, float uy, float uz);
		kxMatrix4&    lookAt(const kxVector3& target);
		kxMatrix4&    lookAt(const kxVector3& target, const kxVector3& up);
		//@@kxMatrix4&    skew(float angle, const kxVector3& axis); //

		// operators
		kxMatrix4     operator+(const kxMatrix4& rhs) const;    // add rhs
		kxMatrix4     operator-(const kxMatrix4& rhs) const;    // subtract rhs
		kxMatrix4&    operator+=(const kxMatrix4& rhs);         // add rhs and update this object
		kxMatrix4&    operator-=(const kxMatrix4& rhs);         // subtract rhs and update this object
		kxVector4     operator*(const kxVector4& rhs) const;    // multiplication: v' = M * v
		kxVector3     operator*(const kxVector3& rhs) const;    // multiplication: v' = M * v
		kxMatrix4     operator*(const kxMatrix4& rhs) const;    // multiplication: M3 = M1 * M2
		kxMatrix4&    operator*=(const kxMatrix4& rhs);         // multiplication: M1' = M1 * M2
		bool        operator==(const kxMatrix4& rhs) const;   // exact compare, no epsilon
		bool        operator!=(const kxMatrix4& rhs) const;   // exact compare, no epsilon
		float       operator[](int index) const;            // subscript operator v[0], v[1]
		float&      operator[](int index);                  // subscript operator v[0], v[1]

															// friends functions
		friend kxMatrix4 operator-(const kxMatrix4& m);                     // unary operator (-)
		friend kxMatrix4 operator*(float scalar, const kxMatrix4& m);       // pre-multiplication
		friend kxVector3 operator*(const kxVector3& vec, const kxMatrix4& m); // pre-multiplication
		friend kxVector4 operator*(const kxVector4& vec, const kxMatrix4& m); // pre-multiplication
		friend std::ostream& operator<<(std::ostream& os, const kxMatrix4& m);

	protected:

	private:
		float       getCofactor(float m0, float m1, float m2,
			float m3, float m4, float m5,
			float m6, float m7, float m8) const;

		float m[16];
		float tm[16];                                       // transpose m

	};



	///////////////////////////////////////////////////////////////////////////
	// inline functions for kxMatrix2
	///////////////////////////////////////////////////////////////////////////
	inline kxMatrix2::kxMatrix2()
	{
		// initially identity matrix
		identity();
	}



	inline kxMatrix2::kxMatrix2(const float src[4])
	{
		set(src);
	}



	inline kxMatrix2::kxMatrix2(float m0, float m1, float m2, float m3)
	{
		set(m0, m1, m2, m3);
	}



	inline void kxMatrix2::set(const float src[4])
	{
		m[0] = src[0];  m[1] = src[1];  m[2] = src[2];  m[3] = src[3];
	}



	inline void kxMatrix2::set(float m0, float m1, float m2, float m3)
	{
		m[0] = m0;  m[1] = m1;  m[2] = m2;  m[3] = m3;
	}



	inline void kxMatrix2::setRow(int index, const float row[2])
	{
		m[index] = row[0];  m[index + 2] = row[1];
	}



	inline void kxMatrix2::setRow(int index, const kxVector2& v)
	{
		m[index] = v.x;  m[index + 2] = v.y;
	}



	inline void kxMatrix2::setColumn(int index, const float col[2])
	{
		m[index * 2] = col[0];  m[index * 2 + 1] = col[1];
	}



	inline void kxMatrix2::setColumn(int index, const kxVector2& v)
	{
		m[index * 2] = v.x;  m[index * 2 + 1] = v.y;
	}



	inline const float* kxMatrix2::get() const
	{
		return m;
	}



	inline kxMatrix2& kxMatrix2::identity()
	{
		m[0] = m[3] = 1.0f;
		m[1] = m[2] = 0.0f;
		return *this;
	}



	inline kxMatrix2 kxMatrix2::operator+(const kxMatrix2& rhs) const
	{
		return kxMatrix2(m[0] + rhs[0], m[1] + rhs[1], m[2] + rhs[2], m[3] + rhs[3]);
	}



	inline kxMatrix2 kxMatrix2::operator-(const kxMatrix2& rhs) const
	{
		return kxMatrix2(m[0] - rhs[0], m[1] - rhs[1], m[2] - rhs[2], m[3] - rhs[3]);
	}



	inline kxMatrix2& kxMatrix2::operator+=(const kxMatrix2& rhs)
	{
		m[0] += rhs[0];  m[1] += rhs[1];  m[2] += rhs[2];  m[3] += rhs[3];
		return *this;
	}



	inline kxMatrix2& kxMatrix2::operator-=(const kxMatrix2& rhs)
	{
		m[0] -= rhs[0];  m[1] -= rhs[1];  m[2] -= rhs[2];  m[3] -= rhs[3];
		return *this;
	}



	inline kxVector2 kxMatrix2::operator*(const kxVector2& rhs) const
	{
		return kxVector2(m[0] * rhs.x + m[2] * rhs.y, m[1] * rhs.x + m[3] * rhs.y);
	}



	inline kxMatrix2 kxMatrix2::operator*(const kxMatrix2& rhs) const
	{
		return kxMatrix2(m[0] * rhs[0] + m[2] * rhs[1], m[1] * rhs[0] + m[3] * rhs[1],
			m[0] * rhs[2] + m[2] * rhs[3], m[1] * rhs[2] + m[3] * rhs[3]);
	}



	inline kxMatrix2& kxMatrix2::operator*=(const kxMatrix2& rhs)
	{
		*this = *this * rhs;
		return *this;
	}



	inline bool kxMatrix2::operator==(const kxMatrix2& rhs) const
	{
		return (m[0] == rhs[0]) && (m[1] == rhs[1]) && (m[2] == rhs[2]) && (m[3] == rhs[3]);
	}



	inline bool kxMatrix2::operator!=(const kxMatrix2& rhs) const
	{
		return (m[0] != rhs[0]) || (m[1] != rhs[1]) || (m[2] != rhs[2]) || (m[3] != rhs[3]);
	}



	inline float kxMatrix2::operator[](int index) const
	{
		return m[index];
	}



	inline float& kxMatrix2::operator[](int index)
	{
		return m[index];
	}



	inline kxMatrix2 operator-(const kxMatrix2& rhs)
	{
		return kxMatrix2(-rhs[0], -rhs[1], -rhs[2], -rhs[3]);
	}



	inline kxMatrix2 operator*(float s, const kxMatrix2& rhs)
	{
		return kxMatrix2(s*rhs[0], s*rhs[1], s*rhs[2], s*rhs[3]);
	}



	inline kxVector2 operator*(const kxVector2& v, const kxMatrix2& rhs)
	{
		return kxVector2(v.x*rhs[0] + v.y*rhs[1], v.x*rhs[2] + v.y*rhs[3]);
	}



	inline std::ostream& operator<<(std::ostream& os, const kxMatrix2& m)
	{
		os << std::fixed << std::setprecision(5);
		os << "[" << std::setw(10) << m[0] << " " << std::setw(10) << m[2] << "]\n"
			<< "[" << std::setw(10) << m[1] << " " << std::setw(10) << m[3] << "]\n";
		os << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
		return os;
	}
	// END OF kxMatrix2 INLINE //////////////////////////////////////////////////////




	///////////////////////////////////////////////////////////////////////////
	// inline functions for kxMatrix3
	///////////////////////////////////////////////////////////////////////////
	inline kxMatrix3::kxMatrix3()
	{
		// initially identity matrix
		identity();
	}



	inline kxMatrix3::kxMatrix3(const float src[9])
	{
		set(src);
	}



	inline kxMatrix3::kxMatrix3(float m0, float m1, float m2,
		float m3, float m4, float m5,
		float m6, float m7, float m8)
	{
		set(m0, m1, m2, m3, m4, m5, m6, m7, m8);
	}



	inline void kxMatrix3::set(const float src[9])
	{
		m[0] = src[0];  m[1] = src[1];  m[2] = src[2];
		m[3] = src[3];  m[4] = src[4];  m[5] = src[5];
		m[6] = src[6];  m[7] = src[7];  m[8] = src[8];
	}



	inline void kxMatrix3::set(float m0, float m1, float m2,
		float m3, float m4, float m5,
		float m6, float m7, float m8)
	{
		m[0] = m0;  m[1] = m1;  m[2] = m2;
		m[3] = m3;  m[4] = m4;  m[5] = m5;
		m[6] = m6;  m[7] = m7;  m[8] = m8;
	}



	inline void kxMatrix3::setRow(int index, const float row[3])
	{
		m[index] = row[0];  m[index + 3] = row[1];  m[index + 6] = row[2];
	}



	inline void kxMatrix3::setRow(int index, const kxVector3& v)
	{
		m[index] = v.x;  m[index + 3] = v.y;  m[index + 6] = v.z;
	}



	inline void kxMatrix3::setColumn(int index, const float col[3])
	{
		m[index * 3] = col[0];  m[index * 3 + 1] = col[1];  m[index * 3 + 2] = col[2];
	}



	inline void kxMatrix3::setColumn(int index, const kxVector3& v)
	{
		m[index * 3] = v.x;  m[index * 3 + 1] = v.y;  m[index * 3 + 2] = v.z;
	}



	inline const float* kxMatrix3::get() const
	{
		return m;
	}



	inline kxMatrix3& kxMatrix3::identity()
	{
		m[0] = m[4] = m[8] = 1.0f;
		m[1] = m[2] = m[3] = m[5] = m[6] = m[7] = 0.0f;
		return *this;
	}



	inline kxMatrix3 kxMatrix3::operator+(const kxMatrix3& rhs) const
	{
		return kxMatrix3(m[0] + rhs[0], m[1] + rhs[1], m[2] + rhs[2],
			m[3] + rhs[3], m[4] + rhs[4], m[5] + rhs[5],
			m[6] + rhs[6], m[7] + rhs[7], m[8] + rhs[8]);
	}



	inline kxMatrix3 kxMatrix3::operator-(const kxMatrix3& rhs) const
	{
		return kxMatrix3(m[0] - rhs[0], m[1] - rhs[1], m[2] - rhs[2],
			m[3] - rhs[3], m[4] - rhs[4], m[5] - rhs[5],
			m[6] - rhs[6], m[7] - rhs[7], m[8] - rhs[8]);
	}



	inline kxMatrix3& kxMatrix3::operator+=(const kxMatrix3& rhs)
	{
		m[0] += rhs[0];  m[1] += rhs[1];  m[2] += rhs[2];
		m[3] += rhs[3];  m[4] += rhs[4];  m[5] += rhs[5];
		m[6] += rhs[6];  m[7] += rhs[7];  m[8] += rhs[8];
		return *this;
	}



	inline kxMatrix3& kxMatrix3::operator-=(const kxMatrix3& rhs)
	{
		m[0] -= rhs[0];  m[1] -= rhs[1];  m[2] -= rhs[2];
		m[3] -= rhs[3];  m[4] -= rhs[4];  m[5] -= rhs[5];
		m[6] -= rhs[6];  m[7] -= rhs[7];  m[8] -= rhs[8];
		return *this;
	}



	inline kxVector3 kxMatrix3::operator*(const kxVector3& rhs) const
	{
		return kxVector3(m[0] * rhs.x + m[3] * rhs.y + m[6] * rhs.z,
			m[1] * rhs.x + m[4] * rhs.y + m[7] * rhs.z,
			m[2] * rhs.x + m[5] * rhs.y + m[8] * rhs.z);
	}



	inline kxMatrix3 kxMatrix3::operator*(const kxMatrix3& rhs) const
	{
		return kxMatrix3(m[0] * rhs[0] + m[3] * rhs[1] + m[6] * rhs[2], m[1] * rhs[0] + m[4] * rhs[1] + m[7] * rhs[2], m[2] * rhs[0] + m[5] * rhs[1] + m[8] * rhs[2],
			m[0] * rhs[3] + m[3] * rhs[4] + m[6] * rhs[5], m[1] * rhs[3] + m[4] * rhs[4] + m[7] * rhs[5], m[2] * rhs[3] + m[5] * rhs[4] + m[8] * rhs[5],
			m[0] * rhs[6] + m[3] * rhs[7] + m[6] * rhs[8], m[1] * rhs[6] + m[4] * rhs[7] + m[7] * rhs[8], m[2] * rhs[6] + m[5] * rhs[7] + m[8] * rhs[8]);
	}



	inline kxMatrix3& kxMatrix3::operator*=(const kxMatrix3& rhs)
	{
		*this = *this * rhs;
		return *this;
	}



	inline bool kxMatrix3::operator==(const kxMatrix3& rhs) const
	{
		return (m[0] == rhs[0]) && (m[1] == rhs[1]) && (m[2] == rhs[2]) &&
			(m[3] == rhs[3]) && (m[4] == rhs[4]) && (m[5] == rhs[5]) &&
			(m[6] == rhs[6]) && (m[7] == rhs[7]) && (m[8] == rhs[8]);
	}



	inline bool kxMatrix3::operator!=(const kxMatrix3& rhs) const
	{
		return (m[0] != rhs[0]) || (m[1] != rhs[1]) || (m[2] != rhs[2]) ||
			(m[3] != rhs[3]) || (m[4] != rhs[4]) || (m[5] != rhs[5]) ||
			(m[6] != rhs[6]) || (m[7] != rhs[7]) || (m[8] != rhs[8]);
	}



	inline float kxMatrix3::operator[](int index) const
	{
		return m[index];
	}



	inline float& kxMatrix3::operator[](int index)
	{
		return m[index];
	}



	inline kxMatrix3 operator-(const kxMatrix3& rhs)
	{
		return kxMatrix3(-rhs[0], -rhs[1], -rhs[2], -rhs[3], -rhs[4], -rhs[5], -rhs[6], -rhs[7], -rhs[8]);
	}



	inline kxMatrix3 operator*(float s, const kxMatrix3& rhs)
	{
		return kxMatrix3(s*rhs[0], s*rhs[1], s*rhs[2], s*rhs[3], s*rhs[4], s*rhs[5], s*rhs[6], s*rhs[7], s*rhs[8]);
	}



	inline kxVector3 operator*(const kxVector3& v, const kxMatrix3& m)
	{
		return kxVector3(v.x*m[0] + v.y*m[1] + v.z*m[2], v.x*m[3] + v.y*m[4] + v.z*m[5], v.x*m[6] + v.y*m[7] + v.z*m[8]);
	}



	inline std::ostream& operator<<(std::ostream& os, const kxMatrix3& m)
	{
		os << std::fixed << std::setprecision(5);
		os << "[" << std::setw(10) << m[0] << " " << std::setw(10) << m[3] << " " << std::setw(10) << m[6] << "]\n"
			<< "[" << std::setw(10) << m[1] << " " << std::setw(10) << m[4] << " " << std::setw(10) << m[7] << "]\n"
			<< "[" << std::setw(10) << m[2] << " " << std::setw(10) << m[5] << " " << std::setw(10) << m[8] << "]\n";
		os << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
		return os;
	}
	// END OF kxMatrix3 INLINE //////////////////////////////////////////////////////




	///////////////////////////////////////////////////////////////////////////
	// inline functions for kxMatrix4
	///////////////////////////////////////////////////////////////////////////
	inline kxMatrix4::kxMatrix4()
	{
		// initially identity matrix
		identity();
	}



	inline kxMatrix4::kxMatrix4(const float src[16])
	{
		set(src);
	}



	inline kxMatrix4::kxMatrix4(float m00, float m01, float m02, float m03,
		float m04, float m05, float m06, float m07,
		float m08, float m09, float m10, float m11,
		float m12, float m13, float m14, float m15)
	{
		set(m00, m01, m02, m03, m04, m05, m06, m07, m08, m09, m10, m11, m12, m13, m14, m15);
	}



	inline void kxMatrix4::set(const float src[16])
	{
		m[0] = src[0];  m[1] = src[1];  m[2] = src[2];  m[3] = src[3];
		m[4] = src[4];  m[5] = src[5];  m[6] = src[6];  m[7] = src[7];
		m[8] = src[8];  m[9] = src[9];  m[10] = src[10]; m[11] = src[11];
		m[12] = src[12]; m[13] = src[13]; m[14] = src[14]; m[15] = src[15];
	}



	inline void kxMatrix4::set(float m00, float m01, float m02, float m03,
		float m04, float m05, float m06, float m07,
		float m08, float m09, float m10, float m11,
		float m12, float m13, float m14, float m15)
	{
		m[0] = m00;  m[1] = m01;  m[2] = m02;  m[3] = m03;
		m[4] = m04;  m[5] = m05;  m[6] = m06;  m[7] = m07;
		m[8] = m08;  m[9] = m09;  m[10] = m10;  m[11] = m11;
		m[12] = m12;  m[13] = m13;  m[14] = m14;  m[15] = m15;
	}



	inline void kxMatrix4::setRow(int index, const float row[4])
	{
		m[index] = row[0];  m[index + 4] = row[1];  m[index + 8] = row[2];  m[index + 12] = row[3];
	}



	inline void kxMatrix4::setRow(int index, const kxVector4& v)
	{
		m[index] = v.x;  m[index + 4] = v.y;  m[index + 8] = v.z;  m[index + 12] = v.w;
	}



	inline void kxMatrix4::setRow(int index, const kxVector3& v)
	{
		m[index] = v.x;  m[index + 4] = v.y;  m[index + 8] = v.z;
	}



	inline void kxMatrix4::setColumn(int index, const float col[4])
	{
		m[index * 4] = col[0];  m[index * 4 + 1] = col[1];  m[index * 4 + 2] = col[2];  m[index * 4 + 3] = col[3];
	}



	inline void kxMatrix4::setColumn(int index, const kxVector4& v)
	{
		m[index * 4] = v.x;  m[index * 4 + 1] = v.y;  m[index * 4 + 2] = v.z;  m[index * 4 + 3] = v.w;
	}



	inline void kxMatrix4::setColumn(int index, const kxVector3& v)
	{
		m[index * 4] = v.x;  m[index * 4 + 1] = v.y;  m[index * 4 + 2] = v.z;
	}



	inline const float* kxMatrix4::get() const
	{
		return m;
	}



	inline const float* kxMatrix4::getTranspose()
	{
		tm[0] = m[0];   tm[1] = m[4];   tm[2] = m[8];   tm[3] = m[12];
		tm[4] = m[1];   tm[5] = m[5];   tm[6] = m[9];   tm[7] = m[13];
		tm[8] = m[2];   tm[9] = m[6];   tm[10] = m[10];  tm[11] = m[14];
		tm[12] = m[3];   tm[13] = m[7];   tm[14] = m[11];  tm[15] = m[15];
		return tm;
	}



	inline kxMatrix4& kxMatrix4::identity()
	{
		m[0] = m[5] = m[10] = m[15] = 1.0f;
		m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0f;
		return *this;
	}



	inline kxMatrix4 kxMatrix4::operator+(const kxMatrix4& rhs) const
	{
		return kxMatrix4(m[0] + rhs[0], m[1] + rhs[1], m[2] + rhs[2], m[3] + rhs[3],
			m[4] + rhs[4], m[5] + rhs[5], m[6] + rhs[6], m[7] + rhs[7],
			m[8] + rhs[8], m[9] + rhs[9], m[10] + rhs[10], m[11] + rhs[11],
			m[12] + rhs[12], m[13] + rhs[13], m[14] + rhs[14], m[15] + rhs[15]);
	}



	inline kxMatrix4 kxMatrix4::operator-(const kxMatrix4& rhs) const
	{
		return kxMatrix4(m[0] - rhs[0], m[1] - rhs[1], m[2] - rhs[2], m[3] - rhs[3],
			m[4] - rhs[4], m[5] - rhs[5], m[6] - rhs[6], m[7] - rhs[7],
			m[8] - rhs[8], m[9] - rhs[9], m[10] - rhs[10], m[11] - rhs[11],
			m[12] - rhs[12], m[13] - rhs[13], m[14] - rhs[14], m[15] - rhs[15]);
	}



	inline kxMatrix4& kxMatrix4::operator+=(const kxMatrix4& rhs)
	{
		m[0] += rhs[0];   m[1] += rhs[1];   m[2] += rhs[2];   m[3] += rhs[3];
		m[4] += rhs[4];   m[5] += rhs[5];   m[6] += rhs[6];   m[7] += rhs[7];
		m[8] += rhs[8];   m[9] += rhs[9];   m[10] += rhs[10];  m[11] += rhs[11];
		m[12] += rhs[12];  m[13] += rhs[13];  m[14] += rhs[14];  m[15] += rhs[15];
		return *this;
	}



	inline kxMatrix4& kxMatrix4::operator-=(const kxMatrix4& rhs)
	{
		m[0] -= rhs[0];   m[1] -= rhs[1];   m[2] -= rhs[2];   m[3] -= rhs[3];
		m[4] -= rhs[4];   m[5] -= rhs[5];   m[6] -= rhs[6];   m[7] -= rhs[7];
		m[8] -= rhs[8];   m[9] -= rhs[9];   m[10] -= rhs[10];  m[11] -= rhs[11];
		m[12] -= rhs[12];  m[13] -= rhs[13];  m[14] -= rhs[14];  m[15] -= rhs[15];
		return *this;
	}



	inline kxVector4 kxMatrix4::operator*(const kxVector4& rhs) const
	{
		return kxVector4(m[0] * rhs.x + m[4] * rhs.y + m[8] * rhs.z + m[12] * rhs.w,
			m[1] * rhs.x + m[5] * rhs.y + m[9] * rhs.z + m[13] * rhs.w,
			m[2] * rhs.x + m[6] * rhs.y + m[10] * rhs.z + m[14] * rhs.w,
			m[3] * rhs.x + m[7] * rhs.y + m[11] * rhs.z + m[15] * rhs.w);
	}



	inline kxVector3 kxMatrix4::operator*(const kxVector3& rhs) const
	{
		return kxVector3(m[0] * rhs.x + m[4] * rhs.y + m[8] * rhs.z + m[12],
			m[1] * rhs.x + m[5] * rhs.y + m[9] * rhs.z + m[13],
			m[2] * rhs.x + m[6] * rhs.y + m[10] * rhs.z + m[14]);
	}



	inline kxMatrix4 kxMatrix4::operator*(const kxMatrix4& n) const
	{
		return kxMatrix4(m[0] * n[0] + m[4] * n[1] + m[8] * n[2] + m[12] * n[3], m[1] * n[0] + m[5] * n[1] + m[9] * n[2] + m[13] * n[3], m[2] * n[0] + m[6] * n[1] + m[10] * n[2] + m[14] * n[3], m[3] * n[0] + m[7] * n[1] + m[11] * n[2] + m[15] * n[3],
			m[0] * n[4] + m[4] * n[5] + m[8] * n[6] + m[12] * n[7], m[1] * n[4] + m[5] * n[5] + m[9] * n[6] + m[13] * n[7], m[2] * n[4] + m[6] * n[5] + m[10] * n[6] + m[14] * n[7], m[3] * n[4] + m[7] * n[5] + m[11] * n[6] + m[15] * n[7],
			m[0] * n[8] + m[4] * n[9] + m[8] * n[10] + m[12] * n[11], m[1] * n[8] + m[5] * n[9] + m[9] * n[10] + m[13] * n[11], m[2] * n[8] + m[6] * n[9] + m[10] * n[10] + m[14] * n[11], m[3] * n[8] + m[7] * n[9] + m[11] * n[10] + m[15] * n[11],
			m[0] * n[12] + m[4] * n[13] + m[8] * n[14] + m[12] * n[15], m[1] * n[12] + m[5] * n[13] + m[9] * n[14] + m[13] * n[15], m[2] * n[12] + m[6] * n[13] + m[10] * n[14] + m[14] * n[15], m[3] * n[12] + m[7] * n[13] + m[11] * n[14] + m[15] * n[15]);
	}



	inline kxMatrix4& kxMatrix4::operator*=(const kxMatrix4& rhs)
	{
		*this = *this * rhs;
		return *this;
	}



	inline bool kxMatrix4::operator==(const kxMatrix4& n) const
	{
		return (m[0] == n[0]) && (m[1] == n[1]) && (m[2] == n[2]) && (m[3] == n[3]) &&
			(m[4] == n[4]) && (m[5] == n[5]) && (m[6] == n[6]) && (m[7] == n[7]) &&
			(m[8] == n[8]) && (m[9] == n[9]) && (m[10] == n[10]) && (m[11] == n[11]) &&
			(m[12] == n[12]) && (m[13] == n[13]) && (m[14] == n[14]) && (m[15] == n[15]);
	}



	inline bool kxMatrix4::operator!=(const kxMatrix4& n) const
	{
		return (m[0] != n[0]) || (m[1] != n[1]) || (m[2] != n[2]) || (m[3] != n[3]) ||
			(m[4] != n[4]) || (m[5] != n[5]) || (m[6] != n[6]) || (m[7] != n[7]) ||
			(m[8] != n[8]) || (m[9] != n[9]) || (m[10] != n[10]) || (m[11] != n[11]) ||
			(m[12] != n[12]) || (m[13] != n[13]) || (m[14] != n[14]) || (m[15] != n[15]);
	}



	inline float kxMatrix4::operator[](int index) const
	{
		return m[index];
	}



	inline float& kxMatrix4::operator[](int index)
	{
		return m[index];
	}



	inline kxMatrix4 operator-(const kxMatrix4& rhs)
	{
		return kxMatrix4(-rhs[0], -rhs[1], -rhs[2], -rhs[3], -rhs[4], -rhs[5], -rhs[6], -rhs[7], -rhs[8], -rhs[9], -rhs[10], -rhs[11], -rhs[12], -rhs[13], -rhs[14], -rhs[15]);
	}



	inline kxMatrix4 operator*(float s, const kxMatrix4& rhs)
	{
		return kxMatrix4(s*rhs[0], s*rhs[1], s*rhs[2], s*rhs[3], s*rhs[4], s*rhs[5], s*rhs[6], s*rhs[7], s*rhs[8], s*rhs[9], s*rhs[10], s*rhs[11], s*rhs[12], s*rhs[13], s*rhs[14], s*rhs[15]);
	}



	inline kxVector4 operator*(const kxVector4& v, const kxMatrix4& m)
	{
		return kxVector4(v.x*m[0] + v.y*m[1] + v.z*m[2] + v.w*m[3], v.x*m[4] + v.y*m[5] + v.z*m[6] + v.w*m[7], v.x*m[8] + v.y*m[9] + v.z*m[10] + v.w*m[11], v.x*m[12] + v.y*m[13] + v.z*m[14] + v.w*m[15]);
	}



	inline kxVector3 operator*(const kxVector3& v, const kxMatrix4& m)
	{
		return kxVector3(v.x*m[0] + v.y*m[1] + v.z*m[2], v.x*m[4] + v.y*m[5] + v.z*m[6], v.x*m[8] + v.y*m[9] + v.z*m[10]);
	}



	inline std::ostream& operator<<(std::ostream& os, const kxMatrix4& m)
	{
		os << std::fixed << std::setprecision(5);
		os << "[" << std::setw(10) << m[0] << " " << std::setw(10) << m[4] << " " << std::setw(10) << m[8] << " " << std::setw(10) << m[12] << "]\n"
			<< "[" << std::setw(10) << m[1] << " " << std::setw(10) << m[5] << " " << std::setw(10) << m[9] << " " << std::setw(10) << m[13] << "]\n"
			<< "[" << std::setw(10) << m[2] << " " << std::setw(10) << m[6] << " " << std::setw(10) << m[10] << " " << std::setw(10) << m[14] << "]\n"
			<< "[" << std::setw(10) << m[3] << " " << std::setw(10) << m[7] << " " << std::setw(10) << m[11] << " " << std::setw(10) << m[15] << "]\n";
		os << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
		return os;
	}
	// END OF kxMatrix4 INLINE //////////////////////////////////////////////////////
}
#endif
