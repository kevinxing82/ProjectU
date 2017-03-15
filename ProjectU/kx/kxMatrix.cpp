#include "kxMatrix.h"
#include <cmath>
#include <algorithm>

USING_KX

const float DEG2RAD = 3.141593f / 180.0f;
const float RAD2DEG = 180.0f / 3.141593f;
const float EPSILON = 0.00001f;

kxMatrix22 & kxMatrix22::transpose()
{
	std::swap(m[1], m[2]);
	return *this;
}

kxMatrix22 & kxMatrix22::invert()
{
	float determinant = getDeterminant();
	if (fabs(determinant) <= EPSILON)
	{
		return identity();
	}

	float tmp = m[0];   // copy the first element
	float invDeterminant = 1.0f / determinant;
	m[0] = invDeterminant * m[3];
	m[1] = -invDeterminant * m[1];
	m[2] = -invDeterminant * m[2];
	m[3] = invDeterminant * tmp;

	return *this;
}

float kxMatrix22::getDeterminant() const
{
	return m[0] * m[3] - m[1] * m[2];
}

///////////////////////////////////////////////////////////////////////////////
// retrieve rotation angle in degree from rotation matrix, R
// R = | c -s |
//     | s  c |
// angle = atan(s / c)
///////////////////////////////////////////////////////////////////////////////
float kxMatrix22::getAngle() const
{
	// angle between -pi ~ +pi (-180 ~ +180)
	return RAD2DEG * atan2f(m[2], m[0]);
}

kxMatrix22 kxMatrix22::operator*(const kxMatrix22 & rhs) const
{
	const float x1((m[0] + m[3]) * (rhs[0] +rhs[3]));
	const float x2((m[2] + m[3]) * rhs[0]);
	const float x3(m[0] * (rhs[1] - rhs[3]));
	const float x4(m[3] * (rhs[2] - rhs[0]));
	const float x5((m[0] + m[1]) * rhs[3]);
	const float x6((m[2] - m[0]) * (rhs[0] + rhs[1]));
	const float x7((m[1] - m[3]) * (rhs[2] + rhs[3]));

	return kxMatrix22(x1 + x4 - x5 + x7, x3 + x5,
		x2 + x4, x1 - x2 + x3 + x6);
}

float kxMatrix33::getDeterminant() const
{
	return m[0] * m[4] * m[8] + m[1] * m[5] * m[6] + m[2] * m[3] * m[7] - m[2] * m[4] * m[6] - m[1] * m[3] * m[8] - m[0] * m[5] * m[7];
}

///////////////////////////////////////////////////////////////////////////////
// retrieve angles in degree from rotation matrix, M = Rx*Ry*Rz
// Rx: rotation about X-axis, pitch
// Ry: rotation about Y-axis, yaw(heading)
// Rz: rotation about Z-axis, roll
//    Rx           Ry          Rz
// |1  0   0| | Cy  0 Sy| |Cz -Sz 0|   | CyCz        -CySz         Sy  |
// |0 Cx -Sx|*|  0  1  0|*|Sz  Cz 0| = | SxSyCz+CxSz -SxSySz+CxCz -SxCy|
// |0 Sx  Cx| |-Sy  0 Cy| | 0   0 1|   |-CxSyCz+SxSz  CxSySz+SxCz  CxCy|
//
// Pitch: atan(-m[7] / m[8]) = atan(SxCy/CxCy)
// Yaw  : asin(m[6]) = asin(Sy)
// Roll : atan(-m[3] / m[0]) = atan(SzCy/CzCy)
///////////////////////////////////////////////////////////////////////////////
kxVector3 kxMatrix33::getAngle() const
{
	float pitch, yaw, roll;         // 3 angles

									// find yaw (around y-axis) first
									// NOTE: asin() returns -90~+90, so correct the angle range -180~+180
									// using z value of forward vector
	yaw = RAD2DEG * asinf(m[2]);
	if (m[8] < 0)
	{
		if (yaw >= 0) yaw = 180.0f - yaw;
		else         yaw = -180.0f - yaw;
	}

	// find roll (around z-axis) and pitch (around x-axis)
	// if forward vector is (1,0,0) or (-1,0,0), then m[0]=m[4]=m[9]=m[10]=0
	if (m[0] > -EPSILON && m[0] < EPSILON)
	{
		roll = 0;  //@@ assume roll=0
		pitch = RAD2DEG * atan2f(m[3], m[4]);
	}
	else
	{
		roll = RAD2DEG * atan2f(-m[1], m[0]);
		pitch = RAD2DEG * atan2f(-m[5], m[8]);
	}

	return kxVector3(pitch, yaw, roll);
}

kxMatrix33 & kxMatrix33::transpose()
{
	std::swap(m[1], m[3]);
	std::swap(m[2], m[6]);
	std::swap(m[5], m[7]);

	return *this;
}

kxMatrix33 & kxMatrix33::invert()
{
	float determinant, invDeterminant;
	float tmp[9];

	tmp[0] = m[4] * m[8] - m[5] * m[7];
	tmp[1] = m[2] * m[7] - m[1] * m[8];
	tmp[2] = m[1] * m[5] - m[2] * m[4];
	tmp[3] = m[5] * m[6] - m[3] * m[8];
	tmp[4] = m[0] * m[8] - m[2] * m[6];
	tmp[5] = m[2] * m[3] - m[0] * m[5];
	tmp[6] = m[3] * m[7] - m[4] * m[6];
	tmp[7] = m[1] * m[6] - m[0] * m[7];
	tmp[8] = m[0] * m[4] - m[1] * m[3];

	// check determinant if it is 0
	determinant = m[0] * tmp[0] + m[3] * tmp[3] + m[6] * tmp[6];
	if (fabs(determinant) <= EPSILON)
	{
		return identity(); // cannot inverse, make it idenety matrix
	}

	// divide by the determinant
	invDeterminant = 1.0f / determinant;
	m[0] = invDeterminant * tmp[0];
	m[1] = invDeterminant * tmp[1];
	m[2] = invDeterminant * tmp[2];
	m[3] = invDeterminant * tmp[3];
	m[4] = invDeterminant * tmp[4];
	m[5] = invDeterminant * tmp[5];
	m[6] = invDeterminant * tmp[6];
	m[7] = invDeterminant * tmp[7];
	m[8] = invDeterminant * tmp[8];

	return *this;
}

kxMatrix44 & kxMatrix44::transpose()
{
	std::swap(m[1], m[4]);
	std::swap(m[2], m[8]);
	std::swap(m[3], m[12]);
	std::swap(m[6], m[9]);
	std::swap(m[7], m[13]);
	std::swap(m[11], m[14]);

	return *this;
}

float kxMatrix44::getDeterminant() const
{
	return m[0] * (m[4] * m[8] - m[5] * m[7]) -
		m[1] * (m[3] * m[8] - m[5] * m[6]) +
		m[2] * (m[3] * m[7] - m[4] * m[6]);
}

kxMatrix44 & kxMatrix44::invert()
{
	// If the 4th row is [0,0,0,1] then it is affine matrix and
	// it has no projective transformation.
	if (m[12] == 0 && m[13] == 0 && m[14] == 0 && m[15] == 1)
		this->invertAffine();
	else
	{
		this->invertGeneral();
		/*@@ invertProjective() is not optimized (slower than generic one)
		if(fabs(m[0]*m[5] - m[1]*m[4]) > EPSILON)
		this->invertProjective();   // inverse using matrix partition
		else
		this->invertGeneral();      // generalized inverse
		*/
	}

	return *this;
}

kxMatrix44 kxMatrix44::operator*(const kxMatrix44 & rhs) const
{
	kxMatrix22 ma11 = kxMatrix22(m[0], m[1], m[4], m[5]);
	kxMatrix22 ma12 = kxMatrix22(m[2], m[3], m[6], m[7]);
	kxMatrix22 ma21 = kxMatrix22(m[8], m[9], m[12], m[13]);
	kxMatrix22 ma22 = kxMatrix22(m[10], m[11], m[14], m[15]);

	kxMatrix22 mb11 = kxMatrix22(rhs[0], rhs[1], rhs[4], rhs[5]);
	kxMatrix22 mb12 = kxMatrix22(rhs[2], rhs[3], rhs[6], rhs[7]);
	kxMatrix22 mb21 = kxMatrix22(rhs[8], rhs[9], rhs[12], rhs[13]);
	kxMatrix22 mb22 = kxMatrix22(rhs[10], rhs[11], rhs[14], rhs[15]);

	kxMatrix22 tmp1 = (ma11 + ma22) * (mb11 + mb22);
	kxMatrix22 tmp2 = (ma21 + ma22) * mb11;
	kxMatrix22 tmp3 = ma11 * (mb12 - mb22);
	kxMatrix22 tmp4 = ma22 * (mb21 - mb11);
	kxMatrix22 tmp5 = (ma11 + ma12) * mb22;
	kxMatrix22 tmp6 = (ma21 - ma11) * (mb11 + mb12);
	kxMatrix22 tmp7 = (ma12 - ma22) * (mb21 + mb22);

	kxMatrix22 r11 = tmp1 + tmp4 - tmp5 + tmp7;
	kxMatrix22 r12 = tmp3 + tmp5;
	kxMatrix22 r21 = tmp2 + tmp4;
	kxMatrix22 r22 = tmp1 + tmp3 - tmp2 + tmp6;
	return kxMatrix44(r11[0],r11[1],r12[0],r12[1],
		r11[2],r11[3],r12[2],r12[3],
		r21[0],r21[1],r22[0],r22[1],
		r21[2],r21[3],r22[2],r22[3]);
}
